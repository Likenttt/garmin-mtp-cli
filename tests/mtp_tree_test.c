#include "test.h"

#include "../src/mtp_tree.h"

void mtp_tree_tests(void) {
  LIBMTP_devicestorage_t storage = {0};
  storage.id = 0x00020001;
  storage.StorageDescription = "Internal Storage";
  storage.VolumeIdentifier = "GARMINVOL";

  ASSERT_TRUE(storage_matches(&storage, "Internal Storage"));
  ASSERT_TRUE(storage_matches(&storage, "internal storage"));
  ASSERT_TRUE(storage_matches(&storage, "GARMINVOL"));
  ASSERT_TRUE(storage_matches(&storage, "0x00020001"));
  ASSERT_TRUE(storage_matches(&storage, "131073"));
  ASSERT_FALSE(storage_matches(&storage, "External Storage"));
  ASSERT_FALSE(storage_matches(NULL, "Internal Storage"));

  ASSERT_TRUE(parent_matches(ROOT_PARENT_ID, ROOT_PARENT_ID, storage.id));
  ASSERT_TRUE(parent_matches(LIBMTP_FILES_AND_FOLDERS_ROOT, ROOT_PARENT_ID, storage.id));
  ASSERT_TRUE(parent_matches(storage.id, ROOT_PARENT_ID, storage.id));
  ASSERT_TRUE(parent_matches(42, 42, storage.id));
  ASSERT_FALSE(parent_matches(42, 43, storage.id));

  LIBMTP_folder_t apps = {0};
  apps.folder_id = 2;
  apps.parent_id = 1;
  apps.storage_id = storage.id;
  apps.name = "APPS";

  LIBMTP_folder_t garmin = {0};
  garmin.folder_id = 1;
  garmin.parent_id = ROOT_PARENT_ID;
  garmin.storage_id = storage.id;
  garmin.name = "GARMIN";
  garmin.child = &apps;

  ASSERT_EQ_PTR(&garmin, find_child_folder(&garmin, storage.id, ROOT_PARENT_ID, "GARMIN"));
  ASSERT_EQ_PTR(&apps, find_child_folder(&garmin, storage.id, 1, "APPS"));
  ASSERT_TRUE(find_child_folder(&garmin, storage.id, ROOT_PARENT_ID, "APPS") == NULL);
  ASSERT_TRUE(find_child_folder(&garmin, 0x999, ROOT_PARENT_ID, "GARMIN") == NULL);

  LIBMTP_devicestorage_t external = {0};
  external.id = 0x00030001;
  external.StorageDescription = "External Storage";
  storage.next = &external;

  LIBMTP_mtpdevice_t device = {0};
  device.storage = &storage;

  DeviceDefaults defaults = {0};
  defaults.default_storage_id = 0x00020001;
  defaults.default_storage_name = "Internal Storage";
  ASSERT_EQ_PTR(&storage, find_default_storage(&device, &defaults));

  DeviceDefaults by_name = {0};
  by_name.default_storage_name = "External Storage";
  ASSERT_EQ_PTR(&external, find_default_storage(&device, &by_name));

  DeviceDefaults no_match = {0};
  no_match.default_storage_id = 0x00040001;
  no_match.default_storage_name = "Missing Storage";
  ASSERT_TRUE(find_default_storage(&device, &no_match) == NULL);
  ASSERT_TRUE(find_default_storage(&device, NULL) == NULL);
  ASSERT_TRUE(find_default_storage(NULL, &defaults) == NULL);
}
