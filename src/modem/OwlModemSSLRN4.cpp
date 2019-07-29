#include "OwlModemSSLRN4.h"
#include <stdio.h>

#include "../utils/md5.h"
#include "../utils/base64.h"

OwlModemSSLRN4::OwlModemSSLRN4(OwlModemAT *atModem) : atModem_(atModem) {
}

static str s_ca_name   = STRDECL("CA");
static str s_cert_name = STRDECL("Cert");
static str s_key_name  = STRDECL("Key");

bool OwlModemSSLRN4::initContext(uint8_t ssl_profile_slot, usecprf_cipher_suite_e cipher_suite) {
  atModem_->commandSprintf("AT+USECPRF=%d,%d,%d", ssl_profile_slot, USECPRF_OP_CODE_Certificate_Validation_Level,
                           USECPRF_VALIDATION_LEVEL_No_Validation);
  if (atModem_->doCommandBlocking(1 * 1000, nullptr) != AT_Result_Code__OK) {
    return false;
  }

  atModem_->commandSprintf("AT+USECPRF=%d,%d,\"%.*s\"", ssl_profile_slot,
                           USECPRF_OP_CODE_Trusted_Root_Certificate_Internal_Name, s_ca_name.len, s_ca_name.s);
  if (atModem_->doCommandBlocking(1 * 1000, nullptr) != AT_Result_Code__OK) {
    return false;
  }

  atModem_->commandSprintf("AT+USECPRF=%d,%d,\"%.*s\"", ssl_profile_slot,
                           USECPRF_OP_CODE_Client_Certificate_Internal_Name, s_cert_name.len, s_cert_name.s);
  if (atModem_->doCommandBlocking(1 * 1000, nullptr) != AT_Result_Code__OK) {
    return false;
  }

  atModem_->commandSprintf("AT+USECPRF=%d,%d,\"%.*s\"", ssl_profile_slot,
                           USECPRF_OP_CODE_Client_Private_Key_Internal_Name, s_key_name.len, s_key_name.s);
  if (atModem_->doCommandBlocking(1 * 1000, nullptr) != AT_Result_Code__OK) {
    return false;
  }

  atModem_->commandSprintf("AT+USECPRF=%d,%d,%d", ssl_profile_slot, USECPRF_OP_CODE_Cipher_Suite, cipher_suite);
  if (atModem_->doCommandBlocking(1 * 1000, nullptr) != AT_Result_Code__OK) {
    return false;
  }

  return true;
}

bool OwlModemSSLRN4::setDeviceCert(str cert) {
  bool write_cert = shouldWriteCertificate(USECMNG_CERTIFICATE_TYPE_Certificate, s_cert_name, cert);

  if (write_cert) {
    if (isPEM(cert)) {
      // remove garbage, most likely trailing zeros
      while (cert.s[cert.len - 1] != '-') {
        --cert.len;
      }
    }

    atModem_->commandSprintf("AT+USECMNG=%d,%d,\"%.*s\",%d", USECMNG_OPERATION_Import_From_Serial,
                             USECMNG_CERTIFICATE_TYPE_Certificate, s_cert_name.len, s_cert_name.s, (int)cert.len);

    if (atModem_->doCommandBlocking(10 * 1000, nullptr, cert) != AT_Result_Code__OK) {
      return false;
    }
  }

  return true;
}

bool OwlModemSSLRN4::setDevicePkey(str pkey) {
  bool write_pkey = shouldWriteCertificate(USECMNG_CERTIFICATE_TYPE_Key, s_key_name, pkey);

  if (write_pkey) {
    if (isPEM(pkey)) {
      // remove garbage, most likely trailing zeros
      while (pkey.s[pkey.len - 1] != '-') {
        --pkey.len;
      }
    }

    atModem_->commandSprintf("AT+USECMNG=%d,%d,\"%.*s\",%d", USECMNG_OPERATION_Import_From_Serial,
                             USECMNG_CERTIFICATE_TYPE_Key, s_key_name.len, s_key_name.s, (int)pkey.len);

    if (atModem_->doCommandBlocking(10 * 1000, nullptr, pkey) != AT_Result_Code__OK) {
      return false;
    }
  }

  return true;
}

bool OwlModemSSLRN4::setServerCA(str ca) {
  bool write_ca = shouldWriteCertificate(USECMNG_CERTIFICATE_TYPE_Trusted_Root_CA, s_ca_name, ca);

  if (write_ca) {
    if (isPEM(ca)) {
      // remove garbage, most likely trailing zeros
      while (ca.s[ca.len - 1] != '-') {
        --ca.len;
      }
    }

    atModem_->commandSprintf("AT+USECMNG=%d,%d,\"%.*s\",%d", USECMNG_OPERATION_Import_From_Serial,
                             USECMNG_CERTIFICATE_TYPE_Trusted_Root_CA, s_ca_name.len, s_ca_name.s, (int)ca.len);

    if (atModem_->doCommandBlocking(10 * 1000, nullptr, ca) != AT_Result_Code__OK) {
      return false;
    }
  }

  return true;
}

bool OwlModemSSLRN4::isPEM(str input) {
  // contains at least '-----BEGIN ... ----- ... -----END -----', therefore 30 characters
  return input.s && input.len > 30 && strstr(input.s, "-----B");
}

int OwlModemSSLRN4::calculateMD5ForCert(char *output, int max_len, str input) {
  if (max_len < 33) {
    // output buffer not big enough
    return 0;
  }

  unsigned char digest[16];

  if (isPEM(input)) {
    char *start = strstr(input.s, "MII");
    owl_base64decode_md5(digest, start);
  } else {
    struct MD5Context context;
    MD5Init(&context);
    MD5Update(&context, (unsigned const char *)input.s, input.len);
    MD5Final(digest, &context);
  }

  for (int i = 0; i < 16; ++i)
    sprintf(&output[i * 2], "%02X", (unsigned int)digest[i]);
  return 32;
}

bool OwlModemSSLRN4::shouldWriteCertificate(usecmng_certificate_type_e type, str name, str new_value) {
  bool should_write = true;

  char md5_buf[33];  // 33 to accomodate 32 characters plus null termination sprintf adds that strstr below needs
  str md5 = {.s = md5_buf, .len = 0};
  md5.len = calculateMD5ForCert(md5_buf, 33, new_value);

  atModem_->commandSprintf("AT+USECMNG=%d,%d,\"%.*s\"", USECMNG_OPERATION_Calculate_MD5, type, name.len, name.s);

  if (atModem_->doCommandBlocking(10 * 1000, &ssl_response) == AT_Result_Code__OK) {
    char *match = strstr(ssl_response.s, md5_buf);
    if (match != NULL) {
      should_write = false;
    } else {
      should_write = true;
    }
  }

  return should_write;
}
