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

#ifndef KB_H
#define KB_H

#ifdef _cplusplus
extern "C" {
#endif

// since the known characters in there are ascii, then it is safe to use
// the range from 128 up for a special code for non-ascii keys
enum KbSpecialKeys { VK_CTRL = 128, VK_LSHIFT, VK_RSHIFT, VK_ALT,
					 VK_CAPS_LOCK, VK_NUM_LOCK, VK_SCR_LOCK,
					 VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, 
					 VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12,
					 VK_HOME, VK_END, VK_PAGE_DOWN, VK_PAGE_UP,
					 VK_INSERT, VK_DELETE, 
					 VK_UP_ARROW, VK_LEFT_ARROW, VK_RIGHT_ARROW, VK_DOWN_ARROW,
					 VK_UNDEFINED = 0 };

/* KBDUS means US Keyboard Layout. This is a scancode table
*	used to layout a standard US keyboard. I have left some
*	comments in to give you an idea of what key is what, even
*	though I set it's array index to 0. You can change that to
*	whatever you want using a macro, if you wish! */
extern unsigned char KeyboardUs[128]; 

void init_kb(); 

#ifdef _cplusplus
}
#endif

#endif //KB_H 
