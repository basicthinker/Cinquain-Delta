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

  InstructionIterator current_iter;
  InstructionIterator next_iter;
  const InstructionIterator end_iter = instructions_.end() - 1;
  
  // Calculate section sizes
  offset_t instruction_size = 0;
  offset_t data_size = 0;
  offset_t length;
  current_iter = instructions_.begin();  
  while (current_iter < end_iter) {
    next_iter = current_iter + 1;
    while (!next_iter->IsValid()) {
      ++next_iter;
    }
    length = next_iter->offset() - current_iter->offset();
    if (current_iter->type() == ADD) {
      current_iter->set_attribute(data_size);
      data_size += length;
    }
    instruction_size += current_iter->size();
    
    current_iter = next_iter;
  }
  const offset_t version_size = current_iter->offset();
  
  // Write header and data
  delta_size_ = offset_t(sizeof(offset_t) + instruction_size + 
      sizeof(offset_t) + data_size);
  output_ = new char[delta_size_];
  
  char *header_ptr = output_;
  *((offset_t *)header_ptr) = sizeof(offset_t) + instruction_size;
  header_ptr += sizeof(offset_t);
  
  char *data_ptr = output_ + sizeof(offset_t) + instruction_size;
  *((offset_t *)data_ptr) = version_size;
  data_ptr += sizeof(offset_t);
  
  current_iter = instructions_.begin();
  while (current_iter < end_iter) {
    next_iter = current_iter + 1;
    while (!next_iter->IsValid()) {
      ++next_iter;
    }
    length = next_iter->offset() - current_iter->offset();
    if (current_iter->type() == ADD) {
      memcpy(data_ptr + current_iter->attribute(),
             string_v_ + current_iter->offset(),
             length);
    }
    header_ptr += current_iter->Write(header_ptr);
    
    current_iter = next_iter;
  }
}

