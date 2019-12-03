/*
 * OwlModemNetwork.h
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
 * \file OwlModemNetwork.h - API for Network Registration Management
 */

#ifndef __OWL_MODEM_NETWORK_RN4_H__
#define __OWL_MODEM_NETWORK_RN4_H__

#include "enums.h"

#include "OwlModemAT.h"

/**
 * Twilio wrapper for the AT serial interface to a modem - Methods to get and set u-blox MNO profile
 */
class OwlModemNetworkRN4 {
 public:
  OwlModemNetworkRN4(OwlModemAT *atModem);

  /**
   * Retrieve the current modem MNO profile selection.
   * @param out_profile - Modem MNO profile
   * @return 1 on success, 0 on failure
   */
  int getModemMNOProfile(at_umnoprof_mno_profile_e *out_profile);

  /**
   * Set the current modem MNO profile selection.
   * @param profile - Modem MNO profile to set
   * @return 1 on success, 0 on failure
   */
  int setModemMNOProfile(at_umnoprof_mno_profile_e profile);

 private:
  OwlModemAT *atModem_ = 0;

  str network_response = {.s = nullptr, .len = 0};
};

#endif
