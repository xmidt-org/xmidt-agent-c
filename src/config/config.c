/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <cutils/file.h>
#include <cutils/hashmap.h>
#include <cutils/memory.h>
#include <cutils/printf.h>
#include <cutils/strings.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../logging/log.h"
#include "config.h"
#include "internal.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define CTX_MAX_DEPTH 5

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
struct config_ctx {
    const char *filename;
    size_t depth;
    const char *obj[CTX_MAX_DEPTH];
};

struct config_building {
    config_t *c;
    hashmap_t interfaces;
    hashmap_t jwt_algs;
    int i;
    bool failed;
};

struct config_map {
    const char *s;
    int val;
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static const struct config_map tls_map[] = {
    {.s = "1.0", .val = (int) TLS_VERSION__1_0},
    {.s = "1.1", .val = (int) TLS_VERSION__1_1},
    {.s = "1.2", .val = (int) TLS_VERSION__1_2},
    {.s = "1.3", .val = (int) TLS_VERSION__1_3},
    {.s = "max", .val = (int) TLS_VERSION__MAX},
    { .s = NULL,                      .val = 0},
};

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
static void output_error(const struct config_ctx *ctx, const char *name,
                         const char *type)
{
    char *obj = NULL;
    for (int i = ctx->depth - 1; 0 < i; i--) {
        if (!obj) {
            obj = must_maprintf(".%s", ctx->obj[i]);
        } else {
            char *tmp = must_maprintf(".%s%s", ctx->obj[i], obj);
            free(obj);
            obj = tmp;
        }
    }
    log_fatal("Error in file: %s in object '%s' is not %s.",
              ctx->filename, obj, name, type);

    if (obj) {
        free(obj);
    }
}


/**
 *  Convert what we expect to be a string object into an xa_string or fail
 *  if that is not possible.
 */
static XAcode process_string(const cJSON *json, const struct config_ctx *ctx,
                             const char *name, struct xa_string *dest,
                             XAcode *rv)
{
    const cJSON *val = cJSON_GetObjectItemCaseSensitive(json, name);

    if (val) {
        if (val->type != cJSON_String) {
            output_error(ctx, name, "a string");
            *rv = XA_CONFIG_FILE_ERROR;
            return *rv;
        }

        /* Free anything that was there. */
        if (dest->s) {
            free(dest->s);
        }

        dest->len = cu_strnlen(val->valuestring, SIZE_MAX);
        dest->s   = cu_must_strndup(val->valuestring, dest->len);
    }

    return *rv;
}


/**
 *  Convert what we expect to be a number object into an xa_string or fail
 *  if that is not possible.
 */
static XAcode process_int___(const cJSON *json, const struct config_ctx *ctx,
                             const char *name, int *dest, XAcode *rv)
{
    const cJSON *val = cJSON_GetObjectItemCaseSensitive(json, name);

    if (val) {
        if (val->type == cJSON_Number) {
            *dest = val->valueint;
        } else {
            output_error(ctx, name, "a number");
            *rv = XA_CONFIG_FILE_ERROR;
            return *rv;
        }
    }

    return *rv;
}


/**
 *  Convert what we expect to be an enum object from an xa_string or fail
 *  if that is not possible.  The config_map should be an array with the last
 *  item being set to all NULL/0.
 */
static XAcode process_enum__(const cJSON *json, const struct config_ctx *ctx,
                             const char *name, int *dest, const struct config_map *map,
                             XAcode *rv)
{
    const cJSON *val = cJSON_GetObjectItemCaseSensitive(json, name);

    if (val) {
        if (val->type != cJSON_String) {
            output_error(ctx, name, "an enum");
            *rv = XA_CONFIG_FILE_ERROR;
            return *rv;
        }

        if (val->valuestring) {
            int i = 0;

            /* do the mapping. */
            while (map[i].s && strcmp(map[i].s, val->valuestring)) {
                i++;
            }

            if (map[i].s) {
                *dest = map[i].val;
            } else {
                output_error(ctx, name, "an enum");
                *rv = XA_CONFIG_FILE_ERROR;
            }
        }
    }

    return *rv;
}


static XAcode process_jwt_alg_array(const cJSON *json, const struct config_ctx *ctx,
                                    const char *name, struct config_building *cfg,
                                    XAcode *rv)
{
    const cJSON *val = cJSON_GetObjectItemCaseSensitive(json, name);
    int len          = 0;

    if (!val) {
        return *rv;
    }

    if (val->type != cJSON_Array) {
        output_error(ctx, name, "an array[string]");
        *rv = XA_CONFIG_FILE_ERROR;
        return *rv;
    }

    len = cJSON_GetArraySize(val);
    for (int i = 0; i < len; i++) {
        const cJSON *item = cJSON_GetArrayItem(val, i);

        if (item->type != cJSON_String) {
            output_error(ctx, name, "an array[string]");
            *rv = XA_CONFIG_FILE_ERROR;
        } else {
            struct xa_string tmp;

            tmp.len = cu_strnlen(item->valuestring, SIZE_MAX);
            tmp.s   = cu_must_strndup(item->valuestring, tmp.len);

            hashmap_put(&cfg->jwt_algs, tmp.s, tmp.len, (void *) 1);
        }
    }

    return *rv;
}

/**
 *  Goes one layer deeper into the structure and updates the context so
 *  meaningful errors can be printed out.  Call end_obj() when done.
 */
static const cJSON *process_obj(const cJSON *json, struct config_ctx *ctx,
                                const char *name)
{
    const cJSON *obj = cJSON_GetObjectItemCaseSensitive(json, name);

    if (obj) {
        if (CTX_MAX_DEPTH <= ctx->depth) {
            /* This is really a bug at build time, but this is still a nice
             * sanity check. */
            log_fatal("Unable to go beyond %d stack layers in the context.",
                      CTX_MAX_DEPTH);
            abort();
        }
        ctx->obj[ctx->depth] = name;
        ctx->depth++;
        log_info("ctx->depth: %d\n", ctx->depth);
    }

    return obj;
}


/**
 *  Does whatever cleanup is needed from process_obj().
 */
static void end_obj(struct config_ctx *ctx)
{
    ctx->depth--;
}


/**
 *  Does the processing of the interface structure into the intermediary
 *  form.
 */
static XAcode process_interfaces(const cJSON *json, struct config_ctx *ctx,
                                 struct config_building *cfg, XAcode *rv)
{
    const cJSON *obj = process_obj(json, ctx, "interfaces");

