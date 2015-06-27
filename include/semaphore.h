/*
 * File:   semaphore.h
 * Author: meemo
 *
 * Created on May 9, 2009, 11:27 PM
 */

#ifndef _SEMAPHORE_H
#define	_SEMAPHORE_H

#ifdef	__cplusplus
extern "C" {
#endif

	typedef int semaphore_t;
	void wait(volatile semaphore_t* sem);
	void signal(volatile semaphore_t* sem);

#ifdef	__cplusplus
}
#endif

#endif	/* _SEMAPHORE_H */
