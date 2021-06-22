/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

/* Needed for clock_gettime */
#define _POSIX_C_SOURCE 199309L

#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "xa_time.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* Any real (wall) clock should return a time after this for now since it's been
 * 50 years since Jan 1, 1970.  This value represents Jan 1, 2020 UTC. */
#define UNIX_AT_50 1577836800

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static pthread_mutex_t __time_mutex = PTHREAD_MUTEX_INITIALIZER;

static struct time_metrics __time_metrics = {
    .last_invalid = 0,
    .first_valid = 0,
    .invalid_count = 0,
    .invalid_count_after_valid = 0,
};

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
static int64_t __boot_now_s(void)
{
    struct timespec tp;
    int64_t rv = 0;

    if (0 == clock_gettime(CLOCK_MONOTONIC, &tp)) {
        rv = tp.tv_sec;
    }

    return rv;
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int64_t time_now_s(void)
{
    struct timespec tp;
    int64_t rv = 0;

    if (0 == clock_gettime(CLOCK_REALTIME, &tp)) {
        rv = tp.tv_sec;
    }

    if (tp.tv_sec < UNIX_AT_50) {
        int64_t bt = __boot_now_s();

        pthread_mutex_lock(&__time_mutex);
        if (0 == __time_metrics.first_valid) {
            if (__time_metrics.last_invalid < bt) {
                __time_metrics.last_invalid = bt;
            }
        } else {
            __time_metrics.invalid_count_after_valid++;
        }
        __time_metrics.invalid_count++;
        pthread_mutex_unlock(&__time_mutex);

        return 0;
    }

    /* Only set this field once. */
    if (0 == __time_metrics.first_valid) {
        pthread_mutex_lock(&__time_mutex);
        __time_metrics.first_valid = __boot_now_s();
        pthread_mutex_unlock(&__time_mutex);
    }


    return rv;
}


int64_t time_boot_now_ns(void)
{
    struct timespec tp;
    int64_t rv = 0;

    if (0 == clock_gettime(CLOCK_MONOTONIC, &tp)) {
        rv = tp.tv_sec;
        rv *= 1000000000;
        rv += tp.tv_nsec;
    }

    return rv;
}


double time_diff(int64_t start, int64_t end)
{
    double s = (double)start;
    double e = (double)end;

    s /= 1000000000;
    e /= 1000000000;

    return (e - s);
}


void time_get_metrics(struct time_metrics *m)
{
    if (m) {
        pthread_mutex_lock(&__time_mutex);
        memcpy(m, &__time_metrics, sizeof(struct time_metrics));
        pthread_mutex_unlock(&__time_mutex);
    }
}
