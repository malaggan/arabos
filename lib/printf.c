#include <console.h> // needed by panic()
#include <printf.h>
#include <timer.h> // needed by panic()

int printed_chars = 0; // guard for MAX_CHARS

/* Format a string and print it on the screen, just like the libc
function printf. */
void
printf (const char *format, ...)
{
	char **arg = (char **) &format;
	int c;
	char buf[20];

	printed_chars = 0;

	arg++;

	while (printed_chars < MAX_CHARS && (c = *format++) != 0)
	{
		if (c != '%' && c != 0x1B)
		{
			putchar (c);
		}
		/* meemo : ECMA-48 */
		else if(c == 0x1B)
		{
			ECMA48(&(format));
		}
		/* meemo : ECMA-48 end*/
		else
		{
			char *p;
			int print = 0;

			c = *format++;
			switch (c)
			{
			case 'd':
			case 'u':
			case 'x':
				itoa (buf, c, *((int *) arg++));
				p = buf;
				print = 1;
				break;
			case 's':
				p = *arg++;
				if (! p)
					p = "(null)";
				print = 1;
				break;
			default:
				putchar (*((int *) arg++));
				break;
			}
			if(print)
			{
				while (*p)
					putchar (*p++);
			}
		}
	}
}

__attribute__((noreturn)) void panic (const char *message) 
{
	printf("\n" NORMAL	"-------------\n");
	printf(REDB 		"KERNEL PANIC: " BLUEB "\n\"");
	printf(			message);
	printf("\"" NORMAL	"\nSYSTEM HALTED (tick %d; %d ms after timer start)\n"
				"-------------",timer_ticks,timer_ticks*(1000/TICKS_PER_SEC));
	ASM ( "hlt" );
	while(1);
}
