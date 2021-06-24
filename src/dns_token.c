/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <arpa/nameser.h>
#include <netinet/in.h>
#include <resolv.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "codes.h"
#include "dns_token.h"
#include "libc/xa_time.h"
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


/**
 * Checks the dns response to make sure it is an expected response to our
 * request for TXT record(s) for the name.  If there is an error, assign it
 * to *err and return the value.
 *
 * @param buf the buffer to examine
 * @param len the length of the buffer
 * @param msg the ns_msg to fill in and use for processing
 * @param err the error code field to set
 *
 * @return XA_OK on sucess or the following on failure:
 *         XA_FORMAT_ERROR    - fatal
 *         XA_SERVER_ERROR    - retry
 *         XA_NAME_ERROR      - fatal
 *         XA_NOT_IMPLEMENTED - retry
 *         XA_REFUSED         - fatal
 *         XA_UNKNOWN_VALUE   - retry
 */
static XAcode check_response(const unsigned char *buf, int len, ns_msg *msg,
                             XAcode *err)
{
    int rcode = 0;

    /* Parse response */
    if ((0 != ns_initparse(buf, len, msg))
        || (1 != ns_msg_getflag(*msg, ns_f_qr))     /* Response */
        || (0 != ns_msg_getflag(*msg, ns_f_opcode)) /* Standard query */
        || (1 != ns_msg_count(*msg, ns_s_qd)))      /* 1 question */
    {
        *err = XA_FORMAT_ERROR;
        return *err;
    }

    rcode = ns_msg_getflag(*msg, ns_f_rcode);

    switch (rcode) {
    case 0: return XA_OK;                     /* No error condition */
    case 1: *err = XA_FORMAT_ERROR; break;    /* DNS Format error - fatal */
    case 2: *err = XA_SERVER_ERROR; break;    /* DNS Server failure - retry */
    case 3: *err = XA_NAME_ERROR; break;      /* DNS Name error - fatal */
    case 4: *err = XA_NOT_IMPLEMENTED; break; /* DNS Not implemented - retry */
    case 5: *err = XA_REFUSED; break;         /* DNS Refused - fatal */
    default: *err = XA_UNKNOWN_VALUE; break;  /* DNS Unknown - retry */
    }

    return *err;
}


/**
 * If this is a TXT record that matches '[0-9][0-9]:{data}' and this is the
 * next sequence needed, then append it to the existing buffer.
 *
 * @param rr        the dns response record to process
 * @param last_frag the previous fragment number
 * @param out       the output buffer to append to and return
 * @param out_len   the lenght of the output buffer
 * @param err the error code field to set
 *
 * @return XA_OK on sucess or the following on failure:
 *         XA_OUT_OF_MEMORY
 */
static XAcode append_frag(const ns_rr *rr, int *last_frag, char **out,
                          size_t *out_len, XAcode *err)
{
    const char *data = NULL;
    uint16_t rdlen = 0;
    int fragment = 0;

    /* Skip if not a TXT record. */
    if (ns_t_txt != ns_rr_type(*rr)) {
        return XA_OK;
    }

    data = (const char *)ns_rr_rdata(*rr);
    rdlen = ns_rr_rdlen(*rr);

    /* Skip if it isn't what we're expecting. */
    if (!data || (rdlen < 4) || (':' != data[3])) {
        return XA_OK;
    }

    /* Ignore the first byte which is the length */
    data++;
    rdlen--;

    /* safe because we know that data[3] is ':' */
    fragment = atoi(data);

    /* If this is the not next fragment, exit. */
    if (fragment != (*last_frag + 1)) {
        return XA_OK;
    }

    if (0 != xa_memappend((void **)out, out_len, &data[3], rdlen - 3)) {
        *err = XA_OUT_OF_MEMORY;
        return *err;
    }

    *last_frag = fragment;

    return XA_OK;
}


/**
 * Wrapps ns_parserr to make error handling easier.
 *
 * @return XA_OK on sucess or the following on failure:
 *         XA_FORMAT_ERROR
 */
