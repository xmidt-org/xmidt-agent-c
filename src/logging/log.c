/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

/* Needed for gmtime_r() */
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <cutils/printf.h>
#include <otelc/time.h>

#include "log.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */
#define BOLD "\x1b[1m"
#define DIM  "\x1b[2m"
#define DRED "\x1b[31m"
#define DYEL "\x1b[33m"
#define WHT  "\x1b[97m"

#define TS DIM WHT

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
enum level {
    TRACE = 0,
    DEBUG = 1,
    INFO  = 2,
    WARN  = 3,
    ERROR = 4,
    FATAL = 5,
};

struct log_opts {
    const char *ts_color;
    const char *level_color;
    const char *level;
    const char *payload_color;
};

const struct log_opts _opts[6] = {
    {.ts_color      = TS,
     .level_color   = "\x1b[2m",
     .level         = "TRACE",
     .payload_color = ""},
    {.ts_color      = TS,
     .level_color   = "",
     .level         = "DEBUG",
     .payload_color = ""},
    {.ts_color      = TS,
     .level_color   = DIM DYEL,
     .level         = "INFO ",
     .payload_color = ""},
    {.ts_color      = TS,
     .level_color   = DYEL,
     .level         = "WARN ",
     .payload_color = ""},
    {.ts_color      = TS,
     .level_color   = DIM DRED,
     .level         = "ERROR",
     .payload_color = ""},
    {.ts_color      = TS,
     .level_color   = BOLD DRED,
     .level         = "FATAL",
     .payload_color = ""},
};

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
static void _log(enum level level, const char *format, va_list args)
{
    char ts[32];
    time_t now;
    struct tm utc;
    char *payload = NULL;

    /* Generate an RFC3339 timestamp string */
    time(&now);
    gmtime_r(&now, &utc);
    strftime(ts, sizeof(ts) - 1, "%F %TZ", &utc);

    payload = mvaprintf(format, args);
    if (payload) {
        const struct log_opts *opt = &_opts[level];
        fprintf(stdout, "%s%s\x1b[0m | %s%s\x1b[0m | %s%s\x1b[0m\n",
                opt->ts_color, ts, opt->level_color, opt->level,
                opt->payload_color, payload);
        free(payload);
    }
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
void log_trace(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    _log(TRACE, format, args);
    va_end(args);
}


void log_debug(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    _log(DEBUG, format, args);
    va_end(args);
}


void log_info(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    _log(INFO, format, args);
    va_end(args);
}


void log_warn(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    _log(WARN, format, args);
    va_end(args);
}


void log_error(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    _log(ERROR, format, args);
    va_end(args);
}


void log_fatal(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    _log(FATAL, format, args);
    va_end(args);
}


void log_va_trace(const char *format, va_list args)
{
    _log(TRACE, format, args);
}


void log_va_debug(const char *format, va_list args)
{
    _log(DEBUG, format, args);
}


void log_va_info(const char *format, va_list args)
{
    _log(INFO, format, args);
}


void log_va_warn(const char *format, va_list args)
{
    _log(WARN, format, args);
}


void log_va_error(const char *format, va_list args)
{
    _log(ERROR, format, args);
}


void log_va_fatal(const char *format, va_list args)
{
    _log(FATAL, format, args);
}
