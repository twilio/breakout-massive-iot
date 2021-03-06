/*
 * OwlModemInformation.cpp
 * Twilio Breakout SDK
 *
 * Copyright (c) 2018 Twilio, Inc.
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

/**
 * \file OwlModemInformation.cpp - API for retrieving various data from the modem
 */

#include "OwlModemInformation.h"

OwlModemInformation::OwlModemInformation(OwlModemAT *atModem) : atModem_(atModem) {
}

int OwlModemInformation::getManufacturer(str *out_response) {
  auto result = atModem_->doCommandBlocking("AT+CGMI", 1000, out_response);
  str_strip(out_response);
  return (result == at_result_code::OK);
}

int OwlModemInformation::getModel(str *out_response) {
  auto result = atModem_->doCommandBlocking("AT+CGMM", 1000, out_response);
  str_strip(out_response);
  return (result == at_result_code::OK);
}

int OwlModemInformation::getVersion(str *out_response) {
  auto result = atModem_->doCommandBlocking("AT+CGMR", 1000, out_response);
  str_strip(out_response);
  return (result == at_result_code::OK);
}

int OwlModemInformation::getIMEI(str *out_response) {
  auto result = atModem_->doCommandBlocking("AT+CGSN", 1000, out_response);
  str_strip(out_response);
  return (result == at_result_code::OK);
}
