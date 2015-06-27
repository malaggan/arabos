#pragma once

#include <types.h>

#ifdef	__cplusplus
extern "C" {
#endif

	unsigned char *memcpy(unsigned char *dest, const unsigned char *src, size_t count);
	unsigned short *memcpyw(unsigned short *dest, const unsigned short *src, size_t count);
	unsigned char *memset(unsigned char *dest, char val, size_t count);
	unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);

	void itoa (char *buf, int base, int d); // int to ascii
	int strnlen(const char *str, int max);
	int strncmp(const char *s1,const char* s2, int max);

#define COM1_PORT 0x3f8   /* COM1 */
	void init_serial(); // COM1 serial port
	void write_serial(char);

#define MAX_CHARS 1024

	void printf (const char *format, ...);
	void printk (const char *format, ...);
	void panic (const char *format);

	unsigned char inportb (unsigned short _port);
	void outportb (unsigned short _port, char _data);
	unsigned short inportw (unsigned short _port);
	void outportw (unsigned short _port, short _data);

// printk levels
#define TRACE   "\310" // = 200 in decimal
#define DEBUG   "\311"
#define LOG     "\312"
#define WARNING "\313"
#define ERROR   "\314"
#define SEVERE  "\315"
#define LOG_ALL TRACE

#define assert(cond) if(!(cond)){printk(ERROR "assertion failure %s:%d: \"" #cond "\"",__FILE__,__LINE__);}

/* Return a new identical string.  */
	char * strdup (const char *str);
	char * strndup (const char *str, size_t at_most);

#define IO_WAIT "nop\n nop\n nop\n nop\n"

// current printk level
#define PRINTK_LEVEL LOG_ALL

#ifdef	__cplusplus
}
#endif
