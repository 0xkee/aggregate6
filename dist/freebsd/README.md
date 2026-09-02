# aggregate6 — FreeBSD Port

FreeBSD port for [aggregate6](https://github.com/0xkee/aggregate6) — fast CIDR prefix aggregation tool.

## Port Structure

```
dist/freebsd/
├── Makefile      # Port build instructions
├── distinfo      # Checksums and file size
└── pkg-descr     # Package description
```

This directory can be copied directly into the FreeBSD ports tree as
`/usr/ports/net/aggregate6/`.

## Installation

### From Ports Tree

```bash
# Copy port into the ports tree
cp -r dist/freebsd /usr/ports/net/aggregate6
cd /usr/ports/net/aggregate6
make install clean
```

### With Poudriere

```bash
# Copy port into your ports tree overlay
cp -r dist/freebsd /usr/local/poudriere/ports/default/net/aggregate6

# Build with poudriere
poudriere bulk -j 14amd64 -p default net/aggregate6
```

### From Source Directly

```bash
# The port fetches the source tarball from GitHub Releases:
# https://github.com/0xkee/aggregate6/releases/download/v0.1.0/aggregate6-0.1.0.tar.gz
```

## Port Details

- **Category:** net
- **Install path:** `${PREFIX}/bin/aggregate6`
- **Dependencies:** none (C99, zero external deps)
- **Build system:** gmake
- **License:** GPL-3.0-or-later

## Version Updates

Version (`DISTVERSION`) is updated by [`dist/update-dist.sh`](../update-dist.sh)
during the release workflow. The `distinfo` file is regenerated with fresh
SHA256 checksum and file size.
