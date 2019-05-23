#include "platform/log.h"

#include <stdio.h>
#include <stdarg.h>

static log_level_t log_level = L_ISSUE;

void owl_log_set_level(log_level_t level) {
  log_level = level;
}

void owl_log(log_level_t loglevel, const char *format, ...) {
  if (loglevel > log_level) {
    return;
  }

  va_list ap;
  va_start(ap, format);
  vfprintf(stderr, format, ap);
}
