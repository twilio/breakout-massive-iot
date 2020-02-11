/*
 * OwlModemNetwork.h
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
 * \file OwlModemNetwork.h - API for Network Registration Management
 */

#ifndef __OWL_MODEM_NETWORK_H__
#define __OWL_MODEM_NETWORK_H__

#include "enums.h"

#include "OwlModemAT.h"


/**
 * Handler function signature for Network Registration events (CS)
 * @param stat - status
 * @param lac - local area code
 * @param ci - cell id
 * @param act - access technology
 */
typedef void (*OwlModem_NetworkRegistrationStatusChangeHandler_f)(creg_stat stat, uint16_t lac, uint32_t ci,
                                                                  creg_act act);

/**
 * Handler function signature for GPRS Registration events (2G/3G)
 * @param stat - status
 * @param lac - local area code
 * @param ci - cell id
 * @param act - access technology
 * @param rac - routing area code
 */
typedef void (*OwlModem_GPRSRegistrationStatusChangeHandler_f)(cgreg_stat stat, uint16_t lac, uint32_t ci,
                                                               cgreg_act act, uint8_t rac);

/**
 * Handler function signature for EPS Registration events (EPC - 2G/3G/LTE/WiFi/etc)
 * @param stat - status
 * @param lac - local area code
 * @param ci - cell id
 * @param act - access technology
 * @param cause_type
 * @param reject_cause
 */
typedef void (*OwlModem_EPSRegistrationStatusChangeHandler_f)(cereg_stat stat, uint16_t lac, uint32_t ci, cereg_act act,
                                                              cereg_cause_type cause_type, uint32_t reject_cause);

/**
 * Handler function signature for eDRX change events
 * @param network - Network technology
 * @param requested_value - Requested eDRX cycle length
 * @param provided_value - Network provided eDRX cycle length
 * @param paging_time_window - Network provided Paging Time Window
 */
typedef void (*OwlModem_EDRXStatusChangeHandler_f)(edrx_act network, edrx_cycle_length requested_value,
                                                   edrx_cycle_length provided_value,
                                                   edrx_paging_time_window paging_time_window);


/**
 * Twilio wrapper for the AT serial interface to a modem - Methods to get information from the Network card
 */
class OwlModemNetwork {
 public:
  OwlModemNetwork(OwlModemAT *atModem);

  /**
   * Handler for Unsolicited Response Codes from the modem - called from OwlModem on timer, when URC is received
   * @param urc - event id
   * @param data - data of the event
   * @return if the line was handled
   */
  static bool processURC(str urc, str data, void *instance);



  /**
   * Retrieve the current modem functionality mode.
   * @param out_power_mode - Modem power mode
   * @return 1 on success, 0 on failure
   */
  int getModemFunctionality(cfun_power_mode *out_power_mode);

  /**
   * Set the modem functionality mode.
   * @param mode - mode to set
   * @return 1 on success, 0 on failure
   */
  int setModemFunctionality(cfun_fun fun, cfun_rst *reset);

  /**
   * Retrieve the current Operator Selection Mode and selected Operator, Radio Access Technology
   * @param out_mode - output current mode
   * @param out_format - output format of the following operator string
   * @param out_oper - output operator string
   * @param max_oper_len - buffer length provided in the operator string
   * @param out_act - output radio access technology type
   * @return 1 on success, 0 on failure
   */
  int getOperatorSelection(cops_mode *out_mode, cops_format *out_format, str_mut *out_oper, unsigned int max_oper_len,
                           cops_act *out_act);

  /**
   * Set operator selection mode and/or select manually operator/access technology
   * @param mode - new mode
   * @param opt_format - optional format (to set output and/or indicate what format the next operator string parameter
   * is in
   * @param opt_oper - optional operator string - must also provide format parameter with this
   * @param opt_act - optional radio access technology code - must also provide format and oper parameters with this
   * @return 1 on success, 0 on failure
   */
  int setOperatorSelection(cops_mode mode, cops_format *opt_format, str *opt_oper, cops_act *opt_act);

  /**
   * Get operator list (a.i. do a scan and provide possible values for setting Operator Selection to Manual
   * The format of the response is a list of operators, each formatted as:
   *
   * (<stat>,long <oper>,short <oper>,numeric <oper>[,<act>])[,(<stat>,long <oper>,short <oper>,numeric <oper>[,<act>])
   * [,...]],,(list-of-supported <mode>s),(list of supported <format>s)
   *
   * @param out_response - str object that will point to the response buffer
   * @return 1 on success, 0 on failure
   */
  int getOperatorList(str *out_response);


  /**
   * Retrieve the current Network Registration Status
   * @param out_n - output Unsolicited Result Code status
   * @param out_stat - output network registration status
   * @param out_lac - output Local/Tracking Area Code
   * @param out_ci - output Cell Identifier, or 0xFFFFFFFFu indicates the current value is invalid
   * @param out_act - output Radio Access Technology (AcT)
   * @return 1 on success, 0 on failure
   */
  int getNetworkRegistrationStatus(creg_n *out_n, creg_stat *out_stat, uint16_t *out_lac, uint32_t *out_ci,
                                   creg_act *out_act);

