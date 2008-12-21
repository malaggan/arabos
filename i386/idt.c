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

#include <idt.h>
#include <lib.h>
#include <asm.h>

/* Declare an IDT of 256 entries. Although we will only use the
*	first 32 entries in this tutorial, the rest exists as a bit
*	of a trap. If any undefined IDT entry is hit, it normally
*	will cause an "Unhandled Interrupt" exception. Any descriptor
*	for which the 'presence' bit is cleared (0) will generate an
*	"Unhandled Interrupt" exception */
struct idt_entry idt[256] = { {0,0,0,0,0}, };
struct tag_idt_ptr idt_ptr = {0,0};

/* Use this function to set an entry in the IDT. Alot simpler
*	than twiddling with the GDT ;) */
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{	//	_set_gate(num,type,dpl,addr)
	/* We'll leave you to try and code this function: take the
	*	argument 'base' and split it up into a high and low 16-bits,
	*	storing them in idt[num].base_hi and base_lo. The rest of the
	*	fields that you must set in idt[num] are fairly self-
	*	explanatory when it comes to setup */
	idt[num].base_hi = base >> 16;
	idt[num].base_lo = base & 0xffff;
	idt[num].sel = sel;
	idt[num].always0 = 0;
	idt[num].flags = flags;
}

/* Installs the IDT */
void idt_install()
{
	/* Sets the special IDT pointer up, just like in 'gdt.c' */
	idt_ptr.limit = (sizeof (struct idt_entry) * 256) - 1;
	idt_ptr.base = (unsigned int)&idt;

	/* Clear out the entire IDT, initializing it to zeros */
	memset((unsigned char*)&idt, (char)0, (size_t)(sizeof(struct idt_entry) * 256));

	/* Add any new ISRs to the IDT here using idt_set_gate */
	isr_install();

	/* Points the processor's internal register to the new IDT */
        lidt(idt_ptr);
}
