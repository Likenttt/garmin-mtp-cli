# CLI Example: Pull CIQ Logs

This example pulls `CIQ_LOG.YML` and `CIQ_LOG.BAK` from a Garmin watch into a local `logs` directory.

Run one MTP command at a time. Do not run multiple `garmin-mtp` commands in parallel against the same watch.

## 1. Check The Installed CLI

```sh
which garmin-mtp
garmin-mtp --help
```

Install it on macOS if needed:

```sh
brew tap Likenttt/tap
brew install garmin-mtp
```

## 2. Check For Conflicting MTP Apps

On macOS, Android File Transfer and Garmin Express can claim the USB interface before `garmin-mtp`.

```sh
ps -axo pid,command | rg 'Android File Transfer|Garmin Express|Garmin Core Service|MTP'
```

If any of those apps are running, close them, unplug and reconnect the watch, then continue.

## 3. Confirm The Watch And Storage

```sh
garmin-mtp devices
garmin-mtp list /
```

Representative output:

```text
[0] Garmin Forerunner 965 vendor=0x091e product=0x50db bus=1 dev=2
s  0x00020001 Internal Storage free=20208713728 capacity=31058427904
```

## 4. List The Connect IQ Log Folder

Garmin paths can be case-sensitive. On many modern Garmin watches, the CIQ log folder is:

```text
/GARMIN/Apps/LOGS
```

List it:

```sh
garmin-mtp list /GARMIN/Apps/LOGS
```

Representative output:

```text
Storage: Internal Storage (0x00020001)
f  391          CIQ_LOG.YML
f  333          CIQ_LOG.BAK
f  4817         WristAlbum.TXT
f  5138         WristAlbum.BAK
```

## 5. Pull CIQ Logs

```sh
mkdir -p logs
garmin-mtp --quiet-progress pull /GARMIN/Apps/LOGS/CIQ_LOG.YML logs/
garmin-mtp --quiet-progress pull /GARMIN/Apps/LOGS/CIQ_LOG.BAK logs/
ls -l logs/CIQ_LOG.*
```

If the path is not found, run:

```sh
garmin-mtp dump
garmin-mtp menu
```

Then retry with the exact path and case shown by the device.

## 6. Pull App Logs From The Same Folder

Example for WristAlbum:

```sh
garmin-mtp --quiet-progress pull /GARMIN/Apps/LOGS/WristAlbum.TXT logs/
garmin-mtp --quiet-progress pull /GARMIN/Apps/LOGS/WristAlbum.BAK logs/
```

## Common Failures

`failed to open MTP device`

Close Android File Transfer, Android File Transfer Agent, Garmin Express, and Garmin Express Service. Unplug and reconnect the watch before retrying.

`directory not found`

List parent folders and use exact case:

```sh
garmin-mtp list /GARMIN
garmin-mtp list /GARMIN/Apps
garmin-mtp list /GARMIN/Apps/LOGS
```

`remote file not found`

List the folder first and pull the exact filename shown by the device.
