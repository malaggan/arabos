#pragma once

#include <idt.h>

/*syscall*/
IRQ(16)
int fork_handler(struct interrupt_frame *r);
IRQ(17)
int spawn_handler(struct interrupt_frame *r);

int fork() __attribute__((returns_twice));
int spawn(void (*proc)(void));
