#ifndef GARMIN_MTP_PROCESS_CONFLICT_H
#define GARMIN_MTP_PROCESS_CONFLICT_H

#include <stdbool.h>
#include <stdio.h>

bool process_line_is_known_mtp_conflict(const char *line);
bool print_running_mtp_conflicts(FILE *err);
void print_mtp_conflict_hint(FILE *err);

#endif
