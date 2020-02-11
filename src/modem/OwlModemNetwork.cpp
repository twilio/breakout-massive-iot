/*
 * OwlModemNetwork.cpp
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
 * \file OwlModemNetwork.cpp - API for Network Registration Management
 */

#include "OwlModemNetwork.h"

#include <stdio.h>


static char URC_ID[] = "Network";

OwlModemNetwork::OwlModemNetwork(OwlModemAT *atModem) : atModem_(atModem) {
  if (atModem_ != nullptr) {
    atModem_->registerUrcHandler(URC_ID, OwlModemNetwork::processURC, this);
  }
}

static str s_creg      = STRDECL("+CREG");
static str s_creg_full = STRDECL("+CREG: ");

void OwlModemNetwork::parseNetworkRegistrationStatus(str response, creg_n *out_n, creg_stat *out_stat,
                                                     uint16_t *out_lac, uint32_t *out_ci, creg_act *out_act) {
  int cnt   = 0;
  str token = {0};
  if (out_n) *out_n = creg_n::URC_Disabled;
  if (out_stat) *out_stat = creg_stat::Not_Registered;
  if (out_lac) *out_lac = 0;
  if (out_ci) *out_ci = 0xFFFFFFFFu;
  if (out_act) *out_act = creg_act::invalid;

  str data = response;
  str_skipover_prefix(&data, s_creg_full);
  int has_n = 1;
  while (str_tok(data, ",", &token)) {
    if (cnt == 1) {
      if (token.len && token.s[0] == '\"') has_n = 0;
      break;
    }
    cnt++;
  }
  token = {0};
  if (has_n)
    cnt = 0;
  else
    cnt = 1;
  while (str_tok(data, ",", &token)) {
    switch (cnt) {
      case 0:
        if (out_n) *out_n = (creg_n)str_to_long_int(token, 10);
        break;
      case 1:
        if (out_stat) *out_stat = (creg_stat)str_to_long_int(token, 10);
        break;
      case 2:
        if (out_lac) *out_lac = (uint16_t)str_to_long_int(token, 16);
        break;
      case 3:
        if (out_ci) *out_ci = (uint32_t)str_to_uint32_t(token, 16);
        break;
      case 4:
        if (out_act) *out_act = (creg_act)str_to_long_int(token, 16);
        break;
      default:
        LOG(L_ERR, "Not handled %d(-th) token [%.*s] data was [%.*s]\r\n", cnt, token.len, token.s, data.len, data.s);
    }
    cnt++;
  }
}

bool OwlModemNetwork::processURCNetworkRegistration(str urc, str data) {
  if (!str_equal(urc, s_creg)) {
    return false;
  }
  this->parseNetworkRegistrationStatus(data, &last_network_status.n, &last_network_status.stat,
                                       &last_network_status.lac, &last_network_status.ci, &last_network_status.act);

  if (!this->handler_creg) {
    LOG(L_INFO,
        "Received URC for CREG [%.*s]. Set a handler with setHandlerNetworkRegistrationURC() if you wish to "
        "receive this event in your application\r\n",
        data.len, data.s);
  } else {
    (this->handler_creg)(last_network_status.stat, last_network_status.lac, last_network_status.ci,
                         last_network_status.act);
  }
  return true;
}



static str s_cgreg      = STRDECL("+CGREG");
static str s_cgreg_full = STRDECL("+CGREG: ");

