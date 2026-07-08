#include "test.h"

#include "../src/push_args.h"

void push_args_tests(void) {
  PushArgs args;

  char *basic[] = {"photo.fit", "/NewFiles"};
  ASSERT_EQ_INT(0, parse_push_args(2, basic, &args, stderr));
  ASSERT_FALSE(args.overwrite);
  ASSERT_STREQ("photo.fit", args.local_path);
  ASSERT_STREQ("/NewFiles", args.remote_dir_path);
  ASSERT_TRUE(args.remote_name == NULL);

  char *renamed[] = {"photo.fit", "/NewFiles", "remote.fit"};
  ASSERT_EQ_INT(0, parse_push_args(3, renamed, &args, stderr));
  ASSERT_FALSE(args.overwrite);
  ASSERT_STREQ("remote.fit", args.remote_name);

  char *overwrite[] = {"--overwrite", "photo.fit", "/NewFiles", "remote.fit"};
  ASSERT_EQ_INT(0, parse_push_args(4, overwrite, &args, stderr));
  ASSERT_TRUE(args.overwrite);
  ASSERT_STREQ("photo.fit", args.local_path);
  ASSERT_STREQ("/NewFiles", args.remote_dir_path);
  ASSERT_STREQ("remote.fit", args.remote_name);

  char *unknown[] = {"--replace", "photo.fit", "/NewFiles"};
  int saved_stderr = silence_stderr();
  int status = parse_push_args(3, unknown, &args, stderr);
  restore_stderr(saved_stderr);
  ASSERT_EQ_INT(1, status);

  char *missing[] = {"--overwrite", "photo.fit"};
  ASSERT_EQ_INT(1, parse_push_args(2, missing, &args, stderr));
}
