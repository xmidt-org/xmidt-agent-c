/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include "../logging/log.h"
#include "config.h"
#include "internal.h"


/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define COLOR "\x1b[38;5;99m"
#define RST   "\x1b[0m"

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
void config_print(const config_t *c)
{
    if (c) {
        int offset = 28;
        log_debug(COLOR "-- identity --------------------------------------" RST);
        log_debug("%-*s: '%s'", offset, ".identity.device_id", c->identity.device_id.s);
        log_debug("%-*s: '%s'", offset, ".identity.partner_id", c->identity.partner_id.s);
        log_debug(COLOR "-- hardware --------------------------------------" RST);
        log_debug("%-*s: '%s'", offset, ".hardware.model", c->hardware.model.s);
        log_debug("%-*s: '%s'", offset, ".hardware.serial_number", c->hardware.serial_number.s);
        log_debug("%-*s: '%s'", offset, ".hardware.manufacturer", c->hardware.manufacturer.s);
        log_debug("%-*s: '%s'", offset, ".hardware.last_reboot_reason", c->hardware.last_reboot_reason.s);
        log_debug(COLOR "-- firmware --------------------------------------" RST);
        log_debug("%-*s: '%s'", offset, ".firmware.name", c->firmware.name.s);
        log_debug(COLOR "-- behavior --------------------------------------" RST);
        log_debug("%-*s: '%s'", offset, ".behavior.url", c->behavior.url.s);
        log_debug("%-*s: %d", offset, ".behavior.ping_timeout", c->behavior.ping_timeout);
        log_debug("%-*s: %d", offset, ".behavior.backoff_max", c->behavior.backoff_max);
        log_debug("%-*s: %d", offset, ".behavior.force_ip", c->behavior.force_ip);
        log_debug(COLOR "-- behavior.interface ----------------------------" RST);
        for (size_t i = 0; i < c->behavior.interface_count; i++) {
            log_debug(".behavior.interface[%zd].name: '%s'", i, c->behavior.interfaces[i].name.s);
            log_debug(".behavior.interface[%zd].cost: %d", i, c->behavior.interfaces[i].cost);
        }
        log_debug(COLOR "-- behavior.dns_txt ------------------------------" RST);
        log_debug("%-*s: '%s'", offset, ".behavior.dns_txt.base_fqdn", c->behavior.dns_txt.base_fqdn.s);
        log_debug(COLOR "-- behavior.dns_txt.jwt.alg_allowed --------------" RST);
        for (size_t i = 0; i < c->behavior.dns_txt.jwt.alg_count; i++) {
            log_debug(".behavior.dns_txt.jwt.algs[%zd]: %d", i, c->behavior.dns_txt.jwt.algs[i]);
        }
        log_debug(COLOR "-- behavior.issuer -------------------------------" RST);
        log_debug("%-*s: '%s'", offset, ".behavior.issuer.url", c->behavior.issuer.url.s);
        log_debug(COLOR "--------------------------------------------------" RST);
    }
}
