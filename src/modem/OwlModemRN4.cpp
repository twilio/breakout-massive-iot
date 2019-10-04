/*
 * OwlModemRN4.cpp
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
 * \file OwlModemRN4.h - wrapper for U-blox SARA-R4/SARA-N4 modems on Seeed WiO tracker board
 */

#include "OwlModemRN4.h"

#include <stdarg.h>
#include <stdio.h>


OwlModemRN4::OwlModemRN4(IOwlSerial *modem_port_in, IOwlSerial *debug_port_in)
    : modem_port(modem_port_in),
      debug_port(debug_port_in),
      AT(modem_port),
      information(&AT),
      SIM(&AT),
      network(&AT),
      network_rn4(&AT),
      pdn(&AT),
      ssl(&AT),
      socket(&AT) {
  if (!modem_port_in) {
    LOG(L_ERR, "OwlModemRN4 initialized without modem port. That is not going to work\r\n");
  }


  has_modem_port = (modem_port_in != nullptr);
  has_debug_port = (debug_port_in != nullptr);
}

OwlModemRN4::~OwlModemRN4() {
}


int OwlModemRN4::powerOn() {
  if (!has_modem_port) {
    return false;
  }

  if (isPoweredOn()) return 1;

  owl_power_on(OWL_POWER_RN4);

  owl_time_t timeout = owl_time() + 10 * 1000;
  while (!isPoweredOn()) {
    if (owl_time() > timeout) {
      LOG(L_ERR, "Timed-out waiting for modem to power on\r\n");
      return 0;
    }
    owl_delay(50);
  }

  return 1;
}

void OwlModemRN4::powerOff() {
  owl_power_off(OWL_POWER_RN4);
}

int OwlModemRN4::isPoweredOn() {
  return AT.doCommandBlocking("AT", 1000, nullptr) == at_result_code::OK;
}


/**
 * Handler for PIN, used during initialization
 * @param message
 */
void initCheckPIN(str message) {
  if (!str_equal_prefix_char(message, "READY")) {
    LOG(L_ERR,
        "PIN status [%.*s] != READY and PIN handler not set. Please disable the SIM card PIN, or set a handler.\r\n",
        message.len, message.s);
  }
}

