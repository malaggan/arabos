// #include <multiboot.h>
#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <memops.h>
#include <fork.h>
#include <kb.h>
#include <asm.h>
#include <string.h>
#include <printf.h>
#include <printk.h>
class Koko { public: Koko(); ~Koko();} kk;
Koko::Koko()
{printf("koko\n");}
Koko::~Koko()
{printf("~koko\n");}

// filled by the linker script
// the linker symbol have only an address (in the symbol table)
extern int __end,__phys;
int kernel_end_addr=reinterpret_cast<int>(&__end),kernel_load_addr=reinterpret_cast<int>(&__phys);

#include <ecma48.h>
#define LBRACK	WHITE "[" NORMAL
#define RBRACK	WHITE "]" NORMAL

#define PREFILL(ch)	memsetw(ttys[active_tty].data+ttys[active_tty].ypos*COLUMNS,ch|(0x0700)/*|(ATTRIBUTE<<8)*/,COLUMNS);

void write_serial(char);

void SHOW_STAT(char const * strWhat, char const * strStat, int len) {
	printf(WHITE "%s" NORMAL, strWhat);
	for(int i = strnlen(strWhat,COLUMNS); i < COLUMNS - len - 2; i++)
		printf(".");

	if(len == 2) // hack because printf only considers ECMA48 if hardcoded in the format string, and not through %s.
	{
		printf(LBRACK GREENB "%s" NORMAL RBRACK, strStat);
		write_serial('\n'); // COM1 needs \n but printf doesn't, since the line auto wraps
	}
	else
		printf(LBRACK REDB "%s" NORMAL RBRACK, strStat); write_serial('\n');
}

void SHOW_STAT_OK(char const * strWhat) {
	SHOW_STAT(strWhat,"OK",2);
}

void SHOW_STAT_FAILED(char const * strWhat) {
	SHOW_STAT(strWhat,/*"FAILED"*/"NOT IMPLEMENTED YET",strnlen("NOT IMPLEMENTED YET",40));
}

extern "C" void _init(); // extern "C" because the function is defined by GCC and we cannot control its name mangling

void cppmain()
{
	cpuid_check();

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

	SHOW_STAT_FAILED("Discovering devices"); // ata.cpp
	SHOW_STAT_FAILED("Filesystem"); // fs
	SHOW_STAT_FAILED("Networking");
	SHOW_STAT_FAILED("Shell");

	void init_process();
	int i = spawn(init_process);
	printk("\nspawn returned: %d\n",i);
	scheduling_started = 1;

	SHOW_STAT_OK("Process Manager");

	// will only get here till one time slice ends and never again
	while(1);
}
