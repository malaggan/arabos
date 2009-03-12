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

extern "C" void call_ctors();
extern "C" void __cxa_finalize(void *d /*unused param*/);

void cppmain();

extern "C"
void enter_cpp()
{   
    call_ctors();
    
    printk(LOG "In C++ main\n");    
        
    cppmain();

    // destruct all static C++ objects.
    __cxa_finalize(/*unused*/ NULL);
}

class Koko { public: Koko() {printf("koko\n");} ~Koko(){printf("~koko\n");}} kk;

extern int __end,__phys; // filled by the linker script
int kernel_end_addr=(int)(&__end),kernel_load_addr=(int)(&__phys); // the linker symbol have only an address (in the symbol table)

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
#define SHOW_STAT_OK(strWhat) SHOW_STAT(strWhat,STAT_OK("OK"),strlen("OK"))
#define SHOW_STAT_FAILED(strWhat) SHOW_STAT(strWhat,STAT_FAILED(/*"FAILED"*/"NOT IMPLEMENTED YET"),strlen("NOT IMPLEMENTED YET"))

// this function doesn't return unless when powering off the system
void cppmain()
{
#if 1
    cpuid_check();
#endif
    
    printf(WHITE "Welcome to " REDB "ArOS" BLUEB " v 0.004!\n" NORMAL);
 
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
    
    //SHOW_STAT_FAILED("Kernel memory allocator");
    //SHOW_STAT_FAILED("Process manager");
    //SHOW_STAT_FAILED("Discovering devices");
    //SHOW_STAT_FAILED("Filesystem");
    //SHOW_STAT_FAILED("Networking");
    //SHOW_STAT_FAILED("Shell");
    
    
    //while(1);
}


