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

#include <kb.h>
#include <console.h>
#include <printf.h>
#include <idt.h>

unsigned char KeyboardUs[128] =
{
	0,	27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
	'9', '0', '-', '=', '\b',	/* Backspace */
	'\t',	/* Tab */
	'q', 'w', 'e', 'r',	/* 19 */
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
	VK_CTRL,	/* 29	- Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
	'\'', '`',	VK_LSHIFT,	/* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',	/* 49 */
	'm', ',', '.', '/',	VK_RSHIFT,	/* Right shift */
	'*',
	VK_ALT,	/* Alt */
	' ',	/* Space bar */
	VK_CAPS_LOCK,	/* Caps lock */
	VK_F1,	/* 59 - F1 key ... > */
	VK_F2,	VK_F3,	VK_F4,	VK_F5,	VK_F6,	VK_F7,	VK_F8,	VK_F9,
	VK_F10,	/* < ... F10 */
	VK_NUM_LOCK,	/* 69 - Num lock*/
	VK_SCR_LOCK,	/* Scroll Lock */
	VK_HOME,	/* Home key */
	VK_UP_ARROW,	/* Up Arrow */
	VK_PAGE_UP,	/* Page Up */
	'-',
	VK_LEFT_ARROW,	/* Left Arrow */
	0,
	VK_RIGHT_ARROW,	/* Right Arrow */
	'+',
	VK_END,	/* 79 - End key*/
	VK_DOWN_ARROW,	/* Down Arrow */
	VK_PAGE_DOWN,	/* Page Down */
	VK_INSERT,	/* Insert Key */
	VK_DELETE,	/* Delete Key */
	0,	0,	0,
	VK_F11,	/* F11 Key */
	VK_F12,	/* F12 Key */
	0,	/* All other keys are undefined */
};

void keyboard_handler(struct interrupt_frame *r)
{
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inportb((unsigned short)0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
    }
    else
    {
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */
        putchar(KeyboardUs[scancode]);
    }
}

void init_kb()
{
	irq_install_custom_handler(1,keyboard_handler);
}
