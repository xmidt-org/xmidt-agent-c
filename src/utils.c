/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

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
/* none */

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/


size_t xa_strnlen(const char *s, size_t maxlen)
{
    for (size_t i = 0; i < maxlen; i++) {
        if ('\0' == s[i]) {
            return i;
        }
    }

    return maxlen;
}


char *xa_strdup(const char *s)
{
    return xa_strndup(s, SIZE_MAX);
}


char *xa_strndup(const char *s, size_t maxlen)
{
    char *rv = NULL;

    if (s && (0 < maxlen)) {
        size_t len = xa_strnlen(s, maxlen);

        rv = (char *)malloc(len + 1);
        if (rv) {
            memcpy(rv, s, len);
            rv[len] = '\0';
        }
    }

    return rv;
}
