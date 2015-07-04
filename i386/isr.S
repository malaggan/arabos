// i mean 4bytes by dword

.macro isr_no_err_code isr_no
	.globl _isr\isr_no
	.type _isr\isr_no, @function
	.align 2
	_isr\isr_no:
  cli
	pushl $0
	pushl $(\isr_no)
	jmp isr_common_stub
.endm

.macro isr_with_err_code isr_no
	.globl _isr\isr_no
	.type _isr\isr_no, @function
	.align 2
	_isr\isr_no:
	cli
	pushl $\isr_no
	jmp isr_common_stub
.endm

.extern printf

.section .text
//	0: Divide By Zero Exception
isr_no_err_code 0

//	1: Debug Exception
isr_no_err_code 1

//	2: Non Maskable Interrupt Exception
isr_no_err_code 2

//	3: Int 3 Exception
isr_no_err_code 3

//	4: INTO Exception
isr_no_err_code 4

//	5: Out of Bounds Exception
isr_no_err_code 5

//	6: Invalid Opcode Exception
isr_no_err_code 6

//	7: Coprocessor Not Available Exception
isr_no_err_code 7

//	8: Double Fault Exception (With Error Code!)
isr_with_err_code 8

//	9: Coprocessor Segment Overrun Exception
isr_no_err_code 9

// 10: Bad TSS Exception (With Error Code!)
isr_with_err_code 10

// 11: Segment Not Present Exception (With Error Code!)
isr_with_err_code 11

// 12: Stack Fault Exception (With Error Code!)
isr_with_err_code 12

// 13: General Protection Fault Exception (With Error Code!)
isr_with_err_code 13

// 14: Page Fault Exception (With Error Code!)
isr_with_err_code 14

// 15: Reserved Exception
isr_no_err_code 15

// 16: Floating Point Exception
isr_no_err_code 16

// 17: Alignment Check Exception
isr_no_err_code 17

// 18: Machine Check Exception
isr_no_err_code 18

// Reserved
isr_no_err_code 19
isr_no_err_code 20
isr_no_err_code 21
isr_no_err_code 22
isr_no_err_code 23
isr_no_err_code 24
isr_no_err_code 25
isr_no_err_code 26
isr_no_err_code 27
isr_no_err_code 28
isr_no_err_code 29
isr_no_err_code 30
isr_no_err_code 31


// We call a C function in here. We need to let the assembler know
// that '_fault_handler' exists in another file
.extern fault_handler

// This is our common ISR stub. It saves the processor state, sets
// up for kernel mode segments, calls the C-level fault handler,
// and finally restores the stack frame.
isr_common_stub:
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

	pushl %eax // param
	movl $fault_handler, %eax
  cld // GCC ABI requires this on entry of C functions, should the eflags be restored before iret?
	call *%eax
	popl %eax // return value is returned through popa, not here

  popl %ss
	popl %gs
	popl %fs
	popl %es
	popl %ds
	popa
	add $8, %esp // remove the int_no and error code
	iret
