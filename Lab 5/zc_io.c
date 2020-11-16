#include "zc_io.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>

// The zc_file struct is analogous to the FILE struct that you get from fopen.
struct zc_file {
  // Insert the fields you need here.
  size_t size;
  void *ptr;
  int fd;
  size_t offset;
};

/**************
 * Exercise 1 *
 **************/

zc_file *zc_open(const char *path) {
  int fd = open(path, O_CREAT | O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);

  if (fd == -1){
    return NULL;
  }

  // To get the size of file
  struct stat buf;
  if (fstat(fd, &buf) == -1) return NULL;
  int size = buf.st_size;

  void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
  if (ptr == MAP_FAILED) return NULL;
  
  zc_file *file = (zc_file *)malloc(sizeof(zc_file));
  file->size = size;
  file->ptr = ptr;
  file->fd = fd;
  file->offset = 0;
  
  return file;
} 

int zc_close(zc_file *file) {
  //To implement: Flush to file

  int error = munmap(file->ptr, file->size) | close(file->fd);
  free(file);
  return error;
}

const char *zc_read_start(zc_file *file, size_t *size) {
  char *ptr = file->ptr;
  ptr += file->offset;
  if (file->offset + *size > file->size) {
    *size = file->size - file->offset;
  }
  file->offset += *size;
  return ptr;
}

void zc_read_end(zc_file *file) {
  // To implement
  // What am i supposed to do here
}

/**************
 * Exercise 2 *
 **************/

char *zc_write_start(zc_file *file, size_t size) {
  char *returnPointer = (char *)file->ptr + file->offset;
  file->offset += size;
  
  if (file->offset > file->size){
    ftruncate(file->fd, file->offset);
    void *ptr = mremap(file->ptr, file->size, new_size, MREMAP_MAYMOVE);
    if (ptr == MAP_FAILED) return NULL;
    
    file->ptr = ptr;
    file->size = file->offset;
    returnPointer = (char *)ptr + file->offset - size;
  }
  return returnPointer;
}

void zc_write_end(zc_file *file) {
  msync(file->ptr, file->size, MS_SYNC);
}

/**************
 * Exercise 3 *
 **************/

off_t zc_lseek(zc_file *file, long offset, int whence) {
  // To implement
  return -1;
}

/**************
 * Exercise 5 *
 **************/

int zc_copyfile(const char *source, const char *dest) {
  // To implement
  return -1;
}
