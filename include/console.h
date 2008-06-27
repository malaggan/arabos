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

#include <sys.h>
#include <string.h>

/* Some screen stuff. */

/* The number of columns. */
#define COLUMNS	80

/* The number of lines. */
#define LINES	25

/* ECMA values
ref : http://pueblo.sourceforge.net/doc/manual/ansi_color_codes.html */

#define ECMA_PREFIX 	"\033["
#define ECMA_SUFFIX	"m"
#define ECMA_SEPARATOR	";"

#define ECMA_SET_DEFAULT	"0"
#define ECMA_SET_BOLD		"1"
#define ECMA_SET_ITALICS	"3"
#define ECMA_SET_UNDERLINE	"4"
#define ECMA_SET_INVERTED_COLOR	"7"
#define ECMA_SET_STRIKETHROUGH	"9"

#define ECMA_UNSET_BOLD			"22"
#define ECMA_UNSET_ITALICS		"23"
#define ECMA_UNSET_UNDERLINE		"24"
#define ECMA_UNSET_INVERTED_COLOR	"27"
#define ECMA_UNSET_STRIKETHROUGH	"29"

#define ECMA_BACK_GND 	"3"
#define ECMA_FORE_GND 	"4"

#define ECMA_BLACK	"0"
#define ECMA_RED	"4" /*"1"*/
#define ECMA_GREEN	"2"
#define ECMA_YELLOW	"3"
#define ECMA_BLUE	"1" /*"4"*/
#define ECMA_MAGENTA	"5"
#define ECMA_CYAN	"6"
#define ECMA_WHITE	"7"
#define ECMA_DEFAULT	"9"

/* The attribute of an character. */
#define NORMAL	ECMA_PREFIX ECMA_SET_DEFAULT ECMA_SUFFIX
#define WHITE	ECMA_PREFIX ECMA_BACK_GND ECMA_WHITE ECMA_SEPARATOR ECMA_SET_BOLD ECMA_SUFFIX
#define GREENB	ECMA_PREFIX ECMA_BACK_GND ECMA_GREEN ECMA_SEPARATOR ECMA_SET_BOLD ECMA_SUFFIX /* B for bold*/
#define REDB	ECMA_PREFIX ECMA_BACK_GND ECMA_RED   ECMA_SEPARATOR ECMA_SET_BOLD ECMA_SUFFIX
#define BLUEB	ECMA_PREFIX ECMA_BACK_GND ECMA_BLUE  ECMA_SEPARATOR ECMA_SET_BOLD ECMA_SUFFIX

extern char ATTRIBUTE; /* COOL this value determines the color */

/* The video memory address. */
#define VIDEO	0xB8000

/* Variables. */
/* Save the X position. */
extern int xpos;

/* Save the Y position. */
extern int ypos;

/* Point to the video memory. */
extern volatile unsigned short *video;

void cls (void);
void putchar (unsigned char c);
void ECMA48(const char ** format); /*color coded ascii*/
void update_cursor(void);
void init_video(void);

/* utility stuff */
#define LBRACK	WHITE "[" NORMAL
#define RBRACK	WHITE "]" NORMAL
#define STAT_OK(str)	GREENB str NORMAL
#define STAT_FAILED(str)	REDB str NORMAL
#define PREFILL(ch)	memsetw((unsigned short*)(video+ypos*COLUMNS),ch|ATTRIBUTE<<8,COLUMNS);
#define SHOW_STAT(strWhat,strStat,len)	\
	PREFILL('.');\
	printf(WHITE strWhat NORMAL);\
	xpos = COLUMNS - len - 2;\
	printf(LBRACK strStat RBRACK)
#define SHOW_STAT_OK(strWhat) SHOW_STAT(strWhat,STAT_OK("OK"),strlen("OK"))
#define SHOW_STAT_FAILED(strWhat) SHOW_STAT(strWhat,STAT_FAILED(/*"FAILED"*/"NOT IMPLEMENTED YET"),strlen("NOT IMPLEMENTED YET"))

#ifdef __cplusplus
}
#endif

#endif /*CONSOLE_H*/
