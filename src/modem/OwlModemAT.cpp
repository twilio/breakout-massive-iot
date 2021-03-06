#include "OwlModemAT.h"
#include <stdarg.h>
#include <stdio.h>

#define AT_DATA_SEND_INTERVAL 100
#define AT_DATA_CHUNK_SIZE 100
#define AT_DO_COMMAND_SPIN_INTERVAL 200

#ifdef BUILD_FOR_TEST
void spinProcessLineTestpoint(str line);
#endif

void OwlModemAT::spinProcessLine() {
#ifdef BUILD_FOR_TEST
  spinProcessLineTestpoint(line_buffer_);
#endif

  switch (state_) {
    case modem_state_t::idle:
    case modem_state_t::send_data:
    case modem_state_t::response_ready:
      if (!processURC()) {  // feed it to URC parser, if no success process as a special prefix
        processPrefix();
      }
      break;

    case modem_state_t::wait_result:
    case modem_state_t::wait_prompt:
      if (processURC()) {
        break;
      }
      at_result_code code = tryParseCode();

      if (code == at_result_code::unknown) {
        processPrefix();
        appendLineToResponse();
        break;
      }

      if (state_ == modem_state_t::wait_prompt && code == at_result_code::CONNECT) {
        processInputPrompt();
      } else {
        last_response_code_ = code;

        if (response_handler_ != nullptr &&
            response_handler_(last_response_code_, response_buffer_, response_handler_param_)) {
          state_ = modem_state_t::idle;
        } else {
          state_ = modem_state_t::response_ready;
        }
      }
      break;
  }
}

void OwlModemAT::spin() {
  spinProcessInput();
  spinProcessTime();
}

void OwlModemAT::spinProcessTime() {
  switch (state_) {
    case modem_state_t::idle:
    case modem_state_t::response_ready:
      return;

    case modem_state_t::wait_result:
    case modem_state_t::wait_prompt:
      if (command_timeout_ == 0) {
        return;
      }

      if (owl_time() > command_started_ + command_timeout_) {
        if (response_handler_ != nullptr &&
            response_handler_(at_result_code::timeout, response_buffer_, response_handler_param_)) {
          state_ = modem_state_t::idle;
        } else {
          last_response_code_ = at_result_code::timeout;
          state_              = modem_state_t::response_ready;
        }
      }
      return;

    case modem_state_t::send_data:
      if (send_data_ts_ == 0 || owl_time() > send_data_ts_ + AT_DATA_SEND_INTERVAL) {
        unsigned int to_send = (command_data_.len > AT_DATA_CHUNK_SIZE) ? AT_DATA_CHUNK_SIZE : command_data_.len;
        str chunk            = {.s = command_data_.s, .len = to_send};
        sendData(chunk);
        send_data_ts_ = owl_time();
        command_data_.len -= to_send;
        command_data_.s += to_send;

        if (command_data_.len == 0) {
          if (command_data_term_ != 0xFFFF) {
            char term_byte = (char)(command_data_term_ & 0xFF);
            str term       = {.s = &term_byte, .len = 1};
            sendData(term);
          }
          command_started_     = owl_time();
          response_buffer_.len = 0;
          state_               = modem_state_t::wait_result;
        }
      }
      return;

    default:
      return;
  }
}

