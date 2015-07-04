#include <string.h>
#include <malloc.h>
#include <memops.h>
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
	else if (base == 'x' || base == 'p' /* pointer */)
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
