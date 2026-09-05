#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (c) 2026 0xkee
#
# Comprehensive test suite for aggregate6
# Tests based on: RFC 791, 950, 1519, 3021, 4193, 4271, 4291, 4632,
#                 5156, 5735, 5952, 6890

set -eu

BIN="${1:?Usage: $0 <path-to-aggregate6>}"
PASS=0
FAIL=0

check() {
    desc="$1"
    expected="$2"
    actual="$3"
    if [ "$expected" = "$actual" ]; then
        PASS=$((PASS + 1))
    else
        FAIL=$((FAIL + 1))
        printf "FAIL: %s\n" "$desc"
        printf "  expected: %s\n" "$(printf '%s' "$expected" | head -5)"
        printf "  actual:   %s\n" "$(printf '%s' "$actual" | head -5)"
    fi
}

# =============================================================================
# A. Basic IPv4 aggregation (RFC 4632, RFC 1519)
# =============================================================================

# --- A1. Adjacent merging ---

check "A1.1: adjacent /24 -> /23 (RFC 4632)" \
    "$(printf '192.168.0.0/23')" \
    "$(printf '192.168.0.0/24\n192.168.1.0/24\n' | "$BIN")"

check "A1.2: adjacent /25 -> /24 (RFC 4632)" \
    "$(printf '10.0.0.0/24')" \
    "$(printf '10.0.0.0/25\n10.0.0.128/25\n' | "$BIN")"

check "A1.3: non-adjacent /24 no merge (RFC 4632)" \
    "$(printf '192.168.0.0/24\n192.168.2.0/24')" \
    "$(printf '192.168.0.0/24\n192.168.2.0/24\n' | "$BIN")"

check "A1.4: four /24 -> /22 (RFC 4632)" \
    "$(printf '10.0.0.0/22')" \
    "$(printf '10.0.0.0/24\n10.0.1.0/24\n10.0.2.0/24\n10.0.3.0/24\n' | "$BIN")"

check "A1.5: adjacent /32 -> /31 (RFC 3021)" \
    "$(printf '192.168.1.0/31')" \
    "$(printf '192.168.1.0/32\n192.168.1.1/32\n' | "$BIN")"

check "A1.6: adjacent but unaligned /24 no merge (RFC 4632)" \
    "$(printf '192.168.1.0/24\n192.168.2.0/24')" \
    "$(printf '192.168.1.0/24\n192.168.2.0/24\n' | "$BIN")"

check "A1.7: eight /24 -> /21 (RFC 4632)" \
    "$(printf '172.16.0.0/21')" \
    "$(printf '172.16.0.0/24\n172.16.1.0/24\n172.16.2.0/24\n172.16.3.0/24\n172.16.4.0/24\n172.16.5.0/24\n172.16.6.0/24\n172.16.7.0/24\n' | "$BIN")"

# --- A2. Covered/nested (RFC 4632) ---

check "A2.1: supernet covers subnet (RFC 4632)" \
    "$(printf '10.0.0.0/8')" \
    "$(printf '10.0.0.0/8\n10.1.0.0/16\n' | "$BIN")"

check "A2.2: supernet covers many subnets (RFC 4632)" \
    "$(printf '10.0.0.0/8')" \
    "$(printf '10.0.0.0/8\n10.0.0.0/16\n10.1.0.0/16\n10.0.0.0/24\n' | "$BIN")"

check "A2.3: partial overlap covered (RFC 4632)" \
    "$(printf '192.168.0.0/23')" \
    "$(printf '192.168.0.0/23\n192.168.1.0/24\n' | "$BIN")"

check "A2.4: 3-level nesting (RFC 4632)" \
    "$(printf '10.0.0.0/8')" \
    "$(printf '10.0.0.0/8\n10.0.0.0/16\n10.0.0.0/24\n10.0.0.0/32\n' | "$BIN")"

# --- A3. Duplicates ---

check "A3.1: exact duplicates" \
    "$(printf '10.0.0.0/8')" \
    "$(printf '10.0.0.0/8\n10.0.0.0/8\n10.0.0.0/8\n' | "$BIN")"

check "A3.2: duplicate + merge" \
    "$(printf '192.168.0.0/23')" \
    "$(printf '192.168.0.0/24\n192.168.0.0/24\n192.168.1.0/24\n' | "$BIN")"

# --- A4. Identity (no aggregation) ---

check "A4.1: single prefix passthrough" \
    "$(printf '10.0.0.0/8')" \
    "$(printf '10.0.0.0/8\n' | "$BIN")"

check "A4.2: unrelated prefixes (RFC 5735)" \
    "$(printf '10.0.0.0/8\n172.16.0.0/12\n192.168.0.0/16')" \
    "$(printf '10.0.0.0/8\n172.16.0.0/12\n192.168.0.0/16\n' | "$BIN")"

