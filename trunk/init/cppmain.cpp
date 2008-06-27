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
#include <printf.h>
#include <console.h>
#include <mem.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <kb.h>
#include <mm.h>
#include <cpp.h>

extern "C" void call_ctors();
extern "C" void __cxa_finalize(void *d /*unused param*/);

void cppmain();

extern "C"
void enter_cpp()
{   
    call_ctors();
    
    printk(LOG "In C++ main\n");    
        
    cppmain();
    
    __cxa_finalize(/*unused*/ NULL);
}

class Koko { public: Koko() {printf("koko\n");} ~Koko(){printf("~koko\n");}} kk;

extern int __end,__phys;
int kernel_end_addr=(int)(&__end),kernel_load_addr=(int)(&__phys); // the linker symbol have only an address (in the symbol table)

void cppmain()
{          
    cpuid_check();
    
    printf(WHITE "Welcome to " REDB "ArOS" BLUEB " v 0.003_1!\n" NORMAL);	
 
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
    ASM ("sti");
    SHOW_STAT_OK("IDT");

    init_kb();

    init_paging();

    //SHOW_STAT_FAILED("Kernel memory allocator");
    //SHOW_STAT_FAILED("Process manager");
    //SHOW_STAT_FAILED("Discovering devices");
    //SHOW_STAT_FAILED("Filesystem");
    //SHOW_STAT_FAILED("Networking");
    //SHOW_STAT_FAILED("Shell");
	
}