    if (!obj) {
        return *rv;
    }

    if (obj->type != cJSON_Array) {
        output_error(ctx, "interfaces", "an array");
        *rv = XA_CONFIG_FILE_ERROR;
    } else {
        int len = cJSON_GetArraySize(obj);

        for (int i = 0; i < len; i++) {
            const cJSON *item         = cJSON_GetArrayItem(obj, i);
            struct interface *ni      = must_calloc(sizeof(struct interface), 1);
            struct interface *present = NULL;

            process_string(item, ctx, "name", &ni->name, rv);
            process_int___(item, ctx, "cost", &ni->cost, rv);

            present = hashmap_get(&cfg->interfaces, ni->name.s, ni->name.len);
            if (NULL != present) {
                present->cost = ni->cost;

                /* We don't need the new one we made */
                free(ni->name.s);
                free(ni);
            } else {
                hashmap_put(&cfg->interfaces, ni->name.s, ni->name.len, ni);
            }
        }
    }

    end_obj(ctx);

    return *rv;
}


/**
 *  This is a callback designed to process the interface hashmap.  It will
 *  move the useful bits into the newly allocated array and then clean up
 *  everything on it's way out.
 */
static int copy_interfaces(void *const ctx, struct hashmap_element *const e)
{
    struct config_building *cfg = (struct config_building *) ctx;
    struct interface *ifc       = (struct interface *) e->data;

    cfg->c->behavior.interfaces[cfg->i].name.s   = ifc->name.s;
    cfg->c->behavior.interfaces[cfg->i].name.len = ifc->name.len;
    cfg->c->behavior.interfaces[cfg->i].cost     = ifc->cost;

    free(ifc);

    /* Move to the next slot. */
    cfg->i++;

    /* remove the item and continue, emptying the list */
    return -1;
}


/**
 *  This is a callback designed to process the jwt algorithm hashmap.  It
 *  will move the useful bits into the array and then clean up everything on
 *  it's way out.
 */
static int copy_jwts(void *const ctx, struct hashmap_element *const e)
{
    struct config_building *cfg = (struct config_building *) ctx;
    cjwt_alg_t alg;

    /* Convert the string to the enum or mark a failure. */
    if (CJWTE_OK != cjwt_alg_string_to_enum(e->key, e->key_len, &alg)) {
        log_fatal("Invalid JWT algorithm: %s", e->key);
        cfg->failed = true;
    }

    cfg->c->behavior.dns_txt.jwt.algs[cfg->i] = alg;

    /* Move to the next slot. */
    cfg->i++;

    /* Cast to shed the const declaration. */
    free((void *) e->key);

    /* remove the item and continue, emptying the list */
    return -1;
}


/**
 *  The overall json object to config process.  From here anything that is
 *  more complex to decode is called.  If it's simple it's all done here.
 */
static XAcode json_to_config(const cJSON *json, struct config_ctx *ctx,
                             struct config_building *cfg, XAcode *rv)
{
    const cJSON *obj = NULL;

