# Gentoo Overlay — aggregate6

This directory is a complete Gentoo overlay for installing `aggregate6`.

## Structure

```
dist/gentoo/
├── metadata/
│   └── layout.conf         # Overlay configuration
├── profiles/
│   ├── repo_name           # Overlay name: "aggregate6"
│   └── categories          # Category list: app-net
└── app-net/
    └── aggregate6/
        ├── aggregate6-*.ebuild   # Versioned ebuilds
        └── metadata.xml          # Package metadata
```

## Installation

### Method 1: eselect repository (quickest)

> **Note:** This pulls the entire `aggregate6` git repository, not just the overlay.
> Consider Method 3 for a leaner setup.

```bash
eselect repository add aggregate6 git https://github.com/0xkee/aggregate6.git
# Since the overlay lives in dist/gentoo/, configure sync-subdir:
cat > /etc/portage/repos.conf/aggregate6.conf <<'EOF'
[aggregate6]
location = /var/db/repos/aggregate6
sync-type = git
sync-uri = https://github.com/0xkee/aggregate6.git
auto-sync = yes
EOF
# Note: Portage doesn't support subdirectory sync natively.
# See Method 3 for a workaround using symlinks.
```

### Method 2: Manual repos.conf

Create `/etc/portage/repos.conf/aggregate6.conf`:

```ini
[aggregate6]
location = /var/db/repos/aggregate6
sync-type = git
sync-uri = https://github.com/0xkee/aggregate6.git
auto-sync = yes
```

> **Sparse checkout note:** Portage's git sync doesn't support subdirectory checkout.
> The full repo will be cloned. If you want only the overlay files, use Method 3.

### Method 3: Local overlay via symlink (recommended)

```bash
# Clone the repository
git clone --depth=1 https://github.com/0xkee/aggregate6.git /tmp/aggregate6

# Symlink the overlay directory
ln -s /tmp/aggregate6/dist/gentoo /var/db/repos/aggregate6

# Or copy instead of symlink
cp -r /tmp/aggregate6/dist/gentoo /var/db/repos/aggregate6
```

Then create `/etc/portage/repos.conf/aggregate6.conf`:

```ini
[aggregate6]
location = /var/db/repos/aggregate6
auto-sync = false
```

### Emerge

```bash
emerge --sync aggregate6   # if using sync method
emerge app-net/aggregate6
```

## Version Management

Ebuild files are versioned by filename (e.g., `aggregate6-0.1.0.ebuild`).
The `dist/update-dist.sh` script automatically creates new versioned ebuilds
and rotates old ones (keeping the last 3 versions).
