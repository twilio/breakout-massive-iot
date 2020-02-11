/*
 * enums.cpp
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
 * \file enums.cpp - various enumerations for AT commands and parameters
 */

#include "enums.h"

const char* at_enum_stringify(int code, const at_enum_text_match* match_table) {
  for (int i = 0; match_table[i].text != nullptr; i++) {
    if (match_table[i].code == code) {
      return match_table[i].text;
    }
  };
  return "unknown enum value";
}

const at_enum_text_match result_code_text_match[] = {{static_cast<int>(at_result_code::cme_error), "CME Error"},
                                                     {static_cast<int>(at_result_code::failure), "failure"},
                                                     {static_cast<int>(at_result_code::timeout), "timeout"},
                                                     {static_cast<int>(at_result_code::unknown), "unknown"},
                                                     {static_cast<int>(at_result_code::OK), "OK"},
                                                     {static_cast<int>(at_result_code::CONNECT), "CONNECT"},
                                                     {static_cast<int>(at_result_code::RING), "RING"},
                                                     {static_cast<int>(at_result_code::NO_CARRIER), "NO CARRIER"},
                                                     {static_cast<int>(at_result_code::ERROR), "ERROR"},
                                                     {static_cast<int>(at_result_code::CONNECT_1200), "CONNECT 1200"},
                                                     {static_cast<int>(at_result_code::NO_DIALTONE), "NO DIALTONE"},
                                                     {static_cast<int>(at_result_code::BUSY), "BUSY"},
                                                     {static_cast<int>(at_result_code::NO_ANSWER), "NO ANSWER"},
                                                     {0, nullptr}};

const at_enum_text_match cfun_fun_text_match[] = {
    {static_cast<int>(cfun_fun::Minimum_Functionality), "minimum functionality"},
    {static_cast<int>(cfun_fun::Full_Functionality), "full functionality"},
    {static_cast<int>(cfun_fun::Airplane_Mode), "airplane mode"},
    {static_cast<int>(cfun_fun::Enable_SIM_Toolkit_Interface), "enable SIM-toolkit interface - SIM-APPL"},
    {static_cast<int>(cfun_fun::Disable_SIM_Toolkit_Interface_7), "disable SIM-toolkit interface - SIM-APPL (7)"},
    {static_cast<int>(cfun_fun::Disable_SIM_Toolkit_Interface_8), "disable SIM-toolkit interface - SIM-APPL (8)"},
    {static_cast<int>(cfun_fun::Enable_SIM_Toolkit_Interface_Raw_Mode), "enable SIM-toolkit interface - raw"},
    {static_cast<int>(cfun_fun::Modem_Silent_Reset_No_SIM_Reset), "silent modem reset, no SIM reset"},
    {static_cast<int>(cfun_fun::Modem_Silent_Reset_With_SIM_Reset), "silent modem reset, with SIM reset"},
    {static_cast<int>(cfun_fun::Minimum_Functionality_with_CS_PS_and_SIM_Deactivated),
     "minimum functionality, CS/PS/SIM deactivated"},
    {static_cast<int>(cfun_fun::Modem_Deep_Low_Power_Mode), "modem deep low power mode"},
    {0, nullptr}};

const at_enum_text_match cfun_rst_text_match[] = {
    {static_cast<int>(cfun_rst::No_Modem_Reset), "no modem/SIM reset"},
    {static_cast<int>(cfun_rst::Modem_and_SIM_Silent_Reset), "modem/SIM silent reset"},
    {0, nullptr}};

const at_enum_text_match cfun_power_mode_text_match[] = {
    {static_cast<int>(cfun_power_mode::Minimum_Functionality), "minimum functionality"},
    {static_cast<int>(cfun_power_mode::Full_Functionality), "full functionality"},
    {static_cast<int>(cfun_power_mode::Airplane_Mode), "airplane mode"},
    {static_cast<int>(cfun_power_mode::Minimum_Functionality_with_CS_PS_and_SIM_Deactivated),
     "minimum functionality, CS/PS/SIM deactivated"},
    {0, nullptr}};

