#ifndef GARMIN_MTP_MEMORY_H
#define GARMIN_MTP_MEMORY_H

#include <stddef.h>

void *xcalloc(size_t count, size_t size);
char *xstrdup(const char *value);

#endif
