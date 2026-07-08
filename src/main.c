#include "cli.h"
#include "commands.h"
#include "manual.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  Options options;
  int command_index = 0;
  CliParseResult parse = parse_options(argc, argv, &options, &command_index);
  if (parse == CLI_PARSE_HELP) {
    usage(stdout);
    return 0;
  }
  if (parse == CLI_PARSE_ERROR) {
    usage(stderr);
    return 1;
  }

  if (command_index >= argc) {
    usage(stderr);
    return 1;
  }

  const char *command = argv[command_index];
  int command_argc = argc - command_index - 1;
  char **command_argv = argv + command_index + 1;

  if (strcmp(command, "devices") == 0) {
    if (command_argc != 0) {
      usage(stderr);
      return 1;
    }
    return command_devices(&options);
  }
  if (strcmp(command, "dump") == 0) {
    if (command_argc != 0) {
      usage(stderr);
      return 1;
    }
    return command_dump(&options);
  }
  if (strcmp(command, "manual") == 0 || strcmp(command, "menu") == 0) {
    if (command_argc != 0) {
      usage(stderr);
      return 1;
    }
    print_manual(stdout);
    return 0;
  }
  if (strcmp(command, "list") == 0) {
    if (command_argc > 1) {
      usage(stderr);
      return 1;
    }
    return command_list(&options, command_argc, command_argv);
  }
  if (strcmp(command, "pull") == 0) {
    return command_pull(&options, command_argc, command_argv);
  }
  if (strcmp(command, "push") == 0) {
    return command_push(&options, command_argc, command_argv);
  }

  fprintf(stderr, "unknown command: %s\n", command);
  usage(stderr);
  return 1;
}
