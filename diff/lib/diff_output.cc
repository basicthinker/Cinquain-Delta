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
//  diff_output.cc
//  Cinquain-Delta
//
//  Created by Jinglei Ren <jinglei.ren@gmail.com> on 2/4/12.
//

#include "diff_output.h"

void InMemoryOutput::Flush() {
  if (header_) free(header_);
  
  header_ = (DeltaInstruction *)
      malloc(sizeof(DeltaInstruction) * instructions_.size());
  // Write header to buffer
  DeltaInstruction *header_iter = header_;
  for (InstructionIterator iter = instructions_.begin();
       iter != instructions_.end(); ++iter) {
    if (iter->IsValid()) {
      *header_iter = *iter;
      ++header_iter;
    }
  }
  num_instructions_ = int(header_iter - header_);
  
  length_ = 0;
}

offset_t InMemoryOutput::GetLength() {
  if (length_) return length_;

  // Calculate version data length
  offset_t data_length = 0;
  DeltaInstruction *header_iter = header_ + num_instructions_;
  offset_t last_offset = (--header_iter)->offset(); // EOF
  while (header_iter > header_) {
    --header_iter;
    if (header_iter->type() == ADD) {
      data_length += (last_offset - header_iter->offset());
    }
    last_offset = header_iter->offset();
  } // when header_iter == header
  
  return length_ = data_length + num_instructions_ * sizeof(DeltaInstruction);
}

offset_t InMemoryOutput::Write(char *output, const offset_t length) {
  if (length != length_) {
    return 0;
  }
  
  offset_t memcpy_length = num_instructions_ * sizeof(DeltaInstruction);
  memcpy(output, header_, memcpy_length);
  output += memcpy_length;
 
  for (int i = 0; i < num_instructions_ - 1; ++i) {
    switch (header_[i].type()) {
      case ADD:
        memcpy_length = header_[i + 1].offset() - header_[i].offset();
        memcpy(output, string_v_ + header_[i].offset(), memcpy_length);
        output += memcpy_length;
        break;
      case COPY:
        memcpy_length = header_[i + 1].offset() - header_[i].offset();
        memcpy(output, string_r_ + header_[i].offset(), memcpy_length);
        output += memcpy_length;
        break;
      case END:
        return header_[i].offset();
    }
  }
}
