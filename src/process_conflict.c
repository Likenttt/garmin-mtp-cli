#define _POSIX_C_SOURCE 200809L

#include "process_conflict.h"

#include <stdio.h>
#include <string.h>

static const char *const known_macos_conflicts[] = {
    "Android File Transfer",
    "Android File Transfer Agent",
    "Garmin Express",
    "Garmin Express Service",
    "GarminDeviceService",
    "Garmin Express Helper",
    NULL,
};

bool process_line_is_known_mtp_conflict(const char *line) {
  if (line == NULL || *line == '\0') {
    return false;
  }

  for (size_t i = 0; known_macos_conflicts[i] != NULL; i++) {
    if (strstr(line, known_macos_conflicts[i]) != NULL) {
      return true;
    }
  }
  return false;
}

bool print_running_mtp_conflicts(FILE *err) {
#ifdef __APPLE__
  FILE *processes = popen("ps -axo pid=,args=", "r");
  if (processes == NULL) {
    return false;
  }

  char line[4096];
  bool printed_header = false;
  while (fgets(line, sizeof(line), processes) != NULL) {
    if (!process_line_is_known_mtp_conflict(line)) {
      continue;
    }
    if (!printed_header) {
      fprintf(err, "Known MTP-conflicting processes currently running:\n");
      printed_header = true;
    }
    fprintf(err, "  %s", line);
    size_t len = strlen(line);
    if (len == 0 || line[len - 1] != '\n') {
      fputc('\n', err);
    }
  }
  pclose(processes);
  return printed_header;
#else
  (void)err;
  return false;
#endif
}

void print_mtp_conflict_hint(FILE *err) {
  fprintf(err,
          "MTP device could not be opened. A background app may already be using the USB/MTP interface.\n"
          "Close Android File Transfer, Android File Transfer Agent, Garmin Express, and Garmin Express Service, "
          "then unplug/replug the watch and retry.\n");
  print_running_mtp_conflicts(err);
}
