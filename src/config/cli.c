/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../log.h"
#include "config.h"
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
static void print_help(const char *program_name)
{
    fprintf(stderr, "Usage: %s --config.dir <directory>\n", program_name);
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
config_t *config_from_cli(int argc, const char *argv[], XAcode *rv)
{
    const char *path = NULL;

    if ((2 == argc) && (!strncmp("--config.dir=", argv[1], 13))) {
        path = &argv[1][13];
        if ('\0' == *path) {
            path = NULL;
        }
    } else if ((3 == argc) && (!strcmp("--config.dir", argv[1]))) {
        path = argv[2];
    }

    if (!path) {
        print_help(argv[0]);
        *rv = XA_CLI_ERROR;
        return NULL;
    }

    return config_read(path, rv);
}
