#include "OwlNMEA0183.h"

str OwlNMEA0183::getLine() {
  int available = serial_->available();

  if (available != 0) {
    int max_to_read = NMEA_INPUT_BUFFER_SIZE - input_buffer_.len;

    input_buffer_.len += serial_->read(reinterpret_cast<uint8_t*>(input_buffer_.s + input_buffer_.len),
                                       ((available > max_to_read) ? max_to_read : available));
  }

  if (input_buffer_.len == 0) {
    return {nullptr, 0};
  }

  str input_buffer_slice = input_buffer_;

  do {
    switch (nmea_state_) {
      case nmea_state_t::idle: {
        const char* beg_pos = (char*)memchr(input_buffer_slice.s, '$', input_buffer_slice.len);

        if (beg_pos != nullptr) {
          int beg_idx = (int)(beg_pos - input_buffer_slice.s);
          input_buffer_slice.s += beg_idx + 1;
          input_buffer_slice.len -= beg_idx + 1;

          nmea_state_      = nmea_state_t::in_line;
          line_buffer_.len = 0;
        } else {
          input_buffer_slice.len = 0;
        }
        continue;
      }

      case nmea_state_t::in_line: {
        int end_idx = -1;
        for (int i = 0; i < input_buffer_slice.len; i++) {
          if (input_buffer_slice.s[i] == '\r' || input_buffer_slice.s[i] == '\n') {
            end_idx = i;
            break;
          }
        }

        int to_copy = (end_idx >= 0) ? end_idx : input_buffer_slice.len;

        if (to_copy + line_buffer_.len > NMEA_LINE_BUFFER_SIZE) {
          LOG(L_WARN, "Input line is too long, dropping\r\n");

          input_buffer_slice.s += to_copy;
          input_buffer_slice.len -= to_copy;
          nmea_state_ = nmea_state_t::idle;

          continue;
        }

        memcpy(line_buffer_.s + line_buffer_.len, input_buffer_slice.s, to_copy);
        line_buffer_.len += to_copy;

        input_buffer_slice.s += to_copy;
        input_buffer_slice.len -= to_copy;

        if (end_idx >= 0) {
          nmea_state_ = nmea_state_t::idle;

          if (checksumOk()) {
            // TODO: moving memory is ugly, consider using ring buffer instead
            memmove(input_buffer_.s, input_buffer_slice.s, input_buffer_slice.len);
            input_buffer_.len = input_buffer_slice.len;
            // drop the checksum, checksumOK() guarantees the string i long enough
            line_buffer_.len -= 3;
            return line_buffer_;
          } else {
            continue;
          }
        }
      }
    }
  } while (input_buffer_slice.len > 0);

  input_buffer_.len = 0;

  return {nullptr, 0};
}

bool OwlNMEA0183::checksumOk() {
  const char* ast_pos = (char*)memchr(line_buffer_.s, '*', line_buffer_.len);

  if (ast_pos == nullptr) {
    return false;
  }

  int ast_idx = (int)(ast_pos - line_buffer_.s);

  if (ast_idx != line_buffer_.len - 3) {
    return false;
  }

  int cs = 0;

  for (int i = 0; i < ast_idx; i++) {
    cs ^= line_buffer_.s[i];
  }

  if (cs & 0xF0 != (hex_to_int(line_buffer_.s[ast_idx + 1]) << 4)) {
    return false;
  }

  if (cs & 0x0F != hex_to_int(line_buffer_.s[ast_idx + 2])) {
    return false;
  }

  return true;
}