const at_enum_text_match umnoprof_mno_profile_text_match[] = {
    {static_cast<int>(umnoprof_mno_profile::SW_Default), "SW default"},
    {static_cast<int>(umnoprof_mno_profile::SIM_ICCID_Select), "SIM_ICCID_Select"},
    {static_cast<int>(umnoprof_mno_profile::ATT), "AT&T"},
    {static_cast<int>(umnoprof_mno_profile::Verizon), "Verizon"},
    {static_cast<int>(umnoprof_mno_profile::Telstra), "Telstra"},
    {static_cast<int>(umnoprof_mno_profile::TMO), "TMO"},
    {static_cast<int>(umnoprof_mno_profile::CT), "CT"},
    {0, nullptr}};

const at_enum_text_match cops_mode_text_match[] = {
    {static_cast<int>(cops_mode::Automatic_Selection), "automatic selection"},
    {static_cast<int>(cops_mode::Manual_Selection), "manual selection"},
    {static_cast<int>(cops_mode::Deregister_From_Network), "deregister from network"},
    {static_cast<int>(cops_mode::Set_Only_Format), "set only <format>"},
    {static_cast<int>(cops_mode::Manual_Automatic), "manual/automatic selection"},
    {0, nullptr}};

const at_enum_text_match cops_format_text_match[] = {
    {static_cast<int>(cops_format::Long_Alphanumeric), "long alphanumeric"},
    {static_cast<int>(cops_format::Short_Alphanumeric), "short alphanumeric"},
    {static_cast<int>(cops_format::Numeric), "numeric"},
    {0, nullptr}};

const at_enum_text_match cops_stat_text_match[] = {{static_cast<int>(cops_stat::Unknown), "unknown"},
                                                   {static_cast<int>(cops_stat::Available), "available"},
                                                   {static_cast<int>(cops_stat::Current), "current"},
                                                   {static_cast<int>(cops_stat::Forbidden), "forbidden"},
                                                   {0, nullptr}};

const at_enum_text_match cops_act_text_match[] = {{static_cast<int>(cops_act::LTE), "LTE"},
                                                  {static_cast<int>(cops_act::EC_GSM_IoT), "EC-GSM-IoT or LTE-Cat-M1"},
                                                  {static_cast<int>(cops_act::LTE_NB_S1), "LTE NB-S1"},
                                                  {0, nullptr}};

const at_enum_text_match creg_n_text_match[] = {
    {static_cast<int>(creg_n::URC_Disabled), "URC disabled"},
    {static_cast<int>(creg_n::Network_Registration_URC), "URC for Network Registration"},
    {static_cast<int>(creg_n::Network_Registration_and_Location_Information_URC),
     "URC for Network Registration and Location Information"},
    {0, nullptr}};

const at_enum_text_match creg_stat_text_match[] = {
    {static_cast<int>(creg_stat::Not_Registered), "not registered, not searching"},
    {static_cast<int>(creg_stat::Registered_Home_Network), "registered, home network"},
    {static_cast<int>(creg_stat::Not_Registered_Searching), "not registered, searching"},
    {static_cast<int>(creg_stat::Registered_Roaming), "registered, roaming"},
    {static_cast<int>(creg_stat::Registration_Denied), "registration denied"},
    {static_cast<int>(creg_stat::Unknown), "unknown"},
    {static_cast<int>(creg_stat::Registered_for_SMS_Only_Home_Network), "registered for SMS only, home network"},
    {static_cast<int>(creg_stat::Registered_for_SMS_Only_Roaming), "registered for SMS only, roaming"},
    {static_cast<int>(creg_stat::Registered_for_CSFB_Only_Home_Network), "registered for CSFB only, home network"},
    {static_cast<int>(creg_stat::Registered_for_CSFB_Only_Roaming), "registered for CSFB only, roaming"},
    {0, nullptr}};

