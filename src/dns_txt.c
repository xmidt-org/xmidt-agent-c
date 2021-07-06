/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <resolv.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "codes.h"
#include "dns_txt.h"
#include "libc/xa_utils.h"

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

static uint16_t get_u16(const uint8_t *buf, int i)
{
    return (uint16_t)((buf[i] << 8) | buf[i + 1]);
}


static uint32_t get_u32(const uint8_t *buf, int i)
{
    return (uint32_t)((buf[i] << 24) | (buf[i + 1] << 16) | (buf[i + 2] << 8) | buf[i + 3]);
}


static XAcode skip_name(const struct dns_response *resp, int *i, XAcode *err)
{
    uint8_t len;

    do {
        if (resp->len < (*i + 1)) {
            return xa_set_error(err, XA_DNS_RECORD_TOO_SHORT);
        }

        len = resp->full[*i];
        if (len < 0x40) {
            /* Label */
            if (resp->len < (*i + 1 + len)) {
                return xa_set_error(err, XA_DNS_RECORD_TOO_SHORT);
            }
            *i += 1 + len;
        } else if (0xc0 == (0xc0 & len)) {
            /* DNS Pointer */
            if (resp->len < (*i + 2)) {
                return xa_set_error(err, XA_DNS_RECORD_TOO_SHORT);
            }
            *i += 2;
            break;
        } else {
            return xa_set_error(err, XA_DNS_RECORD_INVALID);
        }
    } while (len);

    return XA_OK;
}


static XAcode skip_question(const struct dns_response *resp, int *i, XAcode *err)
{
    XAcode e = XA_OK;

    if (XA_OK == skip_name(resp, i, &e)) {
        if ((*i + 4) <= resp->len) {
            *i += 4;
            return XA_OK;
        }
        e = XA_DNS_RECORD_TOO_SHORT;
    }

    return xa_set_error(err, e);
}


static XAcode skip_questions(struct dns_response *resp, int *i,
                             uint16_t count, XAcode *err)
{
    for (uint16_t c = 0; c < count; c++) {
        if (XA_OK != skip_question(resp, i, err)) {
            return *err;
        }
    }

    return XA_OK;
}


static void dns_rr_append(struct dns_rr **list, struct dns_rr *item)
{
    if (!*list) {
        *list = item;
    } else {
        struct dns_rr *p = *list;

        while (p->next) {
            p = p->next;
        }
        p->next = item;
    }
}


static XAcode process_rr(struct dns_response *resp, int *i, XAcode *err)
{
    struct dns_rr r;

    if (XA_OK != skip_name(resp, i, err)) {
        return *err;
    }

    if (resp->len < (*i + 10)) {
        return xa_set_error(err, XA_DNS_RECORD_TOO_SHORT);
    }

    memset(&r, 0, sizeof(struct dns_rr));

    r.type = get_u16(resp->full, *i);
    *i += 2;
    r.class = get_u16(resp->full, *i);
    *i += 2;
    r.ttl = get_u32(resp->full, *i);
    *i += 4;
    r.rdlength = get_u16(resp->full, *i);
    *i += 2;
    r.rdata = &resp->full[*i];

    if (resp->len < (*i + r.rdlength)) {
        return xa_set_error(err, XA_DNS_RECORD_TOO_SHORT);
    } else {
        struct dns_rr *p = NULL;

        /* Only copy the complete structure.  This eliminates partially complete
         * results. */
        p = xa_memdup(&r, sizeof(struct dns_rr));
        if (!p) {
            return xa_set_error(err, XA_OUT_OF_MEMORY);
        }
        dns_rr_append(&resp->answers, p);
    }

    *i += r.rdlength;
    return XA_OK;
}


static XAcode process_answers(struct dns_response *resp, int *i, XAcode *err)
{
    for (uint16_t c = 0; c < resp->answer_count; c++) {
        if (XA_OK != process_rr(resp, i, err)) {
            return *err;
        }
    }

    return XA_OK;
}


static XAcode process_dns_response(struct dns_response *resp, XAcode *err)
{
    int rcode = 0;
    uint16_t qdcount = 0;
    uint16_t ancount = 0;
    int i = 0;

    if (resp->len < 12) {
        return xa_set_error(err, XA_DNS_RECORD_TOO_SHORT);
    }

    /* qr = response, opcode = 0, tc = 0 */
    if (0x80 != (0xfa & resp->full[2])) {
        return xa_set_error(err, XA_DNS_FORMAT_ERROR);
    }

    qdcount = get_u16(resp->full, 4);
    ancount = get_u16(resp->full, 6);

    if (1 != qdcount) {
        return xa_set_error(err, XA_DNS_FORMAT_ERROR);
    }
    if (ancount < 1) {
        return xa_set_error(err, XA_DNS_TOO_FEW_ANSWERS);
    }

    rcode = 0x0f & resp->full[3];
    if (0 != rcode) {
        switch (rcode) {
        case 1: *err = XA_DNS_FORMAT_ERROR; break;    /* DNS Format error - fatal */
        case 2: *err = XA_DNS_SERVER_ERROR; break;    /* DNS Server failure - retry */
        case 3: *err = XA_DNS_NAME_ERROR; break;      /* DNS Name error - fatal */
        case 4: *err = XA_DNS_NOT_IMPLEMENTED; break; /* DNS Not implemented - retry */
        case 5: *err = XA_DNS_REFUSED; break;         /* DNS Refused - fatal */
        default: *err = XA_DNS_UNKNOWN_VALUE; break;  /* DNS Unknown - retry */
        }
        return *err;
    }

    resp->answer_count = ancount;

    i = 12; /* consume the header */

    /* Skip the questions */
    if (XA_OK != skip_questions(resp, &i, qdcount, err)) {
        return *err;
    }

    /* break out the answers */
    if (XA_OK != process_answers(resp, &i, err)) {
        return *err;
    }

    return XA_OK;
}


