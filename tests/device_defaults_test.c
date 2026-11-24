#include "test.h"

#include "../src/device_defaults.h"

void device_defaults_tests(void) {
  const DeviceDefaults *venu4 = device_defaults_lookup(0x091e, 0x5223);
  ASSERT_TRUE(venu4 != NULL);
  ASSERT_STREQ("Garmin", venu4->vendor);
  ASSERT_STREQ("Garmin Venu 4 (45mm)", venu4->model);
  ASSERT_EQ_INT(0x00020001, (int)venu4->default_storage_id);
  ASSERT_STREQ("Internal Storage", venu4->default_storage_name);

  const DeviceDefaults *venu4_small = device_defaults_lookup(0x091e, 0x5224);
  ASSERT_TRUE(venu4_small != NULL);
  ASSERT_STREQ("Garmin Venu 4 (41mm)", venu4_small->model);

  ASSERT_TRUE(device_defaults_lookup(0x091e, 0x5222) == NULL);
  ASSERT_TRUE(device_defaults_lookup(0x1234, 0x5223) == NULL);
  ASSERT_TRUE(device_defaults_lookup(0x0000, 0x0000) == NULL);
}
