
#define OWL_POWER_RN4  1
#define OWL_POWER_BG96 2
#define OWL_POWER_GNSS 4

void owlPowerOn(uint32_t power_bitmask);
void owlPowerOff(uint32_t power_bitmask);
