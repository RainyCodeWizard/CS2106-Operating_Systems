/*************************************
* Lab 4
* Name: H Mohamed Hussain
* Student No: A0199425R
* Lab Group: 10
*************************************/

#include "mmf.h"

void *mmf_create_or_open(const char *name, size_t sz) {
    /* TODO */
    int fd = open(name, O_RDWR|O_CREAT, S_IRWXU|S_IRWXO); // Not sure about mode - it gives others read and write permission
    if(fd == -1){
        printf("Error opening file\n");
        exit(1);
    }
    if(ftruncate(fd, sz) == -1){
        printf("Error doing ftruncate\n");
        exit(2);
    }
    void *ptr = mmap(NULL, sz, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED){
        printf("Error mmap\n");
        exit(3);
    }
    if(close(fd) == -1){
        printf("Error close\n");
        exit(4);
    }
    return ptr;
}

void mmf_close(void *ptr, size_t sz) {
    /* TODO */
    if(munmap(ptr, sz) == -1){
        printf("Error munmap\n");
        exit(5);
    }
}
