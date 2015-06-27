#pragma once


typedef int semaphore_t;
void wait(volatile semaphore_t* sem);
void signal(volatile semaphore_t* sem);
