/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __CLI_CONFIG_H__
#define __CLI_CONFIG_H__

#include "../config/config.h"
#include "../error/codes.h"

/**
 *  config_from_cli takes the program cli arguments and uses them to make
 *  the overall config_t structure the rest of the program will use to define
 *  the behaviors.
 *
 *  The returned object needs to be cleaned up by calling config_destroy().
 */
config_t *config_from_cli(int argc, const char *argv[], XAcode *rv);

#endif
