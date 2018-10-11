#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

size_t get_file_size(const char *filename)
{
  struct stat sb;

  if (lstat(filename, &sb) == -1) {
    perror("lstat");
    exit(EXIT_FAILURE);
  }
  return sb.st_size;
}

uint8_t *get_write_mapping(const char *filename, size_t size)
{
  int fd;
  void *p;
  fd = open(filename, O_RDWR);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  p = mmap(NULL, size, PROT_READ | PROT_WRITE,MAP_SHARED, fd, 0);
  if (p == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  close(fd);
  return (uint8_t*)p;
}

void release_mapping(const uint8_t *a, size_t size)
{
  munmap((void*)a, size);
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
      fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
      exit(EXIT_FAILURE);
  }

  printf("Filename passed: %s\n", argv[1]);
  printf("File size: %zd\n",get_file_size(argv[1]));
  size_t size = get_file_size(argv[1]);
  uint8_t *mapping = get_write_mapping(argv[1], size);
  for (size_t i = 0; i < size; i++)
    mapping[i] = 'a';
  release_mapping(mapping, size);
  return 0;
}
