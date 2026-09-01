# Changelog

All notable changes to aggregate6 will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.1.0] - 2025-09-02

### Added

- CLI with `-h`/`--help`, `-v`/`--version`, `-4` (IPv4 only), `-6` (IPv6 only) flags
- Read CIDR prefixes from stdin, skip empty lines and comments
- Passthrough stub for prefix aggregation (prints input unchanged)
- Patricia trie data structure stubs (`trie_create`, `trie_destroy`)
- Makefile with `all`, `clean`, `install`, `test` targets
- Cross-compilation support via `CC` variable
- Basic shell test suite