void OwlModemNetwork::parseGPRSRegistrationStatus(str response, cgreg_n *out_n, cgreg_stat *out_stat, uint16_t *out_lac,
                                                  uint32_t *out_ci, cgreg_act *out_act, uint8_t *out_rac) {
  int cnt   = 0;
  str token = {0};
  if (out_n) *out_n = cgreg_n::URC_Disabled;
  if (out_stat) *out_stat = cgreg_stat::Not_Registered;
  if (out_lac) *out_lac = 0;
  if (out_ci) *out_ci = 0xFFFFFFFFu;
  if (out_act) *out_act = cgreg_act::invalid;
  if (out_rac) *out_rac = 0;

  str data = response;
  str_skipover_prefix(&data, s_cgreg_full);
  int has_n = 1;
  while (str_tok(data, ",", &token)) {
    if (cnt == 1) {
      if (token.len && token.s[0] == '\"') has_n = 0;
      break;
    }
    cnt++;
  }
  token = {0};
  if (has_n)
    cnt = 0;
  else
    cnt = 1;
  while (str_tok(data, ",", &token)) {
    switch (cnt) {
      case 0:
        if (out_n) *out_n = (cgreg_n)str_to_long_int(token, 10);
        break;
      case 1:
        if (out_stat) *out_stat = (cgreg_stat)str_to_long_int(token, 10);
        break;
      case 2:
        if (out_lac) *out_lac = (uint16_t)str_to_long_int(token, 16);
        break;
      case 3:
        if (out_ci) *out_ci = (uint32_t)str_to_uint32_t(token, 16);
        break;
      case 4:
        if (out_act) *out_act = (cgreg_act)str_to_long_int(token, 10);
        break;
      case 5:
        if (out_rac) *out_rac = (uint8_t)str_to_long_int(token, 16);
        break;
      default:
        LOG(L_ERR, "Not handled %d(-th) token [%.*s] data was [%.*s]\r\n", cnt, token.len, token.s, data.len, data.s);
    }
    cnt++;
  }
}

bool OwlModemNetwork::processURCGPRSRegistration(str urc, str data) {
  if (!str_equal(urc, s_cgreg)) {
    return false;
  }
  this->parseGPRSRegistrationStatus(data, &last_gprs_status.n, &last_gprs_status.stat, &last_gprs_status.lac,
                                    &last_gprs_status.ci, &last_gprs_status.act, &last_gprs_status.rac);
  if (!this->handler_cgreg) {
    LOG(L_INFO,
        "Received URC for CGREG [%.*s]. Set a handler with setHandlerGPRSRegistrationURC() if you wish to "
        "receive this event in your application\r\n",
        data.len, data.s);
  } else {
    (this->handler_cgreg)(last_gprs_status.stat, last_gprs_status.lac, last_gprs_status.ci, last_gprs_status.act,
                          last_gprs_status.rac);
  }
  return true;
}



static str s_cereg      = STRDECL("+CEREG");
static str s_cereg_full = STRDECL("+CEREG: ");

void OwlModemNetwork::parseEPSRegistrationStatus(str response, cereg_n *out_n, cereg_stat *out_stat, uint16_t *out_lac,
                                                 uint32_t *out_ci, cereg_act *out_act, cereg_cause_type *out_cause_type,
                                                 uint32_t *out_reject_cause) {
  int cnt   = 0;
  str token = {0};
  if (out_n) *out_n = cereg_n::URC_Disabled;
  if (out_stat) *out_stat = cereg_stat::Not_Registered;
  if (out_lac) *out_lac = 0;
  if (out_ci) *out_ci = 0xFFFFFFFFu;
  if (out_act) *out_act = cereg_act::invalid;
  if (out_cause_type) *out_cause_type = cereg_cause_type::EMM_Cause;
  if (out_reject_cause) *out_reject_cause = 0;

  str data = response;
  str_skipover_prefix(&data, s_cereg_full);
  int has_n = 1;
  while (str_tok(data, ",", &token)) {
    if (cnt == 1) {
      if (token.len && token.s[0] == '\"') has_n = 0;
      break;
    }
    cnt++;
  }
  token = {0};
  if (has_n)
    cnt = 0;
  else
    cnt = 1;
  while (str_tok(data, ",", &token)) {
    switch (cnt) {
      case 0:
        if (out_n) *out_n = (cereg_n)str_to_long_int(token, 10);
        break;
      case 1:
        if (out_stat) *out_stat = (cereg_stat)str_to_long_int(token, 10);
        break;
      case 2:
        if (out_lac) *out_lac = (uint16_t)str_to_long_int(token, 16);
        break;
      case 3:
        if (out_ci) *out_ci = (uint32_t)str_to_uint32_t(token, 16);
        break;
      case 4:
        if (out_act) *out_act = (cereg_act)str_to_long_int(token, 10);
        break;
      case 5:
        if (out_cause_type) *out_cause_type = (cereg_cause_type)str_to_long_int(token, 10);
        break;
      case 6:
        if (out_reject_cause) *out_reject_cause = (uint32_t)str_to_uint32_t(token, 10);
        break;
      default:
        LOG(L_ERR, "Not handled %d(-th) token [%.*s] data was [%.*s]\r\n", cnt, token.len, token.s, data.len, data.s);
    }
    cnt++;
  }
}

