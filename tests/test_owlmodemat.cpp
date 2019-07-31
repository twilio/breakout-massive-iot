#ifndef ARDUINO  // arduino tries to compile everything in src directory, but this is not intended for the target

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "modem/OwlModemAT.h"
#include "utils/md5.h"
#include "utils/base64.h"
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <string>

std::vector<std::string> received_strings;

void spinProcessLineTestpoint(str line) {
  received_strings.push_back(std::string(line.s, line.len));
}

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

TEST_CASE("OwlModemAT breaks input into lines correctly", "[linesplit]") {
  SECTION("simple case") {
    INFO("Testing simple case");
    TestSerial serial;
    OwlModemAT modem(&serial);

    received_strings.clear();
    serial.mt_to_te += "\r\nLINE0\r\n\r\nLINE1\r\n\r\nLINE2\r\n";

    modem.spin();

    REQUIRE(received_strings == std::vector<std::string>({"LINE0", "LINE1", "LINE2"}));
  }

  SECTION("in the middle") {
    INFO("Testing starting in the middle of stream");
    TestSerial serial;
    OwlModemAT modem(&serial);

    received_strings.clear();
    serial.mt_to_te += "LINE0\r\n\r\nLINE1\r\n\r\nLINE2\r\n";

    modem.spin();

    // LINE0 lacks leading '\r\n', but it still processed to be nice to
    //   misbehaving modems
    REQUIRE(received_strings ==
            std::vector<std::string>({"LINE0", "LINE1", "LINE2"}));
  }

  SECTION("fuzzy input") {
    INFO("Testing input with corrupted delimeters");
    TestSerial serial;
    OwlModemAT modem(&serial);

    received_strings.clear();
    serial.mt_to_te += "\r\nLINE0\n\nLINE1\r\n\r\nLINE2\r\n\rLINE3\r\n\r\nLINE4\r\n";

    modem.spin();

    // lines 0, 1 and 3 can be lost or corrupted, lines 2 and 4 should be present
    bool line_2_found = false;
    bool line_4_found = false;
    for (auto& line : received_strings) {
      if (line == "LINE2") {
        line_2_found = true;
      }

      if (line == "LINE4") {
        line_4_found = true;
      }
    }

    REQUIRE(line_2_found);
    REQUIRE(line_4_found);
  }

  SECTION("Split 0D0A") {
    INFO("Testing input where line delimeters are split between readings");
    TestSerial serial;
    OwlModemAT modem(&serial);

    received_strings.clear();
    serial.mt_to_te = "\r\nLINE0\r";
    modem.spin();

    REQUIRE(received_strings.empty());

    serial.mt_to_te = "\n";
    modem.spin();

    REQUIRE(received_strings.size() == 1);
    REQUIRE(received_strings[0] == "LINE0");
  }
}

std::vector<std::pair<std::string, std::string>> test_urcs;

bool test_urc_handler(str urc, str data, void* priv) {
  if (std::string(urc.s, urc.len) == "+CPIN") {
    test_urcs.push_back({std::string(urc.s, urc.len), std::string(data.s, data.len)});
    return true;
  } else {
    return false;
  }
}

TEST_CASE("OwlModemAT calls URC handlers", "[urc]") {
  INFO("Testing URC handlers");

  TestSerial serial;
  OwlModemAT modem(&serial);

  test_urcs.clear();

  REQUIRE(modem.registerUrcHandler("Test", test_urc_handler, nullptr));

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::idle);

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::idle);

  REQUIRE(test_urcs.size() == 0);

  serial.mt_to_te += "\r\nRDY\r\n\r\n+CPIN: READY\r\n\r\nWild string\r\n";

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(test_urcs.size() == 1);
  REQUIRE(test_urcs[0].first == "+CPIN");
  REQUIRE(test_urcs[0].second == "READY");

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::idle);
}

