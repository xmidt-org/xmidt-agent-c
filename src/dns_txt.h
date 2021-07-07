/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __DNS_TXT_H__
#define __DNS_TXT_H__

#include <stdint.h>

#include "codes.h"

/* The Xmidt agent has a feature that allows it to look up a DNS TXT record that
 * is expected (and validated) to be JWT signed by a known & trusted entity.
 *
 * This JWT allows for temporary offboarding for when a device should go to a
 * different trusted network (like if the hardware is being refurbished).
 *
 * Unfortunately uclibc and other libc implementations don't always support
 * TXT records very well.  This code works around that problem and only depends
 * on a functioning DNS resolver.
 *
 * This does not support DOH or other extensions unless the resolver does &
 * abstracts from this code.
 */

/* See https://datatracker.ietf.org/doc/html/rfc1035#section-3.2.1 */
struct dns_rr {
    /* name isn't needed for our TXT record use case. */
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
    uint8_t *rdata;

    struct dns_rr *next;
};


struct dns_response {
    uint8_t *full; /* The full DNS response bytes. */
    int len;       /* The length of the full DNS response bytes. */

    uint16_t answer_count;
    struct dns_rr *answers;
};


struct dns_xmidt_token {
    uint32_t ttl; /* The time to live for the token based on the closest
                   * expiration time of all the records. */
    size_t len;   /* Length of the buffer. */
    char *buf;    /* free() separately, no trailing '\0' */
};


/**
 * Called to free the dns_response struct provided by dns_txt_fetch().
 *
 * @param r the struct to release resources from
 */
void dns_destroy_response(struct dns_response *r);


/**
 * Called to free the dns_xmidt_token struct provided by dns_token_assemble()
 *
 * @param t the struct to release resources from
 */
void dns_destroy_token(struct dns_xmidt_token *t);


/**
 * Fetches a DNS TXT record from the locally specified resolver for the fqdn
 * provided.
 *
 * @param fqdn the fully qualified domain name to resolve
 * @param resp the resulting dns response struct (must be freed)
 * @param err  the error response code
 *
 * @return XA_OK on success, error otherwise
 */
XAcode dns_txt_fetch(const char *fqdn, struct dns_response **resp, XAcode *err);


/**
 * Re-assembles the DNS TXT records based on the indexing scheme implemented.
 *
 * @note: Until the JWT is decide and the signature is validated, it's not
 *        possible to tell if it is valid.  Don't assume a success here means
 *        that the JWT is valid!
 *
 * @param resp  the full DNS response structure to process
 * @param token the resulting token string and data if successful
 * @param err   the error response code
 *
 * @return XA_OK on success, error otherwise
 */
XAcode dns_token_assemble(const struct dns_response *resp,
                          struct dns_xmidt_token **token,
                          XAcode *err);
#endif
