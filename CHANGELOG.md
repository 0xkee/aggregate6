# Changelog

All notable changes to aggregate6 will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Packaging files (dist/) for 10 distributions: Entware, OpenWrt, Debian/Ubuntu, RPM (Fedora/RHEL/openSUSE), Alpine Linux, Arch Linux, Gentoo, Void Linux, NixOS, FreeBSD
- Packaging documentation (dist/README.md) with build instructions per distribution
- Comprehensive RFC-based test suite: 72 tests covering aggregation, edge cases,
  validation, IPv6 normalization, cascade, mixed v4/v6, and alignment
  (RFC 791, 1519, 3021, 4193, 4291, 4632, 5156, 5735, 5952, 6890)
- Patricia trie: `trie_insert()` with covered-prefix suppression and subtree pruning
- Patricia trie: `trie_aggregate()` — bottom-up sibling merge (post-order)
- Patricia trie: `trie_walk()` — pre-order traversal with numerically sorted output
- CIDR prefix parser with `inet_pton`, IPv4/IPv6 auto-detection, host-bit masking
- Full `aggregate_prefixes()` orchestration: parse → insert → aggregate → walk → output
- IPv4/IPv6 address family filtering via `-4`/`-6` flags

## [0.1.0] - 2025-09-02

### Added

- CLI with `-h`/`--help`, `-v`/`--version`, `-4` (IPv4 only), `-6` (IPv6 only) flags
- Read CIDR prefixes from stdin, skip empty lines and comments
- Passthrough stub for prefix aggregation (prints input unchanged)
- Patricia trie data structure stubs (`trie_create`, `trie_destroy`)
- Makefile with `all`, `clean`, `install`, `test` targets
- Cross-compilation support via `CC` variable
- Basic shell test suite