void OwlModemAT::spinProcessInput() {
  // "tokenizer" looks for strings of type "\r\n.*\r\n"
  //   then gives the string too the main string processor spinProcessLine
  int available = serial_->available();

  if (available == 0) {
    return;
  }

  input_buffer_.len = serial_->read(reinterpret_cast<uint8_t *>(input_buffer_.s),
                                    (available > AT_INPUT_BUFFER_SIZE) ? AT_INPUT_BUFFER_SIZE : available);
  LOG(L_DBG, "Input from the modem\r\n");
  LOGSTR(L_DBG, input_buffer_);

  str input_buffer_slice = input_buffer_;
  do {
    // Special case: current command is expecting input prompt, and we get '>' symbol in the
    //   beginning of the line. In this case no line end delimiter ("\r\n") is expected
    if (line_buffer_.len == 0 && state_ == modem_state_t::wait_prompt && input_buffer_slice.s[0] == '>') {
      processInputPrompt();
      input_buffer_slice.s++;
      input_buffer_slice.len--;
      continue;
    }

    // General case: normal input string delimited by "\r\n" from both ends

    const char *lf_pos = (char *)memchr(input_buffer_slice.s, '\n', input_buffer_slice.len);

    int chunk_len;

    if (lf_pos == nullptr) {
      if (input_buffer_slice.s[input_buffer_slice.len - 1] == '\r') {
        // Annoying corner case: '\r' has arrived while '\n' is to be read next. '\r' doesn't belong to the string
        --input_buffer_slice.len;
      }
      chunk_len = input_buffer_slice.len;
    } else {
      // try to be liberal and allow both "\r\n" and plain "\n" line endings
      if ((lf_pos != input_buffer_slice.s) && (*(lf_pos - 1) == '\r')) {
        chunk_len = (int)(lf_pos - 1 - input_buffer_slice.s);
      } else {
        chunk_len = (int)(lf_pos - input_buffer_slice.s);
      }
    }

    if (line_buffer_.len + chunk_len > AT_LINE_BUFFER_SIZE) {
      LOG(L_ERR, "AT input string is too long, truncating\r\n");
      chunk_len = AT_LINE_BUFFER_SIZE - line_buffer_.len;
    }

    if (chunk_len != 0) {
      memcpy(line_buffer_.s + line_buffer_.len, input_buffer_slice.s, chunk_len);
      line_buffer_.len += chunk_len;
      input_buffer_slice.s += chunk_len;
      input_buffer_slice.len -= chunk_len;
    }

    if (lf_pos != nullptr) {
      // end of line found, process the line
      if (line_buffer_.len == 0) {
        // empty line means we're run into "\r\n\r\n" sequence. Probably
        //   means that we treated end marker as begin marker, so run again
        //   using "end" as "begin"
        // still need to eat delimiters
        while (input_buffer_slice.len != 0) {
          if (input_buffer_slice.s[0] == '\r' || input_buffer_slice.s[0] == '\n') {
            input_buffer_slice.s++;
            input_buffer_slice.len--;
          } else {
            break;
          }
        }
      } else {
        // normal line, process
        spinProcessLine();
        line_buffer_.len = 0;
        input_buffer_slice.s++;  // skip line delimiter
        input_buffer_slice.len--;
      }
    }
  } while (input_buffer_slice.len != 0);
}

bool OwlModemAT::processURC() {
  if (line_buffer_.len < 1 || line_buffer_.s[0] != '+') {
    return false;
  }

  const char *colon_pos = (char *)memchr(line_buffer_.s, ':', line_buffer_.len);
  if (colon_pos == nullptr) {
    return false;
  }

  unsigned int urc_len = (int)(colon_pos - line_buffer_.s);

  // Response format: "+<COMMAND>: <data>" (space after colon is required)
  if (urc_len > line_buffer_.len - 2 || line_buffer_.s[urc_len + 1] != ' ') {
    return false;
  }

  str urc  = {.s = line_buffer_.s, .len = urc_len};
  str data = {.s = line_buffer_.s + urc_len + 2, .len = line_buffer_.len - urc_len - 2};

  LOG(L_DBG, "URC [%.*s] Data [%.*s]\r\n", urc.len, urc.s, data.len, data.s);

  /* ordered based on expected incoming count of events */
  for (int i = 0; i < num_urc_handlers_; i++) {
    if (urc_handlers_[i](urc, data, urc_handler_params_[i])) {
      return true;
    }
  }

  return false;
}

void OwlModemAT::processPrefix() {
  if (prefix_handler_ == nullptr) {
    return;
  }

  for (int i = 0; i < num_special_prefixes_; ++i) {
    if (str_equal_prefix(line_buffer_, special_prefixes_[i])) {
      prefix_handler_(line_buffer_, prefix_handler_param_);
      return;
    }
  }
}

void OwlModemAT::appendLineToResponse() {
  int to_append;

  if (line_buffer_.len + response_buffer_.len < AT_RESPONSE_BUFFER_SIZE) {
    to_append = line_buffer_.len;
  } else {
    LOG(L_ERR, "Line doesn't fit into response buffer, truncating");
    to_append = AT_RESPONSE_BUFFER_SIZE - response_buffer_.len;
  }

  if (to_append != 0) {
    memcpy(response_buffer_.s + response_buffer_.len, line_buffer_.s, to_append);
    response_buffer_.len += to_append;

    if (response_buffer_.len < AT_RESPONSE_BUFFER_SIZE) {
      response_buffer_.s[response_buffer_.len++] = '\n';
    }
  }
}

