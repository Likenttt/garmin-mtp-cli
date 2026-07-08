#include "test.h"

#include "../src/process_conflict.h"

void process_conflict_tests(void) {
  ASSERT_TRUE(process_line_is_known_mtp_conflict("123 /Applications/Android File Transfer.app/Contents/MacOS/Android File Transfer"));
  ASSERT_TRUE(process_line_is_known_mtp_conflict("456 Garmin Express Service"));
  ASSERT_TRUE(process_line_is_known_mtp_conflict("789 GarminDeviceService"));
  ASSERT_FALSE(process_line_is_known_mtp_conflict("999 /usr/bin/ssh-agent"));
  ASSERT_FALSE(process_line_is_known_mtp_conflict(""));
  ASSERT_FALSE(process_line_is_known_mtp_conflict(NULL));
}
