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
  is_opened    = true;
  is_bound     = false;
  is_connected = false;
  is_accepting = false;

  len_outstanding_receive_data = 0;

  protocol = proto;

  handler_TCPAccept    = nullptr;
  handler_SocketClosed = nullptr;
}

void OwlModemSocketRN4Status::setClosed() {
  is_opened    = false;
  is_bound     = false;
  is_connected = false;
  is_accepting = false;

  len_outstanding_receive_data = 0;

  protocol = uso_protocol::none;

  handler_TCPAccept    = nullptr;
  handler_SocketClosed = nullptr;
}

static char URC_ID[] = "Socket";
OwlModemSocketRN4::OwlModemSocketRN4(OwlModemAT *atModem) : atModem_(atModem) {
  for (uint8_t socket = 0; socket < MAX_SOCKETS; socket++) {
    status[socket].setClosed();
  }

  if (atModem_ != nullptr) {
    atModem_->registerUrcHandler(URC_ID, OwlModemSocketRN4::processURC, this);
  }
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
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "handleClosed()  Bad socket %d >= %d\r\n", socket, MAX_SOCKETS);
  } else {
    status[socket].setClosed();
    if (!status[socket].handler_SocketClosed)
      LOG(L_NOTICE,
          "Received URC socket-closed for socket %d. Set a handler with setSocketClosedHandler()"
          " if you wish to receive this event in your application\r\n",
          socket);
    else
      (status[socket].handler_SocketClosed)(socket, status[socket].handler_SocketClosed_priv);
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
  if (new_socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad new_socket %d >= %d\r\n", new_socket, MAX_SOCKETS);
  } else if (listening_socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad listening_socket %d >= %d\r\n", listening_socket, MAX_SOCKETS);
  } else {
    status[new_socket].setOpened(uso_protocol::TCP);
    status[new_socket].is_connected              = true;
    status[new_socket].handler_SocketClosed      = status[listening_socket].handler_SocketClosed;
    status[new_socket].handler_SocketClosed_priv = status[listening_socket].handler_SocketClosed_priv;

    if (!status[listening_socket].handler_TCPAccept) {
      LOG(L_NOTICE,
          "Received URC for TCP-Accept on listening-socket %d local-ip %.*s, from %.*s:%u new socket %d. Set a handler "
          "when calling acceptTCP() if you wish to receive this event in your application\r\n",
          (int)listening_socket, local_ip.len, local_ip.s, remote_ip.len, remote_ip.s, (unsigned int)remote_port,
          (int)new_socket);
    } else {
      (status[listening_socket].handler_TCPAccept)(new_socket, remote_ip, remote_port, listening_socket, local_ip,
                                                   local_port, status[listening_socket].handler_TCPAccept_priv);
    }
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
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MAX_SOCKETS);
  } else {
    if (!status[socket].is_connected) {
      LOG(L_WARN, "Received +UUSORD event on socket %d which is not connected\r\n", socket);
    }
    status[socket].len_outstanding_receive_data = len;
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
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MAX_SOCKETS);
  } else {
    status[socket].len_outstanding_receive_data = len;
    LOG(L_INFO, "Receive URC for queued received-from data on socket %d of %d bytes\r\n", socket, len);
  }
  return true;
}


bool OwlModemSocketRN4::processURC(str urc, str data, void *instance) {
  OwlModemSocketRN4 *inst = reinterpret_cast<OwlModemSocketRN4 *>(instance);

  /* ordered based on the expected frequency of arrival */
  return inst->processURCReceiveFrom(urc, data) || inst->processURCReceive(urc, data) ||
         inst->processURCTCPAccept(urc, data) || inst->processURCClosed(urc, data);
}

static str s_usocr = STRDECL("+USOCR: ");

bool OwlModemSocketRN4::open(uso_protocol protocol, uint16_t local_port, uint8_t *out_socket) {
  if (out_socket) *out_socket = 255;

  atModem_->commandSprintf("AT+USOCR=%d,%u", protocol, local_port);
  if (atModem_->doCommandBlocking(3000, &socket_response) != at_result_code::OK) {
    return false;
  }

  OwlModemAT::filterResponse(s_usocr, socket_response, &socket_response);
  uint8_t socket = str_to_uint32_t(socket_response, 10);
  if (out_socket) {
    *out_socket = socket;
  }

  if (socket < MAX_SOCKETS) {
    status[socket].setOpened(protocol);
    if (local_port != 0) {
      status[socket].is_bound = true;
    }
  }

  return true;
}

