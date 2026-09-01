/*
 * aggregate6 — fast CIDR prefix aggregation tool
 * Copyright (c) 2025 0xkee
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "aggregate.h"

#include <stdio.h>

/* TODO: implement actual aggregation using trie */
int aggregate_prefixes(const char **prefixes, size_t count, int flags)
{
    size_t i;

    (void)flags;

    for (i = 0; i < count; i++)
        puts(prefixes[i]);

    return 0;
}
