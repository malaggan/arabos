#pragma once

extern "C" // because called from assembly
void printf (const char *format, ...) __attribute__((format(__printf__,1,2)));
