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

#include <console.h>
#include <lib.h>

struct tty_t ttys[TTY_CNT];
unsigned int active_tty;

void flush_current_tty()
{
    flush_video(ttys[active_tty]);
}

void flush_video(struct tty_t tty)
{
    memcpyw(VIDEO,tty.data,COLUMNS*LINES);
}

void scroll(void)
{
	unsigned short blank;

	if(ttys[active_tty].ypos >= LINES)
	{
		/* A blank is defined as a space... we need to give it
		*	backcolor too */
		blank = 0x0720; /* 07 black, 20 space*/

		/* Row 25 is the end, this means we need to scroll up */
		/* Move the current text chunk that makes up the screen
		*	back in the buffer by a line */
		memcpyw(ttys[active_tty].data, ttys[active_tty].data + COLUMNS,COLUMNS*(LINES-1));

		/* Finally, we set the chunk of memory that occupies
		*	the last line of text to our 'blank' character */
		memsetw (ttys[active_tty].data + ((LINES-1) * COLUMNS), blank, COLUMNS);
		ttys[active_tty].ypos = LINES - 1;
	}
}

extern int printed_chars;

/* Put the character C on the screen. */
void
putchar (int c)
{
    int attribute = ((c>>8)& 0xff);
    c = c & 0xff;
    if(0 == attribute)
        attribute = 0x07;
    printed_chars++;
    volatile unsigned short *where;
    /* Handle a backspace, by moving the cursor back one space */
    if(c == '\b')
    {
        if(ttys[active_tty].xpos != 0) ttys[active_tty].xpos--;
    }
    /* Handles a tab by incrementing the cursor's x, but only
    *	to a point that will make it divisible by 8 */
    else if(c == '\t')
    {
        ttys[active_tty].xpos = (ttys[active_tty].xpos + 8) & ~(8 - 1);
    }
    /* Handles a 'Carriage Return', which simply brings the
    *	cursor back to the margin */
    else if(c == '\r')
    {
        ttys[active_tty].xpos = 0;
    }
    /* We handle our newlines the way DOS and the BIOS do: we
    *	treat it as if a 'CR' was also there, so we bring the
    *	cursor to the margin and we increment the 'y' value */
    else if(c == '\n')
    {
        ttys[active_tty].xpos = 0;
        ttys[active_tty].ypos++;
    }
    /* Any character greater than and including a space, is a
    *	printable character. The equation for finding the index
    *	in a linear chunk of memory can be represented by:
    *	Index = [(y * width) + x] */
    else if(c >= ' ')
    {
        where = ttys[active_tty].data + (ttys[active_tty].ypos * COLUMNS + ttys[active_tty].xpos);
        *where = c | attribute<<8;	/* Character AND attributes: color */
        ttys[active_tty].xpos++;
    }

    /* If the cursor has reached the edge of the tty0's width, we
    *	insert a new line in there */
    if(ttys[active_tty].xpos >= COLUMNS)
    {
        ttys[active_tty].xpos = 0;
        ttys[active_tty].ypos++;
    }

    /* Scroll the tty if needed, and finally move the cursor */
    scroll();
    update_cursor();
    flush_current_tty();
}

unsigned char ECMA48(const char ** format)
{
    /*	30	black foreground
    *	31	blue foreground
    *	32	green foreground
    *	33	brown foreground
    *	34	red foreground
    *	35	magenta (purple) foreground
    *	36	cyan (light blue) foreground
    *	37	gray foreground
    *
    *	40	black background
    *	41	blue background
    *	42	green background
    *	43	brown background
    *	44	red background
    *	45	magenta background
    *	46	cyan background
    *	47	white background
    *
    *	0	reset all attributes to their defaults
    *	1	set bold
    *	x5	set blink // x for not implemented
    *	x7	set reverse screen
    *	x22	set normal intensity
    *	x25	blink off
    *	x27	reverse screen off
    */
    const char* fmt = *format;
    unsigned char attribute = 0x07;
    if(*fmt == '[')
    {
        fmt++;
        while(*fmt != 'm')
        {
            switch(*fmt)
            {
            case '3':
                {
                    /*foreground color*/
                    fmt++;
                    attribute &= 0xf0;
                    attribute |= (*fmt-'0')&0xf;
                    fmt++;
                    break;
                }
            case '4':
                {
                    /*background color*/
                    fmt++;
                    attribute &= 0x0f;
                    attribute |= ((*fmt-'0')<<4)&0xf0;
                    fmt++;
                    break;
                }
            case '0':
                {
                    /*reset*/
                    fmt++;
                    attribute = 0x07;
                    break;
                }
            case '1':
                {
                    /*bright(bold)*/
                    fmt++;
                    attribute |= 0x08;
                    break;
                }
            case ';':
                {
                    fmt++;
                    break;
                }
            default:
                {
                    *format = fmt;
                    return 0;
                }
            }
        }
        fmt++;
    }
    *format = fmt;
    return attribute;
}



/* Clear the screen and initialize VIDEO, XPOS and YPOS. */
void
cls (void)
{
    memsetw((unsigned short*)ttys[active_tty].data,(unsigned short)0x0700,COLUMNS*LINES);

    ttys[active_tty].xpos = 0;
    ttys[active_tty].ypos = 0;
    update_cursor();
}

void update_cursor(void)
{
    /*doesn't work ??*/
    unsigned temp;

    /* The equation for finding the index in a linear
    *	chunk of memory can be represented by:
    *	Index = [(y * width) + x] */
    temp = (ttys[active_tty].ypos * COLUMNS + ttys[active_tty].xpos);

    /* This sends a command to indicies 14 and 15 in the
    *	CRT Control Register of the VGA controller. These
    *	are the high and low bytes of the index that show
    *	where the hardware cursor is to be 'blinking'. To
    *	learn more, you should look up some VGA specific
    *	programming documents. A great start to graphics:
    *	http://www.brackeen.com/home/vga */	 	

    outportb (0x3D4, 0x0F); 
    outportb (0x3D5, temp & 0xFF);
    temp >>= 8;
    outportb(0x3D4, 0xE); 
    outportb(0x3D5, temp & 0xFF);
}

void init_video(void)
{
    active_tty = 0;
    ttys[active_tty].xpos = 0;
    ttys[active_tty].ypos = 0;
    update_cursor();
    cls();
}
