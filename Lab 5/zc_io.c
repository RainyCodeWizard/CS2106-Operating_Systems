#include "zc_io.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <pthread.h>
// The zc_file struct is analogous to the FILE struct that you get from fopen.
struct zc_file {
  // Insert the fields you need here.
  size_t size;
  void *ptr;
  int fd;
  size_t offset;
  pthread_rwlock_t rwlock;
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
  void *ptr = NULL;
  if (size > 0) ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  zc_file *file = (zc_file *)malloc(sizeof(zc_file));
  file->size = size;
  file->ptr = ptr;
  file->fd = fd;
  file->offset = 0;
  
  if (pthread_rwlock_init(&file->rwlock, NULL) != 0) return NULL;

  return file;
} 

int zc_close(zc_file *file) {
  int error = msync(file->ptr, file->size, MS_SYNC) | munmap(file->ptr, file->size) | close(file->fd);
  if (pthread_rwlock_destroy(&file->rwlock) != 0) error = -1;
  free(file);
  return error;
}

const char *zc_read_start(zc_file *file, size_t *size) {
  pthread_rwlock_rdlock(&file->rwlock);

  char *ptr = file->ptr;
  ptr += file->offset;
  if (file->offset + *size > file->size) {
    *size = file->size - file->offset;
  }
  file->offset += *size;
  return ptr;
}

void zc_read_end(zc_file *file) {
  pthread_rwlock_unlock(&file->rwlock);
}

/**************
 * Exercise 2 *
 **************/

char *zc_write_start(zc_file *file, size_t size) {
  pthread_rwlock_wrlock(&file->rwlock);

  if (file->size == 0){
    file->ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, file->fd, 0);
  }
  char *returnPointer = (char *)file->ptr + file->offset;
  file->offset += size;
  
  if (file->offset > file->size){
    ftruncate(file->fd, file->offset);
    void *ptr = mremap(file->ptr, file->size, file->offset, MREMAP_MAYMOVE);
    if (ptr == MAP_FAILED) return NULL;
    
    file->ptr = ptr;
    file->size = file->offset;
    returnPointer = (char *)ptr + file->offset - size;
  }
  return returnPointer;
}

void zc_write_end(zc_file *file) {
  msync(file->ptr, file->size, MS_SYNC);
  pthread_rwlock_unlock(&file->rwlock);
}

/**************
 * Exercise 3 *
 **************/

off_t zc_lseek(zc_file *file, long offset, int whence) {
  pthread_rwlock_wrlock(&file->rwlock);
  switch (whence){
    case SEEK_SET:
    file->offset = offset;
    break;

    case SEEK_CUR:
    file->offset += offset;
    break;

    case SEEK_END:
    file->offset = file->size + offset;
    break;

    default:
    return -1;
  }
  pthread_rwlock_unlock(&file->rwlock);
  return file->offset;
}

/**************
 * Exercise 5 *
 **************/

int zc_copyfile(const char *source, const char *dest) {
  zc_file *src, *des;

  src = zc_open(source);
  if (!src) return -1;

  des = zc_open(dest);
  if (!des) {
    zc_close(src);
    return -1;
  }

  if (!zc_write_start(des, src->size)) return -1;

  int error = copy_file_range(src->fd, NULL, des->fd, NULL, src->size, 0);
  if (error != -1) error = 0;

  zc_write_end(des);

  error = zc_close(src) | zc_close(des);
  return error;
}
