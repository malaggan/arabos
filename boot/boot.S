	/* boot.S - bootstrap the kernel */
	#define ASM	1
	#include <multiboot.h>

	.text

	.globl	start, _start,
	start:
	_start:
	jmp	multiboot_entry

	.section .__mbHeader
	/* Align 32 bits boundary. */
	.align	4

	multiboot_header:

	/* magic */
	.long	MULTIBOOT_HEADER_MAGIC

	/* flags */
	.long	MULTIBOOT_HEADER_FLAGS

	/* checksum */
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
	#ifndef __ELF__

	/* header_addr */
	.long	multiboot_header

	/* load_addr */
	.long	_start

	/* load_end_addr */
	.long	__end_data

	/* bss_end_addr */
	.long	__end_bss

	/* entry_addr */
	.long	multiboot_entry
	#endif /* ! __ELF__ */
	.text
	multiboot_entry:
	/* Initialize the stack pointer. */
	movl	$(stack + STACK_SIZE), %esp

	/* Reset EFLAGS. */
	pushl	$0
	popf

	/* Push the pointer to the Multiboot information structure. */
	pushl	%ebx
	/* Push the magic value. */
	pushl	%eax
	.global asm_to_c
	/* Now enter the C main function... */
	        pushl $0 /* to stop the back trace here, set ebp = NULL */
	        popl %ebp
	.stabs "asm_to_c:F1",36,0,0,asm_to_c
	asm_to_c: call	cmain

	/* Halt. */
	pushl	timer_ticks
	pushl	$halt_message
	call	printf
	popl	%eax
	popl	%eax

	loop:
	    pause
	    hlt
	    jmp	loop

	halt_message:
	.asciz	"Halted at tick %d :).\n"

	/* Our stack area. */
	.comm	stack, STACK_SIZE
