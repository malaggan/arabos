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

/*SYS calls*/
irq_no_err_code2(16,"irq_fork_16") // fork
irq_no_err_code2(17,"irq_spawn_17") // spawn

