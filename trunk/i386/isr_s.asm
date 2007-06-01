; Copyright (C) 2007  Mohammad Nabil 
; mohammad (dot) nabil (dot) h (at) gmail (dot) com
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.

; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

; i mean 4bytes by dword

%macro isr_no_err_code 1
	global _isr%1
	_isr%1:
	cli
	push dword 0 
	push dword %1
	jmp isr_common_stub
%endmacro

%macro isr_with_err_code 1
	global _isr%1
	_isr%1:
	cli
	push dword %1
	jmp isr_common_stub
%endmacro

extern printf

SECTION .text
;	0: Divide By Zero Exception
isr_no_err_code 0

;	1: Debug Exception
isr_no_err_code 1

;	2: Non Maskable Interrupt Exception
isr_no_err_code 2

;	3: Int 3 Exception
isr_no_err_code 3

;	4: INTO Exception
isr_no_err_code 4

;	5: Out of Bounds Exception
isr_no_err_code 5

;	6: Invalid Opcode Exception
isr_no_err_code 6

;	7: Coprocessor Not Available Exception
isr_no_err_code 7

;	8: Double Fault Exception (With Error Code!)
isr_with_err_code 8

;	9: Coprocessor Segment Overrun Exception
isr_no_err_code 9

; 10: Bad TSS Exception (With Error Code!)
isr_with_err_code 10

; 11: Segment Not Present Exception (With Error Code!)
isr_with_err_code 11

; 12: Stack Fault Exception (With Error Code!)
isr_with_err_code 12

; 13: General Protection Fault Exception (With Error Code!)
isr_with_err_code 13

; 14: Page Fault Exception (With Error Code!)
isr_with_err_code 14

; 15: Reserved Exception
isr_no_err_code 15

; 16: Floating Point Exception
isr_no_err_code 16

; 17: Alignment Check Exception
isr_no_err_code 17

; 18: Machine Check Exception
isr_no_err_code 18

; Reserved
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


; We call a C function in here. We need to let the assembler know
; that '_fault_handler' exists in another file
extern fault_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
	pushad
        push ss
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10        
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax        
	mov eax, esp
	push eax
	mov eax, fault_handler
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popad
; i think this should remove both the interrupt number and the error code
	add esp, 8 
	iret
