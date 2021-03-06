/*
 * OwlModemSocketRN4.cpp
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
 * \file OwlModemSocketRN4.cpp - API for TCP/UDP communication over sockets
 */

#include "OwlModemSocketRN4.h"

#include <stdio.h>

void OwlModemSocketRN4Status::setOpened(uso_protocol proto) {
  is_opened    = 1;
  is_connected = 0;

  len_outstanding_receive_data     = 0;
  len_outstanding_receivefrom_data = 0;

  protocol = proto;

  handler_UDPData      = nullptr;
  handler_TCPData      = nullptr;
  handler_TCPAccept    = nullptr;
  handler_SocketClosed = nullptr;
}

void OwlModemSocketRN4Status::setClosed() {
  is_opened    = 0;
  is_connected = 0;

  // not blanking the values on purpose, because maybe some data was still queued
  //  len_outstanding_receive_data     = 0;
  //  len_outstanding_receivefrom_data = 0;

  protocol = uso_protocol::none;

  handler_UDPData      = nullptr;
  handler_TCPData      = nullptr;
  handler_TCPAccept    = nullptr;
  handler_SocketClosed = nullptr;
}

static char URC_ID[] = "Socket";
OwlModemSocketRN4::OwlModemSocketRN4(OwlModemAT *atModem) : atModem_(atModem) {
  for (uint8_t socket = 0; socket < MODEM_MAX_SOCKETS; socket++)
    status[socket].setClosed();

  if (atModem_ != nullptr) {
    atModem_->registerUrcHandler(URC_ID, OwlModemSocketRN4::processURC, this);
  }
}



static str s_uusoco = STRDECL("+UUSOCO");

bool OwlModemSocketRN4::processURCConnected(str urc, str data) {
  if (!str_equal(urc, s_uusoco)) {
    return false;
  }
  str token        = {0};
  uint8_t socket   = 0;
  int socket_error = 0;
  for (int i = 0; str_tok(data, ",", &token); i++) {
    switch (i) {
      case 0:
        socket = str_to_uint32_t(token, 10);
        break;
      case 1:
        socket_error = str_to_long_int(token, 10);
        break;
      default:
        break;
    }
  }
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
  } else {
    if (socket_error == 0) {
      this->status[socket].is_connected = 1;
      LOG(L_INFO, "Connected event on socket %d reported success\r\n", socket);
    } else {
      this->status[socket].is_connected = 0;
      LOG(L_ERR, "Connected event on socket %d reported socket error %d\r\n", socket, socket_error);
    }
  }
  return true;
}


static str s_uusocl = STRDECL("+UUSOCL");

bool OwlModemSocketRN4::processURCClosed(str urc, str data) {
  if (!str_equal(urc, s_uusocl)) {
    return false;
  }
  str token      = {0};
  uint8_t socket = 0;
  for (int i = 0; str_tok(data, ",", &token); i++) {
    switch (i) {
      case 0:
        socket = str_to_uint32_t(token, 10);
        break;
      default:
        break;
    }
  }
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "handleClosed()  Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
  } else {
    this->status[socket].is_connected = 0;
    if (!this->status[socket].handler_SocketClosed)
      LOG(L_NOTICE,
          "Received URC socket-closed for socket %d. Set a handler when you call connect(), acceptTCP(), etc"
          " if you wish to receive this event in your application\r\n",
          socket);
    else
      (this->status[socket].handler_SocketClosed)(socket, this->status[socket].handler_SocketClosed_priv);
  }
  return true;
}


static str s_uusoli = STRDECL("+UUSOLI");

