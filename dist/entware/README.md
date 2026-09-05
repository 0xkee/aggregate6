# aggregate6 — Entware Feed

Entware package feed for [aggregate6](https://github.com/0xkee/aggregate6) — fast multi-threaded IPv4/IPv6 CIDR prefix aggregation tool.

## Feed Structure

```
dist/entware/
└── net/
    └── aggregate6/
        └── Makefile
```

The `dist/entware/` directory is the feed root. Entware SDK (based on OpenWrt)
automatically scans `net/*/Makefile` to discover packages.

## Quick install (pre-built .ipk)

Pre-built `.ipk` packages (statically linked) are published to every
[GitHub Release](https://github.com/0xkee/aggregate6/releases):

```bash
VERSION=0.3.0
ARCH=mipsel-3x   # see table below

wget https://github.com/0xkee/aggregate6/releases/download/v${VERSION}/aggregate6_${VERSION}-1_${ARCH}.ipk
opkg install aggregate6_${VERSION}-1_${ARCH}.ipk
```

| Entware arch | CPU | Devices |
|---|---|---|
| `mipsel-3x` | MIPS32 R2 LE | Keenetic (MT7621), many routers |
| `aarch64-3x` | ARM64 | Keenetic new (MT7981/7986), NAS |
| `armv7-3x` | ARMv7 HF | ASUS Merlin, Synology ARM NAS |
| `x86-64-3x` | x86-64 | Synology, QNAP x86 NAS |

## Building from source

### As an Entware Feed

Add the feed to `feeds.conf` in your Entware build root:

```bash
src-link aggregate6 /path/to/aggregate6/dist/entware
```

> **Note:** `src-git` won't work because the feed root is `dist/entware/`,
> not the repository root. Use `src-link` or copy the feed directory (see Manual Build below).

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
- **License:** GPL-3.0-only

## Differences from OpenWrt

| Feature | OpenWrt | Entware |
|---------|---------|---------|
| Install path | `/usr/bin` | `/opt/bin` |
| Dependencies | none | `+libc` |
| Maintainer | specified | omitted (Entware convention) |

## Version Updates

Version and checksum (`PKG_HASH`) are automatically updated by
[`dist/update-dist.sh`](../update-dist.sh) during the release workflow.
