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

/**
 * Insert a prefix into the trie.
 *
 * Walks addr bit-by-bit from MSB. If an existing prefix covers
 * this one, insertion is skipped. After marking the node, any
 * subtree below is pruned (covered prefixes).
 *
 * @return 0 on success, -1 on error.
 */
int trie_insert(struct trie *t, const uint8_t *addr,
                int prefixlen, int maxbits)
{
    struct trie_node *cur;
    int pos;
    int bit;

    if (!t || !t->root || !addr)
        return -1;
    if (prefixlen < 0 || prefixlen > maxbits)
        return -1;

    cur = t->root;
    for (pos = 0; pos < prefixlen; pos++) {
        bit = (addr[pos / 8] >> (7 - pos % 8)) & 1;

        /* already covered by a shorter prefix */
        if (cur->is_prefix)
            return 0;

        if (!cur->children[bit]) {
            cur->children[bit] = node_create();
            if (!cur->children[bit])
                return -1;
        }
        cur = cur->children[bit];
    }

    cur->is_prefix = 1;

    /* prune covered subtree */
    node_destroy(cur->children[0]);
    node_destroy(cur->children[1]);
    cur->children[0] = NULL;
    cur->children[1] = NULL;

    return 0;
}

/**
 * Check if a node is a leaf (no children).
 */
static int node_is_leaf(const struct trie_node *n)
{
    return !n->children[0] && !n->children[1];
}

/**
 * Recursively aggregate siblings bottom-up.
 *
 * If both children exist, are marked as prefixes, and are leaves,
 * they merge into the parent.
 */
static void node_aggregate(struct trie_node *n)
{
    if (!n)
        return;

    node_aggregate(n->children[0]);
    node_aggregate(n->children[1]);

    if (n->children[0] && n->children[1] &&
        n->children[0]->is_prefix && n->children[1]->is_prefix &&
        node_is_leaf(n->children[0]) && node_is_leaf(n->children[1])) {
        free(n->children[0]);
        free(n->children[1]);
        n->children[0] = NULL;
        n->children[1] = NULL;
        n->is_prefix = 1;
    }
}

void trie_aggregate(struct trie *t)
{
    if (!t)
        return;
    node_aggregate(t->root);
}

/**
 * Recursive pre-order walk emitting aggregated prefixes.
 *
 * @param n      Current node.
 * @param addr   Address buffer being built (modified in-place).
 * @param depth  Current bit depth in the trie.
 * @param cb     User callback.
 * @param maxbits Max prefix length (32 or 128).
 * @param ctx    User context pointer.
 */
static void node_walk(const struct trie_node *n, uint8_t *addr,
                      int depth, trie_walk_cb cb, int maxbits,
                      void *ctx)
{
    if (!n)
        return;

    if (n->is_prefix) {
        cb(addr, depth, maxbits, ctx);
        return; /* subtree is covered, don't descend */
    }

    /* descend left (bit 0) — bit already 0 in addr */
    node_walk(n->children[0], addr, depth + 1, cb, maxbits, ctx);

    /* descend right (bit 1) — set bit, walk, then clear */
    if (n->children[1]) {
        addr[depth / 8] |= (uint8_t)(1 << (7 - depth % 8));
        node_walk(n->children[1], addr, depth + 1,
                  cb, maxbits, ctx);
        addr[depth / 8] &= (uint8_t)~(1 << (7 - depth % 8));
    }
}

void trie_walk(const struct trie *t, trie_walk_cb cb,
               int maxbits, void *ctx)
{
    uint8_t addr[16]; /* enough for IPv6 */

    if (!t || !t->root || !cb)
        return;

    memset(addr, 0, sizeof(addr));
    node_walk(t->root, addr, 0, cb, maxbits, ctx);
}
