#include "cli.h"

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void usage(FILE *out) {
  fprintf(out,
          "garmin-mtp - read and write Garmin watch files over MTP\n"
          "Compatibility alias: garminmtp\n"
          "\n"
          "Usage:\n"
          "  garmin-mtp [options] devices\n"
          "  garmin-mtp [options] dump\n"
          "  garmin-mtp [options] manual\n"
          "  garmin-mtp [options] menu\n"
          "  garmin-mtp [options] list [mtp-dir]\n"
          "  garmin-mtp [options] pull <mtp-file> <local-file-or-dir>\n"
          "  garmin-mtp [options] push <local-file> <mtp-dir> [remote-name]\n"
          "\n"
          "Options:\n"
          "  --device-index N     Select the Nth detected MTP device (default: 0)\n"
          "  --storage VALUE      Prefer storage by name, volume id, decimal id, or hex id\n"
          "  --quiet-progress     Do not print transfer progress\n"
          "  --debug              Enable libmtp debug output\n"
          "  -h, --help           Show this help\n"
          "\n"
          "Examples:\n"
          "  garmin-mtp devices\n"
          "  garmin-mtp dump\n"
          "  garmin-mtp manual\n"
          "  garmin-mtp list /\n"
          "  garmin-mtp list /GARMIN\n"
          "  garmin-mtp pull /GARMIN/APPS/EXAMPLE.PRG ./EXAMPLE.PRG\n"
          "  garmin-mtp push ./EXAMPLE.FIT /GARMIN/NEWFILES\n");
}

static bool parse_nonnegative_int(const char *value, int *out) {
  if (value == NULL || *value == '\0') {
    return false;
  }

  char *end = NULL;
  errno = 0;
  long parsed = strtol(value, &end, 10);
  if (errno != 0 || end == value || *end != '\0' || parsed < 0 || parsed > INT_MAX) {
    return false;
  }

  *out = (int)parsed;
  return true;
}

CliParseResult parse_options(int argc, char **argv, Options *options, int *command_index) {
  *options = (Options){
      .device_index = 0,
      .storage_filter = NULL,
      .debug = false,
      .quiet_progress = false,
  };

  int i = 1;
  while (i < argc) {
    const char *arg = argv[i];
    if (strcmp(arg, "--") == 0) {
      i++;
      break;
    }
    if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
      *command_index = i;
      return CLI_PARSE_HELP;
    }
    if (strcmp(arg, "--debug") == 0) {
      options->debug = true;
      i++;
      continue;
    }
    if (strcmp(arg, "--quiet-progress") == 0) {
      options->quiet_progress = true;
      i++;
      continue;
    }
    if (strcmp(arg, "--device-index") == 0) {
      if (i + 1 >= argc || !parse_nonnegative_int(argv[i + 1], &options->device_index)) {
        fprintf(stderr, "--device-index requires a non-negative integer\n");
        return CLI_PARSE_ERROR;
      }
      i += 2;
      continue;
    }
    if (strcmp(arg, "--storage") == 0) {
      if (i + 1 >= argc || argv[i + 1][0] == '\0') {
        fprintf(stderr, "--storage requires a value\n");
        return CLI_PARSE_ERROR;
      }
      options->storage_filter = argv[i + 1];
      i += 2;
      continue;
    }
    if (strncmp(arg, "-", 1) == 0) {
      fprintf(stderr, "unknown option: %s\n", arg);
      return CLI_PARSE_ERROR;
    }
    break;
  }

  *command_index = i;
  return CLI_PARSE_OK;
}
