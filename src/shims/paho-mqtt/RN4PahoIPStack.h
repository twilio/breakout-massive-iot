#ifndef __RN4_PAHO_IP_STACK_H__
#define __RN4_PAHO_IP_STACK_H__

#include "../../modem/OwlModemSocketRN4.h"

class RN4PahoIPStack {
 public:
  RN4PahoIPStack(OwlModemSocketRN4* modem) : modem_{modem} {
  }

  bool connect(const char* hostname, int port, bool use_tls = false, int tls_id = 0);
  int disconnect();

  int read(unsigned char* buffer, int len, int timeout_ms);
  int write(unsigned char* buffer, int len, int timeoout_ms);

  bool connected() {
    return open_;
  }

  static void socketCloseHandler(uint8_t socket, void* priv);

 private:
  OwlModemSocketRN4* modem_;
  uint8_t socket_id_;
  bool open_{false};
};

#endif  // __RN4_PAHO_IP_STACK_H__
