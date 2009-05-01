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

#ifndef IDT_H
#define IDT_H

#ifdef __cplusplus
extern "C" {
#endif

#define EOI (char)0x20 /* end of interrupt, used in signaling the PIC after handling IRQ */

#define INTERRUPT_PRESENT		(0x80)
#define INTERRUPT_RING(n)		((n&0x3)<<5)
#define INTERRUPT_OP_SIZE_32		(0x8)
#define IDT_GATE_TYPE_TASK		(0x5)
#define IDT_GATE_TYPE_INTERRUPT		(0x6)
#define IDT_GATE_TYPE_TRAP		(0x7)

#define ISR_GATE ((unsigned char)( INTERRUPT_PRESENT | INTERRUPT_RING( 0 ) | INTERRUPT_OP_SIZE_32 | IDT_GATE_TYPE_INTERRUPT ))
#define IRQ_GATE ( ISR_GATE ) /*0x8E:1 present, 00 ring 0, 0 , 1 32 bits, 110 interrupt	*/

#define ISR(n) extern void _isr##n();
#define IRQ(n) extern void _irq##n();

#define INSTALL_ISR(n) idt_set_gate((unsigned char)n,		\
						(unsigned)_isr##n,	\
						(unsigned short)0x08,				\
						ISR_GATE);

#define INSTALL_IRQ(n) idt_set_gate(n+32,	\
						(unsigned)_irq##n,	\
						(unsigned short)0x08,				\
						IRQ_GATE);

/* This defines what the stack looks like after when ISR/IRQ handler is called */
struct interrupt_frame
{
	unsigned int ss, gs, fs, es, ds;		/* we pushed the segs last */

	unsigned int edi, esi, ebp, esp,
				 ebx, edx, ecx, eax;	/* pushed by 'pusha' */

	unsigned int int_no,err_code;
        /* our 'push byte #' and error codes do this */

	unsigned int eip, cs, eflags;   /* pushed by the processor automatically */
        /*useresp, ss << these only are pushed on a privilage level change*/
} __attribute__((packed));

/* Defines an IDT entry */
struct idt_entry
{
	unsigned short base_lo;
	unsigned short sel;	/* Our kernel segment goes here! */
	unsigned char always0;	/* This will ALWAYS be set to 0! */
	unsigned char flags;	/* access
							P	DPL	Always 01110 (14)
							P - Segment is present? (1 = Yes)
							DPL - Which Ring (0 to 3) */
	unsigned short base_hi;
} __attribute__((packed));

struct tag_idt_ptr
{
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

typedef int (*interrupt_handler_t)(struct interrupt_frame *r);

/* This exists in 'start.asm', and is used to load our IDT */
extern void idt_load();

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
void idt_install();
void isr_install();
void irq_install();
interrupt_handler_t irq_install_custom_handler(int irq,interrupt_handler_t handler);
void irq_uninstall_custom_handler(int irq);

#ifdef __cplusplus
}
#endif

#endif // IDT_H