const at_enum_text_match creg_act_text_match[] = {
    {static_cast<int>(creg_act::GSM), "2G/GSM"},
    {static_cast<int>(creg_act::GSM_Compact), "GSM-Compact"},
    {static_cast<int>(creg_act::UTRAN), "3G/UTRAN"},
    {static_cast<int>(creg_act::GSM_with_EDGE_Availability), "2.5G/GSM+EDGE"},
    {static_cast<int>(creg_act::UTRAN_with_HSDPA_Availability), "3.5G/UTRAN+HSDPA"},
    {static_cast<int>(creg_act::UTRAN_with_HSUPA_Availability), "3.5G/UTRAN+HSUPA"},
    {static_cast<int>(creg_act::UTRAN_with_HSDPA_and_HSUPA_Availability), "3.5G/UTRAN+HSDPA+HSUPA"},
    {static_cast<int>(creg_act::E_UTRAN), "4G/LTE"},
    {static_cast<int>(creg_act::EC_GSM_IoT), "2G/Extended-Coverage GSM for IoT"},
    {static_cast<int>(creg_act::E_UTRAN_NB), "LTE/NB-S1"},
    {static_cast<int>(creg_act::invalid), "invalid"},
    {0, nullptr}};

const at_enum_text_match cgreg_n_text_match[] = {
    {static_cast<int>(cgreg_n::URC_Disabled), "URC disabled"},
    {static_cast<int>(cgreg_n::Network_Registration_URC), "URC for Network Registration"},
    {static_cast<int>(cgreg_n::Network_Registration_and_Location_Information_URC),
     "URC for Network Registration and Location Information"},
    {0, nullptr}};

const at_enum_text_match cgreg_stat_text_match[] = {
    {static_cast<int>(cgreg_stat::Not_Registered), "not registered, not searching"},
    {static_cast<int>(cgreg_stat::Registered_Home_Network), "registered, home network"},
    {static_cast<int>(cgreg_stat::Not_Registered_Searching), "not registered, searching"},
    {static_cast<int>(cgreg_stat::Registration_Denied), "registration denied"},
    {static_cast<int>(cgreg_stat::Unknown), "unknown"},
    {static_cast<int>(cgreg_stat::Registered_Roaming), "registered, roaming"},
    {static_cast<int>(cgreg_stat::Attached_for_Emergency_Bearer_Services_Only),
     "attached for emergency bearer services only"},
    {0, nullptr}};

const at_enum_text_match cgreg_act_text_match[] = {
    {static_cast<int>(cgreg_act::GSM), "2G/GSM"},
    {static_cast<int>(cgreg_act::GSM_Compact), "GSM-Compact"},
    {static_cast<int>(cgreg_act::UTRAN), "3G/UTRAN"},
    {static_cast<int>(cgreg_act::GSM_with_EDGE_Availability), "2.5G/GSM+EDGE"},
    {static_cast<int>(cgreg_act::UTRAN_with_HSDPA_Availability), "3.5G/UTRAN+HSDPA"},
    {static_cast<int>(cgreg_act::UTRAN_with_HSUPA_Availability), "3.5G/UTRAN+HSUPA"},
    {static_cast<int>(cgreg_act::UTRAN_with_HSDPA_and_HSUPA_Availability), "3.5G/UTRAN+HSDPA+HSUPA"},
    {static_cast<int>(cgreg_act::invalid), "invalid"},
    {0, nullptr}};

const at_enum_text_match cereg_n_text_match[] = {
    {static_cast<int>(cereg_n::URC_Disabled), "URC disabled"},
    {static_cast<int>(cereg_n::Network_Registration_URC), "URC for Network Registration"},
    {static_cast<int>(cereg_n::Network_Registration_and_Location_Information_URC),
     "URC for Network Registration and Location Information"},
    {static_cast<int>(cereg_n::PSM_Network_Registration_and_Location_Information_URC),
     "URC for PSM, Network Registration and Location Information"},
    {static_cast<int>(cereg_n::PSM_Network_Registration_Location_Information_and_EMM_URC),
     "URC for PSM, Network Registration and Location Information and EMM"},
    {0, nullptr}};

const at_enum_text_match cereg_stat_text_match[] = {
    {static_cast<int>(cereg_stat::Not_Registered), "not registered, not searching"},
    {static_cast<int>(cereg_stat::Registered_Home_Network), "registered, home network"},
    {static_cast<int>(cereg_stat::Not_Registered_Searching), "not registered, searching"},
    {static_cast<int>(cereg_stat::Registration_Denied), "registration denied"},
    {static_cast<int>(cereg_stat::Unknown), "unknown"},
    {static_cast<int>(cereg_stat::Registered_Roaming), "registered, roaming"},
    {static_cast<int>(cereg_stat::Attached_for_Emergency_Bearer_Services_Only),
     "attached for emergency bearer services only"},
    {0, nullptr}};