bool OwlModemNetwork::processURCEPSRegistration(str urc, str data) {
  if (!str_equal(urc, s_cereg)) {
    return false;
  }

  this->parseEPSRegistrationStatus(data, &last_eps_status.n, &last_eps_status.stat, &last_eps_status.lac,
                                   &last_eps_status.ci, &last_eps_status.act, &last_eps_status.cause_type,
                                   &last_eps_status.reject_cause);
  if (!this->handler_cereg) {
    LOG(L_INFO,
        "Received URC for CEREG [%.*s]. Set a handler with setHandlerEPSRegistrationURC() if you wish to "
        "receive this event in your application\r\n",
        data.len, data.s);
  } else {
    (this->handler_cereg)(last_eps_status.stat, last_eps_status.lac, last_eps_status.ci, last_eps_status.act,
                          last_eps_status.cause_type, last_eps_status.reject_cause);
  }
  return true;
}



static str s_edrx      = STRDECL("+CEDRXP");
static str s_edrx_full = STRDECL("+CEDRXP: ");

void OwlModemNetwork::parseEDRXStatus(str response, edrx_act *out_network, edrx_cycle_length *out_requested_value,
                                      edrx_cycle_length *out_provided_value,
                                      edrx_paging_time_window *out_paging_time_window) {
  int cnt   = 0;
  str token = {0};
  if (out_network) *out_network = edrx_act::Unspecified;
  if (out_requested_value) *out_requested_value = edrx_cycle_length::Unspecified;
  if (out_provided_value) *out_provided_value = edrx_cycle_length::Unspecified;
  if (out_paging_time_window) *out_paging_time_window = edrx_paging_time_window::Unspecified;

  str data = response;
  str_skipover_prefix(&data, s_edrx_full);
  while (str_tok(data, ",", &token)) {
    switch (cnt) {
      case 0:
        if (out_network) *out_network = (edrx_act)str_to_long_int(token, 10);
        break;
      case 1:
        if (out_requested_value) *out_requested_value = (edrx_cycle_length)str_to_uint32_t(token, 2);
        break;
      case 2:
        if (out_provided_value) *out_provided_value = (edrx_cycle_length)str_to_uint32_t(token, 2);
        break;
      case 3:
        if (out_paging_time_window) *out_paging_time_window = (edrx_paging_time_window)str_to_uint32_t(token, 2);
        break;
      default:
        LOG(L_ERR, "Not handled %d(-th) token [%.*s] data was [%.*s]\r\n", cnt, token.len, token.s, data.len, data.s);
    }
    cnt++;
  }
}

bool OwlModemNetwork::processURCEDRXResult(str urc, str data) {
  if (!str_equal(urc, s_edrx)) {
    return false;
  }

  this->parseEDRXStatus(data, &last_edrx_status.network, &last_edrx_status.requested_value,
                        &last_edrx_status.provided_value, &last_edrx_status.paging_time_window);

  if (!this->handler_edrx) {
    LOG(L_INFO,
        "Received URC for eDRX [%.*s]. Set a handler with setHandlerEDRXURC() if you wish to "
        "receive this event in your application\r\n",
        data.len, data.s);
  } else {
    (this->handler_edrx)(last_edrx_status.network, last_edrx_status.requested_value, last_edrx_status.provided_value,
                         last_edrx_status.paging_time_window);
  }

  return true;
}



bool OwlModemNetwork::processURC(str urc, str data, void *instance) {
  OwlModemNetwork *inst = reinterpret_cast<OwlModemNetwork *>(instance);

  return inst->processURCNetworkRegistration(urc, data) || inst->processURCGPRSRegistration(urc, data) ||
         inst->processURCEPSRegistration(urc, data) || inst->processURCEDRXResult(urc, data);
}



static str s_cfun = STRDECL("+CFUN: ");

