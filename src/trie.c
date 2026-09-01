/*
 * aggregate6 — fast CIDR prefix aggregation tool
 * Copyright (c) 2025 0xkee
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "trie.h"

#include <stdlib.h>
#include <string.h>

static struct trie_node *node_create(void)
{
    struct trie_node *n = calloc(1, sizeof(*n));
    return n;
}

static void node_destroy(struct trie_node *n)
{
    if (!n)
        return;
    node_destroy(n->children[0]);
    node_destroy(n->children[1]);
    free(n);
}

struct trie *trie_create(void)
{
    struct trie *t = calloc(1, sizeof(*t));
    if (t)
        t->root = node_create();
    return t;
}

void trie_destroy(struct trie *t)
{
    if (!t)
        return;
    node_destroy(t->root);
    free(t);
}

/* TODO: implement prefix insertion into trie */
int trie_insert(struct trie *t, const uint8_t *addr, int prefixlen, int maxbits)
{
    (void)t;
    (void)addr;
    (void)prefixlen;
    (void)maxbits;
    return -1;
}

/* TODO: implement trie aggregation (merge adjacent, remove redundant) */
void trie_aggregate(struct trie *t)
{
    (void)t;
}

/* TODO: implement trie walk (in-order traversal emitting aggregated prefixes) */
void trie_walk(const struct trie *t, trie_walk_cb cb, int maxbits, void *ctx)
{
    (void)t;
    (void)cb;
    (void)maxbits;
    (void)ctx;
}
