#define _POSIX_C_SOURCE 200809L

#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *xcalloc(size_t count, size_t size) {
  void *ptr = calloc(count, size);
  if (ptr == NULL) {
    fprintf(stderr, "out of memory\n");
    exit(2);
  }
  return ptr;
}

char *xstrdup(const char *value) {
  char *copy = strdup(value == NULL ? "" : value);
  if (copy == NULL) {
    fprintf(stderr, "out of memory\n");
    exit(2);
  }
  return copy;
}
