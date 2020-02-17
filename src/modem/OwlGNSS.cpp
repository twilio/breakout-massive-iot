/*
 * OwlGNSS.h
 * Twilio Breakout SDK
 *
 * Copyright (c) 2018 Twilio, Inc.
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

/**
 * \file OwlGNSS.cpp - API for retrieving various data from the GNSS card
 */

#include "OwlGNSS.h"

#include <math.h>

int OwlGNSS::getGNSSData(gnss_data_t *out_data) {
  str nmea_line;
  int line_count               = 0;
  constexpr int max_line_count = 50;

  do {
    nmea_line = nmea0183_.getLine();

    if (nmea_line.s == nullptr && nmea0183_.bufEmpty()) {
      break;
    }

    line_count++;

    if (nmea_line.len < 6) {
      continue;
    }

    if (memcmp(nmea_line.s, "GPRMC,", 6) != 0) {
      continue;
    }

    nmea_line.s += 6;
    nmea_line.len -= 6;

    str token = {.s = nullptr, .len = 0};
    memset(&lastdata_, 0, sizeof(lastdata_));  // Also sets 'valid' to false
    lastdata_ts_ = owl_time();

    for (int cnt = 0; str_tok_with_empty_tokens(nmea_line, ",", &token); cnt++) {
      LOG(L_DB, "  Token[%d] = [%.*s]\r\n", cnt, token.len, token.s);
      switch (cnt) {
        case 0:
          // time hhmmss.sss
          if (token.len < 6) break;
          lastdata_.time.hours   = (token.s[0] - '0') * 10 + token.s[1] - '0';
          lastdata_.time.minutes = (token.s[2] - '0') * 10 + token.s[3] - '0';
          lastdata_.time.seconds = (token.s[4] - '0') * 10 + token.s[5] - '0';
          //.
          for (unsigned int k = 7; k < token.len; k++)
            lastdata_.time.millis = (token.s[k] - '0') * pow(10, 9 - k);
          break;
        case 1:
          // V/A
          lastdata_.valid = token.len >= 1 && token.s[0] == 'A';
          break;
        case 2:
          // llll.lll
          if (token.len < 8) break;
          lastdata_.position.latitude_degrees = (token.s[0] - '0') * 10 + token.s[1] - '0';
          token.s += 2;
          token.len -= 2;
          lastdata_.position.latitude_minutes = str_to_double(token);
          break;
        case 3:
          // N/S
          if (token.len < 1) break;
          if (token.s[0] == 'N') lastdata_.position.is_north = true;
          break;
        case 4:
          // yyyyy.yyy
          if (token.len < 9) break;
          lastdata_.position.longitude_degrees = (token.s[0] - '0') * 100 + (token.s[1] - '0') * 10 + token.s[2] - '0';
          token.s += 3;
          token.len -= 3;
          lastdata_.position.longitude_minutes = str_to_double(token);
          break;
        case 5:
          // E/W
          if (token.len < 1) break;
          if (token.s[0] == 'W') lastdata_.position.is_west = true;
          break;
        case 6:
          // x.x
          if (token.len < 10) break;
          lastdata_.position.speed_knots = str_to_double(token);
          break;
        case 7:
          // u.u
          if (token.len < 10) break;
          lastdata_.position.course = str_to_double(token);
          break;
        case 8:
          // date ddmmyy
          if (token.len < 6) break;
          lastdata_.date.day   = (token.s[0] - '0') * 10 + token.s[1] - '0';
          lastdata_.date.month = (token.s[2] - '0') * 10 + token.s[3] - '0';
          lastdata_.date.year  = 2000 + (token.s[4] - '0') * 10 + token.s[5] - '0';
          break;
        case 9:
          break;
        case 10:
          break;
        case 11:
          // Mode indicator N/A/D/E
          if (token.len < 1) break;
          lastdata_.mode_indicator = token.s[0];
          break;
        default:
          break;
      }
    }
  } while (line_count < max_line_count);

  if (lastdata_ts_ != 0 && owl_time() <= lastdata_ts_ + GNSS_VALID_MS) {
    if (out_data != nullptr) {
      *out_data = lastdata_;
    }

    return 1;
  } else {
    return 0;
  }
}



void OwlGNSS::logGNSSData(log_level_t level, gnss_data_t data) {
  if (!owl_log_is_printable(level)) return;
  LOG(level, "GNSS Data:  data_valid %s  mode_indicator %c(%s)\r\n", data.valid ? "yes" : "no", data.mode_indicator,
      data.mode_indicator == 'N' ?
          "Data not valid" :
          data.mode_indicator == 'A' ?
          "Autonomous mode" :
          data.mode_indicator == 'D' ? "Differential mode" :
                                       data.mode_indicator == 'E' ? "Estimated (dead reckoning) mode" : "<unknown>");
  if (data.valid) {
    LOG(level, "  - Position:  %d %7.5f %s  %d %7.5f %s\r\n", data.position.latitude_degrees,
        data.position.latitude_minutes, data.position.is_north ? "N" : "S", data.position.longitude_degrees,
        data.position.longitude_minutes, data.position.is_west ? "W" : "E");
    LOG(level, "  - Course:  %4.1f knots  %4.1f degrees\r\n", data.position.speed_knots, data.position.course);
  }
  if (data.date.year) {
    LOG(level, "  - DateTime:  %u-%02u-%02u %02u:%02u:%02u.%03u UTC\r\n", data.date.year, data.date.month,
        data.date.day, data.time.hours, data.time.minutes, data.time.seconds, data.time.millis);
  }
}
