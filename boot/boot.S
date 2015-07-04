	/* boot.S - bootstrap the kernel */
	#define ASM	1
	#include <multiboot.h>

	.text

	.globl	start, _start,
	start:
	_start:
	jmp	multiboot_entry

	.section .__mbHeader
	.align	4                                                /* Align 32 bits boundary. */

	multiboot_header:
	.long	MULTIBOOT_HEADER_MAGIC                             /* magic */
	.long	MULTIBOOT_HEADER_FLAGS                             /* flags */
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS) /* checksum */
  #ifndef __ELF__
	.long	multiboot_header                                   /* header_addr */
	.long	_start                                             /* load_addr */
	.long	__end_data                                         /* load_end_addr */
	.long	__end_bss                                          /* bss_end_addr */
	.long	multiboot_entry                                    /* entry_addr */
	#endif                                                   /* ! __ELF__ */

	.text
	multiboot_entry:

	movl	$(stack + STACK_SIZE), %esp                        /* Initialize the stack pointer. */
	pushl	$0                                                 /* Reset EFLAGS. */
	popf


	pushl	%ebx                                               /* Push the pointer to the Multiboot information structure. */
	pushl	%eax                                               /* Push the magic value. */
	.global asm_to_c                                         /* Now enter the C main function... */
	pushl $0                                                 /* to stop the back trace here, set ebp = NULL */
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
