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

/* bkerndev - Bran's Kernel Development Tutorial
 *	By:	Brandon F. (friesenb@gmail.com)
 *	Desc: Interrupt Service Routines installer and exceptions
 *
 *	Notes: No warranty expressed or implied. Use at own risk. */
#include <idt.h>
#include <lib.h>
#include <timer.h>
#include <asm.h>
#include <debug.h>

/* These are function prototypes for all of the exception
 *	handlers: The first 32 entries in the IDT are reserved
 *	by Intel, and are designed to service exceptions! */
ISR(0)	ISR(1)	ISR(2)	ISR(3)	ISR(4)	ISR(5)	ISR(6)	ISR(7)
ISR(8)	ISR(9)	ISR(10)	ISR(11)	ISR(12)	ISR(13)	ISR(14)	ISR(15)
ISR(16)	ISR(17)	ISR(18)	ISR(19)	ISR(20)	ISR(21)	ISR(22)	ISR(23)
ISR(24)	ISR(25)	ISR(26)	ISR(27)	ISR(28)	ISR(29)	ISR(30)	ISR(31)

/* This is a very repetitive function... it's not hard, it's
 *	just annoying. As you can see, we set the first 32 entries
 *	in the IDT to the first 32 ISRs. We can't use a for loop
 *	for this, because there is no way to get the function names
 *	that correspond to that given entry. We set the access
 *	flags to 0x8E. This means that the entry is present, is
 *	running in ring 0 (kernel level), and has the lower 5 bits
 *	set to the required '14', which is represented by 'E' in
 *	hex. */
void isr_install()
{
	INSTALL_ISR(0)	INSTALL_ISR(1)	INSTALL_ISR(2)	INSTALL_ISR(3)	INSTALL_ISR(4)	INSTALL_ISR(5)	INSTALL_ISR(6)	INSTALL_ISR(7)
		INSTALL_ISR(8)	INSTALL_ISR(9)	INSTALL_ISR(10)	INSTALL_ISR(11)	INSTALL_ISR(12)	INSTALL_ISR(13)	INSTALL_ISR(14)	INSTALL_ISR(15)
		INSTALL_ISR(16)	INSTALL_ISR(17)	INSTALL_ISR(18)	INSTALL_ISR(19)	INSTALL_ISR(20)	INSTALL_ISR(21)	INSTALL_ISR(22)	INSTALL_ISR(23)
		INSTALL_ISR(24)	INSTALL_ISR(25)	INSTALL_ISR(26)	INSTALL_ISR(27)	INSTALL_ISR(28)	INSTALL_ISR(29)	INSTALL_ISR(30)	INSTALL_ISR(31)

		//printf("32 ISRs have been installed...\n");

		//~ ASM ("sti");
		}

/* This is a simple string array. It contains the message that
 *	corresponds to each and every exception. We get the correct
 *	message by accessing like:
 *	exception_message[interrupt_number] */
char *exception_messages[] =
{
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",

	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",

	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

void* get_faulted_address()
{
	int cr2;
	rcr2(cr2);
	return cr2;
}

int handling_fault = 0;
/* All of our Exception handling Interrupt Service Routines will
 *	point to this function. This will tell us what exception has
 *	happened! Right now, we simply halt the system by hitting an
 *	endless loop. All ISRs disable interrupts while they are being
 *	serviced as a 'locking' mechanism to prevent an IRQ from
 *	happening and messing up kernel data structures */
void fault_handler(struct interrupt_frame *r)
{
	handling_fault ++;
	if (r->int_no < 32)
	{
		if(handling_fault > 1)
			printf("\n\n!!fault in fault_handler: ");
		printf("Int %d(Ecode %d):",r->int_no,r->err_code);
		printf(exception_messages[r->int_no]);
		printf("\n");
		if(r->int_no == 14 /*page fault*/)
			printf("Faulting virtual address = 0x%x\n",get_faulted_address());
		printf("At EIP:0x%x CS:0x%x\n",r->eip,r->cs);
		printf("DS:0x%x SS:0x%x\n",r->ds,r->ss);
		printf("Tick: %d\n",timer_ticks);
		printf("Exception. System Halted!\n");

		if(handling_fault <= 1)
			print_stack_trace();

		hlt();
		for (;;);
	}
}
