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
typedef void (*OwlModem_NetworkRegistrationStatusChangeHandler_f)(at_creg_stat_e stat, uint16_t lac, uint32_t ci,
                                                                  at_creg_act_e act);

/**
 * Handler function signature for GPRS Registration events (2G/3G)
 * @param stat - status
 * @param lac - local area code
 * @param ci - cell id
 * @param act - access technology
 * @param rac - routing area code
 */
typedef void (*OwlModem_GPRSRegistrationStatusChangeHandler_f)(at_cgreg_stat_e stat, uint16_t lac, uint32_t ci,
                                                               at_cgreg_act_e act, uint8_t rac);

/**
 * Handler function signature for EPS Registration events (EPC - 2G/3G/LTE/WiFi/etc)
 * @param stat - status
 * @param lac - local area code
 * @param ci - cell id
 * @param act - access technology
 * @param cause_type
 * @param reject_cause
 */
typedef void (*OwlModem_EPSRegistrationStatusChangeHandler_f)(at_cereg_stat_e stat, uint16_t lac, uint32_t ci,
                                                              at_cereg_act_e act, at_cereg_cause_type_e cause_type,
                                                              uint32_t reject_cause);

/**
 * Handler function signature for eDRX change events
 * @param network - Network technology
 * @param requested_value - Requested eDRX cycle length
 * @param provided_value - Network provided eDRX cycle length
 * @param paging_time_window - Network provided Paging Time Window
 */
