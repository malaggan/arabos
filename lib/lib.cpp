#include <asm.h>
#include <lib.h>
#include <liballoc.h> // for malloc
inline void *memcpy(void *dest, const void *src, size_t count)
{
	ASM("cld\n"
	    "rep\n"
	    "movsb"
	    ::
	     "S"(reinterpret_cast<const uint8_t*>(src)),
	     "D"(reinterpret_cast<uint8_t*>(dest)),
	     "c"(count));
	return dest;
}

void *memcpyw(void *dest, const void *src, size_t count)
{
	ASM("cld\n"
	    "rep\n"
	    "movsw"
	    ::
	     "S"(reinterpret_cast<const uint16_t*>(src)),
	     "D"(reinterpret_cast<uint16_t*>(dest)),
	     "c"(count));
	return dest;
}

unsigned char *memset(unsigned char *dest, char val, size_t count)
{
	ASM("cld\n"
	    "rep\n"
	    "stosb" :: "a"(val), "D"(dest), "c"(count));
	return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
	ASM("cld\n"
	    "rep\n"
	    "stosw" :: "a"(val), "D"(dest), "c"(count));
	return dest;
}

unsigned char inportb (unsigned short _port)
{
	unsigned char retval;
	ASM (IO_WAIT "inb %1, %0" : "=a" (retval) : "d" (_port));
	return retval;
}

void outportb (unsigned short _port, char _data)
{
	ASM ("outb %1, %0\n" IO_WAIT : : "d" (_port), "a" (_data));
}

unsigned short inportw (unsigned short _port)
{
	unsigned char retval;
	ASM (IO_WAIT "insw" :: "D" (&retval), "d" (_port) : "memory");
	return retval;
}

void outportw (unsigned short _port, short _data)
{
	ASM ("outw %1, %0\n" IO_WAIT :: "d" (_port), "a" (_data));
}

/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal. */
void
itoa (char *buf, int base, int d)
{
	char *p = buf;
	char *p1, *p2;
	unsigned long ud = d;
	int divisor = 10;

	/* If %d is specified and D is minus, put `-' in the head. */
	if ((base == 'i' || base == 'd') && d < 0)
	{
		*p++ = '-';
		buf++;
		ud = -d;
	}
	else if (base == 'x')
		divisor = 16;

	/* Divide UD by DIVISOR until UD == 0. */
	do
	{
		int remainder = ud % divisor;

		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
	}
	while (ud /= divisor);

	/* Terminate BUF. */
	*p = 0;

	/* Reverse BUF. */
	p1 = buf;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

int strnlen(const char *str, int max)
{
	int c = 0;

	while(*str++ && c < max) c++;

	return c;
}

/*
  inline int strcmp(const char *s1,const char* s2)
  {
  if(s1 == s2)
  return(0);
  while(*s1 == *s2++)
  if(*s1++ == '\0')
  return(0);
  return(*s1 - s2[-1]);
  }
*/

int strncmp(const char *s1,const char* s2, int len)
{
	if(s1 == s2)
		return(0);
	while(*s1 == *s2++ && len--)
		if(*s1++ == '\0')
			return(0);
	return(*s1 - s2[-1]);
}

void init_serial() {
	outportb(COM1_PORT + 1, 0x00);    // Disable all interrupts
	outportb(COM1_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	outportb(COM1_PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	outportb(COM1_PORT + 1, 0x00);    //                  (hi byte)
	outportb(COM1_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
	outportb(COM1_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	outportb(COM1_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int is_transmit_empty() {
	return inportb(COM1_PORT + 5) & 0x20;
}

void write_serial(char c) {
	while (is_transmit_empty() == 0);

	outportb(COM1_PORT,c);
}

char * strdup (const char *s) { // TODO: use shared_ptr, or guarantee there's is no leak
	size_t len = strnlen (s, 10000) + 1;

	void *_new = malloc (len);

	if (_new == nullptr)
		return nullptr;

	return (char *) memcpy (_new, s, len);
}

char * strndup (const char *s, size_t at_most) {
	size_t len = strnlen (s, at_most) + 1;

	if(len > at_most)
		len = at_most;
	void *_new = malloc (len);

	if (_new == nullptr)
		return nullptr;

	return (char *) memcpy (_new, s, len);
}


/*
  void abort(int exit_code)
  {

  }

  void __assert(int s)
  {

  }
*/
