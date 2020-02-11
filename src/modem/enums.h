/*
 * enums.h
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
 * \file enums.h - various enumerations for AT commands and parameters
 */

#ifndef __OWL_MODEM_ENUMS_H__
#define __OWL_MODEM_ENUMS_H__


#include "../utils/utils.h"
#include "../platform/platform.h"

struct at_enum_text_match {
  int code;
  const char *text;
};
const char *at_enum_stringify(int code, const at_enum_text_match *match_table);

enum class at_result_code : int {
  wait_input   = -6,
  in_progress  = -5,
  cme_error    = -4,
  failure      = -3,
  timeout      = -2,
  unknown      = -1,
  OK           = 0,
  CONNECT      = 1,
  RING         = 2,
  NO_CARRIER   = 3,
  ERROR        = 4,
  CONNECT_1200 = 5,
  NO_DIALTONE  = 6,
  BUSY         = 7,
  NO_ANSWER    = 8,
};
extern const at_enum_text_match result_code_text_match[];
static inline const char *at_enum_stringify(at_result_code code) {
  return at_enum_stringify(static_cast<int>(code), result_code_text_match);
}

enum class cfun_fun : int {
  Minimum_Functionality                 = 0,
  Full_Functionality                    = 1,
  Airplane_Mode                         = 4,
  Enable_SIM_Toolkit_Interface          = 6, /**< only for SARA-R4/N4 series */
  Disable_SIM_Toolkit_Interface_7       = 7, /**< only for SARA-R4/N4 series */
  Disable_SIM_Toolkit_Interface_8       = 8, /**< only for SARA-R4/N4 series */
  Enable_SIM_Toolkit_Interface_Raw_Mode = 9, /**< only for SARA-R4/N4 series */
  Modem_Silent_Reset_No_SIM_Reset =
      15, /**< only for SARA-R4/N4 series - with detach from network and saving of NVM parameters */
  Modem_Silent_Reset_With_SIM_Reset =
      16, /**< only for SARA-R4/N4 series - with detach from network and saving of NVM parameters */
  Minimum_Functionality_with_CS_PS_and_SIM_Deactivated = 19, /**< only for SARA-R4/N4 series */
  Modem_Deep_Low_Power_Mode = 127, /**< only for SARA-R4/N4 series - with detach from network and saving
                                               of NVM parameters; wake up with power cycle or module reset */
};
extern const at_enum_text_match cfun_fun_text_match[];
static inline const char *at_enum_stringify(cfun_fun code) {
  return at_enum_stringify(static_cast<int>(code), cfun_fun_text_match);
}

enum class cfun_rst : int {
  No_Modem_Reset             = 0,
  Modem_and_SIM_Silent_Reset = 1, /**< do a modem & SIM silent reset before setting fun mode */
};
extern const at_enum_text_match cfun_rst_text_match[];
static inline const char *at_enum_stringify(cfun_rst code) {
  return at_enum_stringify(static_cast<int>(code), cfun_rst_text_match);
}

enum class cfun_power_mode : int {
  Minimum_Functionality                                = 0,
  Full_Functionality                                   = 1,
  Airplane_Mode                                        = 4,
  Minimum_Functionality_with_CS_PS_and_SIM_Deactivated = 19, /**< only for SARA-R4/N4 series */
};
extern const at_enum_text_match cfun_power_mode_text_match[];
static inline const char *at_enum_stringify(cfun_power_mode code) {
  return at_enum_stringify(static_cast<int>(code), cfun_power_mode_text_match);
}

enum class umnoprof_mno_profile : int {
  SW_Default       = 0,
  SIM_ICCID_Select = 1,
  ATT              = 2,
  Verizon          = 3,
  Telstra          = 4,
  TMO              = 5,
  CT               = 6,
};
extern const at_enum_text_match umnoprof_mno_profile_text_match[];
static inline const char *at_enum_stringify(umnoprof_mno_profile code) {
  return at_enum_stringify(static_cast<int>(code), umnoprof_mno_profile_text_match);
}

