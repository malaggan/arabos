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

%macro irq_no_err_code 1
	global _irq%1
	_irq%1:
	cli
	push byte 0
	push byte %1
	jmp irq_common_stub
%endmacro

SECTION .text
; 32 in IDT: IRQ0
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
; 47 in IDT: IRQ15
irq_no_err_code 15

extern irq_handler

irq_common_stub:
	pusha
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
	mov eax, irq_handler
	call eax
	pop eax

	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8
	iret
	