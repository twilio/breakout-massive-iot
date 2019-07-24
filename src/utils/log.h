/*
 * log.h
 * Twilio Breakout SDK
 *
 * Copyright (c) 2018 Twilio, Inc.
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
 * \file log.g - logging utilities
 */

#ifndef __OWL_UTILS_LOG_H__
#define __OWL_UTILS_LOG_H__

#include "../platform/log.h"

/*
 * Parameters for logging - change here to disable logging or colors
 */
#define LOG_DISABLED 0
#define LOG_NO_ANSI_COLORS 0
#define LOG_WITH_TIME 1
#define LOG_LINE_MAX_LEN 1024
#define LOG_OUTPUT SerialDebugPort


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


#if LOG_DISABLED == 0

#define IS_PRINTABLE(level) (level <= debug_level || level == L_CLI)

#define LOG(level, format, ...) owl_log(level, "%s():%d " format, __func__, __LINE__, ##__VA_ARGS__)
#define LOGE(level, format, ...) owl_log_empty(level, format, ##__VA_ARGS__)
#define LOGF(level, format, ...) owl_log(level, format, ##__VA_ARGS__)
#define LOGSTR(level, x) owl_log_str(level, x)

#else

#define IS_PRINTABLE(level) 0

#define LOG(level, format, args...)
#define LOGE(level, format, args...)
#define LOGF(level, format, args...)
#define LOGSTR(level, x)
#define LOGBIN(level, x)

#endif

#define GOTOERR(label)                                                                                                 \
  do {                                                                                                                 \
    LOG(L_ERR, "Going to label " #label "\r\n");                                                                       \
    goto label;                                                                                                        \
  } while (0)


#endif // __OWL_UTILS_LOG_H__
