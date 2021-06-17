/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#ifndef __UTILS_H__
#define __UTILS_H__

#include <stddef.h>

/* Common non-standard implementations. */
size_t xa_strnlen(const char *s, size_t maxlen);
char *xa_strndup(const char *s, size_t maxlen);
char *xa_strdup(const char *s);

#endif
