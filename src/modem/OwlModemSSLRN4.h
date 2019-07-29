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

  /**
   * Populate a device certificate to the TLS system on a SARA-R410/SARA-N410 modem.
   *
   * The module supports loading either a DER or PEM formatted certificate but stores
   * in DER format.  To assist the library in only writing to the module's NVM when
   * needed, please provide a DER encoded input so the MD5 hash can be compared.
   *
   * The data must be presented as a hex encoded str object, for example,
   * "48656C6C6F20576F726C64" (equivalent to "Hello World").  One way to accomplish this
   * in a unix shell for a DER encoded item is:
   *
   *   cat ca.der | xxd -p -u -c 10000
   *
   * For a pem encoded item:
   *
   *   cat ca.pem | openssl x509 -outform der | xxd -p -u -c 10000
   *
   * @param cert - Certificate in DER or PEM form
   * @return success status
   */
  bool setDeviceCert(str cert);

  /**
   * Populate a device private key to the tls system on a sara-r410/sara-n410 modem.
   *
   * the module supports loading either a der or pem formatted certificate but stores
   * in der format.  to assist the library in only writing to the module's nvm when
   * needed, please provide a der encoded input so the md5 hash can be compared.
   *
   * the data must be presented as a hex encoded str object, for example,
   * "48656c6c6f20576f726c64" (equivalent to "hello world").  one way to accomplish this
   * in a unix shell for a der encoded item is:
   *
   *   cat key.der | xxd -p -u -c 10000
   *
   * for a pem encoded item:
   *
   *   cat key.pem | openssl x509 -outform der | xxd -p -u -c 10000
   *
   * @param pkey - certificate in DER or PEM form
   * @return success status
   */
  bool setDevicePkey(str pkey);

  /**
   * Populate a trusted root CA to the tls system on a sara-r410/sara-n410 modem.
   *
   * the module supports loading either a der or pem formatted certificate but stores
   * in der format.  to assist the library in only writing to the module's nvm when
   * needed, please provide a der encoded input so the md5 hash can be compared.
   *
   * the data must be presented as a hex encoded str object, for example,
   * "48656c6c6f20576f726c64" (equivalent to "hello world").  one way to accomplish this
   * in a unix shell for a der encoded item is:
   *
   *   cat ca.der | xxd -p -u -c 10000
   *
   * for a pem encoded item:
   *
   *   cat ca.pem | openssl x509 -outform der | xxd -p -u -c 10000
   *
   * @param ca - certificate in DER or PEM form
   * @return success status
   */
  bool setServerCA(str ca);

  /**
   * Initialize TLS subsystem.
   *
   * @param ssl_profile_slot - profile slot to use - valid values are 0 - 4 (default: 0)
   * @param cipher_suite - cipher suite to use when negotiating the connection (default:
   * USECPREF_CIPHER_SUITE_TLS_RSA_WITH_AES_256_CBC_SHA256)
   * @return success status
   */
  bool initContext(uint8_t ssl_profile_slot            = 0,
                   usecprf_cipher_suite_e cipher_suite = USECPREF_CIPHER_SUITE_TLS_RSA_WITH_AES_256_CBC_SHA256);

 private:
  OwlModemAT* atModem_;
  str ssl_response = {.s = nullptr, .len = 0};
  bool shouldWriteCertificate(usecmng_certificate_type_e type, str name, str new_value);
  int calculateMD5ForCert(char* output, int max_len, str input);
  bool isPEM(str input);
};

#endif  // __OWL_MODEM_SSL_RN4_H__