static XAcode parserr(ns_msg *msg, uint16_t i, ns_rr *rr, XAcode *err)
{
    if (0 == ns_parserr(msg, ns_s_an, i, rr)) {
        return XA_OK;
    }

    *err = XA_FORMAT_ERROR;
    return *err;
}


/**
 * Reassemble the text records into a text blob that is a signed JWT.
 * Because the JWT is signed, validating the rest of the input is not
 * very useful, so we're not going to do it.
 *
 * @param msg     the ns_msg to fill in and use for processing
 * @param out     the buffer being assembled
 * @param out_len the length of out
 * @param err     the error code field to set
 *
 * @return XA_OK on sucess or the following on failure:
 *         XA_TOO_FEW_ANSWERS
 *         XA_FORMAT_ERROR
 *         XA_OUT_OF_MEMORY
 */
static XAcode reassemble(ns_msg *msg, char **out, size_t *out_len, XAcode *err)
{
    uint16_t answers = 0;
    int fragment = 0;
    int last_frag = -1;
    char *buf = NULL;
    size_t len = 0;

    answers = ns_msg_count(*msg, ns_s_an);

    if (0 == answers) {
        *err = XA_TOO_FEW_ANSWERS;
        return *err;
    }

    /* Loop until we found no more continuous fragments */
    while (last_frag < fragment) {
        last_frag = fragment;
        for (uint16_t i = 0; i < answers; i++) {
            ns_rr rr;

            if ((XA_OK != parserr(msg, i, &rr, err))
                || (XA_OK != append_frag(&rr, &fragment, &buf, &len, err)))
            {
                if (buf) {
                    free(buf);
                }
                return *err;
            }
        }
    }

    *out = buf;
    *out_len = len;
    return XA_OK;
}


static XAcode query_dns(const struct dns_token_in *in, struct dns_token_out *out,
                        int64_t *time)
{
    ns_msg msg;
    struct __res_state state;
    XAcode err = XA_OK;
    uint8_t dns_buf[NS_MAXMSG];
    int len = -1;

    memset(&state, 0, sizeof(struct __res_state));

    if (0 != res_ninit(&state)) {
        *time = time_boot_now_ns();

        return XA_DNS_RESOLVER_ERROR;
    }


    len = res_nquery(&state,
                     in->fqdn,
                     ns_c_in,  /* Class: Internet */
                     ns_t_txt, /* Type: TXT */
                     dns_buf, NS_MAXMSG);

    *time = time_boot_now_ns();

    if (0 < len) {
        if (XA_OK == check_response(dns_buf, len, &msg, &err)) {
            reassemble(&msg, &out->reassembled, &out->reassembled_len, &err);
        }

        /* Only copy the DNS record if it's asked for. */
        if (in->keep_debug_bufs) {
            out->raw_dns = xa_memdup(dns_buf, (size_t)len);
            out->raw_dns_len = (size_t)len;
        }
    } else {
        err = XA_DNS_RESOLVER_ERROR;
    }

    res_nclose(&state);

    return err;
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
XAcode dns_token_fetch(const struct dns_token_in *in, struct dns_token_out *out)
{
    XAcode rv;
    int64_t start_time = time_boot_now_ns();
    int64_t fetch_time = 0;
    int64_t total_time = 0;

    rv = query_dns(in, out, &fetch_time);

    if (XA_OK == rv) {
        out->cjwt_rv = cjwt_decode(out->reassembled, out->reassembled_len, 0,
                                   in->key, in->len,
                                   in->now, in->skew, &out->jwt);

        if (CJWTE_OK != out->cjwt_rv) {
            rv = XA_JWT_DECODE_ERROR;
        }
    }

    if (false == in->keep_debug_bufs) {
        if (out->reassembled) {
            free(out->reassembled);
            out->reassembled = NULL;
        }
        out->reassembled_len = 0;
    }

    out->fetch_time = time_diff(start_time, fetch_time);

    total_time = time_boot_now_ns();

    out->total_time = time_diff(start_time, total_time);

    return rv;
}
