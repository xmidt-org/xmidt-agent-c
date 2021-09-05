/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __CONFIG_CONFIG_H__
#define __CONFIG_CONFIG_H__

#include <cjwt/cjwt.h>
#include <stdbool.h>
#include <stddef.h>

#include "../error/codes.h"
#include "../string.h"

struct interface {
    struct xa_string name;
    int cost;
};

typedef struct {
    /* Identity */
    struct {
        struct xa_string device_id;
        struct xa_string partner_id;
    } identity;

    /* Hardware */
    struct {
        struct xa_string model;
        struct xa_string serial_number;
        struct xa_string manufacturer;
        struct xa_string last_reboot_reason;
    } hardware;

    /* Firmware */
    struct {
        struct xa_string name;
    } firmware;

    /* Behavior */
    struct {
        struct xa_string url;
        int ping_timeout;
        int backoff_max;
        int force_ip;

        size_t interface_count;
        struct interface *interfaces;

        struct {
            struct xa_string base_fqdn;

            struct {
                size_t alg_count;
                cjwt_alg_t algs[num_algorithms];

                struct xa_string keys_dir;
            } jwt;
        } dns_txt;

        struct {
            struct xa_string url;
        } issuer;
    } behavior;
} config_t;


/**
 *  config_read is the high level call that converts a path into the
 *  configuration object.
 */
config_t *config_read(const char *path, XAcode *rv);


/**
 *  config_destroy cleans up an resources associated with the config struct.
 */
void config_destroy(config_t *c);


/**
 *  A simple printer for the config object
 */
void config_print(const config_t *c);

#endif
