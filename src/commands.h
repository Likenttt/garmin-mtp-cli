#ifndef GARMIN_MTP_COMMANDS_H
#define GARMIN_MTP_COMMANDS_H

#include "cli.h"

int command_devices(const Options *options);
int command_dump(const Options *options);
int command_list(const Options *options, int argc, char **argv);
int command_pull(const Options *options, int argc, char **argv);
int command_push(const Options *options, int argc, char **argv);

#endif
