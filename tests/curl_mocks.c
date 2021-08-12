/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cutils/strings.h>

#define SEEN_AND_NEXT(type)    \
    do {                       \
        type->seen++;          \
        if (0 != type->more) { \
            type = &type[1];   \
        } else {               \
            type = NULL;       \
        }                      \
    } while (0)


/*----------------------------------------------------------------------------*/
/*                                 Mock Curl                                  */
/*----------------------------------------------------------------------------*/
#undef curl_easy_getinfo
#undef curl_easy_setopt

#if 0
static bool __curl_info_test_value = false;
static curl_version_info_data __curl_info;
curl_version_info_data *curl_version_info(CURLversion v)
{
    (void)v;

    if (!__curl_info_test_value) {
        __curl_info.version_num = 0x074200;
        __curl_info.version = "7.66.0";
    }

    return &__curl_info;
}
#endif


struct mock_curl_easy_init {
    const char *rv;
    int seen;
    int more;
};
static struct mock_curl_easy_init *__curl_easy_init = NULL;
CURL *curl_easy_init()
{
    CURL *rv = NULL;

    if (__curl_easy_init) {
        rv = (CURL *)__curl_easy_init->rv;
        SEEN_AND_NEXT(__curl_easy_init);
    } else {
        /* Really don't care, just not NULL. */
        rv = (CURL *)42;
    }

    return rv;
}

#if 0
struct mock_curl_easy_pause {
    int bitmask;
    int seen;
    int more;
};
static struct mock_curl_easy_pause *__curl_easy_pause = NULL;
CURLcode curl_easy_pause(CURL *easy, int bitmask)
{
    CU_ASSERT(NULL != easy);

    if (__curl_easy_pause) {
        CU_ASSERT(__curl_easy_pause->bitmask == bitmask);
        SEEN_AND_NEXT(__curl_easy_pause);
    }
    return CURLE_OK;
}
#endif


void *__writedata;
size_t (*__writefunction)(const void *, size_t, size_t, void *);
struct curl_slist *__curl_easy_setopt = NULL;
CURLcode curl_easy_setopt(CURL *easy, CURLoption option, ...)
{
    va_list ap;
    char buf[256];
    int width = 25;
    struct curl_slist *p = NULL;

    CU_ASSERT(NULL != easy);

    memset(buf, 0, sizeof(buf));

    va_start(ap, option);
    switch (option) {
    case CURLOPT_URL:
        snprintf(buf, sizeof(buf), "%-*s: %s", width, "CURLOPT_URL", va_arg(ap, const char *));
        break;
    case CURLOPT_FOLLOWLOCATION:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_FOLLOWLOCATION", va_arg(ap, long));
        break;
    case CURLOPT_MAXREDIRS:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_MAXREDIRS", va_arg(ap, long));
        break;
    case CURLOPT_IPRESOLVE:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_IPRESOLVE", va_arg(ap, long));
        break;
    case CURLOPT_INTERFACE:
        snprintf(buf, sizeof(buf), "%-*s: %s", width, "CURLOPT_INTERFACE", va_arg(ap, const char *));
        break;
    case CURLOPT_SSLVERSION:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_SSLVERSION", va_arg(ap, long));
        break;
    case CURLOPT_SSL_VERIFYPEER:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_SSL_VERIFYPEER", va_arg(ap, long));
        break;
    case CURLOPT_SSL_VERIFYHOST:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_SSL_VERIFYHOST", va_arg(ap, long));
        break;
    case CURLOPT_VERBOSE:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_VERBOSE", va_arg(ap, long));
        break;
    case CURLOPT_HTTP_VERSION:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_HTTP_VERSION", va_arg(ap, long));
        break;
    case CURLOPT_UPLOAD:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_UPLOAD", va_arg(ap, long));
        break;
    case CURLOPT_CUSTOMREQUEST:
        snprintf(buf, sizeof(buf), "%-*s: %s", width, "CURLOPT_CUSTOMREQUEST", va_arg(ap, const char *));
        break;
    case CURLOPT_FORBID_REUSE:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_FORBID_REUSE", va_arg(ap, long));
        break;
    case CURLOPT_FRESH_CONNECT:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_FRESH_CONNECT", va_arg(ap, long));
        break;
    case CURLOPT_STDERR:
        snprintf(buf, sizeof(buf), "%-*s: pointer", width, "CURLOPT_STDERR");
        break;
    case CURLOPT_WRITEFUNCTION:
        snprintf(buf, sizeof(buf), "%-*s: pointer", width, "CURLOPT_WRITEFUNCTION");
        __writefunction = va_arg(ap, size_t(*)(const void *, size_t, size_t, void *));
        break;
    case CURLOPT_WRITEDATA:
        snprintf(buf, sizeof(buf), "%-*s: pointer", width, "CURLOPT_WRITEDATA");
        __writedata = va_arg(ap, void *);
        break;
    case CURLOPT_TIMEOUT:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_TIMEOUT", va_arg(ap, long));
        break;
    case CURLOPT_DNS_CACHE_TIMEOUT:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_DNS_CACHE_TIMEOUT", va_arg(ap, long));
        break;
    case CURLOPT_SSLCERT:
        snprintf(buf, sizeof(buf), "%-*s: %s", width, "CURLOPT_SSLCERT", va_arg(ap, const char *));
        break;
    case CURLOPT_SSLKEY:
        snprintf(buf, sizeof(buf), "%-*s: %s", width, "CURLOPT_SSLKEY", va_arg(ap, const char *));
        break;
    case CURLOPT_CAINFO:
        snprintf(buf, sizeof(buf), "%-*s: %s", width, "CURLOPT_CAINFO", va_arg(ap, const char *));
        break;
    case CURLOPT_HTTPHEADER: /* pointer to headers */
        p = va_arg(ap, struct curl_slist *);
        break;
    default:
        snprintf(buf, sizeof(buf), "%-*s: %ld", width, "CURLOPT_unknown", (long)option);
        break;
    }
    va_end(ap);

    if (p) {
        while (p) {
            __curl_easy_setopt = curl_slist_append(__curl_easy_setopt, p->data);
            p = p->next;
        }
    } else {
        if (*buf) {
            __curl_easy_setopt = curl_slist_append(__curl_easy_setopt, buf);
        }
    }

    return CURLE_OK;
}