at_result_code OwlModemAT::tryParseCode() {
  using at_code_entry = struct {
    str value;
    at_result_code code;
  };

  static at_code_entry at_result_codes[] = {
      {.value = {.s = "OK", .len = 2}, .code = at_result_code::OK},
      {.value = {.s = "CONNECT", .len = 7}, .code = at_result_code::CONNECT},
      {.value = {.s = "RING", .len = 4}, .code = at_result_code::RING},
      {.value = {.s = "NO CARRIER", .len = 10}, .code = at_result_code::NO_CARRIER},
      {.value = {.s = "ERROR", .len = 5}, .code = at_result_code::ERROR},
      {.value = {.s = "NO DIALTONE", .len = 11}, .code = at_result_code::NO_DIALTONE},
      {.value = {.s = "BUSY", .len = 4}, .code = at_result_code::BUSY},
      {.value = {.s = "NO ANSWER", .len = 9}, .code = at_result_code::NO_ANSWER}};

  for (unsigned int i = 0; i < sizeof(at_result_codes) / sizeof(at_code_entry); ++i) {
    if (str_equal(line_buffer_, at_result_codes[i].value)) {
      return at_result_codes[i].code;
    }
  }

  // CONNECT code also comes in a vendor-customized flavour with a postfix
  if (str_equal_prefix_char(line_buffer_, "CONNECT ")) {
    return at_result_code::CONNECT;
  }

  if (str_equal_prefix_char(line_buffer_, "+CME ERROR")) {
    return at_result_code::cme_error;
  }

  return at_result_code::unknown;
}

void OwlModemAT::processInputPrompt() {
  state_               = modem_state_t::send_data;
  send_data_ts_        = 0;
  response_buffer_.len = 0;
  last_response_code_  = at_result_code::unknown;
}

bool OwlModemAT::startATCommand(owl_time_t timeout_ms, str data, uint16_t data_term) {
  if (serial_ == nullptr) {
    LOG(L_ERR, "startATCommand [%.*s] failed: serial device unavailable\r\n", command_buffer_.len, command_buffer_.s);
    return false;
  }

  if (state_ != modem_state_t::idle) {
    LOG(L_ERR, "startATCommand [%.*s] failed: new commands can only be sent in the idle state\r\n", command_buffer_.len,
        command_buffer_.s);
    return false;
  }

  if (!command_valid_) {
    LOG(L_ERR, "startATCommand [%.*s] failed: command in the buffer is invalid\r\n", command_buffer_.len,
        command_buffer_.s);
    return false;
  }

  response_buffer_.len = 0;

  LOG(L_DBG, "Output to the modem \r\n");
  LOGSTR(L_DBG, command_buffer_);
  if (!sendData(command_buffer_)) {
    LOG(L_ERR, "sendCommand [%.*s] failed: writing to serial device failed\r\n", command_buffer_.len,
        command_buffer_.s);
    return false;
  }

  if (!sendData("\r\n")) {
    LOG(L_ERR, "sendCommand [%.*s] failed: writing to serial device failed\r\n", command_buffer_.len,
        command_buffer_.s);
    return false;
  }

  command_valid_ = false;  // Command sent, invalidating the buffer

  command_data_      = data;
  command_data_term_ = data_term;
  command_started_   = owl_time();
  command_timeout_   = timeout_ms;

  state_ = (command_data_.s == nullptr) ? modem_state_t::wait_result : modem_state_t::wait_prompt;

  return true;
}

at_result_code OwlModemAT::doCommandBlocking(owl_time_t timeout_millis, str *out_response, str command_data,
                                             uint16_t data_term) {
  if (!startATCommand(timeout_millis, command_data, data_term)) {
    return at_result_code::ERROR;
  }

  for (;;) {
    owl_delay(AT_DO_COMMAND_SPIN_INTERVAL);
    spin();

    switch (state_) {
      case modem_state_t::send_data:
      case modem_state_t::wait_result:
      case modem_state_t::wait_prompt:
        continue;
      case modem_state_t::response_ready:
        return getLastCommandResponse(out_response);
      case modem_state_t::idle:
      default:
        return at_result_code::unknown;
    }
  }
}

void OwlModemAT::filterResponse(str prefix, str response, str *filtered) {
  if (!filtered) {
    return;
  }

  str line = {0};
  while (str_tok(response, "\n", &line)) {
    if (str_equal_prefix(line, prefix)) {
      line.s += prefix.len;
      line.len -= prefix.len;

      if (line.s[line.len - 1] == '\n') {
        --line.len;
      }
      *filtered = line;
      return;
    }
  }

  *filtered = {nullptr, 0};
}

bool OwlModemAT::sendData(str data) {
  uint32_t written = 0;
  int32_t cnt;

  do {
    cnt = serial_->write((const uint8_t *)data.s + written, data.len - written);
    if (cnt <= 0) {
      LOG(L_ERR, "Had %d bytes to send on modem_port, but wrote only %d.\r\n", data.len, written);
      return false;
    }
    written += cnt;
  } while (written < data.len);
  return true;
}

at_result_code OwlModemAT::getLastCommandResponse(str *out_response) {
  if (state_ != modem_state_t::response_ready) {
    return at_result_code::unknown;
  } else {
    state_ = modem_state_t::idle;
    if (out_response) {
      *out_response = response_buffer_;
    }
    return last_response_code_;
  }
}

