#ifndef ARDUINO  // arduino tries to compile everything in src directory, but this is not intended for the target

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_platform.h"
#include "modem/OwlNMEA0183.h"

std::string str_to_string(str s) {
  return std::string(s.s, s.len);
}

TEST_CASE("OwlNMEA0183 breaks parses correctly", "[nmea0183]") {
  SECTION("simple case") {
    INFO("Testing simple case");
    TestSerial serial;
    OwlNMEA0183 nmea(&serial);

    serial.mt_to_te = "garbage\r\ngarbage$SMMES,0,1,2*5A\r\ngarbage";

    str ret = nmea.getLine();

    REQUIRE(str_to_string(ret) == "SMMES,0,1,2");

    ret = nmea.getLine();

    REQUIRE((ret.s == nullptr && ret.len == 0));
  }

  SECTION("Wrong CS") {
    INFO("Testing input with wrong checksum");
    TestSerial serial;
    OwlNMEA0183 nmea(&serial);

    serial.mt_to_te = "garbage\r\ngarbage$SMMES,0,1,2*5A\r\ngarbage$SMMES,0,1,3*5A\r\ngarbage$SMMES,0,1,2*5A\r\n";

    str ret = nmea.getLine();
    REQUIRE(str_to_string(ret) == "SMMES,0,1,2");

    ret = nmea.getLine();
    REQUIRE(str_to_string(ret) == "SMMES,0,1,2");

    ret = nmea.getLine();
    REQUIRE((ret.s == nullptr && ret.len == 0));
  }

  SECTION("Long input") {
    INFO("Testing long inputs");
    TestSerial serial;
    OwlNMEA0183 nmea(&serial);

    serial.mt_to_te =
        "very long chunk of garbage quickly taking all the space in the input buffer of NMEA0183 parser\r\n";

    for (int i = 0; i < 10; i++) {
      serial.mt_to_te += "garbage\r\ngarbage$SMMES,0,1,2*5A\r\ngarbage$SMMES,0,1,3*5A\r\ngarbage$SMMES,0,1,2*5A\r\n";
      serial.mt_to_te +=
          "$SMMES,a,very,long,message,which,does,not,fit,the,message,buffer,because,it,is,longer,than,100,charachers,"
          "does,have,a,good,checksum,though*08\r\n";
    }

    int line_count = 0;
    for (int i = 0; i < 100; i++) {
      str ret = nmea.getLine();

      if (ret.s != nullptr) {
        REQUIRE(str_to_string(ret) == "SMMES,0,1,2");
        line_count++;
      }
    }

    REQUIRE(line_count == 20);
  }
}

#endif  // ARDUINO
