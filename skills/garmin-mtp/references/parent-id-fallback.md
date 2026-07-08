# Parent-Id Fallback For Garmin MTP

Some Garmin watches expose storage correctly while returning an empty classic libmtp folder tree. In that state, path resolution through `LIBMTP_Get_Folder_List_For_Storage()` can fail even though the same folders are visible through direct object enumeration.

Use parent-id traversal as the fallback:

1. Pick the target storage.
2. Start with root parent id `0`.
3. Call `LIBMTP_Get_Files_And_Folders(device, storage_id, parent_id)`.
4. If root returns no children, retry root with `LIBMTP_FILES_AND_FOLDERS_ROOT`.
5. Match the next path segment case-insensitively against children whose `filetype` is `LIBMTP_FILETYPE_FOLDER`.
6. Use the matched object's `item_id` as the next parent id.
7. Repeat until the directory path is resolved.

For listing, print the direct children returned for the resolved parent. For pulling or duplicate checks before push, enumerate the resolved parent directly instead of relying on the global file listing.

Keep classic folder-tree traversal as the first path because it is fast and works on many devices. Only fall back to parent-id traversal when the folder tree cannot resolve the path.