enum class cops_mode : int {
  Automatic_Selection     = 0,
  Manual_Selection        = 1,
  Deregister_From_Network = 2,
  Set_Only_Format         = 3,
  Manual_Automatic        = 4, /**< Start a manual selection; if it fails, go into automatic mode and
                                * stay there. */
};
extern const at_enum_text_match cops_mode_text_match[];
static inline const char *at_enum_stringify(cops_mode code) {
  return at_enum_stringify(static_cast<int>(code), cops_mode_text_match);
}

enum class cops_format : int {
  Long_Alphanumeric  = 0, /**< Up to 24 characters */
  Short_Alphanumeric = 1, /**< Up to 10 characters */
  Numeric            = 2, /**< 5 or 6 characters MCC/MNC codes - FFFF means undefined */
};
extern const at_enum_text_match cops_format_text_match[];
static inline const char *at_enum_stringify(cops_format code) {
  return at_enum_stringify(static_cast<int>(code), cops_format_text_match);
}

enum class cops_stat : int {
  Unknown   = 0,
  Available = 1,
  Current   = 2,
  Forbidden = 3,
};
extern const at_enum_text_match cops_stat_text_match[];
static inline const char *at_enum_stringify(cops_stat code) {
  return at_enum_stringify(static_cast<int>(code), cops_stat_text_match);
}

enum class cops_act : int {
  // TODO - define more values , for other modems too
  LTE        = 7,
  EC_GSM_IoT = 8, /**< A/Gb mode - on the ublox SARA-R404M-7, SARA-R410M-52B */
  LTE_Cat_M1 = 8, /**< On the ublox SARA-R410M-01B, SARA-R410M-02B */
  LTE_NB_S1  = 9, /**< NB-S1 mode - on the SARA-R410M-52B, SARA-N4 */
};
extern const at_enum_text_match cops_act_text_match[];
static inline const char *at_enum_stringify(cops_act code) {
  return at_enum_stringify(static_cast<int>(code), cops_act_text_match);
}

enum class creg_n : int {
  URC_Disabled             = 0, /**< No Unsolicited Registration Codes reported */
  Network_Registration_URC = 1, /**< Unsolicited Registration Codes reported for Network Registration */
  Network_Registration_and_Location_Information_URC =
      2, /**< Unsolicited Registration Codes reported for Network Registration and Location Information */
};
extern const at_enum_text_match creg_n_text_match[];
static inline const char *at_enum_stringify(creg_n code) {
  return at_enum_stringify(static_cast<int>(code), creg_n_text_match);
}

enum class creg_stat : int {
  Not_Registered                        = 0, /**< Not registered and not searching */
  Registered_Home_Network               = 1,
  Not_Registered_Searching              = 2,
  Registration_Denied                   = 3,
  Unknown                               = 4, /**< E.g. out of coverage */
  Registered_Roaming                    = 5,
  Registered_for_SMS_Only_Home_Network  = 6,  /**< Only ublox SARA-R4/N4 modems */
  Registered_for_SMS_Only_Roaming       = 7,  /**< Only ublox SARA-R4/N4 modems */
  Registered_for_CSFB_Only_Home_Network = 9,  /**< Applicable only for AcT E-UTRAN; Only ublox SARA-R4/N4 modems */
  Registered_for_CSFB_Only_Roaming      = 10, /**< Applicable only for AcT E-UTRAN; Only ublox SARA-R4/N4 modems */
};
extern const at_enum_text_match creg_stat_text_match[];
static inline const char *at_enum_stringify(creg_stat code) {
  return at_enum_stringify(static_cast<int>(code), creg_stat_text_match);
}

