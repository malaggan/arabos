#pragma once

// printk levels
#define TRACE   "\310" // = 200 in decimal
#define DEBUG   "\311"
#define LOG     "\312"
#define WARNING "\313"
#define ERROR   "\314"
#define SEVERE  "\315"
#define LOG_ALL TRACE
// current printk level
#define PRINTK_LEVEL LOG_ALL

void printk (const char *format, ...)  __attribute__((format(__printf__,1,2)));
