/*
 * aggregate6 — fast CIDR prefix aggregation tool
 * Copyright (c) 2025 0xkee
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef AGGREGATE_H
#define AGGREGATE_H

#include <stddef.h>

/* Aggregate a list of CIDR prefixes, writing result to stdout.
 * Returns 0 on success, -1 on error. */
int aggregate_prefixes(const char **prefixes, size_t count, int flags);

/* Flags */
#define AGG_IPV4_ONLY  0x01
#define AGG_IPV6_ONLY  0x02

#endif /* AGGREGATE_H */
