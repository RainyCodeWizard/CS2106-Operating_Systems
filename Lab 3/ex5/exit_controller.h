/**
 * CS2106 AY 20/21 Semester 1 - Lab 3
 *
 * This file contains declarations. You should only modify the exit_controller_t struct,
 * as the method signatures will be needed to compile with the runner.
 */
#ifndef __CS2106_EXIT_CONTROLLER_H_
#define __CS2106_EXIT_CONTROLLER_H_

#include <semaphore.h>

#define MAX_PRIORITIES 5000 // we set a limit on how many possible priorities we have

typedef struct exit_controller {
    // define your variables here
    int exitEmpty;
    // int threads0; // To count the number of threads waiting
    // int threads1; // for priority 0 & 1
    int prioritiesNo;
    int threads[5000];
    sem_t locks[5000];
    sem_t mutex;
    // sem_t lock1; // To lock priority 1
    // sem_t lock0; // To lock priority 0

} exit_controller_t;

void exit_controller_init(exit_controller_t *exit_controller, int no_of_priorities);
void exit_controller_wait(exit_controller_t *exit_controller, int priority);
void exit_controller_post(exit_controller_t *exit_controller, int priority);
void exit_controller_destroy(exit_controller_t *exit_controller);


#endif // __CS2106_EXIT_CONTROLLER_H_
