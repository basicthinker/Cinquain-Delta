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
  COPY = 1
};

class DeltaInstruction {
public:
  DeltaInstruction(InstructionType type, offset_t offset, offset_t attribute = 0);
  bool IsValid();
  void SetInvalid();
  void SetValid();
  InstructionType GetType();
  offset_t GetOffset();
  offset_t GetAttribute();
  bool operator <(const DeltaInstruction &other) const;
private:
  offset_t offset_v_; // the LSB denotes validity
  offset_t attribute_; // the LSB denotes instruction type
};

inline DeltaInstruction::DeltaInstruction(InstructionType type,
                                        offset_t offset, offset_t attribute) {
  offset_v_ = offset << 1 | 1;
  switch (type) {
    case ADD:
      attribute_ = ADD;
      break;
    case COPY:
      attribute_ = attribute << 1 | COPY;
      break;
  }
}

inline bool DeltaInstruction::IsValid() {
  return (char)offset_v_ ^ 0x01;
}

inline void DeltaInstruction::SetInvalid() {
  offset_v_ >>= 1;
  offset_v_ <<= 1;
}

inline void DeltaInstruction::SetValid() {
  offset_v_ |= 1;
}

inline InstructionType DeltaInstruction::GetType() {
  return InstructionType((char)attribute_ ^ 0x01);
}

inline offset_t DeltaInstruction::GetOffset() {
  return offset_v_ >> 1;
}

inline offset_t DeltaInstruction::GetAttribute() {
  return attribute_ >> 1;
}

inline bool DeltaInstruction::operator<(const DeltaInstruction &other) const {
  return this->offset_v_ < other.offset_v_;
}

#endif // CINQUAIN_DELTA_INSTRUCTION_H_