check "A4.3: empty input" \
    "" \
    "$(printf '' | "$BIN")"

# =============================================================================
# B. Edge cases per RFC
# =============================================================================

# --- B1. Host routes (RFC 4632, RFC 4291) ---

check "B1.1: IPv4 host route /32 passthrough (RFC 4632)" \
    "$(printf '192.168.1.1/32')" \
    "$(printf '192.168.1.1/32\n' | "$BIN")"

check "B1.2: adjacent IPv4 host routes -> /31 (RFC 3021)" \
    "$(printf '10.0.0.0/31')" \
    "$(printf '10.0.0.0/32\n10.0.0.1/32\n' | "$BIN")"

check "B1.3: IPv6 host route /128 passthrough (RFC 4291)" \
    "$(printf '2001:db8::1/128')" \
    "$(printf '2001:db8::1/128\n' | "$BIN")"

check "B1.4: adjacent IPv6 host routes -> /127 (RFC 4291)" \
    "$(printf '2001:db8::/127')" \
    "$(printf '2001:db8::/128\n2001:db8::1/128\n' | "$BIN")"

# --- B2. Default route (RFC 4632, RFC 4291) ---

check "B2.1: default route absorbs all IPv4 (RFC 4632)" \
    "$(printf '0.0.0.0/0')" \
    "$(printf '0.0.0.0/0\n10.0.0.0/8\n192.168.0.0/16\n' | "$BIN")"

check "B2.2: default route absorbs all IPv6 (RFC 4291)" \
    "$(printf '::/0')" \
    "$(printf '::/0\n2001:db8::/32\nfe80::/10\n' | "$BIN")"

check "B2.3: dual default separate (RFC 4632, RFC 4291)" \
    "$(printf '0.0.0.0/0\n::/0')" \
    "$(printf '0.0.0.0/0\n::/0\n10.0.0.0/8\n2001:db8::/32\n' | "$BIN")"

check "B2.4: default IPv4 only" \
    "$(printf '0.0.0.0/0')" \
    "$(printf '0.0.0.0/0\n' | "$BIN")"

# --- B3. Min/max prefix length (RFC 4632, RFC 4291) ---

check "B3.1: IPv4 /0 minimum (RFC 4632)" \
    "$(printf '0.0.0.0/0')" \
    "$(printf '0.0.0.0/0\n' | "$BIN")"

check "B3.2: IPv4 /32 maximum (RFC 4632)" \
    "$(printf '255.255.255.255/32')" \
    "$(printf '255.255.255.255/32\n' | "$BIN")"

check "B3.3: IPv6 /0 minimum (RFC 4291)" \
    "$(printf '::/0')" \
    "$(printf '::/0\n' | "$BIN")"

check "B3.4: IPv6 /128 maximum (RFC 4291)" \
    "$(printf 'ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff/128')" \
    "$(printf 'ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff/128\n' | "$BIN")"

check "B3.5: two IPv4 /1 halves -> /0 (RFC 4632)" \
    "$(printf '0.0.0.0/0')" \
    "$(printf '0.0.0.0/1\n128.0.0.0/1\n' | "$BIN")"

check "B3.6: two IPv6 /1 halves -> /0 (RFC 4291)" \
    "$(printf '::/0')" \
    "$(printf '::/1\n8000::/1\n' | "$BIN")"

# --- B4. Special addresses (RFC 5735, RFC 5156, RFC 6890) ---

check "B4.1: IPv4 loopback (RFC 5735)" \
    "$(printf '127.0.0.0/8')" \
    "$(printf '127.0.0.0/8\n' | "$BIN")"

check "B4.2: IPv4 link-local (RFC 5735)" \
    "$(printf '169.254.0.0/16')" \
    "$(printf '169.254.0.0/16\n' | "$BIN")"

check "B4.3: IPv4 RFC1918 blocks unrelated (RFC 5735)" \
    "$(printf '10.0.0.0/8\n172.16.0.0/12\n192.168.0.0/16')" \
    "$(printf '10.0.0.0/8\n172.16.0.0/12\n192.168.0.0/16\n' | "$BIN")"

check "B4.4: IPv4 multicast 16x /8 -> /4 (RFC 5735)" \
    "$(printf '224.0.0.0/4')" \
    "$(printf '224.0.0.0/8\n225.0.0.0/8\n226.0.0.0/8\n227.0.0.0/8\n228.0.0.0/8\n229.0.0.0/8\n230.0.0.0/8\n231.0.0.0/8\n232.0.0.0/8\n233.0.0.0/8\n234.0.0.0/8\n235.0.0.0/8\n236.0.0.0/8\n237.0.0.0/8\n238.0.0.0/8\n239.0.0.0/8\n' | "$BIN")"

