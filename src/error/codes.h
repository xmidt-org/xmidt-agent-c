/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
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
    XA_OUT_OF_MEMORY,         /*  1 */
    XA_DNS_FORMAT_ERROR,      /*  2 */
    XA_DNS_SERVER_ERROR,      /*  3 */
    XA_DNS_NAME_ERROR,        /*  4 */
    XA_DNS_NOT_IMPLEMENTED,   /*  5 */
    XA_DNS_REFUSED,           /*  6 */
    XA_DNS_UNKNOWN_VALUE,     /*  7 */
    XA_DNS_TOO_FEW_ANSWERS,   /*  8 */
    XA_DNS_RESOLVER_ERROR,    /*  9 */
    XA_JWT_DECODE_ERROR,      /* 10 */
    XA_INVALID_INPUT,         /* 11 */
    XA_DNS_RECORD_TOO_SHORT,  /* 12 */
    XA_DNS_RECORD_INVALID,    /* 13 */
    XA_DNS_TOKEN_NOT_PRESENT, /* 14 */
    XA_NOT_A_DIR,             /* 15 */
    XA_PATH_TOO_LONG,         /* 16 */
    XA_FAILED_TO_OPEN_FILE,   /* 17 */
    XA_FAILED_TO_READ_FILE,   /* 18 */
    XA_CLI_ERROR,             /* 19 */
    XA_CONFIG_FILE_ERROR,     /* 20 */

    XA_LAST /* never use! */
} XAcode;


/**
 * Helpful function that sets the error code e to the error variable err if it
 * is not NULL, and returns the same error code.  This is a helps reduce a bunch
 * of if statements making it instead:
 *
 * return xa_set_error(err, XA_OUT_OF_MEMORY);
 */
XAcode xa_set_error(XAcode *err, XAcode e);

#endif
