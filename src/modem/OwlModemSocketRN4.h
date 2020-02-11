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



/* These are limitations of the ublox SARA-R4/N4 modems */
#define MODEM_UDP_BUFFER_SIZE 512
#define MODEM_MAX_SOCKETS 7

/**
 * Handler for UDP data
 * @param socket - socket the data was received on
 * @param remote_ip - where the data came from - in case the socket was connected, this might be missing
 * @param remote_port - the remote port where the data came from
 * @param data - the binary data
 * @param priv - private data
 */
typedef void (*OwlModem_UDPDataHandler_f)(uint8_t socket, str remote_ip, uint16_t remote_port, str data, void* priv);

/**
 * Handler for TCP data
 * @param socket - the socket the data was received on
 * @param data - the binary data
 * @param priv - private data
 */
typedef void (*OwlModem_TCPDataHandler_f)(uint8_t socket, str data, void* priv);

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
  uint8_t is_opened    = 0;
  uint8_t is_connected = 0;

  int len_outstanding_receive_data     = 0;
  int len_outstanding_receivefrom_data = 0;

  uso_protocol protocol = uso_protocol::none;

  // TODO: consider using just one piece of private data for all handlers
  OwlModem_UDPDataHandler_f handler_UDPData           = nullptr;
  void* handler_UDPData_priv                          = nullptr;
  OwlModem_TCPDataHandler_f handler_TCPData           = nullptr;
  void* handler_TCPData_priv                          = nullptr;
  OwlModem_TCPAcceptHandler_f handler_TCPAccept       = nullptr;
  void* handler_TCPAccept_priv                        = nullptr;
  OwlModem_SocketClosedHandler_f handler_SocketClosed = nullptr;
  void* handler_SocketClosed_priv                     = nullptr;

  void setOpened(uso_protocol protocol);
  void setClosed();
};



/**
 * Twilio wrapper for the AT serial interface to a modem - Methods for TCP/UDP communication over sockets
 */
class OwlModemSocketRN4 {
 public:
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
   * @param local_port - local port to bind to, 0 if you don't care what it should be
   * @param out_socket - socket id
   * @return 1 on success, 0 on failure
   */
  int open(uso_protocol protocol, uint16_t local_port, uint8_t* out_socket);

  /**
   * Close socket
   * @param port - port to bind to locally
   * @param out_socket - socket id
   * @return 1 on success, 0 on failure
   */
  int close(uint8_t socket);

  /**
   * Enable TLS encryption on socket.
   * @param socket - socket id. Should be open before calling this function.
   * @param tls_id - TLS profile ID. Should be created with OwlModemSSLRN4::createTLSProfile
   * @return 1 on success, 0 on failure
   */
  int enableTLS(uint8_t socket, int tls_id);

  /**
   * Disable TLS encryption on socket.
   * @param socket - socket id. Should be open before calling this function.
   * @return 1 on success, 0 on failure
   */
  int disableTLS(uint8_t socket);

  /**
   * Retrieve code of last socket error
   * @param socket - socket id
   * @param out_error - output error
   * @return 1 on success, 0 on failure
   */
  int getError(uso_error* out_error);

  /**
   * Connect a socket to a remote IP and port. Use this for both UDP and TCP. For UDP, this allows the use of
   * send() without specifying every time the remote IP & port, as sendToUDP() would require.
   * @param socket - socket id
   * @param remote_ip - remote IP
   * @param remote_port - remote port
   * @param cb - callback for closed connection
   * @param cb_priv - private data for the callback
   * @return 1 on success, 0 on failure
   */
  int connect(uint8_t socket, str remote_ip, uint16_t remote_port, OwlModem_SocketClosedHandler_f cb,
              void* cb_priv = nullptr);

  /**
   * Send data over UDP
   * @param socket
   * @param data - max 512 bytes in binary format
   * @param out_bytes_sent - output number of bytes actually sent
   * @return 1 on success, 0 on failure or not all bytes sent
   */
  int sendUDP(uint8_t socket, str data, int* out_bytes_sent);

  /**
   * Send data over TCP
   * @param socket
   * @param data - max 512 bytes in binary format
   * @param out_bytes_sent - output number of bytes actually sent
   * @return 1 on success or partial success (not all bytes written), 0 on failure
   */
  int sendTCP(uint8_t socket, str data, int* out_bytes_sent);

