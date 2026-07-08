#ifndef GARMIN_MTP_PUSH_ARGS_H
#define GARMIN_MTP_PUSH_ARGS_H

#include <stdbool.h>
#include <stdio.h>

typedef struct {
  bool overwrite;
  const char *local_path;
  const char *remote_dir_path;
  const char *remote_name;
} PushArgs;

int parse_push_args(int argc, char **argv, PushArgs *out, FILE *err);

#endif
