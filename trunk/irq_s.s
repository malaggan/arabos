# 1 "i386/irq_s.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "i386/irq_s.S"
# 18 "i386/irq_s.S"
.macro irq_no_err_code irq_no
 .global _irq\irq_no
 _irq\irq_no:
 pushl $0
 pushl \irq_no
 jmp irq_common_stub
.endm

.section .text

irq_no_err_code 0
irq_no_err_code 1
irq_no_err_code 2
irq_no_err_code 3
irq_no_err_code 4
irq_no_err_code 5
irq_no_err_code 6
irq_no_err_code 7
irq_no_err_code 8
irq_no_err_code 9
irq_no_err_code 10
irq_no_err_code 11
irq_no_err_code 12
irq_no_err_code 13
irq_no_err_code 14

irq_no_err_code 15



.global irq_handler

irq_common_stub:
 pusha
 pushw %ds
 pushw %es
 pushw %fs
 pushw %gs

    movw $0x10,%ax
 movw %ax, %ds
 movw %ax, %es
 movw %ax, %fs
 movw %ax, %gs


 movl %esp, %eax
    subl $4, %eax

 pushl %eax
 movl irq_handler, %eax
 call %eax
 popl %eax

 popw %gs
 popw %fs
 popw %es
 popw %ds
 popa
 add $8, %esp
 iret
