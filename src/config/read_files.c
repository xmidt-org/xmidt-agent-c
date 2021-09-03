/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-FileCopyrightText: 2021 Weston Schmidt */
/* SPDX-License-Identifier: Apache-2.0 */

#include <cutils/file.h>
#include <cutils/memory.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../log.h"
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
/* none */

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
/**
 *  Mostly this function acquires the json text and sends it to processing
 *  downstream.  This function creates/destroys the context.
 */
XAcode file_to_config(const char *filename, struct config_building *cfg, XAcode *rv)
{
    char *data = NULL;
    size_t len = 0;
    cJSON *json = NULL;
    struct config_ctx ctx;

    memset(&ctx, 0, sizeof(struct config_ctx));
    ctx.filename = filename;

    *rv = XA_OK;
    if (0 != freadall(filename, 0, (void **)&data, &len)) {
        *rv = XA_FAILED_TO_OPEN_FILE;
        return *rv;
    }

    json = cJSON_ParseWithLength(data, len);
    if (json) {
        json_to_config(json, &ctx, cfg, rv);
        cJSON_Delete(json);
    }

    free(data);
    return XA_OK;
}


/**
 *  config_read is the high level call that converts a path into the
 *  configuration object.
 */
config_t *config_read(const char *path, XAcode *rv_in)
{
    struct config_building cb;
    struct cfg_file *list = NULL;
    XAcode rv = XA_OK;

    list = cfg_file_list_from_path(path, ".json", &rv);

    switch (rv) {
    case XA_OK:
        break;
    case XA_NOT_A_DIR:
        log_fatal("The argument '%s' is not a directory.", path);
        return NULL;
    case XA_PATH_TOO_LONG:
        log_fatal("The argument '%s' is too long.", path);
        return NULL;
    case XA_FAILED_TO_OPEN_FILE:
        log_fatal("The argument '%s' could not be opened.", path);
        return NULL;
    default:
        log_fatal("An unknown error occured.");
        return NULL;
    }

    memset(&cb, 0, sizeof(struct config_building));

    cb.c = must_calloc(sizeof(config_t), 1);

    log_info("config files found:");
    for (struct cfg_file *p = list; p; p = p->next) {
        log_info("\t%s", p->filename);
        file_to_config(p->filename, &cb, &rv);
    }

    finalize(&cb, &rv);

    return cb.c;
}

