#include <console.h>
#include <port.h>
#include <memops.h>
#include <serial.h>

struct tty_t ttys[TTY_CNT];
unsigned int active_tty;

void flush_current_tty()
{
	flush_video(&ttys[active_tty]);
}

void flush_video(struct tty_t *tty)
{
	memcpyw(VIDEO,tty->data,COLUMNS*LINES);
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

// used to output to the bochs console (the host's console)
void lpt_putc(int c)
{
	#if VM_ECMA
	int i;

	for (i = 0; !(inportb(0x378+1) & 0x80) && i < 12800; i++)
		__asm__("nop");
	outportb(0x378+0, c);
	outportb(0x378+2, 0x08|0x04|0x01);
	outportb(0x378+2, 0x08);
	#endif
}

/* Put the character C on the screen. */
void
putchar (int c)
{
	int attribute = ((c>>8)& 0xff);
	c = c & 0xff;

	lpt_putc(c);

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
	 *  to a point that will make it divisible by 8 */
	else if(c == '\t')
	{
		ttys[active_tty].xpos = (ttys[active_tty].xpos + 8) & ~(8 - 1);
	}
	/* Handles a 'Carriage Return', which simply brings the
	 *  cursor back to the margin */
	else if(c == '\r')
	{
		ttys[active_tty].xpos = 0;
	}
	/* We handle our newlines the way DOS and the BIOS do: we
	 *  treat it as if a 'CR' was also there, so we bring the
	 *  cursor to the margin and we increment the 'y' value */
	else if(c == '\n')
	{
		ttys[active_tty].xpos = 0;
		ttys[active_tty].ypos++;
	}
	/* Any character greater than and including a space, is a
	 *  printable character. The equation for finding the index
	 *  in a linear chunk of memory can be represented by:
	 *  Index = [(y * width) + x] */
	else if(c >= ' ')
	{
		where = ttys[active_tty].data + (ttys[active_tty].ypos * COLUMNS + ttys[active_tty].xpos);
		*where = c | attribute<<8;      /* Character AND attributes: color */
		ttys[active_tty].xpos++;
	}

	/* If the cursor has reached the edge of the tty0's width, we
	 *  insert a new line in there */
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

static void write(char c)
{
	lpt_putc(c);
	write_serial(c);
}

// parse ecma48 code (if existant) and return a VGA attribute value
unsigned char ECMA48(const char ** format)
{
	/*  3x      foreground
	 *  4x      background
	 *  followed by:
	 *  x0      black
	 *  x1      blue
	 *  x2      green
	 *  x3      brown
	 *  x4      red
	 *  x5      magenta (purple)
	 *  x6      cyan (light blue)
	 *  x7      gray
	 *
	 *  0       reset all attributes to their defaults
	 *  1       set bold
	 *  x5      set blink // x for not implemented
	 *  x7      set reverse screen
	 *  x22     set normal intensity
	 *  x25     blink off
	 *  x27     reverse screen off
	 */
	const char* fmt = *format;
	unsigned char attribute = 0x07;
	if(*fmt == '[')
	{
		write(*fmt++);
		while(*fmt != 'm')
		{
			switch(*fmt)
			{
			case '3':
				/*foreground color*/
				write(*fmt++);
				attribute &= 0xf0;
				attribute |= (*fmt-'0')&0xf;
				write(*fmt++);
				break;
			case '4':
				/*background color*/
				write(*fmt++);
				attribute &= 0x0f;
				attribute |= ((*fmt-'0')<<4)&0xf0;
				write(*fmt++);
				break;
			case '0':
				/*reset*/
				write(*fmt++);
				attribute = 0x07;
				break;
			case '1':
				/*bright(bold)*/
				write(*fmt++);
				attribute |= 0x08;
				break;
			case ';':
				write(*fmt++);
				break;
			default:
				*format = fmt;
				return 0;
			}
		}
		write(*fmt++);
	}
	*format = fmt;
	return attribute;
}

/* Clear the screen and initialize VIDEO, XPOS and YPOS. */
void
cls (void)
{
	memsetw(
		(unsigned short*)ttys[active_tty].data,
		(unsigned short)0x0700,
		COLUMNS*LINES);

	ttys[active_tty].xpos = 0;
	ttys[active_tty].ypos = 0;
	update_cursor();
	flush_current_tty();
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
