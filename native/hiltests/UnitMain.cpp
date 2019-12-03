/*
 *
 * Twilio Massive SDK
 *
 * Copyright (c) 2019 Twilio, Inc.
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

#ifndef ARDUINO  // arduino tries to compile everything in src directory, but this is not intended for the target

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <iostream>
#include <string>
#include "modem/OwlModemRN4.h"
#include "CharDeviceSerial.h"

std::string device = std::string("/dev/ttyACM0");
int baudrate       = 115200;
std::string pin    = std::string("0000");
bool berlin        = false;

CharDeviceSerial* serial;
OwlModemRN4* rn4;

int main(int argc, const char* argv[]) {
  Catch::Session session;

  using namespace Catch::clara;
  auto cli = session.cli() |
             Opt(device, "device")["-m"]["--device"]("Path to the device or pcsc:N for a PC/SC interface") |
             Opt(baudrate, "baudrate")["-g"]["--baudrate"]("Baud rate for the serial device") |
             Opt(pin, "pin")["-p"]["--pin"]("PIN code for the Trust Onboard SIM") |
             Opt(berlin)["-1"]["--berlin"]("Set modem to test in Berlin");


  // Now pass the new composite back to Catch so it uses that
  session.cli(cli);

  // Let Catch (using Clara) parse the command line
  int returnCode = session.applyCommandLine(argc, argv);
  if (returnCode != 0)  // Indicates a command line error
    return returnCode;

  owl_log_set_level(L_DBG);

  serial = new CharDeviceSerial(device.c_str(), baudrate);
  rn4    = new OwlModemRN4(serial);

  if (!rn4->powerOn()) {
    LOG(L_ERR, "Failed to power on the modem\n");
    return 1;
  }

  if (berlin) {
    if (!rn4->initModem(Testing__Set_MNO_Profile_to_Default | Testing__Set_APN_Bands_to_Berlin, "iot.1nce.net", "26201",
                        AT_COPS__Format__Numeric)) {
      LOG(L_ERR, "Failed to initialize modem\n");
      return 1;
    }
  } else {
    if (!rn4->initModem(0, "iot.nb")) {
      LOG(L_ERR, "Failed to initialize modem\n");
      return 1;
    }
  }

  return session.run();
}

// TODO: find a better way to insert a test point
void spinProcessLineTestpoint(str line) {
  return;
}

#endif  // ARDUINO
