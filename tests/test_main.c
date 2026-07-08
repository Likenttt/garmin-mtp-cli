#include "test.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int tests_failed = 0;

int silence_stderr(void) {
  fflush(stderr);
  int saved_fd = dup(STDERR_FILENO);
  int null_fd = open("/dev/null", O_WRONLY);
  if (saved_fd >= 0 && null_fd >= 0) {
    dup2(null_fd, STDERR_FILENO);
  }
  if (null_fd >= 0) {
    close(null_fd);
  }
  return saved_fd;
}

void restore_stderr(int saved_fd) {
  fflush(stderr);
  if (saved_fd >= 0) {
    dup2(saved_fd, STDERR_FILENO);
    close(saved_fd);
  }
}

void cli_tests(void);
void file_type_tests(void);
void manual_tests(void);
void mtp_tree_tests(void);
void path_tests(void);
void process_conflict_tests(void);

static void run_test(const char *name, void (*test_fn)(void)) {
  int failures_before = tests_failed;
  test_fn();
  if (tests_failed == failures_before) {
    printf("ok %s\n", name);
  } else {
    printf("not ok %s\n", name);
  }
}

int main(void) {
  run_test("cli", cli_tests);
  run_test("file_type", file_type_tests);
  run_test("manual", manual_tests);
  run_test("mtp_tree", mtp_tree_tests);
  run_test("path", path_tests);
  run_test("process_conflict", process_conflict_tests);

  if (tests_failed != 0) {
    fprintf(stderr, "%d test group(s) failed\n", tests_failed);
    return 1;
  }

  puts("all tests passed");
  return 0;
}
