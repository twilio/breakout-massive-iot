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
#include "CharDeviceSerial.h"

bool got_cpin_urc = false;

extern OwlModemRN4* rn4;

static void cpin_handler(str urc, void* priv) {
  (void) priv;
  got_cpin_urc = true;

  auto urc_string = std::string(urc.s, urc.len);
  std::cerr << "Got CPIN URC: " << urc_string << std::endl;
}

TEST_CASE("CCID", "[SIM]") {
  str response;
  REQUIRE(rn4->SIM.getICCID(&response) == 1);

  auto response_string = std::string(response.s, response.len);

  std::cerr << "Got response from CCID: " << response_string << std::endl;

  // This is a tricky way to write 'response.len == 19 || response.len == 20'
  //   Catch doesn't support complex conditions
  REQUIRE(response.len >= 19);
  REQUIRE(response.len <= 20);
}

TEST_CASE("CIMI", "[SIM]") {
  str response;
  REQUIRE(rn4->SIM.getIMSI(&response) == 1);

  auto response_string = std::string(response.s, response.len);

  std::cerr << "Got response from CIMI: " << response_string << std::endl;

  REQUIRE(response.len >= 6);
  REQUIRE(response.len <= 15);
}

TEST_CASE("CPIN", "[SIM]") {
  rn4->SIM.setHandlerPIN(nullptr);
  REQUIRE(rn4->SIM.getPINStatus() == 1);

  got_cpin_urc = false;
  rn4->SIM.setHandlerPIN(cpin_handler);
  REQUIRE(rn4->SIM.getPINStatus() == 1);
  REQUIRE(got_cpin_urc == true);
}

#endif  // ARDUINO
