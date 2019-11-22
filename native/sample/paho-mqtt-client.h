#ifndef PAHO_MQTT_CLIENT_H
#define PATH_MQTT_CLIENT_H

#include <string>
#include <vector>
#include <MqttClient.h>

#include <shims/paho-mqtt/RN4PahoIPStack.h>
#include <platform/log.h>

class PahoMQTTMassiveLog {
 public:
  void println(const char* buf) {
    LOG(L_INFO, "%s\r\n", buf);
  }
};

class PahoMQTTMassiveClient {
 public:
  static constexpr int MAX_MQTT_PACKET_SIZE   = 500;
  static constexpr int MAX_MQTT_SUBSCRIPTIONS = 2;

  PahoMQTTMassiveClient(std::unique_ptr<RN4PahoIPStack> ip_stack_in, std::unique_ptr<MqttClient::System> system_in,
                        const std::string& broker_host_in, uint16_t broker_port_in, const std::string& client_id_in,
                        const std::string& login_in, const std::string& password_in, int tls_profile_in = -1)
      : ip_stack(std::move(ip_stack_in)),
        system(std::move(system_in)),
        broker_host(broker_host_in),
        broker_port(broker_port_in),
        client_id(client_id_in),
        login(login_in),
        password(password_in),
        tls_profile(tls_profile_in),
        mqtt_logger(log),
        mqtt_network(*ip_stack, *system),
        mqtt_options{.commandTimeoutMs = 10000},
        mqtt(mqtt_options, mqtt_logger, *system, mqtt_network, mqtt_send_buffer, mqtt_recv_buffer,
             mqtt_message_handlers) {
    ;
  }

  bool spin() {
    if (!ip_stack->connected()) {
      if (mqtt.isConnected()) {
        mqtt.disconnect();
      }

      if (!connect()) {
        LOG(L_WARN, "Failed to connect to MQTT broker");
      }

      return false;
    } else if (!mqtt.isConnected()) {
      ip_stack->disconnect();
      return false;
    }

    return true;
  }

  void yield(int time) {
    mqtt.yield(time);
  }

  bool publish(const std::string& topic, const std::string& data) {
    MqttClient::Message message;
    message.qos        = MqttClient::QOS0;
    message.retained   = false;
    message.dup        = false;
    message.payload    = (void*)data.c_str();
    message.payloadLen = data.length();

    return (mqtt.publish(topic.c_str(), message) == 0);
  }

  bool connect() {
    bool stack_res = false;
    if (tls_profile >= 0) {
      stack_res = ip_stack->connect(broker_host.c_str(), broker_port, true, tls_profile);
    } else {
      stack_res = ip_stack->connect(broker_host.c_str(), broker_port, false);
    }

    if (!stack_res) {
      LOG(L_WARN, "Failed to establish connection to the broker\r\n");
      return false;
    }

    MqttClient::ConnectResult connectResult;
    MQTTPacket_connectData options = MQTTPacket_connectData_initializer;
    options.MQTTVersion            = 4;
    options.clientID.cstring       = (char*)client_id.c_str();
    options.cleansession           = true;
    if (!login.empty()) {
      options.username.cstring = (char*)login.c_str();
    }

    if (!password.empty()) {
      options.password.cstring = (char*)password.c_str();
    }

    options.keepAliveInterval = 20;

    MqttClient::Error::type rc = mqtt.connect(options, connectResult);
    if (rc != MqttClient::Error::SUCCESS) {
      LOG(L_WARN, "Failed to connect to MQTT broker, error: %i\r\n", rc);
      return false;
    }

    for (auto& sub : subscriptions) {
      MqttClient::Error::type rc = mqtt.subscribe(sub.first.c_str(), MqttClient::QOS0, sub.second);
      if (rc != MqttClient::Error::SUCCESS) {
        LOG(L_WARN, "Failed to subscribe to \"%s\" topic: %i\r\n", sub.first.c_str(), rc);
        return false;
      }
    }

    return true;
  }

  void subscribe(std::string topic, MqttClient::MessageHandlerCbk handler) {
    subscriptions.push_back(std::make_pair(topic, handler));
  }

 private:
  std::unique_ptr<RN4PahoIPStack> ip_stack;
  std::unique_ptr<MqttClient::System> system;
  std::string broker_host;
  uint16_t broker_port = 0xffff;
  std::string client_id;
  std::string login;
  std::string password;
  int tls_profile = -1;
  PahoMQTTMassiveLog log;
  MqttClient::LoggerImpl<PahoMQTTMassiveLog> mqtt_logger;
  MqttClient::NetworkImpl<RN4PahoIPStack> mqtt_network;
  MqttClient::Options mqtt_options;
  MqttClient::ArrayBuffer<MAX_MQTT_PACKET_SIZE> mqtt_send_buffer;
  MqttClient::ArrayBuffer<MAX_MQTT_PACKET_SIZE> mqtt_recv_buffer;
  MqttClient::MessageHandlersImpl<MAX_MQTT_SUBSCRIPTIONS> mqtt_message_handlers;
  MqttClient mqtt;

  std::vector<std::pair<std::string, MqttClient::MessageHandlerCbk> > subscriptions;
};

#endif  // PAHO_MQTT_CLIENT_H
