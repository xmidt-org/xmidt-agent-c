/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <curl/mprintf.h>

#include "auth_token.h"

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
static int set_string__opt(CURLcode *rv, CURL *curl, CURLoption opt, const char *s)
{
    if (!s) {
        return 0;
    }

    *rv = curl_easy_setopt(curl, opt, s);

    return (CURLE_OK == *rv) ? 0 : -1;
}


static int set_long____opt(CURLcode *rv, CURL *curl, CURLoption opt, long l)
{
    *rv = curl_easy_setopt(curl, opt, l);

    return (CURLE_OK == *rv) ? 0 : -1;
}


static int set_pointer_opt(CURLcode *rv, CURL *curl, CURLoption opt, void *p)
{
    *rv = curl_easy_setopt(curl, opt, p);

    return (CURLE_OK == *rv) ? 0 : -1;
}


static int set_cb______opt(CURLcode *rv, CURL *curl, CURLoption opt,
                           size_t (*cb)(const void *, size_t, size_t, void *))
{
    *rv = curl_easy_setopt(curl, opt, cb);

    return (CURLE_OK == *rv) ? 0 : -1;
}


static int set_verbose_opt(CURLcode *rv, CURL *curl, FILE *f)
{
    *rv = CURLE_OK;

    if (f) {
        *rv = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        if (*rv == CURLE_OK) {
            *rv = curl_easy_setopt(curl, CURLOPT_STDERR, f);
        }
    }

    return (CURLE_OK == *rv) ? 0 : -1;
}


static int append(struct curl_slist **list, char *header)
{
    if (header) {
        struct curl_slist *tmp;
        tmp = curl_slist_append(*list, header);
        curl_free(header);

        if (tmp) {
            *list = tmp;
            return 0;
        }
    }

    return -1;
}


static int append_string(struct curl_slist **list, const char *key, const char *val)
{
    if (!val) {
        return 0;
    }

    return append(list, curl_maprintf("%s: %s", key, val));
}


static int append_int(struct curl_slist **list, const char *key, const int *val)
{
    if (!val) {
        return 0;
    }

    return append(list, curl_maprintf("%s: %d", key, *val));
}


static int build_header_list(const struct auth_info *in, struct curl_slist **list)
{
    if (!append_int(list, "X-Midt-Boot-Retry-Wait", in->boot_retry_wait)
        && !append_string(list, "X-Midt-Mac-Address", in->mac_address)
        && !append_string(list, "X-Midt-Serial-Number", in->serial_number)
        && !append_string(list, "X-Midt-Uuid", in->uuid)
        && !append_string(list, "X-Midt-Partner-Id", in->partner_id)
        && !append_string(list, "X-Midt-Hardware-Model", in->hardware_model)
        && !append_string(list, "X-Midt-Hardware-Manufacturer", in->hardware_manufacturer)
        && !append_string(list, "X-Midt-Firmware-Name", in->firmware_name)
        && !append_string(list, "X-Midt-Protocol", in->protocol)
        && !append_string(list, "X-Midt-Interface-Used", in->interface)
        && !append_string(list, "X-Midt-Last-Reboot-Reason", in->last_reboot_reason)
        && !append_string(list, "X-Midt-Last-Reconnect-Reason", in->last_reconnect_reason))
    {
        return 0;
    }

    return -1;
}


static size_t response_cb(const void *buffer, size_t size, size_t nmemb, void *data)
{
    struct auth_response *r = (struct auth_response *)data;
    size_t len = size * nmemb;

    r->payload = realloc(r->payload, r->len + len);
    if (!r->payload) {
        r->len = 0;
        return 0;
    }

    memcpy(&(r->payload[r->len]), buffer, len);
    r->len += len;

    return len;
}


static int easy_getinfo_long(CURLcode *rv, CURL *curl, CURLINFO info, long *val)
{
    *rv = curl_easy_getinfo(curl, info, val);

    return (CURLE_OK == *rv) ? 0 : -1;
}


static int easy_getinfo_double(CURLcode *rv, CURL *curl, CURLINFO info, double *val)
{
    *rv = curl_easy_getinfo(curl, info, val);

    return (CURLE_OK == *rv) ? 0 : -1;
}