const at_enum_text_match cereg_act_text_match[] = {
    {static_cast<int>(cereg_act::E_UTRAN), "4G/LTE"},
    {static_cast<int>(cereg_act::EC_GSM_IoT), "2G/Extended-Coverage GSM for IoT"},
    {static_cast<int>(cereg_act::E_UTRAN_NB), "LTE/NB-S1"},
    {static_cast<int>(cereg_act::invalid), "invalid"},
    {0, nullptr}};

const at_enum_text_match cereg_cause_type_text_match[] = {
    {static_cast<int>(cereg_cause_type::EMM_Cause), "EMM Cause"},
    {static_cast<int>(cereg_cause_type::Manufacturer_Specific_Cause), "Manufacturer-specific cause"},
    {0, nullptr}};

const at_enum_text_match uso_protocol_text_match[] = {{static_cast<int>(uso_protocol::none), "<none>"},
                                                      {static_cast<int>(uso_protocol::TCP), "TCP"},
                                                      {static_cast<int>(uso_protocol::UDP), "UDP"},
                                                      {0, nullptr}};

const at_enum_text_match uso_error_text_match[] = {
    {static_cast<int>(uso_error::Success), "No Error"},
    {static_cast<int>(uso_error::U_EPERM), "Operation not permitted (internal error)"},
    {static_cast<int>(uso_error::U_ENOENT), "No such resource (internal error)"},
    {static_cast<int>(uso_error::U_EINTR), "Interrupted system call (internal error)"},
    {static_cast<int>(uso_error::U_EIO), "I/O error (internal error)"},
    {static_cast<int>(uso_error::U_EBADF), "Bad file descriptor (internal error)"},
    {static_cast<int>(uso_error::U_ECHILD), "No child processes (internal error)"},
    {static_cast<int>(uso_error::U_EWOULDBLOCK_EAGAIN), "Current operation would block, try again"},
    {static_cast<int>(uso_error::U_ENOMEM), "Out of memory (internal error)"},
    {static_cast<int>(uso_error::U_EFAULT), "Bad address (internal error)"},
    {static_cast<int>(uso_error::U_EINVAL), "Invalid argument"},
    {static_cast<int>(uso_error::U_EPIPE), "Broken pipe (internal error)"},
    {static_cast<int>(uso_error::U_ENOSYS), "Function not implemented"},
    {static_cast<int>(uso_error::U_ENONET), "Machine is not on the internet"},
    {static_cast<int>(uso_error::U_EEOF), "End of file"},
    {static_cast<int>(uso_error::U_EPROTO), "Protocol error"},
    {static_cast<int>(uso_error::U_EBADFD), "File descriptor in bad state (internal error)"},
    {static_cast<int>(uso_error::U_EREMCHG), "Remote address changed"},
    {static_cast<int>(uso_error::U_EDESTADDRREQ), "Destination address required"},
    {static_cast<int>(uso_error::U_EPROTOTYPE), "Wrong protocol type for socket"},
    {static_cast<int>(uso_error::U_ENOPROTOOPT), "Protocol not available"},
    {static_cast<int>(uso_error::U_EPROTONOSUPPORT), "Protocol not supported"},
    {static_cast<int>(uso_error::U_ESOCKTNNOSUPPORT), "Socket type not supported"},
    {static_cast<int>(uso_error::U_EOPNOTSUPP), "Operation not supported on transport endpoint"},
    {static_cast<int>(uso_error::U_EPFNOSUPPORT), "Protocol family not supported"},
    {static_cast<int>(uso_error::U_EAFNOSUPPORT), "Address family not supported by protocol"},
    {static_cast<int>(uso_error::U_EADDRINUSE), "Address already in use"},
    {static_cast<int>(uso_error::U_EADDRNOTAVAIL), "Cannot assign requested address"},
    {static_cast<int>(uso_error::U_ENETDOWN), "Network is down"},
    {static_cast<int>(uso_error::U_ENETUNREACH), "Network is unreachable"},
    {static_cast<int>(uso_error::U_ENETRESET), "Network dropped connection because of reset"},
    {static_cast<int>(uso_error::U_ECONNABORTED), "Software caused connection abort"},
    {static_cast<int>(uso_error::U_ECONNRESET), "Connection reset by peer"},
    {static_cast<int>(uso_error::U_ENOBUFS), "No buffer space available"},
    {static_cast<int>(uso_error::U_EISCONN), "Transport endpoint is already connected"},
    {static_cast<int>(uso_error::U_ENOTCONN), "Transport endpoint is not connected"},
    {static_cast<int>(uso_error::U_ESHUTDOWN), "Cannot send after transport endpoint  shutdown"},
    {static_cast<int>(uso_error::U_ETIMEDOUT), "Connection timed out"},
    {static_cast<int>(uso_error::U_ECONNREFUSED), "Connection refused"},
    {static_cast<int>(uso_error::U_EHOSTDOWN), "Host is down"},
    {static_cast<int>(uso_error::U_EHOSTUNREACH), "No route to host"},
    {static_cast<int>(uso_error::U_EINPROGRESS), "Operation now in progress"},
    {static_cast<int>(uso_error::U_ENSRNODATA), "DNS server returned answer with no data"},
    {static_cast<int>(uso_error::U_ENSRFORMERR), "DNS server claims query was misformatted"},
    {static_cast<int>(uso_error::U_ENSRSERVFAIL), "DNS server returned general failure"},
    {static_cast<int>(uso_error::U_ENSRNOTFOUND), "Domain name not found"},
    {static_cast<int>(uso_error::U_ENSRNOTIMP), "DNS server does not implement requested operation"},
    {static_cast<int>(uso_error::U_ENSRREFUSED), "DNS server refused query"},
    {static_cast<int>(uso_error::U_ENSRBADQUERY), "Misformatted DNS query"},
    {static_cast<int>(uso_error::U_ENSRBADNAME), "Misformatted domain name"},
    {static_cast<int>(uso_error::U_ENSRBADFAMILY), "Unsupported address family"},
    {static_cast<int>(uso_error::U_ENSRBADRESP), "Misformatted DNS reply"},
    {static_cast<int>(uso_error::U_ENSRCONNREFUSED), "Could not contact DNS servers"},
    {static_cast<int>(uso_error::U_ENSRTIMEOUT), "Timeout while contacting DNS servers"},
    {static_cast<int>(uso_error::U_ENSROF), "End of file"},
    {static_cast<int>(uso_error::U_ENSRFILE), "Error reading file"},
    {static_cast<int>(uso_error::U_ENSRNOMEM), "Out of memory"},
    {static_cast<int>(uso_error::U_ENSRDESTRUCTION), "Application terminated lookup"},
    {static_cast<int>(uso_error::U_ENSRQUERYDOMAINTOOLONG), "Domain name is too long"},
    {static_cast<int>(uso_error::U_ENSRCNAMELOOP), "Domain name is too long"},
    {0, nullptr}};