CURLcode curl_easy_getinfo(CURL *easy, CURLINFO info, ...)
{
    (void)easy;
    (void)info;

    return CURLE_OK;
}

void curl_easy_cleanup(CURL *easy)
{
    (void)easy;
}

struct curl_easy_perform_data {
    struct curl_easy_perform_data *next;
    char *payload;
    size_t len;
};

struct curl_easy_perform_data *__curl_easy_perform = NULL;
CURLcode __curl_easy_perform__rv = CURLE_OK;
CURLcode curl_easy_perform(CURL *easy)
{

    if (NULL != __writefunction) {
        while (__curl_easy_perform) {
            struct curl_easy_perform_data *p = __curl_easy_perform;
            size_t rv;

            rv = (*__writefunction)(p->payload, 1, p->len, __writedata);
            CU_ASSERT(p->len == rv);

            __curl_easy_perform = __curl_easy_perform->next;
        }
    }
    (void)easy;
    return __curl_easy_perform__rv;
}

#if 0
CURLMcode curl_multi_add_handle(CURL *easy, CURLM *multi_handle)
{
    (void)easy;
    (void)multi_handle;
    return CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURL *easy, CURLM *multi_handle)
{
    (void)easy;
    (void)multi_handle;
    return CURLM_OK;
}
#endif


/* These basically work like the real thing. */
struct curl_slist *curl_slist_append(struct curl_slist *list, const char *s)
{
    struct curl_slist *last;
    struct curl_slist *new;

    new = malloc(sizeof(struct curl_slist));
    if (!new) {
        return NULL;
    }

    new->next = NULL;
    new->data = cu_strdup(s);
    if (!new->data) {
        free(new);
        return NULL;
    }

    /* Empty starting list. */
    if (!list) {
        return new;
    }

    last = list;
    while (last->next) {
        last = last->next;
    }
    last->next = new;

    return list;
}

void curl_slist_free_all(struct curl_slist *list)
{
    while (list) {
        struct curl_slist *tmp;

        tmp = list;
        list = list->next;
        free(tmp->data);
        free(tmp);
    }
}

void curl_slist_print(struct curl_slist *list)
{
    while (list) {
        printf("%s\n", list->data);
        list = list->next;
    }
}


void curl_slist_compare(struct curl_slist *list, const char *s[])
{
    while (list) {
        int rv = strcmp(list->data, *s);
        if (0 != rv) {
            fprintf(stderr, "\nWanted: %s\n   Got: %s\n", *s, list->data);
        }
        CU_ASSERT_FATAL(0 == rv);
        list = list->next;
        s++;
    }
}