typedef void (*OwlModem_EDRXStatusChangeHandler_f)(at_edrx_access_technology_e network,
                                                   at_edrx_cycle_length_e requested_value,
                                                   at_edrx_cycle_length_e provided_value,
                                                   at_edrx_paging_time_window_e paging_time_window);


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
   * @param out_stk_mode - SIM-Toolkit mode
   * @return 1 on success, 0 on failure
   */
  int getModemFunctionality(at_cfun_power_mode_e *out_power_mode, at_cfun_stk_mode_e *out_stk_mode);

  /**
   * Set the modem functionality mode.
   * @param mode - mode to set
   * @return 1 on success, 0 on failure
   */
  int setModemFunctionality(at_cfun_fun_e fun, at_cfun_rst_e *reset);

  /**
   * Retrieve the current Operator Selection Mode and selected Operator, Radio Access Technology
   * @param out_mode - output current mode
   * @param out_format - output format of the following operator string
   * @param out_oper - output operator string
   * @param max_oper_len - buffer length provided in the operator string
   * @param out_act - output radio access technology type
   * @return 1 on success, 0 on failure
   */
  int getOperatorSelection(at_cops_mode_e *out_mode, at_cops_format_e *out_format, str_mut *out_oper,
                           unsigned int max_oper_len, at_cops_act_e *out_act);

  /**
   * Set operator selection mode and/or select manually operator/access technology
   * @param mode - new mode
   * @param opt_format - optional format (to set output and/or indicate what format the next operator string parameter
   * is in
   * @param opt_oper - optional operator string - must also provide format parameter with this
   * @param opt_act - optional radio access technology code - must also provide format and oper parameters with this
   * @return 1 on success, 0 on failure
   */
  int setOperatorSelection(at_cops_mode_e mode, at_cops_format_e *opt_format, str *opt_oper, at_cops_act_e *opt_act);

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
  int getNetworkRegistrationStatus(at_creg_n_e *out_n, at_creg_stat_e *out_stat, uint16_t *out_lac, uint32_t *out_ci,
                                   at_creg_act_e *out_act);

  /**
   * Set the current style of Unsolicited Response Code (URC) asynchronous reporting for Network Registration Status.
   * @param n - mode to set
   * @return 1 on success, 0 on failure
   */
  int setNetworkRegistrationURC(at_creg_n_e n);

  /**
   * When setting the Network Registration Unsolicited Response Code to AT_CREG__N__Network_Registration_URC
   * or AT_CREG__N__Network_Registration_and_Location_Information_URC, reports will be generated. This handler
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
  int getGPRSRegistrationStatus(at_cgreg_n_e *out_n, at_cgreg_stat_e *out_stat, uint16_t *out_lac, uint32_t *out_ci,
                                at_cgreg_act_e *out_act, uint8_t *out_rac);

  /**
   * Set the current style of Unregistered Response Code (URC) asynchronous reporting for GPRS Registration Status.
   * @param n - mode to set
   * @return 1 on success, 0 on failure
   */
  int setGPRSRegistrationURC(at_cgreg_n_e n);

  /**
   * When setting the GPRS Registration Unsolicited Response Code to AT_CREG__N__GPRS_Registration_URC
   * or AT_CREG__N__GPRS_Registration_and_Location_Information_URC, reports will be generated. This handler
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
  int getEPSRegistrationStatus(at_cereg_n_e *out_n, at_cereg_stat_e *out_stat, uint16_t *out_lac, uint32_t *out_ci,
                               at_cereg_act_e *out_act, at_cereg_cause_type_e *out_cause_type,
                               uint32_t *out_reject_cause);

  /**
   * Set the current style of Unregistered Response Code (URC) asynchronous reporting for EPS Registration Status.
   * @param n - mode to set
   * @return 1 on success, 0 on failure
   */
  int setEPSRegistrationURC(at_cereg_n_e n);

  /**
   * When setting the EPS Registration Unsolicited Response Code to AT_CREG__N__EPS_Registration_URC
   * or AT_CREG__N__EPS_Registration_and_Location_Information_URC, reports will be generated. This handler
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
   * @param out_rssi - output RSSI - see at_csq_rssi_e for special values
   * @param out_qual - output Signal Quality (aka channel Bit Error Rate)
   * @return 1 on success, 0 on failure
   */
  int getSignalQuality(at_csq_rssi_e *out_rssi, at_csq_qual_e *out_qual);


  /**
   * Set the current eDRX mode.  Please note this persists in NVM between module restarts.
   * @param n - mode to set
   * @param t - (optional) technology (NB or M1); if unspecified, use currently set module technology type
   * @param v - (optional) requested eDRX value (ignored for disabling modes)
   * @return 1 on success, 0 on failure
   */
  int setEDRXMode(at_edrx_mode_e n, at_edrx_access_technology_e t = AT_EDRX_Access_Technology__Unspecified,
                  at_edrx_cycle_length_e v = AT_EDRX_Cycle_Length__Unspecified);

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
  int setPSMMode(at_psm_mode_e n, at_psm_tau_interval pt_interval = AT_PSM_TAU_Interval__Timer_Unspecified,
                 uint8_t pt                              = 0,
                 at_psm_active_time_interval at_interval = AT_PSM_Active_Time_Interval__Timer_Unspecified,
                 uint8_t at                              = 0);


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


  void parseNetworkRegistrationStatus(str response, at_creg_n_e *out_n, at_creg_stat_e *out_stat, uint16_t *out_lac,
                                      uint32_t *out_ci, at_creg_act_e *out_act);
  void parseGPRSRegistrationStatus(str response, at_cgreg_n_e *out_n, at_cgreg_stat_e *out_stat, uint16_t *out_lac,
                                   uint32_t *out_ci, at_cgreg_act_e *out_act, uint8_t *out_rac);
  void parseEPSRegistrationStatus(str response, at_cereg_n_e *out_n, at_cereg_stat_e *out_stat, uint16_t *out_lac,
                                  uint32_t *out_ci, at_cereg_act_e *out_act, at_cereg_cause_type_e *out_cause_type,
                                  uint32_t *out_reject_cause);
  void parseEDRXStatus(str response, at_edrx_access_technology_e *out_network,
                       at_edrx_cycle_length_e *out_requested_value, at_edrx_cycle_length_e *out_provided_value,
                       at_edrx_paging_time_window_e *out_paging_time_window);

  typedef struct {
    at_creg_n_e n;
    at_creg_stat_e stat;
    uint16_t lac;
    uint32_t ci;
    at_creg_act_e act;
  } owl_network_status_t;

  owl_network_status_t last_network_status = {
      .n    = AT_CREG__N__URC_Disabled,
      .stat = AT_CREG__Stat__Not_Registered,
      .lac  = 0,
      .ci   = 0xFFFFFFFFu,
      .act  = AT_CREG__AcT__invalid,
  };

  typedef struct {
    at_cgreg_n_e n;
    at_cgreg_stat_e stat;
    uint16_t lac;
    uint32_t ci;
    at_cgreg_act_e act;
    uint8_t rac;
  } owl_last_gprs_status_t;

  owl_last_gprs_status_t last_gprs_status = {
      .n    = AT_CGREG__N__URC_Disabled,
      .stat = AT_CGREG__Stat__Not_Registered,
      .lac  = 0,
      .ci   = 0xFFFFFFFFu,
      .act  = AT_CGREG__AcT__invalid,
      .rac  = 0,
  };

  typedef struct {
    at_cereg_n_e n;
    at_cereg_stat_e stat;
    uint16_t lac;
    uint32_t ci;
    at_cereg_act_e act;
    at_cereg_cause_type_e cause_type;
    uint32_t reject_cause;
  } owl_last_eps_status_t;

  owl_last_eps_status_t last_eps_status = {
      .n            = AT_CEREG__N__URC_Disabled,
      .stat         = AT_CEREG__Stat__Not_Registered,
      .lac          = 0,
      .ci           = 0xFFFFFFFFu,
      .act          = AT_CEREG__AcT__invalid,
      .cause_type   = AT_CEREG__Cause_Type__EMM_Cause,
      .reject_cause = 0,
  };

  typedef struct {
    at_edrx_access_technology_e network;
    at_edrx_cycle_length_e requested_value;
    at_edrx_cycle_length_e provided_value;
    at_edrx_paging_time_window_e paging_time_window;
  } owl_edrx_status_t;

  owl_edrx_status_t last_edrx_status = {
      .network            = AT_EDRX_Access_Technology__Unspecified,
      .requested_value    = AT_EDRX_Cycle_Length__Unspecified,
      .provided_value     = AT_EDRX_Cycle_Length__Unspecified,
      .paging_time_window = AT_EDRX_Paging_Time_Window__Unspecified,
  };
};

#endif
