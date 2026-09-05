# Debian / Ubuntu — aggregate6

Debian packaging files for building `.deb` packages from upstream source tarball.
Uses `3.0 (quilt)` source format with upstream tarball from GitHub Releases.

## Quick install (pre-built .deb)

Pre-built `.deb` packages for amd64 are published automatically to every
[GitHub Release](https://github.com/0xkee/aggregate6/releases):

```bash
VERSION=0.3.0
wget https://github.com/0xkee/aggregate6/releases/download/v${VERSION}/aggregate6_${VERSION}-1_amd64.deb
sudo dpkg -i aggregate6_${VERSION}-1_amd64.deb
```

## Building .deb from source

```bash
# Download upstream tarball
VERSION=0.3.0
wget https://github.com/0xkee/aggregate6/releases/download/v${VERSION}/aggregate6-${VERSION}.tar.gz

# Rename to Debian orig format
mv aggregate6-${VERSION}.tar.gz aggregate6_${VERSION}.orig.tar.gz

# Extract and add debian/ directory
tar xf aggregate6_${VERSION}.orig.tar.gz
cp -r dist/debian aggregate6-${VERSION}/debian
cd aggregate6-${VERSION}

# Build (unsigned)
dpkg-buildpackage -us -uc
```

## Installation

```bash
sudo dpkg -i ../aggregate6_${VERSION}-1_amd64.deb
```

## Creating a PPA (Ubuntu)

1. Create an account on [Launchpad](https://launchpad.net)
2. Create a PPA: `Your Profile → Create a new PPA`
3. Upload the source package:

```bash
# Build signed source package
dpkg-buildpackage -S -sa

# Upload to PPA
dput ppa:yourname/aggregate6 ../aggregate6_${VERSION}-1_source.changes
```

## Creating an APT repository with reprepro

```bash
# Create repository structure
mkdir -p repo/conf
cat > repo/conf/distributions <<EOF
Origin: aggregate6
Label: aggregate6
Codename: stable
Architectures: amd64 arm64 i386
Components: main
Description: aggregate6 APT repository
EOF

# Add built packages
reprepro -b repo includedeb stable ../aggregate6_*.deb

# Serve via HTTP (e.g., GitHub Pages, nginx, etc.)
```

## Checking for new upstream versions

The `watch` file enables `uscan` to detect new releases:

```bash
uscan --no-download --verbose
```

## Files

| File | Description |
|------|-------------|
| `changelog` | Debian changelog (auto-updated by release workflow) |
| `control` | Package metadata and dependencies |
| `copyright` | DEP-5 copyright information |
| `rules` | Build rules (dh-based) |
| `source/format` | Source format: `3.0 (quilt)` |
| `watch` | uscan watch file for upstream version tracking |
