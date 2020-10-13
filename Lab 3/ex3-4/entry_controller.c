/**
 * CS2106 AY 20/21 Semester 1 - Lab 3
 *
 * Your implementation should go in this file.
 */
#include "entry_controller.h"

void entry_controller_init( entry_controller_t *entry_controller, int loading_bays ) {
    // entry_controller->waitingBay = loading_bays;
    sem_init(&entry_controller->mutex,0,1);
    int i;
    for (i = 0; i < ENTRY_CONTROLLER_MAX_USES; i++){
        if (i < loading_bays) {
            sem_init(&entry_controller->waitingQueue[i],0,1);
        }
        else sem_init(&entry_controller->waitingQueue[i],0,0);
    }
    entry_controller->queueStart = loading_bays % ENTRY_CONTROLLER_MAX_USES;
    entry_controller->queueEnd = 0;
}

void entry_controller_wait( entry_controller_t *entry_controller ) {
    
    sem_wait(&entry_controller->mutex);
    int prev = entry_controller->queueEnd;
    entry_controller->queueEnd = (entry_controller->queueEnd + 1) % ENTRY_CONTROLLER_MAX_USES;
    sem_post(&entry_controller->mutex);
    
    sem_wait(&entry_controller->waitingQueue[prev]);    
}

void entry_controller_post( entry_controller_t *entry_controller ) {
    sem_wait(&entry_controller->mutex);
    sem_post(&entry_controller->waitingQueue[entry_controller->queueStart]);
    entry_controller->queueStart = (entry_controller->queueStart + 1) % ENTRY_CONTROLLER_MAX_USES;
    sem_post(&entry_controller->mutex);
}

void entry_controller_destroy( entry_controller_t *entry_controller ) {
    int i;
    for (i = 0; i < ENTRY_CONTROLLER_MAX_USES; i++){
        sem_destroy(&entry_controller->waitingQueue[i]);
    }
    sem_destroy(&entry_controller->mutex);
}

