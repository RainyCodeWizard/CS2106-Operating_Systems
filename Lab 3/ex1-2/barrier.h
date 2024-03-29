/**
 * CS2106 AY 20/21 Semester 1 - Lab 3
 *
 * This file contains declarations. You should only modify the barrier_t struct,
 * as the method signatures will be needed to compile with the runner.
 */
#ifndef __CS2106_BARRIER_H_
#define __CS2106_BARRIER_H_

#include <semaphore.h>
typedef struct barrier {
  int count;
  // add additional fields here
  int arrived;
  sem_t mutex; //Binary semaphore to provide mutual exclusion
  sem_t turnstile1;
  sem_t turnstile2;
} barrier_t;

void barrier_init ( barrier_t *barrier, int count );
void barrier_wait ( barrier_t *barrier );
void barrier_destroy ( barrier_t *barrier );

#endif // __CS2106_BARRIER_H_
