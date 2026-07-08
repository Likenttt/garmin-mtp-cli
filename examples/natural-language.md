# Natural Language Examples

Use these prompts after installing the `garmin-mtp` skill. The agent should run `garmin-mtp` commands serially because MTP device access is exclusive.

## Inspect The Watch

Prompt:

```text
Use $garmin-mtp to check whether my Garmin watch is connected, then list the available storage volumes.
```

Expected command flow:

```sh
garmin-mtp devices
garmin-mtp list /
```

## Pull CIQ Logs

Prompt:

```text
Use $garmin-mtp to create a logs directory and pull CIQ_LOG.YML and CIQ_LOG.BAK from my Garmin watch.
```

Expected command flow:

```sh
mkdir -p logs
garmin-mtp list /GARMIN/Apps/LOGS
garmin-mtp pull /GARMIN/Apps/LOGS/CIQ_LOG.YML logs/
garmin-mtp pull /GARMIN/Apps/LOGS/CIQ_LOG.BAK logs/
```

## Pull App-Specific Logs

Prompt:

```text
Use $garmin-mtp to pull the WristAlbum logs from the Garmin Connect IQ log folder into ./logs.
```

Expected command flow:

```sh
mkdir -p logs
garmin-mtp list /GARMIN/Apps/LOGS
garmin-mtp pull /GARMIN/Apps/LOGS/WristAlbum.TXT logs/
garmin-mtp pull /GARMIN/Apps/LOGS/WristAlbum.BAK logs/
```

## Push A File Without Overwriting

Prompt:

```text
Use $garmin-mtp to copy ./course.fit to the Garmin import folder, but do not overwrite any existing file.
```

Expected command flow:

```sh
garmin-mtp list /GARMIN/NewFiles
garmin-mtp push ./course.fit /GARMIN/NewFiles
```

If the remote file already exists, the command should fail with a message telling the user to pass `--overwrite`.

## Push A File With Explicit Overwrite

Prompt:

```text
Use $garmin-mtp to replace course.fit in the Garmin import folder with ./course.fit.
```

Expected command flow:

```sh
garmin-mtp push --overwrite ./course.fit /GARMIN/NewFiles
```

## Diagnose MTP Conflicts

Prompt:

```text
Use $garmin-mtp to diagnose why the Garmin watch cannot be opened over MTP.
```

Expected command flow on macOS:

```sh
ps -axo pid,command | rg 'Android File Transfer|Garmin Express|Garmin Core Service|MTP'
garmin-mtp devices
garmin-mtp list /
```

If Android File Transfer or Garmin Express is running, close it, unplug and reconnect the watch, then retry.
