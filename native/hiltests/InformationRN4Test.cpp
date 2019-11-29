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

#include "catch.hpp"

#include <iostream>
#include <string>
#include "modem/OwlModemRN4.h"

extern OwlModemRN4* rn4;

TEST_CASE("CGMI", "[information]") {
  str response;
  REQUIRE(rn4->information.getManufacturer(&response) == 1);

  auto response_string = std::string(response.s, response.len);

  std::cerr << "Got response from CFMI: " << response_string << std::endl;

  REQUIRE(response_string == "u-blox");
}

TEST_CASE("CGMM", "[information]") {
  str response;
  REQUIRE(rn4->information.getModel(&response) == 1);

  auto response_string = std::string(response.s, response.len);

  std::cerr << "Got response from CGMM: " << response_string << std::endl;
}

TEST_CASE("CGMR", "[information]") {
  str response;
  REQUIRE(rn4->information.getVersion(&response) == 1);

  auto response_string = std::string(response.s, response.len);

  std::cerr << "Got response from CGMR: " << response_string << std::endl;
}

TEST_CASE("CGSN", "[information]") {
  str response;
  REQUIRE(rn4->information.getIMEI(&response) == 1);

  auto response_string = std::string(response.s, response.len);

  std::cerr << "Got response from CGSN: " << response_string << std::endl;

  // This is a tricky way to write 'response.len == 15 || response.len == 17'
  //   Catch doesn't support complex conditions
  REQUIRE(response.len >= 15);
  REQUIRE(response.len <= 17);
  REQUIRE(response.len != 16);
}
