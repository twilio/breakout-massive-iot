/*
 * OwlModemCall.h
 * Twilio Breakout SDK
 *
 * Copyright (c) 2020 Twilio, Inc.
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
 * \file OwlModemCall.h - API for call-related commands (ATD)
 */

#ifndef __OWL_MODEM_CALL_H__
#define __OWL_MODEM_CALL_H__

#include "enums.h"
#include "OwlModemAT.h"

/**
 * Twilio wrapper for the AT serial interface to a modem - methods related to ATD commands
 */
class OwlModemCall {
 public:
  OwlModemCall(OwlModemAT *atModem);


  /**
   * Enter PPP mode
   * @return 1 on success, 0 on failure
   */
  int enterPPPMode();



 private:
  OwlModemAT *atModem_ = 0;
};

#endif
