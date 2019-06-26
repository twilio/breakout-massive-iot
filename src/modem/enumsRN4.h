/*
 * enumsRN4.h
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
 * \file enumsRN4.h - various enumerations for AT commands and parameters
 */

#ifndef __OWL_MODEM_ENUMSRN4_H__
#define __OWL_MODEM_ENUMSRN4_H__

#include "enums.h"


typedef enum {
  USECPRF_OP_CODE_Certificate_Validation_Level           = 0,
  USECPRF_OP_CODE_TLS_Version                            = 1,
  USECPRF_OP_CODE_Cipher_Suite                           = 2,
  USECPRF_OP_CODE_Trusted_Root_Certificate_Internal_Name = 3,
  USECPRF_OP_CODE_Expected_Server_Hostname               = 4,
  USECPRF_OP_CODE_Client_Certificate_Internal_Name       = 5,
  USECPRF_OP_CODE_Client_Private_Key_Internal_Name       = 6,
  USECPRF_OP_CODE_Client_Private_Key_Password            = 7,
  USECPRF_OP_CODE_Pre_Shared_Key                         = 8,
  USECPRF_OP_CODE_Pre_Shared_Key_Name                    = 9,
  USECPRF_OP_CODE_SNI                                    = 10,
  USECPRF_OP_CODE_RFU                                    = 11,
  USECPRF_OP_CODE_Server_Certificate_Pining              = 12
} usecprf_op_code_e;

typedef enum {
  USECPREF_CIPHER_SUITE_Automatic                           = 0,   // Not supported for SARA-N410-02B
  USECPREF_CIPHER_SUITE_TLS_RSA_WITH_AES_128_CBC_SHA        = 1,   // IANA: 0x002F
  USECPREF_CIPHER_SUITE_TLS_RSA_WITH_AES_128_CBC_SHA256     = 2,   // IANA: 0x003C
  USECPREF_CIPHER_SUITE_TLS_RSA_WITH_AES_256_CBC_SHA        = 3,   // IANA: 0x0035
  USECPREF_CIPHER_SUITE_TLS_RSA_WITH_AES_256_CBC_SHA256     = 4,   // IANA: 0x003D
  USECPREF_CIPHER_SUITE_TLS_RSA_WITH_3DES_EDE_CBC_SHA       = 5,   // IANA: 0x000A
  USECPREF_CIPHER_SUITE_TLS_PSK_WITH_AES_128_CBC_SHA        = 6,   // IANA: 0x008C
  USECPREF_CIPHER_SUITE_TLS_PSK_WITH_AES_256_CBC_SHA        = 7,   // IANA: 0x008D
  USECPREF_CIPHER_SUITE_TLS_PSK_WITH_3DES_EDE_CBC_SHA       = 8,   // IANA: 0x008B
  USECPREF_CIPHER_SUITE_TLS_RSA_PSK_WITH_AES_128_CBC_SHA    = 9,   // IANA: 0x0094
  USECPREF_CIPHER_SUITE_TLS_RSA_PSK_WITH_AES_256_CBC_SHA    = 10,  // IANA: 0x0095
  USECPREF_CIPHER_SUITE_TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA   = 11,  // IANA: 0x0093
  USECPREF_CIPHER_SUITE_TLS_PSK_WITH_AES_128_CBC_SHA256     = 12,  // IANA: 0x00AE
  USECPREF_CIPHER_SUITE_TLS_PSK_WITH_AES_256_CBC_SHA384     = 13,  // IANA: 0x00AF
  USECPREF_CIPHER_SUITE_TLS_RSA_PSK_WITH_AES_128_CBC_SHA256 = 14,  // IANA: 0x00B6
  USECPREF_CIPHER_SUITE_TLS_RSA_PSK_WITH_AES_256_CBC_SHA384 = 15,  // IANA: 0x00B7
  USECPREF_CIPHER_SUITE_IANA_ENUM_VALUE                     = 99   // Not supported for SARA-N410-02B
} usecprf_cipher_suite_e;

typedef enum {
  USECPRF_VALIDATION_LEVEL_No_Validation                             = 0,
  USECPRF_VALIDATION_LEVEL_Root_Certificate_Without_URL_Check        = 1,
  USECPRF_VALIDATION_LEVEL_Root_Certificate_With_URL_Check           = 2,
  USECPRF_VALIDATION_LEVEL_Root_Certificate_With_Validity_Date_Check = 3
} usecprf_validation_level_e;

typedef enum {
  USECMNG_OPERATION_Import_From_Serial = 0,
  USECMNG_OPERATION_Import_From_File   = 1,
  USECMNG_OPERATION_Remove             = 2,
  USECMNG_OPERATION_List               = 3,
  USECMNG_OPERATION_Calculate_MD5      = 4
} usecmng_operation_e;

typedef enum {
  USECMNG_CERTIFICATE_TYPE_Trusted_Root_CA = 0,
  USECMNG_CERTIFICATE_TYPE_Certificate     = 1,
  USECMNG_CERTIFICATE_TYPE_Key             = 2
} usecmng_certificate_type_e;

#endif
