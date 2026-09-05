/*
 * aggregate6 — fast multi-threaded IPv4/IPv6 CIDR prefix aggregation tool
 * Copyright (c) 2026 0xkee
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef AGGREGATE_H
#define AGGREGATE_H

#include <stdio.h>

/* Opaque aggregation context */
struct agg_ctx;

/**
 * Create a new aggregation context.
 *
 * @param flags  Combination of AGG_IPV4_ONLY / AGG_IPV6_ONLY.
 * @return Context pointer, or NULL on error.
 */
struct agg_ctx *agg_create(int flags);

/**
 * Read CIDR prefixes from a stream (stdin or pipe).
 * Can be called multiple times for different sources.
 *
 * @return 0 on success, -1 on error.
 */
int agg_add_stream(struct agg_ctx *ctx, FILE *input);

/**
 * Read CIDR prefixes from a file via mmap (fast path).
 * Falls back to fopen+fgets if mmap is unavailable.
 *
 * @return 0 on success, -1 on error.
 */
int agg_add_file(struct agg_ctx *ctx, const char *path);

/**
 * Aggregate all collected prefixes, write result to stdout,
 * and destroy the context. ctx is invalid after this call.
 *
 * @return 0 on success, -1 on error.
 */
int agg_finish(struct agg_ctx *ctx);

/* Flags */
#define AGG_IPV4_ONLY  0x01
#define AGG_IPV6_ONLY  0x02

#endif /* AGGREGATE_H */
