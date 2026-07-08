#include "test.h"

#include "../src/path.h"

#include <stdlib.h>

void path_tests(void) {
  PathParts parts;
  ASSERT_TRUE(split_mtp_path("/GARMIN//APPS/FILE.PRG", &parts));
  ASSERT_EQ_SIZE(3, parts.count);
  ASSERT_STREQ("GARMIN", parts.items[0]);
  ASSERT_STREQ("APPS", parts.items[1]);
  ASSERT_STREQ("FILE.PRG", parts.items[2]);
  path_parts_free(&parts);

  ASSERT_TRUE(split_mtp_path("/", &parts));
  ASSERT_EQ_SIZE(0, parts.count);
  path_parts_free(&parts);

  int saved_stderr = silence_stderr();
  bool split_ok = split_mtp_path("/GARMIN/../APPS", &parts);
  restore_stderr(saved_stderr);
  ASSERT_FALSE(split_ok);
  path_parts_free(&parts);

  PathParts dir_parts;
  char *filename = NULL;
  ASSERT_EQ_INT(0, split_remote_file_path("/GARMIN/APPS/EXAMPLE.PRG", &dir_parts, &filename));
  ASSERT_EQ_SIZE(2, dir_parts.count);
  ASSERT_STREQ("GARMIN", dir_parts.items[0]);
  ASSERT_STREQ("APPS", dir_parts.items[1]);
  ASSERT_STREQ("EXAMPLE.PRG", filename);
  path_parts_free(&dir_parts);
  free(filename);

  saved_stderr = silence_stderr();
  int split_status = split_remote_file_path("/", &dir_parts, &filename);
  restore_stderr(saved_stderr);
  ASSERT_EQ_INT(1, split_status);

  char *joined = join_local_path("/tmp", "EXAMPLE.PRG");
  ASSERT_STREQ("/tmp/EXAMPLE.PRG", joined);
  free(joined);

  joined = join_local_path("/tmp/", "EXAMPLE.PRG");
  ASSERT_STREQ("/tmp/EXAMPLE.PRG", joined);
  free(joined);

  ASSERT_STREQ("file.fit", local_basename("/GARMIN/file.fit"));
  ASSERT_STREQ("file.fit", local_basename("file.fit"));
  ASSERT_TRUE(ends_with_slash("/tmp/"));
  ASSERT_FALSE(ends_with_slash("/tmp"));

  ASSERT_TRUE(valid_remote_filename("EXAMPLE.FIT"));
  ASSERT_FALSE(valid_remote_filename(""));
  ASSERT_FALSE(valid_remote_filename("."));
  ASSERT_FALSE(valid_remote_filename(".."));
  ASSERT_FALSE(valid_remote_filename("GARMIN/EXAMPLE.FIT"));
}
