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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h> //Delete this later
//Need to free malloc and strdup
int getIndex(const char *processes[], int count);
void execStart(int pipe[], const char *process[]);
void execMiddle(int pipe1[], int pipe2[], const char *process[]);
void execEnd(int pipe[], const char *process[]);

sm_status_t services_array[SM_MAX_SERVICES]; // To take note of the informations of processes
int services_count = 0; // To take note of the amount of processes currently
pid_t pids_in_service[SM_MAX_SERVICES][SM_MAX_SERVICES];
int pids_count[SM_MAX_SERVICES] = {0};

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
    int fd[count-1][2];
    for(i = 0; i<count-1; i++){
        pipe(fd[i]);
    }
    for(i = 0; i<count; i++){
        pid_t childPid = fork();
        if(childPid == 0){
            if (i == 0){
                close(STDIN_FILENO);
                if(count == 1){ //if only 1 process
                    execv(processes[0],processes);
                }
                int j;
                for (j = 1; j<count-1; j++){ //close other unused pipes
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
                execStart(fd[0], processes);
            }
            else if (i == count-1){
                int k;
                for (k = 0; k<count-2; k++){ //close other unused pipes
                    close(fd[k][0]);
                    close(fd[k][1]);
                }
                execEnd(fd[i-1], &processes[getIndex(processes,i)]);
            }
            else{
                int l;
                for(l = 0; l<count-1; l++){
                    if(l != i && l != i-1){ // Except the 2 pipes used, close other pipes
                        close(fd[l][0]);
                        close(fd[l][1]);
                    }
                }
                execMiddle(fd[i-1], fd[i], &processes[getIndex(processes,i)]);
            }
        }
        else{
            if(i==count-1){ // if it is the last process, or the only process
                services_array[services_count].pid = childPid;
                services_array[services_count].path = strdup(processes[getIndex(processes, i)]);
                services_count++;
            }
            else{
                pids_in_service[services_count][pids_count[services_count]++] = childPid;
            }
        }        
    }
    for (i = 0; i<count-1;i++){
        close(fd[i][0]); //close all the pipes for parent
        close(fd[i][1]);
    }
    return;
}

// Outputs to log file if redirect_out!=0
void start(const char *processes[], int redirect_out){
    int i = 0, count = 0; //count represents the number of processes
    while(1){
        if (!processes[i]){ //if NULL
            if(i > 0 && !processes[i-1]) break;
            count++;
        }
        i++;
    }
    if (count != 1){
        int fd[count-1][2];
        for(i = 0; i<count-1; i++){
            pipe(fd[i]);
        }        
    }
    if (redirect_out){
        char logFile[20];
        snprintf(logFile, sizeof(logFile), "./service%d.log", services_count+1);
        int logFile_fd = open(logFile, O_APPEND | O_CREAT, S_IRWXU);
    }
    for(i = 0; i<count; i++){
        pid_t childPid = fork();
        if(childPid == 0){
            if (i == 0){
                close(STDIN_FILENO);
                if(count == 1){ //if only 1 process
                    if (redirect_out){
                        dup2(logFile_fd, STDOUT_FILENO);
                        dup2(logFile_fd, STDERR_FILENO);
                        close(logFile_fd);
                    }
                    execv(processes[0],processes);
                }
                int j;
                for (j = 1; j<count-1; j++){ //close other unused pipes
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
                if (redirect_out) close(logFile_fd);
                execStart(fd[0], processes);
            }
            else if (i == count-1){
                int k;
                for (k = 0; k<count-2; k++){ //close other unused pipes
                    close(fd[k][0]);
                    close(fd[k][1]);
                }
                if (redirect_out) {
                    dup2(logFile_fd, STDOUT_FILENO);
                    dup2(logFile_fd, STDERR_FILENO);
                    close(logFile_fd);
                }
                execEnd(fd[i-1], &processes[getIndex(processes,i)]);
            }
            else{
                int l;
                for(l = 0; l<count-1; l++){
                    if(l != i && l != i-1){ // Except the 2 pipes used, close other pipes
                        close(fd[l][0]);
                        close(fd[l][1]);
                    }
                }
                if (redirect_out) close(logFile_fd);
                execMiddle(fd[i-1], fd[i], &processes[getIndex(processes,i)]);
            }
        }
        else{
            if(i==count-1){ // if it is the last process, or the only process
                services_array[services_count].pid = childPid;
                services_array[services_count].path = strdup(processes[getIndex(processes, i)]);
                services_count++;
            }
            else{
                pids_in_service[services_count][pids_count[services_count]++] = childPid;
            }
        }        
    }
    for (i = 0; i<count-1;i++){
        close(fd[i][0]); //close all the pipes for parent
        close(fd[i][1]);
    }
    if (redirect_out) close(logFile_fd);
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

// command --> pipe
void execStart(int pipe[], const char *process[]){
    dup2(pipe[1], STDOUT_FILENO); // output-->pipe
    close(STDIN_FILENO); //Close stdinput for the first child
    close(pipe[0]);
    close(pipe[1]);
    execv(process[0],process);
}

// pipe1 --> command --> pipe2
void execMiddle(int pipe1[], int pipe2[], const char *process[]){
    dup2(pipe1[0],STDIN_FILENO); // pipe1-->input
    dup2(pipe2[1],STDOUT_FILENO); // output-->pipe2
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    execv(process[0],process);
}

// pipe --> command --> stdout
void execEnd(int pipe[], const char *process[]){
    dup2(pipe[0],STDIN_FILENO); // pipe-->input
    close(pipe[0]);
    close(pipe[1]);
    execv(process[0],process);
}

// Exercise 1b: print service status
size_t sm_status(sm_status_t statuses[]) {
    int i;
    for(i = 0; i<services_count; i++){
        statuses[i] = services_array[i];
        statuses[i].running = waitpid(services_array[i].pid, NULL,WNOHANG) ? 0 : 1;
    }
    return services_count;
}

// Exercise 3: stop service, wait on service, and shutdown
void sm_stop(size_t index) {
    int i, count = pids_count[index];
    for (i = 0; i < count; i++){
        if (waitpid(pids_in_service[index][i], NULL,WNOHANG) == 0){
            kill(pids_in_service[index][i], SIGTERM);
            waitpid(pids_in_service[index][i], NULL, 0);
        }
    }
    if(waitpid(services_array[index].pid, NULL,WNOHANG) == 0){
        kill(services_array[index].pid, SIGTERM);
        waitpid(services_array[index].pid, NULL, 0);
    }
}

void sm_wait(size_t index) {
    int i, count = pids_count[index];
    for (i = 0; i < count; i++){
        waitpid(pids_in_service[index][i], NULL, 0);
    }
    waitpid(services_array[index].pid, NULL, 0);
}

void sm_shutdown(void) {
    int i;
    for(i = 0; i < services_count; i++){
        sm_stop(i);
    }
}

// Exercise 4: start with output redirection
void sm_startlog(const char *processes[]) {
    start(processes, 1);
}

// Exercise 5: show log file
void sm_showlog(size_t index) {
}

