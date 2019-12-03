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
 * \file log.h - logging utilities
 */

#ifndef __OWL_UTILS_LOG_H__
#define __OWL_UTILS_LOG_H__

#include <limits.h>
#include "../platform/log.h"

/*
 * Parameters for logging - change here to disable logging or colors
 */

#ifndef LOG_DISABLED
#define LOG_DISABLED 0
#endif

/*
 * Log levels
 */
#define L_OFF INT_MIN
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

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG(level, format, ...) owl_log(level, "%s:%d:%s() " format, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)
#define LOGE(level, format, ...) owl_log_empty(level, format, ##__VA_ARGS__)
#define LOGF(level, format, ...) owl_log(level, format, ##__VA_ARGS__)
#define LOGSTR(level, x) owl_log_str(level, x)

#else

#define LOG(level, format, args...)
#define LOGE(level, format, args...)
#define LOGF(level, format, args...)
#define LOGSTR(level, x)
#define LOGBIN(level, x)

#endif

#endif  // __OWL_UTILS_LOG_H__
