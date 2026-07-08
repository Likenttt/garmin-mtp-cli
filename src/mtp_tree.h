#ifndef GARMIN_MTP_TREE_H
#define GARMIN_MTP_TREE_H

#include "cli.h"
#include "path.h"

#include <libmtp.h>
#include <stdbool.h>
#include <stdint.h>

#define ROOT_PARENT_ID 0u

typedef struct {
  LIBMTP_devicestorage_t *storage;
  uint32_t parent_id;
  LIBMTP_folder_t *folder_tree;
  LIBMTP_file_t *object_list;
} RemoteDir;

bool storage_matches(LIBMTP_devicestorage_t *storage, const char *filter);
LIBMTP_devicestorage_t *find_storage(LIBMTP_mtpdevice_t *device, const char *filter);
bool parent_matches(uint32_t actual, uint32_t expected, uint32_t storage_id);
LIBMTP_folder_t *find_child_folder(LIBMTP_folder_t *folder, uint32_t storage_id, uint32_t parent_id, const char *name);
LIBMTP_file_t *find_child_object(LIBMTP_file_t *objects, uint32_t storage_id, uint32_t parent_id, const char *name);
void print_child_folders(LIBMTP_folder_t *folder, uint32_t storage_id, uint32_t parent_id);
void print_child_objects(LIBMTP_file_t *objects, uint32_t storage_id, uint32_t parent_id);
void print_folder_tree_raw(LIBMTP_folder_t *folder, unsigned depth);
void print_storage_list(LIBMTP_mtpdevice_t *device);
int resolve_directory(LIBMTP_mtpdevice_t *device, const Options *options, const PathParts *parts, RemoteDir *out);
void remote_dir_free(RemoteDir *dir);

#endif