TEST_CASE("OwlModemAT processes simple commands correctly", "[command]") {
  INFO("Testing simple command");
  TestSerial serial;
  OwlModemAT modem(&serial);

  REQUIRE(modem.startATCommand("AT+COPS?", 1000));
  REQUIRE(serial.te_to_mt == "AT+COPS?\r\n");

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::wait_result);

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::wait_result);

  serial.mt_to_te += "\r\n+COPS: 1\r\n\r\nOK\r\n";

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::response_ready);
  str response;
  REQUIRE(modem.getLastCommandResponse(&response) == AT_Result_Code__OK);

  REQUIRE(std::string(response.s, response.len) == "+COPS: 1\n");
}

TEST_CASE("OwlModemAT processes URC while command is running", "[command-urc]") {
  INFO("Testing command with URC");

  TestSerial serial;
  OwlModemAT modem(&serial);

  test_urcs.clear();

  REQUIRE(modem.registerUrcHandler("Test", test_urc_handler, nullptr));

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::idle);

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.startATCommand("AT+COPS?", 1000));
  REQUIRE(serial.te_to_mt == "AT+COPS?\r\n");

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::wait_result);

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::wait_result);

  REQUIRE(test_urcs.size() == 0);

  serial.mt_to_te += "\r\n+COPS: 1\r\n\r\n+CPIN: READY\r\n\r\nOK\r\n";

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(test_urcs.size() == 1);
  REQUIRE(test_urcs[0].first == "+CPIN");
  REQUIRE(test_urcs[0].second == "READY");

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::response_ready);

  str response;
  REQUIRE(modem.getLastCommandResponse(&response) == AT_Result_Code__OK);

  REQUIRE(std::string(response.s, response.len) == "+COPS: 1\n");
}

TEST_CASE("Command response data is not affected by previous responses", "[command-reponse-clean]") {
  INFO("Veryfying that response data is clean");

  TestSerial serial;
  OwlModemAT modem(&serial);

  test_urcs.clear();

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::idle);

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.startATCommand("AT+COPS?", 1000));
  REQUIRE(serial.te_to_mt == "AT+COPS?\r\n");
  serial.te_to_mt = "";

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::wait_result);

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::wait_result);

  serial.mt_to_te += "\r\n+COPS: 1\r\n\r\nOK\r\n";

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::response_ready);

  str response;
  REQUIRE(modem.getLastCommandResponse(&response) == AT_Result_Code__OK);

  REQUIRE(std::string(response.s, response.len) == "+COPS: 1\n");

  REQUIRE(modem.startATCommand("AT+COPS?", 1000));
  REQUIRE(serial.te_to_mt == "AT+COPS?\r\n");

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::wait_result);

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::wait_result);

  serial.mt_to_te += "\r\n+COPS: 1\r\n\r\nOK\r\n";

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::response_ready);

  REQUIRE(modem.getLastCommandResponse(&response) == AT_Result_Code__OK);

  REQUIRE(std::string(response.s, response.len) == "+COPS: 1\n");
}

TEST_CASE("OwlModemAT processes commands with data", "[command-data]") {
  INFO("Testing command with data");

  TestSerial serial;
  OwlModemAT modem(&serial);
  std::string data_string =
      "For the understanding, like the eye, judging of objects only by its own sight, cannot but be pleased with what "
      "it discovers, having less regret for what has escaped it, because it is unknown. Thus he who has raised himself "
      "above the alms-basket, and, not content to live lazily on scraps of begged opinions, sets his own thoughts on "
      "work, to find and follow truth, will (whatever he lights on) not miss the hunter’s satisfaction; every moment "
      "of his pursuit will reward his pains with some delight; and he will have reason to think his time not ill "
      "spent, even when he cannot much boast of any great acquisition.";
  std::string command_string = "AT_QFUPL=\"file\"," + std::to_string(data_string.length());
  str data                   = {.s = (char*)data_string.c_str(), .len = static_cast<int>(data_string.length())};

  REQUIRE(modem.startATCommand(command_string.c_str(), 1000, data));
  REQUIRE(serial.te_to_mt == (command_string + "\r\n"));

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::wait_prompt);

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::wait_prompt);

  serial.te_to_mt.clear();
  serial.mt_to_te += "\r\nCONNECT\r\n";

  for (int i = 0; i < 50; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::wait_result);
  REQUIRE(serial.te_to_mt == data_string);
  serial.mt_to_te += "\r\nOK\r\n";

  for (int i = 0; i < 5; i++) {
    // spin for a while
    modem.spin();
  }

  REQUIRE(modem.getModemState() == OwlModemAT::modem_state_t::response_ready);
  str response;
  REQUIRE(modem.getLastCommandResponse(&response) == AT_Result_Code__OK);

  REQUIRE(std::string(response.s, response.len) == "");
}