const at_enum_text_match edrx_mode_text_match[] = {
    {static_cast<int>(edrx_mode::Disabled), "Disabled"},
    {static_cast<int>(edrx_mode::Enabled), "Enabled"},
    {static_cast<int>(edrx_mode::Enabled_With_URC), "Enabled with URC"},
    {static_cast<int>(edrx_mode::Disable_And_Reset_Defaults), "Disabled with reset"},
    {0, nullptr}};

const at_enum_text_match edrx_act_text_match[] = {{static_cast<int>(edrx_act::GSM), "GSM"},
                                                  {static_cast<int>(edrx_act::UTRAN), "UTRAN"},
                                                  {static_cast<int>(edrx_act::LTE_Cat_M1), "LTE Cat M1"},
                                                  {static_cast<int>(edrx_act::LTE_Cat_NB1), "LTE Cat NB1"},
                                                  {static_cast<int>(edrx_act::Unspecified), "Unspecified"},
                                                  {0, nullptr}};


const at_enum_text_match psm_mode_text_match[] = {
    {static_cast<int>(psm_mode::Disabled), "Disabled"},
    {static_cast<int>(psm_mode::Enabled), "Enabled"},
    {static_cast<int>(psm_mode::Disable_And_Reset_Defaults), "Disabled with reset"},
    {0, nullptr}};
