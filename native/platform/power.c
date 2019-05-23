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