check "B4.5: IPv6 loopback (RFC 5156)" \
    "$(printf '::1/128')" \
    "$(printf '::1/128\n' | "$BIN")"

check "B4.6: IPv6 link-local (RFC 5156)" \
    "$(printf 'fe80::/10')" \
    "$(printf 'fe80::/10\n' | "$BIN")"

check "B4.7: IPv6 ULA merge fc+fd -> /7 (RFC 4193)" \
    "$(printf 'fc00::/7')" \
    "$(printf 'fc00::/8\nfd00::/8\n' | "$BIN")"

check "B4.8: IPv6 multicast (RFC 5156)" \
    "$(printf 'ff00::/8')" \
    "$(printf 'ff00::/8\n' | "$BIN")"

# =============================================================================
# C. Input validation
# =============================================================================

# --- C1. Invalid input handling ---

check "C1.1: garbage input skipped" \
    "$(printf '10.0.0.0/8')" \
    "$(printf 'not_a_prefix\n10.0.0.0/8\n' | "$BIN")"

check "C1.2: invalid octet skipped (RFC 791)" \
    "$(printf '10.0.0.0/8')" \
    "$(printf '256.0.0.0/8\n10.0.0.0/8\n' | "$BIN")"

check "C1.3: negative mask skipped" \
    "$(printf '10.0.0.0/8')" \
    "$(printf '10.0.0.0/-1\n10.0.0.0/8\n' | "$BIN")"

check "C1.4: IPv4 mask > 32 skipped (RFC 4632)" \
    "$(printf '10.0.0.0/8')" \
    "$(printf '10.0.0.0/33\n10.0.0.0/8\n' | "$BIN")"

check "C1.5: IPv6 mask > 128 skipped (RFC 4291)" \
    "$(printf '2001:db8::/32')" \
    "$(printf '2001:db8::/129\n2001:db8::/32\n' | "$BIN")"

check "C1.6: empty lines skipped" \
    "$(printf '10.0.0.0/8')" \
    "$(printf '\n\n10.0.0.0/8\n\n' | "$BIN")"

check "C1.7: comments skipped" \
    "$(printf '10.0.0.0/8')" \
    "$(printf '# comment\n10.0.0.0/8\n# another comment\n' | "$BIN")"

# --- C2. Host bits (RFC 4632) ---

check "C2.1: IPv4 host bits masked (RFC 4632)" \
    "$(printf '192.168.1.0/24')" \
    "$(printf '192.168.1.1/24\n' | "$BIN")"

check "C2.2: IPv4 all host bits masked (RFC 4632)" \
    "$(printf '192.168.1.0/24')" \
    "$(printf '192.168.1.255/24\n' | "$BIN")"

check "C2.3: IPv6 host bits masked (RFC 4291)" \
    "$(printf '2001:db8::/32')" \
    "$(printf '2001:db8::1/32\n' | "$BIN")"

check "C2.4: host bits + aggregation (RFC 4632)" \
    "$(printf '192.168.0.0/23')" \
    "$(printf '192.168.0.1/24\n192.168.1.255/24\n' | "$BIN")"

# =============================================================================
# D. IPv6 normalization (RFC 5952, RFC 4291)
# =============================================================================

# --- D1. Canonical form ---

check "D1.1: full form -> shortened (RFC 5952)" \
    "$(printf '2001:db8::/32')" \
    "$(printf '2001:0db8:0000:0000:0000:0000:0000:0000/32\n' | "$BIN")"

check "D1.3: lowercase required (RFC 5952)" \
    "$(printf '2001:db8::/32')" \
    "$(printf '2001:0DB8::/32\n' | "$BIN")"

check "D1.4: leading zeros stripped (RFC 5952)" \
    "$(printf '2001:db8:1::/48')" \
    "$(printf '2001:0db8:0001::/48\n' | "$BIN")"

check "D1.5: :: at start (RFC 4291)" \
    "$(printf '::1/128')" \
    "$(printf '0000:0000:0000:0000:0000:0000:0000:0001/128\n' | "$BIN")"

check "D1.6: :: at end (RFC 5952)" \
    "$(printf '2001:db8::/32')" \
    "$(printf '2001:db8::/32\n' | "$BIN")"

# --- D2. IPv6 address types (RFC 4193, RFC 3587, RFC 5156) ---

check "D2.1: link-local merge /16 (RFC 5156)" \
    "$(printf 'fe80::/15')" \
    "$(printf 'fe80::/16\nfe81::/16\n' | "$BIN")"

