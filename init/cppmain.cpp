/* Copyright (C) 2007  Mohammad Nabil 
mohammad (dot) nabil (dot) h (at) gmail (dot) com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.*/

#include <multiboot.h>
#include <console.h>
#include <lib.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <kb.h>
#include <mm.h>
#include <asm.h>
#include <debug.h>
#include <fork.h>

#include <stl/iterator.hh>

void cppmain();

extern "C"
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

extern "C" void write_serial(char);

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
volatile int owner = 0; // this is to force owner change, to test semaphors, instead of looping a constant number of times
void monitor();
// the init process
void init_process()
{    
    printk("in init now :)\n");

    if(0)
    { // put vector in a block, to test deallocation in dtor
	aos::vector<int> v1(true);
	v1.push_back( 1 );
	v1.push_back( 2 );
	v1.push_back( 3 );
	v1.push_back( 4 );
	v1.push_back( 5 );
	v1.push_back( 6 );
	v1.push_back( 7 );
	v1.push_back( 8 );
	v1.push_back( 9 );
	v1.push_back(10 );
	v1.push_back(11 );
	v1.push_back(12 );
	v1.push_back(13 );
	v1.push_back(14 );
	v1.push_back(15 );
	v1.push_back(16 );
	v1.push_back(17 );
	v1.push_back(18 );
	v1.push_back(19 );
	v1.push_back(20 );
	v1.push_back(21 );
	auto b = v1.begin(); printf("*begin() = %d\n", *b);
	auto d = v1.front(); printf("front() = %d\n", d);
	auto f = v1[3]; printf("[3] = %d\n", f);
	auto h = v1.size(); printf("size = %d\n", h);
	auto u = v1.end();
	auto afaf = v1.erase(b);
	if(afaf == v1.end()) printf("no\n");
	if(afaf == v1.begin()+1) printf("yes\n");
	printf("erasing begin():     ");
	for(auto i : v1)
	    printf("%d, ", i);
	printf("\n");

	v1.insert(v1.begin(), 3);
	printf("insert 3 at begin(): ");
	for(auto i : v1)
	    printf("%d, ", i);
	printf("\n");

	aos::vector<int> v3(true); v3.push_back(133); v3.push_back(144); v3.push_back(155);
	v1.insert(v1.begin() + 3, aos::begin(v3), aos::end(v3));
	printf("insert range at begin()+3: ");
	for(auto i : v1)
	    printf("%d, ", i);
	printf("\n");

	v1.insert(v1.begin() + 3, 4, 111); // had to use SFINAE for this one...
	printf("insert bulk at begin()+3: ");
	for(auto i : v1)
	    printf("%d, ", i);
	printf("\n");
        // __asm__ __volatile__ ("xchg %bx,%bx\n"); // xxx
	aos::vector<int> const v2(false);
	// auto c = v2.begin();
	// auto e = v2.front();
	// auto g = v2[3];
	// auto v = v2.end();
	// // v2.insert(v2.begin(), 3); // cannot insert into a const vector

	printf("testing vector ended. dtors (and shared_array deallocation) should run now\n");
    }
    //printf("testing vector ended. now going into monitor\n");
    //monitor();

    int x = 5*6;
    printk("Forking: \n");
    int proc = fork();    
    printk(TRACE "this is process %i\n",proc);
    if(!proc)
    {
        wait(&sem);
    	owner = 0;
        while(x--)
        {
    	    if(x%6==0)
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
    		if(x%6==0)
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
        }
        else
    	{
            wait(&sem);
    	    owner = 3;
            while(x--)
    	    {
    		if(x%6==0)
    		{
    		    signal(&sem);
    		    while(owner == 3)
    			;
    		    wait(&sem);
    		    owner = 3;
    		}
                printk(BLUEB "C" NORMAL);
                for(int i = 0; i < 0xFFF; i++);
            }
            signal(&sem);
        }
    }
    
    SHOW_STAT_FAILED("Discovering devices");
    SHOW_STAT_FAILED("Filesystem");
    SHOW_STAT_FAILED("Networking");
    SHOW_STAT_FAILED("Shell");
}

// this function doesn't return unless when powering off the system
extern "C" void _init();
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
extern "C" void readline(char* buf, int max);
extern "C" char* alloc_page();
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

