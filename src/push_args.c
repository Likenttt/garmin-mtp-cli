#include "push_args.h"

#include <string.h>

int parse_push_args(int argc, char **argv, PushArgs *out, FILE *err) {
  *out = (PushArgs){
      .overwrite = false,
      .local_path = NULL,
      .remote_dir_path = NULL,
      .remote_name = NULL,
  };

  int index = 0;
  if (argc > 0 && strcmp(argv[0], "--overwrite") == 0) {
    out->overwrite = true;
    index = 1;
  } else if (argc > 0 && strncmp(argv[0], "-", 1) == 0) {
    fprintf(err, "unknown push option: %s\n", argv[0]);
    return 1;
  }

  int remaining = argc - index;
  if (remaining != 2 && remaining != 3) {
    return 1;
  }

  out->local_path = argv[index];
  out->remote_dir_path = argv[index + 1];
  out->remote_name = remaining == 3 ? argv[index + 2] : NULL;
  return 0;
}
