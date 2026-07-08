#include "commands.h"

#include "file_type.h"
#include "memory.h"
#include "mtp_device.h"
#include "mtp_tree.h"
#include "path.h"
#include "progress.h"
#include "push_args.h"

#include <errno.h>
#include <inttypes.h>
#include <libmtp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static bool is_existing_dir(const char *path) {
  struct stat st;
  return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

int command_devices(const Options *options) {
  LIBMTP_raw_device_t *raw_devices = NULL;
  int raw_device_count = 0;

  int status = detect_devices(&raw_devices, &raw_device_count, options->debug);
  if (status != 0) {
    return status;
  }

  if (raw_device_count == 0) {
    printf("No MTP devices detected.\n");
    free(raw_devices);
    return 0;
  }

  for (int i = 0; i < raw_device_count; i++) {
    LIBMTP_raw_device_t *raw = &raw_devices[i];
    printf("[%d] %s %s vendor=0x%04x product=0x%04x bus=%u dev=%u\n",
           i,
           raw->device_entry.vendor != NULL ? raw->device_entry.vendor : "unknown-vendor",
           raw->device_entry.product != NULL ? raw->device_entry.product : "unknown-product",
           raw->device_entry.vendor_id,
           raw->device_entry.product_id,
           raw->bus_location,
           raw->devnum);
  }

  free(raw_devices);
  return 0;
}

int command_dump(const Options *options) {
  DeviceHandle handle;
  int status = open_device(options, &handle);
  if (status != 0) {
    device_handle_close(&handle);
    return status;
  }

  puts("Storages:");
  print_storage_list(handle.device);

  puts("\nFolders:");
  bool printed_folder = false;
  for (LIBMTP_devicestorage_t *storage = handle.device->storage; storage != NULL; storage = storage->next) {
    LIBMTP_folder_t *folders = LIBMTP_Get_Folder_List_For_Storage(handle.device, storage->id);
    if (folders != NULL) {
      printed_folder = true;
      print_folder_tree_raw(folders, 0);
    }
    LIBMTP_destroy_folder_t(folders);
  }
  if (!printed_folder) {
    puts("(none)");
  }

  puts("\nFiles:");
  LIBMTP_file_t *files = LIBMTP_Get_Filelisting_With_Callback(handle.device, NULL, NULL);
  if (files == NULL) {
    print_mtp_errors(handle.device);
    puts("(none)");
  }
  for (LIBMTP_file_t *file = files; file != NULL; file = file->next) {
    printf("f id=0x%08" PRIx32 " parent=0x%08" PRIx32 " storage=0x%08" PRIx32 " size=%" PRIu64 " type=%d %s\n",
           file->item_id,
           file->parent_id,
           file->storage_id,
           file->filesize,
           (int)file->filetype,
           file->filename != NULL ? file->filename : "(unnamed)");
  }
  LIBMTP_destroy_file_t(files);

  device_handle_close(&handle);
  return 0;
}

int command_list(const Options *options, int argc, char **argv) {
  const char *remote_path = argc > 0 ? argv[0] : "/";
  DeviceHandle handle;
  int status = open_device(options, &handle);
  if (status != 0) {
    device_handle_close(&handle);
    return status;
  }

  PathParts parts;
  if (!split_mtp_path(remote_path, &parts)) {
    device_handle_close(&handle);
    return 1;
  }

  if (parts.count == 0 && options->storage_filter == NULL) {
    print_storage_list(handle.device);
    path_parts_free(&parts);
    device_handle_close(&handle);
    return 0;
  }

  RemoteDir dir;
  status = resolve_directory(handle.device, options, &parts, &dir);
  if (status != 0) {
    path_parts_free(&parts);
    device_handle_close(&handle);
    return status;
  }

  printf("Storage: %s (0x%08" PRIx32 ")\n",
         dir.storage->StorageDescription != NULL ? dir.storage->StorageDescription : "unnamed storage",
         dir.storage->id);
  print_child_folders(dir.folder_tree, dir.storage->id, dir.parent_id);

  LIBMTP_file_t *files = LIBMTP_Get_Filelisting_With_Callback(handle.device, NULL, NULL);
  if (files == NULL) {
    print_mtp_errors(handle.device);
  }
  for (LIBMTP_file_t *file = files; file != NULL; file = file->next) {
    if (file->storage_id == dir.storage->id &&
        parent_matches(file->parent_id, dir.parent_id, dir.storage->id) &&
        file->filename != NULL) {
      printf("f  %-12" PRIu64 " %s\n", file->filesize, file->filename);
    }
  }
  LIBMTP_destroy_file_t(files);

  remote_dir_free(&dir);
  path_parts_free(&parts);
  device_handle_close(&handle);
  return 0;
}

