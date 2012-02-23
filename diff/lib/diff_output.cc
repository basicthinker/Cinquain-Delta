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
  if (output_) delete[] output_;
  
  const DeltaInstruction *header_begin = (DeltaInstruction *)
      new char[sizeof(DeltaInstruction) * instructions_.size()];
  // Write header to buffer
  DeltaInstruction *header_ptr = (DeltaInstruction *)header_begin;
  for (InstructionIterator instruction_iter = instructions_.begin();
       instruction_iter != instructions_.end(); ++instruction_iter) {
    if (instruction_iter->IsValid()) {
      *header_ptr = *instruction_iter;
      ++header_ptr;
    }
  }
  const DeltaInstruction *header_end = header_ptr - 1; // END
  const offset_t instruction_length = offset_t(sizeof(DeltaInstruction) * 
                                               (header_end - header_begin));
  
  // Calculate version data length
  offset_t data_length = 0;
  offset_t version_size = 0;
  for (header_ptr = (DeltaInstruction *)header_begin;
       header_ptr < header_end; ++header_ptr) {
    if (header_ptr->type() == ADD) {
      header_ptr->set_attribute(data_length);
      data_length += (header_ptr + 1)->offset() - header_ptr->offset();
    }
    version_size += (header_ptr + 1)->offset() - header_ptr->offset();
  }
  
  delta_size_ = offset_t(sizeof(offset_t) + instruction_length + 
      sizeof(offset_t) + data_length);
  output_ = new char[delta_size_];
  char *output_ptr = output_ptr;
  
  // Instruction Section
  *((offset_t *)output_ptr) = instruction_length + sizeof(offset_t);
  output_ptr += sizeof(offset_t);
  
  memcpy(output_ptr, header_begin, instruction_length);
  output_ptr += instruction_length;
  
  // Data Section
  *((offset_t *)output_ptr) = version_size;
  output_ptr += sizeof(offset_t);
  
  offset_t memcpy_length;
  for (header_ptr = (DeltaInstruction *)header_begin;
       header_ptr < header_end; ++header_ptr) {
    if (header_ptr->type() == ADD) {
      memcpy_length = (header_ptr + 1)->offset() - header_ptr->offset();
      memcpy(output_ptr, string_v_ + header_ptr->offset(), memcpy_length);
      output_ptr += memcpy_length;
    }
  }
  
  delete[] header_begin;
}

