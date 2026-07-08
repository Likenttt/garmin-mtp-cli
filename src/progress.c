#include "progress.h"

#include <inttypes.h>
#include <stdio.h>

int progress_callback(uint64_t const sent, uint64_t const total, void const *const data) {
  ProgressState *state = (ProgressState *)data;
  if (state == NULL || state->quiet) {
    return 0;
  }

  if (total == 0) {
    fprintf(stderr, "\r%" PRIu64 " bytes", sent);
    return 0;
  }

  uint64_t one_percent = total / 100u;
  if (one_percent == 0) {
    one_percent = 1;
  }
  unsigned percent = sent >= total ? 100u : (unsigned)(sent / one_percent);
  if (percent > 100u) {
    percent = 100u;
  }
  if (percent != state->last_percent) {
    state->last_percent = percent;
    fprintf(stderr, "\r%3u%%", percent);
    fflush(stderr);
  }
  if (sent >= total) {
    fputc('\n', stderr);
  }
  return 0;
}
