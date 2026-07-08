#ifndef GARMIN_MTP_PATH_H
#define GARMIN_MTP_PATH_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  char **items;
  size_t count;
} PathParts;

void path_parts_free(PathParts *parts);
bool split_mtp_path(const char *path, PathParts *parts);
int split_remote_file_path(const char *remote_path, PathParts *dir_parts, char **filename);
char *join_local_path(const char *dir, const char *filename);
bool ends_with_slash(const char *path);
const char *local_basename(const char *path);
bool valid_remote_filename(const char *name);

#endif
