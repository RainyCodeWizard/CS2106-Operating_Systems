/*************************************
* Lab 4
* Name:
* Student No:
* Lab Group:
*************************************/

#include "mmf.h"

void *mmf_create_or_open(const char *name, size_t sz) {
    /* TODO */
    int fd = open(name, O_RDWR|O_APPEND|O_CREAT, S_IROTH|S_IWOTH); // Not sure about mode - it gives others read and write permission
    ftruncate(fd, sz);
    void *ptr = mmap(NULL, sz, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    return ptr;
}

void mmf_close(void *ptr, size_t sz) {
    /* TODO */
    munmap(ptr, sz);
}
