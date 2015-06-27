// -*- mode: c++; -*-
#pragma once

constexpr char EOI = 0x20; /* end of interrupt, used in signaling the PIC after handling IRQ */

constexpr unsigned int INTERRUPT_PRESENT       =	0x80;
constexpr unsigned int INTERRUPT_RING(unsigned char n) { return	(n & 0x3) << 5; }
constexpr unsigned int INTERRUPT_OP_SIZE_32    =	0x8;
constexpr unsigned int IDT_GATE_TYPE_TASK      =	0x5;
constexpr unsigned int IDT_GATE_TYPE_INTERRUPT =  0x6;
constexpr unsigned int IDT_GATE_TYPE_TRAP      =	0x7;

constexpr unsigned char ISR_GATE = INTERRUPT_PRESENT | INTERRUPT_RING( 0 ) | INTERRUPT_OP_SIZE_32 | IDT_GATE_TYPE_INTERRUPT;
constexpr unsigned char IRQ_GATE = ISR_GATE; /*0x8E:1 present, 00 ring 0, 0 , 1 32 bits, 110 interrupt	*/

// extern "C" because defined in assembly, and we do not want name manling
#define IRQ(n) extern "C" void _irq##n();

#define INSTALL_IRQ(n) idt_set_gate(n+32,	\
                                    (unsigned)_irq##n,	\
                                    (unsigned short)0x08,	\
                                    IRQ_GATE);

/* This defines what the stack looks like after when ISR/IRQ handler is called */
struct interrupt_frame
{
	unsigned int ss, gs, fs, es, ds;		/* we pushed the segs last */
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;	/* pushed by 'pusha' */
	unsigned int int_no,err_code;	/* our 'push byte #' and error codes do this */
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
