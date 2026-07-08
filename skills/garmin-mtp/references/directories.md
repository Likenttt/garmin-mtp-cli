# Garmin MTP Directories

Paths vary by watch model, firmware, maps, and enabled features. Always run `garmin-mtp list /` and then list the target folder before pulling or pushing.

Common paths:

- `/GARMIN/Apps/LOGS` - Connect IQ and app logs, including `CIQ_LOG.YML`, `CIQ_LOG.BAK`, and app-specific logs such as `WristAlbum.TXT`.
- `/GARMIN/Apps/DATA` - Connect IQ app data files. Treat as app state; pull before changing.
- `/GARMIN/Apps/SETTINGS` - Connect IQ settings files. Write only when the expected format is known.
- `/GARMIN/NewFiles` or `/NewFiles` - Garmin import/drop folder for supported incoming files such as FIT courses/workouts on many devices.
- `/Records` - Activity records and history. It may contain many files; listing or pulling repeatedly can be slow.
- `/Metrics` - Health, training, and sensor metric data. Treat as personal data and expect large enumerations.
- `/Monitor` - Background monitoring data. Treat as personal data; large pulls can be slow.
- `/RunningTrack` - Small FIT snippets related to running track calibration or recognition.
- `/Location` - Location cache or position data. Contents depend on enabled features.
- `/PointCache` - Cached route/navigation points. Can grow with map and navigation use.
- `/*.img` - Map image files. These can be multiple gigabytes and may take a very long time to transfer over MTP.

Operational notes:

- Prefer pulling individual files over bulk directory copies.
- Warn users before pulling map images, Records, Metrics, Monitor, or large caches.
- Keep MTP operations serial; concurrent MTP commands can claim the same USB interface and fail.
- If a path exists in Android File Transfer but `garmin-mtp list` cannot find it, run `garmin-mtp dump` and retry with exact case.
