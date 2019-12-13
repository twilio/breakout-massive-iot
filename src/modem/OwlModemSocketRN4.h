/*
 * OwlModemSocketRN4.h
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
 * \file OwlModemSocketRN4.h - API for TCP/UDP communication over sockets
 */

#ifndef __OWL_MODEM_SOCKET_H__
#define __OWL_MODEM_SOCKET_H__

#include "enums.h"
#include "OwlModemAT.h"



/**
 * Handler for TCP new connection accept
 * @param new_socket - the new socket created after accepting it on the listening_socket
 * @param remote_ip - the remote IP address from where this connection was done
 * @param remote_port - the remote port from where this connection was done
 * @param listening_socket - the listening socket where this new connection was accepted
 * @param local_ip - the local IP where this connection was accepted
 * @param local_port - the local port of this connection
 * @param priv - private data
 */
typedef void (*OwlModem_TCPAcceptHandler_f)(uint8_t new_socket, str remote_ip, uint16_t remote_port,
                                            uint8_t listening_socket, str local_ip, uint16_t local_port, void* priv);

/**
 * Handler for TCP socket closed event
 * @param socket - the socket which was closed
 * @param priv - private data
 */
typedef void (*OwlModem_SocketClosedHandler_f)(uint8_t socket, void* priv);

class OwlModemSocketRN4Status {
 public:
  bool is_opened    = 0;
  bool is_bound     = 0;
  bool is_connected = 0;
  bool is_accepting = 0;

  int len_outstanding_receive_data = 0;

  uso_protocol protocol = uso_protocol::none;

  // TODO: consider using just one piece of private data for all handlers
  OwlModem_TCPAcceptHandler_f handler_TCPAccept       = nullptr;
  void* handler_TCPAccept_priv                        = nullptr;
  OwlModem_SocketClosedHandler_f handler_SocketClosed = nullptr;
  void* handler_SocketClosed_priv                     = nullptr;

  void setOpened(uso_protocol protocol);
  void setClosed();
};

enum class SocketError {
  OK = 0,
  Closed,
  NotConnected,
  AlreadyConnected,
  AlreadyBound,
  AlreadyAccepting,
  ModemError,
  BadParameter,
  BadProtocol,
  BadResponse,
};


/**
 * Twilio wrapper for the AT serial interface to a modem - Methods for TCP/UDP communication over sockets
 */
class OwlModemSocketRN4 {
 public:
  /* This is a limitation of the ublox SARA-R4/N4 modems */
  constexpr static int MAX_SOCKETS = 7;

  OwlModemSocketRN4(OwlModemAT* atModem);

  /**
   * Handler for Unsolicited Response Codes from the modem - called from OwlModem on timer, when URC is received
   * @param urc - event id
   * @param data - data of the event
   * @param instance - pointer to OwlModemSocketRN4 instance
   * @return if the line was handled
   */
  static bool processURC(str urc, str data, void* instance);

  /**
   * Handler for incoming data - triggers receive and handler calling for UDP/TCP queued packets.
   * Call this function from the main loop, every once in a while, to trigger receive of data and calling of
   * incoming data handlers.
   */
  void handleWaitingData();


  /**
   * Open socket
   * @param protocol - 6 for TCP, 17 for UDP
   * @param local_port - local port to bind to, 0 if you don't want to bind yet. NB! If you plan to call `listen` on
   * this port you should leave it unbound
   * @param out_socket - socket id
   * @return success status
   */
  bool open(uso_protocol protocol, uint16_t local_port, uint8_t* out_socket);

  /**
   * Close socket
   * @param port - port to bind to locally
   * @param out_socket - socket id
   * @return operation result
   */
  SocketError close(uint8_t socket);

  /**
   * Enable TLS encryption on socket.
   * @param socket - socket id. Should be open before calling this function.
   * @param tls_id - TLS profile ID. Should be created with OwlModemSSLRN4::createTLSProfile
   * @return 1 on success, 0 on failure
   */
  SocketError enableTLS(uint8_t socket, int tls_id);

  /**
   * Disable TLS encryption on socket.
   * @param socket - socket id. Should be open before calling this function.
   * @return 1 on success, 0 on failure
   */
  SocketError disableTLS(uint8_t socket);

  /**
   * Retrieve code of last socket error
   * @param socket - socket id
   * @param out_error - output error
   * @return success status getting the error itself
   */
  SocketError getError(uso_error* out_error);

