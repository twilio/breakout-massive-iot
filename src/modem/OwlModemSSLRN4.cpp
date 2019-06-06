#include "OwlModemSSLRN4.h"
#include <stdio.h>

OwlModemSSLRN4::OwlModemSSLRN4(OwlModemAT* atModem) : atModem_(atModem) {
}

static str s_ca_name = STRDECL("CA");
static str s_cert_name = STRDECL("Cert");
static str s_key_name = STRDECL("Key");

//static str s_qfopen = STRDECL("+QFOPEN: ");

bool OwlModemSSLRN4::initContext(uint8_t ssl_context_slot) {
  char buffer[64];

  snprintf(buffer, 64, "AT+USECPRF=%d,%d,%d", ssl_context_slot, USECPRF_OP_CODE_Certificate_Validation_Level, USECPRF_VALIDATION_LEVEL_No_Validation);
  if (atModem_->doCommandBlocking(buffer, 1 * 1000, nullptr) != AT_Result_Code__OK) {
    return false;
  }

  snprintf(buffer, 64, "AT+USECPRF=%d,%d,\"%.*s\"", ssl_context_slot, USECPRF_OP_CODE_Trusted_Root_Certificate_Internal_Name, s_ca_name.len, s_ca_name.s);
  if (atModem_->doCommandBlocking(buffer, 1 * 1000, nullptr) != AT_Result_Code__OK) {
    return false;
  }

  snprintf(buffer, 64, "AT+USECPRF=%d,%d,\"%.*s\"", ssl_context_slot, USECPRF_OP_CODE_Client_Certificate_Internal_Name, s_cert_name.len, s_cert_name.s);
  if (atModem_->doCommandBlocking(buffer, 1 * 1000, nullptr) != AT_Result_Code__OK) {
    return false;
  }

  snprintf(buffer, 64, "AT+USECPRF=%d,%d,\"%.*s\"", ssl_context_slot, USECPRF_OP_CODE_Client_Private_Key_Internal_Name, s_key_name.len, s_key_name.s);
  if (atModem_->doCommandBlocking(buffer, 1 * 1000, nullptr) != AT_Result_Code__OK) {
    return false;
  }

  return true;
}

bool OwlModemSSLRN4::setDeviceCert(str cert, bool force) {
  bool write_cert = true;

  if (!force) {
    char buffer[64];
    snprintf(buffer, 64, "AT+USECMNG=%d,%d,\"%.*s\"", USECMNG_OPERATION_Calculate_MD5, USECMNG_CERTIFICATE_TYPE_Certificate, s_cert_name.len, s_cert_name.s);

    if (atModem_->doCommandBlocking(buffer, 10 * 1000, &ssl_response) == AT_Result_Code__OK) {
      // TODO: compare MD5 against cert
      // cert already exists, in future calculate md5 and compare against one returned but for
      // now just assume it doesn't need setting
      LOG(L_NOTICE, "Using existing cert: %.*s\r\n", ssl_response.len, ssl_response.s);
      write_cert = false;
    }
  }

  if (write_cert) {
    LOG(L_NOTICE, "Setting cert...\r\n");
    char buffer[64];
    snprintf(buffer, 64, "AT+USECMNG=%d,%d,\"%.*s\",%d", USECMNG_OPERATION_Import_From_Serial, USECMNG_CERTIFICATE_TYPE_Certificate, s_cert_name.len, s_cert_name.s, (int)cert.len);

    if (atModem_->doCommandBlocking(buffer, 10 * 1000, nullptr, cert) != AT_Result_Code__OK) {
      return false;
    }
  }

  return true;
}

bool OwlModemSSLRN4::setDevicePkey(str pkey, bool force) {
  bool write_pkey = true;

  if (!force) {
    char buffer[64];
    snprintf(buffer, 64, "AT+USECMNG=%d,%d,\"%.*s\"", USECMNG_OPERATION_Calculate_MD5, USECMNG_CERTIFICATE_TYPE_Key, s_key_name.len, s_key_name.s);

    if (atModem_->doCommandBlocking(buffer, 10 * 1000, &ssl_response) == AT_Result_Code__OK) {
      // TODO: compare MD5 against key 
      // key already exists, in future calculate md5 and compare against one returned but for
      // now just assume it doesn't need setting
      LOG(L_NOTICE, "Using existing key: %.*s\r\n", ssl_response.len, ssl_response.s);
      write_pkey = false;
    }
  }

  if (write_pkey) {
    LOG(L_NOTICE, "Setting private key...\r\n");
    char buffer[64];
    snprintf(buffer, 64, "AT+USECMNG=%d,%d,\"%.*s\",%d", USECMNG_OPERATION_Import_From_Serial, USECMNG_CERTIFICATE_TYPE_Key, s_key_name.len, s_key_name.s, (int)pkey.len);

    if (atModem_->doCommandBlocking(buffer, 10 * 1000, nullptr, pkey) != AT_Result_Code__OK) {
      return false;
    }
  }

  return true;
}

bool OwlModemSSLRN4::setServerCA(str ca, bool force) {
  bool write_ca = true;

  if (!force) {
    char buffer[64];
    snprintf(buffer, 64, "AT+USECMNG=%d,%d,\"%.*s\"", USECMNG_OPERATION_Calculate_MD5, USECMNG_CERTIFICATE_TYPE_Trusted_Root_CA, s_ca_name.len, s_ca_name.s);

    if (atModem_->doCommandBlocking(buffer, 10 * 1000, &ssl_response) == AT_Result_Code__OK) {
      // TODO: compare MD5 against ca 
      // ca already exists, in future calculate md5 and compare against one returned but for
      // now just assume it doesn't need setting
      LOG(L_NOTICE, "Using existing server ca: %.*s\r\n", ssl_response.len, ssl_response.s);
      write_ca = false;
    }
  }

  if (write_ca) {
    LOG(L_NOTICE, "Setting server CA...\r\n");
    char buffer[64];
    snprintf(buffer, 64, "AT+USECMNG=%d,%d,\"%.*s\",%d", USECMNG_OPERATION_Import_From_Serial, USECMNG_CERTIFICATE_TYPE_Trusted_Root_CA, s_ca_name.len, s_ca_name.s, (int)ca.len);
    LOG(L_NOTICE, "> %s\r\n", buffer);

    if (atModem_->doCommandBlocking(buffer, 10 * 1000, nullptr, ca) != AT_Result_Code__OK) {
      return false;
    }
  }

  return true;
}
