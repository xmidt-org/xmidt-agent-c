/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "xa_file.h"

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
int fslirp(const char *filename, size_t max, void **data, size_t *len)
{
    FILE *f = NULL;
    long file_len = 0;

    f = fopen(filename, "rb");
    if (!f) {
        return EINVAL;
    }

    if (0 != fseek(f, 0, SEEK_END)) {
        fclose(f);

        /* probably EINVAL or ESPIPE */
        return errno;
    }

    file_len = ftell(f);
    if (-1 == file_len) {
        fclose(f);
        return errno;
    }

    if ((0 < max) && (max < file_len)) {
        fclose(f);
        return E2BIG;
    }

    if (0 != fseek(f, 0, SEEK_SET)) {
        fclose(f);

        /* probably EINVAL or ESPIPE */
        return errno;
    }

    *len = 0;
    *data = malloc(file_len);
    if (*data) {
        char *p = (char *)*data;
        size_t want = (size_t)file_len;
        size_t have = 0;

        while (!feof(f) && (0 < want)) {
            size_t got = fread(p, 1, want, f);
            p += got;
            have += got;
        }
        *len = have;
    }
    fclose(f);

    return 0;
}
