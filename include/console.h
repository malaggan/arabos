#pragma once

#include <lib.h>

#define VM_ECMA 0 // if we should print ECMA in the VM lpt output

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
void flush_video(struct tty_t* tty);
void flush_current_tty();
