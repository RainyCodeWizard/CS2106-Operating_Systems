/**
 * CS2106 AY 20/21 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include "sm.h"
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h> //Delete this later
//Need to free malloc
int getIndex(const char *processes[], int count);
void execStart(int pipe[], const char *process[]);
void execMiddle(int pipe1[], int pipe2[], const char *process[]);
void execEnd(int pipe[], const char *process[]);

sm_status_t processes_array[SM_MAX_SERVICES]; // To take note of the informations of processes
int processes_count = 0; // To take note of the amount of processes currently

// Use this function to any initialisation if you need to.
void sm_init(void) {
}

// Use this function to do any cleanup of resources.
void sm_free(void) {
}

// Exercise 1a/2: start services
void sm_start(const char *processes[]) {
    int i = 0, count = 0; //count represents the number of processes
    while(1){
        if (!processes[i]){ //if NULL
            if(i > 0 && !processes[i-1]) break;
            count++;
        }
        i++;
    }
    sm_status_t* arr = (sm_status_t*)malloc(count*sizeof(sm_status_t)); //Creates an array to store child processors
    pid_t childPid = fork();
    arr[0].pid = childPid;
    if (childPid == 0){
        if (count==1) execv(processes[0],processes);
        else {
            int pipeFd[count - 1][2]; // File descriptors for each process.  
            for(i = 0; i<count-1; i++){
                pipe(pipeFd[i]);
                arr[i].pid = fork();
                if (!arr[i].pid){
                    if(i == 0){
                        execStart(pipeFd[0], processes);
                    }
                    else{
                        execMiddle(pipeFd[i-1], pipeFd[i], &processes[getIndex(processes,i)]);
                    }
                }                
            }
            execEnd(pipeFd[count-2], &processes[getIndex(processes,count-1)]);
        }
    }
    else{
        processes_array[processes_count].pid = childPid;
        processes_array[processes_count].path = strdup(processes[0]);
        processes_count++;
    }
    return;
}

// Returns the index of the process wanted
int getIndex(const char *processes[], int count){
    int index = 0, i = 0;
    while(1){
        if(i == count){
            break;
        }
        if(!processes[index]){ // if NULL
            i++;
        }
        index++;
    }
    return index;
}

// To get the correct process argument in array form
// void sliceArray(const char *processes[], const char *process[], int index){
//     int i = 0, count = 0, j = 0;
//     while(1){
//         if(count > index){
//             break;
//         }
//         if(!process[i]){ // if NULL
//             count++;
//         }
//         else if(count == index){
//             process[j] = strdup(processes[i]);
//             j++;
//         }
//         i++;
//     }
//     process[j] = NULL;
// }

// INCLUDE close stdin ?
// serivce --> pipe
void execStart(int pipe[], const char *process[]){
    dup2(pipe[1], 1); // output-->pipe
    close(pipe[0]);
    close(pipe[1]);
    execv(process[0],process);
}

// pipe1 --> service --> pipe2
void execMiddle(int pipe1[], int pipe2[], const char *process[]){
    dup2(pipe1[0],0); // pipe1-->input
    dup2(pipe2[1],1); // output-->pipe2
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    execv(process[0],process);
}

// pipe --> service --> stdout
void execEnd(int pipe[], const char *process[]){
    dup2(pipe[0],0); // pipe-->input
    close(pipe[0]);
    close(pipe[1]);
    execv(process[0],process);
}

// Exercise 1b: print service status
size_t sm_status(sm_status_t statuses[]) {
    int i;
    for(i = 0; i<processes_count; i++){
        statuses[i] = processes_array[i];
        statuses[i].running = waitpid(processes_array[i].pid, NULL,WNOHANG) ? 0 : 1;
    }
    return processes_count;
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

