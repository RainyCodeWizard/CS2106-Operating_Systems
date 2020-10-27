/*************************************
* Lab 4
* Name:
* Student No:
* Lab Group:
*************************************/

#include "shmheap.h"

shmheap_memory_handle shmheap_create(const char *name, size_t len) {
    /* TODO */
    shmheap_memory_handle mem;
    int fd = shm_open(name, O_RDWR|O_CREAT, S_IRWXU|S_IRWXO);
    if(fd == -1){
        printf("Error opening file\n");
        exit(1);
    }
    if(ftruncate(fd, len) == -1){
        printf("Error doing ftruncate\n");
        exit(2);
    }
    void *ptr = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED){
        printf("Error mmap\n");
        exit(3);
    }
    if(close(fd) == -1){
        printf("Error close\n");
        exit(4);
    }
    mem.ptr = ptr;
    mem.len = len;
    return mem;
}

shmheap_memory_handle shmheap_connect(const char *name) {
    /* TODO */
    shmheap_memory_handle mem;
    int fd = shm_open(name, O_RDWR);
    if(fd == -1){
        printf("Error opening file\n");
        exit(1);
    }

    //Using fstat to get length
    struct stat statbuff;
    if (fstat(fd, &statbuf) == -1)
    {
        printf("Error fstat %s\n");
        exit(1);
    }
    size_t len = statbuff.st_size;

    void *ptr = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED){
        printf("Error mmap\n");
        exit(3);
    }

    if(close(fd) == -1){
        printf("Error close\n");
        exit(4);
    }
    
    mem.ptr = ptr;
    mem.len = len;
    return mem;
}

void shmheap_disconnect(shmheap_memory_handle mem) {
    /* TODO */
    if(munmap(mem.ptr, mem.len) == -1){
        printf("Error munmap\n");
        exit(5);
    }
}

void shmheap_destroy(const char *name, shmheap_memory_handle mem) {
    /* TODO */
    shmheap_disconnect(mem);
    if(shm_unlink(*name) == -1){
        printf("Error shm_unlink\n");
        exit(5);
    }
}

void *shmheap_underlying(shmheap_memory_handle mem) {
    /* TODO */
}

void *shmheap_alloc(shmheap_memory_handle mem, size_t sz) {
    /* TODO */
    return mem.ptr;
}

void shmheap_free(shmheap_memory_handle mem, void *ptr) {
    /* TODO */
}

shmheap_object_handle shmheap_ptr_to_handle(shmheap_memory_handle mem, void *ptr) {
    /* TODO */
    shmheap_object_handle handle;
    handle.offset = ptr - mem.ptr;
    return handle;
}

void *shmheap_handle_to_ptr(shmheap_memory_handle mem, shmheap_object_handle hdl) {
    /* TODO */
    return mem.ptr + hdl.offset;
}
