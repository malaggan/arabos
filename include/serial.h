#pragma once

#include <types.h>

constexpr uint16_t COM1_PORT = 0x3f8;   /* COM1 */
void init_serial(); // COM1 serial port
void write_serial(char);
