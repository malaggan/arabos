#pragma once

#include "idt.h"



struct ProcessData
{
	struct interrupt_frame frame;
	int valid;
};

extern int currentProcess;
#define MAX_PROCESSES 8
extern struct ProcessData processes[MAX_PROCESSES];

extern volatile unsigned int timer_ticks;
extern volatile unsigned int scheduling_started;
int schedule(struct interrupt_frame *r);

// 100 hz = 10 ms, so 20 hz is 50ms, and 10 hz = 100ms, 50 hz = 20ms , 200 hz = 5ms
#define TICKS_PER_SEC 2000
// 1000ms / desired_ms = freq

void timer_phase(int hz);
void timer_install();
void timer_wait(int ticks);
