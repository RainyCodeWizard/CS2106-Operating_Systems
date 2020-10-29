/*************************************
* Lab 4
* Name:
* Student No:
* Lab Group:
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
    header.next_partition = NULL
    memcpy(ptr, &header, sizeof(shmheap_header));

    shmheap_memory_handle mem;
    mem.ptr = ptr;
    mem.len = len;
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
    shmheap_disconnect(mem);
    shm_unlink(name);
}

void *shmheap_underlying(shmheap_memory_handle mem) {
    return mem.ptr;
}

void *shmheap_alloc(shmheap_memory_handle mem, size_t sz) {
    shmheap_header *header_ptr = mem.ptr;
    if (!header_ptr->next_partition){ //If next_partition is NULL, i.e. no memory is occupied
        shmheap_allocate_partition(header_ptr + 1, sz, NULL, 0);
        return header_ptr + 1; //After the header
    }
    else if(header_ptr->free && header_ptr->data_size >= sz){
        shmheap_allocate_partition(header_ptr + 1, sz, header_ptr->next_partition, 0);
        return header_ptr + 1;
    }

    shmheap_partition *partition_ptr = header_ptr->next_partition;
    while (partition_ptr){
        if (!partition_ptr->next_partition){ //If there is no next partition. ASSUMPTION: there is enough space in memory
            shmheap_allocate_partition(partition_ptr + 1, sz, NULL, 1);
            return partition_ptr + 1;
        }
        else if (partition_ptr->free && partition_ptr->data_size >= sz){
            shmheap_allocate_partition(partition_ptr + 1, sz, partition_ptr->next_partition, 1);
            return partition_ptr + 1;
        }
        partition_ptr = partition_ptr->next_partition;
    }
}

/* Cases of partitions:
Case 1: Next partition is NULL. Means memory region after the curr pointer is all empty.
    a) If current ptr is Header. Means the whole memory region is empty
    b) If current ptr is a partition. Means no suitable space is found in previous memory partitions.
Case 2: There is a partition next.
    a) If current ptr is Header. Means memory to be allocated is between Header and a partition.
    b) If current ptr is a partition. Means memory to be allocated is between 2 partitions.
*/
void shmheap_allocate_partition(void *start_ptr, size_t sz, shmheap_partition *next_partition, int notHeader){ // notHeader is 0 when header.
    if (sz % 8) sz += 8 - sz % 8; // Rounding size to multiple of 8

    if (!next_partition){ //If next_partition is NULL, i.e. free memory from pointer onwards
        //Not checking if there is enough space (Part of assumption)

        char *temp_ptr = start_ptr; // Changing ptr to char type. As char is 1 byte, easier to do pointer arithmetic
        temp_ptr += sz;
        
        if (notHeader == 0){
            shmheap_header *header = start_ptr; // Might give error
            header--;

            shmheap_partition temp;
            temp.free = 1;
            temp.data_size = header->data_size - sz - sizeof(shmheap_partition);
            memcpy(temp_ptr, &temp, sizeof(shmheap_partition));

            
            // Point header to the partition
            shmheap_header temp_header;
            temp_header.len = header->len;
            temp_header.free = 0;
            temp_header.data_size = sz;
            temp_header.next_partition = temp_ptr;
            memcpy(header, &temp_header, sizeof(shmheap_header));
        }
        else{
            // Might give error, didnt check if have enough space for partition.
            shmheap_partition *curr_partition = start_ptr;
            curr_partition--;

            if (curr_partition->data_size - sz >= sizeof(shmheap_partition)){ //To check if there is space for partition
                shmheap_partition temp;
                temp.free = 1;
                temp.data_size = curr_partition->data_size - sz - sizeof(shmheap_partition);
                temp.next_partition = NULL;
                memcpy(temp_ptr, &temp, sizeof(shmheap_partition));
            }

            temp.free = 0;
            temp.data_size = sz;
            temp.next_partition = temp_ptr;
            memcpy(curr_partition, &temp, sizeof(shmheap_partition)); // Change current partition status to allocated
        }
    }
    else if (notHeader == 0){ // if header. Same as the else below, catered for header case
        shmheap_header *header = start_ptr;
        header--; // moving from start_ptr to header. Might give error!
        if (header->data_size < sz + sizeof(shmheap_partition)){ // If there is not enough space for another partition, dont put in partition
            header->free = 0; // Might give error, didnt use memcpy
            header->next_partition = next_partition;
        }
        else{ // Put in a partition
            char *temp_ptr = start_ptr;
            temp_ptr += sz;

            shmheap_partition temp;
            temp.free = 1;
            temp.data_size = header->data_size - sz - sizeof(shmheap_partition); // Remaining size of free space after allocating object and partition
            temp.next_partition = next_partition;
            memcpy(temp_ptr, &temp, sizeof(shmheap_partition));

            header->free = 0; //Might giver error
            header->data_size = sz;
            header->next_partition = temp_ptr;
        }
    }
    else{
        shmheap_partition *curr_partition = start_ptr;
        curr_partition--; // moving from start_ptr to curr_partition. Might give error!
        if (curr_partition->data_size < sz + sizeof(shmheap_partition)){ // If there is not enough space for another partition, dont put in partition
            curr_partition->free = 0; // Might give error, didnt use memcpy
            curr_partition->next_partition = next_partition;
        }
        else{ // Put in a partition
            char *temp_ptr = start_ptr;
            temp_ptr += sz;

            shmheap_partition temp;
            temp.free = 1;
            temp.data_size = curr_partition->data_size - sz - sizeof(shmheap_partition); // Remaining size of free space after allocating object and partition
            temp.next_partition = next_partition;
            memcpy(temp_ptr, &temp, sizeof(shmheap_partition));

            //curr_partition->next_partition = temp_ptr; // Might give error, not sure if we can do this. Changing mapped mem without using memcpy
            temp.free = 0;
            temp.data_size = sz;
            temp.next_partition = temp_ptr;
            memcpy(curr_partition, &temp, sizeof(shmheap_partition)); // Change current partition status to allocated
        }
        

    }

}
void shmheap_free(shmheap_memory_handle mem, void *ptr) {
    shmheap_header *header = mem.ptr;
    if (header + 1 == ptr) {
        header->free = 1;
        if (header->next_partition && ((shmheap_partition *)header->next_partition)->free){ // If there is a next partition and it is free
            header->next_partition = ((shmheap_partition *)header->next_partition)->next_partition;
            header->data_size += ((shmheap_partition *)header->next_partition)->data_size + sizeof(shmheap_partition);
        }
        return;
    }
    shmheap_partition *prev_partition = NULL;
    shmheap_partition *partition = header->next_partition;
    while (partition) {
        if (partition + 1 == ptr){ // found
            break;
        }
        prev_partition = partition;
        partition = partition->next_partition;
    }

    partition->free = 1; // Labeling the region as free
    shmheap_partition *next_partition = partition->next_partition;
    //Doing merging below
    if (!prev_partition){ //If previous partition is header
        // Check Left and Right to merge
        if(header->free && next_partition && next_partition->free){
            header->next_partition = next_partition->next_partition;
            header->data_size += partition->data_size + next_partition->data_size + 2 * sizeof(shmheap_partition);
        }
        // Check Left to merge
        else if (header->free){
            header->next_partition = partition->next_partition;
            header->data_size += partition->data_size + sizeof(shmheap_partition);
        }
        // Check Right to merge
        else if (next_partition){
            if (next_partition->free){
                partition->next_partition = next_partition->next_partition;
                partition->data_size += next_partition->data_size + sizeof(shmheap_partition);
            }
        }
    }
    else{
        // Check Left and Right to merge
        if (prev_partition->free && next_partition && next_partition->free) {
            prev_partition->next_partition = next_partition->next_partition;
            prev_partition->data_size += partition->data_size + next_partition->data_size + 2 * sizeof(shmheap_partition);
        }
        // Check Left to merge
        else if (prev_partition->free) {
            prev_partition->next_partition = partition->next_partition;
            prev_partition->data_size += partition->data_size + sizeof(shmheap_partition);
        }
        // Check Right to merge
        else if (next_partition && next_partition->free) {
            partition->next_partition = next_partition->next_partition;
            partition->data_size += next_partition->data_size + sizeof(shmheap_partition);
        }
    }
}

shmheap_object_handle shmheap_ptr_to_handle(shmheap_memory_handle mem, void *ptr) {
    shmheap_object_handle handle;// = {ptr - mem.ptr};
    handle.offset = ptr - mem.ptr;
    return handle;
}

void *shmheap_handle_to_ptr(shmheap_memory_handle mem, shmheap_object_handle hdl) {
    return mem.ptr + hdl.offset;
}
