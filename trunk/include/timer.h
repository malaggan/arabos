/* Copyright (C) 2007  Mohammad Nabil 
mohammad (dot) nabil (dot) h (at) gmail (dot) com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.*/

#ifndef TIMER_H
#define TIMER_H

#include "idt.h"


#ifdef __cplusplus
extern "C" {
#endif

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
#define TICKS_PER_SEC 200
// 1000ms / desired_ms = freq

void timer_phase(int hz);
void timer_install();
void timer_wait(int ticks);

#ifdef __cplusplus
}
#endif

#endif //TIMER_H
