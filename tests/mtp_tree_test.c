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

  LIBMTP_file_t logs_object = {0};
  logs_object.item_id = 3;
  logs_object.parent_id = 2;
  logs_object.storage_id = storage.id;
  logs_object.filename = "LOGS";

  LIBMTP_file_t apps_object = {0};
  apps_object.item_id = 2;
  apps_object.parent_id = 1;
  apps_object.storage_id = storage.id;
  apps_object.filename = "Apps";
  apps_object.next = &logs_object;

  LIBMTP_file_t garmin_object = {0};
  garmin_object.item_id = 1;
  garmin_object.parent_id = ROOT_PARENT_ID;
  garmin_object.storage_id = storage.id;
  garmin_object.filename = "GARMIN";
  garmin_object.next = &apps_object;

  ASSERT_EQ_PTR(&garmin_object, find_child_object(&garmin_object, storage.id, ROOT_PARENT_ID, "GARMIN"));
  ASSERT_EQ_PTR(&apps_object, find_child_object(&garmin_object, storage.id, 1, "Apps"));
  ASSERT_EQ_PTR(&logs_object, find_child_object(&garmin_object, storage.id, 2, "LOGS"));
  ASSERT_TRUE(find_child_object(&garmin_object, storage.id, ROOT_PARENT_ID, "LOGS") == NULL);
}
