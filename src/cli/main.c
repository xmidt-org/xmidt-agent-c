/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "../config/config.h"
#include "../logging/log.h"
#include "config.h"
#include "signals.h"

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
static volatile bool done;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/

static void handle_lifecycle_command(enum signals_command e)
{
    (void) e;

    done = true;
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    XAcode xa_rv = XA_OK;
    config_t *c  = NULL;

    /* Handle args */
    log_info("hello, world");
    c = config_from_cli(argc, (const char **) argv, &xa_rv);
    if (!c) {
        return -1;
    }

    signals_config(&handle_lifecycle_command);

    done = false;

    while (!done) {
        /* Get auth JWT */

        /* Perform DNS TXT lookup */

        /* Connect the websocket */
        sleep(1);
    }

    /* Clean up */
    config_destroy(c);

    return 0;
}
