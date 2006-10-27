#include <console.h>
#include <mem.h>

/* Variables. */
/* Save the X position. */
int xpos = 0;

/* Save the Y position. */
int ypos = 0;

/* Point to the video memory. */
volatile unsigned short *video = NULL;

char ATTRIBUTE	=	0x07; /* COOL this value determines the color */

void scroll(void)
{

	unsigned short blank;

	if(ypos >= LINES)
	{
		/* A blank is defined as a space... we need to give it
		*	backcolor too */
		blank = 0x0720; /* 07 black, 20 space*/

		/* Row 25 is the end, this means we need to scroll up */
		/* Move the current text chunk that makes up the screen
		*	back in the buffer by a line */
		memcpy ((void*)video, (void*)(video + COLUMNS),COLUMNS*2*(LINES-1));

		/* Finally, we set the chunk of memory that occupies
		*	the last line of text to our 'blank' character */
		memsetw ((unsigned short*)(video + ((LINES-1) * COLUMNS)), blank, COLUMNS);
		ypos = LINES - 1;
	}
}

/* Put the character C on the screen. */
void
putchar (unsigned char c)
{
	printed_chars++;
	volatile unsigned short *where;
	/* Handle a backspace, by moving the cursor back one space */
	if(c == '\b')
	{
		if(xpos != 0) xpos--;
	}
	/* Handles a tab by incrementing the cursor's x, but only
	*	to a point that will make it divisible by 8 */
	else if(c == '\t')
	{
		xpos = (xpos + 8) & ~(8 - 1);
	}
	/* Handles a 'Carriage Return', which simply brings the
	*	cursor back to the margin */
	else if(c == '\r')
	{
		xpos = 0;
	}
	/* We handle our newlines the way DOS and the BIOS do: we
	*	treat it as if a 'CR' was also there, so we bring the
	*	cursor to the margin and we increment the 'y' value */
	else if(c == '\n')
	{
		xpos = 0;
		ypos++;
	}
	/* Any character greater than and including a space, is a
	*	printable character. The equation for finding the index
	*	in a linear chunk of memory can be represented by:
	*	Index = [(y * width) + x] */
	else if(c >= ' ')
	{
		where = video + (ypos * COLUMNS + xpos);
		*where = c | ATTRIBUTE<<8;	/* Character AND attributes: color */
		xpos++;
	}

	/* If the cursor has reached the edge of the screen's width, we
	*	insert a new line in there */
	if(xpos >= COLUMNS)
	{
		xpos = 0;
		ypos++;
	}

	/* Scroll the screen if needed, and finally move the cursor */
	scroll();
	update_cursor();

}

void ECMA48(const char ** format)
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
	*	x7	set reverse video
	*	x22	set normal intensity
	*	x25	blink off
	*	x27	reverse video off
	*/
	const char* fmt = *format;
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
					ATTRIBUTE &= 0xf0;
					ATTRIBUTE |= (*fmt-'0')&0xf;
					fmt++;
					break;
				}
			case '4':
				{
					/*background color*/
					fmt++;
					ATTRIBUTE &= 0x0f;
					ATTRIBUTE |= ((*fmt-'0')<<4)&0xf0;
					fmt++;
					break;
				}
			case '0':
				{
					/*reset*/
					fmt++;
					ATTRIBUTE = 0x07;
					break;
				}
			case '1':
				{
					/*bright(bold)*/
					fmt++;
					ATTRIBUTE |= 0x08;
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
					return;
				}
			}
		}
		fmt++;
	}
	*format = fmt;
}



/* Clear the screen and initialize VIDEO, XPOS and YPOS. */
void
cls (void)
{
	memsetw((unsigned short*)video,(unsigned short)0x0700,COLUMNS*LINES);

	xpos = 0;
	ypos = 0;
	update_cursor();
}

void update_cursor(void)
{
	/*doesn't work ??*/
	unsigned temp;

	/* The equation for finding the index in a linear
	*	chunk of memory can be represented by:
	*	Index = [(y * width) + x] */
	temp = (ypos * COLUMNS + xpos);

	/* This sends a command to indicies 14 and 15 in the
	*	CRT Control Register of the VGA controller. These
	*	are the high and low bytes of the index that show
	*	where the hardware cursor is to be 'blinking'. To
	*	learn more, you should look up some VGA specific
	*	programming documents. A great start to graphics:
	*	http://www.brackeen.com/home/vga */	 	
	
	outportb (0x3D4, 0x0F); outportb (0x3D5, temp & 0xFF);
	temp >>= 8;
	outportb(0x3D4, 0xE); outportb(0x3D5, temp & 0xFF);
}

void init_video(void)
{
	video = (unsigned short *)VIDEO;
	xpos = 0;
	ypos = 0;
	update_cursor();
	cls();
}