int OwlModemNetwork::getModemFunctionality(cfun_power_mode *out_power_mode) {
  if (out_power_mode) *out_power_mode = cfun_power_mode::Minimum_Functionality;
  int result = atModem_->doCommandBlocking("AT+CFUN?", 15 * 1000, &network_response) == at_result_code::OK;
  if (!result) return 0;
  OwlModemAT::filterResponse(s_cfun, network_response, &network_response);
  str token = {0};
  int cnt   = 0;
  while (str_tok(network_response, ",\r\n", &token)) {
    switch (cnt) {
      case 0:
        if (out_power_mode) *out_power_mode = (cfun_power_mode)str_to_long_int(token, 10);
        break;
      case 1:
        break;
      default:
        LOG(L_ERR, "Not handled %d(-th) token [%.*s]\r\n", cnt, token.len, token.s);
    }
    cnt++;
  }
  return 1;
}

int OwlModemNetwork::setModemFunctionality(cfun_fun fun, cfun_rst *reset) {
  if (!reset)
    atModem_->commandSprintf("AT+CFUN=%d", fun);
  else
    atModem_->commandSprintf("AT+CFUN=%d,%d", fun, *reset);
  return atModem_->doCommandBlocking(3 * 60 * 1000, nullptr) == at_result_code::OK;
}

static str s_cops = STRDECL("+COPS: ");

int OwlModemNetwork::getOperatorSelection(cops_mode *out_mode, cops_format *out_format, str_mut *out_oper,
                                          unsigned int max_oper_len, cops_act *out_act) {
  int cnt   = 0;
  str token = {0};
  if (out_mode) *out_mode = cops_mode::Automatic_Selection;
  if (out_format) *out_format = cops_format::Long_Alphanumeric;
  if (out_oper) out_oper->len = 0;
  if (out_act) *out_act = (cops_act)0;
  int result = atModem_->doCommandBlocking("AT+COPS?", 3 * 60 * 1000, &network_response) == at_result_code::OK;
  if (!result) return 0;
  OwlModemAT::filterResponse(s_cops, network_response, &network_response);
  while (str_tok(network_response, ",\r\n", &token)) {
    switch (cnt) {
      case 0:
        if (out_mode) *out_mode = (cops_mode)str_to_long_int(token, 10);
        break;
      case 1:
        if (out_format) *out_format = (cops_format)str_to_long_int(token, 10);
        break;
      case 2:
        if (out_oper) {
          memcpy(out_oper->s, token.s, token.len > max_oper_len ? max_oper_len : token.len);
          out_oper->len = token.len;
        }
        break;
      case 3:
        if (out_act) *out_act = (cops_act)str_to_long_int(token, 10);
        break;
      default:
        LOG(L_ERR, "Not handled %d(-th) token [%.*s]\r\n", cnt, token.len, token.s);
    }
    cnt++;
  }
  return 1;
}

int OwlModemNetwork::setOperatorSelection(cops_mode mode, cops_format *opt_format, str *opt_oper, cops_act *opt_act) {
  if (opt_oper && !opt_format) {
    LOG(L_ERR, " - when opt_oper is specific, opt_format must be also specified\r\n");
    return 0;
  }
  if (opt_act && !opt_oper) {
    LOG(L_ERR, " - when opt_act is specific, opt_format and opt_oper must be also specified\r\n");
    return 0;
  }
  if (opt_oper) {
    if (opt_act) {
      atModem_->commandSprintf("AT+COPS=%d,%d,\"%.*s\",%d", mode, *opt_format, opt_oper->len, opt_oper->s, *opt_act);
    } else {
      atModem_->commandSprintf("AT+COPS=%d,%d,\"%.*s\"", mode, *opt_format, opt_oper->len, opt_oper->s);
    }
  } else {
    atModem_->commandSprintf("AT+COPS=%d", mode);
  }
  return atModem_->doCommandBlocking(3 * 60 * 1000, nullptr) == at_result_code::OK;
}

int OwlModemNetwork::getOperatorList(str *out_response) {
  str command_response;
  int result = atModem_->doCommandBlocking("AT+COPS=?", 3 * 60 * 1000, &command_response) == at_result_code::OK;
  if (!result) return 0;
  OwlModemAT::filterResponse(s_cops, command_response, out_response);
  return 1;
}



