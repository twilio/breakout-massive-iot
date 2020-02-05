/*
 * OwlModemCall.cpp
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
 * \file OwlModemCall.cpp - API for call-related commands (ATD)
 */

#include "OwlModemCall.h"


OwlModemCall::OwlModemCall(OwlModemAT *atModem) : atModem_(atModem) {
}

int OwlModemCall::enterPPPMode() {
  auto result = atModem_->doCommandBlocking("ATD*99#", 3000, nullptr);
  return (result == AT_Result_Code__OK || result == AT_Result_Code__CONNECT);
}