static int easy_getinfo__off_t(CURLcode *rv, CURL *curl, CURLINFO info, curl_off_t *val)
{
    *rv = curl_easy_getinfo(curl, info, val);

    return (CURLE_OK == *rv) ? 0 : -1;
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
CURLcode auth_token_req(const struct auth_info *in, struct auth_response *r)
{
    CURLcode rv = CURLE_OK;
    CURL *curl = NULL;
    struct curl_slist *list = NULL;
    long tls_version = CURL_SSLVERSION_MAX_DEFAULT;

    if (0 != in->tls_version) {
        tls_version = in->tls_version;
    }

    memset(r, 0, sizeof(struct auth_response));

    if (0 == build_header_list(in, &list)) {
        curl = curl_easy_init();
        if (!curl) {
            rv = CURLE_OUT_OF_MEMORY;
        }
    } else {
        rv = CURLE_OUT_OF_MEMORY;
    }

    /* Set everything up or fail */
    if (curl
        && !set_string__opt(&rv, curl, CURLOPT_URL, in->url)
        && !set_long____opt(&rv, curl, CURLOPT_SSLVERSION, tls_version)
        /* Setup the data callback handler */
        && !set_cb______opt(&rv, curl, CURLOPT_WRITEFUNCTION, response_cb)
        && !set_pointer_opt(&rv, curl, CURLOPT_WRITEDATA, r)
        /* Follow redirection the specified amount */
        && !set_long____opt(&rv, curl, CURLOPT_FOLLOWLOCATION, 1L)
        && !set_long____opt(&rv, curl, CURLOPT_MAXREDIRS, in->max_redirects)
        /* Make sure we have a reasonable timeout */
        && !set_long____opt(&rv, curl, CURLOPT_TIMEOUT, in->timeout)
        && !set_string__opt(&rv, curl, CURLOPT_INTERFACE, in->interface)
        /* Choose IPv4 or IPv6 */
        && !set_long____opt(&rv, curl, CURLOPT_IPRESOLVE, in->ip_resolve)
        /* Don't cache DNS */
        && !set_long____opt(&rv, curl, CURLOPT_DNS_CACHE_TIMEOUT, 0L)
        /* Don't try to reuse this connection */
        && !set_long____opt(&rv, curl, CURLOPT_FORBID_REUSE, 1L)
        && !set_long____opt(&rv, curl, CURLOPT_FRESH_CONNECT, 1L)
        /* Force hostname validation and peer cert validation */
        && !set_long____opt(&rv, curl, CURLOPT_SSL_VERIFYHOST, 2L)
        && !set_long____opt(&rv, curl, CURLOPT_SSL_VERIFYPEER, 1L)
        /* Set the mTLS cert information */
        && !set_string__opt(&rv, curl, CURLOPT_SSLCERT, in->client_cert_path)
        && !set_string__opt(&rv, curl, CURLOPT_SSLKEY, in->private_key_path)
        /* Set the CA bundle information */
        && !set_string__opt(&rv, curl, CURLOPT_CAINFO, in->ca_bundle_path)
        && !set_verbose_opt(&rv, curl, in->verbose_stream)
        && !set_pointer_opt(&rv, curl, CURLOPT_HTTPHEADER, list))
    {
        rv = curl_easy_perform(curl);
        r->state = REQ_STATE__PERFORMED;

        if ((CURLE_OK == rv)
            && !easy_getinfo_long(&rv, curl, CURLINFO_RESPONSE_CODE, &r->http_status)
            && !easy_getinfo_double(&rv, curl, CURLINFO_NAMELOOKUP_TIME, &r->namelookup)
            && !easy_getinfo_double(&rv, curl, CURLINFO_CONNECT_TIME, &r->connect)
            && !easy_getinfo_double(&rv, curl, CURLINFO_APPCONNECT_TIME, &r->appconnect)
            && !easy_getinfo_double(&rv, curl, CURLINFO_PRETRANSFER_TIME, &r->pretransfer)
            && !easy_getinfo_double(&rv, curl, CURLINFO_STARTTRANSFER_TIME, &r->starttransfer)
            && !easy_getinfo_double(&rv, curl, CURLINFO_TOTAL_TIME, &r->total)
            && !easy_getinfo_double(&rv, curl, CURLINFO_REDIRECT_TIME, &r->redirect)
            && !easy_getinfo__off_t(&rv, curl, CURLINFO_RETRY_AFTER, &r->retry_after))
        {
            /* We have a meaningful response */
            r->state = REQ_STATE__COMPLETED;
        }
    }

    curl_slist_free_all(list);
    if (curl) {
        curl_easy_cleanup(curl);
    }

    r->curl_rv = rv;

    return rv;
}