TEST_CASE("MD5 hash is calculated correctly", "[md5]") {
  std::string data =
      "Beware the Jabberwock, my son!\nThe jaws that bite, the claws that catch!\nBeware the Jubjub bird, and "
      "shun\nThe frumious Bandersnatch!";

  /* Hash with utils/md5.c */
  struct MD5Context ctx;
  MD5Init(&ctx);
  MD5Update(&ctx, (unsigned char*)data.c_str(), data.length());

  unsigned char utils_digest[16];
  MD5Final(utils_digest, &ctx);

  /* Hash with OpenSSL*/
  MD5_CTX ctx_openssl;
  REQUIRE(MD5_Init(&ctx_openssl) == 1);
  REQUIRE(MD5_Update(&ctx_openssl, (unsigned char*)data.c_str(), data.length()) == 1);

  unsigned char openssl_digest[16];
  REQUIRE(MD5_Final(openssl_digest, &ctx_openssl));
  REQUIRE(memcmp(utils_digest, openssl_digest, 16) == 0);
}

TEST_CASE("Base64 encoding/decoding works correctly", "[base64]") {
  std::string data =
      "Beware the Jabberwock, my son!\nThe jaws that bite, the claws that catch!\nBeware the Jubjub bird, and "
      "shun\nThe frumious Bandersnatch!";

  // Encode with utils/base64.h
  char* data_b64_utils = new char[owl_base64encode_len(data.length())];
  owl_base64encode(data_b64_utils, (unsigned char*)data.c_str(), data.length());
  unsigned char* data_plain_utils = new unsigned char[owl_base64decode_len(data_b64_utils)];
  owl_base64decode(data_plain_utils, data_b64_utils);

  REQUIRE(owl_base64decode_len(data_b64_utils) == data.length());
  REQUIRE(std::string((char*)data_plain_utils, owl_base64decode_len(data_b64_utils)) == data);

  // Encode with OpenSSL
  char* data_b64_openssl = new char[owl_base64encode_len(data.length())];
  EVP_EncodeBlock((unsigned char*)data_b64_openssl, (unsigned char*)data.c_str(), data.length());
  REQUIRE(strcmp(data_b64_utils, data_b64_openssl) == 0);

  delete[] data_b64_utils;
  delete[] data_plain_utils;
  delete[] data_b64_openssl;
}

TEST_CASE("MD5 hash on base64-encoded data is calculated correctly", "[md5][base64]") {
  std::string data =
      "Beware the Jabberwock, my son!\nThe jaws that bite, the claws that catch!\nBeware the Jubjub bird, and "
      "shun\nThe frumious Bandersnatch!";

  // Hash plain data
  struct MD5Context ctx;
  MD5Init(&ctx);
  MD5Update(&ctx, (unsigned char*)data.c_str(), data.length());

  unsigned char plain_digest[16];
  MD5Final(plain_digest, &ctx);

  // Encode data
  char* data_b64 = new char[owl_base64encode_len(data.length())];
  owl_base64encode(data_b64, (unsigned char*)data.c_str(), data.length());

  // Hash encoded data
  unsigned char base64_digest[16];
  owl_base64decode_md5(base64_digest, data_b64);
  REQUIRE(memcmp(base64_digest, plain_digest, 16) == 0);
}

#endif  // ARDUINO
