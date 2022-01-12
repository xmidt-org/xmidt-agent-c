/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <cutils/memory.h>
#include <cutils/printf.h>
#include <cutils/strings.h>
#include <dirent.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "internal.h"

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

static bool does_ext_match(const char *ext, size_t ext_len, const char *filename)
{
    size_t len = 0;

    /* No extension = match everything */
    if (ext_len) {

        len = cu_strnlen(filename, NAME_MAX);

        if (len < ext_len) {
            /* filename is too short */
            return false;
        }

        if (0 != memcmp(ext, &filename[len - ext_len], ext_len)) {
            /* doesn't match */
            return false;
        }
    }

    return true;
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
struct cfg_file *cfg_file_insert(struct cfg_file *list, char *filename)
{
    struct cfg_file *p    = list;
    struct cfg_file *last = NULL;
    struct cfg_file *node = must_calloc(sizeof(struct cfg_file), 1);

    node->filename = filename;
    node->next     = NULL;
    if (NULL == p) {
        return node;
    }

    while (p) {
        /*               bob          sam */
        int cmp = strcmp(p->filename, node->filename);
        if (cmp < 0) {
            /* p < n, move on */
            last = p;
            p    = p->next;
        } else if (NULL == last) {
            /* p == n, or n < p, insert at the beginning of the list */
            node->next = list;
            return node;
        } else {
            /* p == n, or n < p, insert after the first node */
            node->next = last->next;
            last->next = node;
            return list;
        }
    }

    /* insert at the end of the list */
    last->next = node;

    return list;
}


void cfg_file_list_destroy(struct cfg_file *list)
{
    struct cfg_file *next = NULL;

    while (list) {
        next = list->next;
        if (list->filename) {
            free(list->filename);
        }
        free(list);

        list = next;
    }
}


struct cfg_file *cfg_file_list_from_path(const char *path,
                                         const char *ext,
                                         XAcode *rv)
{
    struct stat sb;
    DIR *d                = NULL;
    struct dirent *dir    = NULL;
    struct cfg_file *list = NULL;
    size_t path_len       = cu_strnlen(path, (PATH_MAX + 1));
    size_t ext_len        = cu_strnlen(ext, NAME_MAX);

    if (path_len == (PATH_MAX + 1)) {
        *rv = XA_PATH_TOO_LONG;
        return NULL;
    }

    /* If the last character is a '/' for the path, don't include it later
     * to prevent // from showing up in the filename+path */
    if ('/' == path[path_len - 1]) {
        path_len--;
    }

    if ((0 != stat(path, &sb)) || !S_ISDIR(sb.st_mode)) {
        *rv = XA_NOT_A_DIR;
        return NULL;
    }

    d = opendir(path);
    if (!d) {
        *rv = XA_FAILED_TO_OPEN_FILE;
        return NULL;
    }

    while ((dir = readdir(d)) != NULL) {
        char *fn = NULL;

        if (!does_ext_match(ext, ext_len, dir->d_name)) {
            /* Skip if it doesn't match */
            continue;
        }

        fn = must_maprintf("%.*s/%s", (int) path_len, path, dir->d_name);

        if ((0 == stat(fn, &sb)) && S_ISREG(sb.st_mode)) {
            list = cfg_file_insert(list, fn);
        } else {
            free(fn);
        }
    }

    closedir(d);

    return list;
}
