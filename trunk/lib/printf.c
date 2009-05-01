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

#include <ecma48.h>
#include <console.h> // needed by panic()
#include <lib.h>
#include <timer.h> // needed by panic()
#include <asm.h>

// imbed attribute in char, given c is an int
int MK_CH_ATT(unsigned char c,unsigned char a)
{
    return c|(a<<8);
}

int printed_chars = 0; // guard for MAX_CHARS // TODO non-re-entrant !!

void print0 (char **format0, int ignore_first_char);

/* Format a string and print it on the screen, just like the libc
function printf. */
void
printf (const char *format, ...)
{
    print0((char **)&format,0);
}

// first character is a printk level code 
void
printk (const char *format, ...)
{
    //block_timer();
    char level = format[0];
    
    if(level < *TRACE || level > *SEVERE) // no printk level specified
        print0((char**)&format,0);    
    else if(level >= *PRINTK_LEVEL)
    {
        if(*TRACE == level)                
            printf(ECMA_PREFIX ECMA_BACK_GND ECMA_WHITE ECMA_SUFFIX 
                    "TRC: ");
        else if(*DEBUG == level)
            printf(ECMA_PREFIX ECMA_BACK_GND ECMA_GREEN ECMA_SUFFIX 
                    "DBG: ");
        else if(*LOG == level)                
            printf(ECMA_PREFIX ECMA_BACK_GND ECMA_GREEN ECMA_SEPARATOR ECMA_SET_BOLD ECMA_SUFFIX 
                    "LOG: ");
        else if(*WARNING == level)
            printf(ECMA_PREFIX ECMA_BACK_GND ECMA_MAGENTA ECMA_SUFFIX 
                    "WRN: ");
        else if(*ERROR == level)                
            printf(ECMA_PREFIX ECMA_BACK_GND ECMA_RED ECMA_SUFFIX 
                    "ERR: ");
        else if(*SEVERE == level)                
            printf(ECMA_PREFIX ECMA_BACK_GND ECMA_RED ECMA_SEPARATOR ECMA_SET_BOLD ECMA_SUFFIX 
                    "SVR: ");
        
        print0((char**)&format, 1);
        printf(NORMAL);
    }
    //unmask_timer();
}

// a non-variable arg list version
void print0 (char **format0, int ignore_first_char)
{
    const char * format = (const char *)*format0;
    if(ignore_first_char)
        format++;
    
    char **arg = format0;
    int c;
    char buf[20];

    printed_chars = 0;

    arg++;
    unsigned char attribute = 0x07;
    while (printed_chars < MAX_CHARS && (c = *format++) != 0)
    {
        if (c != '%' && c != 0x1B)
        {
            putchar (MK_CH_ATT(c,attribute));
        }
        /* meemo : ECMA-48 */
        else if(c == 0x1B)
        {	
#if VM_ECMA
            void lpt_putc(int c);
            lpt_putc(c); // puts the ESC character only, cuz
            // ECMA48 does not process it
#endif
            attribute = ECMA48(&(format));
        }
        /* meemo : ECMA-48 end*/
        else // % specifier
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
            case '%':
                putchar (MK_CH_ATT('%',attribute));
                break;
            default:
                putchar (MK_CH_ATT(*((int *) arg++),attribute));
                break;
            }
            if(print)
            {
                while (*p)
                    putchar (MK_CH_ATT(*p++,attribute));
            }
        }
    }

    flush_current_tty();
}

__attribute__((noreturn)) void panic (const char *message) 
{
	printf("\n" NORMAL	"-------------\n");
	printf(REDB 		"KERNEL PANIC: " BLUEB "\n\"");
	printf(			message);
	printf("\"" NORMAL	"\nSYSTEM HALTED (tick %d; %d ms after timer start)\n"
				"-------------",timer_ticks,timer_ticks*(1000/TICKS_PER_SEC));
	hlt();
}
