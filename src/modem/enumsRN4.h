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
  USECPRF_OP_CODE_Certificate_Validation_Level = 0,
  USECPRF_OP_CODE_TLS_Version = 1,
  USECPRF_OP_CODE_Cipher_Suite = 2,
  USECPRF_OP_CODE_Trusted_Root_Certificate_Internal_Name = 3,
  USECPRF_OP_CODE_Expected_Server_Hostname = 4,
  USECPRF_OP_CODE_Client_Certificate_Internal_Name = 5,
  USECPRF_OP_CODE_Client_Private_Key_Internal_Name = 6,
  USECPRF_OP_CODE_Client_Private_Key_Password = 7,
  USECPRF_OP_CODE_Pre_Shared_Key = 8,
  USECPRF_OP_CODE_Pre_Shared_Key_Name = 9,
  USECPRF_OP_CODE_SNI = 10,
  USECPRF_OP_CODE_RFU = 11,
  USECPRF_OP_CODE_Server_Certificate_Pining = 12
} usecprf_op_code_e;

typedef enum {
  USECPREF_CIPHER_SUITE_Automatic                           =  0, // Not supported for SARA-N410-02B
  USECPREF_CIPHER_SUITE_TLS_RSA_WITH_AES_128_CBC_SHA        =  1, // IANA: 0x002F
  USECPREF_CIPHER_SUITE_TLS_RSA_WITH_AES_128_CBC_SHA256     =  2, // IANA: 0x003C
  USECPREF_CIPHER_SUITE_TLS_RSA_WITH_AES_256_CBC_SHA        =  3, // IANA: 0x0035
  USECPREF_CIPHER_SUITE_TLS_RSA_WITH_AES_256_CBC_SHA256     =  4, // IANA: 0x003D
  USECPREF_CIPHER_SUITE_TLS_RSA_WITH_3DES_EDE_CBC_SHA       =  5, // IANA: 0x000A
  USECPREF_CIPHER_SUITE_TLS_PSK_WITH_AES_128_CBC_SHA        =  6, // IANA: 0x008C
  USECPREF_CIPHER_SUITE_TLS_PSK_WITH_AES_256_CBC_SHA        =  7, // IANA: 0x008D
  USECPREF_CIPHER_SUITE_TLS_PSK_WITH_3DES_EDE_CBC_SHA       =  8, // IANA: 0x008B
  USECPREF_CIPHER_SUITE_TLS_RSA_PSK_WITH_AES_128_CBC_SHA    =  9, // IANA: 0x0094
  USECPREF_CIPHER_SUITE_TLS_RSA_PSK_WITH_AES_256_CBC_SHA    = 10, // IANA: 0x0095
  USECPREF_CIPHER_SUITE_TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA   = 11, // IANA: 0x0093
  USECPREF_CIPHER_SUITE_TLS_PSK_WITH_AES_128_CBC_SHA256     = 12, // IANA: 0x00AE
  USECPREF_CIPHER_SUITE_TLS_PSK_WITH_AES_256_CBC_SHA384     = 13, // IANA: 0x00AF
  USECPREF_CIPHER_SUITE_TLS_RSA_PSK_WITH_AES_128_CBC_SHA256 = 14, // IANA: 0x00B6
  USECPREF_CIPHER_SUITE_TLS_RSA_PSK_WITH_AES_256_CBC_SHA384 = 15, // IANA: 0x00B7
  USECPREF_CIPHER_SUITE_IANA_ENUM_VALUE                     = 99  // Not supported for SARA-N410-02B
} usecprf_cipher_suite_e;

typedef enum {
  USECPRF_VALIDATION_LEVEL_No_Validation = 0,
  USECPRF_VALIDATION_LEVEL_Root_Certificate_Without_URL_Check = 1,
  USECPRF_VALIDATION_LEVEL_Root_Certificate_With_URL_Check = 2,
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
  USECMNG_CERTIFICATE_TYPE_Certificate = 1,
  USECMNG_CERTIFICATE_TYPE_Key = 2
} usecmng_certificate_type_e;

typedef enum {
  AT_MQTT__Config_Op_Code__Unique_Client_ID      = 0,  /**< Unique client ID */
  AT_MQTT__Config_Op_Code__Port_Number           = 1,  /**< Local port number */
  AT_MQTT__Config_Op_Code__Server_Name           = 2,  /**< Server name */
  AT_MQTT__Config_Op_Code__IP_Address            = 3,  /**< Server IP address */
  AT_MQTT__Config_Op_Code__Username_And_Password = 4,  /**< Username and Password */
  AT_MQTT__Config_Op_Code__Will_QoS              = 6,  /**< Will QoS (Set not supported) */
  AT_MQTT__Config_Op_Code__Will_Retain           = 7,  /**< Will Retain (Set not supported) */
  AT_MQTT__Config_Op_Code__Will_Topic            = 8,  /**< Will Topic (Set not supported) */
  AT_MQTT__Config_Op_Code__Will_Message          = 9,  /**< Will Message (Set not supported) */
  AT_MQTT__Config_Op_Code__Inactivity_Timeout    = 10, /**< Inactivity timeout period */
  AT_MQTT__Config_Op_Code__Secure                = 11, /**< Secure */
  AT_MQTT__Config_Op_Code__Clean_Session         = 12, /**< Clean session */
  AT_MQTT__Config_Op_Code__Message_Format        = 14  /**< (Set not supported) */
} at_mqtt_config_op_code_e;

