#ifndef __OWL_PLATFORM_POWER_H__
#define __OWL_PLATFORM_POWER_H__

#include <stdint.h>

#define OWL_POWER_RN4  1
#define OWL_POWER_BG96 2
#define OWL_POWER_GNSS 4

void owl_power_on(uint32_t power_bitmask);
void owl_power_off(uint32_t power_bitmask);

#endif // __OWL_PLATFORM_POWER_H__
