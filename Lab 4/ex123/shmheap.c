/*************************************
* Lab 4
* Name: H Mohamed Hussain
* Student No: A0199425R
* Lab Group: 10
*************************************/

#include "shmheap.h"

shmheap_memory_handle shmheap_create(const char *name, size_t len) {

    int fd = shm_open(name, O_CREAT | O_RDWR, S_IRWXU);
    ftruncate(fd, len);
    void *ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    //Bookkeeping. Adding header to the start of shared memory
    shmheap_header header;
    header.len = len;
    header.free = 1;
    header.data_size = len - sizeof(shmheap_header);
    memcpy(ptr, &header, sizeof(shmheap_header));

    shmheap_memory_handle mem;
    mem.ptr = ptr;
    mem.len = len;

    // Allocate memory for mutex
    sem_t *mutex = shmheap_alloc(mem, sizeof(sem_t));
    sem_init(mutex, 1, 1);

    return mem;

}

shmheap_memory_handle shmheap_connect(const char *name) {
    int fd = shm_open(name, O_RDWR,S_IRWXU);

    shmheap_header *header_ptr = mmap(NULL, sizeof(shmheap_header), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    size_t len = header_ptr->len;
    void *ptr = mremap(header_ptr, sizeof(shmheap_header), len, MREMAP_MAYMOVE);
    
    shmheap_memory_handle mem;
    mem.ptr = ptr;
    mem.len = len;
    return mem;
}

void shmheap_disconnect(shmheap_memory_handle mem) {
    munmap(mem.ptr, mem.len);
}

void shmheap_destroy(const char *name, shmheap_memory_handle mem) {
    shmheap_header *header_ptr = mem.ptr;
    sem_t *mutex = (void *)(header_ptr + 1);
    sem_destroy(mutex);
    shmheap_disconnect(mem);
    shm_unlink(name);
}

void *shmheap_underlying(shmheap_memory_handle mem) {
    return mem.ptr;
}

void *shmheap_alloc(shmheap_memory_handle mem, size_t sz) {
    shmheap_header *header_ptr = mem.ptr;
    sem_t *mutex;
    if (header_ptr->free == 0) { // If header is not free
        mutex = (void *)(header_ptr + 1);
        sem_wait(mutex);
    }
    
    if (sz % 8) sz += 8 - sz % 8; // Rounding sz to multiple of 8

    if (header_ptr->free && header_ptr->data_size >= sz){
        header_ptr->free = 0;
        if (header_ptr->data_size >= sz + sizeof(shmheap_partition)) {
            char *temp_ptr = (char *) (header_ptr + 1);
            temp_ptr += sz;

            shmheap_partition partition; // Insert partition
            partition.free = 1;
            partition.data_size = header_ptr->data_size - sz - sizeof(shmheap_partition);
            memcpy(temp_ptr, &partition, sizeof(shmheap_partition));
            
            header_ptr->data_size = sz;
        }
        return header_ptr + 1;
    }
    size_t size = header_ptr->data_size;
    shmheap_partition *partition = (void *)((char *)(header_ptr + 1) + size);
    while (!partition->free || partition->data_size < sz){
        size = partition->data_size;
        partition = (void *)((char *)(partition + 1) + size);
    }
    partition->free = 0;
    if (partition->data_size >= sz + sizeof(shmheap_partition)) {
        char *temp_ptr = (char *) (partition + 1);
        temp_ptr += sz;

        shmheap_partition temp; // Insert partition
        temp.free = 1;
        temp.data_size = partition->data_size - sz - sizeof(shmheap_partition);
        memcpy(temp_ptr, &temp, sizeof(shmheap_partition));

        partition->data_size = sz; 
    }
    sem_post(mutex);
    return partition + 1;
    
}

void shmheap_free(shmheap_memory_handle mem, void *ptr) {
    shmheap_header *header = mem.ptr;

    sem_t *mutex;
    
    mutex = (void *)(header + 1);
    sem_wait(mutex);

    if (header + 1 == ptr) header->free = 1;

    size_t remaining_size = mem.len - sizeof(shmheap_header) - header->data_size; // If remaining_size==0, Means no more partitions left
    shmheap_partition *prev_partition = NULL;
    shmheap_partition *partition;
    if (remaining_size) partition = (void *)((char *)(header + 1) + header->data_size);
    
    while (remaining_size) {
        remaining_size -= partition->data_size + sizeof(shmheap_partition);
        if (partition + 1 == ptr) {
            partition->free = 1;
        }
        if (!prev_partition && header->free && partition->free) {
            header->data_size += partition->data_size + sizeof(shmheap_partition);
            partition = (void *)((char *)(partition + 1) + partition->data_size); //Might give segmentation error
        }
        else if (prev_partition && prev_partition->free && partition->free) {
            prev_partition->data_size += partition->data_size + sizeof(shmheap_partition);
            partition = (void *)((char *)(partition + 1) + partition->data_size); //Might give segmentation error
        }
        else if (remaining_size) {
            prev_partition = partition;
            partition = (void *)((char *)(partition + 1) + partition->data_size);
        }
    }
    sem_post(mutex);

}



shmheap_object_handle shmheap_ptr_to_handle(shmheap_memory_handle mem, void *ptr) {
    shmheap_object_handle handle;// = {ptr - mem.ptr};
    handle.offset = ptr - mem.ptr;
    return handle;
}

void *shmheap_handle_to_ptr(shmheap_memory_handle mem, shmheap_object_handle hdl) {
    return mem.ptr + hdl.offset;
}