  /**
   * Set the current style of Unsolicited Response Code (URC) asynchronous reporting for Network Registration Status.
   * @param n - mode to set
   * @return 1 on success, 0 on failure
   */
  int setNetworkRegistrationURC(creg_n n);

  /**
   * When setting the Network Registration Unsolicited Response Code to creg_n::Network_Registration_URC
   * or creg_n::Network_Registration_and_Location_Information_URC, reports will be generated. This handler
   * will be called when that happens, so that you can handle the event.
   *
   * Note: This will happen asynchronously, so don't forget to call the OwlModem->handleRx() method on an interrupt
   * timer.
   *
   * @param cb
   */
  void setHandlerNetworkRegistrationURC(OwlModem_NetworkRegistrationStatusChangeHandler_f cb);


  /**
   * Retrieve the current GPRS Registration Status
   * @param out_n - output Unsolicited Result Code status
   * @param out_stat - output network registration status
   * @param out_lac - output Local/Tracking Area Code
   * @param out_ci - output Cell Identifier, or 0xFFFFFFFFu indicates the current value is invalid
   * @param out_act - output Radio Access Technology (AcT)
   * @param out_rac - output Routing Area Code
   * @return 1 on success, 0 on failure
   */
  int getGPRSRegistrationStatus(cgreg_n *out_n, cgreg_stat *out_stat, uint16_t *out_lac, uint32_t *out_ci,
                                cgreg_act *out_act, uint8_t *out_rac);

  /**
   * Set the current style of Unregistered Response Code (URC) asynchronous reporting for GPRS Registration Status.
   * @param n - mode to set
   * @return 1 on success, 0 on failure
   */
  int setGPRSRegistrationURC(cgreg_n n);

  /**
   * When setting the GPRS Registration Unsolicited Response Code to creg_n::GPRS_Registration_URC
   * or creg_n::GPRS_Registration_and_Location_Information_URC, reports will be generated. This handler
   * will be called when that happens, so that you can handle the event.
   *
   * Note: This will happen asynchronously, so don't forget to call the OwlModem->handleRx() method on an interrupt
   * timer.
   *
   * @param cb
   */
  void setHandlerGPRSRegistrationURC(OwlModem_GPRSRegistrationStatusChangeHandler_f cb);


  /**
   * Retrieve the current EPS Registration Status
   * @param out_n - output Unsolicited Result Code status
   * @param out_stat - output network registration status
   * @param out_lac - output Local/Tracking Area Code
   * @param out_ci - output Cell Identifier, or 0xFFFFFFFFu indicates the current value is invalid
   * @param out_act - output Radio Access Technology (AcT)
   * @param out_cause_type - output cause type for reject cause
   * @param out_reject_cause - output reject cause
   * @return 1 on success, 0 on failure
   */
  int getEPSRegistrationStatus(cereg_n *out_n, cereg_stat *out_stat, uint16_t *out_lac, uint32_t *out_ci,
                               cereg_act *out_act, cereg_cause_type *out_cause_type, uint32_t *out_reject_cause);

  /**
   * Set the current style of Unregistered Response Code (URC) asynchronous reporting for EPS Registration Status.
   * @param n - mode to set
   * @return 1 on success, 0 on failure
   */
  int setEPSRegistrationURC(cereg_n n);

  /**
   * When setting the EPS Registration Unsolicited Response Code to creg_n::EPS_Registration_URC
   * or creg_n::EPS_Registration_and_Location_Information_URC, reports will be generated. This handler
   * will be called when that happens, so that you can handle the event.
   *
   * Note: This will happen asynchronously, so don't forget to call the OwlModem->handleRx() method on an interrupt
   * timer.
   *
   * @param cb
   */
  void setHandlerEPSRegistrationURC(OwlModem_EPSRegistrationStatusChangeHandler_f cb);


  /**
   * Retrieve the Signal Quality at the moment
   * @param out_rssi - output RSSI - see csq_rssi for special values
   * @param out_qual - output Signal Quality (aka channel Bit Error Rate)
   * @return 1 on success, 0 on failure
   */
  int getSignalQuality(csq_rssi *out_rssi, csq_qual *out_qual);


  /**
   * Set the current eDRX mode.  Please note this persists in NVM between module restarts.
   * @param n - mode to set
   * @param t - (optional) technology (NB or M1); if unspecified, use currently set module technology type
   * @param v - (optional) requested eDRX value (ignored for disabling modes)
   * @return 1 on success, 0 on failure
   */
  int setEDRXMode(edrx_mode n, edrx_act t = edrx_act::Unspecified,
                  edrx_cycle_length v = edrx_cycle_length::Unspecified);

