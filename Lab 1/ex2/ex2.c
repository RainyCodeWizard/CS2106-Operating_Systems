/*************************************
* Lab 1 Exercise 2
* Name: H Mohamed Hussain
* Student No: A0199425R
* Lab Group: 10
*************************************/

#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "function_pointers.h"

// the runner is empty now! Modify it to fulfill the requirements of the
// exercise. You can use ex1.c as a template

// macros
#define SUM_LIST 0
#define INSERT_FROM_HEAD_AT 1
#define INSERT_FROM_TAIL_AT 2
#define DELETE_FROM_HEAD_AT 3
#define DELETE_FROM_TAIL_AT 4
#define RESET_LIST 5
#define MAP 6

void read_file_inputs(list *lst,FILE *filePointer);
void run_instruction(list *lst, int instr, FILE *filePointer);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Error: expecting 1 argument, %d found\n", argc - 1);
        exit(1);
    }

    // we read in the file name provided as argument
    char *fname = argv[1];

    // updating the array of function pointers
    // do not remove this call 
    // (you may leave the function empty if you do not need it)
    update_functions();

    // rest of code logic here

    FILE *filePointer;
    filePointer = fopen(fname,"r");

    if (filePointer == NULL){
        printf("Cannot open file %s\n", fname);
        exit(1);
    }
    else{
        list *lst = (list *)malloc(sizeof(list));
        lst->head = NULL;
        lst->tail = NULL;  
        read_file_inputs(lst, filePointer);
    }
    return 0;
}

void read_file_inputs(list *lst, FILE *filePointer){
    char arr[1];
    while (fgets(arr, 2, filePointer)){
        if (arr[0] != '\n' && arr[0] != '\40'){ //if not next line and space
            run_instruction(lst, arr[0]-48, filePointer);
        }
    }
    run_instruction(lst, RESET_LIST, filePointer); //Resets list
    fclose(filePointer);
}

void run_instruction(list *lst, int instr ,FILE *filePointer)
{
    int index, data;
    switch (instr)
    {
    case SUM_LIST:
        printf("%ld\n", sum_list(lst));
        break;
    case INSERT_FROM_HEAD_AT:
        fscanf(filePointer," %d %d", &index, &data);
        // printf("1 %d %d \n",index,data);
        insert_node_from_head_at(lst, index, data);
        break;
    case INSERT_FROM_TAIL_AT:
        fscanf(filePointer, " %d %d", &index, &data);
        // printf("2 %d %d \n",index,data);
        insert_node_from_tail_at(lst, index, data);
        break;
    case DELETE_FROM_HEAD_AT:
        fscanf(filePointer, " %d", &index);
        // printf("3 %d \n",index);
        delete_node_from_head_at(lst, index);
        break;
     case DELETE_FROM_TAIL_AT:
        fscanf(filePointer, " %d", &index);
        // printf("4 %d \n",index);
        delete_node_from_tail_at(lst, index);
        break;   
    case RESET_LIST:
        // printf("5\n");
        reset_list(lst);
        break;
    
    case MAP:
        fscanf(filePointer, " %d", &index);
        map(lst, func_list[index]);
        break;
    }

}