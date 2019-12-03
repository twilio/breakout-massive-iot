/*
 *
 * Twilio Massive SDK
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

#ifndef ARDUINO  // arduino tries to compile everything in src directory, but this is not intended for the target

#include <sys/mman.h>
#include <sys/stat.h>
#include <chrono>
#include <modem/OwlModemRN4.h>
#include <json.hpp>
#include "CharDeviceSerial.h"
#include "paho-mqtt-client.h"
#include "dummy-data-source.h"

#define OVERWRITE_TLS_CREDS false

// TODO: make them parameters as well
#define INIT_FLAGS 0

bool sleep_state = false;

static void print_message(str topic, str data) {
  LOG(L_ERR, "MQTT message: %.*s - %.*s\n", topic.len, topic.s, data.len, data.s);
}

class ChronoPahoSystem : public MqttClient::System {
 public:
  unsigned long millis() const {
    std::chrono::milliseconds ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    return ms.count();
  }
};

static void device_state_callback(MqttClient::MessageData& message) {
  if (strncmp((char*)message.message.payload, "sleep", message.message.payloadLen)) {
    if (sleep_state) {
      LOG(L_INFO, "Already sleeping\r\n");
    } else {
      LOG(L_INFO, "Going to sleep\r\n");
      sleep_state = true;
    }
  } else if (strncmp((char*)message.message.payload, "wakeup", message.message.payloadLen)) {
    if (!sleep_state) {
      LOG(L_INFO, "Already awake\r\n");
    } else {
      LOG(L_INFO, "Waking up\r\n");
      sleep_state = false;
    }
    return;
  }

  LOG(L_WARN, "Unknown state: %.*s\r\n", message.message.payloadLen, (char*)message.message.payload);
}

static bool map_file(const char* filename, char** buf, int* len) {
  struct stat sb;
  int fd = open(filename, O_RDONLY);

  if (fd < 0) {
    return false;
  }

  fstat(fd, &sb);
  *len = sb.st_size;

  *buf = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  if (*buf == nullptr) {
    return false;
  }
  return true;
}

int main(int argc, const char** argv) {
  owl_log_set_level(L_DBG);

  if (argc != 2) {
    LOG(L_ERR,
        "Usage: sara-paho-mqtt-sample /path/to/config.json\n\n"
        "  config.json should contain a JSON object with the following members:\n"
        "   * \"device_path\" - path to serial modem device. String, required\n"
        "   * \"apn\" - APN to use. String, required\n"
        "   * \"cops\" - MNO code in short format as understood by AT+COPS command. String, optional\n"
        "   * \"mqtt\" - subobject to configure connection to an MQTT broker. Required.\n"
        "     * \"mqtt\".\"client_id\" - MQTT client ID. String, required.\n"
        "     * \"mqtt\".\"host\" - MQTT broker host. String, required.\n"
        "     * \"mqtt\".\"post\" - MQTT broker port. Integer, required.\n"
        "     * \"mqtt\".\"login\" - login on the MQTT broker. String, optional\n"
        "     * \"mqtt\".\"password\" - password on the MQTT broker. String, optional\n"
        "   * \"tls\" - subobject to configure TLS. Optional.\n"
        "     * \"tls\".\"ca_path\" - Path to server CA certificate. String, optional.\n"
        "     * \"tls\".\"cert_path\" - Path to client certificate. String, optional.\n"
        "     * \"tls\".\"pkey_path\" - Path to client private key. String, optional.\n");
    return 1;
  }

  const char* config_path = argv[1];
  str config_str;
  if (!map_file(config_path, &config_str.s, &config_str.len)) {
    LOG(L_ERR, "Failed to read configuration\n");
    return 1;
  }

  std::string config(config_str.s, config_str.len);
  munmap(config_str.s, config_str.len);

  nlohmann::json config_json;
  config_json = nlohmann::json::parse(config);

  if (!config_json.is_object()) {
    LOG(L_ERR, "Configuration file is not a json object");
    return 1;
  }

  std::string device_path;
  std::string cops;
  std::string apn;
  std::string mqtt_broker_host;
  int mqtt_broker_port;
  std::string mqtt_client_id;
  std::string mqtt_login;
  std::string mqtt_password;
  std::string tls_ca_path;
  std::string tls_cert_path;
  std::string tls_pkey_path;
  bool use_tls;

  // for (auto& el : config_json) {
  for (nlohmann::json::iterator el = config_json.begin(); el != config_json.end(); ++el) {
    if (el.key() == "device_path") {
      device_path = el.value();
    } else if (el.key() == "cops") {
      cops = el.value();
    } else if (el.key() == "apn") {
      apn = el.value();
    } else if (el.key() == "mqtt") {
      if (!el.value().is_object()) {
        LOG(L_ERR, "Config: \"mqtt\" should be an object");
        return 1;
      }
      // for (auto& mqtt_el : el.value()) {
      for (nlohmann::json::iterator mqtt_el = el.value().begin(); mqtt_el != el.value().end(); ++mqtt_el) {
        if (mqtt_el.key() == "client_id") {
          mqtt_client_id = mqtt_el.value();
        } else if (mqtt_el.key() == "login") {
          mqtt_login = mqtt_el.value();
        } else if (mqtt_el.key() == "password") {
          mqtt_password = mqtt_el.value();
        } else if (mqtt_el.key() == "host") {
          mqtt_broker_host = mqtt_el.value();
        } else if (mqtt_el.key() == "port") {
          mqtt_broker_port = mqtt_el.value();
        }
      }
    } else if (el.key() == "tls") {
      if (!el.value().is_object()) {
        LOG(L_ERR, "Config: \"tls\" should be an object");
        return 1;
      }

      use_tls = true;

      // for (auto& tls_el : el.value()) {
      for (nlohmann::json::iterator tls_el = el.value().begin(); tls_el != el.value().end(); ++tls_el) {
        if (tls_el.key() == "ca_path") {
          tls_ca_path = tls_el.value();
        } else if (tls_el.key() == "cert_path") {
          tls_cert_path = tls_el.value();
        } else if (tls_el.key() == "pkey_path") {
          tls_pkey_path = tls_el.value();
        }
      }
    }
  }

  str tls_cert;
  str tls_pkey;
  str tls_cacert;

  if (!tls_ca_path.empty()) {
    if (!map_file(tls_ca_path.c_str(), &tls_cacert.s, &tls_cacert.len)) {
      LOG(L_ERR, "Failed to read server CA certificate\n");
      return 1;
    }
  }

  if (!tls_cert_path.empty()) {
    if (!map_file(tls_cert_path.c_str(), &tls_cert.s, &tls_cert.len)) {
      LOG(L_ERR, "Failed to device certificate\n");
      return 1;
    }
  }

  if (!tls_pkey_path.empty()) {
    if (!map_file(tls_pkey_path.c_str(), &tls_pkey.s, &tls_pkey.len)) {
      LOG(L_ERR, "Failed to read device private key\n");
      return 1;
    }
  }

  owl_log_set_level(L_DBG);
  CharDeviceSerial serial(device_path.c_str(), 115200);
  OwlModemRN4 rn4(&serial);

  if (!rn4.powerOn()) {
    LOG(L_ERR, "Failed to power on the modem\n");
    return 1;
  }

  bool res;
  if (!cops.empty()) {
    res = rn4.initModem(INIT_FLAGS, apn.c_str(), cops.c_str(), AT_COPS__Format__Numeric);
  } else {
    res = rn4.initModem(INIT_FLAGS, apn.c_str());
  }

  if (!res) {
    LOG(L_ERR, "Failed to initialize the modem\n");
    return 1;
  }

  LOG(L_ERR, "Modem initialized, waiting for network registration\n");

  if (!rn4.waitForNetworkRegistration("Telemetry", 0)) {
    LOG(L_ERR, "Failed to register on network\n");
    return 1;
  }

  if (use_tls) {
    if (!rn4.ssl.initContext()) {
      LOG(L_ERR, "Failed to initialize TLS context\n");
      return 1;
    }

    if (!rn4.ssl.setDeviceCert(tls_cert)) {
      LOG(L_ERR, "Failed to set device certificate\n");
      return 1;
    }

    if (!rn4.ssl.setDevicePkey(tls_pkey)) {
      LOG(L_ERR, "Failed to set device private key\n");
      return 1;
    }

    if (!rn4.ssl.setServerCA(tls_cacert)) {
      LOG(L_ERR, "Failed to set device private key\n");
      return 1;
    }
  }
  munmap(tls_cacert.s, tls_cacert.len);
  munmap(tls_cert.s, tls_cert.len);
  munmap(tls_pkey.s, tls_pkey.len);

  std::unique_ptr<RN4PahoIPStack> ip_stack        = std::make_unique<RN4PahoIPStack>(&rn4.socket);
  std::unique_ptr<MqttClient::System> paho_system = std::make_unique<ChronoPahoSystem>();
  PahoMQTTMassiveClient mqtt_client(std::move(ip_stack), std::move(paho_system), mqtt_broker_host, mqtt_broker_port,
                                    mqtt_client_id, mqtt_login, mqtt_password, (use_tls) ? 0 : -1);

  mqtt_client.subscribe("state", device_state_callback);

  DummyDataSource data_source;
  for (;;) {
    rn4.AT.spin();
    mqtt_client.spin();

    if (!sleep_state && data_source.ready()) {
      nlohmann::json mqtt_json;
      mqtt_json["device"] = mqtt_client_id;
      mqtt_json["data"]   = data_source.data();
      mqtt_client.publish("data", mqtt_json.dump());
    }

    mqtt_client.yield(200);
  }

  return 0;
}

// TODO: find a better way to insert a test point
void spinProcessLineTestpoint(str line) {
  return;
}

#endif  // ARDUINO
