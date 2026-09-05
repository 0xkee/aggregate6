/*
 * aggregate6 — fast multi-threaded IPv4/IPv6 CIDR prefix aggregation tool
 * Copyright (c) 2026 0xkee
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef PREFIX_H
#define PREFIX_H

#include <stddef.h>
#include <stdint.h>

/**
 * Compact IPv4 prefix — 8 bytes (4+1 + padding).
 * addr is in host byte order, host bits zeroed.
 */
struct prefix4 {
    uint32_t addr;
    uint8_t len;
};

/**
 * Compact IPv6 prefix — 24 bytes (8+8+1 + padding).
 * hi/lo in host byte order, host bits zeroed.
 */
struct prefix6 {
    uint64_t hi;
    uint64_t lo;
    uint8_t len;
};

/**
 * Dynamic arrays for IPv4 and IPv6 prefixes.
 */
struct prefix_arrays {
    struct prefix4 *v4;
    size_t v4_count;
    size_t v4_cap;
    struct prefix6 *v6;
    size_t v6_count;
    size_t v6_cap;
};

int pa_init(struct prefix_arrays *pa);
void pa_free(struct prefix_arrays *pa);

/**
 * Pre-allocate arrays to avoid realloc during parsing.
 * Call after pa_init with an estimated count.
 */
void pa_reserve(struct prefix_arrays *pa,
                size_t v4_hint, size_t v6_hint);

/**
 * Parse a NUL-terminated CIDR line and add to array.
 */
int pa_add_line(struct prefix_arrays *pa, const char *line,
                int skip_v4, int skip_v6);

/**
 * Parse a memory buffer (e.g. mmap'd file) with
 * newline-delimited CIDR prefixes. Modifies buf in-place
 * (replaces newlines with NUL).
 */
int pa_add_buf(struct prefix_arrays *pa, char *buf,
               size_t len, int skip_v4, int skip_v6);

/**
 * Sort, deduplicate, and merge siblings in-place.
 * Updates *count to the new array size.
 */
void prefix4_aggregate(struct prefix4 *arr, size_t *count);
void prefix6_aggregate(struct prefix6 *arr, size_t *count);

/**
 * Format prefix to string buffer.
 *
 * @return Number of characters written (excl. NUL).
 */
int prefix4_format(const struct prefix4 *p, char *buf,
                   size_t bufsz);
int prefix6_format(const struct prefix6 *p, char *buf,
                   size_t bufsz);

#endif /* PREFIX_H */