int OwlModemNetwork::getNetworkRegistrationStatus(creg_n *out_n, creg_stat *out_stat, uint16_t *out_lac,
                                                  uint32_t *out_ci, creg_act *out_act) {
  if (out_n) *out_n = creg_n::URC_Disabled;
  if (out_stat) *out_stat = creg_stat::Not_Registered;
  if (out_lac) *out_lac = 0;
  if (out_ci) *out_ci = 0xFFFFFFFFu;
  if (out_act) *out_act = creg_act::invalid;
  int result = atModem_->doCommandBlocking("AT+CREG?", 1000, nullptr) == at_result_code::OK;
  if (!result) return 0;
  // the URC handlers are catching this, so serving from local cache
  if (out_n) *out_n = last_network_status.n;
  if (out_stat) *out_stat = last_network_status.stat;
  if (out_lac) *out_lac = last_network_status.lac;
  if (out_ci) *out_ci = last_network_status.ci;
  if (out_act) *out_act = last_network_status.act;
  return 1;
}

int OwlModemNetwork::setNetworkRegistrationURC(creg_n n) {
  atModem_->commandSprintf("AT+CREG=%d", n);
  return atModem_->doCommandBlocking(180 * 1000, nullptr) == at_result_code::OK;
}

void OwlModemNetwork::setHandlerNetworkRegistrationURC(OwlModem_NetworkRegistrationStatusChangeHandler_f cb) {
  this->handler_creg = cb;
}


int OwlModemNetwork::getGPRSRegistrationStatus(cgreg_n *out_n, cgreg_stat *out_stat, uint16_t *out_lac,
                                               uint32_t *out_ci, cgreg_act *out_act, uint8_t *out_rac) {
  if (out_n) *out_n = cgreg_n::URC_Disabled;
  if (out_stat) *out_stat = cgreg_stat::Not_Registered;
  if (out_lac) *out_lac = 0;
  if (out_ci) *out_ci = 0xFFFFFFFFu;
  if (out_act) *out_act = cgreg_act::invalid;
  if (out_rac) *out_rac = 0;
  int result = atModem_->doCommandBlocking("AT+CGREG?", 1000, nullptr) == at_result_code::OK;
  if (!result) return 0;
  // the URC handlers are catching this, so serving from local cache
  if (out_n) *out_n = last_gprs_status.n;
  if (out_stat) *out_stat = last_gprs_status.stat;
  if (out_lac) *out_lac = last_gprs_status.lac;
  if (out_ci) *out_ci = last_gprs_status.ci;
  if (out_act) *out_act = last_gprs_status.act;
  if (out_act) *out_rac = last_gprs_status.rac;
  return 1;
}

int OwlModemNetwork::setGPRSRegistrationURC(cgreg_n n) {
  atModem_->commandSprintf("AT+CGREG=%d", n);
  return atModem_->doCommandBlocking(180 * 1000, nullptr) == at_result_code::OK;
}

void OwlModemNetwork::setHandlerGPRSRegistrationURC(OwlModem_GPRSRegistrationStatusChangeHandler_f cb) {
  this->handler_cgreg = cb;
}

int OwlModemNetwork::getEPSRegistrationStatus(cereg_n *out_n, cereg_stat *out_stat, uint16_t *out_lac, uint32_t *out_ci,
                                              cereg_act *out_act, cereg_cause_type *out_cause_type,
                                              uint32_t *out_reject_cause) {
  if (out_n) *out_n = cereg_n::URC_Disabled;
  if (out_stat) *out_stat = cereg_stat::Not_Registered;
  if (out_lac) *out_lac = 0;
  if (out_ci) *out_ci = 0xFFFFFFFFu;
  if (out_act) *out_act = cereg_act::invalid;
  if (out_cause_type) *out_cause_type = cereg_cause_type::EMM_Cause;
  if (out_reject_cause) *out_reject_cause = 0;
  int result = atModem_->doCommandBlocking("AT+CEREG?", 1000, nullptr) == at_result_code::OK;
  if (!result) return 0;
  // the URC handlers are catching this, so serving from local cache
  if (out_n) *out_n = last_eps_status.n;
  if (out_stat) *out_stat = last_eps_status.stat;
  if (out_lac) *out_lac = last_eps_status.lac;
  if (out_ci) *out_ci = last_eps_status.ci;
  if (out_act) *out_act = last_eps_status.act;
  if (out_cause_type) *out_cause_type = last_eps_status.cause_type;
  if (out_reject_cause) *out_reject_cause = last_eps_status.reject_cause;
  return 1;
}

int OwlModemNetwork::setEPSRegistrationURC(cereg_n n) {
  atModem_->commandSprintf("AT+CEREG=%d", n);
  return atModem_->doCommandBlocking(180 * 1000, nullptr) == at_result_code::OK;
}

