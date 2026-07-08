#include "test.h"

#include "../src/cli.h"

void cli_tests(void) {
  Options options;
  int command_index = -1;

  char *basic[] = {"garmin-mtp", "list", "/"};
  ASSERT_EQ_INT(CLI_PARSE_OK, parse_options(3, basic, &options, &command_index));
  ASSERT_EQ_INT(0, options.device_index);
  ASSERT_TRUE(options.storage_filter == NULL);
  ASSERT_FALSE(options.debug);
  ASSERT_FALSE(options.quiet_progress);
  ASSERT_EQ_INT(1, command_index);

  char *alias_name[] = {"garminmtp", "manual"};
  ASSERT_EQ_INT(CLI_PARSE_OK, parse_options(2, alias_name, &options, &command_index));
  ASSERT_EQ_INT(1, command_index);

  char *configured[] = {
      "garmin-mtp",
      "--device-index",
      "2",
      "--storage",
      "Internal Storage",
      "--debug",
      "--quiet-progress",
      "pull",
      "/GARMIN/A",
      "./A",
  };
  ASSERT_EQ_INT(CLI_PARSE_OK, parse_options(10, configured, &options, &command_index));
  ASSERT_EQ_INT(2, options.device_index);
  ASSERT_STREQ("Internal Storage", options.storage_filter);
  ASSERT_TRUE(options.debug);
  ASSERT_TRUE(options.quiet_progress);
  ASSERT_EQ_INT(7, command_index);

  char *help[] = {"garmin-mtp", "--help"};
  ASSERT_EQ_INT(CLI_PARSE_HELP, parse_options(2, help, &options, &command_index));

  char *bad_device_index[] = {"garmin-mtp", "--device-index", "abc", "list"};
  int saved_stderr = silence_stderr();
  CliParseResult parse = parse_options(4, bad_device_index, &options, &command_index);
  restore_stderr(saved_stderr);
  ASSERT_EQ_INT(CLI_PARSE_ERROR, parse);

  char *unknown[] = {"garmin-mtp", "--unknown", "list"};
  saved_stderr = silence_stderr();
  parse = parse_options(3, unknown, &options, &command_index);
  restore_stderr(saved_stderr);
  ASSERT_EQ_INT(CLI_PARSE_ERROR, parse);
}
