/*
 * OwlModemMQTTRN4.cpp
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
 * \file OwlModemMQTTRN4.cpp - API for MQTT client on module
 */

#include "OwlModemMQTTRN4.h"

#include <stdio.h>

#include "OwlModemAT.h"

static char URC_ID[] = "MQTTRN4";

OwlModemMQTTRN4::OwlModemMQTTRN4(OwlModemAT *owlModem) : owlModem(owlModem) {
  owlModem->registerUrcHandler(URC_ID, OwlModemMQTTRN4::processURC, this);
}

static str s_uumqttc = STRDECL("+UUMQTTC");

int OwlModemMQTTRN4::processURCMQTTClient(str urc, str data) {
  if (!str_equal(urc, s_uumqttc)) return 0;
  str token = {0};
  at_mqtt_client_op_code_e op_code;

  at_mqtt_login_result_e login_result;

  at_mqtt_subscribe_result_e subscribe_result;
  at_mqtt_qos_e subscribe_qos;
  str subscribe_topic;

  uint16_t unread_message_count;
  
  // pop-off op_code which is parameter 0
  if (!str_tok(data, ",", &token)) {
    return 0;
  }
  op_code = (at_mqtt_client_op_code_e)str_to_uint32_t(token, 10);
  switch (op_code) {
    case AT_MQTT__Client_Op_Code__Connect:
      if (str_tok(data, ",", &token)) {
        login_result = (at_mqtt_login_result_e)str_to_uint32_t(token, 10);

        // TODO: handle this by passing it up to user
        LOG(L_NOTICE,
            "Received URC for mqtt client %u(%s) - result: %u(%s)\r\n",
            op_code, at_mqtt_client_op_code_text(op_code), login_result, at_mqtt_login_result_text(login_result));
      }
      break;
    case AT_MQTT__Client_Op_Code__Subscribe_Topic:
      for (int i = 1; str_tok(data, ",", &token); i++) {
        switch (i) {
          case 1:
            subscribe_result = (at_mqtt_subscribe_result_e)str_to_uint32_t(token, 10);
            break;
          case 2:
            subscribe_qos = (at_mqtt_qos_e)str_to_uint32_t(token, 10);
            break;
          case 3:
            subscribe_topic = token;
        }
      }

      // TODO: handle this by passing it up to user
      LOG(L_NOTICE,
          "Received URC for mqtt client %u(%s) - result: %u(%s); qos: %u(%s); topic: %.*s\r\n",
          op_code, at_mqtt_client_op_code_text(op_code), subscribe_result, at_mqtt_subscribe_result_text(subscribe_result), subscribe_qos, at_mqtt_qos_text(subscribe_qos), subscribe_topic.len, subscribe_topic.s);
      break;
    case AT_MQTT__Client_Op_Code__Read_Unread_Messages:
      if (str_tok(data, ",", &token)) {
        unread_message_count = str_to_uint32_t(token, 10);

        // TODO: handle this by passing it up to user
        LOG(L_NOTICE,
            "Received URC for mqtt client %u(%s) - count %u\r\n",
            op_code, at_mqtt_client_op_code_text(op_code), unread_message_count);
      }
      break;
    default:
      // the remaining op_codes are not used in URCs
      return 0;
      break;
  }
  return 1;
}

static str s_uumqttm = STRDECL("+UUMQTTCM");

int OwlModemMQTTRN4::processURCMQTTCM(str urc, str data) {
  // TODO: also consume:
  // 00:01:12.001 DBG  consumeUnsolicitedInCommandResponse():674 Line [Topic:/tests/devices]
  // 00:01:12.001 DBG  consumeUnsolicitedInCommandResponse():674 Line [Len:34 QoS:0]
  // 00:01:12.001 DBG  consumeUnsolicitedInCommandResponse():674 Line [Msg:hello Thu Jan  3 20:51:38 EST 2019]
  if (!str_equal(urc, s_uumqttm)) return 0;
  str token = {0};
  at_mqtt_client_op_code_e op_code;
  long int count;
  for (int i = 0; str_tok(data, ",", &token); i++) {
    switch (i) {
      case 0:
        op_code = (at_mqtt_client_op_code_e)str_to_uint32_t(token, 10);
        break;
      case 1:
        count = str_to_long_int(token, 10);
        break;
      default:
        break;
    }
  }

  // TODO: handle this by passing it up to user
  LOG(L_NOTICE,
      "Received URC for received messages with count %u\r\n",
      count);
  return 1;
}

