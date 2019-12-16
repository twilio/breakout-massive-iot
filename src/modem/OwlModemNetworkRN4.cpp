/*
 * OwlModemNetwork.cpp
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
 * \file OwlModemNetworkRN4.cpp - API for getting/setting u-blox MNO profile
 */

#include "OwlModemNetworkRN4.h"

#include <stdio.h>


OwlModemNetworkRN4::OwlModemNetworkRN4(OwlModemAT *atModem) : atModem_(atModem) {
}

static str s_umnoprof = STRDECL("+UMNOPROF: ");

int OwlModemNetworkRN4::getModemMNOProfile(umnoprof_mno_profile *out_profile) {
  if (out_profile) *out_profile = umnoprof_mno_profile::SW_Default;
  int result = atModem_->doCommandBlocking("AT+UMNOPROF?", 15 * 1000, &network_response) == at_result_code::OK;
  if (!result) return 0;
  OwlModemAT::filterResponse(s_umnoprof, network_response, &network_response);
  *out_profile = (umnoprof_mno_profile)str_to_long_int(network_response, 10);
  return 1;
}

int OwlModemNetworkRN4::setModemMNOProfile(umnoprof_mno_profile profile) {
  atModem_->commandSprintf("AT+UMNOPROF=%d", profile);
  return atModem_->doCommandBlocking(3 * 60 * 1000, nullptr) == at_result_code::OK;
}
