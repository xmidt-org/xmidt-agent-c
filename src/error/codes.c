/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "codes.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define MAKE_ERROR_MAP_ENTRY(entry)    \
    {                                  \
        .code = entry, .desc = #entry, \
    }

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
struct error_map {
    XAcode code;
    const char *desc;
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static struct error_map map[] = {
    /* Keep this list matching the XAcode enum list. */
    MAKE_ERROR_MAP_ENTRY(XA_OK),
    MAKE_ERROR_MAP_ENTRY(XA_OUT_OF_MEMORY),
    MAKE_ERROR_MAP_ENTRY(XA_DNS_FORMAT_ERROR),
    MAKE_ERROR_MAP_ENTRY(XA_DNS_SERVER_ERROR),
    MAKE_ERROR_MAP_ENTRY(XA_DNS_NAME_ERROR),
    MAKE_ERROR_MAP_ENTRY(XA_DNS_NOT_IMPLEMENTED),
    MAKE_ERROR_MAP_ENTRY(XA_DNS_REFUSED),
    MAKE_ERROR_MAP_ENTRY(XA_DNS_UNKNOWN_VALUE),
    MAKE_ERROR_MAP_ENTRY(XA_DNS_TOO_FEW_ANSWERS),
    MAKE_ERROR_MAP_ENTRY(XA_DNS_RESOLVER_ERROR),
    MAKE_ERROR_MAP_ENTRY(XA_JWT_DECODE_ERROR),
    MAKE_ERROR_MAP_ENTRY(XA_INVALID_INPUT),
    MAKE_ERROR_MAP_ENTRY(XA_DNS_RECORD_TOO_SHORT),
    MAKE_ERROR_MAP_ENTRY(XA_DNS_RECORD_INVALID),
    MAKE_ERROR_MAP_ENTRY(XA_DNS_TOKEN_NOT_PRESENT),
    MAKE_ERROR_MAP_ENTRY(XA_NOT_A_DIR),
    MAKE_ERROR_MAP_ENTRY(XA_PATH_TOO_LONG),
    MAKE_ERROR_MAP_ENTRY(XA_FAILED_TO_OPEN_FILE),
    MAKE_ERROR_MAP_ENTRY(XA_FAILED_TO_READ_FILE),
    MAKE_ERROR_MAP_ENTRY(XA_CLI_ERROR),
    MAKE_ERROR_MAP_ENTRY(XA_CONFIG_FILE_ERROR),
    MAKE_ERROR_MAP_ENTRY(XA_INSUFFICIENT_RESOURCES),
};

// clang-format off
// TODO: Once we have the internet, find how to assert that our struct matches
// all the values in the enum at compile time.
// _Static_assert(sizeof(map) / sizeof(struct error_map) == XA_LAST)
// clang-format on

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

XAcode xa_set_error(XAcode *err, XAcode e)
{
    if (err) {
        *err = e;
    }

    return e;
}

const char *xa_error_to_string(XAcode e)
{
    int e_int = (int) e;

    if ((e_int < 0) || (((int) XA_LAST) <= e_int)) {
        return "XAcode is out of bounds";
    }

    return map[e].desc;
}