int OwlModemMQTTRN4::processIncomingMessages(str urc, str data) {
  if (readingCommands) {
    LOG(L_NOTICE,
        "proessing incoming message [%.*s] [%.*s]\r\n",
        urc.len, urc.s, data.len, data.s);
  }
  return 0;
}

static str s_uumqtt = STRDECL("+UUMQTT");

int OwlModemMQTTRN4::processURCMQTTProfileConfiguration(str urc, str data) {
  if (!str_equal(urc, s_uumqtt)) return 0;
  str token = {0};
  at_mqtt_config_op_code_e op_code;
  str params = {0};
  for (int i = 0; str_tok(data, ":", &token); i++) {
    switch (i) {
      case 0:
        op_code = (at_mqtt_config_op_code_e)str_to_uint32_t(token, 10);
        break;
      case 1:
        params = token;
        break;
      default:
        break;
    }
  }
  str param1 = {0};
  str param2 = {0};
  for (int i = 0; str_tok(data, ",", &token); i++) {
    switch (i) {
      case 0:
        param1 = token;
        if (param1.len >= 2 && param1.s[0] == '\"' && param1.s[param1.len - 1] == '\"') {
          param1.s += 1;
          param1.len -= 2;
        }
        break;
      case 1:
        param2 = token;
        if (param2.len >= 2 && param2.s[0] == '\"' && param2.s[param2.len - 1] == '\"') {
          param2.s += 1;
          param2.len -= 2;
        }
        break;
      default:
        break;
    }
  }
  // TODO: handle this by passing it up to user
  LOG(L_NOTICE,
      "Received URC for profile configuration %u(%s) - [%.*s] [%.*s]\r\n",
      op_code, at_mqtt_config_op_code_text(op_code), param1.len, param1.s, param2.len, param2.s);
  return 1;
}

static str s_uumqttwtopic = STRDECL("+UUMQTTWTOPIC");

int OwlModemMQTTRN4::processURCMQTTWillTopic(str urc, str data) {
  if (!str_equal(urc, s_uumqttwtopic)) return 0;
  str token = {0};
  at_mqtt_qos_e qos;
  at_mqtt_retain_e retain;
  str topic = {0};
  for (int i = 0; str_tok(data, ",", &token); i++) {
    switch (i) {
      case 0:
        qos = (at_mqtt_qos_e)str_to_uint32_t(token, 10);
        break;
      case 1:
        retain = (at_mqtt_retain_e)str_to_uint32_t(token, 10);
        break;
      case 2:
        topic = token;
        break;
      default:
        break;
    }
  }
  // TODO: handle this by passing it up to user
  LOG(L_NOTICE,
      "Received URC for will topic %u(%s) - %u(%s) - [%.*s]\r\n",
      qos, at_mqtt_qos_text(qos), retain, at_mqtt_retain_text(retain), topic.len, topic.s);
  return 1;
}

static str s_uumqttwmsg = STRDECL("+UUMQTTWMSG");

int OwlModemMQTTRN4::processURCMQTTWillMessage(str urc, str data) {
  if (!str_equal(urc, s_uumqttwmsg)) return 0;
  // TODO: handle this by passing it up to user
  LOG(L_NOTICE,
      "Received URC for will message [%.*s]\r\n",
      data.len, data.s);
  return 1;
}

bool OwlModemMQTTRN4::processURC(str urc, str data, void *instance) {
  OwlModemMQTTRN4 *inst = reinterpret_cast<OwlModemMQTTRN4 *>(instance);

  return inst->processURCMQTTClient(urc, data) || 
    inst->processURCMQTTCM(urc, data) || 
    inst->processIncomingMessages(urc, data) || 
    inst->processURCMQTTProfileConfiguration(urc, data) || 
    inst->processURCMQTTWillTopic(urc, data) || 
    inst->processURCMQTTWillMessage(urc, data);
}

int OwlModemMQTTRN4::processUnsolicitedInCommandResponse(str line) {
  if (!readingCommands) return 0;

  LOG(L_NOTICE,
      "attempting to parse [%.*s]\r\n",
      line.len, line.s);

  if (str_find(line, STRDECL("Topic:")) == 0) {
    return 1;
  } else if (str_find(line, STRDECL("Len:")) == 0) {
    return 1;
  } else if (str_find(line, STRDECL("Msg:")) == 0) {
    return 1;
  } else {
    return 0;
  }
}

