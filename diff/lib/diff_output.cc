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

#include "delta_output.h"

void InMemoryOutput::Flush() {
  if (output_) free(output_);
  
  const DeltaInstruction *header_begin = (DeltaInstruction *)
      malloc(sizeof(DeltaInstruction) * instructions_.size());
  // Write header to buffer
  DeltaInstruction *header_iter = (DeltaInstruction *)header_begin;
  for (InstructionIterator iter = instructions_.begin();
       iter != instructions_.end(); ++iter) {
    if (iter->IsValid()) {
      *header_iter = *iter;
      ++header_iter;
    }
  }
  const DeltaInstruction *header_end = header_iter;
  
  // Calculate version data length
  offset_t data_length = 0;
  offset_t file_length = 0;
  offset_t last_offset = (--header_iter)->offset(); // EOF
  while (header_iter > header_begin) {
    --header_iter;
    if (header_iter->type() == ADD) {
      data_length += (last_offset - header_iter->offset());
    }
    file_length += (last_offset - header_iter->offset());
    last_offset = header_iter->offset();
  } // when header_iter == header_begin
  
  offset_t memcpy_length =
      (offset_t)(header_iter - header_begin) * sizeof(DeltaInstruction);
  memcpy(output_, header_begin, memcpy_length);
  output_ += memcpy_length;
  
  while (header_iter < header_end) {
    switch (header_iter->type()) {
      case ADD:
        memcpy_length = (header_iter + 1)->offset() - header_iter->offset();
        memcpy(output_, string_v_ + header_iter->offset(), memcpy_length);
        output_ += memcpy_length;
        break;
      case COPY:
        memcpy_length = (header_iter + 1)->offset() - header_iter->offset();
        memcpy(output_, string_r_ + header_iter->offset(), memcpy_length);
        output_ += memcpy_length;
        break;
      default:
        break;
    }
    ++header_iter;
  }
  
  free((void *)header_begin);
}

