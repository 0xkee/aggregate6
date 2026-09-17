# tests/

Functional test suite for `aggregate6` — validates CIDR aggregation correctness
against RFC 791, 950, 1519, 3021, 4193, 4271, 4291, 4632, 5156, 5735, 5952, 6890.

## Running

```sh
make test
# or directly:
sh tests/test-basic.sh ./aggregate6
```

The binary path is passed as the first argument. Exit code `1` if any test fails.

## Test file

### [`test-basic.sh`](test-basic.sh)

Single self-contained test script (~55 checks). Each test pipes prefixes into
`aggregate6` and compares stdout against expected output.

## Test sections

### A — Basic IPv4 aggregation (RFC 4632, RFC 1519)

| ID | What is tested |
|---|---|
| A1.1–A1.7 | Adjacent prefix merging (`/24`→`/23`, `/25`→`/24`, four `/24`→`/22`, eight `/24`→`/21`, `/32`→`/31`), non-adjacent and unaligned non-merge |
| A2.1–A2.4 | Supernet absorbs subnets, 3-level nesting |
| A3.1–A3.2 | Exact duplicates removed, duplicates + merge |
| A4.1–A4.3 | Single prefix passthrough, unrelated prefixes preserved, empty input |

### B — Edge cases per RFC

| ID | What is tested |
|---|---|
| B1.1–B1.4 | Host routes: IPv4 `/32` and IPv6 `/128` passthrough, adjacent host routes merge to `/31` and `/127` (RFC 3021, RFC 4291) |
| B2.1–B2.4 | Default route (`0.0.0.0/0`, `::/0`) absorbs everything in its address family; dual-stack defaults stay separate |
| B3.1–B3.6 | Min/max prefix length: `/0` and `/32` (IPv4), `/0` and `/128` (IPv6); two `/1` halves → `/0` |
| B4.1–B4.8 | Special addresses: loopback, link-local, RFC 1918 blocks, multicast `/4` merge (IPv4); loopback, link-local, ULA `fc00::/7` merge, multicast (IPv6) |

### C — Input validation

| ID | What is tested |
|---|---|
| C1.1–C1.7 | Invalid input skipped: garbage text, octet > 255, negative mask, mask > 32 (IPv4) / > 128 (IPv6), empty lines, comments (`#`) |
| C2.1–C2.4 | Host bits masked off: `192.168.1.1/24` → `192.168.1.0/24`, IPv6 host bits, host-bits + aggregation combo |

### D — IPv6 normalization (RFC 5952, RFC 4291)

| ID | What is tested |
|---|---|
| D1.1–D1.6 | Canonical output: full-form → compressed, lowercase enforced, leading zeros stripped, `::` expansion at start/end |
| D2.1–D2.4 | Address types: link-local merge, ULA merge, GUA merge; mixed types don't cross-merge |

### E — Cascade aggregation (RFC 4632)

| ID | What is tested |
|---|---|
| E1.1–E1.5 | Multi-level cascade: four `/26` → `/24`, four `/32` → `/30`, partial cascade, IPv6 `/128` → `/126` |
| E2.2–E2.3 | Stress: all 256 `/32` in a `/24` → single `/24`; interleaved aggregatable prefixes from different ranges |

### F — Mixed IPv4/IPv6

| ID | What is tested |
|---|---|
| F1 | Simultaneous v4 + v6 aggregation in single stream |
| F2 | `-4` flag filters out IPv6 |
| F3 | `-6` flag filters out IPv4 |
| F4 | Output sort order: IPv4 before IPv6 |

### G — Alignment (RFC 4632)

| ID | What is tested |
|---|---|
| G1 | Unaligned adjacent `/24` blocks don't merge |
| G2 | Aligned adjacent `/24` blocks merge to `/23` |
| G3 | 3 of 4 blocks in a `/22` — partial merge only |
| G4 | IPv6 unaligned blocks don't merge |