/**
 * If this is a TXT record that matches '[0-9][0-9]:{data}' and this is the
 * next sequence needed, then append it to the existing buffer.
 *
 * @param rr        the dns response record to process
 * @param last_frag the previous fragment number
 * @param token     the token to update
 * @param err the error code field to set
 *
 * @return XA_OK on sucess or the following on failure:
 *         XA_OUT_OF_MEMORY
 */
static XAcode append_frag(const struct dns_rr *rr, int *last_frag,
                          struct dns_xmidt_token *token, XAcode *err)
{
    const uint8_t *data = NULL;
    uint16_t rdlen = 0;
    int fragment = 0;

    /* Skip if not a TXT record. */
    if (ns_t_txt != rr->type) {
        return XA_OK;
    }

    /* We know that rdata can't be NULL. */
    data = rr->rdata;
    rdlen = rr->rdlength;

    /* Skip if it isn't what we're expecting. */
    if ((rdlen < 4) || (':' != data[3])) {
        return XA_OK;
    }

    /* Ignore the first byte which is the length */
    data++;
    rdlen--;

    /* safe because we know that data[3] is ':' */
    fragment = atoi((const char *)data);

    /* If this is the not next fragment, exit. */
    if (fragment != (*last_frag + 1)) {
        return XA_OK;
    }

    if (0 != xa_memappend((void **)&token->buf, &token->len, &data[3], rdlen - 3)) {
        return xa_set_error(err, XA_OUT_OF_MEMORY);
    }

    if (rr->ttl < token->ttl) {
        token->ttl = rr->ttl;
    }

    *last_frag = fragment;

    return XA_OK;
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
void dns_destroy_response(struct dns_response *r)
{
    if (r) {
        while (r->answers) {
            struct dns_rr *p = r->answers;
            r->answers = r->answers->next;
            free(p);
        }

        if (r->full) {
            free(r->full);
        }
        free(r);
    }
}


void dns_destroy_token(struct dns_xmidt_token *t)
{
    if (t) {
        if (t->buf) {
            free(t->buf);
        }
        free(t);
    }
}


XAcode dns_txt_fetch(const char *fqdn, struct dns_response **resp, XAcode *err)
{
    struct dns_response *p = NULL;
    struct __res_state state;
    uint8_t buf[NS_MAXMSG];
    int len = -1;
    XAcode e = XA_OK;

    if (!fqdn || !resp) {
        return xa_set_error(err, XA_INVALID_INPUT);
    }

    if (0 != res_ninit(&state)) {
        return xa_set_error(err, XA_DNS_RESOLVER_ERROR);
    }

    /* Fetch the DNS record. */
    len = res_nquery(&state,
                     fqdn,
                     ns_c_in,  /* Class: Internet */
                     ns_t_txt, /* Type: TXT */
                     buf, NS_MAXMSG);

    res_nclose(&state);

    if (len < 1) {
        return xa_set_error(err, XA_DNS_RESOLVER_ERROR);
    }

    p = calloc(1, sizeof(struct dns_response));
    if (!p) {
        return xa_set_error(err, XA_OUT_OF_MEMORY);
    }

    p->len = len;
    p->full = xa_memdup(buf, p->len);
    if (!p->full) {
        free(p);
        return xa_set_error(err, XA_OUT_OF_MEMORY);
    }

    if (XA_OK != process_dns_response(p, &e)) {
        dns_destroy_response(p);
        return xa_set_error(err, e);
    }

    *resp = p;

    return XA_OK;
}


XAcode dns_token_assemble(const struct dns_response *resp,
                          struct dns_xmidt_token **token,
                          XAcode *err)
{
    int fragment = 0;
    int last_frag = -1;
    struct dns_xmidt_token t;
    XAcode e = XA_OK;

    if (!resp || !token) {
        return xa_set_error(err, XA_INVALID_INPUT);
    }

    memset(&t, 0, sizeof(struct dns_xmidt_token));
    t.ttl = UINT32_MAX;

    /* Loop until we found no more continuous fragments */
    while (last_frag < fragment) {
        const struct dns_rr *rr;
        last_frag = fragment;

        rr = resp->answers;
        while (rr) {
            if (XA_OK != append_frag(rr, &fragment, &t, &e)) {
                goto ERROR;
            }
            rr = rr->next;
        }
    }

    if (0 == t.len) {
        return xa_set_error(err, XA_DNS_TOKEN_NOT_PRESENT);
    }

    *token = xa_memdup(&t, sizeof(struct dns_xmidt_token));
    if (NULL != *token) {
        return XA_OK;
    }

    e = XA_OUT_OF_MEMORY;

ERROR:
    if (t.buf) {
        free(t.buf);
    }
    return xa_set_error(err, e);
}