  /**
   * When setting the eDRX mode to AT_EDRX_Mode__Enabled_With_URC, reports will be generated.  These
   * can indicate the actual effective cycle length as well as the network assigned Paging Time Window
   * if they are passed by the network (not always the case). This handler will be called when that
   * happens, so that you can handle the event.
   *
   * Note: This will happen asynchronously, so don't forget to call the OwlModem->handleRx() method on an interrupt
   * timer.
   *
   * @param cb
   */
  void setHandlerEDRXURC(OwlModem_EDRXStatusChangeHandler_f cb);

  /**
   * Set the current PSM mode.  Please note this persists in NVM between module restarts.
   *
   * Since this affects the module soon after startup, you may not be able to communicate with
   * the module during sleep periods.  To un-set this, restart the module and immediately call
   * this to disable or issue the AT command "AT+CPSMS=0"
   * @param n - mode to set
   * @param pt_interval - (optional, unless specifying Active Time Interval) requested periodic TAU interval
   * @param pt - (optional, unless specifying Active Time Interval) requested periodic TAU (only values from 0 to 31 are
   * valid)
   * @param at_interval - (optional) requested active time interval
   * @param at - (optional) requested active time (only values from 0 to 31 are valid)
   * @return 1 on success, 0 on failure
   */
  int setPSMMode(psm_mode n, psm_tau_interval pt_interval = psm_tau_interval::Timer_Unspecified, uint8_t pt = 0,
                 psm_active_time_interval at_interval = psm_active_time_interval::Timer_Unspecified, uint8_t at = 0);


 private:
  OwlModemAT *atModem_ = 0;

  str network_response = {.s = nullptr, .len = 0};

  OwlModem_NetworkRegistrationStatusChangeHandler_f handler_creg = 0;
  OwlModem_GPRSRegistrationStatusChangeHandler_f handler_cgreg   = 0;
  OwlModem_EPSRegistrationStatusChangeHandler_f handler_cereg    = 0;
  OwlModem_EDRXStatusChangeHandler_f handler_edrx                = 0;

  bool processURCNetworkRegistration(str urc, str data);
  bool processURCGPRSRegistration(str urc, str data);
  bool processURCEPSRegistration(str urc, str data);
  bool processURCEDRXResult(str urc, str data);


  void parseNetworkRegistrationStatus(str response, creg_n *out_n, creg_stat *out_stat, uint16_t *out_lac,
                                      uint32_t *out_ci, creg_act *out_act);
  void parseGPRSRegistrationStatus(str response, cgreg_n *out_n, cgreg_stat *out_stat, uint16_t *out_lac,
                                   uint32_t *out_ci, cgreg_act *out_act, uint8_t *out_rac);
  void parseEPSRegistrationStatus(str response, cereg_n *out_n, cereg_stat *out_stat, uint16_t *out_lac,
                                  uint32_t *out_ci, cereg_act *out_act, cereg_cause_type *out_cause_type,
                                  uint32_t *out_reject_cause);
  void parseEDRXStatus(str response, edrx_act *out_network, edrx_cycle_length *out_requested_value,
                       edrx_cycle_length *out_provided_value, edrx_paging_time_window *out_paging_time_window);

  typedef struct {
    creg_n n;
    creg_stat stat;
    uint16_t lac;
    uint32_t ci;
    creg_act act;
  } owl_network_status_t;

  owl_network_status_t last_network_status = {
      .n    = creg_n::URC_Disabled,
      .stat = creg_stat::Not_Registered,
      .lac  = 0,
      .ci   = 0xFFFFFFFFu,
      .act  = creg_act::invalid,
  };

  typedef struct {
    cgreg_n n;
    cgreg_stat stat;
    uint16_t lac;
    uint32_t ci;
    cgreg_act act;
    uint8_t rac;
  } owl_last_gprs_status_t;

  owl_last_gprs_status_t last_gprs_status = {
      .n    = cgreg_n::URC_Disabled,
      .stat = cgreg_stat::Not_Registered,
      .lac  = 0,
      .ci   = 0xFFFFFFFFu,
      .act  = cgreg_act::invalid,
      .rac  = 0,
  };

  typedef struct {
    cereg_n n;
    cereg_stat stat;
    uint16_t lac;
    uint32_t ci;
    cereg_act act;
    cereg_cause_type cause_type;
    uint32_t reject_cause;
  } owl_last_eps_status_t;

  owl_last_eps_status_t last_eps_status = {
      .n            = cereg_n::URC_Disabled,
      .stat         = cereg_stat::Not_Registered,
      .lac          = 0,
      .ci           = 0xFFFFFFFFu,
      .act          = cereg_act::invalid,
      .cause_type   = cereg_cause_type::EMM_Cause,
      .reject_cause = 0,
  };

  typedef struct {
    edrx_act network;
    edrx_cycle_length requested_value;
    edrx_cycle_length provided_value;
    edrx_paging_time_window paging_time_window;
  } owl_edrx_status_t;

  owl_edrx_status_t last_edrx_status = {
      .network            = edrx_act::Unspecified,
      .requested_value    = edrx_cycle_length::Unspecified,
      .provided_value     = edrx_cycle_length::Unspecified,
      .paging_time_window = edrx_paging_time_window::Unspecified,
  };
};

#endif
