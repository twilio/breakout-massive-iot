#include "RN4PahoIPStack.h"

#define RN4_PAHO_LOCAL_PORT 8883

bool RN4PahoIPStack::connect(const char* hostname, int port, bool use_tls, int tls_id) {
  if (!modem_->open(AT_USO_Protocol__TCP, RN4_PAHO_LOCAL_PORT, &socket_id_)) {
    return false;
  }

  if (use_tls) {
    if (!modem_->enableTLS(socket_id_, tls_id)) {
      modem_->close(socket_id_);
      return false;
    }
  } else {
    if (!modem_->disableTLS(socket_id_)) {
      modem_->close(socket_id_);
      return false;
    }
  }

  str hostname_str = STRDECL(hostname);
  if (!modem_->connect(socket_id_, hostname_str, port, socketCloseHandler, this)) {
    modem_->close(socket_id_);
    return false;
  }

  open_ = true;

  return true;
}

void RN4PahoIPStack::socketCloseHandler(uint8_t socket, void* priv) {
  RN4PahoIPStack* instance = (RN4PahoIPStack*)priv;
  instance->open_          = false;
}

int RN4PahoIPStack::read(unsigned char* buffer, int len, int timeout_ms) {
  // TODO: support timeout
  if (!open_) {
    return -1;
  }

  str out_data;
  out_data.s   = (char*)buffer;
  out_data.len = 0;
  if (!modem_->receiveTCP(socket_id_, len, &out_data, len)) {
    return -1;
  }

  return out_data.len;
}

int RN4PahoIPStack::write(unsigned char* buffer, int len, int timeout_ms) {
  str data;
  data.s   = (char*)buffer;
  data.len = len;
  int bytes_sent;
  if (!open_) {
    return -1;
  }

  if (!modem_->sendTCP(socket_id_, data, &bytes_sent)) {
    return -1;
  }

  return bytes_sent;
}
