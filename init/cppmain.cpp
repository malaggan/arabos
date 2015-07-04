#include <multiboot.h>
#include <console.h>
#include <lib.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <kb.h>
#include <asm.h>
#include <debug.h>
#include <fork.h>

#include <stl/iterator.hh>

void cppmain();

void enter_cpp()
{

	printk(LOG "In C++ main\n");

	cppmain();

	// this should never return.
	// TODO: panic here.
}

class Koko { public: Koko(); ~Koko();} kk;
Koko::Koko()
{printf("koko\n");}
Koko::~Koko()
{printf("~koko\n");}


extern int __end,__phys; // filled by the linker script
int kernel_end_addr=reinterpret_cast<int>(&__end),kernel_load_addr=reinterpret_cast<int>(&__phys); // the linker symbol have only an address (in the symbol table)

#include <ecma48.h>
#define LBRACK	WHITE "[" NORMAL
#define RBRACK	WHITE "]" NORMAL
#define PREFILL(ch)	memsetw(ttys[active_tty].data+ttys[active_tty].ypos*COLUMNS,ch|(0x0700)/*|(ATTRIBUTE<<8)*/,COLUMNS);

void write_serial(char);

void SHOW_STAT(char const * strWhat, char const * strStat, int len) {
	// PREFILL('.');
	// printf(WHITE strWhat NORMAL);
	// ttys[active_tty].xpos = COLUMNS - len - 2;
	// printf(LBRACK strStat RBRACK);
	printf(WHITE "%s" NORMAL, strWhat);
	for(int i = strnlen(strWhat,COLUMNS); i < COLUMNS - len - 2; i++) printf(".");

	if(len == 2) // hack because printf only considers ECMA48 if hardcoded in the format string, and not through %s.
	{ printf(LBRACK GREENB "%s" NORMAL RBRACK, strStat); write_serial('\n'); } // COM1 needs \n but printf doesn't, since the line auto wraps
	else
	{ printf(LBRACK REDB "%s" NORMAL RBRACK, strStat); write_serial('\n'); }
}
void SHOW_STAT_OK(char const * strWhat) {SHOW_STAT(strWhat,"OK",2);}
void SHOW_STAT_FAILED(char const * strWhat) {SHOW_STAT(strWhat,/*"FAILED"*/"NOT IMPLEMENTED YET",strnlen("NOT IMPLEMENTED YET",40));}

int esp = 0;

#include <semaphore.h>

volatile semaphore_t sem = 1;
volatile semaphore_t children_finished = -1;
volatile int owner = 0; // this is to force owner change, to test semaphors, instead of looping a constant number of times
void monitor();
void identify_drive();
// the init process
void init_process()
{
	printk("in init now :)\n");

	if(1)
	{ // put vector in a block, to test deallocation in dtor
		aos::vector<int> v1{};
		v1.push_back( 1 );
		v1.push_back( 2 );
		v1.push_back( 3 );
		v1.push_back( 4 );
		v1.push_back( 5 );
		v1.push_back( 6 );
		v1.push_back( 7 );
		auto b = v1.begin(); printf("*begin() = %d\n", *b);
		auto d = v1.front(); printf("front() = %d\n", d);
		auto f = v1[3]; printf("[3] = %d\n", f);
		auto h = v1.size(); printf("size = %d\n", h);
		auto u = v1.end();

		printf("erasing begin():     ");
		auto afaf = v1.erase(b);
		if(afaf == v1.end()) printf("no\n");
		if(afaf == v1.begin()+1) printf("yes\n");
		for(auto i : v1)
			printf("%d, ", i);
		printf("\n");

		printf("insert 3 at begin(): ");
		v1.insert(v1.begin(), 3);
		for(auto i : v1)
			printf("%d, ", i);
		printf("\n");

		v1.erase(v1.begin());
		v1.erase(v1.begin());
		v1.erase(v1.begin());

		aos::vector<int> v3{}; v3.push_back(133); v3.push_back(144); v3.push_back(155);
		printf("insert range at begin()+3: ");
		v1.insert(v1.begin() + 3, aos::begin(v3), aos::end(v3));

		for(auto i : v1)
			printf("%d, ", i);
		printf("\n");

		aos::string<300> s1{"hi"};
		printf("s1: %s\n", s1.c_str());
		if(s1 == aos::string<3>{"hi"} && !(s1== aos::string<3>{"hI"})) { printf("string == ok\n"); }

		printf("insert bulk at begin()+3: ");
		v1.insert(v1.begin() + 3, 4, 111); // had to use SFINAE for this one...
		for(auto i : v1)
			printf("%d, ", i);
		printf("\n");
		// __asm__ __volatile__ ("xchg %bx,%bx\n"); // magic breakpoint for bochs internal debugger
		aos::vector<int> const v2{};
		printf("1 \n");auto c = v2.begin();
		printf("2 \n");auto e = v2.front(); // TODO: should be invalid
		//printf("3 \n");auto g = v2[3]; // invalid access sicne v2 is empty
		printf("4 \n");auto v = v2.end();
		// v2.insert(v2.begin(), 3); // cannot insert into a const vector

		// printf("testing vector ended. dtors (and shared_array deallocation) should run now\n");
	}
	// printf("testing vector ended. now going into monitor\n");

	int x = 5*6;
	printk("Forking: \n");
	int proc = fork();
	printk(TRACE "this is process %i\n",proc);
	if(!proc) // FIXME: fork returns zero to parent ??? this is wrong.
	{
		wait(&sem);
		owner = 0;
		while(x--)
		{
			if(x%6==5)
			{
				signal(&sem);
				// test semaphor: at no cost break printing 'A' except every 6 A's printed
				while(owner == 0)
					;
				wait(&sem);
				owner = 0;
			}
			printk(REDB "A" NORMAL);
			for(int i = 0; i < 0xFFF; i++);
		}
		signal(&sem);

		// wait for all threads to finish
		wait(&children_finished);
		printf("\n");
		SHOW_STAT_FAILED("Discovering devices");
		SHOW_STAT_FAILED("Filesystem");
		SHOW_STAT_FAILED("Networking");
		SHOW_STAT_FAILED("Shell");

		monitor();
	}
	else
	{
		proc = fork();

		if(!proc)
		{
			wait(&sem);
			owner = 1;
			while(x--)
			{
				if(x%6==5)
				{
					signal(&sem);
					while(owner == 1)
						;
					wait(&sem);
					owner = 1;
				}
				printk(GREENB "B" NORMAL);
				for(int i = 0; i < 0xFFF; i++);
			}
			signal(&sem);
			signal(&children_finished);
		}
		else
		{
			wait(&sem);
			owner = 2;
			while(x--)
			{
				if(x%6==5)
				{
					signal(&sem);
					while(owner == 2)
						;
					wait(&sem);
					owner = 2;
				}
				printk(BLUEB "C" NORMAL);
				for(int i = 0; i < 0xFFF; i++);
			}
			signal(&sem);
			signal(&children_finished);
		}
	}
}

