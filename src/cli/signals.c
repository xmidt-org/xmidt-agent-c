/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "../logging/log.h"
#include "signals.h"

#ifdef INCLUDE_BREAKPAD
#include <breakpad_wrapper.h>
#endif

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */


/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef void (*command_t)(enum signals_command);


/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static void default_shutdown(enum signals_command);


/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static command_t command_fn = &default_shutdown;


/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
static void sig_ignore(int sig)
{
    log_trace("ignoring signal: '%s' (%d)", strsignal(sig), sig);
    signal(sig, sig_ignore);
}


static void sig_stop(int sig)
{
    log_fatal("handling signal: '%s' (%d)", strsignal(sig), sig);
    log_fatal("lifecycle command(STOP) received");
    (*command_fn)(SIG_EVENT__STOP);
}


static void sig_usr1(int sig)
{
    log_fatal("handling signal: '%s' (%d)", strsignal(sig), sig);
    log_fatal("lifecycle command(SYSTEM IS RESTARTING) received");
    (*command_fn)(SIG_EVENT__SYSTEM_IS_RESTARTING);
}


static void sig_term(int sig)
{
    log_fatal("handling signal: '%s' (%d)", strsignal(sig), sig);
    log_fatal("lifecycle command(TERMINATE) received");
    (*command_fn)(SIG_EVENT__TERMINATE);
}


static void default_shutdown(enum signals_command e)
{
    (void) e;

    log_fatal("lifecycle command: default handler just exit(-1)");
    exit(-1);
}


/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
void signals_config(void (*f)(enum signals_command))
{
    if (f) {
        command_fn = f;
    }

    signal(SIGINT, sig_stop);
    signal(SIGTERM, sig_term);
    signal(SIGUSR1, sig_usr1);
    signal(SIGQUIT, sig_stop);

    signal(SIGUSR2, sig_ignore);
    signal(SIGCHLD, sig_ignore);
    signal(SIGPIPE, sig_ignore);
    signal(SIGALRM, sig_ignore);
    signal(SIGHUP, sig_ignore);

#ifdef INCLUDE_BREAKPAD
    /* breakpad handles the signals SIGSEGV, SIGBUS, SIGFPE, and SIGILL */
    breakpad_ExceptionHandler();
#endif
}