bool OwlModemSocketRN4::processURCTCPAccept(str urc, str data) {
  if (!str_equal(urc, s_uusoli)) {
    return false;
  }

  str token                = {0};
  uint8_t new_socket       = 0;
  str remote_ip            = {0};
  uint16_t remote_port     = 0;
  uint8_t listening_socket = 0;
  str local_ip             = {0};
  uint16_t local_port      = 0;
  for (int i = 0; str_tok(data, ",", &token); i++) {
    switch (i) {
      case 0:
        new_socket = str_to_uint32_t(token, 10);
        break;
      case 1:
        remote_ip = token;
        if (remote_ip.len >= 2 && remote_ip.s[0] == '\"' && remote_ip.s[remote_ip.len - 1] == '\"') {
          remote_ip.s += 1;
          remote_ip.len -= 2;
        }
        break;
      case 2:
        remote_port = (uint16_t)str_to_uint32_t(token, 10);
        break;
      case 3:
        listening_socket = str_to_uint32_t(token, 10);
        break;
      case 4:
        local_ip = token;
        if (local_ip.len >= 2 && local_ip.s[0] == '\"' && local_ip.s[local_ip.len - 1] == '\"') {
          local_ip.s += 1;
          local_ip.len -= 2;
        }
        break;
      case 5:
        local_port = (uint16_t)str_to_uint32_t(token, 10);
        break;
      default:
        break;
    }
  }
  if (new_socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad new_socket %d >= %d\r\n", new_socket, MODEM_MAX_SOCKETS);
  } else if (listening_socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad listening_socket %d >= %d\r\n", listening_socket, MODEM_MAX_SOCKETS);
  } else {
    this->status[new_socket].setOpened(uso_protocol::TCP);
    this->status[new_socket].is_connected              = 1;
    this->status[new_socket].handler_TCPData           = this->status[listening_socket].handler_TCPData;
    this->status[new_socket].handler_TCPData_priv      = this->status[listening_socket].handler_TCPData_priv;
    this->status[new_socket].handler_SocketClosed      = this->status[listening_socket].handler_SocketClosed;
    this->status[new_socket].handler_SocketClosed_priv = this->status[listening_socket].handler_SocketClosed_priv;

    if (!this->status[listening_socket].handler_TCPAccept)
      LOG(L_NOTICE,
          "Received URC for TCP-Accept on listening-socket %d local-ip %.*s, from %.*s:%u new socket %d. Set a handler "
          "when calling acceptTCP(), openAcceptTCP(), etc if you wish to receive this event in your application\r\n",
          (int)listening_socket, local_ip.len, local_ip.s, remote_ip.len, remote_ip.s, (unsigned int)remote_port,
          (int)new_socket);
    else
      (this->status[listening_socket].handler_TCPAccept)(new_socket, remote_ip, remote_port, listening_socket, local_ip,
                                                         local_port,
                                                         this->status[listening_socket].handler_TCPAccept_priv);
  }
  return true;
}


static str s_uusord = STRDECL("+UUSORD");

bool OwlModemSocketRN4::processURCReceive(str urc, str data) {
  if (!str_equal(urc, s_uusord)) {
    return false;
  }

  str token      = {0};
  uint8_t socket = 0;
  uint16_t len   = 0;
  for (int i = 0; str_tok(data, ",", &token); i++) {
    switch (i) {
      case 0:
        socket = str_to_uint32_t(token, 10);
        break;
      case 1:
        len = str_to_uint32_t(token, 10);
        break;
      default:
        break;
    }
  }
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
  } else {
    if (!this->status[socket].is_connected) {
      LOG(L_WARN, "Received +UUSORD event on socket %d which is not connected\r\n", socket);
    }
    this->status[socket].len_outstanding_receive_data = len;
    LOG(L_INFO, "Receive URC for queued received data on socket %d of %d bytes\r\n", socket, len);
  }
  return true;
}


static str s_uusorf = STRDECL("+UUSORF");

bool OwlModemSocketRN4::processURCReceiveFrom(str urc, str data) {
  if (!str_equal(urc, s_uusorf)) {
    return false;
  }

  str token      = {0};
  uint8_t socket = 0;
  uint16_t len   = 0;
  for (int i = 0; str_tok(data, ",", &token); i++) {
    switch (i) {
      case 0:
        socket = str_to_uint32_t(token, 10);
        break;
      case 1:
        len = str_to_uint32_t(token, 10);
        break;
      default:
        break;
    }
  }
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
  } else {
    this->status[socket].len_outstanding_receivefrom_data = len;
    LOG(L_INFO, "Receive URC for queued received-from data on socket %d of %d bytes\r\n", socket, len);
  }
  return true;
}


bool OwlModemSocketRN4::processURC(str urc, str data, void *instance) {
  OwlModemSocketRN4 *inst = reinterpret_cast<OwlModemSocketRN4 *>(instance);

  /* ordered based on the expected frequency of arrival */
  return inst->processURCReceiveFrom(urc, data) || inst->processURCReceive(urc, data) ||
         inst->processURCTCPAccept(urc, data) || inst->processURCConnected(urc, data) ||
         inst->processURCClosed(urc, data);
}



