#ifndef GARMIN_MTP_TEST_H
#define GARMIN_MTP_TEST_H

#include <stdio.h>
#include <string.h>

extern int tests_failed;

int silence_stderr(void);
void restore_stderr(int saved_fd);

#define ASSERT_TRUE(expr)                                                                                              \
  do {                                                                                                                 \
    if (!(expr)) {                                                                                                     \
      fprintf(stderr, "%s:%d: assertion failed: %s\n", __FILE__, __LINE__, #expr);                                    \
      tests_failed++;                                                                                                  \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (0)

#define ASSERT_FALSE(expr) ASSERT_TRUE(!(expr))

#define ASSERT_EQ_INT(expected, actual)                                                                                 \
  do {                                                                                                                 \
    int test_expected = (expected);                                                                                     \
    int test_actual = (actual);                                                                                         \
    if (test_expected != test_actual) {                                                                                 \
      fprintf(stderr, "%s:%d: expected %d, got %d\n", __FILE__, __LINE__, test_expected, test_actual);                 \
      tests_failed++;                                                                                                  \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (0)

#define ASSERT_EQ_SIZE(expected, actual)                                                                                \
  do {                                                                                                                 \
    size_t test_expected = (expected);                                                                                  \
    size_t test_actual = (actual);                                                                                      \
    if (test_expected != test_actual) {                                                                                 \
      fprintf(stderr, "%s:%d: expected %zu, got %zu\n", __FILE__, __LINE__, test_expected, test_actual);               \
      tests_failed++;                                                                                                  \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (0)

#define ASSERT_EQ_PTR(expected, actual)                                                                                 \
  do {                                                                                                                 \
    const void *test_expected = (expected);                                                                             \
    const void *test_actual = (actual);                                                                                 \
    if (test_expected != test_actual) {                                                                                 \
      fprintf(stderr, "%s:%d: expected pointer %p, got %p\n", __FILE__, __LINE__, test_expected, test_actual);         \
      tests_failed++;                                                                                                  \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (0)

#define ASSERT_STREQ(expected, actual)                                                                                  \
  do {                                                                                                                 \
    const char *test_expected = (expected);                                                                             \
    const char *test_actual = (actual);                                                                                 \
    if (strcmp(test_expected, test_actual) != 0) {                                                                      \
      fprintf(stderr, "%s:%d: expected \"%s\", got \"%s\"\n", __FILE__, __LINE__, test_expected, test_actual);        \
      tests_failed++;                                                                                                  \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (0)

#endif