static int pull_file_by_id(const Options *options,
                           LIBMTP_mtpdevice_t *device,
                           uint32_t item_id,
                           uint64_t size,
                           const char *remote_path,
                           const char *local_path) {
  ProgressState progress = {.quiet = options->quiet_progress, .last_percent = UINT32_MAX};
  if (LIBMTP_Get_File_To_File(device, item_id, local_path, progress_callback, &progress) != 0) {
    fprintf(stderr, "failed to pull '%s' to '%s'\n", remote_path, local_path);
    print_mtp_errors(device);
    return 1;
  }

  printf("Pulled %s -> %s (%" PRIu64 " bytes)\n", remote_path, local_path, size);
  return 0;
}

int command_pull(const Options *options, int argc, char **argv) {
  if (argc != 2) {
    usage(stderr);
    return 1;
  }

  const char *remote_path = argv[0];
  const char *local_target = argv[1];

  PathParts dir_parts;
  char *remote_filename = NULL;
  if (split_remote_file_path(remote_path, &dir_parts, &remote_filename) != 0) {
    return 1;
  }

  DeviceHandle handle;
  int status = open_device(options, &handle);
  if (status != 0) {
    path_parts_free(&dir_parts);
    free(remote_filename);
    device_handle_close(&handle);
    return status;
  }

  RemoteDir dir;
  status = resolve_directory(handle.device, options, &dir_parts, &dir);
  if (status != 0) {
    RemoteFile full_name_file;
    int full_name_status = find_file_by_full_name(handle.device, remote_path, &full_name_file);
    if (full_name_status == 0) {
      char *local_path = NULL;
      if (is_existing_dir(local_target)) {
        local_path = join_local_path(local_target, remote_filename);
      } else if (ends_with_slash(local_target)) {
        fprintf(stderr, "local target directory does not exist: %s\n", local_target);
        remote_file_free(&full_name_file);
        path_parts_free(&dir_parts);
        free(remote_filename);
        device_handle_close(&handle);
        return 1;
      } else {
        local_path = xstrdup(local_target);
      }

      status = pull_file_by_id(options, handle.device, full_name_file.item_id, full_name_file.size, remote_path, local_path);
      free(local_path);
      remote_file_free(&full_name_file);
      path_parts_free(&dir_parts);
      free(remote_filename);
      device_handle_close(&handle);
      return status;
    }

    path_parts_free(&dir_parts);
    free(remote_filename);
    device_handle_close(&handle);
    return full_name_status == 2 ? 1 : status;
  }

  RemoteFile remote_file;
  status = find_file_in_dir(handle.device, &dir, remote_filename, &remote_file);
  if (status != 0) {
    if (status == 1) {
      fprintf(stderr, "remote file not found: %s\n", remote_filename);
    }
    remote_dir_free(&dir);
    path_parts_free(&dir_parts);
    free(remote_filename);
    device_handle_close(&handle);
    return 1;
  }

  char *local_path = NULL;
  if (is_existing_dir(local_target)) {
    local_path = join_local_path(local_target, remote_file.filename);
  } else if (ends_with_slash(local_target)) {
    fprintf(stderr, "local target directory does not exist: %s\n", local_target);
    remote_file_free(&remote_file);
    remote_dir_free(&dir);
    path_parts_free(&dir_parts);
    free(remote_filename);
    device_handle_close(&handle);
    return 1;
  } else {
    local_path = xstrdup(local_target);
  }

  status = pull_file_by_id(options, handle.device, remote_file.item_id, remote_file.size, remote_path, local_path);

  free(local_path);
  remote_file_free(&remote_file);
  remote_dir_free(&dir);
  path_parts_free(&dir_parts);
  free(remote_filename);
  device_handle_close(&handle);
  return status;
}

