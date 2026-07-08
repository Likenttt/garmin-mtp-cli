#ifndef GARMIN_MTP_CLI_H
#define GARMIN_MTP_CLI_H

#include <stdbool.h>
#include <stdio.h>

typedef struct {
  int device_index;
  const char *storage_filter;
  bool debug;
  bool quiet_progress;
} Options;

typedef enum {
  CLI_PARSE_OK,
  CLI_PARSE_HELP,
  CLI_PARSE_ERROR
} CliParseResult;

void usage(FILE *out);
CliParseResult parse_options(int argc, char **argv, Options *options, int *command_index);

#endif
