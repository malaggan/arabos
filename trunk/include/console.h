#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef _cplusplus
extern "C" {
#endif

#include <sys.h>
#include <string.h>

/* Some screen stuff. */

/* The number of columns. */
#define COLUMNS	80

/* The number of lines. */
#define LINES	25

/* The attribute of an character. */
#define NORMAL	"\033[0m"
#define WHITE	"\033[37;1m"
#define GREENB	"\033[32;1m" /* B for bold*/
#define REDB	"\033[34;1m"
#define BLUEB	"\033[31;1m"

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
#define PREFILL(ch)	memsetw((void*)(video+ypos*COLUMNS),ch|ATTRIBUTE<<8,COLUMNS);
#define SHOW_STAT(strWhat,strStat,len)	\
	PREFILL('.');\
	printf(WHITE strWhat NORMAL);\
	xpos = COLUMNS - len - 2;\
	printf(LBRACK strStat RBRACK)
#define SHOW_STAT_OK(strWhat) SHOW_STAT(strWhat,STAT_OK("OK"),strlen("OK"))
#define SHOW_STAT_FAILED(strWhat) SHOW_STAT(strWhat,STAT_FAILED(/*"FAILED"*/"NOT IMPLEMENTED YET"),strlen("NOT IMPLEMENTED YET"))

#ifdef _cplusplus
}
#endif

#endif /*CONSOLE_H*/
