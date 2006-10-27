; Loads the IDT defined in '_idtp' into the processor.
; This is declared in C as 'extern void idt_load();'
extern idt_ptr
global idt_load
SECTION .text
idt_load:
	lidt [idt_ptr]
	ret
