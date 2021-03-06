#include "OwlModemMQTTBG96.h"
#include <stdio.h>

static char URC_ID[] = "MQTTBG96";
OwlModemMQTTBG96::OwlModemMQTTBG96(OwlModemAT* atModem) : atModem_(atModem) {
  if (atModem_ != nullptr) {
    atModem_->registerUrcHandler(URC_ID, OwlModemMQTTBG96::processURC, this);
  }

  for (int i = 0; i < _num_mqtt_commands; ++i) {
    wait_for_command_[i] = false;
  }
}

bool OwlModemMQTTBG96::waitResultBlocking(mqtt_command command, int32_t timeout) {
  owl_time_t timeout_time = owl_time() + timeout;

  do {
    if (!wait_for_command_[command]) {
      return command_success_[command];
    }

    atModem_->spin();

    if (owl_time() >= timeout_time) {
      return false;
    }

    owl_delay(50);
  } while (1);
}

static str s_qmtopen_urc  = STRDECL("+QMTOPEN");
static str s_qmtclose_urc = STRDECL("+QMTCLOSE");
static str s_qmtconn_urc  = STRDECL("+QMTCONN");
static str s_qmtdisc_urc  = STRDECL("+QMTDISC");
static str s_qmtpub_urc   = STRDECL("+QMTPUB");
static str s_qmtsub_urc   = STRDECL("+QMTSUB");
static str s_qmtuns_urc   = STRDECL("+QMTUNS");
static str s_qmtrecv_urc  = STRDECL("+QMTRECV");

bool OwlModemMQTTBG96::processURC(str urc, str data, void* instance) {
  OwlModemMQTTBG96* inst = reinterpret_cast<OwlModemMQTTBG96*>(instance);

  if (str_equal(urc, s_qmtopen_urc)) {
    inst->processURCQmtopen(data);
    return true;
  } else if (str_equal(urc, s_qmtclose_urc)) {
    inst->processURCQmtclose(data);
    return true;
  } else if (str_equal(urc, s_qmtconn_urc)) {
    inst->processURCQmtconn(data);
    return true;
  } else if (str_equal(urc, s_qmtdisc_urc)) {
    inst->processURCQmtdisc(data);
    return true;
  } else if (str_equal(urc, s_qmtpub_urc)) {
    inst->processURCQmtpub(data);
    return true;
  } else if (str_equal(urc, s_qmtsub_urc)) {
    inst->processURCQmtsub(data);
    return true;
  } else if (str_equal(urc, s_qmtuns_urc)) {
    inst->processURCQmtuns(data);
    return true;
  } else if (str_equal(urc, s_qmtrecv_urc)) {
    inst->processURCQmtrecv(data);
    return true;
  }
  return false;
}

void OwlModemMQTTBG96::processURCQmtopen(str data) {
  str token = {0};

  if (!wait_for_command_[qmtopen]) {
    return;
  }

  wait_for_command_[qmtopen] = false;
  if (!str_tok(data, ",", &token)) {
    command_success_[qmtopen] = false;
    return;
  }

  // ignore tcpconnectID

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtopen] = false;
    return;
  }

  command_success_[qmtopen] = (str_to_long_int(token, 10) == 0);
}

void OwlModemMQTTBG96::processURCQmtclose(str data) {
  str token = {0};

  if (!wait_for_command_[qmtclose]) {
    return;
  }

  wait_for_command_[qmtclose] = false;
  if (!str_tok(data, ",", &token)) {
    command_success_[qmtclose] = false;
    return;
  }

  // ignore tcpconnectID

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtclose] = false;
    return;
  }

  command_success_[qmtclose] = (str_to_long_int(token, 10) == 0);
}