SocketError OwlModemSocketRN4::close(uint8_t socket) {
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MAX_SOCKETS);
    return SocketError::BadParameter;
  }

  if (!status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return SocketError::Closed;
  }

  atModem_->commandSprintf("AT+USOCL=%u", socket);
  if (atModem_->doCommandBlocking(120 * 1000, nullptr) != at_result_code::OK) {
    return SocketError::ModemError;
  }

  status[socket].setClosed();

  return SocketError::OK;
}

SocketError OwlModemSocketRN4::enableTLS(uint8_t socket, int tls_id) {
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MAX_SOCKETS);
    return SocketError::BadParameter;
  }

  if (tls_id > 4 || tls_id < 0) {
    LOG(L_ERR, "Bad TLS profile id %d\r\n", tls_id);
    return SocketError::BadParameter;
  }

  atModem_->commandSprintf("AT+USOSEC=%u,1,%d", socket, tls_id);
  return (atModem_->doCommandBlocking(1000, nullptr) == at_result_code::OK) ? SocketError::OK : SocketError::ModemError;
}

SocketError OwlModemSocketRN4::disableTLS(uint8_t socket) {
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MAX_SOCKETS);
    return SocketError::BadParameter;
  }

  atModem_->commandSprintf("AT+USOSEC=%u,0", socket);
  return (atModem_->doCommandBlocking(1000, nullptr) == at_result_code::OK) ? SocketError::OK : SocketError::ModemError;
}

static str s_usoer = STRDECL("+USOER: ");

SocketError OwlModemSocketRN4::getError(uso_error *out_error) {
  if (out_error) {
    *out_error = (uso_error)-1;
  }

  if (atModem_->doCommandBlocking("AT+USOER", 1000, &socket_response) != at_result_code::OK) {
    return SocketError::ModemError;
  }
  atModem_->filterResponse(s_usoer, socket_response, &socket_response);

  if (out_error) {
    *out_error = (uso_error)str_to_long_int(socket_response, 10);
  }

  return SocketError::OK;
}

SocketError OwlModemSocketRN4::connect(uint8_t socket, str remote_ip, uint16_t remote_port) {
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MAX_SOCKETS);
    return SocketError::BadParameter;
  }

  if (!status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return SocketError::Closed;
  }

  if (status[socket].is_connected) {
    LOG(L_ERR, "Socket %d is already connected\r\n", socket);
    return SocketError::AlreadyConnected;
  }

  atModem_->commandSprintf("AT+USOCO=%u,\"%.*s\",%u", socket, remote_ip.len, remote_ip.s, remote_port);
  if (atModem_->doCommandBlocking(120 * 1000, &socket_response) != at_result_code::OK) {
    return SocketError::ModemError;
  }
  status[socket].is_connected = true;
  return SocketError::OK;
}

static str s_usowr = STRDECL("+USOWR: ");

SocketError OwlModemSocketRN4::send(uint8_t socket, str data, int *out_bytes_sent) {
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MAX_SOCKETS);
    return SocketError::BadParameter;
  }
  if (data.len > 512) {
    LOG(L_ERR, "Too much data %d > max 512 bytes\r\n", data.len);
    return SocketError::BadParameter;
  }
  if (!status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return SocketError::Closed;
  }
  if (!status[socket].is_connected) {
    LOG(L_ERR, "Socket %d is not connected\r\n", socket);
    return SocketError::NotConnected;
  }

  int bytes_sent = 0;
  atModem_->commandSprintf("AT+USOWR=%u,%d,\"", socket, data.len);
  atModem_->commandAppendHex(data);
  atModem_->commandStrcat("\"");
  if (atModem_->doCommandBlocking(120 * 1000, &socket_response) != at_result_code::OK) {
    return SocketError::ModemError;
  };

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

  if (out_bytes_sent) {
    *out_bytes_sent = bytes_sent;
  }

  LOG(L_INFO, "Sent data on socket %u %d bytes\r\n", socket, bytes_sent);
  return SocketError::OK;
}

