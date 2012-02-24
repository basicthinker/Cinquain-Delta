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
  END = 0,
  ADD = 1,
  COPY = 2,
  INVALID = -1
};

class DeltaInstruction {
public:
  DeltaInstruction();
  DeltaInstruction(InstructionType type, offset_t offset,
                   offset_t attribute = 0);
  
  InstructionType type() const;
  void set_type(InstructionType type);
  
  // Returns the offset on the version string
  offset_t offset() const;
  void set_offset(const offset_t offset);
  
  // Returns the length for ADD instruction or
  // the offset on the reference string for COPY instruction.
  offset_t attribute() const;
  void set_attribute(const offset_t attribute);
  
  void Reset(InstructionType type, offset_t offset, offset_t attribute = 0);
  bool IsValid();
  void SetInvalid();
  
  friend bool operator<(const int value, const DeltaInstruction &instruction);
  
private:
  offset_t offset_; // placed first to get version_file_size in end instruction
  InstructionType type_;
  offset_t attribute_;
};

inline DeltaInstruction::DeltaInstruction()
    : offset_(0), attribute_(0), type_(INVALID){
  
}

inline DeltaInstruction::DeltaInstruction(InstructionType type,
                                        offset_t offset, offset_t attribute)
    : offset_(offset), attribute_(attribute), type_(type){

}

inline void DeltaInstruction::Reset(InstructionType type,
                                    offset_t offset, offset_t attribute) {
  type_ = type;
  offset_ = offset;
  attribute_ = attribute;
}

inline InstructionType DeltaInstruction::type() const {
  return type_;
}

inline void DeltaInstruction::set_type(InstructionType type) {
  type_ = type;
}

inline bool DeltaInstruction::IsValid() {
  return type_ != INVALID;
}

inline void DeltaInstruction::SetInvalid() {
  type_ = INVALID;
}

inline offset_t DeltaInstruction::offset() const {
  return offset_;
}

inline void DeltaInstruction::set_offset(const offset_t offset) {
  offset_ = offset;
}

inline offset_t DeltaInstruction::attribute() const {
  return attribute_;
}

inline void DeltaInstruction::set_attribute(const offset_t attribute) {
  attribute_ = attribute;
}

inline bool operator<(const int value, const DeltaInstruction &instruction) {
  return value < instruction.offset_;
}

#endif // CINQUAIN_DELTA_INSTRUCTION_H_
