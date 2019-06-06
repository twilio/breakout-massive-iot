/*
 * enumsRN4.cpp
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
 * \file enums.cpp - various enumerations for AT commands and parameters
 */

#include "enumsRN4.h"


struct {
  char *value;
  at_mqtt_config_op_code_e code;
} at_mqtt_config_op_codes[] = {
    {.value = "Unique Client ID", .code = AT_MQTT__Config_Op_Code__Unique_Client_ID},
    {.value = "Port Number", .code = AT_MQTT__Config_Op_Code__Port_Number},
    {.value = "Server Name", .code = AT_MQTT__Config_Op_Code__Server_Name},
    {.value = "IP Address", .code = AT_MQTT__Config_Op_Code__IP_Address},
    {.value = "Username and Password", .code = AT_MQTT__Config_Op_Code__Username_And_Password},
    {.value = "Will QoS", .code = AT_MQTT__Config_Op_Code__Will_QoS},
    {.value = "Will Retain", .code = AT_MQTT__Config_Op_Code__Will_Retain},
    {.value = "Will Topic", .code = AT_MQTT__Config_Op_Code__Will_Topic},
    {.value = "Will Message", .code = AT_MQTT__Config_Op_Code__Will_Message},
    {.value = "Inactivity Timeout", .code = AT_MQTT__Config_Op_Code__Inactivity_Timeout},
    {.value = "Secure", .code = AT_MQTT__Config_Op_Code__Secure},
    {.value = "Clean Session", .code = AT_MQTT__Config_Op_Code__Clean_Session},
    {.value = "Message Format", .code = AT_MQTT__Config_Op_Code__Message_Format},
    {.value = 0, .code = (at_mqtt_config_op_code_e)-1},
};

char *at_mqtt_config_op_code_text(at_mqtt_config_op_code_e code) {
  int i;
  for (i = 0; at_mqtt_config_op_codes[i].value != 0; i++)
    if (at_mqtt_config_op_codes[i].code == code) return at_mqtt_config_op_codes[i].value;
  return "<unknown-mqtt-config-op-code>";
}

struct {
  char *value;
  at_mqtt_secure_e code;
} at_mqtt_secures[] = {
    {.value = "No TLS", .code = AT_MQTT__Secure__No_TLS},
    {.value = "TLS", .code = AT_MQTT__Secure__TLS},
    {.value = 0, .code = (at_mqtt_secure_e)-1},
};

char *at_mqtt_secure_text(at_mqtt_secure_e code) {
  int i;
  for (i = 0; at_mqtt_secures[i].value != 0; i++)
    if (at_mqtt_secures[i].code == code) return at_mqtt_secures[i].value;
  return "<unknown-mqtt-secure>";
}

struct {
  char *value;
  at_mqtt_clean_session_e code;
} at_mqtt_clean_sessions[] = {
    {.value = "Persist Session", .code = AT_MQTT__Clean_Session__Persist},
    {.value = "Clean Session", .code = AT_MQTT__Clean_Session__Clean},
    {.value = 0, .code = (at_mqtt_clean_session_e)-1},
};

char *at_mqtt_clean_session_text(at_mqtt_clean_session_e code) {
  int i;
  for (i = 0; at_mqtt_clean_sessions[i].value != 0; i++)
    if (at_mqtt_clean_sessions[i].code == code) return at_mqtt_clean_sessions[i].value;
  return "<unknown-mqtt-clean-session>";
}

struct {
  char *value;
  at_mqtt_qos_e code;
} at_mqtt_qoses[] = {
    {.value = "At Most Once", .code = AT_MQTT__QoS__At_Most_Once},
    {.value = "At Least Once", .code = AT_MQTT__QoS__At_Least_Once},
    {.value = "Exactly Once", .code = AT_MQTT__QoS__Exactly_Once},
    {.value = 0, .code = (at_mqtt_qos_e)-1},
};

char *at_mqtt_qos_text(at_mqtt_qos_e code) {
  int i;
  for (i = 0; at_mqtt_qoses[i].value != 0; i++)
    if (at_mqtt_qoses[i].code == code) return at_mqtt_qoses[i].value;
  return "<unknown-mqtt-qos>";
}

struct {
  char *value;
  at_mqtt_nvm_mode_e code;
} at_mqtt_nvm_modes[] = {
    {.value = "Restore to Default", .code = AT_MQTT__NVM_Mode__Restore_To_Default},
    {.value = "Load from NVM", .code = AT_MQTT__NVM_Mode__Load_From_NVM},
    {.value = "Save to NVM", .code = AT_MQTT__NVM_Mode__Save_To_NVM},
    {.value = 0, .code = (at_mqtt_nvm_mode_e)-1},
};

char *at_mqtt_nvm_mode_text(at_mqtt_nvm_mode_e code) {
  int i;
  for (i = 0; at_mqtt_nvm_modes[i].value != 0; i++)
    if (at_mqtt_nvm_modes[i].code == code) return at_mqtt_nvm_modes[i].value;
  return "<unknown-mqtt-nvm-mode>";
}

