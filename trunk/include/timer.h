#ifndef TIMER_H
#define TIMER_H

#ifdef _cplusplus
extern "C" {
#endif

extern volatile unsigned int timer_ticks;

// 100 hz = 10 ms, so 20 hz is 50ms, and 10 hz = 100ms, 50 hz = 20ms , 200 hz = 5ms
#define TICKS_PER_SEC 200
// 1000ms / desired_ms = freq

void timer_phase(int hz);
void timer_install();
void timer_wait(int ticks);

#ifdef _cplusplus
}
#endif

#endif //TIMER_H
