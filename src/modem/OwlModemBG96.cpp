/*
 * OwlModemBG96.cpp
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
 * \file OwlModemBG96.cpp - wrapper for BG96 modems on Seeed WiO tracker board via the Grove port
 */

#include "OwlModemBG96.h"

OwlModemBG96::OwlModemBG96(IOwlSerial* modem_port_in)
    : modem_port(modem_port_in),
      AT(modem_port),
      information(&AT),
      sim(&AT),
      network(&AT),
      pdn(&AT),
      ssl(&AT),
      mqtt(&AT) {
  if (!modem_port_in) {
    LOG(L_ERR, "OwlModemBG96 initialized without modem port. That is not going to work\r\n");
  }

  has_modem_port = (modem_port_in != nullptr);
}

OwlModemBG96::~OwlModemBG96() {
}

int OwlModemBG96::powerOn() {
  owl_power_on(OWL_POWER_BG96);

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

int OwlModemBG96::powerOff() {
  AT.doCommandBlocking("AT+QPOWD=0", 500, nullptr);
  owl_delay(300);

  owl_power_off(OWL_POWER_BG96);
}

int OwlModemBG96::isPoweredOn() {
  return AT.doCommandBlocking("AT", 1000, nullptr) == AT_Result_Code__OK;
}

/**
 * Handler for PIN, used during initialization
 * @param message
 */
void OwlModemBG96::initCheckPIN(str message) {
  if (!str_equal_prefix_char(message, "READY")) {
    LOG(L_ERR,
        "PIN status [%.*s] != READY and PIN handler not set. Please disable the SIM card PIN, or set a handler.\r\n",
        message.len, message.s);
  }
}

int OwlModemBG96::initModem(const char* apn, const char* cops, at_cops_format_e cops_format) {
  at_result_code_e rc;
  OwlModem_PINHandler_f saved_handler = 0;

  if (!AT.initTerminal()) {
    return 0;
  }

  if (cops != nullptr) {
    // deregister from the network before the modem hangs
    if (!network.setOperatorSelection(AT_COPS__Mode__Deregister_from_Network, nullptr, nullptr, nullptr)) {
      LOG(L_ERR, "Potential deregistering from network\r\n");
    }
  }

  // TODO: skip if modem is already in the desired state. Writing some magic number to a file maybe?
  if (true) {
    /* A modem reset is required */
    if (!network.setModemFunctionality(AT_CFUN__FUN__Minimum_Functionality, 0)) {
      LOG(L_WARN, "Error turning modem off\r\n");
    }

    if (AT.doCommandBlocking("AT+QCFG=\"nwscanseq\",03,1", 5000, nullptr) != AT_Result_Code__OK) {
      LOG(L_WARN, "Error setting RAT priority to NB1\r\n");
    }

    if (AT.doCommandBlocking("AT+QCFG=\"nwscanmode\",3,1", 5000, nullptr) != AT_Result_Code__OK) {
      LOG(L_WARN, "Error setting RAT technology to LTE\r\n");
    }

    if (AT.doCommandBlocking("AT+QCFG=\"iotopmode\",1,1", 5000, nullptr) != AT_Result_Code__OK) {
      LOG(L_WARN, "Error setting network node to NB1\r\n");
    }

    if (AT.doCommandBlocking("AT+QCFG=\"band\",0,0,A0E189F,1", 5000, nullptr) != AT_Result_Code__OK) {
      LOG(L_WARN, "Error setting NB bands to \"all bands\"\r\n");
    }

    // TODO: celevel and servicedomain

    if (AT.doCommandBlocking("AT+QURCCFG=\"urcport\",\"uart1\"", 5000, nullptr) != AT_Result_Code__OK) {
      LOG(L_WARN, "Error directing URCs to the main UART\r\n");
    }

    AT.commandSprintf("AT+QICSGP=1,1,\"%s\"", apn);
    if (AT.doCommandBlocking(1000, nullptr) != AT_Result_Code__OK) {
      LOG(L_WARN, "Error setting custom APN\r\n");
    }


    // at_cfun_rst_e rst = AT_CFUN__RST__Modem_and_SIM_Silent_Reset;
    // NOTE: BG96 seens to go to power down instead of resetting
    if (!network.setModemFunctionality(AT_CFUN__FUN__Full_Functionality, 0)) {
      LOG(L_WARN, "Error resetting modem\r\n");
    }

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
      if (!network.setOperatorSelection(AT_COPS__Mode__Deregister_from_Network, nullptr, nullptr, nullptr)) {
        LOG(L_ERR, "Potential deregistering from network\r\n");
      }
    }
  }


  if (cops != nullptr) {
    at_cops_act_e cops_act = AT_COPS__Access_Technology__LTE_NB_S1;
    str oper               = STRDECL(cops);

    LOG(L_INFO, "Selecting network operator \"%s\", it can take a while.\r\n", cops);
    if (!network.setOperatorSelection(AT_COPS__Mode__Manual_Selection, &cops_format, &oper, &cops_act)) {
      LOG(L_ERR, "Error selecting mobile operator\r\n");
      return 0;
    }
  }

  if (AT.doCommandBlocking("AT+CSCS=\"GSM\"", 1000, nullptr) != AT_Result_Code__OK) {
    LOG(L_WARN, "Potential error setting character set to GSM\r\n");
  }

  if (AT.doCommandBlocking("AT+CREG=2", 1000, nullptr) != AT_Result_Code__OK) {
    LOG(L_WARN,
        "Potential error setting URC to Registration and Location Updates for Network Registration Status events\r\n");
  }
  if (AT.doCommandBlocking("AT+CGREG=2", 1000, nullptr) != AT_Result_Code__OK) {
    LOG(L_WARN,
        "Potential error setting GPRS URC to Registration and Location Updates for Network Registration Status "
        "events\r\n");
  }
  if (AT.doCommandBlocking("AT+CEREG=2", 1000, nullptr) != AT_Result_Code__OK) {
    LOG(L_WARN,
        "Potential error setting EPS URC to Registration and Location Updates for Network Registration Status "
        "events\r\n");
  }

  if (sim.handler_cpin) saved_handler = sim.handler_cpin;
  sim.setHandlerPIN(initCheckPIN);
  if (AT.doCommandBlocking("AT+CPIN?", 5000, nullptr) != AT_Result_Code__OK) {
    LOG(L_WARN, "Error checking PIN status\r\n");
  }

  AT.doCommandBlocking("AT+QIACT=1", 5000, nullptr);  // ignore the result, which will be an error if already activated

  if (AT.doCommandBlocking("AT+QIDNSCFG=1,\"8.8.8.8\"", 2000, nullptr) != AT_Result_Code__OK) {
    LOG(L_WARN, "Potential error setting DNS server\n");
  }

  LOG(L_DBG, "Modem correctly initialized\r\n");
  return 1;
}

int OwlModemBG96::waitForNetworkRegistration(owl_time_t timeout) {
  bool network_ready    = false;
  owl_time_t begin_time = owl_time();

  while (true) {
    at_cereg_stat_e stat;
    if (network.getEPSRegistrationStatus(0, &stat, 0, 0, 0, 0, 0)) {
      network_ready = (stat == AT_CEREG__Stat__Registered_Home_Network || stat == AT_CEREG__Stat__Registered_Roaming);
      if (network_ready) break;
    }
    if (timeout != 0 && owl_time() > begin_time + timeout) {
      LOG(L_ERR, "Bailing out from network registration - for testing purposes only\r\n");
      return 0;
    }
    LOG(L_NOTICE, ".. waiting for network registration\r\n");
    owl_delay(200);
  }

  return 1;
}