void OwlModemSocketRN4::handleWaitingData() {
  LOG(L_MEM, "Starting handleWaitingData\r\n");

  char buf[64];
  str_mut remote_ip    = {.s = buf, .len = 0};
  uint16_t remote_port = 0;
  int data_len         = 0;

  for (uint8_t socket = 0; socket < MODEM_MAX_SOCKETS; socket++) {
    /* Receive-From - UDP */
    if (status[socket].len_outstanding_receivefrom_data) {
      remote_ip.len = 0;
      remote_port   = 0;
      switch (status[socket].protocol) {
        case uso_protocol::UDP:
          data_len = status[socket].len_outstanding_receivefrom_data;
          /* receive might include an event for the next data, so reset the current value now */
          status[socket].len_outstanding_receivefrom_data = 0;
          if (receiveFromUDP(socket, data_len, &remote_ip, &remote_port, &udp_data, MODEM_UDP_BUFFER_SIZE)) {
            if (status[socket].handler_UDPData)
              (status[socket].handler_UDPData)(socket, remote_ip, remote_port, udp_data,
                                               status[socket].handler_UDPData_priv);
            else
              LOG(L_NOTICE, "ReceiveFrom on socket %u UDP Data of %u bytes without handler - ignored\r\n", socket,
                  udp_data.len);
          } else {
            /* Should we reset the indicator here and retry? Maybe that's an infinite loop, so probably not */
          }
          break;
        default:
          LOG(L_ERR, "Received on socket %u with bad protocol %d - ignored\r\n", socket, status[socket].protocol);
          status[socket].len_outstanding_receivefrom_data = 0;
          break;
      }
    }

    /* Receive - UDP or TCP */
    if (status[socket].len_outstanding_receive_data) {
      remote_ip.len = 0;
      remote_port   = 0;
      switch (status[socket].protocol) {
        case uso_protocol::UDP:
          data_len = status[socket].len_outstanding_receive_data;
          /* receive might include an event for the next data, so reset the current value now */
          status[socket].len_outstanding_receive_data = 0;
          if (receiveUDP(socket, data_len, &udp_data, MODEM_UDP_BUFFER_SIZE)) {
            if (status[socket].handler_UDPData)
              (status[socket].handler_UDPData)(socket, remote_ip, remote_port, udp_data,
                                               status[socket].handler_UDPData_priv);
            else
              LOG(L_NOTICE, "Receive on socket %u UDP Data of %u bytes without handler - ignored\r\n", socket,
                  udp_data.len);
          } else {
            /* Should we reset the indicator here and retry? Maybe that's an infinite loop, so probably not */
          }
          break;
        case uso_protocol::TCP:
          data_len = status[socket].len_outstanding_receive_data;
          /* receive might include an event for the next data, so reset the current value now */
          status[socket].len_outstanding_receive_data -= data_len;
          if (receiveTCP(socket, status[socket].len_outstanding_receivefrom_data, &udp_data, MODEM_UDP_BUFFER_SIZE)) {
            if (status[socket].handler_TCPData)
              (status[socket].handler_TCPData)(socket, udp_data, status[socket].handler_TCPData_priv);
            else
              LOG(L_NOTICE, "Received on socket %u TCP Data of %u bytes without handler - ignored\r\n", socket,
                  udp_data.len);

            status[socket].len_outstanding_receive_data -= udp_data.len;
            if (status[socket].len_outstanding_receive_data < 0) {
              LOG(L_ERR, "Bad len_outstanding_receive_data calculation %d < 0\r\n",
                  status[socket].len_outstanding_receive_data);
              status[socket].len_outstanding_receive_data = 0;
            }
          }
          break;
        default:
          LOG(L_ERR, "Received on socket %u with bad protocol %d - ignored\r\n", socket, status[socket].protocol);
          status[socket].len_outstanding_receive_data = 0;
          break;
      }
    }
  }
  LOG(L_MEM, "Done handleWaitingData\r\n");
}


static str s_usocr = STRDECL("+USOCR: ");

