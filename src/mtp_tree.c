#include "mtp_tree.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

static bool parse_u32(const char *value, uint32_t *out) {
  if (value == NULL || *value == '\0') {
    return false;
  }

  char *end = NULL;
  errno = 0;
  unsigned long parsed = strtoul(value, &end, 0);
  if (errno != 0 || end == value || *end != '\0' || parsed > UINT32_MAX) {
    return false;
  }

  *out = (uint32_t)parsed;
  return true;
}

bool storage_matches(LIBMTP_devicestorage_t *storage, const char *filter) {
  if (storage == NULL || filter == NULL || *filter == '\0') {
    return false;
  }

  uint32_t parsed = 0;
  if (parse_u32(filter, &parsed) && parsed == storage->id) {
    return true;
  }

  return (storage->StorageDescription != NULL && strcasecmp(storage->StorageDescription, filter) == 0) ||
         (storage->VolumeIdentifier != NULL && strcasecmp(storage->VolumeIdentifier, filter) == 0);
}

LIBMTP_devicestorage_t *find_storage(LIBMTP_mtpdevice_t *device, const char *filter) {
  for (LIBMTP_devicestorage_t *storage = device->storage; storage != NULL; storage = storage->next) {
    if (storage_matches(storage, filter)) {
      return storage;
    }
  }
  return NULL;
}

bool parent_matches(uint32_t actual, uint32_t expected, uint32_t storage_id) {
  if (expected == ROOT_PARENT_ID) {
    return actual == ROOT_PARENT_ID || actual == LIBMTP_FILES_AND_FOLDERS_ROOT || actual == storage_id;
  }
  return actual == expected;
}

LIBMTP_folder_t *find_child_folder(LIBMTP_folder_t *folder, uint32_t storage_id, uint32_t parent_id, const char *name) {
  for (LIBMTP_folder_t *current = folder; current != NULL; current = current->sibling) {
    if (current->storage_id == storage_id &&
        parent_matches(current->parent_id, parent_id, storage_id) &&
        current->name != NULL &&
        strcmp(current->name, name) == 0) {
      return current;
    }

    LIBMTP_folder_t *found = find_child_folder(current->child, storage_id, parent_id, name);
    if (found != NULL) {
      return found;
    }
  }
  return NULL;
}

void print_child_folders(LIBMTP_folder_t *folder, uint32_t storage_id, uint32_t parent_id) {
  for (LIBMTP_folder_t *current = folder; current != NULL; current = current->sibling) {
    if (current->storage_id == storage_id &&
        parent_matches(current->parent_id, parent_id, storage_id) &&
        current->name != NULL) {
      printf("d  %-12s %s/\n", "-", current->name);
    }
    print_child_folders(current->child, storage_id, parent_id);
  }
}

void print_folder_tree_raw(LIBMTP_folder_t *folder, unsigned depth) {
  for (LIBMTP_folder_t *current = folder; current != NULL; current = current->sibling) {
    for (unsigned i = 0; i < depth; i++) {
      printf("  ");
    }
    printf("d id=0x%08" PRIx32 " parent=0x%08" PRIx32 " storage=0x%08" PRIx32 " %s\n",
           current->folder_id,
           current->parent_id,
           current->storage_id,
           current->name != NULL ? current->name : "(unnamed)");
    print_folder_tree_raw(current->child, depth + 1);
  }
}

void print_storage_list(LIBMTP_mtpdevice_t *device) {
  for (LIBMTP_devicestorage_t *storage = device->storage; storage != NULL; storage = storage->next) {
    printf("s  0x%08" PRIx32 " %s",
           storage->id,
           storage->StorageDescription != NULL ? storage->StorageDescription : "unnamed storage");
    if (storage->VolumeIdentifier != NULL && *storage->VolumeIdentifier != '\0') {
      printf(" (%s)", storage->VolumeIdentifier);
    }
    printf(" free=%" PRIu64 " capacity=%" PRIu64 "\n",
           storage->FreeSpaceInBytes,
           storage->MaxCapacity);
  }
}

static int resolve_in_storage(LIBMTP_mtpdevice_t *device,
                              LIBMTP_devicestorage_t *storage,
                              const PathParts *parts,
                              size_t start_index,
                              RemoteDir *out) {
  LIBMTP_folder_t *folder_tree = LIBMTP_Get_Folder_List_For_Storage(device, storage->id);
  uint32_t parent_id = ROOT_PARENT_ID;

  for (size_t i = start_index; i < parts->count; i++) {
    LIBMTP_folder_t *folder = find_child_folder(folder_tree, storage->id, parent_id, parts->items[i]);
    if (folder == NULL) {
      LIBMTP_destroy_folder_t(folder_tree);
      return 1;
    }
    parent_id = folder->folder_id;
  }

  out->storage = storage;
  out->parent_id = parent_id;
  out->folder_tree = folder_tree;
  return 0;
}

int resolve_directory(LIBMTP_mtpdevice_t *device, const Options *options, const PathParts *parts, RemoteDir *out) {
  memset(out, 0, sizeof(*out));

  if (options->storage_filter != NULL) {
    LIBMTP_devicestorage_t *storage = find_storage(device, options->storage_filter);
    if (storage == NULL) {
      fprintf(stderr, "storage not found: %s\n", options->storage_filter);
      return 1;
    }

    size_t start = 0;
    if (parts->count > 0 && storage_matches(storage, parts->items[0])) {
      start = 1;
    }
    if (resolve_in_storage(device, storage, parts, start, out) != 0) {
      fprintf(stderr, "directory not found on storage '%s'\n", options->storage_filter);
      return 1;
    }
    return 0;
  }

  if (parts->count > 0) {
    LIBMTP_devicestorage_t *prefixed_storage = find_storage(device, parts->items[0]);
    if (prefixed_storage != NULL) {
      if (resolve_in_storage(device, prefixed_storage, parts, 1, out) != 0) {
        fprintf(stderr, "directory not found: path below storage '%s'\n", parts->items[0]);
        return 1;
      }
      return 0;
    }
  }

  int matches = 0;
  RemoteDir candidate;
  memset(&candidate, 0, sizeof(candidate));

  for (LIBMTP_devicestorage_t *storage = device->storage; storage != NULL; storage = storage->next) {
    RemoteDir current;
    memset(&current, 0, sizeof(current));
    if (resolve_in_storage(device, storage, parts, 0, &current) == 0) {
      matches++;
      if (matches == 1) {
        candidate = current;
      } else {
        LIBMTP_destroy_folder_t(current.folder_tree);
      }
    }
  }

  if (matches == 0) {
    fprintf(stderr, "directory not found\n");
    return 1;
  }
  if (matches > 1) {
    LIBMTP_destroy_folder_t(candidate.folder_tree);
    fprintf(stderr, "directory is ambiguous across storages; pass --storage or prefix the path with a storage name\n");
    return 1;
  }

  *out = candidate;
  return 0;
}

void remote_dir_free(RemoteDir *dir) {
  if (dir == NULL) {
    return;
  }
  LIBMTP_destroy_folder_t(dir->folder_tree);
  memset(dir, 0, sizeof(*dir));
}
