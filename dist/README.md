# Distribution Packages

Pre-made packaging for **10 platforms**. Each `dist/<distro>/` directory is a
self-contained repository, overlay, or feed that can be used directly by the
target distro's package manager.

All packages fetch source tarballs from
[GitHub Releases](https://github.com/0xkee/aggregate6/releases) — no `git clone`
at build time.

## Supported Platforms

| Distribution | Type | Directory | Package Format | Quick Install |
|---|---|---|---|---|
| Gentoo | Overlay | [`gentoo/`](gentoo/) | ebuild | `emerge app-net/aggregate6` |
| Alpine Linux | aports | [`alpine/`](alpine/) | APKBUILD | `abuild -r` |
| Arch Linux | AUR | [`archlinux/`](archlinux/) | PKGBUILD | `yay -S aggregate6` |
| Void Linux | void-packages | [`void/`](void/) | xbps-src template | `./xbps-src pkg aggregate6` |
| Fedora / RHEL | COPR | [`rpm/`](rpm/) | RPM spec | `dnf copr enable 0xkee/aggregate6 && dnf install aggregate6` |
| NixOS | Flake | [`nix/`](nix/) | Nix derivation | `nix profile install github:0xkee/aggregate6?dir=dist/nix` |
| Debian / Ubuntu | dpkg | [`debian/`](debian/) | deb | `dpkg-buildpackage -us -uc` |
| OpenWrt | Feed | [`openwrt/`](openwrt/) | opkg Makefile | `make package/aggregate6/compile V=s` |
| Entware | Feed | [`entware/`](entware/) | opkg Makefile | `make package/aggregate6/compile V=s` |
| FreeBSD | Port | [`freebsd/`](freebsd/) | ports Makefile | `cd /usr/ports/net/aggregate6 && make install clean` |

## Directory Structure

```
dist/
├── update-dist.sh                          # Version/checksum updater
├── README.md                               # This file
├── gentoo/                                 # Gentoo Overlay
│   ├── metadata/layout.conf
│   ├── profiles/{repo_name,categories}
│   ├── app-net/aggregate6/*.ebuild
│   └── README.md
├── alpine/                                 # Alpine aports
│   ├── testing/aggregate6/APKBUILD
│   └── README.md
├── archlinux/                              # Arch Linux AUR
│   ├── PKGBUILD, .SRCINFO
│   └── README.md
├── void/                                   # Void Linux
│   ├── srcpkgs/aggregate6/template
│   └── README.md
├── rpm/                                    # RPM / COPR
│   ├── aggregate6.spec, aggregate6.repo
│   └── README.md
├── nix/                                    # NixOS Flake
│   ├── default.nix, flake.nix
│   └── README.md
├── debian/                                 # Debian / Ubuntu
│   ├── control, rules, changelog, ...
│   └── README.md
├── openwrt/                                # OpenWrt Feed
│   ├── net/aggregate6/Makefile
│   └── README.md
├── entware/                                # Entware Feed
│   ├── net/aggregate6/Makefile
│   └── README.md
└── freebsd/                                # FreeBSD Port
    ├── Makefile, distinfo, pkg-descr
    └── README.md
```

## Release Automation

### GitHub Actions (`release.yml`)

Pushing a tag `v*` triggers the
[release workflow](../.github/workflows/release.yml):

1. Checkout, build, and run tests
2. Create a source tarball via `git archive`
3. Calculate SHA-256/SHA-512 checksums
4. Run `dist/update-dist.sh` to update **all** packaging files
5. Commit changes to `master` and push
6. Create a GitHub Release with the tarball and checksum files

### `update-dist.sh`

```bash
dist/update-dist.sh VERSION SHA256 SHA512 SIZE
```

Updates version numbers, source URLs, and checksums across all 10 packaging
formats in a single pass. Called automatically by the release workflow — can
also be run manually.

Files updated:
- `alpine/testing/aggregate6/APKBUILD` — `pkgver`, `source`, `sha512sums`
- `archlinux/PKGBUILD` + `.SRCINFO` — `pkgver`, `sha256sums`
- `rpm/aggregate6.spec` — `Version`, `%changelog`
- `void/srcpkgs/aggregate6/template` — `version`, `checksum`
- `nix/default.nix` — `version`, `sha256` (SRI format)
- `openwrt/net/aggregate6/Makefile` — `PKG_VERSION`, `PKG_HASH`
- `entware/net/aggregate6/Makefile` — `PKG_VERSION`, `PKG_HASH`
- `freebsd/Makefile` + `distinfo` — `DISTVERSION`, checksums
- `gentoo/app-net/aggregate6/*.ebuild` — new versioned ebuild (rotated)
- `debian/changelog` — new changelog entry prepended

## Per-Distribution Documentation

Each directory contains its own `README.md` with detailed instructions:

- [Gentoo](gentoo/README.md) — overlay setup, eselect/repos.conf/symlink methods
- [Alpine](alpine/README.md) — APKBUILD, abuild, aports integration
- [Arch Linux](archlinux/README.md) — AUR publishing, makepkg, .SRCINFO
- [Void Linux](void/README.md) — void-packages setup, xbps-src
- [Fedora / RHEL](rpm/README.md) — COPR setup, rpmbuild, mock
- [NixOS](nix/README.md) — flake, overlay, nix-build
- [Debian / Ubuntu](debian/README.md) — dpkg-buildpackage, PPA, reprepro
- [OpenWrt](openwrt/README.md) — feed setup, SDK build
- [Entware](entware/README.md) — feed setup, embedded routers
- [FreeBSD](freebsd/README.md) — ports tree, poudriere
