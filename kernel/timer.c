#include <timer.h>
#include <idt.h>
#include <sys.h>
#include <printf.h>

/* This will keep track of how many ticks that the system
*	has been running for */
volatile unsigned int timer_ticks = 0;

/* Handles the timer. In this case, it's very simple: We
*	increment the 'timer_ticks' variable every time the
*	timer fires. By default, the timer fires 18.222 times
*	per second. Why 18.222Hz? Some engineer at IBM must've
*	been smoking something funky */
void timer_handler(struct interrupt_frame *r)
{
	/* Increment our 'tick count' */
	timer_ticks++;

	/* Every 100 clocks (approximately 1 second), we will
	*	display a message on the screen */
	if (timer_ticks % TICKS_PER_SEC == 0)
	{
		//printf("One second has passed\n");
	}
}

/* Sets up the system clock by installing the timer handler
*	into IRQ0 */
void timer_install()
{
	/* Installs 'timer_handler' to IRQ0 */
	timer_phase(TICKS_PER_SEC/*Hz*/); // PIT
	irq_install_custom_handler(0, timer_handler);
}

void timer_phase(int hz)
{
	int divisor = 1193180 / hz;	/* Calculate our divisor */
	outportb((unsigned short)0x43, (char)0x36);	/* Set our command byte 0x36 */
	outportb((unsigned short)0x40, (char)(divisor & 0xFF));	/* Set low byte of divisor */
	outportb((unsigned short)0x40, (char)(divisor >> 8));	/* Set high byte of divisor */
}

/* This will continuously loop until the given time has
*	been reached */
void timer_wait(int ticks)
{
	unsigned long eticks;

	eticks = timer_ticks + ticks;
	while(timer_ticks < eticks);
}
