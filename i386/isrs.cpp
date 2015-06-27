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
// extern "C" because defined in assembly, and we do not want name manling
extern "C" void _isr0 ();
extern "C" void _isr1 ();
extern "C" void _isr2 ();
extern "C" void _isr3 ();
extern "C" void _isr4 ();
extern "C" void _isr5 ();
extern "C" void _isr6 ();
extern "C" void _isr7 ();
extern "C" void _isr8 ();
extern "C" void _isr9 ();
extern "C" void _isr10 ();
extern "C" void _isr11 ();
extern "C" void _isr12 ();
extern "C" void _isr13 ();
extern "C" void _isr14 ();
extern "C" void _isr15 ();
extern "C" void _isr16 ();
extern "C" void _isr17 ();
extern "C" void _isr18 ();
extern "C" void _isr19 ();
extern "C" void _isr20 ();
extern "C" void _isr21 ();
extern "C" void _isr22 ();
extern "C" void _isr23 ();
extern "C" void _isr24 ();
extern "C" void _isr25 ();
extern "C" void _isr26 ();
extern "C" void _isr27 ();
extern "C" void _isr28 ();
extern "C" void _isr29 ();
extern "C" void _isr30 ();
extern "C" void _isr31 ();

/* This is a very repetitive function... it's not hard, it's
 *	just annoying. As you can see, we set the first 32 entries
 *	in the IDT to the first 32 ISRs. We can't use a for loop
 *	for this, because there is no way to get the function names
 *	that correspond to that given entry. We set the access
 *	flags to 0x8E. This means that the entry is present, is
 *	running in ring 0 (kernel level), and has the lower 5 bits
 *	set to the required '14', which is represented by 'E' in
 *	hex. */
using isr_t = void(void);
constexpr isr_t* isrs[] = { _isr0,_isr1,_isr2,_isr3,_isr4,_isr5,_isr6,_isr7,_isr8,_isr9,_isr10,_isr11,_isr12,_isr13,_isr14,_isr15,_isr16,_isr17,_isr18,_isr19,_isr20,_isr21,_isr22,_isr23,_isr24,_isr25,_isr26,_isr27,_isr28,_isr29,_isr30,_isr31 };

void isr_install() {
	size_t i = 0;
	for(auto isr : isrs)
		idt_set_gate(i++, reinterpret_cast<unsigned int>(isr), 0x08,	ISR_GATE);
}

/* This is a simple string array. It contains the message that
 *	corresponds to each and every exception. We get the correct
 *	message by accessing like:
 *	exception_message[interrupt_number] */
char const *exception_messages[] =
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
	return reinterpret_cast<void*>(cr2);
}

int handling_fault = 0;
/* All of our Exception handling Interrupt Service Routines will
 *	point to this function. This will tell us what exception has
 *	happened! Right now, we simply halt the system by hitting an
 *	endless loop. All ISRs disable interrupts while they are being
 *	serviced as a 'locking' mechanism to prevent an IRQ from
 *	happening and messing up kernel data structures */
extern "C" // because called from assembly
void
fault_handler(struct interrupt_frame *r)
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
