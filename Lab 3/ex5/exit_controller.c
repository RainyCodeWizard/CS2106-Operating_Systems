/**
 * CS2106 AY 20/21 Semester 1 - Lab 3
 *
 * This file contains declarations. You should only modify the fifo_sem_t struct,
 * as the method signatures will be needed to compile with the runner.
 */
#include "exit_controller.h"

void exit_controller_init(exit_controller_t *exit_controller, int no_of_priorities) {
    exit_controller->exitEmpty = 1;
    exit_controller->prioritiesNo = no_of_priorities;
    sem_init(&exit_controller->mutex,0,1);
    int i;
    for(i=0; i<no_of_priorities; i++){
        exit_controller->threads[i] = 0;
        sem_init(&exit_controller->locks[i],0,0);
    }
}

void exit_controller_wait(exit_controller_t *exit_controller, int priority) {
    sem_wait(&exit_controller->mutex);
    if(exit_controller->exitEmpty){
        exit_controller->exitEmpty = 0;
        sem_post(&exit_controller->mutex);
    }
    else{
        exit_controller->threads[priority]++;
        sem_post(&exit_controller->mutex);
        sem_wait(&exit_controller->locks[priority]);
    }
}

void exit_controller_post(exit_controller_t *exit_controller, int priority) {
    sem_wait(&exit_controller->mutex);
    int i, flag = 1;
    for(i = 0; i < exit_controller->prioritiesNo ; i++){
        if (exit_controller->threads[i] != 0){
            exit_controller->threads[i]--;
            sem_post(&exit_controller->locks[i]);
            flag = 0;
            break;
        }
    }
    if (flag){
        exit_controller->exitEmpty = 1;
    }
    sem_post(&exit_controller->mutex);
}

void exit_controller_destroy(exit_controller_t *exit_controller){
    sem_destroy(&exit_controller->mutex);
    int i;
    for(i = 0; i < exit_controller->prioritiesNo; i++){
        sem_destroy(&exit_controller->locks[i]);
    }
}
