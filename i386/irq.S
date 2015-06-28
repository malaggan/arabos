	/* extern irq_handler*/
	.extern irq_handler

	//.stabs "irq_common_stub",36,0,0,irq_common_stub
	.macro irq_common_stub
	pusha
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	pushl %ss
	
	movw $0x10,%ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	
	/* the pointer to the stack-frame-based-img of the interrupt frame */
	movl %esp, %eax
	//subl $4, %eax /* FIXME: there is an extra parameter pushed on the stack !!, this is just a fix */
	
	pushl %eax // param
	movl $irq_handler, %eax
	call *%eax
	popl %eax // return value is returned through popa, not here
	
	popl %ss
	popl %gs
	popl %fs
	popl %es
	popl %ds
	popa
	add $8, %esp // remove the int_no and error code
	sti
	iret
	.endm
	
	#define mkstr(s) #s
	
	#define irq_no_err_code2(irq_no,s) \
	.globl _irq##irq_no; \
	.type _irq##irq_no, @function; \
	.align 2; \
	.stabs s,36,0,0,_irq##irq_no; \
	_irq##irq_no: \
	cli; \
	pushl $0; \
	pushl $(irq_no); \
	irq_common_stub //jmp irq_common_stub
	
	#define irq_no_err_code(irq_no) irq_no_err_code2(irq_no,mkstr(_irq##irq_no:F1))
	
	.section .text
	/* 32 in IDT: IRQ0 */
	irq_no_err_code (0)
	irq_no_err_code (1) 
	irq_no_err_code (2)
	irq_no_err_code (3)
	irq_no_err_code (4)
	irq_no_err_code (5)
	irq_no_err_code (6)
	irq_no_err_code (7)
	irq_no_err_code (8)
	irq_no_err_code (9)
	irq_no_err_code (10)
	irq_no_err_code (11)
	irq_no_err_code (12)
	irq_no_err_code (13)
	irq_no_err_code (14)
	/* 47 in IDT: IRQ15 */
	irq_no_err_code (15)

	.globl _irqqq1		
	.type _irqqq1, @function	
	.align 2
	.stabs mkstr(_irqqq1:F1),77,0,0,_irqqq1
_irqqq1: 	
	cli
	pusha

	mov $65,%al
	outb %al,$0xe9
	
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	pushl %ss
	
	movw $0x10,%ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	mov $50, %ecx /* loop only for 50 loops */
/*	jmp wait_ready*/
	
/*parity_err:	
	mov $67,%al
	outb %al,$0xe9*/
/*wait_ready: */ /* only used if we are not using interrupts ? */
/* 	inb $0x64, %al
	mov %al, %bl
 	test $0x1,%al
 	loopnz wait_ready */

flush:
	mov $66,%al
	outb %al,$0xe9
	
 	inb $0x60, %al
 	outb %al, $0xe9

	/* parity error or receive timeout? */
	/* test $0xc0, %bl
	jnz parity_err */

	/* flush buffer */
 	inb $0x64, %al 
 	test $0x1, %al	
 	jnz flush

	/* reset */
/*wait_write:	
	inb $0x64, %al
	test $0x2, %al
	jnz wait_write
	mov $0x60, %al
	outb %al, $0x64
	mov $0xFF, %al
	outb %al, $0x60 */
	
	popl %ss
	popl %gs
	popl %fs
	popl %es
	popl %ds
	popa

	mov $0x20, %al
	outb %al,$0x20 		

	sti
	
	mov $79,%al
	outb %al,$0xe9

	iret	

	/*SYS calls*/
	irq_no_err_code2(16,"irq_fork_16") // fork
	irq_no_err_code2(17,"irq_spawn_17") // spawn