int OwlModemSocketRN4::open(uso_protocol protocol, uint16_t local_port, uint8_t *out_socket) {
  if (out_socket) *out_socket = 255;
  int socket = 255;

  if (local_port != 0) {
    atModem_->commandSprintf("AT+USOCR=%d,%u", protocol, local_port);
  } else {
    atModem_->commandSprintf("AT+USOCR=%d", protocol);
  }
  int result = atModem_->doCommandBlocking(3000, &socket_response) == at_result_code::OK;
  if (!result) return 0;
  OwlModemAT::filterResponse(s_usocr, socket_response, &socket_response);
  socket = str_to_uint32_t(socket_response, 10);
  if (out_socket) *out_socket = socket;

  if (socket < MODEM_MAX_SOCKETS) {
    this->status[socket].setOpened(protocol);
  }

  return result;
}

int OwlModemSocketRN4::close(uint8_t socket) {
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
    return 0;
  }
  switch (status[socket].protocol) {
    case uso_protocol::TCP:
      atModem_->commandSprintf("AT+USOCL=%u,0", socket);
      break;
    case uso_protocol::UDP:
    default:
      atModem_->commandSprintf("AT+USOCL=%u", socket);
      break;
  }
  int result = (atModem_->doCommandBlocking(120 * 1000, nullptr) == at_result_code::OK);
  if (!result) return 0;

  this->status[socket].setClosed();

  return result;
}

int OwlModemSocketRN4::enableTLS(uint8_t socket, int tls_id) {
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
    return 0;
  }

  if (tls_id > 4 || tls_id < 0) {
    LOG(L_ERR, "Bad TLS profile id %d\r\n", tls_id);
    return 0;
  }

  atModem_->commandSprintf("AT+USOSEC=%u,1,%d", socket, tls_id);
  return (atModem_->doCommandBlocking(1000, nullptr) == at_result_code::OK);
}

int OwlModemSocketRN4::disableTLS(uint8_t socket) {
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
    return 0;
  }

  atModem_->commandSprintf("AT+USOSEC=%u,0", socket);
  return (atModem_->doCommandBlocking(1000, nullptr) == at_result_code::OK);
}

static str s_usoer = STRDECL("+USOER: ");

int OwlModemSocketRN4::getError(uso_error *out_error) {
  if (out_error) *out_error = (uso_error)-1;
  int result = (atModem_->doCommandBlocking("AT+USOER", 1000, &socket_response) == at_result_code::OK);
  if (!result) return 0;
  atModem_->filterResponse(s_usoer, socket_response, &socket_response);

  if (out_error) *out_error = (uso_error)str_to_long_int(socket_response, 10);

  return result;
}

int OwlModemSocketRN4::connect(uint8_t socket, str remote_ip, uint16_t remote_port, OwlModem_SocketClosedHandler_f cb,
                               void *cb_priv) {
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
    return 0;
  }
  if (!this->status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return 0;
  }
  this->status[socket].is_connected = 0;
  switch (this->status[socket].protocol) {
    case uso_protocol::TCP:
      atModem_->commandSprintf("AT+USOCO=%u,\"%.*s\",%u,0", socket, remote_ip.len, remote_ip.s, remote_port);
      break;
    case uso_protocol::UDP:
      atModem_->commandSprintf("AT+USOCO=%u,\"%.*s\",%u", socket, remote_ip.len, remote_ip.s, remote_port);
      break;
    default:
      LOG(L_ERR, "Socket %d has unsupported protocol %d\r\n", this->status[socket].protocol);
      return 0;
  }
  int result = (atModem_->doCommandBlocking(120 * 1000, &socket_response) == at_result_code::OK);
  if (!result) return 0;
  this->status[socket].is_connected              = 1;
  this->status[socket].handler_SocketClosed      = cb;
  this->status[socket].handler_SocketClosed_priv = cb_priv;
  return result;
}

static str s_usowr = STRDECL("+USOWR: ");

int OwlModemSocketRN4::send(uint8_t socket, str data) {
  int bytes_sent = 0;
  atModem_->commandSprintf("AT+USOWR=%u,%d,\"", socket, data.len);
  atModem_->commandAppendHex(data);
  atModem_->commandStrcat("\"");
  int result = atModem_->doCommandBlocking(120 * 1000, &socket_response) == at_result_code::OK;
  if (!result) return -1;
  OwlModemAT::filterResponse(s_usowr, socket_response, &socket_response);
  str token = {0};
  for (int i = 0; str_tok(socket_response, ",\r\n", &token); i++)
    switch (i) {
      case 0:
        // socket
        break;
      case 1:
        bytes_sent = str_to_long_int(token, 10);
        break;
      default:
        break;
    }
  return bytes_sent;
}

