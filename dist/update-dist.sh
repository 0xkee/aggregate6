#!/bin/bash
# update-dist.sh — Update version and checksums in all dist/ package files.
#
# Usage: dist/update-dist.sh VERSION SHA256 SHA512 SIZE
#
# Called by .github/workflows/release.yml after creating a release tarball.
# Idempotent: safe to run multiple times with the same arguments.

set -euo pipefail

if [ $# -lt 4 ]; then
    echo "Usage: $0 VERSION SHA256 SHA512 SIZE" >&2
    exit 1
fi

VERSION="$1"
SHA256="$2"
SHA512="$3"
SIZE="$4"

MAX_VERSIONS=3   # Keep last N Gentoo ebuild versions

TARBALL="aggregate6-${VERSION}.tar.gz"
URL="https://github.com/0xkee/aggregate6/releases/download/v${VERSION}/${TARBALL}"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "Updating dist/ to version ${VERSION}..."

# --- Alpine APKBUILD ---
APKBUILD="${SCRIPT_DIR}/alpine/testing/aggregate6/APKBUILD"
sed -i "s/^pkgver=.*/pkgver=${VERSION}/" "$APKBUILD"
sed -i "s|^source=.*|source=\"\$pkgname-\$pkgver.tar.gz::${URL}\"|" "$APKBUILD"
# Replace sha512sums block: remove old trailing block, append fresh one
sed -i '/^sha512sums=/,$d' "$APKBUILD"
cat >> "$APKBUILD" <<EOF
sha512sums="
${SHA512}  ${TARBALL}
"
EOF
echo "  Updated: alpine/testing/aggregate6/APKBUILD"

# --- Arch PKGBUILD + .SRCINFO ---
PKGBUILD="${SCRIPT_DIR}/archlinux/PKGBUILD"
sed -i "s/^pkgver=.*/pkgver=${VERSION}/" "$PKGBUILD"
sed -i "s/^sha256sums=.*/sha256sums=('${SHA256}')/" "$PKGBUILD"
# Regenerate .SRCINFO without makepkg (format is simple key-value text)
cat > "${SCRIPT_DIR}/archlinux/.SRCINFO" <<EOF
pkgbase = aggregate6
	pkgdesc = Fast CIDR prefix aggregation tool
	pkgver = ${VERSION}
	pkgrel = 1
	url = https://github.com/0xkee/aggregate6
	arch = x86_64
	arch = aarch64
	license = GPL-3.0-or-later
	makedepends = gcc
	makedepends = make
	depends = glibc
	source = aggregate6-${VERSION}.tar.gz::https://github.com/0xkee/aggregate6/releases/download/v${VERSION}/aggregate6-${VERSION}.tar.gz
	sha256sums = ${SHA256}

pkgname = aggregate6
EOF
echo "  Updated: archlinux/PKGBUILD + .SRCINFO"

# --- RPM spec ---
RPM_SPEC="${SCRIPT_DIR}/rpm/aggregate6.spec"
sed -i "s/^Version:.*/Version:        ${VERSION}/" "$RPM_SPEC"
# Add %changelog entry (skip if version already present)
if ! grep -q "^.*- ${VERSION}-1$" "$RPM_SPEC"; then
    RPM_DATE=$(date +"%a %b %d %Y")
    sed -i "/^%changelog$/a\\* ${RPM_DATE} 0xkee <0xkee@users.noreply.github.com> - ${VERSION}-1\n- Update to ${VERSION}" \
        "$RPM_SPEC"
fi
echo "  Updated: rpm/aggregate6.spec"

# --- Void template ---
VOID_TEMPLATE="${SCRIPT_DIR}/void/srcpkgs/aggregate6/template"
sed -i "s/^version=.*/version=${VERSION}/" "$VOID_TEMPLATE"
sed -i "s/^checksum=.*/checksum=${SHA256}/" "$VOID_TEMPLATE"
echo "  Updated: void/srcpkgs/aggregate6/template"

# --- NixOS default.nix ---
NIX_DEFAULT="${SCRIPT_DIR}/nix/default.nix"
SRI_HASH="sha256-$(echo -n "${SHA256}" | xxd -r -p | base64 -w0)"
sed -i "s/version = \".*\"/version = \"${VERSION}\"/" "$NIX_DEFAULT"
# Match both quoted hash and bare lib.fakeHash
sed -i "s|sha256 = .*|sha256 = \"${SRI_HASH}\";|" "$NIX_DEFAULT"
echo "  Updated: nix/default.nix"

# --- OpenWrt Makefile ---
OPENWRT_MK="${SCRIPT_DIR}/openwrt/net/aggregate6/Makefile"
sed -i "s/^PKG_VERSION:=.*/PKG_VERSION:=${VERSION}/" "$OPENWRT_MK"
sed -i "s/^PKG_HASH:=.*/PKG_HASH:=${SHA256}/" "$OPENWRT_MK"
echo "  Updated: openwrt/net/aggregate6/Makefile"

# --- Entware Makefile ---
ENTWARE_MK="${SCRIPT_DIR}/entware/net/aggregate6/Makefile"
sed -i "s/^PKG_VERSION:=.*/PKG_VERSION:=${VERSION}/" "$ENTWARE_MK"
sed -i "s/^PKG_HASH:=.*/PKG_HASH:=${SHA256}/" "$ENTWARE_MK"
echo "  Updated: entware/net/aggregate6/Makefile"

# --- FreeBSD ---
FREEBSD_DIR="${SCRIPT_DIR}/freebsd"
TAB=$'\t'
sed -i "s/^DISTVERSION=.*/DISTVERSION=${TAB}${VERSION}/" "${FREEBSD_DIR}/Makefile"
cat > "${FREEBSD_DIR}/distinfo" <<EOF
TIMESTAMP = $(date +%s)
SHA256 (${TARBALL}) = ${SHA256}
SIZE (${TARBALL}) = ${SIZE}
EOF
echo "  Updated: freebsd/Makefile + distinfo"

# --- Gentoo ebuild ---
# SRC_URI uses Portage variables ${PV}/${P} — no sed for URL needed.
# Copy latest ebuild to new versioned filename.
EBUILD_DIR="${SCRIPT_DIR}/gentoo/app-net/aggregate6"
NEW_EBUILD="${EBUILD_DIR}/aggregate6-${VERSION}.ebuild"
if [ ! -f "${NEW_EBUILD}" ]; then
    LATEST_EBUILD=$(find "${EBUILD_DIR}" -name 'aggregate6-*.ebuild' 2>/dev/null \
        | sort -V | tail -1)
    if [ -n "${LATEST_EBUILD}" ]; then
        cp "${LATEST_EBUILD}" "${NEW_EBUILD}"
        echo "  Created: gentoo ebuild aggregate6-${VERSION}.ebuild"
    fi
fi
# Rotation: keep only the last MAX_VERSIONS ebuilds
mapfile -t EBUILDS < <(find "${EBUILD_DIR}" -name 'aggregate6-*.ebuild' 2>/dev/null | sort -V)
while [ "${#EBUILDS[@]}" -gt "${MAX_VERSIONS}" ]; do
    echo "  Removing old ebuild: $(basename "${EBUILDS[0]}")"
    rm -f "${EBUILDS[0]}"
    EBUILDS=("${EBUILDS[@]:1}")
done

# --- Debian changelog ---
# Full history — no rotation. Prepend new entry if not already present.
CHANGELOG="${SCRIPT_DIR}/debian/changelog"
if ! head -1 "$CHANGELOG" | grep -q "(${VERSION}-1)"; then
    TIMESTAMP=$(date -R)
    TMPFILE=$(mktemp)
    cat > "$TMPFILE" <<EOF
aggregate6 (${VERSION}-1) unstable; urgency=low

  * New upstream release.

 -- 0xkee <0xkee@users.noreply.github.com>  ${TIMESTAMP}

EOF
    cat "$CHANGELOG" >> "$TMPFILE"
    mv "$TMPFILE" "$CHANGELOG"
    echo "  Updated: debian/changelog"
fi

echo "Updated all dist/ files to version ${VERSION}"
