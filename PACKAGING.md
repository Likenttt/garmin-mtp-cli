# Packaging and CI

## CI matrix

The GitHub Actions workflow at `.github/workflows/ci.yml` checks the tool on:

- macOS 14 and macOS 15 with Homebrew `libmtp` + `pkgconf`.
- Ubuntu 22.04 and Ubuntu 24.04 with `libmtp-dev` + `pkg-config`.
- Windows latest with MSYS2/MinGW + CMake, compiled without libmtp because the current libmtp/vcpkg port does not build reliably on Windows.

Use the Makefile path as the primary developer workflow on macOS/Linux:

```sh
make clean check
```

Use the CMake path for package managers that prefer CMake:

```sh
cmake -S . -B build/cmake
cmake --build build/cmake
ctest --test-dir build/cmake --output-on-failure
```

Windows CI currently uses:

```sh
cmake -S . -B build/cmake -G Ninja -DGARMIN_MTP_WITH_LIBMTP=OFF
```

That checks the CLI, manual, argument parsing, and shared logic. Full MTP device access is currently supported by the macOS/Linux libmtp builds.

## Naming

Recommended public command name: `garmin-mtp`. The build also installs `garminmtp` as a compatibility alias.

Avoid `gmtp`: it is short, but it is easy to confuse with the existing gMTP project and generic MTP tooling. `garminmtp` is usable, but the unhyphenated form is harder to read and less consistent with Homebrew formula naming. `garmin-mtp` is explicit and currently has no Homebrew formula name conflict in local `brew search` checks.

## Homebrew tap

For early releases, publish your own tap first:

1. Create a GitHub repo named `homebrew-tap`.
2. Add `Formula/garmin-mtp.rb`.
3. Users install with:

```sh
brew tap Likenttt/tap
brew install garmin-mtp
```

Formula skeleton:

```ruby
class GarminMtp < Formula
  desc "Read and write Garmin watch files over MTP"
  homepage "https://github.com/Likenttt/garmin-mtp-cli"
  url "https://github.com/Likenttt/garmin-mtp-cli/archive/refs/tags/v0.1.0.tar.gz"
  sha256 "REPLACE_WITH_RELEASE_TARBALL_SHA256"
  license "MIT"

  depends_on "pkgconf" => :build
  depends_on "libmtp"

  def install
    system "make"
    bin.install "build/garmin-mtp"
    bin.install "build/garminmtp"
  end

  test do
    assert_match "garmin-mtp", shell_output("#{bin}/garmin-mtp --help")
    assert_match "Garmin MTP directory manual", shell_output("#{bin}/garminmtp menu")
  end
end
```

Validate locally:

```sh
brew install --build-from-source ./Formula/garmin-mtp.rb
brew test garmin-mtp
brew audit --strict --new --online garmin-mtp
```

After the project has real users and stable releases, submit to `homebrew/core` with a pull request. Homebrew core expects source builds, a meaningful test, and support for the currently supported macOS/Linux runners.
