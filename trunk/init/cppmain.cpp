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

extern "C" void call_ctors();
extern "C" void __cxa_finalize(void *d /*unused param*/);

void cppmain();

extern "C"
void enter_cpp()
{   
    
    printk(LOG "In C++ main\n");
    
    cppmain();

    // destruct all static C++ objects.
    __cxa_finalize(/*unused*/ NULL);
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
#define STAT_OK(str)	GREENB str NORMAL
#define STAT_FAILED(str)	REDB str NORMAL
#define PREFILL(ch)	memsetw(ttys[active_tty].data+ttys[active_tty].ypos*COLUMNS,ch|(0x0700)/*|(ATTRIBUTE<<8)*/,COLUMNS);
#define SHOW_STAT(strWhat,strStat,len)	\
	PREFILL('.');\
	printf(WHITE strWhat NORMAL);\
	ttys[active_tty].xpos = COLUMNS - len - 2;\
	printf(LBRACK strStat RBRACK)
#define SHOW_STAT_OK(strWhat) SHOW_STAT(strWhat,STAT_OK("OK"),2)
#define SHOW_STAT_FAILED(strWhat) SHOW_STAT(strWhat,STAT_FAILED(/*"FAILED"*/"NOT IMPLEMENTED YET"),strnlen("NOT IMPLEMENTED YET",40))

int esp = 0;

#include <semaphore.h>

volatile semaphore_t sem = 1;
volatile int owner = 0; // this is to force owner change, to test semaphors, instead of looping a constant number of times
void monitor();
// the init process
void init()
{    
    printk("in init now :)\n");

    int x = 5*6;
    monitor();
    
    printk("Forking: \n");
    int proc = fork();    
    printk(TRACE "this is process %i\n",proc);
    // if(!proc)
    // {
    //     wait(&sem);
    // 	owner = 0;
    //     while(x--)
    //     {
    // 	    if(x%6==0)
    // 	    {
    // 		signal(&sem);
    // 		// test semaphor: at no cost break printing 'A' except every 6 A's printed
    // 		while(owner == 0)
    // 		    ;
    // 		wait(&sem);
    // 		owner = 0;
    // 	    }
    //         printk(REDB "A" NORMAL);
    //         for(int i = 0; i < 0xFFF; i++);
    //     }
    //     signal(&sem);
    // }
    // else
    // {
    //     proc = fork();
	
    //     if(!proc)
    //     {
    // 	    wait(&sem);
    // 	    owner = 1;
    // 	    while(x--)
    //         {
    // 		if(x%6==0)
    // 		{
    // 		    signal(&sem);
    // 		    while(owner == 1)
    // 			;
    // 		    wait(&sem);
    // 		    owner = 1;
    // 		}
    // 		printk(GREENB "B" NORMAL);
    // 		for(int i = 0; i < 0xFFF; i++);
    //         }
    // 	    signal(&sem);
    //     }
    //     else
    // 	{
    //         wait(&sem);
    // 	    owner = 3;
    //         while(x--)
    // 	    {
    // 		if(x%6==0)
    // 		{
    // 		    signal(&sem);
    // 		    while(owner == 3)
    // 			;
    // 		    wait(&sem);
    // 		    owner = 3;
    // 		}
    //             printk(BLUEB "C" NORMAL);
    //             for(int i = 0; i < 0xFFF; i++);
    //         }
    //         signal(&sem);
    //     }
    // }
    
    //SHOW_STAT_FAILED("Discovering devices");
    //SHOW_STAT_FAILED("Filesystem");
    //SHOW_STAT_FAILED("Networking");
    //SHOW_STAT_FAILED("Shell");
}

// this function doesn't return unless when powering off the system
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

    call_ctors();
    
    int i = spawn(init);
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

