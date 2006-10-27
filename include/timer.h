#ifndef TIMER_H
#define TIMER_H

#ifdef _cplusplus
extern "C" {
#endif

extern volatile unsigned int timer_ticks;

#define TICKS_PER_SEC 10

void timer_phase(int hz);
void timer_install();
void timer_wait(int ticks);

#ifdef _cplusplus
}
#endif

#endif //TIMER_H
