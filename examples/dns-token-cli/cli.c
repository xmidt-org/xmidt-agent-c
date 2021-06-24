/*
 * SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC
 * SPDX-License-Identifier: Apache-2.0
 */
#include <inttypes.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/dns_token.h"
#include "../../src/libc/xa_file.h"
#include "../../src/libc/xa_time.h"
#include "../../src/libc/xa_xxd.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/* none */


/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/

/* A simple options parser helper. */
static bool is_opt(const char *in, const char *s1, const char *s2)
{
    if (s1 && s2) {
        return ((0 == strcmp(in, s1)) || (0 == strcmp(in, s2))) ? true : false;
    } else if (s2) {
        return (0 == strcmp(in, s2)) ? true : false;
    } else if (s1) {
        return (0 == strcmp(in, s1)) ? true : false;
    }
    return false;
}

void print_usage(char *name)
{
    printf("Usage: %s [options...] <fqdn>\n"
           " -h, --help                       This help text.\n"
           "     --interface       <name>     Use network INTERFACE (or address).\n"
           " -v  --verbose                    Verbose debugging is enabled, repeat for more.\n"
           " -k  --key             <file>     The public key for validating the jwt.\n"
           " -n  --now             <seconds>  seconds since Jan 1, 1970 or 'now' if not specified.\n"
           "     --skew            <seconds>  Allowable skew in seconds or defaults to 0.\n",
           name);
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    struct dns_token_in in;
    struct dns_token_out out;
    XAcode rv;
    const char *key_file = NULL;

    memset(&in, 0, sizeof(struct dns_token_in));
    memset(&out, 0, sizeof(struct dns_token_out));

    in.now = time_now_s();

    /* Very simple args parser. */
    for (int i = 1; i < argc; i++) {
        if (is_opt(argv[i], "-h", "--help")) {
            print_usage(argv[0]);
            return 0;
        } else if (is_opt(argv[i], NULL, "--interface")) {
            i++;
        } else if (is_opt(argv[i], "-v", "--verbose")) {
            in.keep_debug_bufs = true;
        } else if (is_opt(argv[i], "-k", "--key")) {
            i++;
            key_file = argv[i];
        } else if (is_opt(argv[i], "-n", "--now")) {
            i++;
            in.now = atoll(argv[i]);
        } else if (is_opt(argv[i], NULL, "--skew")) {
            i++;
            in.skew = atoll(argv[i]);
        } else {
            in.fqdn = argv[i];
            break;
        }
    }

    if (!in.fqdn) {
        print_usage(argv[0]);
    }

    if (key_file) {
        if (0 != fslirp(key_file, 0, (void **)&in.key, &in.len)) {
            printf("Failed to open the file: %s\n", key_file);
            return -1;
        }
    }

    rv = dns_token_fetch(&in, &out);

    printf("in: fqdn: %s\n", in.fqdn);
    printf("in:  now: %" PRId64 "\n", in.now);
    printf("in: skew: %" PRId64 "\n", in.skew);
    printf("dns_token_fetch(): %d\n", rv);
    printf(" dns fetch time: %.6f s\n", out.fetch_time);
    printf("processing time: %.6f s\n", (out.total_time - out.fetch_time));
    printf("     total time: %.6f s\n", out.total_time);

    if (out.raw_dns) {
        printf("\nThe raw dns response:\n");
        xxd(out.raw_dns, out.raw_dns_len, stdout);
    } else {
        if (in.keep_debug_bufs) {
            printf("\nThere was an performing the dns request.\n");
        }
    }

    if (out.reassembled) {
        printf("\nThe reassembled buffer:\n%.*s\n", (int)out.reassembled_len, out.reassembled);

        free(out.reassembled);
        out.reassembled = NULL;
    } else {
        if (in.keep_debug_bufs) {
            printf("\nThere was an error prior to or during buffer reassembly.\n");
        }
    }

    if (out.jwt) {
        printf("\nThe complete jwt:\n");
        cjwt_print(stdout, out.jwt);
        cjwt_destroy(out.jwt);
    } else {
        if (CJWTE_OK == out.cjwt_rv) {
            printf("\nThere was an error prior to processing the jwt.\n");
        } else {
            printf("\nThe jwt processing had an error: %d\n", out.cjwt_rv);
        }
    }

    if (out.raw_dns) {
        free(out.raw_dns);
        out.raw_dns = NULL;
    }

    return 0;
}