enum class creg_act : int {
  GSM                                     = 0,   /**< 2G */
  GSM_Compact                             = 1,   /**< 2G */
  UTRAN                                   = 2,   /**< 3G */
  GSM_with_EDGE_Availability              = 3,   /**< 2.5G */
  UTRAN_with_HSDPA_Availability           = 4,   /**< 3.5G */
  UTRAN_with_HSUPA_Availability           = 5,   /**< 3.5G */
  UTRAN_with_HSDPA_and_HSUPA_Availability = 6,   /**< 3.5G */
  E_UTRAN                                 = 7,   /**< 4G */
  EC_GSM_IoT                              = 8,   /**<  A/Gb mode - Only ublox SARA-R410M-01B/02B */
  E_UTRAN_NB                              = 9,   /**<  NB-S1 mode - Only ublox SARA-R410M-01B/02B/52B, SARA-N4*/
  invalid                                 = 255, /**< Indicates that the current AcT is invalid */
};
extern const at_enum_text_match creg_act_text_match[];
static inline const char *at_enum_stringify(creg_act code) {
  return at_enum_stringify(static_cast<int>(code), creg_act_text_match);
}

enum class cgreg_n : int {
  URC_Disabled             = 0, /**< No Unsolicited Registration Codes reported */
  Network_Registration_URC = 1, /**< Unsolicited Registration Codes reported for Network Registration */
  Network_Registration_and_Location_Information_URC =
      2, /**< Unsolicited Registration Codes reported for Network Registration and Location Information */
};
extern const at_enum_text_match cgreg_n_text_match[];
static inline const char *at_enum_stringify(cgreg_n code) {
  return at_enum_stringify(static_cast<int>(code), cgreg_n_text_match);
}

enum class cgreg_stat : int {
  Not_Registered                              = 0, /**< Not registered and not searching */
  Registered_Home_Network                     = 1,
  Not_Registered_Searching                    = 2,
  Registration_Denied                         = 3,
  Unknown                                     = 4, /**< E.g. out of coverage */
  Registered_Roaming                          = 5,
  Attached_for_Emergency_Bearer_Services_Only = 8,
};
extern const at_enum_text_match cgreg_stat_text_match[];
static inline const char *at_enum_stringify(cgreg_stat code) {
  return at_enum_stringify(static_cast<int>(code), cgreg_stat_text_match);
}

enum class cgreg_act : int {
  GSM                                     = 0,   /**< 2G */
  GSM_Compact                             = 1,   /**< 2G */
  UTRAN                                   = 2,   /**< 3G */
  GSM_with_EDGE_Availability              = 3,   /**< 2.5G */
  UTRAN_with_HSDPA_Availability           = 4,   /**< 3.5G */
  UTRAN_with_HSUPA_Availability           = 5,   /**< 3.5G */
  UTRAN_with_HSDPA_and_HSUPA_Availability = 6,   /**< 3.5G */
  invalid                                 = 255, /**< Indicates that the current AcT is invalid */
};
extern const at_enum_text_match cgreg_act_text_match[];
static inline const char *at_enum_stringify(cgreg_act code) {
  return at_enum_stringify(static_cast<int>(code), cgreg_act_text_match);
}

enum class cereg_n : int {
  URC_Disabled             = 0, /**< No Unsolicited Registration Codes reported */
  Network_Registration_URC = 1, /**< Unsolicited Registration Codes reported for Network Registration */
  Network_Registration_and_Location_Information_URC =
      2, /**< Unsolicited Registration Codes reported for Network Registration and Location Information */
  Network_Registration_Location_Information_and_EMM_URC     = 3,
  PSM_Network_Registration_and_Location_Information_URC     = 4,
  PSM_Network_Registration_Location_Information_and_EMM_URC = 5,
};
extern const at_enum_text_match cereg_n_text_match[];
static inline const char *at_enum_stringify(cereg_n code) {
  return at_enum_stringify(static_cast<int>(code), cereg_n_text_match);
}

enum class cereg_stat : int {
  Not_Registered                              = 0, /**< Not registered and not searching */
  Registered_Home_Network                     = 1,
  Not_Registered_Searching                    = 2,
  Registration_Denied                         = 3,
  Unknown                                     = 4, /**< E.g. out of coverage */
  Registered_Roaming                          = 5,
  Attached_for_Emergency_Bearer_Services_Only = 8,
};
extern const at_enum_text_match cereg_stat_text_match[];
static inline const char *at_enum_stringify(cereg_stat code) {
  return at_enum_stringify(static_cast<int>(code), cereg_stat_text_match);
}

