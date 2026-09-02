# Alpine Linux — aggregate6 APKBUILD

This directory contains an aports-compatible APKBUILD for `aggregate6`.

## Structure

```
dist/alpine/
└── testing/
    └── aggregate6/
        └── APKBUILD    # Alpine package build recipe
```

The `testing/aggregate6/` path mirrors the
[Alpine aports](https://gitlab.alpinelinux.org/alpine/aports) tree structure.

## Building

### Option 1: Build directly

```bash
cd dist/alpine/testing/aggregate6
abuild -r
```

> **Prerequisites:** `abuild` must be installed and configured.
> Run `abuild-keygen -a -i` once to set up signing keys.

### Option 2: Link into aports tree

```bash
# Clone aports (or use existing checkout)
git clone --depth=1 https://gitlab.alpinelinux.org/alpine/aports.git ~/aports

# Copy the package directory
cp -r dist/alpine/testing/aggregate6 ~/aports/testing/aggregate6

# Build
cd ~/aports/testing/aggregate6
abuild -r
```

## Installing the built package

```bash
# Find the built .apk (path shown by abuild output)
apk add --allow-untrusted ~/packages/testing/x86_64/aggregate6-*.apk
```

## Checksum updates

The `sha512sums` field is automatically updated by `dist/update-dist.sh`
during the release process. For manual builds, regenerate with:

```bash
abuild checksum
```
