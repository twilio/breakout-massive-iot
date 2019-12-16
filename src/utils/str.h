/*
 * str.h
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
 * \file str.h - string and operations - not null terminated
 */

#ifndef __OWL_UTILS_STR_H__
#define __OWL_UTILS_STR_H__

#include <stdint.h>
#include <string.h>


/* str type & friends */

typedef struct {
  const char *s;
  unsigned int len;
} str;

typedef struct {
  char *s;
  unsigned int len;

  operator str() const {
    return str{.s = s, .len = len};
  }
} str_mut;



#define STRDEF(_string_, _value_) ((_string_).s = (_value_), (_string_).len = strlen(_value_))

#define STRDECL(_value_)                                                                                               \
  { .s = (char *)(_value_), .len = (unsigned int)strlen(_value_) }



#define str_equal(a, b) ((a).len == (b).len && memcmp((a).s, (b).s, (a).len) == 0)
#define str_equal_char(a, c) ((a).len == strlen(c) && memcmp((a).s, (c), (a).len) == 0)
#define str_equal_prefix(a, p) ((a).len >= (p).len && memcmp((a).s, (p).s, (p).len) == 0)
#define str_equal_prefix_char(a, p) ((a).len >= strlen(p) && memcmp((a).s, (p), strlen(p)) == 0)

#define str_equalcase(a, b) ((a).len == (b).len && strncasecmp((a).s, (b).s, (a).len) == 0)
#define str_equalcase_char(a, c) ((a).len == strlen(c) && strncasecmp((a).s, (c), (a).len) == 0)
#define str_equalcase_prefix(a, p) ((a).len >= (p).len && strncasecmp((a).s, (p).s, (p).len) == 0)
#define str_equalcase_prefix_char(a, p) ((a).len >= strlen(p) && strncasecmp((a).s, (p), strlen(p)) == 0)

void str_skipover_prefix(str *x, str prefix);
int str_tok(str src, const char *sep, str *dst);
int str_tok_with_empty_tokens(str src, const char *sep, str *dst);
long int str_to_long_int(str x, int base);
uint32_t str_to_uint32_t(str x, int base);
double str_to_double(str x);

/**
 * Convert uint8_t to a binary string (e.g. "1010101010")
 * @param x - uint8_t input value
 * @param dst - str with at least [precision] (default 8) character capacity (only [precision] characters will be used)
 * @param precision - (optional) default is 8, can specify smaller precision to get a shorter result; bits that don't
 * fit in input value are ignored
 */
void uint8_t_to_binary_str(uint8_t x, str_mut *dst, uint8_t precision);

int hex_to_int(char c);

unsigned int hex_to_str(char *dst, unsigned int max_dst_len, str src);
unsigned int str_to_hex(char *dst, unsigned int max_dst_len, str src);

int str_find(str x, str y);
int str_find_char(str x, const char *y);

void str_strip(str *s);

#endif
