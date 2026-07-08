#define _POSIX_C_SOURCE 200809L

#include "path.h"

#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void path_parts_free(PathParts *parts) {
  if (parts == NULL) {
    return;
  }
  for (size_t i = 0; i < parts->count; i++) {
    free(parts->items[i]);
  }
  free(parts->items);
  parts->items = NULL;
  parts->count = 0;
}

static bool path_parts_push(PathParts *parts, const char *item) {
  char **next = realloc(parts->items, sizeof(char *) * (parts->count + 1));
  if (next == NULL) {
    return false;
  }
  parts->items = next;
  parts->items[parts->count++] = xstrdup(item);
  return true;
}

bool split_mtp_path(const char *path, PathParts *parts) {
  memset(parts, 0, sizeof(*parts));
  if (path == NULL || *path == '\0' || strcmp(path, "/") == 0) {
    return true;
  }

  char *copy = xstrdup(path);
  char *save = NULL;
  for (char *token = strtok_r(copy, "/", &save); token != NULL; token = strtok_r(NULL, "/", &save)) {
    if (strcmp(token, ".") == 0 || *token == '\0') {
      continue;
    }
    if (strcmp(token, "..") == 0) {
      fprintf(stderr, "MTP paths cannot contain '..': %s\n", path);
      free(copy);
      path_parts_free(parts);
      return false;
    }
    if (!path_parts_push(parts, token)) {
      fprintf(stderr, "out of memory\n");
      free(copy);
      path_parts_free(parts);
      return false;
    }
  }

  free(copy);
  return true;
}

int split_remote_file_path(const char *remote_path, PathParts *dir_parts, char **filename) {
  PathParts parts;
  if (!split_mtp_path(remote_path, &parts)) {
    return 1;
  }

  if (parts.count == 0) {
    fprintf(stderr, "remote file path must include a filename\n");
    path_parts_free(&parts);
    return 1;
  }

  memset(dir_parts, 0, sizeof(*dir_parts));
  if (parts.count > 1) {
    dir_parts->items = xcalloc(parts.count - 1, sizeof(char *));
    dir_parts->count = parts.count - 1;
    for (size_t i = 0; i + 1 < parts.count; i++) {
      dir_parts->items[i] = xstrdup(parts.items[i]);
    }
  }

  *filename = xstrdup(parts.items[parts.count - 1]);
  path_parts_free(&parts);
  return 0;
}

char *join_local_path(const char *dir, const char *filename) {
  size_t dir_len = strlen(dir);
  size_t name_len = strlen(filename);
  bool needs_slash = dir_len > 0 && dir[dir_len - 1] != '/';
  char *result = xcalloc(dir_len + (needs_slash ? 1u : 0u) + name_len + 1u, sizeof(char));
  memcpy(result, dir, dir_len);
  if (needs_slash) {
    result[dir_len] = '/';
  }
  memcpy(result + dir_len + (needs_slash ? 1u : 0u), filename, name_len);
  return result;
}

bool ends_with_slash(const char *path) {
  size_t len = strlen(path);
  return len > 0 && path[len - 1] == '/';
}

const char *local_basename(const char *path) {
  const char *last = strrchr(path, '/');
  return last == NULL ? path : last + 1;
}

bool valid_remote_filename(const char *name) {
  return name != NULL && *name != '\0' && strchr(name, '/') == NULL && strcmp(name, ".") != 0 && strcmp(name, "..") != 0;
}
