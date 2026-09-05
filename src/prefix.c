/*
 * aggregate6 — fast multi-threaded IPv4/IPv6 CIDR prefix aggregation tool
 * Copyright (c) 2026 0xkee
 * SPDX-License-Identifier: GPL-3.0-only
 */

#define _POSIX_C_SOURCE 200809L

#include "prefix.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAP 4096

/* ----------------------------------------------------------------
 * IPv4 — radix sort, dedup, merge
 * ---------------------------------------------------------------- */

/** Does 'a' contain 'b'? Both must have host bits zeroed. */
static int p4_contains(const struct prefix4 *a,
                       const struct prefix4 *b)
{
    uint32_t mask;

    if (a->len > b->len)
        return 0;
    if (a->len == 0)
        return 1;
    mask = ~0U << (32 - a->len);
    return a->addr == (b->addr & mask);
}

static int p4_siblings(const struct prefix4 *a,
                       const struct prefix4 *b)
{
    if (a->len != b->len || a->len == 0)
        return 0;
    return (a->addr ^ b->addr) == (1U << (32 - a->len));
}

/**
 * 5-pass LSD radix sort for prefix4 by (addr, len).
 * Pass 0: sort by len (LSB of sort key).
 * Passes 1-4: sort by addr bytes (LSB to MSB).
 * Stable sort guarantees correct (addr, len) ordering.
 * Requires tmp array of same size as arr.
 */
static void radix_sort_p4(struct prefix4 *arr, size_t n,
                           struct prefix4 *tmp)
{
    size_t count[256];
    size_t i, total, c;
    int pass, j;
    struct prefix4 *src, *dst;

    for (pass = 0; pass < 5; pass++) {
        src = (pass & 1) ? tmp : arr;
        dst = (pass & 1) ? arr : tmp;
        memset(count, 0, sizeof(count));

        for (i = 0; i < n; i++) {
            uint8_t k = (pass == 0)
                ? src[i].len
                : (uint8_t)(src[i].addr >> ((pass - 1) * 8));
            count[k]++;
        }

        total = 0;
        for (j = 0; j < 256; j++) {
            c = count[j];
            count[j] = total;
            total += c;
        }

        for (i = 0; i < n; i++) {
            uint8_t k = (pass == 0)
                ? src[i].len
                : (uint8_t)(src[i].addr >> ((pass - 1) * 8));
            dst[count[k]++] = src[i];
        }
    }
    /* 5 passes (odd) → result in tmp; copy back */
    memcpy(arr, tmp, n * sizeof(*arr));
}

static size_t p4_dedup(struct prefix4 *arr, size_t n)
{
    size_t w = 0, r;

    for (r = 0; r < n; r++) {
        if (w > 0 && p4_contains(&arr[w - 1], &arr[r]))
            continue;
        arr[w++] = arr[r];
    }
    return w;
}

static size_t p4_merge_pass(struct prefix4 *arr, size_t n,
                            int *merged)
{
    size_t w = 0, r = 0;

    *merged = 0;
    while (r < n) {
        if (r + 1 < n && p4_siblings(&arr[r], &arr[r + 1])) {
            arr[w].addr = arr[r].addr &
                          (~0U << (33 - arr[r].len));
            arr[w].len = arr[r].len - 1;
            w++;
            r += 2;
            *merged = 1;
        } else {
            arr[w++] = arr[r++];
        }
    }
    return w;
}

void prefix4_aggregate(struct prefix4 *arr, size_t *count)
{
    size_t n = *count;
    struct prefix4 *tmp;
    int merged;

    if (n < 2) {
        *count = n;
        return;
    }

    tmp = malloc(n * sizeof(*tmp));
    if (tmp) {
        radix_sort_p4(arr, n, tmp);
        free(tmp);
    } else {
        /* fallback: qsort if malloc fails */
        qsort(arr, n, sizeof(*arr),
              (int (*)(const void *, const void *))p4_contains);
    }

    n = p4_dedup(arr, n);
    do {
        n = p4_merge_pass(arr, n, &merged);
    } while (merged);
    *count = n;
}

