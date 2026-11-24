#ifndef GARMIN_MTP_DEVICE_DEFAULTS_H
#define GARMIN_MTP_DEVICE_DEFAULTS_H

#include <stdint.h>

/*
 * Per-model defaults for devices that the installed libmtp release does not
 * know about yet. The defaults let `devices` print a useful model name and let
 * path resolution prefer the watch's internal storage when a path is not
 * otherwise qualified.
 */
typedef struct {
  uint16_t vendor_id;
  uint16_t product_id;
  const char *vendor;
  const char *model;
  uint32_t default_storage_id;
  const char *default_storage_name;
} DeviceDefaults;

const DeviceDefaults *device_defaults_lookup(uint16_t vendor_id, uint16_t product_id);

#endif
