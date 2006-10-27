#ifndef PRINTF_H
#define PRINTF_H

#ifdef _cplusplus
extern "C" {
#endif

#include <sys.h>
#include <console.h>
#include <string.h>

void printf (const char *format, ...);
void panic (const char *format);

#ifdef _cplusplus
}
#endif

#endif /*PRINTF_H*/
