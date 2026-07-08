#define _POSIX_C_SOURCE 200809L

#include "mtp_device.h"

#include "memory.h"
#include "process_conflict.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *detect_error_name(LIBMTP_error_number_t error) {
  switch (error) {
  case LIBMTP_ERROR_NONE:
    return "no error";
  case LIBMTP_ERROR_GENERAL:
    return "general error";
  case LIBMTP_ERROR_PTP_LAYER:
    return "PTP layer error";
  case LIBMTP_ERROR_USB_LAYER:
    return "USB layer error";
  case LIBMTP_ERROR_MEMORY_ALLOCATION:
    return "memory allocation error";
  case LIBMTP_ERROR_NO_DEVICE_ATTACHED:
    return "no MTP device attached";
  case LIBMTP_ERROR_STORAGE_FULL:
    return "storage full";
  case LIBMTP_ERROR_CONNECTING:
    return "device connection error";
  case LIBMTP_ERROR_CANCELLED:
    return "operation cancelled";
  default:
    return "unknown error";
  }
}

void print_mtp_errors(LIBMTP_mtpdevice_t *device) {
  if (device == NULL) {
    return;
  }

  LIBMTP_error_t *error = LIBMTP_Get_Errorstack(device);
  for (LIBMTP_error_t *current = error; current != NULL; current = current->next) {
    fprintf(stderr, "libmtp: %s\n", current->error_text != NULL ? current->error_text : "unknown error");
  }
  LIBMTP_Clear_Errorstack(device);
}

int detect_devices(LIBMTP_raw_device_t **raw_devices, int *raw_device_count, bool debug) {
  LIBMTP_Init();
  if (debug) {
    LIBMTP_Set_Debug(LIBMTP_DEBUG_ALL);
  }

  LIBMTP_error_number_t detect = LIBMTP_Detect_Raw_Devices(raw_devices, raw_device_count);
  if (detect == LIBMTP_ERROR_NONE) {
    return 0;
  }
  if (detect == LIBMTP_ERROR_NO_DEVICE_ATTACHED) {
    *raw_devices = NULL;
    *raw_device_count = 0;
    return 0;
  }

  fprintf(stderr, "failed to detect MTP devices: %s\n", detect_error_name(detect));
  return 1;
}

int open_device(const Options *options, DeviceHandle *handle) {
  memset(handle, 0, sizeof(*handle));

  int status = detect_devices(&handle->raw_devices, &handle->raw_device_count, options->debug);
  if (status != 0) {
    return status;
  }

  if (handle->raw_device_count == 0) {
    fprintf(stderr, "no MTP devices detected\n");
    if (print_running_mtp_conflicts(stderr)) {
      fprintf(stderr,
              "If your Garmin watch is connected, close the process above, unplug/replug the watch, and retry.\n");
    }
    return 1;
  }
  if (options->device_index < 0 || options->device_index >= handle->raw_device_count) {
    fprintf(stderr,
            "device index %d is out of range; %d device(s) detected\n",
            options->device_index,
            handle->raw_device_count);
    return 1;
  }

  handle->device = LIBMTP_Open_Raw_Device_Uncached(&handle->raw_devices[options->device_index]);
  if (handle->device == NULL) {
    fprintf(stderr, "failed to open MTP device %d\n", options->device_index);
    print_mtp_conflict_hint(stderr);
    return 1;
  }

  if (LIBMTP_Get_Storage(handle->device, LIBMTP_STORAGE_SORTBY_NOTSORTED) != 0) {
    fprintf(stderr, "failed to query device storage\n");
    print_mtp_errors(handle->device);
    return 1;
  }

  return 0;
}

void device_handle_close(DeviceHandle *handle) {
  if (handle == NULL) {
    return;
  }
  if (handle->device != NULL) {
    LIBMTP_Release_Device(handle->device);
  }
  free(handle->raw_devices);
  memset(handle, 0, sizeof(*handle));
}

void remote_file_free(RemoteFile *file) {
  if (file == NULL) {
    return;
  }
  free(file->filename);
  memset(file, 0, sizeof(*file));
}

int find_file_in_dir(LIBMTP_mtpdevice_t *device, const RemoteDir *dir, const char *filename, RemoteFile *out) {
  memset(out, 0, sizeof(*out));

  LIBMTP_file_t *files = dir->uses_object_listing
      ? LIBMTP_Get_Files_And_Folders(device, dir->storage->id, dir->parent_id)
      : LIBMTP_Get_Filelisting_With_Callback(device, NULL, NULL);
  if (files == NULL && dir->uses_object_listing && dir->parent_id == ROOT_PARENT_ID) {
    files = LIBMTP_Get_Files_And_Folders(device, dir->storage->id, LIBMTP_FILES_AND_FOLDERS_ROOT);
  }
  if (files == NULL) {
    print_mtp_errors(device);
    return 1;
  }

  int matches = 0;
  for (LIBMTP_file_t *file = files; file != NULL; file = file->next) {
    if (file->storage_id == dir->storage->id &&
        parent_matches(file->parent_id, dir->parent_id, dir->storage->id) &&
        file->filename != NULL &&
        strcmp(file->filename, filename) == 0) {
      matches++;
      if (matches == 1) {
        out->item_id = file->item_id;
        out->size = file->filesize;
        out->filetype = file->filetype;
        out->filename = xstrdup(file->filename);
      }
    }
  }

  LIBMTP_destroy_file_t(files);

  if (matches == 0) {
    return 1;
  }
  if (matches > 1) {
    remote_file_free(out);
    fprintf(stderr, "multiple files named '%s' were found in the same MTP directory\n", filename);
    return 2;
  }
  return 0;
}

int find_file_by_full_name(LIBMTP_mtpdevice_t *device, const char *remote_path, RemoteFile *out) {
  memset(out, 0, sizeof(*out));

  const char *without_leading_slash = remote_path;
  while (*without_leading_slash == '/') {
    without_leading_slash++;
  }

  LIBMTP_file_t *files = LIBMTP_Get_Filelisting_With_Callback(device, NULL, NULL);
  if (files == NULL) {
    print_mtp_errors(device);
    return 1;
  }

  int matches = 0;
  for (LIBMTP_file_t *file = files; file != NULL; file = file->next) {
    if (file->filename == NULL) {
      continue;
    }
    if (strcmp(file->filename, remote_path) == 0 || strcmp(file->filename, without_leading_slash) == 0) {
      matches++;
      if (matches == 1) {
        out->item_id = file->item_id;
        out->size = file->filesize;
        out->filetype = file->filetype;
        out->filename = xstrdup(file->filename);
      }
    }
  }

  LIBMTP_destroy_file_t(files);

  if (matches == 0) {
    return 1;
  }
  if (matches > 1) {
    remote_file_free(out);
    fprintf(stderr, "multiple files matched full MTP name '%s'\n", remote_path);
    return 2;
  }
  return 0;
}
