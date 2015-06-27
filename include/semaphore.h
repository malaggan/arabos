#pragma once

#ifdef	__cplusplus
extern "C" {
#endif

	typedef int semaphore_t;
	void wait(volatile semaphore_t* sem);
	void signal(volatile semaphore_t* sem);

#ifdef	__cplusplus
}
#endif
