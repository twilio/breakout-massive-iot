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

#include "catch.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include "modem/OwlModemRN4.h"

extern OwlModemRN4* rn4;

constexpr uint16_t OUT_PORT = 1111;
constexpr uint16_t IN_PORT  = 2222;

class TestSocketGuard {
 public:
  TestSocketGuard(uso_protocol protocol, uint16_t local_port) {
    REQUIRE(rn4->socket.open(protocol, local_port, &sock) == 1);
  }

  uint8_t get_id() {
    return sock;
  }

  ~TestSocketGuard() {
    REQUIRE(rn4->socket.close(sock) == SocketError::OK);
  }

 private:
  uint8_t sock;
};

static void wait_for_ip(std::string& addr_str) {
  addr_str = "";
  REQUIRE(rn4->waitForNetworkRegistration("Telemetry", Testing__Timeout_Network_Registration_30_Sec) == 1);
  auto ts = std::chrono::system_clock::now();
  for (;;) {
    uint8_t ipv4[4];
    uint8_t ipv6[16];
    bool no_v4 = true;
    bool no_v6 = true;

    REQUIRE(rn4->pdn.getAPNIPAddress(1, ipv4, ipv6) == 1);

    for (int i = 0; i < 4; i++) {
      if (ipv4[i] != 0x00) {
        no_v4 = false;
      }
    }

    for (int i = 0; i < 16; i++) {
      if (ipv6[i] != 0x00) {
        no_v6 = false;
      }
    }

    if (!no_v4) {
      addr_str = std::to_string(ipv4[0]) + "." + std::to_string(ipv4[1]) + "." + std::to_string(ipv4[2]) + "." +
                 std::to_string(ipv4[3]);
      break;
    } else if (!no_v6) {
      char buf[3];
      for (int i = 0; i < 16; i += 2) {
        sprintf(buf, "%02X", ipv6[i]);
        addr_str += std::string(buf);
        sprintf(buf, "%02X", ipv6[i + 1]);
        addr_str += std::string(buf) + ":";
      }
      addr_str = addr_str.substr(0, addr_str.size() - 1);  // drop the last ':'
      break;
    } else {
      std::chrono::duration<float> dt = std::chrono::system_clock::now() - ts;

      if (dt.count() >= 30.0) {
        std::cerr << "Timeout waiting for IP address";
        REQUIRE_FALSE(true);
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
    }
  }
}

static bool is_accepted = false;
static uint8_t accepted_sock;

void accept_handler(uint8_t sock, str remote_ip, uint16_t remote_port, uint8_t sock_list, str local_ip,
                    uint16_t local_port, void* priv) {
  if (is_accepted) {
    return;
  }

  is_accepted   = true;
  accepted_sock = sock;
}

TEST_CASE("UDP", "[SocketRN4]") {
  std::string addr_str;
  wait_for_ip(addr_str);

  str addr_s = {(char*)addr_str.c_str(), (unsigned int)addr_str.size()};

  TestSocketGuard s_out(uso_protocol::UDP, OUT_PORT);
  TestSocketGuard s_in(uso_protocol::UDP, IN_PORT);
  REQUIRE(rn4->socket.connect(s_out.get_id(), addr_s, IN_PORT) == SocketError::OK);

  str udp_data      = {"Hello!", 6};
  int udp_data_sent = 0;
  REQUIRE(rn4->socket.send(s_out.get_id(), udp_data, &udp_data_sent) == SocketError::OK);
  REQUIRE(udp_data_sent == 6);

  auto ts = std::chrono::system_clock::now();

  for (;;) {
    char udp_in_data_buf[6];
    str_mut udp_in_data{udp_in_data_buf, 0};
    REQUIRE(rn4->socket.receiveFrom(s_in.get_id(), 6, nullptr, nullptr, &udp_in_data) == SocketError::OK);

    if (udp_in_data.len == 6) {
      REQUIRE(std::string(udp_in_data.s, udp_in_data.len) == "Hello!");
      break;
    } else {
      std::chrono::duration<float> dt = std::chrono::system_clock::now() - ts;

      if (dt.count() >= 30.0) {
        std::cerr << "Timeout waiting for UDP data";
        REQUIRE_FALSE(true);
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
    }
  }
}

TEST_CASE("TCP", "[SocketRN4]") {
  std::string addr_str;
  wait_for_ip(addr_str);

  str addr_s = {(char*)addr_str.c_str(), (unsigned int)addr_str.size()};

  TestSocketGuard s_out(uso_protocol::TCP, OUT_PORT);
  TestSocketGuard s_in(uso_protocol::TCP, 0);
  REQUIRE(rn4->socket.acceptTCP(s_in.get_id(), IN_PORT, accept_handler, nullptr) == SocketError::OK);
  REQUIRE(rn4->socket.connect(s_out.get_id(), addr_s, IN_PORT) == SocketError::OK);

  str tcp_data      = {"Hello!", 6};
  int tcp_data_sent = 0;
  REQUIRE(rn4->socket.send(s_out.get_id(), tcp_data, &tcp_data_sent) == SocketError::OK);
  REQUIRE(tcp_data_sent == 6);

  auto ts = std::chrono::system_clock::now();

  for (;;) {
    if (!is_accepted) {
      rn4->AT.spin();
    } else {
      char tcp_in_data_buf[6];
      str tcp_in_data{tcp_in_data_buf, 0};
      str_mut tcp_in_data_chunk{tcp_in_data_buf, 0};
      REQUIRE(rn4->socket.receive(accepted_sock, 6 - tcp_in_data.len, &tcp_in_data_chunk) == SocketError::OK);
      tcp_in_data.len += tcp_in_data_chunk.len;
      tcp_in_data_chunk.s += tcp_in_data_chunk.len;
      tcp_in_data_chunk.len = 0;

      if (tcp_in_data.len >= 6) {
        REQUIRE(std::string(tcp_in_data.s, 6) == "Hello!");
        break;
      }
    }

    std::chrono::duration<float> dt = std::chrono::system_clock::now() - ts;

    if (dt.count() >= 30.0) {
      std::cerr << "Timeout waiting for TCP data";
      REQUIRE_FALSE(true);
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }
}

TEST_CASE("Error codes", "[SocketRN4]") {
  std::string addr_str;
  wait_for_ip(addr_str);

  str addr_s    = {(char*)addr_str.c_str(), (unsigned int)addr_str.size()};
  str data      = {"Hello!", 6};
  int data_sent = 0;

  char in_data_buf[6];
  str_mut in_data{in_data_buf, 0};

  // SocketError::BadParameter
  REQUIRE(rn4->socket.close(OwlModemSocketRN4::MAX_SOCKETS) == SocketError::BadParameter);
  REQUIRE(rn4->socket.enableTLS(OwlModemSocketRN4::MAX_SOCKETS, 0) == SocketError::BadParameter);
  REQUIRE(rn4->socket.enableTLS(0, 5) == SocketError::BadParameter);
  REQUIRE(rn4->socket.disableTLS(OwlModemSocketRN4::MAX_SOCKETS) == SocketError::BadParameter);
  REQUIRE(rn4->socket.connect(OwlModemSocketRN4::MAX_SOCKETS, addr_s, IN_PORT) == SocketError::BadParameter);
  REQUIRE(rn4->socket.send(OwlModemSocketRN4::MAX_SOCKETS, data, &data_sent) == SocketError::BadParameter);
  data.len = 1024;
  REQUIRE(rn4->socket.send(0, data, &data_sent) == SocketError::BadParameter);
  data.len = 6;
  REQUIRE(rn4->socket.sendTo(OwlModemSocketRN4::MAX_SOCKETS, addr_s, IN_PORT, data) == SocketError::BadParameter);
  data.len = 1024;
  REQUIRE(rn4->socket.sendTo(0, addr_s, IN_PORT, data) == SocketError::BadParameter);
  data.len = 6;
  REQUIRE(rn4->socket.getQueuedForReceive(OwlModemSocketRN4::MAX_SOCKETS, nullptr) == SocketError::BadParameter);
  REQUIRE(rn4->socket.receive(OwlModemSocketRN4::MAX_SOCKETS, 6, &in_data) == SocketError::BadParameter);
  REQUIRE(rn4->socket.receive(0, 1024, &in_data) == SocketError::BadParameter);
  REQUIRE(rn4->socket.receiveFrom(OwlModemSocketRN4::MAX_SOCKETS, 6, nullptr, nullptr, &in_data) ==
          SocketError::BadParameter);
  REQUIRE(rn4->socket.receiveFrom(0, 1024, nullptr, nullptr, &in_data) == SocketError::BadParameter);
  REQUIRE(rn4->socket.bindLocalUDP(OwlModemSocketRN4::MAX_SOCKETS, OUT_PORT) == SocketError::BadParameter);
  REQUIRE(rn4->socket.acceptTCP(OwlModemSocketRN4::MAX_SOCKETS, OUT_PORT, accept_handler, nullptr) ==
          SocketError::BadParameter);
  REQUIRE(rn4->socket.acceptTCP(0, OUT_PORT, nullptr, nullptr) == SocketError::BadParameter);

  // SocketError::Closed
  REQUIRE(rn4->socket.close(0) == SocketError::Closed);
  REQUIRE(rn4->socket.connect(0, addr_s, IN_PORT) == SocketError::Closed);
  REQUIRE(rn4->socket.send(0, data, &data_sent) == SocketError::Closed);
  REQUIRE(rn4->socket.sendTo(0, addr_s, IN_PORT, data) == SocketError::Closed);
  REQUIRE(rn4->socket.receive(0, 6, &in_data) == SocketError::Closed);
  REQUIRE(rn4->socket.receiveFrom(0, 6, nullptr, nullptr, &in_data) == SocketError::Closed);
  REQUIRE(rn4->socket.acceptTCP(0, OUT_PORT, accept_handler, nullptr) == SocketError::Closed);

  {
    TestSocketGuard s_udp_out(uso_protocol::UDP, 0);
    TestSocketGuard s_tcp_out(uso_protocol::TCP, 0);

    // SocketError::NotConnected
    REQUIRE(rn4->socket.send(s_udp_out.get_id(), data, &data_sent) == SocketError::NotConnected);
    REQUIRE(rn4->socket.receive(s_udp_out.get_id(), 6, &in_data) == SocketError::NotConnected);

    // SocketError::BadProtocol
    REQUIRE(rn4->socket.sendTo(s_tcp_out.get_id(), addr_s, IN_PORT, data) == SocketError::BadProtocol);
    REQUIRE(rn4->socket.receiveFrom(s_tcp_out.get_id(), 6, nullptr, nullptr, &in_data) == SocketError::BadProtocol);
    REQUIRE(rn4->socket.bindLocalUDP(s_tcp_out.get_id(), OUT_PORT) == SocketError::BadProtocol);
    REQUIRE(rn4->socket.acceptTCP(s_udp_out.get_id(), OUT_PORT, accept_handler, nullptr) == SocketError::BadProtocol);
  }

  {
    TestSocketGuard s_udp_out(uso_protocol::UDP, OUT_PORT);
    TestSocketGuard s_tcp_out(uso_protocol::TCP, OUT_PORT);

    // SocketError::AlreadyBound
    REQUIRE(rn4->socket.bindLocalUDP(s_udp_out.get_id(), OUT_PORT) == SocketError::AlreadyBound);
    REQUIRE(rn4->socket.acceptTCP(s_tcp_out.get_id(), OUT_PORT, accept_handler, nullptr) == SocketError::AlreadyBound);
  }

  {
    TestSocketGuard s_tcp_out(uso_protocol::TCP, 0);

    REQUIRE(rn4->socket.acceptTCP(s_tcp_out.get_id(), OUT_PORT, accept_handler, nullptr) == SocketError::OK);
    REQUIRE(rn4->socket.acceptTCP(s_tcp_out.get_id(), OUT_PORT, accept_handler, nullptr) ==
            SocketError::AlreadyAccepting);
  }

  {
    TestSocketGuard s_udp_out(uso_protocol::UDP, 0);

    // SocketError::AlreadyConnected
    REQUIRE(rn4->socket.connect(s_udp_out.get_id(), addr_s, IN_PORT) == SocketError::OK);
    REQUIRE(rn4->socket.connect(s_udp_out.get_id(), addr_s, IN_PORT) == SocketError::AlreadyConnected);
    REQUIRE(rn4->socket.sendTo(s_udp_out.get_id(), addr_s, IN_PORT, data) == SocketError::AlreadyConnected);
  }
}

#endif  // ARDUINO