void OwlModemNetwork::setHandlerEPSRegistrationURC(OwlModem_EPSRegistrationStatusChangeHandler_f cb) {
  this->handler_cereg = cb;
}

static str s_csq = STRDECL("+CSQ: ");

int OwlModemNetwork::getSignalQuality(csq_rssi *out_rssi, csq_qual *out_qual) {
  int cnt   = 0;
  str token = {0};
  if (out_rssi) *out_rssi = csq_rssi::Not_Known_or_Detectable_99;
  if (out_qual) *out_qual = csq_qual::Not_Known_or_Not_Detectable;
  int result = atModem_->doCommandBlocking("AT+CSQ", 1000, &network_response) == at_result_code::OK;
  if (!result) return 0;
  OwlModemAT::filterResponse(s_csq, network_response, &network_response);
  while (str_tok(network_response, ",", &token)) {
    switch (cnt) {
      case 0:
        if (out_rssi) *out_rssi = (csq_rssi)str_to_long_int(token, 10);
        break;
      case 1:
        if (out_qual) *out_qual = (csq_qual)str_to_long_int(token, 10);
        break;
      default:
        LOG(L_ERR, "Not handled %d(-th) token [%.*s]\r\n", cnt, token.len, token.s);
    }
    cnt++;
  }
  return 1;
}

int OwlModemNetwork::setEDRXMode(edrx_mode n, edrx_act t, edrx_cycle_length v) {
  if (t == edrx_act::Unspecified) {
    atModem_->commandSprintf("AT+CEDRXS=%d", n);
  } else {
    if ((n == edrx_mode::Enabled || n == edrx_mode::Enabled_With_URC) && (v != edrx_cycle_length::Unspecified)) {
      char v_str_[8];
      str_mut v_str = {.s = v_str_, .len = 0};
      uint8_t_to_binary_str(static_cast<uint8_t>(v), &v_str, 4);
      atModem_->commandSprintf("AT+CEDRXS=%d,%d,\"%.*s\"", n, t, v_str.len, v_str.s);
    } else {
      atModem_->commandSprintf("AT+CEDRXS=%d,%d", n, t);
    }
  }

  return atModem_->doCommandBlocking(180 * 1000, nullptr) == at_result_code::OK;
}

void OwlModemNetwork::setHandlerEDRXURC(OwlModem_EDRXStatusChangeHandler_f cb) {
  this->handler_edrx = cb;
}

int OwlModemNetwork::setPSMMode(psm_mode n, psm_tau_interval pt_interval, uint8_t pt,
                                psm_active_time_interval at_interval, uint8_t at) {
  if (n == psm_mode::Enabled && pt_interval == psm_tau_interval::Timer_Unspecified &&
      at_interval != psm_active_time_interval::Timer_Unspecified) {
    LOG(L_ERR, "PSM enable called with Active Time Interval but no TAU Interval which is invalid.\r\n");
    return 0;
  }

  if (n != psm_mode::Enabled || pt_interval == psm_tau_interval::Timer_Unspecified) {
    atModem_->commandSprintf("AT+CPSMS=%d", n);
  } else {
    uint8_t pt_value = ((uint8_t)pt_interval << 5) | (pt & 0x1F);  // msb 3 bits are interval, lsb 5 bits are value
    char pt_str_[8];
    str_mut pt_str = {.s = pt_str_, .len = 0};
    uint8_t_to_binary_str(pt_value, &pt_str, 8);

    if (at_interval == psm_active_time_interval::Timer_Unspecified) {
      atModem_->commandSprintf("AT+CPSMS=%d,,,\"%.*s\"", n, pt_str.len, pt_str.s);
    } else {
      uint8_t at_value = ((uint8_t)at_interval << 5) | (at & 0x1F);  // msb 3 bits are interval, lsb 5 bits are value
      char at_str_[8];
      str_mut at_str = {.s = at_str_, .len = 0};
      uint8_t_to_binary_str(at_value, &at_str, 8);
      atModem_->commandSprintf("AT+CPSMS=%d,,,\"%.*s\",\"%.*s\"", n, pt_str.len, pt_str.s, at_str.len, at_str.s);
    }
  }

  return atModem_->doCommandBlocking(180 * 1000, nullptr) == at_result_code::OK;
}
