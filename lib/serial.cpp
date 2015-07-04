#include <serial.h>
#include <port.h>

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
