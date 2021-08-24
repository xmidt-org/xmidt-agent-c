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

#include <cjwt/cjwt.h>

#include <cutils/file.h>
#include <cutils/xxd.h>
#include <otelc/time.h>

#include "../../src/dns_txt.h"

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
    XAcode rv = XA_OK;
    struct dns_response *resp = NULL;
    struct dns_xmidt_token *token = NULL;
    cjwt_t *jwt = NULL;
    const char *key_file = NULL;
    const char *fqdn = NULL;
    int64_t skew = 0;
    int64_t now = 0;
    uint8_t *pub_key = NULL;
    size_t pub_key_len = 0;
    bool verbose = false;

    int64_t start_time = 0;
    int64_t fetch_time = 0;
    int64_t assembled_time = 0;
    int64_t jwt_time = 0;

    now = time_now_s();

    /* Very simple args parser. */
    for (int i = 1; i < argc; i++) {
        if (is_opt(argv[i], "-h", "--help")) {
            print_usage(argv[0]);
            return 0;
        } else if (is_opt(argv[i], NULL, "--interface")) {
            i++;
        } else if (is_opt(argv[i], "-v", "--verbose")) {
            verbose = true;
        } else if (is_opt(argv[i], "-k", "--key")) {
            i++;
            key_file = argv[i];
        } else if (is_opt(argv[i], "-n", "--now")) {
            i++;
            now = atoll(argv[i]);
        } else if (is_opt(argv[i], NULL, "--skew")) {
            i++;
            skew = atoll(argv[i]);
        } else {
            fqdn = argv[i];
            break;
        }
    }

    if (!fqdn) {
        print_usage(argv[0]);
    }

    if (key_file) {
        if (0 != freadall(key_file, 0, (void **)&pub_key, &pub_key_len)) {
            printf("Failed to open the file: %s\n", key_file);
            return -1;
        }
    }

    start_time = time_boot_now_ns();

    rv = dns_txt_fetch(fqdn, &resp, NULL);
    if (XA_OK != rv) {
        printf("Unable to fetch a TXT record for the fqdn: '%s'\n", fqdn);
        return -1;
    }

    fetch_time = time_boot_now_ns();

    rv = dns_token_assemble(resp, &token, NULL);
    if (XA_OK != rv) {
        printf("Unable to reassemble the text record.\n\n");
        xxd(resp->full, resp->len, stdout);
        dns_destroy_response(resp);
        return -1;
    }

    assembled_time = time_boot_now_ns();

    if (CJWTE_OK != cjwt_decode(token->buf, token->len, 0, pub_key, pub_key_len, now, skew, &jwt)) {
        printf("Unable to decode the jwt from the text record.\n");
        printf("jwt:\n'%.*s'\nttl: %ud\n\n", (int)token->len, token->buf, token->ttl);
        printf("Original DNS record:\n");
        xxd(resp->full, resp->len, stdout);
        dns_destroy_response(resp);
        return -1;
    }
    jwt_time = time_boot_now_ns();

    printf("          fqdn: %s\n", fqdn);
    printf("          time: %" PRId64 "\n", now);
    printf("          skew: %" PRId64 "\n", skew);
    printf("dns fetch time: %.6f s\n", time_diff(start_time, fetch_time));
    printf(" assembly time: %.6f s\n", time_diff(fetch_time, assembled_time));
    printf("      jwt time: %.6f s\n", time_diff(assembled_time, jwt_time));
    printf("    total time: %.6f s\n", time_diff(start_time, jwt_time));

    if (verbose) {
        printf("\nThe raw dns response:\n");
        xxd(resp->full, resp->len, stdout);
    }

    if (verbose) {
        printf("\nThe reassembed buffer:\n'%.*s'\nttl: %ud\n\n", (int)token->len, token->buf, token->ttl);
    }

    if (verbose) {
        printf("\nThe complete jwt:\n");
        cjwt_print(stdout, jwt);
    }

    dns_destroy_response(resp);
    dns_destroy_token(token);
    cjwt_destroy(jwt);

    return 0;
}

