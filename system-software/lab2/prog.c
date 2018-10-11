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

const uint8_t *get_read_mapping(const char *filename, size_t size)
{
  int fd;
  void *p;
  fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  p = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (p == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  close(fd);
  return (const uint8_t*)p;
}

void release_mapping(const uint8_t *p, size_t size)
{
  munmap((void*)p, size);
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
      fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
      exit(EXIT_FAILURE);
  }

  uint8_t  checksum = 0;
  printf("Filename passed: %s\n", argv[1]);
  size_t size = get_file_size(argv[1]);
  printf("File size: %zd\n",size);
  printf("Mapping address: %p\n",get_read_mapping(argv[1],size));
  const uint8_t *  mapping = get_read_mapping(argv[1], size);
    for (size_t i = 0; i < size; i++)
      checksum ^= mapping[i];

  release_mapping(mapping,size);

  printf("Check sum: %hhu\n",checksum );
  return 0;
}
