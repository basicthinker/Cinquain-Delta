/*
 * Copyright (C) 2012 Jinglei Ren <jinglei.ren@stanzax.org>
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
 *
 */

//
//  delta_decoder.cpp
//  Cinquain-Delta
//
//  Created by Jinglei Ren <jinglei.ren@gmail.com> on 2/22/12.
//

#include "delta_decoder.h"

void CinquainDecoder::Decode(const char *reference, const char *delta) {
  const offset_t end_instruction = *((offset_t *)delta);
  version_size_ = *((offset_t *)(delta + end_instruction));
  const char *data_section = delta + end_instruction + sizeof(offset_t);
  
  if (output_) {
    delete[] output_;
  }
  output_ = new char[version_size_];
  
  char *header_ptr = (char *)delta + sizeof(offset_t);
  const char *header_end = delta + end_instruction;
  
  char *delta_ptr = output_;
  offset_t length;
  DeltaInstruction instruction[2]; // restore current & next instructions
  bool current = 0;
  bool next = 1;
  header_ptr += instruction[current].Read(header_ptr);
  while (header_ptr < header_end) {
    header_ptr += instruction[next].Read(header_ptr);
    length = instruction[next].offset() - instruction[current].offset();
    switch (instruction[current].type()) {
      case COPY:
        memcpy(delta_ptr,
               reference + instruction[current].attribute(), length);
        delta_ptr += length;
        break;
      case ADD:
        memcpy(delta_ptr,
               data_section + instruction[current].attribute(), length);
        delta_ptr += length;
        break;
      default:
        throw "[CinquainDecoder::Decode] Invalid type for header_ptr.";
        break;
    }
    current = next;
    next = !next;
  }
  // Write the last instruction
  length = version_size_ - instruction[current].offset();
  switch (instruction[current].type()) {
    case COPY:
      memcpy(delta_ptr,
             reference + instruction[current].attribute(), length);
      break;
    case ADD:
      memcpy(delta_ptr,
             data_section + instruction[current].attribute(), length);
      break;
    default:
      throw "[CinquainDecoder::Decode] Invalid type for header_ptr.";
      break;
  }
}
