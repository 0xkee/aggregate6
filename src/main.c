/*
 * aggregate6 — fast CIDR prefix aggregation tool
 * Copyright (c) 2025 0xkee
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define _POSIX_C_SOURCE 200809L

#include "aggregate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "0.2.0"
#define MAX_LINE 256
#define INITIAL_CAP 1024

static void usage(const char *prog)
{
    fprintf(stderr,
        "Usage: %s [OPTIONS]\n"
        "\n"
        "Aggregate CIDR prefixes from stdin.\n"
        "\n"
        "Options:\n"
        "  -4          IPv4 prefixes only\n"
        "  -6          IPv6 prefixes only\n"
        "  -h, --help  Show this help\n"
        "  -v, --version  Show version\n",
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
    char line[MAX_LINE];
    const char **prefixes = NULL;
    size_t count = 0;
    size_t cap = 0;
    int ret;

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
        fprintf(stderr, "Unknown option: %s\n", argv[i]);
        usage(argv[0]);
        return 1;
    }

    /* Read prefixes from stdin */
    cap = INITIAL_CAP;
    prefixes = malloc(cap * sizeof(*prefixes));
    if (!prefixes) {
        perror("malloc");
        return 1;
    }

    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        char *copy;

        /* Strip trailing newline */
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';

        /* Skip empty lines and comments */
        if (len == 0 || line[0] == '#')
            continue;

        if (count >= cap) {
            cap *= 2;
            prefixes = realloc(prefixes, cap * sizeof(*prefixes));
            if (!prefixes) {
                perror("realloc");
                return 1;
            }
        }

        copy = strdup(line);
        if (!copy) {
            perror("strdup");
            return 1;
        }
        prefixes[count++] = copy;
    }

    ret = aggregate_prefixes(prefixes, count, flags);

    /* Cleanup */
    for (i = 0; (size_t)i < count; i++)
        free((void *)prefixes[i]);
    free(prefixes);

    return ret < 0 ? 1 : 0;
}
