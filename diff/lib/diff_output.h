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

#ifndef CINQUAIN_DELTA_DIFF_OUTPUT_H_
#define CINQUAIN_DELTA_DIFF_OUTPUT_H_

#include "delta_instruction.h"
#include <vector>
#include <algorithm>
using std::vector;
using std::upper_bound;

class DiffOutputInterface;
class InMemoryOutput;

typedef DiffOutputInterface DiffOutput; // one of the above classes


class DiffOutputInterface {
  public:
    virtual void Append(InstructionType instruction,
                        const offset_t offset, const offset_t attribute) = 0;
  virtual void TailCorrect(const offset_t begin_v, const offset_t match_r) = 0;
  virtual void GeneralCorrect(const offset_t begin_v, const offset_t end_v,
                              const offset_t match_r) = 0;
    virtual void Flush() = 0;
    virtual ~DiffOutputInterface() {}
};


/* Implementing classes */

class InMemoryOutput : public DiffOutputInterface {
  public:
    explicit InMemoryOutput(const int capacity);
    void Append(InstructionType instruction,
                const offset_t offset, const offset_t attribute);
    void TailCorrect(const offset_t begin_v, const offset_t match_r);
    void GeneralCorrect(const offset_t begin_v, const offset_t end_v,
                        const offset_t match_r);
    void Flush();
  
  private:
    vector<DeltaInstruction> instructions_;
};

inline InMemoryOutput::InMemoryOutput(const int capacity) {
  instructions_.reserve(capacity);
}

inline void InMemoryOutput::Append(InstructionType instruction,
                            const offset_t offset_v, const offset_t attribute) {
  instructions_.push_back(DeltaInstruction(instruction, offset_v, attribute));
}

inline void InMemoryOutput::TailCorrect(const offset_t begin_v,
                                        const offset_t match_r) {
  DeltaInstruction tail;
  while ((tail = instructions_.back()).type() == INVALID
         || tail.offset() >= begin_v) {
    instructions_.pop_back();
  }
  if (tail.type() == ADD) {
    tail.set_attribute(begin_v - tail.offset());
  }
  Append(COPY, begin_v, match_r);
}

inline void InMemoryOutput::GeneralCorrect(const offset_t begin_v,
                                           const offset_t end_v,
                                           const offset_t match_r) {
  vector<DeltaInstruction>::iterator overlap_end =
    upper_bound(instructions_.begin(), instructions_.end(), end_v);
}

#endif // CINQUAIN_DELTA_DIFF_OUTPUT_H_
