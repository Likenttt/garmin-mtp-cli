#include "test.h"

#include "../src/manual.h"

#include <string.h>

void manual_tests(void) {
  const char *text = manual_text();

  ASSERT_TRUE(strstr(text, "/RunningTrack") != NULL);
  ASSERT_TRUE(strstr(text, "/Records") != NULL);
  ASSERT_TRUE(strstr(text, "/Metrics") != NULL);
  ASSERT_TRUE(strstr(text, "/Monitor") != NULL);
  ASSERT_TRUE(strstr(text, "/NewFiles") != NULL);
  ASSERT_TRUE(strstr(text, "/RemoteSW") != NULL);
  ASSERT_TRUE(strstr(text, "/*.img") != NULL);
  ASSERT_TRUE(strstr(text, "multiple gigabytes") != NULL);
  ASSERT_TRUE(strstr(text, "very long") != NULL);
  ASSERT_TRUE(strstr(text, "garmin-mtp menu") != NULL);
}
