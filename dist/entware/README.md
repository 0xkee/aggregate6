# aggregate6 — Entware Feed

Entware package feed for [aggregate6](https://github.com/0xkee/aggregate6) — fast CIDR prefix aggregation tool.

## Feed Structure

```
dist/entware/
└── net/
    └── aggregate6/
        └── Makefile
```

The `dist/entware/` directory is the feed root. Entware SDK (based on OpenWrt)
automatically scans `net/*/Makefile` to discover packages.

## Installation

### As an Entware Feed

Add the feed to `feeds.conf` in your Entware build root:

```bash
# Remote feed (uses dist/entware/ subdirectory):
src-git aggregate6 https://github.com/0xkee/aggregate6.git;master

# Or local feed (if you cloned the repo):
src-link aggregate6 /path/to/aggregate6/dist/entware
```

Then update, install, and build:

```bash
./scripts/feeds update aggregate6
./scripts/feeds install aggregate6
make package/aggregate6/compile V=s
```

### Manual Build

```bash
# From Entware build root:
cp -r /path/to/aggregate6/dist/entware/net/aggregate6 package/net/
make package/aggregate6/compile V=s
```

## Package Details

- **Section:** Network
- **Install path:** `/opt/bin/aggregate6` (Entware standard)
- **Dependencies:** `libc`
- **License:** GPL-3.0-or-later

## Differences from OpenWrt

| Feature | OpenWrt | Entware |
|---------|---------|---------|
| Install path | `/usr/bin` | `/opt/bin` |
| Dependencies | none | `+libc` |
| Maintainer | specified | omitted (Entware convention) |

## Version Updates

Version and checksum (`PKG_HASH`) are automatically updated by
[`dist/update-dist.sh`](../update-dist.sh) during the release workflow.