int command_push(const Options *options, int argc, char **argv) {
  PushArgs push_args;
  if (parse_push_args(argc, argv, &push_args, stderr) != 0) {
    usage(stderr);
    return 1;
  }

  const char *local_path = push_args.local_path;
  const char *remote_dir_path = push_args.remote_dir_path;
  const char *remote_name = push_args.remote_name != NULL ? push_args.remote_name : local_basename(local_path);

  if (!valid_remote_filename(remote_name)) {
    fprintf(stderr, "invalid remote filename: %s\n", remote_name != NULL ? remote_name : "(null)");
    return 1;
  }

  struct stat st;
  if (stat(local_path, &st) != 0) {
    fprintf(stderr, "cannot stat local file '%s': %s\n", local_path, strerror(errno));
    return 1;
  }
  if (!S_ISREG(st.st_mode)) {
    fprintf(stderr, "local path is not a regular file: %s\n", local_path);
    return 1;
  }

  PathParts dir_parts;
  if (!split_mtp_path(remote_dir_path, &dir_parts)) {
    return 1;
  }

  DeviceHandle handle;
  int status = open_device(options, &handle);
  if (status != 0) {
    path_parts_free(&dir_parts);
    device_handle_close(&handle);
    return status;
  }

  RemoteDir dir;
  status = resolve_directory(handle.device, options, &dir_parts, &dir);
  if (status != 0) {
    path_parts_free(&dir_parts);
    device_handle_close(&handle);
    return status;
  }

  RemoteFile existing_file;
  int exists = find_file_in_dir(handle.device, &dir, remote_name, &existing_file);
  if (exists == 0) {
    if (!push_args.overwrite) {
      fprintf(stderr, "remote file already exists: %s; pass --overwrite to replace it\n", remote_name);
      remote_file_free(&existing_file);
      remote_dir_free(&dir);
      path_parts_free(&dir_parts);
      device_handle_close(&handle);
      return 1;
    }

    if (LIBMTP_Delete_Object(handle.device, existing_file.item_id) != 0) {
      fprintf(stderr, "failed to delete existing remote file before overwrite: %s\n", remote_name);
      print_mtp_errors(handle.device);
      remote_file_free(&existing_file);
      remote_dir_free(&dir);
      path_parts_free(&dir_parts);
      device_handle_close(&handle);
      return 1;
    }
    printf("Deleted existing remote file before overwrite: %s\n", remote_name);
    remote_file_free(&existing_file);
  }
  if (exists == 2) {
    remote_dir_free(&dir);
    path_parts_free(&dir_parts);
    device_handle_close(&handle);
    return 1;
  }

  LIBMTP_file_t *filedata = LIBMTP_new_file_t();
  if (filedata == NULL) {
    fprintf(stderr, "failed to allocate MTP file metadata\n");
    remote_dir_free(&dir);
    path_parts_free(&dir_parts);
    device_handle_close(&handle);
    return 1;
  }

  filedata->filename = xstrdup(remote_name);
  filedata->parent_id = dir.parent_id;
  filedata->storage_id = dir.storage->id;
  filedata->filesize = (uint64_t)st.st_size;
  filedata->modificationdate = st.st_mtime;
  filedata->filetype = filetype_from_name(remote_name);

  ProgressState progress = {.quiet = options->quiet_progress, .last_percent = UINT32_MAX};
  if (LIBMTP_Send_File_From_File(handle.device, local_path, filedata, progress_callback, &progress) != 0) {
    fprintf(stderr, "failed to push '%s' to '%s/%s'\n", local_path, remote_dir_path, remote_name);
    print_mtp_errors(handle.device);
    status = 1;
  } else {
    printf("Pushed %s -> %s/%s (%" PRIu64 " bytes)\n",
           local_path,
           remote_dir_path,
           remote_name,
           (uint64_t)st.st_size);
    status = 0;
  }

  LIBMTP_destroy_file_t(filedata);
  remote_dir_free(&dir);
  path_parts_free(&dir_parts);
  device_handle_close(&handle);
  return status;
}
