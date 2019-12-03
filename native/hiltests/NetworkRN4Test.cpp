/*
 *
 * Twilio Massive SDK
 *
 * Copyright (c) 2019 Twilio, Inc.
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

#ifndef ARDUINO  // arduino tries to compile everything in src directory, but this is not intended for the target

#include "catch.hpp"

#include <iostream>
#include <string>
#include "modem/OwlModemRN4.h"

extern OwlModemRN4* rn4;

bool got_creg_urc     = false;
bool creg_registered  = false;
bool got_cgreg_urc    = false;
bool cgreg_registered = false;
bool got_cereg_urc    = false;
bool cereg_registered = false;

static void creg_handler(at_creg_stat_e stat, uint16_t lac, uint32_t ci, at_creg_act_e act) {
  got_creg_urc = true;

  std::cerr << "Got CREG URC: " << (int)stat << std::endl;

  creg_registered = (stat == AT_CREG__Stat__Registered_Home_Network || stat == AT_CREG__Stat__Registered_Roaming);
}

static void cgreg_handler(at_cgreg_stat_e stat, uint16_t lac, uint32_t ci, at_cgreg_act_e act, uint8_t rac) {
  got_cgreg_urc = true;

  std::cerr << "Got CGREG URC: " << (int)stat << std::endl;

  creg_registered = (stat == AT_CGREG__Stat__Registered_Home_Network || stat == AT_CGREG__Stat__Registered_Roaming);
}

static void cereg_handler(at_cereg_stat_e stat, uint16_t lac, uint32_t ci, at_cereg_act_e act,
                          at_cereg_cause_type_e cause_type, uint32_t reject_cause) {
  got_cereg_urc = true;

  std::cerr << "Got CEREG URC: " << (int)stat << std::endl;

  creg_registered = (stat == AT_CEREG__Stat__Registered_Home_Network || stat == AT_CEREG__Stat__Registered_Roaming);
}

TEST_CASE("CFUN", "[Network]") {
  REQUIRE(rn4->network.setModemFunctionality(AT_CFUN__FUN__Airplane_Mode, nullptr) == 1);
  at_cfun_power_mode_e power_mode = (at_cfun_power_mode_e)-1;
  REQUIRE(rn4->network.getModemFunctionality(&power_mode, nullptr) == 1);
  REQUIRE(power_mode == AT_CFUN__POWER_MODE__Airplane_Mode);

  REQUIRE(rn4->network.setModemFunctionality(AT_CFUN__FUN__Full_Functionality, nullptr) == 1);
  power_mode = (at_cfun_power_mode_e)-1;
  REQUIRE(rn4->network.getModemFunctionality(&power_mode, nullptr) == 1);
  REQUIRE(power_mode == AT_CFUN__POWER_MODE__Full_Functionality);
}

TEST_CASE("Registration", "[Network]") {
  // Disable URCs, only respond registration status when asked
  REQUIRE(rn4->network.setNetworkRegistrationURC(AT_CREG__N__URC_Disabled) == 1);
  REQUIRE(rn4->network.setGPRSRegistrationURC(AT_CGREG__N__URC_Disabled) == 1);
  REQUIRE(rn4->network.setEPSRegistrationURC(AT_CEREG__N__URC_Disabled) == 1);

  // Set handlers
  rn4->network.setHandlerNetworkRegistrationURC(creg_handler);
  rn4->network.setHandlerGPRSRegistrationURC(cgreg_handler);
  rn4->network.setHandlerEPSRegistrationURC(cereg_handler);

  got_creg_urc  = false;
  got_cgreg_urc = false;
  got_cereg_urc = false;

  // TODO: check that it really eventually registers somewhere, and with appropriate RAT

  at_creg_n_e creg_urc_status;
  at_creg_stat_e creg_reg_status;
  at_creg_act_e creg_rat;
  uint16_t lac;
  uint32_t ci;
  REQUIRE(rn4->network.getNetworkRegistrationStatus(&creg_urc_status, &creg_reg_status, &lac, &ci, &creg_rat) == 1);
  REQUIRE(got_creg_urc == true);

  at_cgreg_n_e cgreg_urc_status;
  at_cgreg_stat_e cgreg_reg_status;
  at_cgreg_act_e cgreg_rat;
  uint8_t cgreg_rac;
  REQUIRE(rn4->network.getGPRSRegistrationStatus(&cgreg_urc_status, &cgreg_reg_status, &lac, &ci, &cgreg_rat,
                                                 &cgreg_rac) == 1);
  REQUIRE(got_cgreg_urc == true);

  at_cereg_n_e cereg_urc_status;
  at_cereg_stat_e cereg_reg_status;
  at_cereg_act_e cereg_rat;
  at_cereg_cause_type_e cereg_cause_type;
  uint32_t cereg_reject_cause;
  REQUIRE(rn4->network.getEPSRegistrationStatus(&cereg_urc_status, &cereg_reg_status, &lac, &ci, &cereg_rat,
                                                &cereg_cause_type, &cereg_reject_cause) == 1);
  REQUIRE(got_cereg_urc == true);
}

TEST_CASE("CSQ", "[Network]") {
  at_csq_rssi_e rssi;
  at_csq_qual_e qual;
  REQUIRE(rn4->network.getSignalQuality(&rssi, &qual) == 1);

  // Minimum requirements: some quality is reported
  REQUIRE(rssi != AT_CSQ__RSSI__Not_Known_or_Detectable_199);

  // For SARA-R4/SARA-N4 CSQ is not supported
  // TODO: conditional check once BG96 is tested as well
  REQUIRE(qual == AT_CSQ__Qual__Not_Known_or_Not_Detectable);
}

#endif  // ARDUINO