/* ----------------------------------------------------------------
 * IPv6 — qsort, dedup, merge
 * ---------------------------------------------------------------- */

static int cmp_p6(const void *a, const void *b)
{
    const struct prefix6 *pa = a, *pb = b;

    if (pa->hi != pb->hi)
        return pa->hi < pb->hi ? -1 : 1;
    if (pa->lo != pb->lo)
        return pa->lo < pb->lo ? -1 : 1;
    return (int)pa->len - (int)pb->len;
}

static int p6_contains(const struct prefix6 *a,
                       const struct prefix6 *b)
{
    uint64_t mask;

    if (a->len > b->len)
        return 0;
    if (a->len == 0)
        return 1;
    if (a->len <= 64) {
        mask = ~0ULL << (64 - a->len);
        return a->hi == (b->hi & mask);
    }
    if (a->hi != b->hi)
        return 0;
    mask = ~0ULL << (128 - a->len);
    return a->lo == (b->lo & mask);
}

static int p6_siblings(const struct prefix6 *a,
                       const struct prefix6 *b)
{
    if (a->len != b->len || a->len == 0)
        return 0;
    if (a->len <= 64) {
        if (a->lo != b->lo)
            return 0;
        return (a->hi ^ b->hi) ==
               (1ULL << (64 - a->len));
    }
    if (a->hi != b->hi)
        return 0;
    return (a->lo ^ b->lo) ==
           (1ULL << (128 - a->len));
}

static size_t p6_dedup(struct prefix6 *arr, size_t n)
{
    size_t w = 0, r;

    for (r = 0; r < n; r++) {
        if (w > 0 && p6_contains(&arr[w - 1], &arr[r]))
            continue;
        arr[w++] = arr[r];
    }
    return w;
}

static size_t p6_merge_pass(struct prefix6 *arr, size_t n,
                            int *merged)
{
    size_t w = 0, r = 0;
    uint64_t bit;

    *merged = 0;
    while (r < n) {
        if (r + 1 < n &&
            p6_siblings(&arr[r], &arr[r + 1])) {
            arr[w] = arr[r];
            if (arr[r].len <= 64) {
                bit = 1ULL << (64 - arr[r].len);
                arr[w].hi &= ~bit;
            } else {
                bit = 1ULL << (128 - arr[r].len);
                arr[w].lo &= ~bit;
            }
            arr[w].len--;
            w++;
            r += 2;
            *merged = 1;
        } else {
            arr[w++] = arr[r++];
        }
    }
    return w;
}

void prefix6_aggregate(struct prefix6 *arr, size_t *count)
{
    size_t n = *count;
    int merged;

    if (n < 2) {
        *count = n;
        return;
    }
    qsort(arr, n, sizeof(*arr), cmp_p6);
    n = p6_dedup(arr, n);
    do {
        n = p6_merge_pass(arr, n, &merged);
    } while (merged);
    *count = n;
}

/* ----------------------------------------------------------------
 * Fast IPv4 CIDR parser
 * ---------------------------------------------------------------- */

static int parse_v4(const char *s, struct prefix4 *out)
{
    unsigned a = 0, b = 0, c = 0, d = 0, pfx;
    const char *p = s;

    while (*p >= '0' && *p <= '9') a = a * 10 + (unsigned)(*p++ - '0');
    if (*p++ != '.' || a > 255) return -1;
    while (*p >= '0' && *p <= '9') b = b * 10 + (unsigned)(*p++ - '0');
    if (*p++ != '.' || b > 255) return -1;
    while (*p >= '0' && *p <= '9') c = c * 10 + (unsigned)(*p++ - '0');
    if (*p++ != '.' || c > 255) return -1;
    while (*p >= '0' && *p <= '9') d = d * 10 + (unsigned)(*p++ - '0');
    if (d > 255) return -1;

    out->addr = (a << 24) | (b << 16) | (c << 8) | d;

    if (*p == '/') {
        p++;
        pfx = 0;
        while (*p >= '0' && *p <= '9')
            pfx = pfx * 10 + (unsigned)(*p++ - '0');
        if (pfx > 32 || *p != '\0')
            return -1;
        out->len = (uint8_t)pfx;
    } else if (*p == '\0') {
        out->len = 32;
    } else {
        return -1;
    }

    if (out->len < 32)
        out->addr &= ~0U << (32 - out->len);
    else if (out->len == 0)
        out->addr = 0;

    return 0;
}

