/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __CODES_H__
#define __CODES_H__

/* All possible error codes from all the xmidt-agent functions. Future versions
 * may return other values.
 *
 * Always add new return codes last.  Do not remove any.  The return codes
 * must remain the same.
 */
typedef enum {
    XA_OK = 0,
    XA_OUT_OF_MEMORY,      /*  1 */
    XA_FORMAT_ERROR,       /*  2 */
    XA_SERVER_ERROR,       /*  3 */
    XA_NAME_ERROR,         /*  4 */
    XA_NOT_IMPLEMENTED,    /*  5 */
    XA_REFUSED,            /*  6 */
    XA_UNKNOWN_VALUE,      /*  7 */
    XA_TOO_FEW_ANSWERS,    /*  8 */
    XA_DNS_RESOLVER_ERROR, /*  9 */
    XA_JWT_DECODE_ERROR,   /* 10 */
    XA_INVALID_INPUT,      /* 11 */

    XA_LAST /* never use! */
} XAcode;

#endif
