---
name: garmin-mtp
description: "Use when working with Garmin watches over MTP using the garmin-mtp CLI: listing watch files, pulling CIQ logs or FIT files, pushing files to Garmin import directories, diagnosing Android File Transfer or Garmin Express conflicts, handling storage/path selection, or explaining Garmin MTP directory behavior."
---

# Garmin MTP

Use the `garmin-mtp` command for Garmin watch file transfer over MTP. The tool reads the MTP object tree directly; it does not mount the watch as a normal filesystem and cannot bypass Garmin secure storage rules.

## First Checks

Run MTP commands serially against one watch. Do not parallelize `garmin-mtp` calls; libmtp needs exclusive access to the USB/MTP interface.

Prefer the installed command:

```sh
which garmin-mtp
garmin-mtp devices
```

If it is missing on macOS:

```sh
brew tap Likenttt/tap
brew install garmin-mtp
```

If opening the device fails, check for competing MTP apps:

```sh
ps -axo pid,command | rg 'Android File Transfer|Garmin Express|Garmin Core Service|MTP'
```

Ask the user to close Android File Transfer, Android File Transfer Agent, Garmin Express, or Garmin Express Service, then unplug and reconnect the watch before retrying.

## Listing And Pulling

Start from storage and then list the target directory:

```sh
garmin-mtp list /
garmin-mtp list /GARMIN/Apps/LOGS
```

If `list /` sees storage but a Garmin path reports `directory not found`, retry after updating to `garmin-mtp` 0.1.3 or newer. That version falls back to parent-id object traversal for watches that expose storage but return an empty classic folder tree.

Garmin paths are case-sensitive on some devices. On Forerunner-class devices, app logs commonly live at `/GARMIN/Apps/LOGS`, not `/GARMIN/APPS/LOG`.

Pull CIQ logs into a local `logs` directory:

```sh
mkdir -p logs
garmin-mtp pull /GARMIN/Apps/LOGS/CIQ_LOG.YML logs/
garmin-mtp pull /GARMIN/Apps/LOGS/CIQ_LOG.BAK logs/
```

Pull app-specific logs by listing the folder first, then pulling the exact filenames:

```sh
garmin-mtp list /GARMIN/Apps/LOGS
garmin-mtp pull /GARMIN/Apps/LOGS/WristAlbum.TXT logs/
garmin-mtp pull /GARMIN/Apps/LOGS/WristAlbum.BAK logs/
```

For multiple storage volumes or devices:

```sh
garmin-mtp --device-index 1 list /
garmin-mtp --storage "Internal Storage" list /GARMIN
```

## Pushing Files

Push only to directories that already exist and that Garmin expects for imports. For incoming files, prefer `NewFiles` when it appears in the device tree:

```sh
garmin-mtp push ./example.fit /GARMIN/NewFiles
```

Do not overwrite silently. Use the CLI's explicit overwrite flag only when replacement is intended:

```sh
garmin-mtp push --overwrite ./example.fit /GARMIN/NewFiles
```

## Debugging

Use `manual` or `menu` to show the built-in directory guide:

```sh
garmin-mtp menu
```

Use `dump` when path resolution fails or Garmin returns unusual object-tree data:

```sh
garmin-mtp dump
```

For directory purpose and transfer-risk guidance, read `references/directories.md`. For implementation details of the reliable fallback, read `references/parent-id-fallback.md`.