check "D2.2: ULA merge /16 (RFC 4193)" \
    "$(printf 'fd00::/15')" \
    "$(printf 'fd00::/16\nfd01::/16\n' | "$BIN")"

check "D2.3: GUA merge /32 (RFC 3587)" \
    "$(printf '2001:db8::/31')" \
    "$(printf '2001:db8::/32\n2001:db9::/32\n' | "$BIN")"

check "D2.4: mixed types no cross-merge" \
    "$(printf '2001:db8::/32\nfd00::/8\nfe80::/10')" \
    "$(printf '2001:db8::/32\nfe80::/10\nfd00::/8\n' | "$BIN")"

# =============================================================================
# E. Cascade aggregation (RFC 4632)
# =============================================================================

# --- E1. Multi-level cascade ---

check "E1.1: cascade /26 -> /24 (RFC 4632)" \
    "$(printf '10.0.0.0/24')" \
    "$(printf '10.0.0.0/26\n10.0.0.64/26\n10.0.0.128/26\n10.0.0.192/26\n' | "$BIN")"

check "E1.2: cascade /32 -> /30 (RFC 4632)" \
    "$(printf '10.0.0.0/30')" \
    "$(printf '10.0.0.0/32\n10.0.0.1/32\n10.0.0.2/32\n10.0.0.3/32\n' | "$BIN")"

check "E1.4: partial cascade (RFC 4632)" \
    "$(printf '10.0.0.0/25\n10.0.0.128/26')" \
    "$(printf '10.0.0.0/26\n10.0.0.64/26\n10.0.0.128/26\n' | "$BIN")"

check "E1.5: IPv6 cascade /128 -> /126 (RFC 4291)" \
    "$(printf '2001:db8::/126')" \
    "$(printf '2001:db8::/128\n2001:db8::1/128\n2001:db8::2/128\n2001:db8::3/128\n' | "$BIN")"

# --- E2. Stress tests (programmatic input) ---

check "E2.2: all /32 in 192.168.0.0/24 -> /24 (RFC 4632)" \
    "$(printf '192.168.0.0/24')" \
    "$(seq 0 255 | while read i; do printf '192.168.0.%d/32\n' "$i"; done | "$BIN")"

check "E2.3: interleaved aggregatable" \
    "$(printf '10.0.0.0/23\n172.16.0.0/23')" \
    "$(printf '10.0.0.0/24\n10.0.1.0/24\n172.16.0.0/24\n172.16.1.0/24\n' | "$BIN")"

# =============================================================================
# F. Mixed IPv4/IPv6
# =============================================================================

check "F1: mixed v4+v6 separate aggregation" \
    "$(printf '192.168.0.0/23\n2001:db8::/47')" \
    "$(printf '192.168.0.0/24\n192.168.1.0/24\n2001:db8::/48\n2001:db8:1::/48\n' | "$BIN")"

check "F2: flag -4 filters IPv6" \
    "$(printf '192.168.0.0/24')" \
    "$(printf '192.168.0.0/24\n2001:db8::/32\n' | "$BIN" -4)"

check "F3: flag -6 filters IPv4" \
    "$(printf '2001:db8::/32')" \
    "$(printf '192.168.0.0/24\n2001:db8::/32\n' | "$BIN" -6)"

check "F4: sort order IPv4 before IPv6" \
    "$(printf '10.0.0.0/8\n2001:db8::/32')" \
    "$(printf '2001:db8::/32\n10.0.0.0/8\n' | "$BIN")"

# =============================================================================
# G. Alignment (RFC 4632)
# =============================================================================

check "G1: unaligned adjacent no merge (RFC 4632)" \
    "$(printf '192.168.1.0/24\n192.168.2.0/24')" \
    "$(printf '192.168.1.0/24\n192.168.2.0/24\n' | "$BIN")"

check "G2: aligned adjacent merge (RFC 4632)" \
    "$(printf '192.168.0.0/23')" \
    "$(printf '192.168.0.0/24\n192.168.1.0/24\n' | "$BIN")"

check "G3: 3 of 4 in /22 block (RFC 4632)" \
    "$(printf '10.0.0.0/23\n10.0.2.0/24')" \
    "$(printf '10.0.0.0/24\n10.0.1.0/24\n10.0.2.0/24\n' | "$BIN")"

check "G4: IPv6 unaligned no merge (RFC 4291)" \
    "$(printf '2001:db8:1::/48\n2001:db8:2::/48')" \
    "$(printf '2001:db8:1::/48\n2001:db8:2::/48\n' | "$BIN")"

# =============================================================================
# Results
# =============================================================================

printf "\n%d passed, %d failed\n" "$PASS" "$FAIL"
[ "$FAIL" -eq 0 ] || exit 1