bool OwlModemAT::initTerminal() {
  if (doCommandBlocking("ATV1", 1000, nullptr) != at_result_code::OK) {
    LOG(L_WARN, "Potential error setting commands to always return response codes\r\n");
  }

  if (doCommandBlocking("ATQ0", 1000, nullptr) != at_result_code::OK) {
    LOG(L_ERR, "Error setting commands to return text response codes\r\n");
    return false;
  }

  if (doCommandBlocking("ATE0", 1000, nullptr) != at_result_code::OK) {
    LOG(L_ERR, "Error setting echo off\r\n");
    return false;
  }

  if (doCommandBlocking("AT+CMEE=2", 1000, nullptr) != at_result_code::OK) {
    LOG(L_ERR, "Error setting Modem Errors output to verbose (not numeric) values\r\n");
    return false;
  }

  if (doCommandBlocking("ATS3=13", 1000, nullptr) != at_result_code::OK) {
    LOG(L_WARN, "Error setting command terminating character\r\n");
  }

  if (doCommandBlocking("ATS4=10", 1000, nullptr) != at_result_code::OK) {
    LOG(L_WARN, "Error setting response separator character\r\n");
  }

  return true;
}

bool OwlModemAT::registerUrcHandler(const char *unique_id, UrcHandler handler, void *priv) {
  // First look if the handler with this id is already registered, if so replace
  for (int i = 0; i < num_urc_handlers_; i++) {
    if (strcmp(urc_handler_ids_[i], unique_id) == 0) {
      urc_handlers_[i]       = handler;
      urc_handler_params_[i] = priv;
      return true;
    }
  }

  // Then append the handler to the array
  if (num_urc_handlers_ >= MaxUrcHandlers) {
    return false;
  }

  urc_handlers_[num_urc_handlers_]       = handler;
  urc_handler_params_[num_urc_handlers_] = priv;
  urc_handler_ids_[num_urc_handlers_]    = unique_id;

  ++num_urc_handlers_;
  return true;
}

void OwlModemAT::registerPrefixHandler(PrefixHandler handler, void *priv, const str *prefixes, int num_prefixes) {
  num_special_prefixes_ = num_prefixes;

  for (int i = 0; i < num_prefixes && i < MaxPrefixes; ++i) {
    special_prefixes_[i] = prefixes[i];
  }

  prefix_handler_       = handler;
  prefix_handler_param_ = priv;
}

void OwlModemAT::deregisterPrefixHandler() {
  num_special_prefixes_ = 0;
  prefix_handler_       = nullptr;
}

bool OwlModemAT::commandSprintf(const char *format, ...) {
  va_list args;
  va_start(args, format);

  int res = vsnprintf(command_buffer_.s, AT_COMMAND_BUFFER_SIZE, format, args);

  va_end(args);

  // vsnprintf returns the number of bytes that _would_ have been written if the
  //   buffer was unlimited
  if (res <= 0 || res > AT_COMMAND_BUFFER_SIZE) {
    LOG(L_ERR, "Command does not fit in the buffer, invalidating\r\n");
    command_valid_ = false;
    return false;
  }
  command_buffer_.len = res;
  command_valid_      = true;


  return true;
}

bool OwlModemAT::commandStrcpy(const char *command) {
  int res = strlen(command);
  if (res > AT_COMMAND_BUFFER_SIZE) {
    LOG(L_ERR, "Command does not fit in the buffer, invalidating\r\n");
    command_valid_ = false;
    return false;
  }
  command_buffer_.len = res;
  memcpy(command_buffer_.s, command, command_buffer_.len);
  command_valid_ = true;
  return true;
}

bool OwlModemAT::commandStrcat(const char *data) {
  if (!command_valid_) {
    return false;
  }

  int data_len = strlen(data);

  if (command_buffer_.len + data_len > AT_COMMAND_BUFFER_SIZE) {
    LOG(L_ERR, "Command does not fit in the buffer, invalidating\r\n");
    command_valid_ = false;
    return false;
  }

  if (data_len > 0) {
    memcpy(command_buffer_.s + command_buffer_.len, data, data_len);
    command_buffer_.len += data_len;
  }
  command_valid_ = true;
  return true;
}

bool OwlModemAT::commandAppendHex(str data) {
  if (!command_valid_) {
    return false;
  }

  if (command_buffer_.len + 2 * data.len > AT_COMMAND_BUFFER_SIZE) {
    LOG(L_ERR, "Command does not fit in the buffer, invalidating\r\n");
    command_valid_ = false;
    return false;
  }

  command_buffer_.len +=
      str_to_hex(command_buffer_.s + command_buffer_.len, AT_COMMAND_BUFFER_SIZE - command_buffer_.len, data);

  command_valid_ = true;
  return true;
}
