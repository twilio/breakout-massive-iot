/*
 * OwlModemSSLRN4.h
 * Twilio Breakout SDK
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

/**
 * \file OwlModemSSLRN4.h - API for SSL support in Quectel RN4 modems
 */

#ifndef __OWL_MODEM_SSL_RN4_H__
#define __OWL_MODEM_SSL_RN4_H__

#include "enums.h"
#include "enumsRN4.h"

#include "OwlModemAT.h"

class OwlModemSSLRN4 {
 public:
  OwlModemSSLRN4(OwlModemAT* atModem);

  bool setDeviceCert(str cert, bool force = false);
  bool setDevicePkey(str pkey, bool force = false);
  bool setServerCA(str ca, bool force = false);

  bool initContext(uint8_t ssl_context_slot            = 0,
                   usecprf_cipher_suite_e cipher_suite = USECPREF_CIPHER_SUITE_TLS_RSA_WITH_AES_256_CBC_SHA256);

 private:
  OwlModemAT* atModem_;
  str ssl_response = {.s = nullptr, .len = 0};
};

#endif  // __OWL_MODEM_SSL_RN4_H__