    obj = process_obj(json, ctx, "identity");
    if (obj) {
        process_string(obj, ctx, "device_id", &cfg->c->identity.device_id, rv);
        process_string(obj, ctx, "partner_id", &cfg->c->identity.partner_id, rv);
        end_obj(ctx);
    }

    obj = process_obj(json, ctx, "hardware");
    if (obj) {
        process_string(obj, ctx, "model", &cfg->c->hardware.model, rv);
        process_string(obj, ctx, "serial_number", &cfg->c->hardware.serial_number, rv);
        process_string(obj, ctx, "manufacturer", &cfg->c->hardware.manufacturer, rv);
        process_string(obj, ctx, "last_reboot_reason", &cfg->c->hardware.last_reboot_reason, rv);
        end_obj(ctx);
    }

    obj = process_obj(json, ctx, "firmware");
    if (obj) {
        process_string(obj, ctx, "name", &cfg->c->firmware.name, rv);
        end_obj(ctx);
    }

    obj = process_obj(json, ctx, "behavior");
    if (obj) {
        const cJSON *dns_txt = NULL;
        const cJSON *issuer  = NULL;

        process_string(obj, ctx, "url", &cfg->c->behavior.url, rv);
        process_int___(obj, ctx, "ping_timeout", &cfg->c->behavior.ping_timeout, rv);
        process_int___(obj, ctx, "backoff_max", &cfg->c->behavior.backoff_max, rv);
        process_int___(obj, ctx, "force_ip", &cfg->c->behavior.force_ip, rv);

        process_interfaces(obj, ctx, cfg, rv);

        dns_txt = process_obj(obj, ctx, "dns_txt");
        if (dns_txt) {
            const cJSON *jwt = NULL;
            process_string(dns_txt, ctx, "base_fqdn", &cfg->c->behavior.dns_txt.base_fqdn, rv);

            jwt = process_obj(dns_txt, ctx, "jwt");
            if (jwt) {
                process_string(jwt, ctx, "keys_dir", &cfg->c->behavior.dns_txt.jwt.keys_dir, rv);
                process_jwt_alg_array(jwt, ctx, "allowed_algs", cfg, rv);

                end_obj(ctx);
            }
            end_obj(ctx);
        }

        issuer = process_obj(obj, ctx, "issuer");
        if (issuer) {
            const cJSON *mtls = NULL;

            process_string(issuer, ctx, "url", &cfg->c->behavior.issuer.url, rv);
            process_int___(issuer, ctx, "request_timeout", &cfg->c->behavior.issuer.request_timeout, rv);
            process_int___(issuer, ctx, "max_redirects", &cfg->c->behavior.issuer.max_redirects, rv);
            process_enum__(issuer, ctx, "tls_version", (int *) &cfg->c->behavior.issuer.tls_version, tls_map, rv);
            process_string(issuer, ctx, "ca_bundle_path", &cfg->c->behavior.issuer.ca_bundle_path, rv);

            mtls = process_obj(issuer, ctx, "mtls");
            if (mtls) {
                process_string(mtls, ctx, "cert_path", &cfg->c->behavior.issuer.mtls.cert_path, rv);
                process_string(mtls, ctx, "private_key_path", &cfg->c->behavior.issuer.mtls.private_key_path, rv);
                end_obj(ctx);
            }
            end_obj(ctx);
        }

        end_obj(ctx);
    }

