/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _BASE64_H_
#define _BASE64_H_

int owl_base64encode_len(int len);
int owl_base64encode(char *coded_dst, const unsigned char *plain_src, int len_plain_src);

int owl_base64decode_len(const char *coded_src);
int owl_base64decode(unsigned char *plain_dst, const char *coded_src);
int owl_base64decode_md5(unsigned char digest[16], const char *coded_src);

#endif  //_BASE64_H_
