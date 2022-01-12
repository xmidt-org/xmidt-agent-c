/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __SIGNALS_H__
#define __SIGNALS_H__


enum signals_command {
    SIG_EVENT__STOP,
    SIG_EVENT__TERMINATE,
    SIG_EVENT__SYSTEM_IS_RESTARTING
};

/**
 *  config_signals() configures the signals to call the provided handler with
 *  the information about what the system is telling us to do.
 */
void signals_config(void (*command)(enum signals_command));

#endif