enum class cereg_act : int {
  E_UTRAN    = 7,   /**< 4G */
  EC_GSM_IoT = 8,   /**<  A/Gb mode - Only ublox SARA-R410M-01B/02B */
  E_UTRAN_NB = 9,   /**<  NB-S1 mode - Only ublox SARA-R410M-01B/02B/52B, SARA-N4*/
  invalid    = 255, /**< Indicates that the current AcT is invalid */
};
extern const at_enum_text_match cereg_act_text_match[];
static inline const char *at_enum_stringify(cereg_act code) {
  return at_enum_stringify(static_cast<int>(code), cereg_act_text_match);
}

enum class cereg_cause_type : int {
  EMM_Cause                   = 0,
  Manufacturer_Specific_Cause = 1,
};
extern const at_enum_text_match cereg_cause_type_text_match[];
static inline const char *at_enum_stringify(cereg_cause_type code) {
  return at_enum_stringify(static_cast<int>(code), cereg_cause_type_text_match);
}

enum class csq_rssi : int {
  // either a numeric value or not detectable
  Not_Known_or_Detectable_99  = 99,
  Not_Known_or_Detectable_199 = 199,
  /* 100..199 - for use in TDSCDMA */
};

enum class csq_qual : int {
  /* Actual values depend on 2G/3G/LTE/etc, so check with 3GPP TS 45.008 */
  // either an integer value or not detectable
  Not_Known_or_Not_Detectable = 99,
};

enum class uso_protocol : int {
  none = 0,
  TCP  = 6,
  UDP  = 17,
};
extern const at_enum_text_match uso_protocol_text_match[];
static inline const char *at_enum_stringify(uso_protocol code) {
  return at_enum_stringify(static_cast<int>(code), uso_protocol_text_match);
}

