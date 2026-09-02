# Void Linux — aggregate6 template

This directory contains a void-packages compatible template for `aggregate6`.

## Structure

```
dist/void/
└── srcpkgs/
    └── aggregate6/
        └── template    # Void package build template
```

The `srcpkgs/aggregate6/` path mirrors the
[void-packages](https://github.com/void-linux/void-packages) tree structure.

## Building

### Step 1: Set up void-packages

```bash
git clone --depth=1 https://github.com/void-linux/void-packages.git
cd void-packages
./xbps-src binary-bootstrap
```

### Step 2: Copy the template

```bash
cp -r /path/to/aggregate6/dist/void/srcpkgs/aggregate6 srcpkgs/aggregate6
```

### Step 3: Build

```bash
./xbps-src pkg aggregate6
```

### Step 4: Install

```bash
xi aggregate6
# Or manually:
xbps-install -R hostdir/binpkgs aggregate6
```

## Checksum updates

The `checksum` field is automatically updated by `dist/update-dist.sh`
during the release process. For manual builds, regenerate with:

```bash
cd void-packages
xgensum -i srcpkgs/aggregate6/template
```
