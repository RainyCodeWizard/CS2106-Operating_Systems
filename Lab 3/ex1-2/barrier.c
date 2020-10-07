/**
 * CS2106 AY 20/21 Semester 1 - Lab 3
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include "barrier.h"

// Initialise barrier here
void barrier_init ( barrier_t *barrier, int count ) {
    barrier->count = count;
    barrier->arrived = 0;
    
    sem_init(&barrier->mutex,0,1);
    // sem_init(&barrier->waitQ,0,0);
    sem_init(&barrier->turnstile1,0,0);
    sem_init(&barrier->turnstile2,0,1);
}

void barrier_wait ( barrier_t *barrier ) {
    sem_wait(&barrier->mutex);
    barrier->arrived++;
    if(barrier->arrived == barrier->count){
        sem_wait(&barrier->turnstile2);
        sem_post(&barrier->turnstile1);
    }
    sem_post(&barrier->mutex);
    
    sem_wait(&barrier->turnstile1);
    sem_post(&barrier->turnstile1);

    sem_wait(&barrier->mutex);
    barrier->arrived--;
    if(barrier->arrived == 0){
        sem_wait(&barrier->turnstile1);
        sem_post(&barrier->turnstile2);
    }
    sem_post(&barrier->mutex);

    sem_wait(&barrier->turnstile2);
    sem_post(&barrier->turnstile2);
}

// Perform cleanup here if you need to
void barrier_destroy ( barrier_t *barrier ) {
    sem_destroy(&barrier->mutex);
    // sem_destroy(&barrier->waitQ);
    sem_destroy(&barrier->turnstile1);
    sem_destroy(&barrier->turnstile2);
}
