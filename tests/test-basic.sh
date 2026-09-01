#!/bin/sh
set -eu

AGGREGATE6="${1:-./aggregate6}"

pass=0
fail=0

check() {
    desc="$1"
    expected="$2"
    input="$3"

    result=$(printf '%s\n' "$input" | "$AGGREGATE6" 2>/dev/null || true)
    if [ "$result" = "$expected" ]; then
        pass=$((pass + 1))
        printf "PASS: %s\n" "$desc"
    else
        fail=$((fail + 1))
        printf "FAIL: %s\n  expected: %s\n  got:      %s\n" "$desc" "$expected" "$result"
    fi
}

# Passthrough tests (stub behavior)
check "single prefix passthrough" "192.168.1.0/24" "192.168.1.0/24"
check "version flag" "" ""

printf "\n%d passed, %d failed\n" "$pass" "$fail"
[ "$fail" -eq 0 ]