char *at_mqtt_config_op_code_text(at_mqtt_config_op_code_e code);

typedef enum {
  AT_MQTT__Secure__No_TLS = 0, /**< Do not use encryption */
  AT_MQTT__Secure__TLS    = 1, /**< Use MQTT TLS encryption */
} at_mqtt_secure_e;

char *at_mqtt_secure_text(at_mqtt_secure_e code);

typedef enum {
  AT_MQTT__Clean_Session__Persist = 0, /**< Persist subscriptions and message delivery between sessions */
  AT_MQTT__Clean_Session__Clean = 1,   /**< Explicit disconnection cleans all session state information */
} at_mqtt_clean_session_e;

char *at_mqtt_clean_session_text(at_mqtt_clean_session_e code);

typedef enum {
  AT_MQTT__QoS__At_Most_Once  = 0, /**< At most once delivery (default) */
  AT_MQTT__QoS__At_Least_Once = 1, /**< At least once delivery */
  AT_MQTT__QoS__Exactly_Once  = 2, /**< Exactly once delivery */
} at_mqtt_qos_e;

char *at_mqtt_qos_text(at_mqtt_qos_e code);

typedef enum {
  AT_MQTT__NVM_Mode__Restore_To_Default = 0, /**< Restore MQTT client profile parameters to factory defaults */
  AT_MQTT__NVM_Mode__Load_From_NVM      = 1, /**< Set MQTT client profile parameters to values stored previously in NVM */
  AT_MQTT__NVM_Mode__Save_To_NVM        = 2, /**< Store current MQTT client profile parameters to NVM */
} at_mqtt_nvm_mode_e;

char *at_mqtt_nvm_mode_text(at_mqtt_nvm_mode_e code);

typedef enum {
  AT_MQTT__Client_Op_Code__Disconnect           = 0, /**< Disconnect / Log Out from server */
  AT_MQTT__Client_Op_Code__Connect              = 1, /**< Connect / Log In to server */
  AT_MQTT__Client_Op_Code__Publish_Message      = 2, /**< Publish a message to a specific topic to the MQTT message broker */
  AT_MQTT__Client_Op_Code__Publish_File         = 3, /**< Publish a message from a file to a specific topic to the MQTT message broker */
  AT_MQTT__Client_Op_Code__Subscribe_Topic      = 4, /**< Subscribe to a topic from the MQTT message broker */
  AT_MQTT__Client_Op_Code__Unsubscribe_Topic    = 5, /**< Unsubscribe to a topic from the MQTT message broker. This should exactly match the Topic Filter used during the Subscribe */
  AT_MQTT__Client_Op_Code__Read_Unread_Messages = 6, /**< Read all unread messages received from MQTT message broker at current verbosity level */
  AT_MQTT__Client_Op_Code__Set_Message_Format   = 7, /**< Sets the terse/verbose format for received messages (i.e. the amount of information and headers with each received MQTT message) */
  AT_MQTT__Client_Op_Code__Ping_MQTT_Server     = 8  /**< Ping the MQTT message broker */
} at_mqtt_client_op_code_e;

char *at_mqtt_client_op_code_text(at_mqtt_client_op_code_e code);

typedef enum {
  AT_MQTT__Retain__No_Retain = 0, /**< The message will not persist across disconnects of MQTT client */
  AT_MQTT__Retain__Retain    = 1, /**< The message will be retained by the MQTT server across disconnects */
} at_mqtt_retain_e;

char *at_mqtt_retain_text(at_mqtt_retain_e code);

typedef enum {
  AT_MQTT__Mode__ASCII = 0, /**< ASCII message mode */
  AT_MQTT__Mode__HEX   = 1, /**< Hexidecimal message mode */
} at_mqtt_mode_e;

char *at_mqtt_mode_text(at_mqtt_mode_e code);

typedef enum {
  AT_MQTT__Format__No_Formatting       = 0, /**< All messages will be concatenated into single line */
  AT_MQTT__Format__Default_Formatting  = 1, /**< Each message will contain the <topic_name> and <message_contents> */
  AT_MQTT__Format__Extended_Formatting = 2, /**< Each message will contain the <topic_name>, <msg_length>, <QoS> and <message_contents> */
} at_mqtt_format_e;

char *at_mqtt_format_text(at_mqtt_format_e code);

typedef enum {
  AT_MQTT__Subscribe_Result__Success = 0,   /**< Subscribe was successful */
  AT_MQTT__Subscribe_Result__Failure = 128, /**< Subscribe failed */
} at_mqtt_subscribe_result_e;

char *at_mqtt_subscribe_result_text(at_mqtt_subscribe_result_e code);

typedef enum {
  AT_MQTT__Login_Result__Accepted               = 0, /**< Connection accepted */
  AT_MQTT__Login_Result__Protocol_Unsupported   = 1, /**< Server does not support level of the MQTT protocol requested */
  AT_MQTT__Login_Result__Identifier_Unsupported = 2, /**< Client identifier is correct UTF-8 but not allowed by server */
  AT_MQTT__Login_Result__MQTT_Unavailable       = 3, /**< Connection has been made but MQTT is unavailable */
  AT_MQTT__Login_Result__Malformed_Credentials  = 4, /**< Data in the username or password is malformed */
  AT_MQTT__Login_Result__Not_Authorized         = 5, /**< Client is not authorized to connect */
} at_mqtt_login_result_e;

char *at_mqtt_login_result_text(at_mqtt_login_result_e code);

#endif
