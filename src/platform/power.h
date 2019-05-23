#ifndef __OWL_PLATFORM_POWER_H__
#define __OWL_PLATFORM_POWER_H__

#include <stdint.h>

#define OWL_POWER_RN4  1
#define OWL_POWER_BG96 2
#define OWL_POWER_GNSS 4

#ifdef __cplusplus
extern "C" {
#endif

void owlPowerOn(uint32_t power_bitmask);
void owlPowerOff(uint32_t power_bitmask);

#ifdef __cplusplus
}
#endif

#endif // __OWL_PLATFORM_POWER_H__