int OwlModemRN4::initModem(int testing_variant, const char *apn, const char *cops, cops_format cops_format) {
  OwlModem_PINHandler_f saved_handler = 0;
  umnoprof_mno_profile current_profile;
  umnoprof_mno_profile expected_profile = (testing_variant & Testing__Set_MNO_Profile_to_Default) == 0 ?
                                              umnoprof_mno_profile::TMO :
                                              umnoprof_mno_profile::SW_Default;

  if (!AT.initTerminal()) {
    return 0;
  }

  if (cops != nullptr) {
    // deregister from the network before the modem hangs
    if (!network.setOperatorSelection(cops_mode::Deregister_From_Network, nullptr, nullptr, nullptr)) {
      LOG(L_ERR, "Potential deregistering from network\r\n");
    }
  }

  /* Resetting the modem network parameters */
  if (!network_rn4.getModemMNOProfile(&current_profile)) {
    LOG(L_ERR, "Error retrieving current MNO Profile\r\n");
    return 0;
  }

  if (current_profile != expected_profile || (testing_variant & Testing__Set_APN_Bands_to_Berlin) != 0 ||
      (testing_variant & Testing__Set_APN_Bands_to_US) != 0) {
    /* A modem reset is required */
    if (!network.setModemFunctionality(cfun_fun::Minimum_Functionality, 0)) LOG(L_WARN, "Error turning modem off\r\n");

    if (current_profile != expected_profile) {
      LOG(L_WARN, "Updating MNO Profile to %d - %s\r\n", expected_profile, at_enum_stringify(expected_profile));
      if (!network_rn4.setModemMNOProfile(expected_profile)) {
        LOG(L_ERR, "Error re-setting MNO Profile from %d to %d\r\n", current_profile, expected_profile);
        return 0;
      }
    }

    if ((testing_variant & Testing__Set_APN_Bands_to_Berlin) != 0) {
      if (AT.doCommandBlocking("AT+URAT=8", 5000, nullptr) != at_result_code::OK)
        LOG(L_WARN, "Error setting RAT to NB1\r\n");
      // This is a bitmask, LSB meaning Band1, MSB meaning Band64
      if (AT.doCommandBlocking("AT+UBANDMASK=0,0", 5000, nullptr) != at_result_code::OK)
        LOG(L_WARN, "Error setting band mask for Cat-M1 to none\r\n");
      if (AT.doCommandBlocking("AT+UBANDMASK=1,168761503", 5000, nullptr) != at_result_code::OK)
        LOG(L_WARN, "Error setting band mask for NB1 to 168761503 (manual default\r\n");

      AT.commandSprintf("AT+CGDCONT=1,\"IP\",\"%s\"", apn);
      if (AT.doCommandBlocking(5000, nullptr) != at_result_code::OK) LOG(L_WARN, "Error setting custom APN\r\n");
    }
    if ((testing_variant & Testing__Set_APN_Bands_to_US) != 0) {
      if (AT.doCommandBlocking("AT+URAT=8", 5000, nullptr) != at_result_code::OK)
        LOG(L_WARN, "Error setting RAT to NB1\r\n");
      // This is a bitmask, LSB meaning Band1, MSB meaning Band64
      if (AT.doCommandBlocking("AT+UBANDMASK=0,0", 5000, nullptr) != at_result_code::OK)
        LOG(L_WARN, "Error setting band mask for Cat-M1 to 2/4/5/12\r\n");
      if (AT.doCommandBlocking("AT+UBANDMASK=1,2074", 5000, nullptr) != at_result_code::OK)
        LOG(L_WARN, "Error setting band mask for NB1 to 2/4/5/12 (manual default)\r\n");
    }

    if (!network.setModemFunctionality(cfun_fun::Modem_Silent_Reset_No_SIM_Reset, 0))
      LOG(L_WARN, "Error resetting modem\r\n");
    // wait for the modem to come back
    while (!isPoweredOn()) {
      LOG(L_INFO, "..  - waiting for modem to power back on after reset\r\n");
      owl_delay(100);
    }

    if (!AT.initTerminal()) {
      return 0;
    }

    if (cops != nullptr) {
      // deregister from the network before the modem hangs
      if (!network.setOperatorSelection(cops_mode::Deregister_From_Network, nullptr, nullptr, nullptr)) {
        LOG(L_ERR, "Potential deregistering from network\r\n");
      }
    }
  }


  if (cops != nullptr) {
    cops_act cops_act = cops_act::LTE_NB_S1;
    str oper          = STRDECL(cops);
    LOG(L_INFO, "Selecting network operator \"%s\", it can take a while.\r\n", cops);
    if (!network.setOperatorSelection(cops_mode::Manual_Selection, &cops_format, &oper, &cops_act)) {
      LOG(L_ERR, "Error selecting mobile operator\r\n");
      return 0;
    }
  }

  if (AT.doCommandBlocking("AT+CSCS=\"GSM\"", 1000, nullptr) != at_result_code::OK) {
    LOG(L_WARN, "Potential error setting character set to GSM\r\n");
  }

  /* Set the on-board LEDs */
  if (AT.doCommandBlocking("AT+UGPIOC=23,10", 5000, nullptr) != at_result_code::OK) {
    LOG(L_WARN, "..  - failed to map pin 23 (yellow led)  to \"module operating status indication\"\r\n");
  }
  if (AT.doCommandBlocking("AT+UGPIOC=16,2", 5000, nullptr) != at_result_code::OK) {
    LOG(L_WARN, "..  - failed to map pin 16 (blue led) to \"network status indication\"\r\n");
  }

  /* TODO - decide if to keep this in */
  if (AT.doCommandBlocking("AT+CREG=2", 1000, nullptr) != at_result_code::OK) {
    LOG(L_WARN,
        "Potential error setting URC to Registration and Location Updates for Network Registration Status events\r\n");
  }
  if (AT.doCommandBlocking("AT+CGREG=2", 1000, nullptr) != at_result_code::OK) {
    LOG(L_WARN,
        "Potential error setting GPRS URC to Registration and Location Updates for Network Registration Status "
        "events\r\n");
  }
  if (AT.doCommandBlocking("AT+CEREG=2", 1000, nullptr) != at_result_code::OK) {
    LOG(L_WARN,
        "Potential error setting EPS URC to Registration and Location Updates for Network Registration Status "
        "events\r\n");
  }

  if (SIM.handler_cpin) saved_handler = SIM.handler_cpin;
  SIM.setHandlerPIN(initCheckPIN);
  if (AT.doCommandBlocking("AT+CPIN?", 5000, nullptr) != at_result_code::OK) {
    LOG(L_WARN, "Error checking PIN status\r\n");
  }
  SIM.setHandlerPIN(saved_handler);

  if (AT.doCommandBlocking("AT+UDCONF=1,1", 1000, nullptr) != at_result_code::OK) {
    LOG(L_WARN, "Potential error setting ublox HEX mode for socket ops send/receive\r\n");
  }

  LOG(L_DBG, "Modem correctly initialized\r\n");
  return 1;
}

