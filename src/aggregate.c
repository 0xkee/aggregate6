/*
 * aggregate6 — fast CIDR prefix aggregation tool
 * Copyright (c) 2025 0xkee
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define _POSIX_C_SOURCE 200809L

#include "aggregate.h"
#include "trie.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PFXLEN_V4  32
#define MAX_PFXLEN_V6  128
#define ADDR_BUF_SIZE  64

/**
 * Zero out host bits beyond prefixlen in addr.
 *
 * @param addr      Address bytes (modified in-place).
 * @param addr_len  Total byte length (4 or 16).
 * @param prefixlen Prefix length in bits.
 */
static void mask_host_bits(uint8_t *addr, int addr_len,
                           int prefixlen)
{
    int byte_idx = prefixlen / 8;
    int bit_off  = prefixlen % 8;

    if (bit_off > 0) {
        addr[byte_idx] &=
            (uint8_t)~((1 << (8 - bit_off)) - 1);
        byte_idx++;
    }
    if (byte_idx < addr_len)
        memset(addr + byte_idx, 0, (size_t)(addr_len - byte_idx));
}

/**
 * Parse a single CIDR prefix string and insert into the
 * appropriate trie.
 *
 * @return 0 on success (or skip), -1 only on fatal error.
 */
static int parse_and_insert(const char *str, struct trie *t4,
                            struct trie *t6, int flags)
{
    char buf[ADDR_BUF_SIZE];
    const char *slash;
    int af, maxbits, prefixlen, addr_len;
    uint8_t addr[16];
    struct trie *t;

    slash = strchr(str, '/');
    if (slash) {
        size_t host_len = (size_t)(slash - str);
        if (host_len >= sizeof(buf)) {
            fprintf(stderr, "warning: prefix too long: %s\n",
                    str);
            return 0;
        }
        memcpy(buf, str, host_len);
        buf[host_len] = '\0';
        prefixlen = atoi(slash + 1);
    } else {
        if (strlen(str) >= sizeof(buf)) {
            fprintf(stderr, "warning: prefix too long: %s\n",
                    str);
            return 0;
        }
        strcpy(buf, str);
        prefixlen = -1; /* determined below */
    }

    af = strchr(buf, ':') ? AF_INET6 : AF_INET;

    if (af == AF_INET6) {
        maxbits  = MAX_PFXLEN_V6;
        addr_len = 16;
        t = t6;
        if (prefixlen < 0)
            prefixlen = MAX_PFXLEN_V6;
    } else {
        maxbits  = MAX_PFXLEN_V4;
        addr_len = 4;
        t = t4;
        if (prefixlen < 0)
            prefixlen = MAX_PFXLEN_V4;
    }

    /* apply address family filter */
    if ((flags & AGG_IPV4_ONLY) && af == AF_INET6)
        return 0;
    if ((flags & AGG_IPV6_ONLY) && af == AF_INET)
        return 0;

    if (prefixlen < 0 || prefixlen > maxbits) {
        fprintf(stderr, "warning: bad prefix length: %s\n",
                str);
        return 0;
    }

    if (inet_pton(af, buf, addr) != 1) {
        fprintf(stderr, "warning: bad address: %s\n", str);
        return 0;
    }

    mask_host_bits(addr, addr_len, prefixlen);

    return trie_insert(t, addr, prefixlen, maxbits);
}

/**
 * trie_walk callback — print a single aggregated prefix.
 */
static void print_prefix(const uint8_t *addr, int prefixlen,
                         int maxbits, void *ctx)
{
    char buf[INET6_ADDRSTRLEN];
    int af;

    (void)ctx;

    af = (maxbits == MAX_PFXLEN_V4) ? AF_INET : AF_INET6;
    inet_ntop(af, addr, buf, sizeof(buf));
    printf("%s/%d\n", buf, prefixlen);
}

int aggregate_prefixes(const char **prefixes, size_t count,
                       int flags)
{
    struct trie *t4;
    struct trie *t6;
    size_t i;

    t4 = trie_create();
    t6 = trie_create();
    if (!t4 || !t6) {
        trie_destroy(t4);
        trie_destroy(t6);
        return -1;
    }

    for (i = 0; i < count; i++) {
        if (parse_and_insert(prefixes[i], t4, t6, flags) < 0) {
            trie_destroy(t4);
            trie_destroy(t6);
            return -1;
        }
    }

    trie_aggregate(t4);
    trie_aggregate(t6);

    trie_walk(t4, print_prefix, MAX_PFXLEN_V4, NULL);
    trie_walk(t6, print_prefix, MAX_PFXLEN_V6, NULL);

    trie_destroy(t4);
    trie_destroy(t6);

    return 0;
}