enum class uso_error : int {
  Success                  = 0,   /**< No Error */
  U_EPERM                  = 1,   /**< Operation not permitted (internal error) */
  U_ENOENT                 = 2,   /**< No such resource (internal error) */
  U_EINTR                  = 4,   /**< Interrupted system call (internal error) */
  U_EIO                    = 5,   /**< I/O error (internal error) */
  U_EBADF                  = 9,   /**< Bad file descriptor (internal error) */
  U_ECHILD                 = 10,  /**< No child processes (internal error) */
  U_EWOULDBLOCK_EAGAIN     = 11,  /**< Current operation would block, try again */
  U_ENOMEM                 = 12,  /**< Out of memory (internal error) */
  U_EFAULT                 = 14,  /**< Bad address (internal error) */
  U_EINVAL                 = 22,  /**< Invalid argument */
  U_EPIPE                  = 32,  /**< Broken pipe (internal error) */
  U_ENOSYS                 = 38,  /**< Function not implemented */
  U_ENONET                 = 64,  /**< Machine is not on the internet */
  U_EEOF                   = 65,  /**< End of file */
  U_EPROTO                 = 71,  /**< Protocol error */
  U_EBADFD                 = 77,  /**< File descriptor in bad state (internal error) */
  U_EREMCHG                = 78,  /**< Remote address changed */
  U_EDESTADDRREQ           = 89,  /**< Destination address required */
  U_EPROTOTYPE             = 91,  /**< Wrong protocol type for socket */
  U_ENOPROTOOPT            = 92,  /**< Protocol not available */
  U_EPROTONOSUPPORT        = 93,  /**< Protocol not supported */
  U_ESOCKTNNOSUPPORT       = 94,  /**< Socket type not supported */
  U_EOPNOTSUPP             = 95,  /**< Operation not supported on transport endpoint */
  U_EPFNOSUPPORT           = 96,  /**< Protocol family not supported */
  U_EAFNOSUPPORT           = 97,  /**< Address family not supported by protocol */
  U_EADDRINUSE             = 98,  /**< Address already in use */
  U_EADDRNOTAVAIL          = 99,  /**< Cannot assign requested address */
  U_ENETDOWN               = 100, /**< Network is down */
  U_ENETUNREACH            = 101, /**< Network is unreachable */
  U_ENETRESET              = 102, /**< Network dropped connection because of reset */
  U_ECONNABORTED           = 103, /**< Software caused connection abort */
  U_ECONNRESET             = 104, /**< Connection reset by peer */
  U_ENOBUFS                = 105, /**< No buffer space available */
  U_EISCONN                = 106, /**< Transport endpoint is already connected */
  U_ENOTCONN               = 107, /**< Transport endpoint is not connected */
  U_ESHUTDOWN              = 108, /**< Cannot send after transport endpoint  shutdown */
  U_ETIMEDOUT              = 110, /**< Connection timed out */
  U_ECONNREFUSED           = 111, /**< Connection refused */
  U_EHOSTDOWN              = 112, /**< Host is down */
  U_EHOSTUNREACH           = 113, /**< No route to host */
  U_EINPROGRESS            = 115, /**< Operation now in progress */
  U_ENSRNODATA             = 160, /**< DNS server returned answer with no data */
  U_ENSRFORMERR            = 161, /**< DNS server claims query was misformatted */
  U_ENSRSERVFAIL           = 162, /**< DNS server returned general failure */
  U_ENSRNOTFOUND           = 163, /**< Domain name not found */
  U_ENSRNOTIMP             = 164, /**< DNS server does not implement requested operation */
  U_ENSRREFUSED            = 165, /**< DNS server refused query */
  U_ENSRBADQUERY           = 166, /**< Misformatted DNS query */
  U_ENSRBADNAME            = 167, /**< Misformatted domain name */
  U_ENSRBADFAMILY          = 168, /**< Unsupported address family */
  U_ENSRBADRESP            = 169, /**< Misformatted DNS reply */
  U_ENSRCONNREFUSED        = 170, /**< Could not contact DNS servers */
  U_ENSRTIMEOUT            = 171, /**< Timeout while contacting DNS servers */
  U_ENSROF                 = 172, /**< End of file */
  U_ENSRFILE               = 173, /**< Error reading file */
  U_ENSRNOMEM              = 174, /**< Out of memory */
  U_ENSRDESTRUCTION        = 175, /**< Application terminated lookup */
  U_ENSRQUERYDOMAINTOOLONG = 176, /**< Domain name is too long */
  U_ENSRCNAMELOOP          = 177, /**< Domain name is too long */
};
extern const at_enum_text_match uso_error_text_match[];
static inline const char *at_enum_stringify(uso_error code) {
  return at_enum_stringify(static_cast<int>(code), uso_error_text_match);
}

enum class edrx_mode : int {
  Disabled                   = 0, /**< eDRX Mode Disabled, retain parameters */
  Enabled                    = 1, /**< eDRX Mode Enabled */
  Enabled_With_URC           = 2, /**< eDRX Mode enabled with URC responses */
  Disable_And_Reset_Defaults = 3, /**< eDRX Mode Disabled, reset default params */
};
extern const at_enum_text_match edrx_mode_text_match[];
static inline const char *at_enum_stringify(edrx_mode code) {
  return at_enum_stringify(static_cast<int>(code), edrx_mode_text_match);
}

enum class edrx_act : int {
  GSM         = 2,   /**< Not valid for SARA-RN4 or BG96 */
  UTRAN       = 3,   /**< Not valid for SARA-RN4 or BG96 */
  LTE_Cat_M1  = 4,   /**< Specify Cat M1 */
  LTE_Cat_NB1 = 5,   /**< Specify Cat NB1 */
  Unspecified = 255, /**< Used to indicate unspecified, will use the current active technology type */
};
extern const at_enum_text_match edrx_act_text_match[];
static inline const char *at_enum_stringify(edrx_act code) {
  return at_enum_stringify(static_cast<int>(code), edrx_act_text_match);
}

