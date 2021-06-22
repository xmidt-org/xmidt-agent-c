/* SPDX-FileCopyrightText: 1998-2021 Daniel Stenberg, <daniel@haxx.se>, et al */
/* SPDX-License-Identifier: MIT */

#ifndef __XA_CTYPE_H__
#define __XA_CTYPE_H__

/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2020, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

/* Originally from:
 * https://github.com/curl/curl/blob/master/lib/curl_ctype.h
 *
 * Commit:
 * https://github.com/curl/curl/commit/ac0a88fd2552524955233198de96cc66f6b15a07
 *
 * Date:
 * June 21, 2021
 *
 * Changes:
 *    - Renamed Curl_* to be xa_*
 *    - Replaced the wrapper #ifdef with the filename
 *    - Removed EBCDIC ifdef
 */


int xa_isspace(int c);
int xa_isdigit(int c);
int xa_isalnum(int c);
int xa_isxdigit(int c);
int xa_isgraph(int c);
int xa_isprint(int c);
int xa_isalpha(int c);
int xa_isupper(int c);
int xa_islower(int c);
int xa_iscntrl(int c);

#define ISSPACE(x) (xa_isspace((int)((unsigned char)x)))
#define ISDIGIT(x) (xa_isdigit((int)((unsigned char)x)))
#define ISALNUM(x) (xa_isalnum((int)((unsigned char)x)))
#define ISXDIGIT(x) (xa_isxdigit((int)((unsigned char)x)))
#define ISGRAPH(x) (xa_isgraph((int)((unsigned char)x)))
#define ISALPHA(x) (xa_isalpha((int)((unsigned char)x)))
#define ISPRINT(x) (xa_isprint((int)((unsigned char)x)))
#define ISUPPER(x) (xa_isupper((int)((unsigned char)x)))
#define ISLOWER(x) (xa_islower((int)((unsigned char)x)))
#define ISCNTRL(x) (xa_iscntrl((int)((unsigned char)x)))
#define ISASCII(x) (((x) >= 0) && ((x) <= 0x80))

#define ISBLANK(x) (int)((((unsigned char)x) == ' ') || (((unsigned char)x) == '\t'))

#endif