struct {
  char *value;
  at_mqtt_client_op_code_e code;
} at_mqtt_client_op_codes[] = {
    {.value = "Disconnect / Log Out", .code = AT_MQTT__Client_Op_Code__Disconnect},
    {.value = "Connect", .code = AT_MQTT__Client_Op_Code__Connect},
    {.value = "Publish Message", .code = AT_MQTT__Client_Op_Code__Publish_Message},
    {.value = "Publish File", .code = AT_MQTT__Client_Op_Code__Publish_File},
    {.value = "Subscribe Topic", .code = AT_MQTT__Client_Op_Code__Subscribe_Topic},
    {.value = "Unsubscribe Topic", .code = AT_MQTT__Client_Op_Code__Unsubscribe_Topic},
    {.value = "Read Unread Messages", .code = AT_MQTT__Client_Op_Code__Read_Unread_Messages},
    {.value = "Set Verbosity", .code = AT_MQTT__Client_Op_Code__Set_Message_Format},
    {.value = "Ping MQTT Server", .code = AT_MQTT__Client_Op_Code__Ping_MQTT_Server},
    {.value = 0, .code = (at_mqtt_client_op_code_e)-1},
};

char *at_mqtt_client_op_code_text(at_mqtt_client_op_code_e code) {
  int i;
  for (i = 0; at_mqtt_client_op_codes[i].value != 0; i++)
    if (at_mqtt_client_op_codes[i].code == code) return at_mqtt_client_op_codes[i].value;
  return "<unknown-mqtt-client-op-code>";
}

struct {
  char *value;
  at_mqtt_retain_e code;
} at_mqtt_retains[] = {
    {.value = "No Retain", .code = AT_MQTT__Retain__No_Retain},
    {.value = "Retain", .code = AT_MQTT__Retain__Retain},
    {.value = 0, .code = (at_mqtt_retain_e)-1},
};

char *at_mqtt_retain_text(at_mqtt_retain_e code) {
  int i;
  for (i = 0; at_mqtt_retains[i].value != 0; i++)
    if (at_mqtt_retains[i].code == code) return at_mqtt_retains[i].value;
  return "<unknown-mqtt-retain>";
}

struct {
  char *value;
  at_mqtt_mode_e code;
} at_mqtt_modes[] = {
    {.value = "ASCII", .code = AT_MQTT__Mode__ASCII},
    {.value = "Hexidecimal", .code = AT_MQTT__Mode__HEX},
    {.value = 0, .code = (at_mqtt_mode_e)-1},
};

char *at_mqtt_mode_text(at_mqtt_mode_e code) {
  int i;
  for (i = 0; at_mqtt_modes[i].value != 0; i++)
    if (at_mqtt_modes[i].code == code) return at_mqtt_modes[i].value;
  return "<unknown-mqtt-mode>";
}

struct {
  char *value;
  at_mqtt_format_e code;
} at_mqtt_formats[] = {
    {.value = "No Formatting", .code = AT_MQTT__Format__No_Formatting},
    {.value = "Default Formatting", .code = AT_MQTT__Format__Default_Formatting},
    {.value = "Extended Formatting", .code = AT_MQTT__Format__Extended_Formatting},
    {.value = 0, .code = (at_mqtt_format_e)-1},
};

char *at_mqtt_format_text(at_mqtt_format_e code) {
  int i;
  for (i = 0; at_mqtt_formats[i].value != 0; i++)
    if (at_mqtt_formats[i].code == code) return at_mqtt_formats[i].value;
  return "<unknown-mqtt-format>";
}

struct {
  char *value;
  at_mqtt_subscribe_result_e code;
} at_mqtt_subscribe_results[] = {
    {.value = "Success", .code = AT_MQTT__Subscribe_Result__Success},
    {.value = "Failure", .code = AT_MQTT__Subscribe_Result__Failure},
    {.value = 0, .code = (at_mqtt_subscribe_result_e)-1},
};

char *at_mqtt_subscribe_result_text(at_mqtt_subscribe_result_e code) {
  int i;
  for (i = 0; at_mqtt_subscribe_results[i].value != 0; i++)
    if (at_mqtt_subscribe_results[i].code == code) return at_mqtt_subscribe_results[i].value;
  return "<unknown-mqtt-subscribe-result>";
}

struct {
  char *value;
  at_mqtt_login_result_e code;
} at_mqtt_login_results[] = {
    {.value = "Accepted", .code = AT_MQTT__Login_Result__Accepted},
    {.value = "Protocol Unsupported", .code = AT_MQTT__Login_Result__Protocol_Unsupported},
    {.value = "Identifier Unsupported", .code = AT_MQTT__Login_Result__Identifier_Unsupported},
    {.value = "MQTT Unavailable", .code = AT_MQTT__Login_Result__MQTT_Unavailable},
    {.value = "Malformed Credentials", .code = AT_MQTT__Login_Result__Malformed_Credentials},
    {.value = "Not Authorized", .code = AT_MQTT__Login_Result__Not_Authorized},
    {.value = 0, .code = (at_mqtt_login_result_e)-1},
};

char *at_mqtt_login_result_text(at_mqtt_login_result_e code) {
  int i;
  for (i = 0; at_mqtt_login_results[i].value != 0; i++)
    if (at_mqtt_login_results[i].code == code) return at_mqtt_login_results[i].value;
  return "<unknown-mqtt-login-result>";
}