enum class edrx_cycle_length : int {
  b_0000      = 0,   /**< 5.12 seconds; WB-S1 mode only otherwise interpreted as 0010 */
  b_0001      = 1,   /**< 10.24 seconds; WB-S1 mode only otherwise interpreted as 0010 */
  b_0010      = 2,   /**< 20.48 seconds */
  b_0011      = 3,   /**< 40.96 seconds */
  b_0100      = 4,   /**< 61.44 seconds; WB-S1 mode only otherwise interpreted as 0010 */
  b_0101      = 5,   /**< 81.92 seconds */
  b_0110      = 6,   /**< 102.4 seconds; WB-S1 mode only otherwise interpreted as 0010 */
  b_0111      = 7,   /**< 122.88 seconds; WB-S1 mode only otherwise interpreted as 0010 */
  b_1000      = 8,   /**< 143.36 seconds; WB-S1 mode only otherwise interpreted as 0010 */
  b_1001      = 9,   /**< 163.84 seconds */
  b_1010      = 10,  /**< 327.68 seconds */
  b_1011      = 11,  /**< 655.36 seconds */
  b_1100      = 12,  /**< 1310.72 seconds */
  b_1101      = 13,  /**< 2621.44 seconds */
  b_1110      = 14,  /**< 5242.88 seconds; NB-S1 mode only otherwise interpreted as 1101 */
  b_1111      = 15,  /**< 10485.76 seconds; NB-S1 mode only otherwise interpreted as 1101 */
  Unspecified = 255, /**< Passed when disabling or retaining current setting */
};

enum class edrx_paging_time_window : int {
  b_0000      = 0,   /**< NB1  1.28s ; M1  2.56s */
  b_0001      = 1,   /**< NB1  2.56s ; M1  5.12s */
  b_0010      = 2,   /**< NB1  3.84s ; M1  7.68s */
  b_0011      = 3,   /**< NB1  5.12s ; M1 10.24s */
  b_0100      = 4,   /**< NB1  6.4s  ; M1 12.8s  */
  b_0101      = 5,   /**< NB1  7.68s ; M1 15.36s */
  b_0110      = 6,   /**< NB1  8.96s ; M1 17.92s */
  b_0111      = 7,   /**< NB1 10.24s ; M1 20.48s */
  b_1000      = 8,   /**< NB1 11.52s ; M1 23.04s */
  b_1001      = 9,   /**< NB1 12.8s  ; M1 25.6s  */
  b_1010      = 10,  /**< NB1 14.08s ; M1 28.16s */
  b_1011      = 11,  /**< NB1 15.36s ; M1 30.72s */
  b_1100      = 12,  /**< NB1 16.64s ; M1 33.28s */
  b_1101      = 13,  /**< NB1 17.92s ; M1 35.84s */
  b_1110      = 14,  /**< NB1 19.20s ; M1 38.4s  */
  b_1111      = 15,  /**< NB1 20.48s ; M1 40.96s */
  Unspecified = 255, /**< Unspecified by URC */
};

enum class psm_mode : int {
  Disabled                   = 0, /**< PSM Mode Disabled, retain paramaters */
  Enabled                    = 1, /**< PSM Mode Enabled */
  Disable_And_Reset_Defaults = 2, /**< PSM Mode Disabled, reset default params */
};
extern const at_enum_text_match psm_mode_text_match[];
static inline const char *at_enum_stringify(psm_mode code) {
  return at_enum_stringify(static_cast<int>(code), psm_mode_text_match);
}

enum class psm_tau_interval : int {
  Minutes_10        = 0x0, /**< 10 minutes (000) */
  Hour_1            = 0x1, /**< 1 hour (001) */
  Hours_10          = 0x2, /**< 10 hours (010) */
  Seconds_2         = 0x3, /**< 2 seconds (011) */
  Seconds_30        = 0x4, /**< 30 seconds (100) */
  Minute_1          = 0x5, /**< 1 minute (101) */
  Hours_320         = 0x6, /**< 320 hours (110) */
  Timer_Unspecified = 0x7, /**< unspecified - used for either off or default values */
};

enum class psm_active_time_interval : int {
  Seconds_2         = 0x0, /**< 2 seconds (000) */
  Minute_1          = 0x1, /**< 1 minute (001) */
  Minutes_6         = 0x2, /**< 1 decihour == 6 minutes (010) */
  Timer_Unspecified = 0x7, /**< unspecified - used for either off or default values */
};

#endif
