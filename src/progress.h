#ifndef GARMIN_MTP_PROGRESS_H
#define GARMIN_MTP_PROGRESS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool quiet;
  unsigned last_percent;
} ProgressState;

int progress_callback(uint64_t const sent, uint64_t const total, void const *const data);

#endif
