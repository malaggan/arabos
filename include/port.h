#pragma once

unsigned char inportb (unsigned short _port);
void outportb (unsigned short _port, char _data);
unsigned short inportw (unsigned short _port);
void outportw (unsigned short _port, short _data);

#define IO_WAIT "nop\n nop\n nop\n nop\n"
