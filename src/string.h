/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __XA_STRING_H__
#define __XA_STRING_H__

#include <stddef.h>
#include <stdint.h>

struct xa_string {
    size_t len;
    char *s;
};

struct xa_const_string {
    size_t len;
    const char *s;
};

#endif