void OwlModemMQTTBG96::processURCQmtconn(str data) {
  str token = {0};

  if (!wait_for_command_[qmtconn]) {
    return;
  }

  wait_for_command_[qmtconn] = false;
  if (!str_tok(data, ",", &token)) {
    command_success_[qmtconn] = false;
    return;
  }

  // ignore tcpconnectID

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtconn] = false;
    return;
  }

  int ack_result = str_to_long_int(token, 10);
  if (ack_result == 2) {  // Failed to send
    command_success_[qmtconn] = false;
    return;
  } else if (ack_result == 1) {         // Retransmission
    wait_for_command_[qmtconn] = true;  // wait for next URC
    return;
  }  // else ack succeeded

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtconn] = false;
    return;
  }
  command_success_[qmtconn] = (str_to_long_int(token, 10) == 0);
}

void OwlModemMQTTBG96::processURCQmtdisc(str data) {
  str token = {0};

  if (!wait_for_command_[qmtdisc]) {
    return;
  }

  wait_for_command_[qmtdisc] = false;
  if (!str_tok(data, ",", &token)) {
    command_success_[qmtdisc] = false;
    return;
  }

  // ignore tcpconnectID

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtdisc] = false;
    return;
  }

  command_success_[qmtdisc] = (str_to_long_int(token, 10) == 0);
}

void OwlModemMQTTBG96::processURCQmtsub(str data) {
  str token = {0};

  if (!wait_for_command_[qmtsub]) {
    return;
  }

  wait_for_command_[qmtsub] = false;
  if (!str_tok(data, ",", &token)) {
    command_success_[qmtsub] = false;
    return;
  }

  // ignore tcpconnectID

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtsub] = false;
    return;
  }

  // ignore msgID

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtsub] = false;
    return;
  }

  int ack_result = str_to_long_int(token, 10);
  if (ack_result == 2) {  // Failed to send
    command_success_[qmtsub] = false;
    return;
  } else if (ack_result == 1) {        // Retransmission
    wait_for_command_[qmtsub] = true;  // wait for next URC
    return;
  } else {
    command_success_[qmtsub] = true;
    return;
  }
}

void OwlModemMQTTBG96::processURCQmtuns(str data) {
  str token = {0};

  if (!wait_for_command_[qmtuns]) {
    return;
  }

  wait_for_command_[qmtuns] = false;
  if (!str_tok(data, ",", &token)) {
    command_success_[qmtuns] = false;
    return;
  }

  // ignore tcpconnectID

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtuns] = false;
    return;
  }

  // ignore msgID

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtuns] = false;
    return;
  }

  int ack_result = str_to_long_int(token, 10);
  if (ack_result == 2) {  // Failed to send
    command_success_[qmtuns] = false;
    return;
  } else if (ack_result == 1) {        // Retransmission
    wait_for_command_[qmtuns] = true;  // wait for next URC
    return;
  } else {
    command_success_[qmtuns] = true;
    return;
  }
}

void OwlModemMQTTBG96::processURCQmtpub(str data) {
  str token = {0};

  if (!wait_for_command_[qmtpub]) {
    return;
  }

  wait_for_command_[qmtpub] = false;
  if (!str_tok(data, ",", &token)) {
    command_success_[qmtpub] = false;
    return;
  }

  // ignore tcpconnectID

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtpub] = false;
    return;
  }

  // ignore msgID

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtpub] = false;
    return;
  }

  int ack_result = str_to_long_int(token, 10);
  if (ack_result == 2) {  // Failed to send
    command_success_[qmtpub] = false;
    return;
  } else if (ack_result == 1) {        // Retransmission
    wait_for_command_[qmtpub] = true;  // wait for next URC
    return;
  } else {
    command_success_[qmtpub] = true;
    return;
  }
}

void OwlModemMQTTBG96::processURCQmtrecv(str data) {
  str token = {0};

  if (message_callback_ == nullptr) {
    return;
  }

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtuns] = false;
    return;
  }

  // ignore tcpconnectID

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtuns] = false;
    return;
  }

  // ignore msgID

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtuns] = false;
    return;
  }

  str topic = token;
  if (topic.len >= 2) {
    // strip quotes
    topic.s++;
    topic.len -= 2;
  }

  if (!str_tok(data, ",", &token)) {
    command_success_[qmtuns] = false;
    return;
  }
  str message = token;
  if (message.len >= 2) {
    // strip quotes
    message.s++;
    message.len -= 2;
  }


  message_callback_(topic, message);
}

