/**
 * CS2106 AY 20/21 Semester 1 - Lab 3
 *
 * This file contains declarations. You should only modify the fifo_sem_t struct,
 * as the method signatures will be needed to compile with the runner.
 */
#include "exit_controller.h"

void exit_controller_init(exit_controller_t *exit_controller, int no_of_priorities) {
    exit_controller->exitEmpty = 1;
    exit_controller->threads0 = 0;
    exit_controller->threads1 = 0;
    sem_init(&exit_controller->mutex,0,1);
    sem_init(&exit_controller->lock0,0,0);
    sem_init(&exit_controller->lock1,0,0);
}

void exit_controller_wait(exit_controller_t *exit_controller, int priority) {
    sem_wait(&exit_controller->mutex);
    if(exit_controller->exitEmpty){
        exit_controller->exitEmpty = 0;
        sem_post(&exit_controller->mutex);
    }
    else if(priority==0){
        exit_controller->threads0++;
        sem_post(&exit_controller->mutex);
        sem_wait(&exit_controller->lock0);
    }
    else{
        exit_controller->threads1++;
        sem_post(&exit_controller->mutex);
        sem_wait(&exit_controller->lock1);
    }
}

void exit_controller_post(exit_controller_t *exit_controller, int priority) {
    sem_wait(&exit_controller->mutex);
    if(exit_controller->threads0 != 0 ){
        exit_controller->threads0--;
        sem_post(&exit_controller->lock0);
    }
    else if (exit_controller->threads1 != 0){
        exit_controller->threads1--;
        sem_post(&exit_controller->lock1);
    }
    else{
        exit_controller->exitEmpty = 1;
    }
    sem_post(&exit_controller->mutex);
}

void exit_controller_destroy(exit_controller_t *exit_controller){
    sem_destroy(&exit_controller->mutex);
    sem_destroy(&exit_controller->lock0);
    sem_destroy(&exit_controller->lock1);
}
