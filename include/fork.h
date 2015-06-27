/*
 * File:   fork.h
 * Author: meemo
 *
 * Created on April 25, 2009, 1:15 AM
 */

#ifndef _FORK_H
#define	_FORK_H

#include <idt.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*syscall*/
	IRQ(16)
	int fork_handler(struct interrupt_frame *r);
	IRQ(17)
	int spawn_handler(struct interrupt_frame *r);

	int fork();
	int spawn(void (*proc)(void));


#ifdef	__cplusplus
}
#endif

#endif	/* _FORK_H */
