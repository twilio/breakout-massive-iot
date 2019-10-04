#ifndef TEST_PLATFORM_H__
#define TEST_PLATFORM_H__

#include "modem/IOwlSerial.h"
#include <string>

class TestSerial : public IOwlSerial {
 public:
  int32_t available() {
    return mt_to_te.length();
  }
  int32_t read(uint8_t* buf, uint32_t count) {
    uint32_t to_read = (count > mt_to_te.length()) ? mt_to_te.length() : count;

    memcpy(buf, mt_to_te.c_str(), to_read);
    mt_to_te = mt_to_te.substr(to_read);

    return to_read;
  }

  int32_t write(const uint8_t* buf, uint32_t count) {
    std::string data = std::string((char*)buf, count);

    te_to_mt += data;

    return count;
  }
  std::string mt_to_te;
  std::string te_to_mt;
};


#endif // TEST_PLATFORM_H__