int OwlModemMQTTRN4::evaluateMQTTResponse(str mqtt_response) {
  str token = {0};
  long int response = 0;
  for (int i = 0; str_tok(mqtt_response, ",\r\n", &token); i++) {
    switch (i) {
      case 0:
        // op_code
        break;
      case 1:
        response = str_to_long_int(token, 10);
        break;
      default:
        break;
    }
  }

  return response;
}

static str s_umqtt = STRDECL("+UMQTT: ");

int OwlModemMQTTRN4::setClientID(str client_id) {
  if (client_id.len == 0 || client_id.len > 23) {
    return 0;
  }
  char buf[64];
  snprintf(buf, 64, "AT+UMQTT=%u,\"%.*s\"", AT_MQTT__Config_Op_Code__Unique_Client_ID, client_id.len, client_id.s);
  int result = (owlModem->doCommandBlocking(buf, 30 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqtt, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}

int OwlModemMQTTRN4::setLocalPort(uint16_t port) {
  if (port == 0) {
    return 0;
  }
  char buf[64];
  snprintf(buf, 64, "AT+UMQTT=%u,%u", AT_MQTT__Config_Op_Code__Port_Number, port);
  int result = (owlModem->doCommandBlocking(buf, 30 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqtt, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}

int OwlModemMQTTRN4::setServerHostname(str hostname, uint16_t port) {
  char buf[64];
  if (port == 0) {
    snprintf(buf, 64, "AT+UMQTT=%u,\"%.*s\"", AT_MQTT__Config_Op_Code__Server_Name, hostname.len, hostname.s);
  } else {
    snprintf(buf, 64, "AT+UMQTT=%u,\"%.*s\",%u", AT_MQTT__Config_Op_Code__Server_Name, hostname.len, hostname.s, port);
  }
  int result = (owlModem->doCommandBlocking(buf, 30 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqtt, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}

int OwlModemMQTTRN4::setServerIPAddress(str ip_address, uint16_t port) {
  char buf[64];
  if (port == 0) {
    snprintf(buf, 64, "AT+UMQTT=%u,\"%.*s\"", AT_MQTT__Config_Op_Code__IP_Address, ip_address.len, ip_address.s);
  } else {
    snprintf(buf, 64, "AT+UMQTT=%u,\"%.*s\",%u", AT_MQTT__Config_Op_Code__IP_Address, ip_address.len, ip_address.s, port);
  }
  int result = (owlModem->doCommandBlocking(buf, 30 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqtt, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}

int OwlModemMQTTRN4::setCredentials(str username, str password) {
  if (username.len > 30 || password.len > 30) {
    return 0;
  }
  char buf[128];
  snprintf(buf, 128, "AT+UMQTT=%u,\"%.*s\",\"%.*s\"", AT_MQTT__Config_Op_Code__Username_And_Password, username.len, username.s, password.len, password.s);
  int result = (owlModem->doCommandBlocking(buf, 30 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqtt, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}

int OwlModemMQTTRN4::setInactivityTimeout(uint16_t timeout) {
  char buf[64];
  snprintf(buf, 64, "AT+UMQTT=%u,%u", AT_MQTT__Config_Op_Code__Inactivity_Timeout, timeout);
  int result = (owlModem->doCommandBlocking(buf, 30 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqtt, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}

int OwlModemMQTTRN4::setSecureMode(at_mqtt_secure_e secure, uint8_t profile) {
  if (secure < AT_MQTT__Secure__No_TLS || secure > AT_MQTT__Secure__TLS || profile > 4) {
    return 0;
  }
  char buf[64];
  snprintf(buf, 64, "AT+UMQTT=%u,%u,%u", AT_MQTT__Config_Op_Code__Secure, secure, profile);
  int result = (owlModem->doCommandBlocking(buf, 30 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqtt, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}

int OwlModemMQTTRN4::setCleanSessionMode(at_mqtt_clean_session_e clean) {
  if (clean != AT_MQTT__Clean_Session__Persist && clean != AT_MQTT__Clean_Session__Clean) {
    return 0;
  }
  char buf[64];
  snprintf(buf, 64, "AT+UMQTT=%u,%u", AT_MQTT__Config_Op_Code__Clean_Session, clean);
  int result = (owlModem->doCommandBlocking(buf, 30 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqtt, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}

static str s_umqttwtopic = STRDECL("+UMQTTWTOPIC: ");

int OwlModemMQTTRN4::setWillTopic(at_mqtt_qos_e qos, at_mqtt_retain_e retain, str topic) {
  if (!(qos >= AT_MQTT__QoS__At_Most_Once && qos <= AT_MQTT__QoS__Exactly_Once) ||
      !(retain >= AT_MQTT__Retain__No_Retain && retain <= AT_MQTT__Retain__Retain) ||
      !(topic.len > 0 && topic.len <= MODEM_MQTT_TOPIC_MAX_LEN)) {
    return 0;
  }
  char buf[32 + MODEM_MQTT_TOPIC_MAX_LEN];
  snprintf(buf, 32 + MODEM_MQTT_TOPIC_MAX_LEN, "AT+UMQTTWTOPIC=%u,%u,\"%.*s\"", qos, retain, topic.len, topic.s);
  int result = (owlModem->doCommandBlocking(buf, 30 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqttwtopic, mqtt_response, &mqtt_response);
  str token = {0};
  long int response = 0;
  for (int i = 0; str_tok(mqtt_response, ",\r\n", &token); i++) {
    switch (i) {
      case 0:
        response = str_to_long_int(token, 10);
        break;
      default:
        break;
    }
  }
  result &= response;

  return result;
}

static str s_umqttwmsg = STRDECL("+UMQTTWMSG: ");

int OwlModemMQTTRN4::setWillMessage(str message) {
  if (!(message.len > 0 && message.len <= MODEM_MQTT_MESSAGE_MAX_LEN)) {
    return 0;
  }
  char buf[32 + MODEM_MQTT_MESSAGE_MAX_LEN];
  snprintf(buf, 32 + MODEM_MQTT_MESSAGE_MAX_LEN, "AT+UMQTTWMSG=\"%.*s\"", message.len, message.s);
  int result = (owlModem->doCommandBlocking(buf, 30 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqttwmsg, mqtt_response, &mqtt_response);
  str token = {0};
  long int response = 0;
  for (int i = 0; str_tok(mqtt_response, ",\r\n", &token); i++) {
    switch (i) {
      case 0:
        response = str_to_long_int(token, 10);
        break;
      default:
        break;
    }
  }
  result &= response;

  return result;
}

int OwlModemMQTTRN4::nvmRestoreDefaultProfile() {
  return nvmProfileOperation(AT_MQTT__NVM_Mode__Restore_To_Default);
}

static str s_umqttnv = STRDECL("+UMQTTNV: ");

int OwlModemMQTTRN4::nvmLoadProfile() {
  return nvmProfileOperation(AT_MQTT__NVM_Mode__Load_From_NVM);
}

int OwlModemMQTTRN4::nvmSaveProfile() {
  return nvmProfileOperation(AT_MQTT__NVM_Mode__Save_To_NVM);
}

int OwlModemMQTTRN4::nvmProfileOperation(at_mqtt_nvm_mode_e operation) {
  char buf[32];
  snprintf(buf, 32, "AT+UMQTTNV=%u", operation);
  int result = (owlModem->doCommandBlocking(buf, 30 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqttnv, mqtt_response, &mqtt_response);
  str token = {0};
  long int response = 0;
  for (int i = 0; str_tok(mqtt_response, ",\r\n", &token); i++) {
    switch (i) {
      case 0:
        // nvm_mode
        break;
      case 1:
        response = str_to_long_int(token, 10);
        break;
      default:
        break;
    }
  }
  result &= response;

  return result;
}

static str s_umqttc = STRDECL("+UMQTTC: ");

int OwlModemMQTTRN4::disconnect() {
  char buf[32];
  snprintf(buf, 32, "AT+UMQTTC=%u", AT_MQTT__Client_Op_Code__Disconnect);
  int result = (owlModem->doCommandBlocking(buf, 60 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqttc, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}

int OwlModemMQTTRN4::connect() {
  char buf[32];
  snprintf(buf, 32, "AT+UMQTTC=%u", AT_MQTT__Client_Op_Code__Connect);
  int result = (owlModem->doCommandBlocking(buf, 120 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqttc, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response); // this indicates if the connection is attempting

  return result;
}

int OwlModemMQTTRN4::publishMessage(at_mqtt_qos_e qos, at_mqtt_retain_e retain, str topic, str message) {
  // TODO: topic + message should not exceed the command buffer's limit either
  if (!(qos >= AT_MQTT__QoS__At_Most_Once && qos <= AT_MQTT__QoS__Exactly_Once) ||
      !(retain >= AT_MQTT__Retain__No_Retain && retain <= AT_MQTT__Retain__Retain) ||
      !(topic.len > 0 && topic.len <= MODEM_MQTT_TOPIC_MAX_LEN) ||
      !(message.len > 0 && message.len <= MODEM_MQTT_MESSAGE_MAX_LEN)) {
    return 0;
  }

  char buf[32 + MODEM_MQTT_TOPIC_MAX_LEN + MODEM_MQTT_MESSAGE_MAX_LEN];
  snprintf(buf, 32 + MODEM_MQTT_TOPIC_MAX_LEN + MODEM_MQTT_MESSAGE_MAX_LEN, "AT+UMQTTC=%u,%u,%u,%u,\"%.*s\",\"%.*s\"", 
      AT_MQTT__Client_Op_Code__Publish_Message,
      qos,
      retain,
      AT_MQTT__Mode__ASCII,
      topic.len, topic.s,
      message.len, message.s);
  int result = (owlModem->doCommandBlocking(buf, 60 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqttc, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}

int OwlModemMQTTRN4::subscribeTopic(at_mqtt_qos_e max_qos, str topic) {
  if (!(topic.len > 0 && topic.len <= MODEM_MQTT_TOPIC_MAX_LEN)) {
    return 0;
  }

  char buf[32 + MODEM_MQTT_TOPIC_MAX_LEN];
  snprintf(buf, 32 + MODEM_MQTT_TOPIC_MAX_LEN, "AT+UMQTTC=%u, %u,\"%.*s\"", 
      AT_MQTT__Client_Op_Code__Subscribe_Topic,
      max_qos,
      topic.len, topic.s);
  int result = (owlModem->doCommandBlocking(buf, 60 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqttc, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}

int OwlModemMQTTRN4::unsubscribeTopic(str topic) {
  if (!(topic.len > 0 && topic.len <= MODEM_MQTT_TOPIC_MAX_LEN)) {
    return 0;
  }

  char buf[32 + MODEM_MQTT_TOPIC_MAX_LEN];
  snprintf(buf, 32 + MODEM_MQTT_TOPIC_MAX_LEN, "AT+UMQTTC=%u,\"%.*s\"", 
      AT_MQTT__Client_Op_Code__Unsubscribe_Topic,
      topic.len, topic.s);
  int result = (owlModem->doCommandBlocking(buf, 60 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqttc, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}

int OwlModemMQTTRN4::readMessages() {
  char buf[32];
  snprintf(buf, 32, "AT+UMQTTC=%u", 
      AT_MQTT__Client_Op_Code__Read_Unread_Messages);
  readingCommands = true;
  LOG(L_NOTICE, "setting readingCommands\r\n");
  int result = (owlModem->doCommandBlocking(buf, 60 * 1000, &mqtt_response) == AT_Result_Code__OK);
  LOG(L_NOTICE, "unsetting readingCommands\r\n");
  readingCommands = false;
  if (!result) return 0;
  LOG(L_NOTICE,
      "Received read message response: %.*s\r\n",
      mqtt_response.len, mqtt_response.s);
  owlModem->filterResponse(s_umqttc, mqtt_response, &mqtt_response);

  //char recv_buf[1200];
  //str recv = { .s = recv_buf, .len = 0 };
  //at_result_code_e resultCode = owlModem->extractResult(&recv, 1200); 
  result &= evaluateMQTTResponse(mqtt_response);
  //LOG(L_NOTICE,
      //"Extracted result: %.*s\r\n",
      //recv.len, recv.s);

  return result;
}

// TODO: set this ourselves to always be Extended_Formatting?
int OwlModemMQTTRN4::setMessageFormat(at_mqtt_format_e format) {
  if (!(format >= AT_MQTT__Format__No_Formatting && format <= AT_MQTT__Format__Extended_Formatting)) {
    return 0;
  }

  char buf[32];
  snprintf(buf, 32, "AT+UMQTTC=%u,%u", 
      AT_MQTT__Client_Op_Code__Set_Message_Format,
      format);
  int result = (owlModem->doCommandBlocking(buf, 60 * 1000, &mqtt_response) == AT_Result_Code__OK);
  if (!result) return 0;
  owlModem->filterResponse(s_umqttc, mqtt_response, &mqtt_response);
  result &= evaluateMQTTResponse(mqtt_response);

  return result;
}
