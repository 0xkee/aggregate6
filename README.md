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

## License

GPL-3.0-or-later — see [LICENSE](LICENSE).
