#include <idt.h>
#include <lib.h>
#include <asm.h>

/* These are own ISRs that point to our special IRQ handler
 *	instead of the regular 'fault_handler' function */
IRQ(0)	IRQ(1)	IRQ(2)	IRQ(3)	IRQ(4)	IRQ(5)	IRQ(6)	IRQ(7)
IRQ(8)	IRQ(9)	IRQ(10)	IRQ(11)	IRQ(12)	IRQ(13)	IRQ(14)	IRQ(15)

/* This array is actually an array of function pointers. We use
 *	this to handle custom IRQ handlers for a given IRQ */
interrupt_handler_t irq_routines[160] =
{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

/* This installs a custom IRQ handler for the given IRQ */
interrupt_handler_t irq_install_custom_handler(int irq, interrupt_handler_t handler)
{
	interrupt_handler_t old = irq_routines[irq];
	irq_routines[irq] = handler;
	return old;
}

/* This clears the handler for a given IRQ */
void irq_uninstall_custom_handler(int irq)
{
	irq_routines[irq] = 0;
}

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
 *	is a problem in protected mode, because IDT entry 8 is a
 *	Double Fault! Without remapping, every time IRQ0 fires,
 *	you get a Double Fault Exception, which is NOT actually
 *	what's happening. We send commands to the Programmable
 *	Interrupt Controller (PICs - also called the 8259's) in
 *	order to make IRQ0 to 15 be remapped to IDT entries 32 to
 *	47 */
void irq_remap(void)
{
	//if you plan to return to real mode (for any purpose), you really must restore the PIC to its former configration.
	// u can save them, see the link below

	// to do : io_wait ?

	outportb((unsigned short)0x20, (char)0x11); // init
	outportb((unsigned short)0x21, (char)0x20); // offset (v.) ( must be divisible by 8 )
	outportb((unsigned short)0x21, (char)0x04);
	outportb((unsigned short)0x21, (char)0x01); // 8086/88 (MCS-80/85) mode ( see http://www.mega-tokyo.com/osfaq/Can%20I%20remap%20the%20PIC%3F )
	outportb((unsigned short)0x21, (char)0x0);  // FIXME // ?? // old value 0x01  // unmask all interrupts

	outportb((unsigned short)0xA0, (char)0x11);
	outportb((unsigned short)0xA1, (char)0x28);
	outportb((unsigned short)0xA1, (char)0x02);
	outportb((unsigned short)0xA1, (char)0x01);
	outportb((unsigned short)0xA1, (char)0x0);
}

int handle_coprocessor_math_fault(struct interrupt_frame *r)
{
	ASM (" fnclex "); // clear the exception existance
	printf("fnclex done\n");
	return 0;
}

/* We first remap the interrupt controllers, and then we install
 *	the appropriate ISRs to the correct entries in the IDT. This
 *	is just like installing the exception handlers */
void irq_install()
{
/*
  ASM( "mov %cr0,%eax\r\n"
  "and $0xfffffff7,%eax\r\n"
  "or $4,%eax\r\n"
  "mov %eax,%cr0" ); // clear TS and sets EM in the cr0
*/
	irq_remap();

	INSTALL_IRQ(0)	INSTALL_IRQ(1)	INSTALL_IRQ(2)	INSTALL_IRQ(3)	INSTALL_IRQ(4)	INSTALL_IRQ(5)	INSTALL_IRQ(6)	INSTALL_IRQ(7)
		INSTALL_IRQ(8)	INSTALL_IRQ(9)	INSTALL_IRQ(10)	INSTALL_IRQ(11)	INSTALL_IRQ(12)	INSTALL_IRQ(13)	INSTALL_IRQ(14)	INSTALL_IRQ(15)
		irq_install_custom_handler(13,handle_coprocessor_math_fault);
}

/* Each of the IRQ ISRs point to this function, rather than
 *	the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
 *	to be told when you are done servicing them, so you need
 *	to send them an "End of Interrupt" command (0x20). There
 *	are two 8259 chips: The first exists at 0x20, the second
 *	exists at 0xA0. If the second controller (an IRQ from 8 to
 *	15) gets an interrupt, you need to acknowledge the
 *	interrupt at BOTH controllers, otherwise, you only send
 *	an EOI command to the first controller. If you don't send
 *	an EOI, you won't raise any more IRQs */
#include <debug.h>
void irq_handler(struct interrupt_frame *r)
{
	/* This is a blank function pointer */
	interrupt_handler_t handler;

	/* Find out if we have a custom handler to run for this
	 *	IRQ, and then finally, run it */
	handler = irq_routines[r->int_no];

	if(r->int_no > 0)
	{
		//printf("\nIRQ %d\n",r->int_no);
		//printf("IRQ %d(Ecode %d):\n",r->int_no,r->err_code);
		//printf("At EIP:0x%x CS:0x%x\n",r->eip,r->cs);
		//printf("DS:0x%x SS:0x%x\n",r->ds,r->ss);
	}

	if (handler)
	{
		handler(r);
	}
	else
	{
		//printf("No custom handler exists for IRQ%d\n",r->int_no);
	}

	/* If the IDT entry that was invoked was greater than 40
	 *	(meaning IRQ8 - 15), then we need to send an EOI to
	 *	the slave controller */
	if (r->int_no >= 8)
	{
		outportb((unsigned short)0xA0, EOI /*end of interrupt*/);
	}

	/* In either case, we need to send an EOI to the master
	 *	interrupt controller too */
	outportb((unsigned short)0x20, EOI /*end of interrupt*/);
}
