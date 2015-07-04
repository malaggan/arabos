#pragma once

#include <types.h>

void *memcpy(void *dest, const void *src, size_t count) __attribute__((nonnull));
void *memcpyw(void *dest, const void *src, size_t count) __attribute__((nonnull));
unsigned char *memset(unsigned char *dest, char val, size_t count) __attribute__((nonnull));
unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count) __attribute__((nonnull));
