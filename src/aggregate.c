/*
 * aggregate6 — fast multi-threaded IPv4/IPv6 CIDR prefix aggregation tool
 * Copyright (c) 2026 0xkee
 * SPDX-License-Identifier: GPL-3.0-only
 */

#define _POSIX_C_SOURCE 200809L

#include "aggregate.h"
#include "prefix.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_LINE   256
#define OUT_BUF_SZ 64
#define AVG_LINE   18  /* avg bytes per CIDR line */

struct agg_ctx {
    struct prefix_arrays pa;
    int flags;
};

/** Thread argument for parallel aggregation. */
struct agg_thread_arg {
    struct prefix4 *arr;
    size_t count;
};

static void *agg_v4_thread(void *arg)
{
    struct agg_thread_arg *ta = arg;

    prefix4_aggregate(ta->arr, &ta->count);
    return NULL;
}

struct agg_ctx *agg_create(int flags)
{
    struct agg_ctx *ctx;

    ctx = calloc(1, sizeof(*ctx));
    if (!ctx)
        return NULL;
    ctx->flags = flags;
    if (pa_init(&ctx->pa) < 0) {
        free(ctx);
        return NULL;
    }
    return ctx;
}

int agg_add_stream(struct agg_ctx *ctx, FILE *input)
{
    char line[MAX_LINE];
    int skip_v4, skip_v6;

    skip_v4 = (ctx->flags & AGG_IPV6_ONLY) != 0;
    skip_v6 = (ctx->flags & AGG_IPV4_ONLY) != 0;

    while (fgets(line, sizeof(line), input)) {
        size_t len = strlen(line);

        while (len > 0 &&
               (line[len - 1] == '\n' ||
                line[len - 1] == '\r'))
            line[--len] = '\0';

        if (len == 0 || line[0] == '#')
            continue;

        if (pa_add_line(&ctx->pa, line,
                        skip_v4, skip_v6) < 0)
            return -1;
    }
    return 0;
}

/**
 * Read file via mmap for zero-copy parsing.
 * Falls back to fopen+fgets if mmap fails.
 */
int agg_add_file(struct agg_ctx *ctx, const char *path)
{
    int fd;
    struct stat st;
    char *data;
    size_t est;
    int skip_v4, skip_v6;
    int ret;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "%s: ", path);
        perror(NULL);
        return -1;
    }

    if (fstat(fd, &st) < 0 || st.st_size == 0) {
        close(fd);
        if (st.st_size == 0)
            return 0;
        /* fallback for special files */
        goto fallback;
    }

    data = mmap(NULL, (size_t)st.st_size,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE, fd, 0);
    close(fd);

    if (data == MAP_FAILED)
        goto fallback;

    /* pre-allocate arrays based on file size */
    est = (size_t)st.st_size / AVG_LINE;
    pa_reserve(&ctx->pa, est, est / 4);

    skip_v4 = (ctx->flags & AGG_IPV6_ONLY) != 0;
    skip_v6 = (ctx->flags & AGG_IPV4_ONLY) != 0;

    ret = pa_add_buf(&ctx->pa, data, (size_t)st.st_size,
                     skip_v4, skip_v6);

    munmap(data, (size_t)st.st_size);
    return ret;

fallback:;
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "%s: ", path);
        perror(NULL);
        return -1;
    }
    ret = agg_add_stream(ctx, fp);
    fclose(fp);
    return ret;
}

int agg_finish(struct agg_ctx *ctx)
{
    struct agg_thread_arg ta;
    pthread_t tid;
    int threaded = 0;
    char buf[OUT_BUF_SZ];
    size_t i;

    if (!ctx)
        return -1;

    /* parallel: v4 in thread, v6 in main */
    if (ctx->pa.v4_count > 0 && ctx->pa.v6_count > 0) {
        ta.arr = ctx->pa.v4;
        ta.count = ctx->pa.v4_count;
        if (pthread_create(&tid, NULL,
                           agg_v4_thread, &ta) == 0)
            threaded = 1;
    }

    if (!threaded && ctx->pa.v4_count > 0)
        prefix4_aggregate(ctx->pa.v4, &ctx->pa.v4_count);

    if (ctx->pa.v6_count > 0)
        prefix6_aggregate(ctx->pa.v6, &ctx->pa.v6_count);

    if (threaded) {
        pthread_join(tid, NULL);
        ctx->pa.v4_count = ta.count;
    }

    for (i = 0; i < ctx->pa.v4_count; i++) {
        prefix4_format(&ctx->pa.v4[i], buf, sizeof(buf));
        fputs(buf, stdout);
    }
    for (i = 0; i < ctx->pa.v6_count; i++) {
        prefix6_format(&ctx->pa.v6[i], buf, sizeof(buf));
        fputs(buf, stdout);
    }

    pa_free(&ctx->pa);
    free(ctx);
    return 0;
}