static str s_usost = STRDECL("+USOST: ");
SocketError OwlModemSocketRN4::sendTo(uint8_t socket, str remote_ip, uint16_t remote_port, str data) {
  int bytes_sent = 0;
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MAX_SOCKETS);
    return SocketError::BadParameter;
  }
  if (data.len > 512) {
    LOG(L_ERR, "Too much data %d > max 512 bytes\r\n", data.len);
    return SocketError::BadParameter;
  }
  if (!status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return SocketError::Closed;
  }
  if (status[socket].protocol != uso_protocol::UDP) {
    LOG(L_ERR, "Socket %d is not an UDP socket\r\n", socket);
    return SocketError::BadProtocol;
  }
  if (status[socket].is_connected) {
    LOG(L_ERR, "Socket %d is bound to a remote address, use 'send' instead\r\n", socket);
    return SocketError::AlreadyConnected;
  }

  atModem_->commandSprintf("AT+USOST=%u,\"%.*s\",%u,%d,\"", socket, remote_ip.len, remote_ip.s, remote_port, data.len);
  atModem_->commandAppendHex(data);
  atModem_->commandStrcat("\"");
  if (atModem_->doCommandBlocking(10 * 1000, &socket_response) != at_result_code::OK) {
    return SocketError::ModemError;
  }
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
  return SocketError::OK;
}

SocketError OwlModemSocketRN4::getQueuedForReceive(uint8_t socket, int *out_receive) {
  if (out_receive) {
    *out_receive = 0;
  }

  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket);
    return SocketError::BadParameter;
  }

  if (out_receive) {
    *out_receive = status[socket].len_outstanding_receive_data;
  }
  return SocketError::OK;
}

static str s_usord = STRDECL("+USORD: ");

SocketError OwlModemSocketRN4::receive(uint8_t socket, uint16_t len, str_mut *out_data) {
  if (out_data) out_data->len = 0;
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket);
    return SocketError::BadParameter;
  }

  if (len > 512) {
    LOG(L_ERR, "Only supporting up to 512 binary bytes\r\n");
    return SocketError::BadParameter;
  }

  if (!status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return SocketError::Closed;
  }

  if (status[socket].protocol == uso_protocol::UDP && !status[socket].is_connected) {
    LOG(L_ERR, "TCP socket %d is not connected \r\n", socket);
    return SocketError::NotConnected;
  }

  if (len == 0) {
    // 0 means "read all available data"
    len = status[socket].len_outstanding_receive_data;
  }

  if (len == 0) {
    // no data available, exit
    return SocketError::OK;
  }

  atModem_->commandSprintf("AT+USORD=%u,%u", socket, len);
  if (atModem_->doCommandBlocking(1000, &socket_response) != at_result_code::OK) {
    return SocketError::ModemError;
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
        if (sub.len != 0) {
          out_data->len = hex_to_str(out_data->s, len, sub);
          if (out_data->len == 0) {
            LOG(L_ERR, "Bad payload\r\n");
            return SocketError::BadResponse;
          }
        }
        break;
      default:
        break;
    }
  return SocketError::OK;
}

static str s_usorf = STRDECL("+USORF: ");