int OwlModemSocketRN4::sendUDP(uint8_t socket, str data, int *out_bytes_sent) {
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
    return 0;
  }
  if (data.len > 512) {
    LOG(L_ERR, "Too much data %d > max 512 bytes\r\n", data.len);
    return 0;
  }
  if (!this->status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return 0;
  }
  if (!this->status[socket].is_connected) {
    LOG(L_ERR, "Socket %d is not connected\r\n", socket);
    return 0;
  }
  if (this->status[socket].protocol != uso_protocol::UDP) {
    LOG(L_ERR, "Socket %d is not an UDP socket\r\n", socket);
    return 0;
  }
  int bytes_sent = this->send(socket, data);
  if (out_bytes_sent) *out_bytes_sent = bytes_sent;
  LOG(L_INFO, "Sent data over UDP on socket %u %d bytes\r\n", socket, bytes_sent);
  return (bytes_sent >= 0) && (static_cast<unsigned int>(bytes_sent) == data.len);
}

int OwlModemSocketRN4::sendTCP(uint8_t socket, str data, int *out_bytes_sent) {
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
    return 0;
  }
  if (data.len > 512) {
    LOG(L_ERR, "Too much data %d > max 512 bytes\r\n", data.len);
    return 0;
  }
  if (!this->status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return 0;
  }
  if (!this->status[socket].is_connected) {
    LOG(L_ERR, "Socket %d is not connected\r\n", socket);
    return 0;
  }
  if (this->status[socket].protocol != uso_protocol::TCP) {
    LOG(L_ERR, "Socket %d is not an TCP socket\r\n", socket);
    return 0;
  }
  int bytes_sent = this->send(socket, data);
  if (out_bytes_sent) *out_bytes_sent = bytes_sent;
  LOG(L_INFO, "Sent data over TCP on socket %u %d bytes\r\n", socket, bytes_sent);
  return bytes_sent > 0;
}

static str s_usost = STRDECL("+USOST: ");

int OwlModemSocketRN4::sendToUDP(uint8_t socket, str remote_ip, uint16_t remote_port, str data) {
  int bytes_sent = 0;
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
    return 0;
  }
  if (data.len > 512) {
    LOG(L_ERR, "Too much data %d > max 512 bytes\r\n", data.len);
    return 0;
  }
  if (!this->status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return 0;
  }
  if (this->status[socket].protocol != uso_protocol::UDP) {
    LOG(L_ERR, "Socket %d is not an UDP socket\r\n", socket);
    return 0;
  }
  atModem_->commandSprintf("AT+USOST=%u,\"%.*s\",%u,%d,\"", socket, remote_ip.len, remote_ip.s, remote_port, data.len);
  atModem_->commandAppendHex(data);
  atModem_->commandStrcat("\"");
  int result = atModem_->doCommandBlocking(10 * 1000, &socket_response) == at_result_code::OK;
  if (!result) return 0;
  OwlModemAT::filterResponse(s_usost, socket_response, &socket_response);
  str token = {0};
  for (int i = 0; str_tok(socket_response, ",\r\n", &token); i++)
    switch (i) {
      case 0:
        // socket
        break;
      case 1:
        bytes_sent = str_to_long_int(token, 10);
        break;
      default:
        break;
    }
  LOG(L_INFO, "Sent data over UDP on socket %u %d bytes\r\n", socket, bytes_sent);
  return (bytes_sent >= 0) && (static_cast<unsigned int>(bytes_sent) == data.len);
}

int OwlModemSocketRN4::getQueuedForReceive(uint8_t socket, int *out_receive_tcp, int *out_receive_udp,
                                           int *out_receivefrom_udp) {
  if (out_receive_tcp) *out_receive_tcp = 0;
  if (out_receive_udp) *out_receive_udp = 0;
  if (out_receivefrom_udp) *out_receivefrom_udp = 0;
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket);
    return 0;
  }
  switch (status[socket].protocol) {
    case uso_protocol::TCP:
      if (out_receive_tcp) *out_receive_tcp = status[socket].len_outstanding_receive_data;
      break;
    case uso_protocol::UDP:
      if (out_receive_udp) *out_receive_udp = status[socket].len_outstanding_receive_data;
      if (out_receivefrom_udp) *out_receivefrom_udp = status[socket].len_outstanding_receivefrom_data;
      break;
    default:
      break;
  }
  return 1;
}