  /**
   * Send data over UDP to remote_ip:remote_port
   * @param socket - socket id, obtained from openUDPSocket()
   * @param remote_ip - destination IP
   * @param remote_port - destination port
   * @param data - max 512 bytes in binary format
   * @return 1 on success, 0 on failure
   */
  int sendToUDP(uint8_t socket, str remote_ip, uint16_t remote_port, str data);

  /**
   * Retrieve lengths of currently queued data for receive. Use this function as an alternative to calling
   * handleWaitingData(). If data is available, you can retrieve it with receiveTCP(), receiveUDP(), respectively
   * receiveFromUDP().
   * Note: if multiple UDP packets are queued, the value returned is that of the packet at the top of the queue. Hence
   * if you want all packets, call iteratively and drain the queue using the receive functions.
   * @param socket - socket to check on
   * @param out_receive_tcp - output number of bytes queued for receive if this is a TCP socket
   * @param out_receive_udp - output number of bytes queued for receive if this is a UDP socket and it was connected
   * @param out_receivefrom_udp - output number of bytes queued for receive if this is a UDP socket and it wasn't
   * connected to a particular remote IP:port.
   * @return 1 on success, 0 on failure
   */
  int getQueuedForReceive(uint8_t socket, int* out_receive_tcp, int* out_receive_udp, int* out_receivefrom_udp);

  /**
   * Do Receive on UDP when the socket was connected to a remote IP:port - typically called on +UUSORD events.
   * @param socket - socket id to read from
   * @param len - how much data to read
   * @param out_data - output buffer for the data - binary data here
   * @param max_data_len - maximum bytes to write in out_data
   * @return 1 on success, 0 on failure
   * @return
   */
  int receiveUDP(uint8_t socket, uint16_t len, str_mut* out_data, int max_data_len);

  /**
   * Do Receive on TCP after the socket was connected to a remote IP:port - typically called on +UUSORD events.
   * @param socket - socket id to read from
   * @param len - how much data to read
   * @param out_data - output buffer for the data - binary data here
   * @param max_data_len - maximum bytes to write in out_data
   * @return 1 on success, 0 on failure
   * @return
   */
  int receiveTCP(uint8_t socket, uint16_t len, str_mut* out_data, int max_data_len);

  /**
   * Do ReceiveFrom on UDP - typically called on +UUSORF events.
   * @param socket - socket id to read from
   * @param len - how much data to read
   * @param out_remote_ip - output remote IP - buffer must be at least 40 bytes long
   * @param out_remote_port - output remote port
   * @param out_data - output buffer for the data - binary data here
   * @param max_data_len - maximum bytes to write in out_data
   * @return 1 on success, 0 on failure
   */
  int receiveFromUDP(uint8_t socket, uint16_t len, str_mut* out_remote_ip, uint16_t* out_remote_port, str_mut* out_data,
                     int max_data_len);

  /**
   * Start listening on a port for incoming UDP data. Call handler function when data is available.
   * @param socket - socket id to listen on
   * @param local_port - local port to listen on
   * @param handler - (re-)set the handler for data to this function
   * @param handler_priv - private data for the callback
   * @return 1 on success, 0 on failure
   */
  int listenUDP(uint8_t socket, uint16_t local_port, OwlModem_UDPDataHandler_f handler, void* handler_priv = nullptr);

  /**
   * Register handler for incoming TCP data
   * @param socket - socket id to listen on
   * @param handler - (re-)set the handler for data to this function
   * @param handler_priv - private data for the callback
   * @return 1 on success, 0 on failure
   */
  int listenTCP(uint8_t socket, OwlModem_TCPDataHandler_f handler_tcp_data, void* handler_priv = nullptr);

  /**
   * Start listening for new TCP connection on a socket. Call handler function when new connection is accepted.
   * @param socket - socket id to listen on
   * @param local_port - local port to listen on
   * @param handler_tcp_accept - callback for accepted connection
   * @param handler_socket_closed - callback for closed socket
   * @param handler_tcp_data - callback for incoming UDP data
   * @param handler_tcp_accept_priv - private data for accepted connection callback
   * @param handler_socket_closed_priv - private data for closed socket callback
   * @param handler_tcp_data_priv - private data for the callback

   * @return 1 on success, 0 on failure
   */
  int acceptTCP(uint8_t socket, uint16_t local_port, OwlModem_TCPAcceptHandler_f handler_tcp_accept,
                OwlModem_SocketClosedHandler_f handler_socket_closed, OwlModem_TCPDataHandler_f handler_tcp_data,
                void* handler_tcp_accept_priv = nullptr, void* handler_socket_closed_priv = nullptr,
                void* handler_tcp_data_pirv = nullptr);

