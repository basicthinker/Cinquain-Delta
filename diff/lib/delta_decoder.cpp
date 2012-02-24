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
  
  DeltaInstruction *header_ptr = (DeltaInstruction *)(delta + sizeof(offset_t));
  const DeltaInstruction *header_end =
      (DeltaInstruction *)(delta + end_instruction);
      // version_file_size as the offset of end instruction
  
  offset_t length;
  char *delta_ptr = output_;
  while (header_ptr < header_end) {
    switch (header_ptr->type()) {
      case COPY:
        length = (header_ptr + 1)->offset() - header_ptr->offset();
        memcpy(delta_ptr, reference + header_ptr->attribute(), length);
        delta_ptr += length;
        break;
      case ADD:
        length = (header_ptr + 1)->offset() - header_ptr->offset();
        memcpy(delta_ptr, data_section + header_ptr->attribute(), length);
        delta_ptr += length;
        break;
      default:
        throw "[CinquainDecoder::Decode] Invalid type for header_ptr.";
        break;
    }
    ++header_ptr;
  }
}