/* ----------------------------------------------------------------
 * IPv6 parser (inet_pton — complex syntax)
 * ---------------------------------------------------------------- */

static int parse_v6(const char *s, struct prefix6 *out)
{
    char buf[64];
    const char *slash;
    uint8_t raw[16];
    int pfx, i;

    slash = strchr(s, '/');
    if (slash) {
        size_t hlen = (size_t)(slash - s);
        if (hlen >= sizeof(buf))
            return -1;
        memcpy(buf, s, hlen);
        buf[hlen] = '\0';
        pfx = atoi(slash + 1);
        if (pfx < 0 || pfx > 128)
            return -1;
    } else {
        if (strlen(s) >= sizeof(buf))
            return -1;
        strcpy(buf, s);
        pfx = 128;
    }

    if (inet_pton(AF_INET6, buf, raw) != 1)
        return -1;

    out->hi = 0;
    out->lo = 0;
    for (i = 0; i < 8; i++)
        out->hi |= (uint64_t)raw[i] << (56 - i * 8);
    for (i = 0; i < 8; i++)
        out->lo |= (uint64_t)raw[8 + i] << (56 - i * 8);
    out->len = (uint8_t)pfx;

    if (pfx == 0) {
        out->hi = 0;
        out->lo = 0;
    } else if (pfx <= 64) {
        out->hi &= ~0ULL << (64 - pfx);
        out->lo = 0;
    } else if (pfx < 128) {
        out->lo &= ~0ULL << (128 - pfx);
    }

    return 0;
}

/* ----------------------------------------------------------------
 * Array management
 * ---------------------------------------------------------------- */

int pa_init(struct prefix_arrays *pa)
{
    memset(pa, 0, sizeof(*pa));
    pa->v4_cap = INITIAL_CAP;
    pa->v6_cap = INITIAL_CAP;
    pa->v4 = malloc(pa->v4_cap * sizeof(*pa->v4));
    pa->v6 = malloc(pa->v6_cap * sizeof(*pa->v6));
    if (!pa->v4 || !pa->v6) {
        free(pa->v4);
        free(pa->v6);
        return -1;
    }
    return 0;
}

void pa_free(struct prefix_arrays *pa)
{
    free(pa->v4);
    free(pa->v6);
    memset(pa, 0, sizeof(*pa));
}

void pa_reserve(struct prefix_arrays *pa,
                size_t v4_hint, size_t v6_hint)
{
    if (v4_hint > pa->v4_cap) {
        pa->v4 = realloc(pa->v4, v4_hint * sizeof(*pa->v4));
        if (pa->v4)
            pa->v4_cap = v4_hint;
    }
    if (v6_hint > pa->v6_cap) {
        pa->v6 = realloc(pa->v6, v6_hint * sizeof(*pa->v6));
        if (pa->v6)
            pa->v6_cap = v6_hint;
    }
}

static int pa_push_v4(struct prefix_arrays *pa,
                      const struct prefix4 *p)
{
    if (pa->v4_count >= pa->v4_cap) {
        pa->v4_cap *= 2;
        pa->v4 = realloc(pa->v4,
                         pa->v4_cap * sizeof(*pa->v4));
        if (!pa->v4)
            return -1;
    }
    pa->v4[pa->v4_count++] = *p;
    return 0;
}

