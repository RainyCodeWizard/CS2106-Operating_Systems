/**
 * CS2106 AY 20/21 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include "sm.h"
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>

// Use this function to any initialisation if you need to.
void sm_init(void) {
}

// Use this function to do any cleanup of resources.
void sm_free(void) {
}

// Exercise 1a/2: start services
void sm_start(const char *processes[]) {
    int childPid = fork();
    //printf("%s",processes[0]);
    if(childPid == 0){
        execv(processes[0],processes);
    }
    //else{
    //    wait(NULL);
    //}
    //
    //int i = 0;
    //for (i = 0;i<2;i++){
    //	printf("%s\n",processes[i]);
    //}
    //execv(processes[0], processes);
    return;
}

// Exercise 1b: print service status
size_t sm_status(sm_status_t statuses[]) {
}

// Exercise 3: stop service, wait on service, and shutdown
void sm_stop(size_t index) {
}

void sm_wait(size_t index) {
}

void sm_shutdown(void) {
}

// Exercise 4: start with output redirection
void sm_startlog(const char *processes[]) {
}

// Exercise 5: show log file
void sm_showlog(size_t index) {
}
