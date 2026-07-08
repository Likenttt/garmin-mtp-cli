# Garmin MTP CLI Examples

These examples show two ways to use Garmin MTP CLI:

- [Natural language with Codex or Claude Code](natural-language.md)
- [CLI: pull CIQ logs from a Garmin watch](cli-ciq-log.md)

The examples assume `garmin-mtp` is installed and a Garmin watch is connected over USB.

On macOS:

```sh
brew tap Likenttt/tap
brew install garmin-mtp
```

Install the agent skill:

```sh
npx github:Likenttt/garmin-mtp-cli install --target both
```

Restart Codex or Claude Code after installing the skill.
