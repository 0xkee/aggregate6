#!/bin/sh
# Generate per-architecture opkg Packages indices from .ipk files.
# Usage: ./scripts/generate-packages-index.sh [dir]
#
# Scans <dir> for .ipk files, extracts architecture from each control,
# organizes into <dir>/<arch>/ subdirectories, and generates
# Packages + Packages.gz in each subdirectory.
#
# Example result:
#   dev/
#     mipsel-3.4/
#       aggregate6_0.3.1-1_mipsel-3.4.ipk
#       Packages
#       Packages.gz
#     aarch64-3.10/
#       ...
set -eu

DIR="${1:-.}"

if [ ! -d "$DIR" ]; then
    echo "ERROR: Directory not found: $DIR" >&2
    exit 1
fi

total=0

# Phase 1: Organize .ipk files into per-arch subdirectories
for ipk in "$DIR"/*.ipk; do
    [ -e "$ipk" ] || continue

    # Extract architecture from control inside .ipk
    tmpdir="$(mktemp -d)"
    tar -xzf "$ipk" -C "$tmpdir" ./control.tar.gz
    tar -xzf "$tmpdir/control.tar.gz" -C "$tmpdir" ./control
    arch=$(sed -n 's/^Architecture: *//p' "$tmpdir/control")
    rm -rf "$tmpdir"

    if [ -z "$arch" ]; then
        echo "WARNING: No Architecture in $ipk, skipping" >&2
        continue
    fi

    mkdir -p "$DIR/$arch"
    cp "$ipk" "$DIR/$arch/"
    rm -f "$ipk"
done

# Phase 2: Generate Packages index in each arch subdirectory
for archdir in "$DIR"/*/; do
    [ -d "$archdir" ] || continue

    packages_file="${archdir}Packages"
    : > "$packages_file"
    count=0

    for ipk in "$archdir"*.ipk; do
        [ -e "$ipk" ] || continue

        tmpdir="$(mktemp -d)"
        tar -xzf "$ipk" -C "$tmpdir" ./control.tar.gz
        tar -xzf "$tmpdir/control.tar.gz" -C "$tmpdir" ./control

        control_content="$(cat "$tmpdir/control")"
        filename="$(basename "$ipk")"
        size="$(stat -c%s "$ipk")"
        md5="$(md5sum "$ipk" | cut -d' ' -f1)"
        sha256="$(sha256sum "$ipk" | cut -d' ' -f1)"

        {
            printf "%s\n" "$control_content"
            printf "Filename: %s\n" "$filename"
            printf "Size: %s\n" "$size"
            printf "MD5sum: %s\n" "$md5"
            printf "SHA256sum: %s\n" "$sha256"
            printf "\n"
        } >> "$packages_file"

        rm -rf "$tmpdir"
        count=$((count + 1))
    done

    gzip -k -f "$packages_file"

    archname="$(basename "$archdir")"
    echo "  ${archname}: ${count} package(s)"
    total=$((total + count))
done

# Phase 3: Clean up any leftover flat Packages files
rm -f "$DIR/Packages" "$DIR/Packages.gz"

echo "Generated per-arch indices: ${total} package(s) total in $DIR"