static str s_usord = STRDECL("+USORD: ");

int OwlModemSocketRN4::receive(uint8_t socket, uint16_t len, str_mut *out_data, int max_data_len) {
  if (out_data) out_data->len = 0;

  atModem_->commandSprintf("AT+USORD=%u,%u", socket, len);
  int result = (atModem_->doCommandBlocking(1000, &socket_response) == at_result_code::OK);
  if (!result) {
    return 0;
  }
  OwlModemAT::filterResponse(s_usord, socket_response, &socket_response);
  str token             = {0};
  str sub               = {0};
  uint16_t received_len = 0;
  for (int i = 0; str_tok(socket_response, "\r\n,", &token); i++)
    switch (i) {
      case 0:
        // socket
        break;
      case 1:
        if (len == 0) {
          // This was a call to figure out how much data is there - re-send command with new length
          int available_data = str_to_long_int(token, 10);
          if (available_data > 0)
            return receive(socket, available_data, out_data, max_data_len);
          else
            return 1;
        }
        received_len = str_to_uint32_t(token, 10);
        break;
      case 2:
        sub = token;
        if (sub.len >= 2 && sub.s[0] == '"' && sub.s[sub.len - 1] == '"') {
          sub.s += 1;
          sub.len -= 2;
        }
        if (sub.len != received_len * 2) {
          LOG(L_ERR, "Indicator said payload has %d bytes follow, but %d hex characters found\r\n", received_len,
              sub.len);
        }
        if (sub.len) {
          out_data->len = hex_to_str(out_data->s, max_data_len, sub);
          if (!out_data->len) goto error;
        }
        break;
      default:
        break;
    }
  return 1;
error:
  LOG(L_ERR, "Bad payload\r\n");
  return 0;
}

int OwlModemSocketRN4::receiveUDP(uint8_t socket, uint16_t len, str_mut *out_data, int max_data_len) {
  if (out_data) out_data->len = 0;
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket);
    return 0;
  }
  if (len > 512) {
    LOG(L_ERR,
        "Unfortunately, only supporting up to 512 binary bytes. 1024 would be possible, but in ASCII mode only\r\n",
        len);
    return 0;
  }
  if (!this->status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return 0;
  }
  if (!this->status[socket].is_connected) {
    LOG(L_ERR, "Socket %d is not connected - use receiveFromUDP\r\n", socket);
    return 0;
  }
  if (this->status[socket].protocol != uso_protocol::UDP) {
    LOG(L_ERR, "Socket %d is not an UDP socket\r\n", socket);
    return 0;
  }
  return this->receive(socket, len, out_data, max_data_len);
}

int OwlModemSocketRN4::receiveTCP(uint8_t socket, uint16_t len, str_mut *out_data, int max_data_len) {
  if (out_data) out_data->len = 0;
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket);
    return 0;
  }
  if (len > 512) {
    LOG(L_ERR,
        "Unfortunately, only supporting up to 512 binary bytes. 1024 would be possible, but in ASCII mode only\r\n",
        len);
    return 0;
  }
  if (!this->status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return 0;
  }
  if (!this->status[socket].is_connected) {
    LOG(L_ERR, "Socket %d is not connected\r\n", socket);
    return 0;
  }
  if (this->status[socket].protocol != uso_protocol::TCP) {
    LOG(L_ERR, "Socket %d is not a TCP socket\r\n", socket);
    return 0;
  }
  return this->receive(socket, len, out_data, max_data_len);
}

static str s_usorf = STRDECL("+USORF: ");

