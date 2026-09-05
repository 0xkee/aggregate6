# aggregate6

Fast multi-threaded IPv4/IPv6 CIDR prefix aggregation tool written in C.

Reads IPv4/IPv6 prefixes from stdin, aggregates them, outputs minimal covering set.

Inspired by the original [aggregate](https://ftp.isc.org/isc/aggregate/) by Joe Abley — rewritten from scratch with IPv6 support.

## Features

- IPv4 and IPv6 support
- Reads from stdin, pipes, or file arguments
- Zero external dependencies — C99 + POSIX only
- `mmap`-based file I/O with automatic fallback to `fgets`
- Radix sort for IPv4, parallel IPv4/IPv6 aggregation via pthreads

## Platform Support

| Platform | Arch | libc | Status |
|----------|------|------|--------|
| **Linux** | amd64, aarch64, mipsel | glibc ≥ 2.17 | ✅ Primary |
| **Linux** | amd64, aarch64 | musl ≥ 1.1 | ✅ Tested (Alpine, embedded) |
| **FreeBSD** | amd64 | system libc | ✅ Supported |
| **macOS** | amd64, arm64 | system libc | ✅ Supported |
| **OpenBSD / NetBSD** | amd64 | system libc | ✅ Should work (POSIX) |
| **Entware** (routers) | mipsel, aarch64 | musl / uClibc | ✅ Tested |
| **OpenWrt** | mips, arm | musl | ✅ Supported |

Requires: C99 compiler (gcc or clang), POSIX 2008 (`mmap`, `pthreads`), GNU make.

## Build

```sh
make
```

Produces `aggregate6` binary in the project root.

For cross-compilation (e.g. Entware toolchain):

```sh
make CC=mipsel-linux-gnu-gcc
```

On musl-based systems where pthreads is built into libc:

```sh
make LDLIBS=""
```

## Usage

```sh
# from stdin
echo "192.168.1.0/24\n192.168.2.0/24" | ./aggregate6

# from files (uses mmap — faster than pipe)
./aggregate6 prefixes.txt more-prefixes.txt
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
| Debian / Ubuntu | `dpkg -i aggregate6_VERSION-1_amd64.deb` | [dist/debian/](dist/debian/README.md) |
| FreeBSD | `cd /usr/ports/net/aggregate6 && make install clean` | [dist/freebsd/](dist/freebsd/README.md) |
| OpenWrt | `make package/aggregate6/compile V=s` | [dist/openwrt/](dist/openwrt/README.md) |
| Entware | `opkg install aggregate6_VERSION-1_ARCH.ipk` | [dist/entware/](dist/entware/README.md) |

See [`dist/README.md`](dist/README.md) for full documentation, release automation details,
and directory structure.

## License

GPL-3.0-only — see [LICENSE](LICENSE).
