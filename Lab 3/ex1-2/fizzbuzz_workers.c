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

int check(int n, int thread); // To check if the function needs to print

barrier_t *barrier = NULL;


void fizzbuzz_init ( int n ) {
    barrier = malloc(sizeof(barrier_t));
    barrier_init( barrier, 4 ); // There are only 4 threads
}
//Thread 1
void num_thread( int n, void (*print_num)(int) ) {
    int i;
    for(i = 1; i<=n; i++){
        if(check(i, 1)){
            (*print_num)(i);
        }
        barrier_wait(barrier);
    }
}
//Thread 2
void fizz_thread( int n, void (*print_fizz)(void) ) {
    int i;
    for(i = 1; i<=n; i++){
        if(check(i, 2)){
            (*print_fizz)();
        }
        barrier_wait(barrier);
    }
}
//Thread 3
void buzz_thread( int n, void (*print_buzz)(void) ) {
    int i;
    for(i = 1; i<=n; i++){
        if(check(i, 3)){
            (*print_buzz)();
        }
        barrier_wait(barrier);
    }
}
//Thread 4
void fizzbuzz_thread( int n, void (*print_fizzbuzz)(void) ) {
    int i;
    for(i = 1; i<=n; i++){
        if(check(i, 4)){
            (*print_fizzbuzz)();
        }
        barrier_wait(barrier);
    }
}

void fizzbuzz_destroy() {
    barrier_destroy(barrier);
    free(barrier);
}

// int thread refers to their respective threads
int check(int n, int thread){
    if (n % 3 == 0 && n % 5 == 0){
        if (thread == 4) return 1;
	return 0;
    }
    if (n % 5 == 0){
        if (thread == 3) return 1;
	return 0;
    }
    if (n % 3 == 0){
        if (thread == 2) return 1;
	return 0;
    }
    if (thread ==  1) return 1;
    return 0;
}