int OwlModemSocketRN4::receiveFromUDP(uint8_t socket, uint16_t len, str_mut *out_remote_ip, uint16_t *out_remote_port,
                                      str_mut *out_data, int max_data_len) {
  if (out_remote_ip) out_remote_ip->len = 0;
  if (out_remote_port) *out_remote_port = 0;
  if (out_data) out_data->len = 0;
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket);
    return 0;
  }
  if (len > 512) {
    LOG(L_ERR,
        "Unfortunately, only supporting up to 512 binary bytes. 1024 would be possible, but in ASCII mode only\r\n",
        len);
    return 0;
  }
  if (!this->status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return 0;
  }
  if (this->status[socket].protocol != uso_protocol::UDP) {
    LOG(L_ERR, "Socket %d is not an UDP socket\r\n", socket);
    return 0;
  }
  atModem_->commandSprintf("AT+USORF=%u,%u", socket, len);
  int result = (atModem_->doCommandBlocking(1000, &socket_response) == at_result_code::OK);
  if (!result) return 0;
  OwlModemAT::filterResponse(s_usorf, socket_response, &socket_response);
  str token             = {0};
  str sub               = {0};
  uint16_t received_len = 0;
  for (int i = 0; str_tok(socket_response, ",\r\n", &token); i++)
    switch (i) {
      case 0:
        // socket
        break;
      case 1:
        if (len == 0) {
          // This was a call to figure out how much data is there - re-send command with new length
          int available_data = str_to_long_int(token, 10);
          if (available_data > 0)
            return receiveFromUDP(socket, available_data, out_remote_ip, out_remote_port, out_data, max_data_len);
        }
        if (out_remote_ip) {
          sub = token;
          if (sub.len >= 2 && sub.s[0] == '"' && sub.s[sub.len - 1] == '"') {
            sub.s += 1;
            sub.len -= 2;
          }
          memcpy(out_remote_ip->s, sub.s, sub.len);
          out_remote_ip->len = sub.len;
        }
        break;
      case 2:
        if (out_remote_port) *out_remote_port = (uint16_t)str_to_uint32_t(token, 10);
        break;
      case 3:
        received_len = str_to_uint32_t(token, 10);
        break;
      case 4:
        sub = token;
        if (sub.len >= 2 && sub.s[0] == '"' && sub.s[sub.len - 1] == '"') {
          sub.s += 1;
          sub.len -= 2;
        }
        if (sub.len != received_len * 2) {
          LOG(L_ERR, "Indicator said payload has %d bytes follow, but %d hex characters found\r\n", received_len,
              sub.len);
        }
        if (sub.len) {
          out_data->len = hex_to_str(out_data->s, max_data_len, sub);
          if (!out_data->len) goto error;
        }
        break;
      default:
        break;
    }

  return 1;
error:
  LOG(L_ERR, "Bad payload\r\n");
  return 0;
}

int OwlModemSocketRN4::listenUDP(uint8_t socket, uint16_t local_port, OwlModem_UDPDataHandler_f cb, void *cb_priv) {
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
    return 0;
  }
  if (!this->status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return 0;
  }
  if (this->status[socket].protocol != uso_protocol::UDP) {
    LOG(L_ERR, "Socket %d is not an UDP socket\r\n", socket);
    return 0;
  }
  atModem_->commandSprintf("AT+USOLI=%u,%u", socket, local_port);
  int result = (atModem_->doCommandBlocking(1000, nullptr) == at_result_code::OK);
  if (!result) return 0;

  this->status[socket].handler_UDPData      = cb;
  this->status[socket].handler_UDPData_priv = cb_priv;

  return result;
}

int OwlModemSocketRN4::listenTCP(uint8_t socket, OwlModem_TCPDataHandler_f handler_tcp_data, void *handler_priv) {
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
    return 0;
  }
  if (!this->status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return 0;
  }
  if (this->status[socket].protocol != uso_protocol::TCP) {
    LOG(L_ERR, "Socket %d is not an TCP socket\r\n", socket);
    return 0;
  }

  this->status[socket].handler_TCPData      = handler_tcp_data;
  this->status[socket].handler_TCPData_priv = handler_priv;

  return 1;
}

int OwlModemSocketRN4::acceptTCP(uint8_t socket, uint16_t local_port, OwlModem_TCPAcceptHandler_f handler_tcp_accept,
                                 OwlModem_SocketClosedHandler_f handler_socket_closed,
                                 OwlModem_TCPDataHandler_f handler_tcp_data, void *handler_tcp_accept_priv,
                                 void *handler_socket_closed_priv, void *handler_tcp_data_priv) {
  if (socket >= MODEM_MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MODEM_MAX_SOCKETS);
    return 0;
  }
  if (!this->status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return 0;
  }
  if (this->status[socket].protocol != uso_protocol::TCP) {
    LOG(L_ERR, "Socket %d is not an UDP socket\r\n", socket);
    return 0;
  }
  atModem_->commandSprintf("AT+USOLI=%u,%u", socket, local_port);
  int result = atModem_->doCommandBlocking(1000, nullptr) == at_result_code::OK;
  if (!result) return 0;

  this->status[socket].handler_TCPAccept         = handler_tcp_accept;
  this->status[socket].handler_TCPAccept_priv    = handler_tcp_accept_priv;
  this->status[socket].handler_SocketClosed      = handler_socket_closed;
  this->status[socket].handler_SocketClosed_priv = handler_socket_closed_priv;
  this->status[socket].handler_TCPData           = handler_tcp_data;
  this->status[socket].handler_TCPData_priv      = handler_tcp_data_priv;
  return result;
}