// this function doesn't return unless when powering off the system
extern "C" void _init(); // extern "C" because the function is defined by GCC and we cannot control its name mangling
void cppmain()
{
#if 1
	cpuid_check();
#endif

	printf(WHITE "Welcome to " REDB "ArOS" BLUEB " v 0.005!\n" NORMAL);

	printk(LOG "Kernel size is %d bytes (%d KB) [end= 0x%x,load= 0x%x]\n",
	       (kernel_end_addr-kernel_load_addr),
	       (kernel_end_addr-kernel_load_addr)>>10,
	       kernel_end_addr,
	       kernel_load_addr);

	SHOW_STAT_OK("Boot");

	gdt_install();
	SHOW_STAT_OK("GDT");

	idt_install(); //SHOW_STAT_OK("IDT");
	irq_install(); //SHOW_STAT_OK("IRQ");
	timer_install();
	printk(LOG "Setting interrupts\n");

	sti();

	SHOW_STAT_OK("IDT");

	init_kb();

	void init_paging();
	init_paging();

	SHOW_STAT_OK("Memory Manager");

	/* setup process management */
	idt_set_gate (16+32,reinterpret_cast<unsigned>(_irq16), 0x08, IRQ_GATE);
	irq_install_custom_handler(16,fork_handler);

	idt_set_gate (17+32,reinterpret_cast<unsigned>(_irq17), 0x08, IRQ_GATE);
	irq_install_custom_handler(17,spawn_handler);

	memset(reinterpret_cast<unsigned char*>(processes),0,MAX_PROCESSES*sizeof(ProcessData));

	_init();

	int i = spawn(init_process);
	printk("\nspawn returned: %d\n",i);
	scheduling_started = 1;

	SHOW_STAT_OK("Process Manager");

	while(1); // will only get here till one time slice ends and never again
}
void readline(char* buf, int max);
char* alloc_page();
void print_monitor_help();
void monitor()
{
	char cmd[100];
	printf("The MONITOR ! - type h for help");

	for(;;)
	{
		printf("\n%% ");
		*cmd=0;
		readline(cmd,99);


		// handle the command
		if(!strncmp(cmd,"alloc_page",10))
			printf("0x%x\n",alloc_page());
		else if(!strncmp(cmd,"h",1))
			print_monitor_help();
		else if(!strncmp(cmd,"pst",3))
			print_stack_trace();
		else if(!strncmp(cmd,"quit",4))
			return;
		else
			printf("unrecognized montior command\n");
	}
}

void print_monitor_help()
{
	printf("This is the ArOS monitor shell.\n"
	       "Commands:\n"
	       "alloc_page - allocates a new page\n"
	       "h - shows this screen\n"
	       "quit - quits the monitor shell\n"
	       "pst - print stack trace (of the monitor itself)");

}
