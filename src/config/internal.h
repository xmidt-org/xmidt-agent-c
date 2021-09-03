/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __CFG_FILE_H__
#define __CFG_FILE_H__

#include <cjson/cJSON.h>
#include <cutils/hashmap.h>

#include "../codes.h"
#include "config.h"

#define CTX_MAX_DEPTH 4

struct cfg_file {
    char *filename;

    /* A place for the data */
    size_t len;
    char *data;

    cJSON *tree;

    struct cfg_file *next;
};

/**
 *  Creates a cfg_file node, assigns the filename to it and inserts it
 *  into the specified list in alphabetic order.  If something goes wrong
 *  the list is cleaned up and NULL is returned.  A NULL list is ok, a NULL
 *  filename is not.
 *
 *  @param list     the list to add to
 *  @param filename the filename to add
 *
 *  @return the resulting list or NULL on error
 */
struct cfg_file *cfg_file_insert(struct cfg_file *list, char *filename);


/**
 *  Destroys a list and frees the filenames and nodes.  A NULL list is fine.
 */
void cfg_file_list_destroy(struct cfg_file *list);


/**
 *  Takes a path that we hope is a directory, finds all the files that match the
 *  extension provided and puts them in an alphabetized list that is returned.
 *
 *  rv will be set to one of:
 *      XA_OK
 *      XA_PATH_TOO_LONG
 *      XA_NOT_A_DIR
 *      XA_FAILED_TO_OPEN_FILE (could be either a file or a directory)
 */
struct cfg_file *cfg_file_list_from_path(const char *path,
                                         const char *ext,
                                         XAcode *rv);

config_t *config_read(const char *path, XAcode *rv);

#endif
