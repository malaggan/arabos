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

#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lib.h>

#define COLUMNS	80
#define LINES	25

/* The video memory address. */
#define VIDEO	((unsigned short*)0xB8000)  
#define TTY_CNT 1  
    
struct tty_t
{
    int xpos,ypos;
    unsigned short data[COLUMNS*LINES];
};

extern struct tty_t ttys[];
extern unsigned int active_tty;

void cls (void);
void putchar (int c);
void update_cursor(void);
void init_video(void);
void flush_video(struct tty_t tty);
void flush_current_tty();

#ifdef __cplusplus
}
#endif

#endif /*CONSOLE_H*/
