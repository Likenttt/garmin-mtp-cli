#ifndef GARMIN_MTP_DEVICE_H
#define GARMIN_MTP_DEVICE_H

#include "cli.h"
#include "mtp_tree.h"

#include <libmtp.h>
#include <stdint.h>

typedef struct {
  LIBMTP_mtpdevice_t *device;
  LIBMTP_raw_device_t *raw_devices;
  int raw_device_count;
} DeviceHandle;

typedef struct {
  uint32_t item_id;
  uint64_t size;
  LIBMTP_filetype_t filetype;
  char *filename;
} RemoteFile;

const char *detect_error_name(LIBMTP_error_number_t error);
void print_mtp_errors(LIBMTP_mtpdevice_t *device);
int detect_devices(LIBMTP_raw_device_t **raw_devices, int *raw_device_count, bool debug);
int open_device(const Options *options, DeviceHandle *handle);
void device_handle_close(DeviceHandle *handle);
int find_file_in_dir(LIBMTP_mtpdevice_t *device, const RemoteDir *dir, const char *filename, RemoteFile *out);
int find_file_by_full_name(LIBMTP_mtpdevice_t *device, const char *remote_path, RemoteFile *out);
void remote_file_free(RemoteFile *file);

#endif
