# Packaging aggregate6

This directory contains packaging files for building `aggregate6` on various Linux distributions and operating systems.

## Supported Distributions

| Distribution | Directory | Format |
|---|---|---|
| Entware (OpenWrt-based routers) | `entware/` | opkg Makefile |
| OpenWrt | `openwrt/` | opkg Makefile |
| Debian / Ubuntu | `debian/` | dpkg (control, rules, etc.) |
| Fedora / RHEL / CentOS / openSUSE | `rpm/` | RPM spec |
| Alpine Linux | `alpine/` | APKBUILD |
| Arch Linux | `archlinux/` | PKGBUILD |
| Gentoo | `gentoo/` | ebuild |
| Void Linux | `void/` | xbps-src template |
| NixOS | `nix/` | Nix derivation |
| FreeBSD | `freebsd/` | ports Makefile |

## Build Instructions

### Entware

```bash
# Add to your Entware packages feed, then:
make package/aggregate6/compile V=s
```

### OpenWrt

```bash
# Copy dist/openwrt/ to package/aggregate6/ in OpenWrt SDK
# Then:
make package/aggregate6/compile V=s
```

### Debian / Ubuntu

```bash
# From project root:
cp -r dist/debian .
dpkg-buildpackage -us -uc -b
# .deb will be in parent directory
```

### Fedora / RHEL / RPM-based

```bash
# Create source tarball first:
tar czf aggregate6-0.1.0.tar.gz --transform='s,^,aggregate6-0.1.0/,' src/ Makefile LICENSE README.md
rpmbuild -bb dist/rpm/aggregate6.spec --define "_sourcedir $(pwd)"
```

### Alpine Linux

```bash
# Copy dist/alpine/ to an aports tree, then:
cd aports/testing/aggregate6
abuild -r
```

### Arch Linux

```bash
cd dist/archlinux
makepkg -si
```

### Gentoo

```bash
# Copy dist/gentoo/app-net/aggregate6/ to local overlay
# Then:
emerge app-net/aggregate6
```

### Void Linux

```bash
# Copy dist/void/template to void-packages/srcpkgs/aggregate6/template
cd void-packages
./xbps-src pkg aggregate6
```

### NixOS

```bash
nix-build dist/nix/default.nix
# Or add to your flake/overlay
```

### FreeBSD

```bash
# Copy dist/freebsd/ to /usr/ports/net/aggregate6/
cd /usr/ports/net/aggregate6
make install clean
```

## Notes

- All packaging files reference version `0.1.0`. Update version numbers when releasing new versions.
- Source URLs use `https://github.com/0xkee/aggregate6` — update if the repository location changes.
- Checksums (sha256sums, sha512sums) are placeholders — generate actual checksums after creating release tarballs.
- The project has **zero external dependencies** — only a C99 compiler, make, and libc are required.