int OwlModemSocketRN4::openListenUDP(uint16_t local_port, uint8_t *out_socket, OwlModem_UDPDataHandler_f handler_data,
                                     void *handler_data_priv) {
  if (out_socket) *out_socket = 255;
  uint8_t socket = 255;

  if (!this->open(uso_protocol::UDP, 0, &socket)) goto error;
  if (!this->listenUDP(socket, local_port, handler_data, handler_data_priv)) goto error;

  if (out_socket) *out_socket = socket;
  return 1;
error:
  if (socket != 255) this->close(socket);
  return 0;
}

int OwlModemSocketRN4::openConnectUDP(str remote_ip, uint16_t remote_port, uint8_t *out_socket,
                                      OwlModem_UDPDataHandler_f handler_data, void *handler_data_priv) {
  if (out_socket) *out_socket = 255;
  uint8_t socket = 255;

  if (!this->open(uso_protocol::UDP, 0, &socket)) goto error;
  if (!this->connect(socket, remote_ip, remote_port, (OwlModem_SocketClosedHandler_f)0)) goto error;

  this->status[socket].handler_UDPData      = handler_data;
  this->status[socket].handler_UDPData_priv = handler_data_priv;

  if (out_socket) *out_socket = socket;
  return 1;
error:
  if (socket != 255) this->close(socket);
  return 0;
}

int OwlModemSocketRN4::openListenConnectUDP(uint16_t local_port, str remote_ip, uint16_t remote_port,
                                            uint8_t *out_socket, OwlModem_UDPDataHandler_f handler_data,
                                            void *handler_data_priv) {
  if (out_socket) *out_socket = 255;
  uint8_t socket = 255;

  if (!this->open(uso_protocol::UDP, 0, &socket)) goto error;
  if (!this->listenUDP(socket, local_port, handler_data, handler_data_priv)) goto error;
  if (!this->connect(socket, remote_ip, remote_port, (OwlModem_SocketClosedHandler_f)0)) goto error;

  if (out_socket) *out_socket = socket;
  return 1;
error:
  if (socket != 255) this->close(socket);
  return 0;
}

int OwlModemSocketRN4::openListenConnectTCP(uint16_t local_port, str remote_ip, uint16_t remote_port,
                                            uint8_t *out_socket, OwlModem_SocketClosedHandler_f handler_close,
                                            OwlModem_TCPDataHandler_f handler_data, void *handler_close_priv,
                                            void *handler_data_priv) {
  if (out_socket) *out_socket = 255;
  uint8_t socket = 255;

  if (!this->open(uso_protocol::TCP, local_port, &socket)) goto error;
  if (!this->listenTCP(socket, handler_data, handler_data_priv)) goto error;
  if (!this->connect(socket, remote_ip, remote_port, handler_close, handler_close_priv)) goto error;

  if (out_socket) *out_socket = socket;
  return 1;
error:
  if (socket != 255) this->close(socket);
  return 0;
}

int OwlModemSocketRN4::openAcceptTCP(uint16_t local_port, uint8_t *out_socket,
                                     OwlModem_TCPAcceptHandler_f handler_accept,
                                     OwlModem_SocketClosedHandler_f handler_close,
                                     OwlModem_TCPDataHandler_f handler_data, void *handler_accept_priv,
                                     void *handler_close_priv, void *handler_data_priv) {
  if (out_socket) *out_socket = 255;
  uint8_t socket = 255;

  if (!this->open(uso_protocol::TCP, 0, &socket)) goto error;
  if (!this->acceptTCP(socket, local_port, handler_accept, handler_close, handler_data, handler_accept_priv,
                       handler_close_priv, handler_data_priv))
    goto error;

  if (out_socket) *out_socket = socket;
  return 1;
error:
  if (socket != 255) this->close(socket);
  return 0;
}
