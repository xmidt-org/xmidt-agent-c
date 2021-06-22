/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __TIME_H__
#define __TIME_H__

#include <stdint.h>

struct time_metrics {
    /* The last time (seconds from boot) that the real clock was not correctly
     * set based on a query for real time.  0 if never invalid or queried. */
    int64_t last_invalid;

    /* The first time (seconds from boot) that the real clock was correctly set
     * based on a query for real time.  0 if never queried. */
    int64_t first_valid;

    /* The number of calls that resulted in an invalid time. */
    uint32_t invalid_count;

    /* The number of calls that resulted in an invalid time after the first
     * valid time was set. */
    uint32_t invalid_count_after_valid;
};

/**
 * A simple and consistent wrapper that gives the present number of seconds
 * since Jan 1, 1970 in UTC time.
 *
 * @note It is reasonable and possible that this clock is not set correctly.
 *
 * @return the number of seconds since Jan 1, 1970
 */
int64_t time_now_s(void);


/**
 * A simple and consistent wrapper that gives the present number of nanoseconds
 * since boot.
 *
 * @return the number of nanoseconds since booting
 */
int64_t time_boot_now_ns(void);


/**
 * A simple function that takes the difference between two nanosecond times
 * and returns a double representing the value.
 *
 * @param start the starting time
 * @param end   the end time
 *
 * @return the double representing the seconds between the two points
 */
double time_diff(int64_t start, int64_t end);


/**
 * Gets a copy of the time metrics right now.
 */
void time_get_metrics(struct time_metrics *m);
#endif
