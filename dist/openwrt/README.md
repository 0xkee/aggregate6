# aggregate6 — OpenWrt Feed

OpenWrt package feed for [aggregate6](https://github.com/0xkee/aggregate6) — fast multi-threaded IPv4/IPv6 CIDR prefix aggregation tool.

## Feed Structure

```
dist/openwrt/
└── net/
    └── aggregate6/
        └── Makefile
```

The `dist/openwrt/` directory is the feed root. OpenWrt SDK automatically scans
`net/*/Makefile` to discover packages.

## Installation

### As an OpenWrt Feed

Add the feed to `feeds.conf` or `feeds.conf.default` in your OpenWrt build root:

```bash
src-link aggregate6 /path/to/aggregate6/dist/openwrt
```

> **Note:** `src-git` won't work because the feed root is `dist/openwrt/`,
> not the repository root. Use `src-link` or copy the feed directory (see Manual Build below).

Then update, install, and build:

```bash
./scripts/feeds update aggregate6
./scripts/feeds install aggregate6
make package/aggregate6/compile V=s
```

The resulting `.ipk` will be in `bin/packages/*/aggregate6/`.

### Manual Build

```bash
# From OpenWrt build root:
cp -r /path/to/aggregate6/dist/openwrt/net/aggregate6 package/net/
make package/aggregate6/compile V=s
```

## Package Details

- **Section:** Network
- **Install path:** `/usr/bin/aggregate6`
- **Dependencies:** none (C99, zero external deps)
- **License:** GPL-3.0-only

## Version Updates

Version and checksum (`PKG_HASH`) are automatically updated by
[`dist/update-dist.sh`](../update-dist.sh) during the release workflow.
