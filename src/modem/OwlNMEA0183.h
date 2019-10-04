#ifndef __OWL_NMEA0183_H__
#define __OWL_NMEA0183_H__

#include "IOwlSerial.h"
#include "../utils/utils.h"

#define NMEA_INPUT_BUFFER_SIZE 64
#define NMEA_LINE_BUFFER_SIZE 100  // Maximum message size according to NMEA0183 is 82 characters, but let's be graceful
class OwlNMEA0183 {
 public:
  OwlNMEA0183(IOwlSerial *serial) : serial_(serial) {
  }

  str getLine();
  bool bufEmpty() {
    return serial_->available() == 0;
  }

 private:
  bool checksumOk();
  enum class nmea_state_t {
    idle,
    in_line,
  };

  IOwlSerial *serial_{nullptr};

  char input_buffer_c_[NMEA_INPUT_BUFFER_SIZE];
  str input_buffer_ = {.s = input_buffer_c_, .len = 0};

  char line_buffer_c_[NMEA_LINE_BUFFER_SIZE];
  str line_buffer_ = {.s = line_buffer_c_, .len = 0};

  nmea_state_t nmea_state_{nmea_state_t::idle};
};

#endif  // __OWL_NMEA0183_H__