  /**
   * Open UDP socket and set the listen callback in one operation.
   * @param local_port - local port to listen on
   * @param out_socket - output socket id
   * @param handler_data - callback for incoming UDP data
   * @param handler_data_priv - private data for the callback
   * @return 1 on success, 0 on failure
   */
  int openListenUDP(uint16_t local_port, uint8_t* out_socket, OwlModem_UDPDataHandler_f handler_data,
                    void* hanlder_data_priv = nullptr);

  /**
   * Open UDP socket, connect to the remote IP:port and set the listen callback in one operation.
   * @param remote_ip - remote IP
   * @param remote_port - remote port
   * @param out_socket - output socket id
   * @param handler_data - callback for incoming UDP data
   * @param handler_data_priv - private data for the callback
   * @return 1 on success, 0 on failure
   */
  int openConnectUDP(str remote_ip, uint16_t remote_port, uint8_t* out_socket, OwlModem_UDPDataHandler_f handler_data,
                     void* hanlder_data_priv = nullptr);

  /**
   * Open UDP socket, connect to the remote IP:port and set the listen callback in one operation.
   * @param local_port - local port to listen on
   * @param remote_ip - remote IP
   * @param remote_port - remote port
   * @param out_socket - output socket id
   * @param handler_data - callback for incoming UDP data
   * @param handler_data_priv - private data for the callback
   * @return 1 on success, 0 on failure
   */
  int openListenConnectUDP(uint16_t local_port, str remote_ip, uint16_t remote_port, uint8_t* out_socket,
                           OwlModem_UDPDataHandler_f handler_data, void* handler_data_priv = nullptr);

  /**
   * TCP client - Open TCP socket, connect to the remote IP:port and set the listen callback in one operation.
   * @param local_port - local port to listen on
   * @param remote_ip - remote IP
   * @param remote_port - remote port
   * @param out_socket - output socket id
   * @param handler_close - callback for closed socket
   * @param handler_data - callback for incoming TCP data
   * @param handler_close_priv - private data for closed socket callback
   * @param handler_data_priv - private data for data callback
   * @return 1 on success, 0 on failure
   */
  int openListenConnectTCP(uint16_t local_port, str remote_ip, uint16_t remote_port, uint8_t* out_socket,
                           OwlModem_SocketClosedHandler_f handler_close, OwlModem_TCPDataHandler_f handler_data,
                           void* handler_close_priv = nullptr, void* handler_data_priv = nullptr);

  /**
   * TCP server - Open TCP socket, set into accept mode (listening for incoming connection) and set handler for data
   * and close to be carried-over to the accepted connections.
   * @param local_port - local port to listen on
   * @param out_socket - output socket id
   * @param handler_accept - callback for accepted connection
   * @param handler_close - callback for closed socket
   * @param handler_data - callback for incoming UDP data
   * @param handler_accept_priv - private data for accepted connection callback
   * @param handler_close_priv - private data for closed socket callback
   * @param handler_data_priv - private data for the callback
   * @return 1 on success, 0 on failure
   */
  int openAcceptTCP(uint16_t local_port, uint8_t* out_socket, OwlModem_TCPAcceptHandler_f handler_accept,
                    OwlModem_SocketClosedHandler_f handler_close, OwlModem_TCPDataHandler_f handler_data,
                    void* handler_accept_priv = nullptr, void* handler_close_priv = nullptr,
                    void* handler_data_priv = nullptr);



 private:
  OwlModemAT* atModem_ = 0;


  OwlModemSocketRN4Status status[MODEM_MAX_SOCKETS];

  str socket_response = {.s = nullptr, .len = 0};


  /** UDP buffer, to be used internally when receiving data */
  char udp_buffer[MODEM_UDP_BUFFER_SIZE];
  str_mut udp_data = {.s = udp_buffer, .len = 0};

  int send(uint8_t socket, str data);
  int receive(uint8_t socket, uint16_t len, str_mut* out_data, int max_data_len);

  bool processURCConnected(str urc, str data);
  bool processURCClosed(str urc, str data);
  bool processURCTCPAccept(str urc, str data);
  bool processURCReceive(str urc, str data);
  bool processURCReceiveFrom(str urc, str data);
};

#endif