static int pa_push_v6(struct prefix_arrays *pa,
                      const struct prefix6 *p)
{
    if (pa->v6_count >= pa->v6_cap) {
        pa->v6_cap *= 2;
        pa->v6 = realloc(pa->v6,
                         pa->v6_cap * sizeof(*pa->v6));
        if (!pa->v6)
            return -1;
    }
    pa->v6[pa->v6_count++] = *p;
    return 0;
}

int pa_add_line(struct prefix_arrays *pa, const char *line,
                int skip_v4, int skip_v6)
{
    int is_v6 = (strchr(line, ':') != NULL);

    if (is_v6) {
        struct prefix6 p;
        if (skip_v6)
            return 0;
        if (parse_v6(line, &p) < 0) {
            fprintf(stderr, "warning: bad prefix: %s\n",
                    line);
            return 0;
        }
        return pa_push_v6(pa, &p);
    } else {
        struct prefix4 p;
        if (skip_v4)
            return 0;
        if (parse_v4(line, &p) < 0) {
            fprintf(stderr, "warning: bad prefix: %s\n",
                    line);
            return 0;
        }
        return pa_push_v4(pa, &p);
    }
}

int pa_add_buf(struct prefix_arrays *pa, char *buf,
               size_t len, int skip_v4, int skip_v6)
{
    char *p = buf;
    char *end = buf + len;
    char *nl, *line;
    size_t line_len;

    while (p < end) {
        nl = memchr(p, '\n', (size_t)(end - p));
        line = p;
        line_len = nl ? (size_t)(nl - p) : (size_t)(end - p);

        /* NUL-terminate in place (mmap MAP_PRIVATE) */
        if (nl)
            *nl = '\0';
        else if (p + line_len < end)
            p[line_len] = '\0';

        /* strip \r */
        while (line_len > 0 && line[line_len - 1] == '\r')
            line[--line_len] = '\0';

        if (line_len > 0 && line[0] != '#') {
            if (pa_add_line(pa, line, skip_v4, skip_v6) < 0)
                return -1;
        }

        p = nl ? nl + 1 : end;
    }
    return 0;
}

/* ----------------------------------------------------------------
 * Formatters
 * ---------------------------------------------------------------- */

static int fmt_u8(char *buf, unsigned val)
{
    if (val >= 100) {
        buf[0] = (char)('0' + val / 100);
        buf[1] = (char)('0' + (val / 10) % 10);
        buf[2] = (char)('0' + val % 10);
        return 3;
    }
    if (val >= 10) {
        buf[0] = (char)('0' + val / 10);
        buf[1] = (char)('0' + val % 10);
        return 2;
    }
    buf[0] = (char)('0' + val);
    return 1;
}

int prefix4_format(const struct prefix4 *p, char *buf,
                   size_t bufsz)
{
    int pos = 0;

    (void)bufsz;
    pos += fmt_u8(buf + pos, (p->addr >> 24) & 0xFF);
    buf[pos++] = '.';
    pos += fmt_u8(buf + pos, (p->addr >> 16) & 0xFF);
    buf[pos++] = '.';
    pos += fmt_u8(buf + pos, (p->addr >> 8) & 0xFF);
    buf[pos++] = '.';
    pos += fmt_u8(buf + pos, p->addr & 0xFF);
    buf[pos++] = '/';
    pos += fmt_u8(buf + pos, p->len);
    buf[pos++] = '\n';
    buf[pos] = '\0';
    return pos;
}

int prefix6_format(const struct prefix6 *p, char *buf,
                   size_t bufsz)
{
    uint8_t raw[16];
    char addr[INET6_ADDRSTRLEN];
    int i;

    for (i = 0; i < 8; i++)
        raw[i] = (uint8_t)(p->hi >> (56 - i * 8));
    for (i = 0; i < 8; i++)
        raw[8 + i] = (uint8_t)(p->lo >> (56 - i * 8));

    inet_ntop(AF_INET6, raw, addr, sizeof(addr));
    return snprintf(buf, bufsz, "%s/%d\n", addr, p->len);
}
