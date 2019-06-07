#ifndef ARDUINO  // arduino tries to compile everything in src directory, but this is not intended for the target

#include "platform/time.h"
#include <time.h>
#include <unistd.h>

owl_time_t owl_time() {
  struct timespec spec;
  clock_gettime(CLOCK_REALTIME, &spec);

  return (spec.tv_sec * 1000) + (spec.tv_nsec / 1000000);
}

void owl_delay(uint32_t ms) {
  usleep(ms * 1000);
}

#endif  // ARDUINO
