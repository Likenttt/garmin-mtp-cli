#!/usr/bin/env node

const fs = require("node:fs");
const os = require("node:os");
const path = require("node:path");

const skillName = "garmin-mtp";
const repoRoot = path.resolve(__dirname, "..");
const skillSource = path.join(repoRoot, "skills", skillName);

function usage() {
  console.log(`garmin-mtp-skill - install the Garmin MTP Codex skill

Usage:
  garmin-mtp-skill install [--target codex|claude|both] [--force]
  garmin-mtp-skill print-path
  garmin-mtp-skill --help

Options:
  --target VALUE   Install target: codex, claude, or both (default: codex)
  --dest PATH      Install into a custom skills root; copies to PATH/${skillName}
  --force          Replace an existing installed skill
  --dry-run        Print planned actions without copying files

Default destinations:
  Codex:       \${CODEX_HOME:-~/.codex}/skills/${skillName}
  Claude Code: \${CLAUDE_HOME:-~/.claude}/skills/${skillName}`);
}

function fail(message) {
  console.error(`error: ${message}`);
  process.exit(1);
}

function parseArgs(argv) {
  const options = {
    command: "install",
    target: "codex",
    dest: null,
    force: false,
    dryRun: false,
  };

  const args = [...argv];
  if (args[0] && !args[0].startsWith("-")) {
    options.command = args.shift();
  }

  for (let i = 0; i < args.length; i++) {
    const arg = args[i];
    if (arg === "--help" || arg === "-h") {
      options.command = "help";
      continue;
    }
    if (arg === "--force") {
      options.force = true;
      continue;
    }
    if (arg === "--dry-run") {
      options.dryRun = true;
      continue;
    }
    if (arg === "--target") {
      options.target = args[++i];
      continue;
    }
    if (arg.startsWith("--target=")) {
      options.target = arg.slice("--target=".length);
      continue;
    }
    if (arg === "--dest") {
      options.dest = args[++i];
      continue;
    }
    if (arg.startsWith("--dest=")) {
      options.dest = arg.slice("--dest=".length);
      continue;
    }
    fail(`unknown option: ${arg}`);
  }

  if (!["install", "print-path", "help"].includes(options.command)) {
    fail(`unknown command: ${options.command}`);
  }
  if (!["codex", "claude", "both"].includes(options.target)) {
    fail("--target must be codex, claude, or both");
  }
  if (options.dest && options.target === "both") {
    fail("--dest can only be used with --target codex or --target claude");
  }
  return options;
}

function defaultSkillsRoot(target) {
  if (target === "codex") {
    return path.join(process.env.CODEX_HOME || path.join(os.homedir(), ".codex"), "skills");
  }
  return path.join(process.env.CLAUDE_HOME || path.join(os.homedir(), ".claude"), "skills");
}

function installTarget(target, options) {
  const skillsRoot = options.dest ? path.resolve(options.dest) : defaultSkillsRoot(target);
  const destination = path.join(skillsRoot, skillName);

  console.log(`${options.dryRun ? "Would install" : "Installing"} ${skillSource} -> ${destination}`);
  if (options.dryRun) {
    return;
  }

  if (!fs.existsSync(path.join(skillSource, "SKILL.md"))) {
    fail(`skill source is missing: ${skillSource}`);
  }

  fs.mkdirSync(skillsRoot, { recursive: true });
  if (fs.existsSync(destination)) {
    if (!options.force) {
      fail(`destination already exists: ${destination}; pass --force to replace it`);
    }
    fs.rmSync(destination, { recursive: true, force: true });
  }
  fs.cpSync(skillSource, destination, { recursive: true });
}

function main() {
  const options = parseArgs(process.argv.slice(2));
  if (options.command === "help") {
    usage();
    return;
  }
  if (options.command === "print-path") {
    console.log(skillSource);
    return;
  }

  const targets = options.target === "both" ? ["codex", "claude"] : [options.target];
  for (const target of targets) {
    installTarget(target, options);
  }
  console.log("Done. Restart Codex or Claude Code to pick up the installed skill.");
}

main();