bool OwlModemMQTTBG96::openConnection(const char* host_addr, uint16_t port) {
  if (use_tls_) {
    if (atModem_->doCommandBlocking("AT+QMTCFG=\"ssl\",0,1,0", 1 * 1000, nullptr) != at_result_code::OK) {
      return false;
    }
  } else {
    if (atModem_->doCommandBlocking("AT+QMTCFG=\"ssl\",0,0,0", 1 * 1000, nullptr) != at_result_code::OK) {
      return false;
    }
  }

  atModem_->commandSprintf("AT+QMTOPEN=0,\"%s\",%d", host_addr, (int)port);

  wait_for_command_[qmtopen] = true;

  if (atModem_->doCommandBlocking(1 * 1000, nullptr) != at_result_code::OK) {
    wait_for_command_[qmtopen] = false;
    return false;
  }

  if (!waitResultBlocking(qmtopen, 60 * 1000)) {
    return false;
  }

  return true;
}

bool OwlModemMQTTBG96::closeConnection() {
  wait_for_command_[qmtclose] = true;

  if (atModem_->doCommandBlocking("AT+QMTCLOSE=0", 1 * 1000, nullptr) != at_result_code::OK) {
    wait_for_command_[qmtclose] = false;
    return false;
  }

  return waitResultBlocking(qmtclose, 60 * 1000);
}

bool OwlModemMQTTBG96::login(const char* client_id, const char* uname, const char* password) {
  if (uname == nullptr || password == nullptr) {
    atModem_->commandSprintf("AT+QMTCONN=0,\"%s\"", client_id);
  } else {
    atModem_->commandSprintf("AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"", client_id, uname, password);
  }

  wait_for_command_[qmtconn] = true;

  if (atModem_->doCommandBlocking(1 * 1000, nullptr) != at_result_code::OK) {
    wait_for_command_[qmtconn] = false;
    return false;
  }

  return waitResultBlocking(qmtconn, 60 * 1000);
}

bool OwlModemMQTTBG96::logout() {
  wait_for_command_[qmtdisc] = true;

  if (atModem_->doCommandBlocking("AT+QMTDISC=0", 1 * 1000, nullptr) != at_result_code::OK) {
    wait_for_command_[qmtdisc] = false;
    return false;
  }

  return waitResultBlocking(qmtdisc, 60 * 1000);
}

bool OwlModemMQTTBG96::publish(const char* topic, str data, bool retain, qos_t qos, uint16_t msg_id) {
  if (qos == qos_t::atMostOnce) {
    msg_id = 0;
  }

  atModem_->commandSprintf("AT+QMTPUB=0,%d,%d,%d,\"%s\"", (int)msg_id, qos, retain, topic);

  wait_for_command_[qmtpub] = true;

  if (atModem_->doCommandBlocking(1 * 1000, nullptr, data, 0x1A) != at_result_code::OK) {
    wait_for_command_[qmtpub] = false;
    return false;
  }

  return waitResultBlocking(qmtpub, 60 * 1000);
}

bool OwlModemMQTTBG96::subscribe(const char* topic_filter, uint16_t msg_id, qos_t qos) {
  atModem_->commandSprintf("AT+QMTSUB=0,%d,\"%s\",%d", (int)msg_id, topic_filter, qos);

  wait_for_command_[qmtsub] = true;

  if (atModem_->doCommandBlocking(1 * 1000, nullptr) != at_result_code::OK) {
    wait_for_command_[qmtsub] = false;
    return false;
  }

  return waitResultBlocking(qmtsub, 60 * 1000);
}