int OwlModemRN4::waitForNetworkRegistration(const char *purpose, int testing_variant) {
  bool network_ready = false;
  bool needs_reset   = false;
  owl_time_t timeout = owl_time() + 30 * 1000;
  while (true) {
    cereg_stat stat;
    if (network.getEPSRegistrationStatus(0, &stat, 0, 0, 0, 0, 0)) {
      network_ready = (stat == cereg_stat::Registered_Home_Network || stat == cereg_stat::Registered_Roaming);
      if (network_ready) break;
      if (stat == cereg_stat::Registration_Denied || stat == cereg_stat::Not_Registered) needs_reset = true;
    }
    if ((testing_variant & Testing__Timeout_Network_Registration_30_Sec) != 0 && owl_time() > timeout) {
      LOG(L_ERR, "Bailing out from network registration - for testing purposes only\r\n");
      return 0;
    }
    if (needs_reset && owl_time() > timeout) {
      LOG(L_INFO, "Failed to connect to network, resetting\r\n");
      needs_reset = false;
      if (!network.setModemFunctionality(cfun_fun::Modem_Silent_Reset_No_SIM_Reset, 0))
        LOG(L_WARN, "Error resetting modem\r\n");
      // wait for the modem to come back
      while (!isPoweredOn()) {
        LOG(L_INFO, "..  - waiting for modem to power back on after reset\r\n");
        owl_delay(100);
      }
    }
    LOG(L_NOTICE, ".. waiting for network registration\r\n");
    owl_delay(2000);
  }

  if ((testing_variant & Testing__Skip_Set_Host_Device_Information) != 0) return 1;

  if (!setHostDeviceInformation(purpose)) {
    LOG(L_WARN, "Error setting HostDeviceInformation.  If this persists, please inform Twilio support.\r\n");
    // TODO: set a flag to report to Twilio Object-16 registration timed out or failed
  }

  return 1;
}



static str s_exitbypass = {.s = "exitbypass", .len = 10};

void OwlModemRN4::bypassCLI() {
  if (!has_modem_port || !has_debug_port) {
    return;
  }

  // TODO - set echo on/off - maybe with parameter to this function? but that will mess with other code
  in_bypass = 1;
  uint8_t c;
  unsigned int index = 0;
  while (1) {
    if (modem_port->available()) {
      modem_port->read(&c, 1);
      debug_port->write(&c, 1);
    }
    if (debug_port->available()) {
      debug_port->read(&c, 1);
      modem_port->write(&c, 1);
      if (s_exitbypass.s[index] == c)
        index++;
      else
        index = 0;
      if (index == s_exitbypass.len) {
        modem_port->write((uint8_t *)"\r\n", 2);
        in_bypass = 0;
        return;
      }
    }
  }
}

void OwlModemRN4::bypass() {
  if (!has_modem_port || !has_debug_port) {
    return;
  }

  uint8_t c;
  while (modem_port->available()) {
    modem_port->read(&c, 1);
    debug_port->write(&c, 1);
  }
  while (debug_port->available())
    debug_port->read(&c, 1);
  modem_port->write(&c, 1);
}

static str s_dev_kit = STRDECL("devkit");

