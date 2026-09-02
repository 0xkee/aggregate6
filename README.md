# aggregate6

Fast CIDR prefix aggregation tool written in C.

Reads IPv4/IPv6 prefixes from stdin, aggregates them, outputs minimal covering set.

Inspired by the original [aggregate](https://ftp.isc.org/isc/aggregate/) by Joe Abley — rewritten from scratch with IPv6 support.

## Features

- IPv4 and IPv6 support
- Reads from stdin or files
- Minimal dependencies (libc only)
- Fast Patricia trie implementation
- Portable C99 — works on Linux, *BSD, Entware/embedded

## Build

```sh
make
```

Produces `aggregate6` binary in the project root.

For cross-compilation (e.g. Entware toolchain):

```sh
make CC=mipsel-linux-gnu-gcc
```

## Usage

```sh
echo "192.168.1.0/24\n192.168.2.0/24" | ./aggregate6
```

Filter by address family:

```sh
./aggregate6 -4    # IPv4 only
./aggregate6 -6    # IPv6 only
```

## Install

```sh
make install PREFIX=/opt
```

Installs to `$(PREFIX)/bin/aggregate6`.

## Tests

```sh
make test
```

## Installation — Distribution Packages

Pre-made packaging for **10 platforms** is available in [`dist/`](dist/).
Each directory is a self-contained overlay, feed, or port — ready to use
with your distro's package manager.

| Distribution | Quick Install | Details |
|---|---|---|
| Arch Linux | `yay -S aggregate6` | [dist/archlinux/](dist/archlinux/README.md) |
| Fedora / RHEL | `dnf copr enable 0xkee/aggregate6 && dnf install aggregate6` | [dist/rpm/](dist/rpm/README.md) |
| NixOS | `nix profile install github:0xkee/aggregate6?dir=dist/nix` | [dist/nix/](dist/nix/README.md) |
| Gentoo | `emerge app-net/aggregate6` | [dist/gentoo/](dist/gentoo/README.md) |
| Alpine Linux | `abuild -r` | [dist/alpine/](dist/alpine/README.md) |
| Void Linux | `./xbps-src pkg aggregate6` | [dist/void/](dist/void/README.md) |
| Debian / Ubuntu | `dpkg-buildpackage -us -uc` | [dist/debian/](dist/debian/README.md) |
| FreeBSD | `cd /usr/ports/net/aggregate6 && make install clean` | [dist/freebsd/](dist/freebsd/README.md) |
| OpenWrt | `make package/aggregate6/compile V=s` | [dist/openwrt/](dist/openwrt/README.md) |
| Entware | `make package/aggregate6/compile V=s` | [dist/entware/](dist/entware/README.md) |

See [`dist/README.md`](dist/README.md) for full documentation, release automation details,
and directory structure.

## License

GPL-3.0-or-later — see [LICENSE](LICENSE).
