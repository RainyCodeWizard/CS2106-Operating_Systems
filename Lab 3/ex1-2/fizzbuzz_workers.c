/**
 * CS2106 AY 20/21 Semester 1 - Lab 3
 *
 * Your implementation should go in this file.
 */
#include "fizzbuzz_workers.h"
#include "barrier.h" // you may use barriers if you think it can help your
                     // implementation
#include "stdlib.h"
#include "semaphore.h"
// declare variables to be used here

barrier_t *barrier = NULL;


void fizzbuzz_init ( int n ) {
    barrier = malloc(sizeof(barrier_t));
    barrier_init( barrier, 4 ); // There are only 4 threads
}

void num_thread( int n, void (*print_num)(int) ) {
    int i;
    for(i = 1; i<=n; i++){
        (*print_num)(i);
        barrier_wait(barrier);
    }
}

void fizz_thread( int n, void (*print_fizz)(void) ) {
    int i;
    for(i = 1; i<=n; i++){
        if(i % 3 == 0){
            (*print_fizz)();
        }
        barrier_wait(barrier);
    }
}

void buzz_thread( int n, void (*print_buzz)(void) ) {
    int i;
    for(i = 1; i<=n; i++){
        if(i % 5 == 0){
            (*print_buzz)();
        }
        barrier_wait(barrier);
    }
}

void fizzbuzz_thread( int n, void (*print_fizzbuzz)(void) ) {
    int i;
    for(i = 1; i<=n; i++){
        if(i % 3 == 0 && i % 5 == 0){
            (*print_fizzbuzz)();
        }
        barrier_wait(barrier);
    }
}

void fizzbuzz_destroy() {
    barrier_destroy(barrier);
    free(barrier);
}