int OwlModemRN4::setHostDeviceInformation(const char *purpose) {
  str s_purpose;
  if (purpose) {
    s_purpose.s   = purpose;
    s_purpose.len = strlen(purpose);
  } else {
    s_purpose = s_dev_kit;
  }

  return setHostDeviceInformation(s_purpose);
}

void OwlModemRN4::computeHostDeviceInformation(str purpose) {
  const char *hostDeviceID      = "Twilio-Alfa";
  const char *hostDeviceIDShort = "alfa";
  const char *board_name        = "WioLTE-Cat-NB1";
  const char *sdk_ver           = "0.1.0";

  // Param 2: Twilio_Seeed_(AT+CGMI -> u-blox) // OwlModemInformation::getManufacturer()
  char module_mfgr_buffer[64];
  str module_mfgr;
  information.getManufacturer(&module_mfgr);
  if (module_mfgr.len > 64) {
    module_mfgr.len = 64;
  }
  memcpy(module_mfgr_buffer, module_mfgr.s, module_mfgr.len);
  module_mfgr.s = module_mfgr_buffer;

  // Param 3: Wio-LTE-Cat-NB1_(AT+CGMM -> SARA-N410-02B) // OwlModemInformation::getModel()
  char module_model_buffer[64];
  str module_model;
  information.getModel(&module_model);
  if (module_model.len > 64) {
    module_model.len = 64;
  }
  memcpy(module_model_buffer, module_model.s, module_model.len);
  module_model.s = module_model_buffer;

  // Param 4: twilio-v0.9_u-blox-v7.4 (AT+CGMR -> L0.0.00.00.07.04 [May 25 2018 15:05:31]) //
  // OwlModemInformation::getVersion()
  char module_ver_buffer[64];
  str module_ver;
  information.getVersion(&module_ver);
  if (module_ver.len > 64) {
    module_ver.len = 64;
  }
  memcpy(module_ver_buffer, module_ver.s, module_ver.len);
  module_ver.s = module_ver_buffer;

  hostdevice_information.len =
      snprintf(hostdevice_information.s, MODEM_HOSTDEVICE_INFORMATION_SIZE,
               "\"%s_%.*s\",\"Twilio_%.*s\",\"%s_%.*s\",\"twilio-v%s_%.*s-v%.*s\"", hostDeviceID, purpose.len,
               purpose.s, module_mfgr.len, module_mfgr.s, board_name, module_model.len, module_model.s, sdk_ver,
               module_mfgr.len, module_mfgr.s, module_ver.len, module_ver.s);

  /* v<sdk_version>/<hostDeviceID> */
  short_hostdevice_information.len =
      snprintf(short_hostdevice_information.s, MODEM_HOSTDEVICE_INFORMATION_SIZE, "v%s/%s", sdk_ver, hostDeviceIDShort);
}

int OwlModemRN4::setHostDeviceInformation(str purpose) {
  bool registered = false;

  if (!purpose.len) purpose = s_dev_kit;
  computeHostDeviceInformation(purpose);

  LOG(L_INFO, "Setting HostDeviceInformation to: %.*s\r\n", hostdevice_information.len, hostdevice_information.s);

  for (int attempts = 10; attempts > 0; attempts--) {
    AT.commandSprintf("AT+UHOSTDEV=%.*s", hostdevice_information.len, hostdevice_information.s);
    if (AT.doCommandBlocking(1000, nullptr) == at_result_code::OK) {
      LOG(L_INFO, ".. setting HostDeviceInformation successful.\r\n");
      registered = true;
      break;
    }
    if (attempts > 0) {
      LOG(L_INFO, ".. setting HostDeviceInformation failed - will retry after a short delay\r\n");
      owl_delay(7000);
    }
  }
  if (!registered) {
    LOG(L_ERR, "Setting HostDeviceInformation failed.\r\n");
    return 0;
  }

  return 1;
}

str OwlModemRN4::getHostDeviceInformation() {
  if (!hostdevice_information.len) computeHostDeviceInformation(s_dev_kit);
  return hostdevice_information;
}

str OwlModemRN4::getShortHostDeviceInformation() {
  if (!short_hostdevice_information.len) computeHostDeviceInformation(s_dev_kit);
  return short_hostdevice_information;
}
