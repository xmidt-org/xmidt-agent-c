/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>

/* Until otelc is able to adopt a better mechanism, this will do. */

void log_trace(const char *format, ...);
void log_debug(const char *format, ...);
void log_info(const char *format, ...);
void log_warn(const char *format, ...);
void log_error(const char *format, ...);
void log_fatal(const char *format, ...);

void log_va_trace(const char *format, va_list args);
void log_va_debug(const char *format, va_list args);
void log_va_info(const char *format, va_list args);
void log_va_warn(const char *format, va_list args);
void log_va_error(const char *format, va_list args);
void log_va_fatal(const char *format, va_list args);

#endif
