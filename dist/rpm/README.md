# RPM / COPR — aggregate6

RPM package for Fedora, RHEL, CentOS and other RPM-based distributions.
Built via [Fedora COPR](https://copr.fedorainfracloud.org/).

## Installation from COPR

```bash
# Fedora
sudo dnf copr enable 0xkee/aggregate6
sudo dnf install aggregate6

# RHEL/CentOS (with EPEL)
sudo dnf copr enable 0xkee/aggregate6
sudo dnf install aggregate6
```

## Manual repo file

Alternatively, copy `aggregate6.repo` to `/etc/yum.repos.d/`:

```bash
sudo cp dist/rpm/aggregate6.repo /etc/yum.repos.d/
sudo dnf install aggregate6
```

## Building from spec

```bash
# Install build dependencies
sudo dnf install rpm-build gcc make

# Download source tarball
VERSION=0.1.0
spectool -g -R dist/rpm/aggregate6.spec

# Build RPM
rpmbuild -ba dist/rpm/aggregate6.spec \
  --define "_sourcedir $(pwd)"
```

Or using `mock` for clean chroot builds:

```bash
rpmbuild -bs dist/rpm/aggregate6.spec --define "_sourcedir $(pwd)"
mock ~/rpmbuild/SRPMS/aggregate6-*.src.rpm
```

## COPR Maintainer Setup

1. Create a project at <https://copr.fedorainfracloud.org>
   - Project name: `aggregate6`
   - Chroots: select desired Fedora/EPEL versions

2. Configure SCM integration:
   - Source type: **SCM**
   - Clone URL: `https://github.com/0xkee/aggregate6.git`
   - Subdir: (leave empty — `.copr/Makefile` is in repo root)
   - Spec file: `dist/rpm/aggregate6.spec`
   - The `.copr/Makefile` in the repository root handles SRPM creation automatically.

3. Enable auto-rebuild:
   - Set webhook or use COPR's built-in SCM polling

## Files

| File | Description |
|------|-------------|
| `aggregate6.spec` | RPM spec file |
| `aggregate6.repo` | Yum/DNF repo configuration template |
| `../../.copr/Makefile` | COPR SRPM build integration (in repo root) |
