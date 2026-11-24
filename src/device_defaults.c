#include "device_defaults.h"

#include <stddef.h>

#define GARMIN_VENDOR_ID 0x091e
#define GARMIN_INTERNAL_STORAGE_ID UINT32_C(0x00020001)
#define GARMIN_INTERNAL_STORAGE_NAME "Internal Storage"

static const DeviceDefaults device_defaults[] = {
    {
        .vendor_id = GARMIN_VENDOR_ID,
        .product_id = 0x5223,
        .vendor = "Garmin",
        .model = "Garmin Venu 4 (45mm)",
        .default_storage_id = GARMIN_INTERNAL_STORAGE_ID,
        .default_storage_name = GARMIN_INTERNAL_STORAGE_NAME,
    },
    {
        .vendor_id = GARMIN_VENDOR_ID,
        .product_id = 0x5224,
        .vendor = "Garmin",
        .model = "Garmin Venu 4 (41mm)",
        .default_storage_id = GARMIN_INTERNAL_STORAGE_ID,
        .default_storage_name = GARMIN_INTERNAL_STORAGE_NAME,
    },
};

const DeviceDefaults *device_defaults_lookup(uint16_t vendor_id, uint16_t product_id) {
  for (size_t i = 0; i < sizeof(device_defaults) / sizeof(device_defaults[0]); i++) {
    if (device_defaults[i].vendor_id == vendor_id && device_defaults[i].product_id == product_id) {
      return &device_defaults[i];
    }
  }
  return NULL;
}
