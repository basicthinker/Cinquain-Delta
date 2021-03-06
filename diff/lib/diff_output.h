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
//  diff_output.h
//  Cinquain-Delta
//
//  Created by Jinglei Ren <jinglei.ren@gmail.com> on 2/4/12.
//

/* Compact Format of Output for Ajti's Algorithm (NOT IN USE) */
// Offset          | Size                     | Number | Note   
//
// Address Section: addresses in reference for each copy instruction
// 0               | sizeof(offset_t)         | 1      | = end_address
// NEXT            | sizeof(offset_t)         | ANY
//
// Instruction Section: delta instructions
// end_address     | sizeof(offset_t)         | 1      | = end_instruction
// NEXT            | sizeof(offset_t) + sizeof(InstructionType) | ANY
//
// Data Section: data for add instructions
// end_instruction | sizeof(offset_t)         | 1      | = version_file_size
// NEXT            | ANY                      | ANY

/* Extensible Format of Output (IN USE) */
// Offset          | Size                     | Number | Note
//
// Instruction Section: delta instructions
// 0               | sizeof(offset_t)         | 1      | = end_instruction
// NEXT            | sizeof(DeltaInstruction) | ANY
//
// Data Section: data for add instructions
// end_instruction | sizeof(offset_t)         | 1      | = version_file_size
// NEXT            | ANY                      | ANY

// NEXT = the address following the last specified part
// ANY = some reasonable number

#ifndef CINQUAIN_DELTA_DIFF_OUTPUT_H_
#define CINQUAIN_DELTA_DIFF_OUTPUT_H_

#include "delta_instruction.h"
#include <vector>
#include <algorithm>
using std::vector;
using std::upper_bound;

typedef vector<DeltaInstruction>::iterator InstructionIterator;

class InMemoryOutput;

typedef InMemoryOutput DiffOutput; // one of the above classes


class DiffOutputInterface {
  public:
    virtual void Append(InstructionType instruction,
                        const offset_t offset,
                        const offset_t attribute = 0) = 0;
    virtual void TailCorrect(const offset_t begin_v,
                             const offset_t match_r) = 0;
    virtual void GeneralCorrect(const offset_t begin_v,
                                const offset_t end_v,
                                const offset_t match_r) = 0;
    virtual void Flush() = 0;
    virtual offset_t GetDeltaSize() = 0;
    virtual ~DiffOutputInterface() {}
};


/* Implementing classes */

class InMemoryOutput : public DiffOutputInterface {
  public:
    explicit InMemoryOutput(const char *string_r, const char *string_v,
                            const int capacity, char *&output);
    ~InMemoryOutput();
  
    void Append(InstructionType instruction,
                const offset_t offset, const offset_t attribute = 0);
    void TailCorrect(const offset_t begin_v, const offset_t match_r);
    void GeneralCorrect(const offset_t begin_v, const offset_t end_v,
                        const offset_t match_r);
    void Flush();
    offset_t GetDeltaSize();
  
  private:
    vector<DeltaInstruction> instructions_;

    const char *string_r_;
    const char *string_v_;
    char *&output_;
    
    offset_t delta_size_;
};

inline InMemoryOutput::InMemoryOutput(const char *string_r, const char *string_v,
                                      const int capacity, char *&output) 
    : string_r_(string_r), string_v_(string_v), output_(output) {
  instructions_.reserve(capacity);
  output_ = 0;
  delta_size_ = 0;
}

inline InMemoryOutput::~InMemoryOutput() {
  if (output_) {
    delete[] output_;
  }
}

inline void InMemoryOutput::Append(InstructionType instruction,
                            const offset_t offset_v, const offset_t attribute) {
  instructions_.push_back(DeltaInstruction(instruction, offset_v, attribute));
}

/* Implementation of correction algorithms */
//
// Invariant: all instructions, either valid or not, always keep
//    in order by offset.

inline void InMemoryOutput::TailCorrect(const offset_t begin_v,
                                        const offset_t match_r) {
  if (begin_v == 0) {
    instructions_.clear();
  } else {
    while (instructions_.back().offset() >= begin_v) {
      instructions_.pop_back();
    }
    while (instructions_.back().type() == INVALID) {
      instructions_.pop_back(); // Does not violate the invariant.
    }
  }
  Append(COPY, begin_v, match_r);
}

inline void InMemoryOutput::GeneralCorrect(const offset_t begin_v,
                                           const offset_t end_v,
                                           const offset_t match_r) {
  InstructionIterator overlap_iter = instructions_.back().offset() <= end_v ?
      instructions_.end() - 1 :
      upper_bound(instructions_.begin(), instructions_.end(), end_v) - 1;
  const InstructionIterator physical_tail = overlap_iter; // offset <= end_v
  
  while (!overlap_iter->IsValid()) {
    --overlap_iter;
  }
  const InstructionIterator logic_tail = overlap_iter; // valid offset <= end_v
  if (logic_tail->offset() <= begin_v) { // includes logic_tail->offset() == 0
    return; // when overlapping sequence is covered by an original instruction
  }
  
  InstructionIterator logic_head_end = logic_tail; // = overlap_iter
  --overlap_iter;
  while (overlap_iter->offset() > begin_v) {
    if (overlap_iter->IsValid()) {
      overlap_iter->SetInvalid();
      logic_head_end = overlap_iter;
    }
    --overlap_iter;
  }
  const InstructionIterator physical_head = overlap_iter; // offset <= begin_v
  
  while (!overlap_iter->IsValid()) {
    --overlap_iter;
  }
  const InstructionIterator logic_head = overlap_iter; // valid offset <= begin_v
  
  // Where the new COPY covers
  InstructionIterator new_begin;
  InstructionIterator new_end;
  
  offset_t truncate_length = 0;
  switch (logic_head->type()) {
    case ADD:
      if (logic_head < physical_head) {
        new_begin = physical_head;
      } else if (logic_head->offset() == begin_v) {
        new_begin = logic_head;
      } else {
        new_begin = physical_head + 1;
      }
      break;
    case COPY:
      if (logic_head->offset() < begin_v) {
        new_begin = logic_head_end;
        truncate_length = logic_head_end->offset() - begin_v;
      } else if (logic_head->offset() == begin_v) {
        new_begin = logic_head;
      } else {
        throw "[InMemoryOutput::GeneralCorrect] Invalid logic_head.";
      }
      break;
    default:
      throw "[InMemoryOutput::GeneralCorrect] Invalid type for logic_head.";
      break;
  }
  
  switch (logic_tail->type()) {
    case ADD:
      if (physical_tail->offset() < end_v && 
          physical_tail + 1 != instructions_.end() &&
          !(physical_tail + 1)->IsValid()) {
        new_end = physical_tail + 1;
      } else {
        new_end = physical_tail;
      }
      if (new_begin < new_end) {
        logic_tail->SetInvalid();
        new_end->Reset(ADD, end_v);
        new_begin->Reset(COPY, begin_v + truncate_length,
                         match_r + truncate_length);
      }
      break;
    case COPY:
      new_end = logic_tail;
      if (new_begin < new_end) {
        new_begin->Reset(COPY, begin_v + truncate_length,
                         match_r + truncate_length);
      }
      break;
    default:
      throw "[InMemoryOutput::GeneralCorrect] Invalid type for logic_tail.";
      break;
  }
}

inline offset_t InMemoryOutput::GetDeltaSize() {
  return delta_size_;
}

#endif // CINQUAIN_DELTA_DIFF_OUTPUT_H_
