#ifndef ARDUINO  // arduino tries to compile everything in src directory, but this is not intended for the target

#include "platform/power.h"

void owlPowerOn(uint32_t bitmask) {
  (void)bitmask;

  // power on/off is not supported on native platform
  return;
}

void owlPowerOff(uint32_t bitmask) {
  (void)bitmask;

  // power on/off is not supported on native platform
  return;
}

#endif  // ARDUINO
