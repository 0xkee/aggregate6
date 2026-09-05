/*
 * aggregate6 — fast multi-threaded IPv4/IPv6 CIDR prefix aggregation tool
 * Copyright (c) 2026 0xkee
 * SPDX-License-Identifier: GPL-3.0-only
 */

#define _POSIX_C_SOURCE 200809L

#include "aggregate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "0.3.0"
#define IO_BUF_SIZE 65536

static void usage(const char *prog)
{
    fprintf(stderr,
        "Usage: %s [OPTIONS] [FILE...]\n"
        "\n"
        "Aggregate CIDR prefixes from stdin or files.\n"
        "With no FILE, or when FILE is -, read stdin.\n"
        "\n"
        "Options:\n"
        "  -4               IPv4 prefixes only\n"
        "  -6               IPv6 prefixes only\n"
        "  -h, --help       Show this help\n"
        "  -v, --version    Show version\n",
        prog);
}

static void version(void)
{
    printf("aggregate6 %s\n", VERSION);
}

int main(int argc, char **argv)
{
    int flags = 0;
    int i;
    int first_file = 0;
    struct agg_ctx *ctx;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 ||
            strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        }
        if (strcmp(argv[i], "-v") == 0 ||
            strcmp(argv[i], "--version") == 0) {
            version();
            return 0;
        }
        if (strcmp(argv[i], "-4") == 0) {
            flags |= AGG_IPV4_ONLY;
            continue;
        }
        if (strcmp(argv[i], "-6") == 0) {
            flags |= AGG_IPV6_ONLY;
            continue;
        }
        if (argv[i][0] == '-' && argv[i][1] != '\0') {
            fprintf(stderr, "Unknown option: %s\n",
                    argv[i]);
            usage(argv[0]);
            return 1;
        }
        if (!first_file)
            first_file = i;
    }

    /* large I/O buffers for stdin and stdout */
    setvbuf(stdin, NULL, _IOFBF, IO_BUF_SIZE);
    setvbuf(stdout, NULL, _IOFBF, IO_BUF_SIZE);

    ctx = agg_create(flags);
    if (!ctx) {
        perror("agg_create");
        return 1;
    }

    if (first_file) {
        for (i = first_file; i < argc; i++) {
            if (argv[i][0] == '-' &&
                argv[i][1] != '\0')
                continue;

            if (strcmp(argv[i], "-") == 0) {
                if (agg_add_stream(ctx, stdin) < 0) {
                    agg_finish(ctx);
                    return 1;
                }
            } else {
                if (agg_add_file(ctx, argv[i]) < 0) {
                    agg_finish(ctx);
                    return 1;
                }
            }
        }
    } else {
        if (agg_add_stream(ctx, stdin) < 0) {
            agg_finish(ctx);
            return 1;
        }
    }

    return agg_finish(ctx) < 0 ? 1 : 0;
}
