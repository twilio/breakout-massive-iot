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

static void creg_handler(creg_stat stat, uint16_t lac, uint32_t ci, creg_act act) {
  got_creg_urc = true;

  std::cerr << "Got CREG URC: " << (int)stat << std::endl;

  creg_registered = (stat == creg_stat::Registered_Home_Network || stat == creg_stat::Registered_Roaming);
}

static void cgreg_handler(cgreg_stat stat, uint16_t lac, uint32_t ci, cgreg_act act, uint8_t rac) {
  got_cgreg_urc = true;

  std::cerr << "Got CGREG URC: " << (int)stat << std::endl;

  creg_registered = (stat == cgreg_stat::Registered_Home_Network || stat == cgreg_stat::Registered_Roaming);
}

static void cereg_handler(cereg_stat stat, uint16_t lac, uint32_t ci, cereg_act act, cereg_cause_type cause_type,
                          uint32_t reject_cause) {
  got_cereg_urc = true;

  std::cerr << "Got CEREG URC: " << (int)stat << std::endl;

  creg_registered = (stat == cereg_stat::Registered_Home_Network || stat == cereg_stat::Registered_Roaming);
}

TEST_CASE("CFUN", "[Network]") {
  REQUIRE(rn4->network.setModemFunctionality(cfun_fun::Airplane_Mode, nullptr) == 1);
  cfun_power_mode power_mode = (cfun_power_mode)-1;
  REQUIRE(rn4->network.getModemFunctionality(&power_mode) == 1);
  REQUIRE(power_mode == cfun_power_mode::Airplane_Mode);

  REQUIRE(rn4->network.setModemFunctionality(cfun_fun::Full_Functionality, nullptr) == 1);
  power_mode = (cfun_power_mode)-1;
  REQUIRE(rn4->network.getModemFunctionality(&power_mode) == 1);
  REQUIRE(power_mode == cfun_power_mode::Full_Functionality);
}

TEST_CASE("Registration", "[Network]") {
  // Disable URCs, only respond registration status when asked
  REQUIRE(rn4->network.setNetworkRegistrationURC(creg_n::URC_Disabled) == 1);
  REQUIRE(rn4->network.setGPRSRegistrationURC(cgreg_n::URC_Disabled) == 1);
  REQUIRE(rn4->network.setEPSRegistrationURC(cereg_n::URC_Disabled) == 1);

  // Set handlers
  rn4->network.setHandlerNetworkRegistrationURC(creg_handler);
  rn4->network.setHandlerGPRSRegistrationURC(cgreg_handler);
  rn4->network.setHandlerEPSRegistrationURC(cereg_handler);

  got_creg_urc  = false;
  got_cgreg_urc = false;
  got_cereg_urc = false;

  // TODO: check that it really eventually registers somewhere, and with appropriate RAT

  creg_n creg_urc_status;
  creg_stat creg_reg_status;
  creg_act creg_rat;
  uint16_t lac;
  uint32_t ci;
  REQUIRE(rn4->network.getNetworkRegistrationStatus(&creg_urc_status, &creg_reg_status, &lac, &ci, &creg_rat) == 1);
  REQUIRE(got_creg_urc == true);

  cgreg_n cgreg_urc_status;
  cgreg_stat cgreg_reg_status;
  cgreg_act cgreg_rat;
  uint8_t cgreg_rac;
  REQUIRE(rn4->network.getGPRSRegistrationStatus(&cgreg_urc_status, &cgreg_reg_status, &lac, &ci, &cgreg_rat,
                                                 &cgreg_rac) == 1);
  REQUIRE(got_cgreg_urc == true);

  cereg_n cereg_urc_status;
  cereg_stat cereg_reg_status;
  cereg_act cereg_rat;
  cereg_cause_type cereg_cause_type;
  uint32_t cereg_reject_cause;
  REQUIRE(rn4->network.getEPSRegistrationStatus(&cereg_urc_status, &cereg_reg_status, &lac, &ci, &cereg_rat,
                                                &cereg_cause_type, &cereg_reject_cause) == 1);
  REQUIRE(got_cereg_urc == true);
}

TEST_CASE("CSQ", "[Network]") {
  csq_rssi rssi;
  csq_qual qual;
  REQUIRE(rn4->network.getSignalQuality(&rssi, &qual) == 1);

  // Minimum requirements: some quality is reported
  REQUIRE(rssi != csq_rssi::Not_Known_or_Detectable_199);

  // For SARA-R4/SARA-N4 CSQ is not supported
  // TODO: conditional check once BG96 is tested as well
  REQUIRE(qual == csq_qual::Not_Known_or_Not_Detectable);
}

#endif  // ARDUINO
