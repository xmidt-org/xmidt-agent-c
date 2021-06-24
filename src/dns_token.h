/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __DNS_TOKEN_H__
#define __DNS_TOKEN_H__

#include <stdint.h>

#include <cjwt/cjwt.h>

#include "codes.h"

struct dns_token_in {
    /* The FQDN query to resolve.  It is generally going to be
     * something like: {mac address}.fabric.xmidt.example.com
     *                 112233445566.fabric.xmidt.example.com */
    const char *fqdn;

    /* The public PEM signing key string and length */
    const uint8_t *key;
    size_t len;

    /* The current time to use to validate the jwt against.
     *
     * now is seconds since Jan 1, 1970 */
    int64_t now;

    /* The allowable skew time to use while validating the jwt.
     * Generally this will be a value that is like 5min (300).
     *
     * skew is in seconds */
    int64_t skew;

    /* If set, instructs the function to keep the two intermediate buffers
     * and pass them back in the dns_token_out struct.  Otherwise, they are
     * not kept. */
    bool keep_debug_bufs;
};


struct dns_token_out {
    /* The dns response packet & length.  This is useful for debugging.  The dns
     * records should be TXT records containing a string that follows this
     * format:
     *
     * '[0-9][0-9]:{base64 encoded string}'
     *
     * max 255 characters, not nil terminated
     *
     * The buffer must be freed to avoid a memory leak. */
    uint8_t *raw_dns;
    size_t raw_dns_len;

    /* The reassembled packet & length based on assembly rules.  This should
     * now be a raw JWT.
     *
     * This is useful for debugging.  The buffer must be freed. */
    char *reassembled;
    size_t reassembled_len;


    /* The time taken to fetch the dns record in seconds. */
    double fetch_time;

    /* The time taken to fetch and process the dns record in seconds. */
    double total_time;


    /* The response code from cjwt for use during debugging. */
    cjwt_code_t cjwt_rv;

    /* The valid jwt or NULL. */
    cjwt_t *jwt;
};


/**
 * dns_token_fetch() takes the DNS query name to make to look for a JWT token
 * encoded as TXT record(s) and returns the resulting token if one is present.
 *
 * @param query_name the input query name to examine
 * @param token      the resulting token if one is present
 *
 * @return 0 if successful, error otherwise
 */
XAcode dns_token_fetch(struct dns_token_in *in, struct dns_token_out *out);

#endif
