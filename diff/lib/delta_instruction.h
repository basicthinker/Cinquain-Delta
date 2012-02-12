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
//  delta_instruction.h
//  Cinquain-Delta
//
//  Created by Jinglei Ren <jinglei.ren@gmail.com> on 2/12/12.
//

#ifndef CINQUAIN_DELTA_INSTRUCTION_H_
#define CINQUAIN_DELTA_INSTRUCTION_H_

typedef unsigned int offset_t;

enum InstructionType {
  ADD = 0,
  COPY = 1,
  INVALID = -1
};

class DeltaInstruction {
public:
  DeltaInstruction();
  DeltaInstruction(InstructionType type, offset_t offset, offset_t length);
  InstructionType type() const;
  void set_type(InstructionType type);
  offset_t offset() const;
  offset_t length() const;
  void set_length(const offset_t length);
  bool operator <(const DeltaInstruction &other) const;
private:
  offset_t offset_;
  offset_t length_;
  InstructionType type_;
};

inline DeltaInstruction::DeltaInstruction()
    : offset_(0), length_(0), type_(INVALID){
  
}

inline DeltaInstruction::DeltaInstruction(InstructionType type,
                                        offset_t offset, offset_t length)
    : offset_(offset), length_(length), type_(type){

}

inline InstructionType DeltaInstruction::type() const {
  return type_;
}

inline void DeltaInstruction::set_type(InstructionType type) {
  type_ = type;
}

inline offset_t DeltaInstruction::offset() const {
  return offset_;
}

inline offset_t DeltaInstruction::length() const {
  return length_;
}

inline void DeltaInstruction::set_length(const offset_t length) {
  length_ = length;
}

inline bool DeltaInstruction::operator<(const DeltaInstruction &other) const {
  return this->offset_ < other.offset_;
}

#endif // CINQUAIN_DELTA_INSTRUCTION_H_