    return *rv;
}


/**
 *  Mostly this function acquires the json text and sends it to processing
 *  downstream.  This function creates/destroys the context.
 */
static XAcode file_to_config(const char *filename, struct config_building *cfg,
                             XAcode *rv)
{
    char *data  = NULL;
    size_t len  = 0;
    cJSON *json = NULL;
    struct config_ctx ctx;

    memset(&ctx, 0, sizeof(struct config_ctx));
    ctx.filename = filename;

    if (0 != freadall(filename, 0, (void **) &data, &len)) {
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
 *  After we've done all the initial processing this function merges all the
 *  information into the structure that is captured in hashmaps and similar.
 */
static XAcode finalize(struct config_building *cfg, XAcode *rv)
{
    size_t tmp = 0;

    tmp = hashmap_num_entries(&cfg->interfaces);

    cfg->c->behavior.interface_count = tmp;
    cfg->c->behavior.interfaces      = must_calloc(sizeof(struct interface), tmp);

    cfg->i = 0;
    hashmap_iterate_pairs(&cfg->interfaces, &copy_interfaces, cfg);
    hashmap_destroy(&cfg->interfaces);

    cfg->i      = 0;
    cfg->failed = false;
    tmp         = hashmap_num_entries(&cfg->jwt_algs);

    cfg->c->behavior.dns_txt.jwt.alg_count = tmp;
    hashmap_iterate_pairs(&cfg->jwt_algs, &copy_jwts, cfg);

    if (cfg->failed) {
        *rv = XA_CONFIG_FILE_ERROR;
    }
    hashmap_destroy(&cfg->jwt_algs);

    return *rv;
}


static void free_string(struct xa_string *s)
{
    if (s->s) {
        free(s->s);
    }
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/**
 *  config_read is the high level call that converts a path into the
 *  configuration object.
 */
config_t *config_read(const char *path, XAcode *rv)
{
    struct config_building cb;
    struct cfg_file *list = NULL;

    *rv = XA_OK;

    list = cfg_file_list_from_path(path, ".json", rv);

    switch (*rv) {
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
        file_to_config(p->filename, &cb, rv);
    }

    finalize(&cb, rv);

    cfg_file_list_destroy(list);

    if (XA_OK != *rv) {
        config_destroy(cb.c);
        cb.c = NULL;
    }

    return cb.c;
}


void config_destroy(config_t *c)
{
    if (c) {
        free_string(&c->identity.device_id);
        free_string(&c->identity.partner_id);

        free_string(&c->hardware.model);
        free_string(&c->hardware.serial_number);
        free_string(&c->hardware.manufacturer);
        free_string(&c->hardware.last_reboot_reason);

        free_string(&c->firmware.name);

        free_string(&c->behavior.url);
        if (c->behavior.interfaces) {
            for (size_t i = 0; i < c->behavior.interface_count; i++) {
                free_string(&c->behavior.interfaces[i].name);
            }
            free(c->behavior.interfaces);
        }

        free_string(&c->behavior.dns_txt.base_fqdn);

        free_string(&c->behavior.dns_txt.jwt.keys_dir);

        free_string(&c->behavior.issuer.ca_bundle_path);
        free_string(&c->behavior.issuer.url);

        free_string(&c->behavior.issuer.mtls.cert_path);
        free_string(&c->behavior.issuer.mtls.private_key_path);

        free(c);
    }
}
