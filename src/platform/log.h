/*
 * log.h
 * Twilio Breakout SDK
 *
 * Copyright (c) 2019 Twilio, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file log.h - declarations of platform-specific logging functions
 */

#ifndef __OWL_PLATFORM_LOG_H__
#define __OWL_PLATFORM_LOG_H__

#include <stdint.h>

#include "../utils/str.h"
#include "../utils/bin_t.h"

/*
 * Log levels
 */
#define L_CLI -4
#define L_ALERT -3
#define L_CRIT -2
#define L_ERR -1
#define L_ISSUE 0
#define L_WARN 1
#define L_NOTICE 2
#define L_INFO 3
#define L_DB 4
#define L_DBG 5
#define L_MEM 6

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t log_level_t;

/**
 * Functions declared below are to be implemented in ports
 */

/**
 * Retrieve the current log level
 * @return the log level
 */
log_level_t owl_log_get_level();

/**
 * Set the current debug level to a certain value - log lower or equal to this will be printed, others ignored
 * @param level - the level to set
 */
void owl_log_set_level(log_level_t level);

/**
 * Check if a level will produce an output at the current debug level
 * @return 1 if it will be printed, 0 if not
 */
int owl_log_is_printable(log_level_t level);

/**
 * Log something out. Use the LOG() macros instead, to also get the function and line information from the code. If you
 * want your log lines to align to the left (e.g. printing a table), use the LOGF() macro, which is a direct call to
 * this function.
 * @param level - level to output on
 * @param format - printf format
 * @param ... - parameters for the printf format
 */
void owl_log(log_level_t, const char *format, ...);

/**
 * Log something out, without the time/level/etc prefix. Use the LOGE() macros instead, to also get the function and
 * line information from the code.
 * @param level - level to output on
 * @param format - printf format
 * @param ... - parameters for the printf format
 */
void owl_log_empty(log_level_t level, const char *format, ...);

/**
 * Log a binary str in a nice binary dump format. Use the LOGSTR() macro instead, to also get the function and
 * line information from the code.
 * @param level - level to output on
 * @param x - the str to log
 */
void owl_log_str(log_level_t level, str x);

/**
 * Log a binary bin_t in a nice binary dump format. Use the LOGSTR() macro instead, to also get the function and
 * line information from the code.
 * @param level - level to output on
 * @param x - the bin_t to log
 */
void owl_log_bin_t(log_level_t level, bin_t x);


#ifdef __cplusplus
}
#endif



#endif
