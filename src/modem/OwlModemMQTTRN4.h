/*
 * OwlModemMQTTRN4.h
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
 * \file OwlModemMQTTRN4.h - API for MQTT functionality
 */

#ifndef __OWL_MODEM_MQTT_RN4_H__
#define __OWL_MODEM_MQTT_RN4_H__

#include "enumsRN4.h"



// FIXME: review buffer size
#define MODEM_MQTT_RESPONSE_BUFFER_SIZE 1024
// TODO: determine appropriate max length here
#define MODEM_MQTT_TOPIC_MAX_LEN 256
#define MODEM_MQTT_MESSAGE_MAX_LEN 512




class OwlModemAT;


/**
 * Twilio wrapper for the AT serial interface to a modem - Methods to get interface with the module's MQTT client
 */
class OwlModemMQTTRN4 {
 public:
  OwlModemMQTTRN4(OwlModemAT *owlModem);

  /**
   * Handler for Unsolicited Response Codes from the modem - called from OwlModem on timer, when URC is received
   * @param urc - event id
   * @param data - data of the event
   * @return if the line was handled
   */
  static bool processURC(str urc, str data, void *instance);

  /**
   * Process unsolicited content within a command response
   * @param line - the line
   * @return 1 if the line was handled, 0 if no match here
   */
  int processUnsolicitedInCommandResponse(str line);

  /**
   * Set MQTT client ID
   * @param clientID - Unique client identifier, max length 23 characters
   * @return 1 on success, 0 on failure
   */
  int setClientID(str client_id);

  /**
   * Set local MQTT port (optional)
   * @param port - Local MQTT port number for requests
   * @return 1 on success, 0 on failure
   */
  int setLocalPort(uint16_t port);

  /**
   * Set server MQTT hostname
   * Set either hostname or IP address, the last to be set will be used.
   * @param hostname - MQTT server hostname
   * @param port - MQTT server port (optional - defaults to 1883 for No_TLS, 8883 for TLS)
   * @return 1 on success, 0 on failure
   */
  int setServerHostname(str hostname, uint16_t port=0);

  /**
   * Set server MQTT IP address 
   * Set either hostname or IP address, the last to be set will be used.
   * @param ip_address - MQTT server hostname
   * @param port - MQTT server port (optional - defaults to 1883 for No_TLS, 8883 for TLS)
   * @return 1 on success, 0 on failure
   */
  int setServerIPAddress(str ip_address, uint16_t port=0);

  /**
   * Set server MQTT credentials
   * @param username - MQTT username (maximum 30 characters)
   * @param password - MQTT password (maximum 30 characters)
   * @return 1 on success, 0 on failure
   */
  int setCredentials(str username, str password);

  /**
   * Set inactivity timeout
   * @param timeout - Inactivity timeout in seconds (max value is 65535 - 18h 12m 15s)
   * @return 1 on success, 0 on failure
   */
  int setInactivityTimeout(uint16_t timeout);

  /**
   * Set MQTT secure mode
   * @param secure - Secure mode (AT_MQTT__Secure__No_TLS or AT_MQTT__Secure__TLS)
   * @param profile - USECMNG profile identifier (valid values 0 - 4, default is 0)
   * @return 1 on success, 0 on failure
   */
  int setSecureMode(at_mqtt_secure_e secure, uint8_t profile = 0);

  /**
   * Set MQTT clean session mode
   * @param clean - Clean session mode (AT_MQTT__Clean_Session__Persist or AT_MQTT__Clean_Session__Clean).  Default is Persist.
   * @return 1 on success, 0 on failure
   */
  int setCleanSessionMode(at_mqtt_clean_session_e clean);

  /**
   * Set MQTT will topic 
   * @param qos - Will QoS
   * @param retain - Will retain setting
   * @param topic - Will topic (maximum 256 bytes)
   * @return 1 on success, 0 on failure
   */
  int setWillTopic(at_mqtt_qos_e qos, at_mqtt_retain_e retain, str topic);

  /**
   * Set MQTT will message
   * @param message - Will message (maximum 256 bytes)
   * @return 1 on success, 0 on failure
   */
  int setWillMessage(str message);

  /**
   * Restore MQTT profile state to defaults
   * @return 1 on success, 0 on failure
   */
  int nvmRestoreDefaultProfile();

  /**
   * Load saved MQTT profile state from NVM
   * @return 1 on success, 0 on failure
   */
  int nvmLoadProfile();

  /**
   * Save MQTT profile state to NVM
   * @return 1 on success, 0 on failure
   */
  int nvmSaveProfile();

  /**
   * Disconnect from the MQTT server
   * @return 1 on success, 0 on failure
   */
  int disconnect();

  /**
   * Initiate a connection to the previously specified MQTT server
   * @return 1 on success, 0 on failure
   */
  int connect();

  /**
   * Publish an MQTT message
   * @param qos - QoS for the message
   * @param retain - Retain for the message
   * @param topic - Topic for the message (maximum 256 bytes)
   * @param message - The message (maximum 512 bytes)
   * @return 1 on success, 0 on failure
   */
  int publishMessage(at_mqtt_qos_e qos, at_mqtt_retain_e retain, str topic, str message);

  /**
   * Subscribe to an MQTT topic
   * @param max_qos - Maximum QOS desired
   * @param topic - Topic for the message (maximum 256 bytes)
   * @return 1 on success, 0 on failure
   */
  int subscribeTopic(at_mqtt_qos_e max_qos, str topic);

  /**
   * Unsubscribe to an MQTT topic
   * @param topic - Topic for the message (maximum 256 bytes)
   * @return 1 on success, 0 on failure
   */
  int unsubscribeTopic(str topic);

  /**
   * Initiate a read of pending MQTT messages, messages will be delivered by callback
   * @return 1 on success, 0 on failure
   */
  int readMessages();

  /**
   * Set MQTT message format
   * @param format - Message format (AT_MQTT__Format__Extended_Formatting is recommended)
   * @return 1 on success, 0 on failure
   */
  int setMessageFormat(at_mqtt_format_e format);

 private:
  OwlModemAT *owlModem = 0;

  char mqtt_response_buffer[MODEM_MQTT_RESPONSE_BUFFER_SIZE];
  str mqtt_response = {.s = mqtt_response_buffer, .len = 0};

  bool readingCommands = false;

  int processURCMQTTClient(str urc, str data);
  int processURCMQTTCM(str urc, str data);
  int processIncomingMessages(str urc, str data);
  int processURCMQTTProfileConfiguration(str urc, str data);
  int processURCMQTTWillTopic(str urc, str data);
  int processURCMQTTWillMessage(str urc, str data);

  int nvmProfileOperation(at_mqtt_nvm_mode_e operation);
  int evaluateMQTTResponse(str mqtt_response);
};

#endif