  /**
   * Connect a socket to a remote IP and port. Use this for both UDP and TCP. For UDP, this allows the use of
   * send() without specifying every time the remote IP & port, as sendToUDP() would require.
   * @param socket - socket id
   * @param remote_ip - remote IP
   * @param remote_port - remote port
   * @return 1 on success, 0 on failure
   */
  SocketError connect(uint8_t socket, str remote_ip, uint16_t remote_port);

  /**
   * Send data over a connected socket
   * @param socket
   * @param data - max 512 bytes in binary format
   * @param out_bytes_sent - output number of bytes actually sent
   * @return 1 on success, 0 on failure or not all bytes sent
   */
  SocketError send(uint8_t socket, str data, int* out_bytes_sent);

  /**
   * Send data over UDP to remote_ip:remote_port. Socket should not be connected, if it is use `send`
   * @param socket - socket id, obtained from openUDPSocket()
   * @param remote_ip - destination IP
   * @param remote_port - destination port
   * @param data - max 512 bytes in binary format
   * @return 1 on success, 0 on failure
   */
  SocketError sendTo(uint8_t socket, str remote_ip, uint16_t remote_port, str data);

  /**
   * Retrieve lengths of currently queued data for receive. Use this function as an alternative to calling
   * handleWaitingData(). If data is available, you can retrieve it with receiveTCP(), receiveUDP(), respectively
   * receiveFromUDP().
   * Note: if multiple UDP packets are queued, the value returned is that of the packet at the top of the queue. Hence
   * if you want all packets, call iteratively and drain the queue using the receive functions.
   * @param socket - socket to check on
   * @param out_receive_tcp - output number of bytes queued for receive
   * @return 1 on success, 0 on failure
   */
  SocketError getQueuedForReceive(uint8_t socket, int* out_receive);

  /**
   * Receive from a socket. Doesn't return peer host and port.
   * @param socket - socket id to read from
   * @param len - how much data to read, 0 to read all available data.
   * @param out_data - output buffer for the data - binary data here
   * @return success status
   * @return
   */
  SocketError receive(uint8_t socket, uint16_t len, str_mut* out_data);

  /**
   * Call recvfrom on a UDP socket. Returns peer host and port
   * @param socket - socket id to read from
   * @param len - how much data to read, 0 to read all available data.
   * @param out_remote_ip - output remote IP - buffer must be at least 40 bytes long
   * @param out_remote_port - output remote port
   * @param out_data - output buffer for the data - binary data here
   * @param max_data_len - maximum bytes to write in out_data
   * @return 1 on success, 0 on failure
   */
  SocketError receiveFrom(uint8_t socket, uint16_t len, str_mut* out_remote_ip, uint16_t* out_remote_port,
                          str_mut* out_data);

  /**
   * Start listening on a port for incoming UDP data. Call handler function when data is available.
   * @param socket - socket id to listen on
   * @param local_port - local port to listen on
   * @return 1 on success, 0 on failure
   */
  SocketError bindLocalUDP(uint8_t socket, uint16_t local_port);

  /**
   * Start listening for new TCP connection on a socket. Call handler function when new connection is accepted.
   * @param socket - socket id to listen on
   * @param local_port - local port to listen on
   * @param handler_tcp_accept - callback for accepted connection
   * @param handler_tcp_accept_priv - private data for accepted connection callback

   * @return 1 on success, 0 on failure
   */
  SocketError acceptTCP(uint8_t socket, uint16_t local_port, OwlModem_TCPAcceptHandler_f handler_tcp_accept,
                        void* handler_tcp_accept_priv = nullptr);

  /**
    * Set handler for "socket closed" event. The modem doesn't always send it immediately, so don't rely on it much.
    * @param socket - socket id
    * @param handler_socket_closed - callback for closed socket
    * @param handler_socket_closed_priv - private data for the callback

    * @return 1 on success, 0 on failure
    */
  SocketError setSocketClosedHandler(uint8_t socket, OwlModem_SocketClosedHandler_f handler_socket_closed,
                                     void* handler_socket_closed_priv = nullptr);

 private:
  OwlModemAT* atModem_ = 0;


  OwlModemSocketRN4Status status[MAX_SOCKETS];

  str socket_response = {.s = nullptr, .len = 0};

  bool processURCClosed(str urc, str data);
  bool processURCTCPAccept(str urc, str data);
  bool processURCReceive(str urc, str data);
  bool processURCReceiveFrom(str urc, str data);
};

#endif
