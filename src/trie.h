/*
 * aggregate6 — fast CIDR prefix aggregation tool
 * Copyright (c) 2025 0xkee
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TRIE_H
#define TRIE_H

#include <stdint.h>

struct trie_node {
    struct trie_node *children[2];
    int is_prefix;
};

struct trie {
    struct trie_node *root;
};

struct trie *trie_create(void);
void trie_destroy(struct trie *t);
int trie_insert(struct trie *t, const uint8_t *addr, int prefixlen, int maxbits);
void trie_aggregate(struct trie *t);

/* Callback: called for each aggregated prefix */
typedef void (*trie_walk_cb)(const uint8_t *addr, int prefixlen, int maxbits,
                             void *ctx);
void trie_walk(const struct trie *t, trie_walk_cb cb, int maxbits, void *ctx);

#endif /* TRIE_H */
