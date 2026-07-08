#include "commands.h"

#include <stdio.h>

static int print_no_mtp_backend(void) {
  fprintf(stderr,
          "This garmin-mtp build was compiled without libmtp support.\n"
          "MTP device operations are unavailable in this build. Use macOS/Linux packages with libmtp, "
          "or build with -DGARMIN_MTP_WITH_LIBMTP=ON on a platform where libmtp is available.\n");
  return 1;
}

int command_devices(const Options *options) {
  (void)options;
  return print_no_mtp_backend();
}

int command_dump(const Options *options) {
  (void)options;
  return print_no_mtp_backend();
}

int command_list(const Options *options, int argc, char **argv) {
  (void)options;
  (void)argc;
  (void)argv;
  return print_no_mtp_backend();
}

int command_pull(const Options *options, int argc, char **argv) {
  (void)options;
  (void)argc;
  (void)argv;
  return print_no_mtp_backend();
}

int command_push(const Options *options, int argc, char **argv) {
  (void)options;
  (void)argc;
  (void)argv;
  return print_no_mtp_backend();
}
