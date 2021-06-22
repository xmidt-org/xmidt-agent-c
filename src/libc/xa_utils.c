/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "xa_utils.h"

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


int xa_toupper(int c)
{
    /* Note: Does not work on EBCDIC systems */
    if (('a' <= c) && (c <= 'z')) {
        return 'A' + c - 'a';
    }

    return c;
}


int xa_tolower(int c)
{
    /* Note: Does not work on EBCDIC systems */
    if (('A' <= c) && (c <= 'Z')) {
        return 'a' + c - 'A';
    }

    return c;
}


int xa_strncasecmp(const char *s1, const char *s2, size_t maxlen)
{
    int c;

    if ((0 == maxlen) || (s1 == s2)) {
        return 0;
    }

    do {
        c = xa_tolower(*s1) - xa_tolower(*s2);
        maxlen--;
        s1++;
        s2++;
    } while (0 < maxlen && 0 == c);

    return c;
}


void *xa_saferealloc(void *ptr, size_t size)
{
    void *p = NULL;

    /* Consistently always return NULL in this case. */
    if (!ptr && !size) {
        return NULL;
    }

    p = realloc(ptr, size);

    /* If size == 0, ptr has been freed. */
    if (!p && ptr && (0 < size)) {
        free(ptr);
    }

    return p;
}


void *xa_memdup(const void *src, size_t len)
{
    void *dest = NULL;

    if (src && len) {
        dest = malloc(len);
        if (dest) {
            memcpy(dest, src, len);
        }
    }

    return dest;
}
