#ifndef ARDUINO  // arduino tries to compile everything in src directory, but this is not intended for the target

#include "platform/log.h"

#include <stdio.h>
#include <stdarg.h>

static log_level_t log_level = L_ISSUE;

void owl_log_set_level(log_level_t level) {
  log_level = level;
}

int owl_log_is_printable(log_level_t level) {
  return level <= log_level;
}

void owl_log(log_level_t loglevel, const char *format, ...) {
  if (!owl_log_is_printable(loglevel)) {
    return;
  }

  va_list ap;
  va_start(ap, format);
  vfprintf(stderr, format, ap);
}

void owl_log_str(log_level_t ll, str data) {
  owl_log(ll, "%.*s\r\n", data.len, data.s);
}

#endif  // ARDUINO
