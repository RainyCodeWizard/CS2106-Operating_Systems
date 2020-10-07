/**
 * CS2106 AY 20/21 Semester 1 - Lab 3
 *
 * Your implementation should go in this file.
 */
#include "fizzbuzz_workers.h"
#include "barrier.h" // you may use barriers if you think it can help your
                     // implementation

// declare variables to be used here
#define ensure_successful_malloc(ptr)                           \
  if (ptr == NULL) {                                            \
    perror("Memory allocation unsuccessful for" #ptr "\n");     \
    exit(1);                                                    \
  }

barrier_t *barrier = malloc( sizeof(barrier_t) );
ensure_successful_malloc(barrier);

void fizzbuzz_init ( int n ) {
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