SocketError OwlModemSocketRN4::receiveFrom(uint8_t socket, uint16_t len, str_mut *out_remote_ip,
                                           uint16_t *out_remote_port, str_mut *out_data) {
  if (out_remote_ip) {
    out_remote_ip->len = 0;
  }

  if (out_remote_port) {
    *out_remote_port = 0;
  }

  if (out_data) {
    out_data->len = 0;
  }

  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket);
    return SocketError::BadParameter;
  }
  if (len > 512) {
    LOG(L_ERR, "Only supporting up to 512 binary bytes.");
    return SocketError::BadParameter;
  }
  if (!status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return SocketError::Closed;
  }
  if (status[socket].protocol != uso_protocol::UDP) {
    LOG(L_ERR, "Socket %d is not an UDP socket\r\n", socket);
    return SocketError::BadProtocol;
  }

  if (len == 0) {
    // 0 means "read all available data"
    len = status[socket].len_outstanding_receive_data;
  }

  if (len == 0) {
    // no data available, exit
    return SocketError::OK;
  }

  atModem_->commandSprintf("AT+USORF=%u,%u", socket, len);
  if (atModem_->doCommandBlocking(1000, &socket_response) != at_result_code::OK) {
    return SocketError::ModemError;
  }

  OwlModemAT::filterResponse(s_usorf, socket_response, &socket_response);
  str token             = {0};
  str sub               = {0};
  uint16_t received_len = 0;
  for (int i = 0; str_tok(socket_response, ",\r\n", &token); i++) {
    switch (i) {
      case 0:
        // socket
        break;
      case 1:
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
        if (sub.len != 0) {
          out_data->len = hex_to_str(out_data->s, len, sub);
          if (out_data->len == 0) {
            LOG(L_ERR, "Bad payload\r\n");
            return SocketError::BadResponse;
          }
        }
        break;
      default:
        break;
    }
  }

  return SocketError::OK;
}

SocketError OwlModemSocketRN4::bindLocalUDP(uint8_t socket, uint16_t local_port) {
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MAX_SOCKETS);
    return SocketError::BadParameter;
  }
  if (!status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return SocketError::Closed;
  }
  if (status[socket].protocol != uso_protocol::UDP) {
    LOG(L_ERR, "Socket %d is not an UDP socket\r\n", socket);
    return SocketError::BadProtocol;
  }
  if (status[socket].is_bound) {
    LOG(L_ERR, "Socket %d is already bound\r\n", socket);
    return SocketError::AlreadyBound;
  }
  atModem_->commandSprintf("AT+USOLI=%u,%u", socket, local_port);
  if (atModem_->doCommandBlocking(1000, nullptr) != at_result_code::OK) {
    return SocketError::ModemError;
  }
  status[socket].is_bound = true;

  return SocketError::OK;
}

SocketError OwlModemSocketRN4::acceptTCP(uint8_t socket, uint16_t local_port,
                                         OwlModem_TCPAcceptHandler_f handler_tcp_accept,
                                         void *handler_tcp_accept_priv) {
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MAX_SOCKETS);
    return SocketError::BadParameter;
  }

  if (handler_tcp_accept == nullptr) {
    LOG(L_ERR, "Accept handler is required\r\n");
    return SocketError::BadParameter;
  }

  if (!status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return SocketError::Closed;
  }
  if (status[socket].protocol != uso_protocol::TCP) {
    LOG(L_ERR, "Socket %d is not a TCP socket\r\n", socket);
    return SocketError::BadProtocol;
  }
  if (status[socket].is_bound) {
    LOG(L_ERR, "Socket %d is already bound\r\n", socket);
    return SocketError::AlreadyBound;
  }
  if (status[socket].is_accepting) {
    LOG(L_ERR, "Socket %d is accepting connections\r\n", socket);
    return SocketError::AlreadyAccepting;
  }
  atModem_->commandSprintf("AT+USOLI=%u,%u", socket, local_port);
  if (atModem_->doCommandBlocking(1000, nullptr) != at_result_code::OK) {
    return SocketError::ModemError;
  }

  status[socket].handler_TCPAccept      = handler_tcp_accept;
  status[socket].handler_TCPAccept_priv = handler_tcp_accept_priv;

  status[socket].is_accepting = true;
  return SocketError::OK;
}

SocketError OwlModemSocketRN4::setSocketClosedHandler(uint8_t socket,
                                                      OwlModem_SocketClosedHandler_f handler_socket_closed,
                                                      void *handler_socket_closed_priv) {
  if (socket >= MAX_SOCKETS) {
    LOG(L_ERR, "Bad socket %d >= %d\r\n", socket, MAX_SOCKETS);
    return SocketError::BadParameter;
  }
  if (!status[socket].is_opened) {
    LOG(L_ERR, "Socket %d is not opened\r\n", socket);
    return SocketError::Closed;
  }

  status[socket].handler_SocketClosed      = handler_socket_closed;
  status[socket].handler_SocketClosed_priv = handler_socket_closed_priv;

  return SocketError::OK;
}
