#!/usr/bin/env node

const childProcess = require("node:child_process");
const fs = require("node:fs");
const os = require("node:os");
const path = require("node:path");

const repoRoot = path.resolve(__dirname, "..");
const installer = path.join(repoRoot, "scripts", "install-codex-skill.js");
const tempRoot = fs.mkdtempSync(path.join(os.tmpdir(), "garmin-mtp-skill-"));

function run(args, extraEnv = {}, expectSuccess = true) {
  const result = childProcess.spawnSync(process.execPath, [installer, ...args], {
    cwd: repoRoot,
    env: {...process.env, ...extraEnv},
    encoding: "utf8",
  });

  if (expectSuccess && result.status !== 0) {
    process.stderr.write(result.stdout);
    process.stderr.write(result.stderr);
    throw new Error(`expected success for ${args.join(" ")}`);
  }
  if (!expectSuccess && result.status === 0) {
    throw new Error(`expected failure for ${args.join(" ")}`);
  }
  return result;
}

try {
  run(["--help"]);
  run(["print-path"]);

  const env = {
    CODEX_HOME: path.join(tempRoot, "codex"),
    CLAUDE_HOME: path.join(tempRoot, "claude"),
  };

  run(["install", "--target", "both"], env);

  const codexSkill = path.join(env.CODEX_HOME, "skills", "garmin-mtp", "SKILL.md");
  const claudeSkill = path.join(env.CLAUDE_HOME, "skills", "garmin-mtp", "SKILL.md");
  if (!fs.existsSync(codexSkill) || !fs.existsSync(claudeSkill)) {
    throw new Error("installed skill files were not found");
  }

  run(["install", "--target", "codex"], env, false);
  run(["install", "--target", "codex", "--force"], env);

  console.log("skill installer smoke test passed");
} finally {
  fs.rmSync(tempRoot, { recursive: true, force: true });
}
