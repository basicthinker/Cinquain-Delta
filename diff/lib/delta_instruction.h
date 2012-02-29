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

#include <stdint.h>

// #define DEBUG_DELTA

#ifdef DEBUG_DELTA
#include <stdio.h>
#endif

class RawInstruction;
class CompactInstruction;

typedef CompactInstruction DeltaInstruction; // one of the above

typedef uint32_t offset_t;

/*
 * Developers' Adaptable Section.
 * CompactInstruction is extensible to wider width than uint32_t.
 */

const offset_t uint8_max = 0xff;
const offset_t uint16_max = 0xffff;
const offset_t uint24_max = 0xffffff;
const offset_t uint32_max = 0xffffffff;

int GetWidth(offset_t value);
void WriteOffset(int width, offset_t value, char *string);
void ReadOffset(int width, char *string, offset_t &value);

// Returns a number less than 8 (3-bit width).
// Currently the return equals to the number of bytes.
//
// If 3-bit width is not enough, extend
//    CompactInstruction.type_, and
//    CompactInstruction::kTypeBase, as well.
// See below.
inline int GetWidth(offset_t value) {
  if (value <= uint8_max) {
    return 1;
  } else if (value <= uint16_max) {
    return 2;
  } else if (value <= uint24_max) {
    return 3;
  } else if (value <= uint32_max) {
    return 4;
  }
#ifdef DEBUG_DELTA
  throw "[GetWidth] Value overflows.";
#endif
  return 7; // max value
}

inline void WriteOffset(int width, offset_t value, char *string) {
  switch (width) {
    case 1:
      *((uint8_t *)string) = (uint8_t)value;
      break;
    case 2:
      *((uint16_t *)string) = (uint16_t)value;
      break;
    case 3:
      *((uint16_t *)string) = (uint16_t)(value >> 8);
      *((uint8_t *)(string + 2)) = (uint8_t)value;
      break;
    case 4:
      *((uint32_t *)string) = (uint32_t)value;
      break;
#ifdef DEBUG_DELTA
    default:
      throw "[WriteOffset] Width overflows.";
#endif
  }
}

inline void ReadOffset(int width, char *string, offset_t &value) {
  switch (width) {
    case 1:
      value = *((uint8_t *)string);
      break;
    case 2:
      value = *((uint16_t *)string);
      break;
    case 3:
      value = *((uint16_t *)string);
      value = (value << 8) + *((uint8_t *)(string + 2));
      break;
    case 4:
      value = *((uint32_t *)string);
      break;
#ifdef DEBUG_DELTA
    default:
      throw "[ReadOffset] Width overflows.";
#endif
  }
}

// End of adaptable section.

enum InstructionType {
  END = 0,
  ADD = 1,
  COPY = 2,
  INVALID = -1
};

/* Peer Classes */

// Raw format of instructions, using fixed length of fields
// that are directly written to or read from byte strings.
class RawInstruction {
  public:
    RawInstruction() {};
    RawInstruction(InstructionType type, offset_t offset,
                   offset_t attribute = 0);
  
    InstructionType type() const;
    // END instruction should not be invoked this function on.
    void set_type(InstructionType type);
  
    // Returns the offset on the version string
    offset_t offset() const;
    void set_offset(offset_t offset);
  
    // Returns the length for ADD instruction or
    // the offset on the reference string for COPY instruction.
    offset_t attribute() const;
    void set_attribute(offset_t attribute);
    
    void Reset(InstructionType type, offset_t offset, offset_t attribute = 0);
    bool IsValid();
    void SetInvalid();
    
    offset_t size();
    offset_t Write(char *string);
    offset_t Read(char *string);
    
    friend bool operator<(const int value, const RawInstruction &instruction);
    
  private:
    offset_t offset_; // placed first to get version_file_size in end instruction
    InstructionType type_;
    offset_t attribute_;
};

inline RawInstruction::RawInstruction(InstructionType type,
                                        offset_t offset, offset_t attribute)
    : offset_(offset), attribute_(attribute), type_(type){
}

inline void RawInstruction::Reset(InstructionType type,
                                    offset_t offset, offset_t attribute) {
  type_ = type;
  offset_ = offset;
  attribute_ = attribute;
}

inline InstructionType RawInstruction::type() const {
  return type_;
}

inline void RawInstruction::set_type(InstructionType type) {
  type_ = type;
}

inline bool RawInstruction::IsValid() {
  return type_ != INVALID;
}

inline void RawInstruction::SetInvalid() {
  type_ = INVALID;
}

inline offset_t RawInstruction::offset() const {
  return offset_;
}

inline void RawInstruction::set_offset(offset_t offset) {
  offset_ = offset;
}

inline offset_t RawInstruction::attribute() const {
  return attribute_;
}

inline void RawInstruction::set_attribute(offset_t attribute) {
  attribute_ = attribute;
}

inline offset_t RawInstruction::Read(char *string) {
  *this = *((RawInstruction *)string);
  return sizeof(RawInstruction);
}

inline offset_t RawInstruction::Write(char *string) {
  *((RawInstruction *)string) = *this;
  return sizeof(RawInstruction);
}

inline offset_t RawInstruction::size() {
  return sizeof(RawInstruction);
}

inline bool operator<(const int value, const RawInstruction &instruction) {
  return value < instruction.offset_;
}

// Compact format of instructions, using variable length of fields
// that are written to or read from byte strings according to their length.
class CompactInstruction {
  public:
    CompactInstruction() {};
    CompactInstruction(InstructionType type, offset_t offset,
                   offset_t attribute = 0);
    
    InstructionType type() const;
    void set_type(InstructionType type);
    
    // Returns the offset on the version string
    offset_t offset() const;
    void set_offset(offset_t offset);
    
    // Returns the length for ADD instruction or
    // the offset on the reference string for COPY instruction.
    offset_t attribute() const;
    void set_attribute(offset_t attribute);
    
    void Reset(InstructionType type, offset_t offset, offset_t attribute = 0);
    bool IsValid();
    void SetInvalid();
    
    offset_t size();
    offset_t Write(char *string);
    offset_t Read(char *string);
    
    friend bool operator<(const int value, const CompactInstruction &instruction);
    
  private:
    offset_t offset_;
    offset_t attribute_;
    
    /* Developers' adaptable section */
    uint8_t offset_width_; // value less than 7
    uint8_t attribute_width_; // value less than 7 
    int8_t type_; // extensible to denote wider offset values
    
    static const int8_t kTypeBase = 1 << 6;
    
    // Serializes to one byte (currently)
    // the fields type_, offset_width_, and attribute_width_.
    // Only ADD or COPY instruction should be serialized.
    inline offset_t EncodeType(char *string) {
      switch (type_) {
        case ADD:
          *string = (offset_width_ << 3) + attribute_width_;
          break;
        case COPY:
          *string = kTypeBase + (offset_width_ << 3) + attribute_width_;
          break;
    #ifdef DEBUG_DELTA
        default:
          throw "[CompactInstruction::EncodeType] Invalid type to encode.";
    #endif
      }
      return 1; // sizeof(int8_t)
    }
    
    // Sets the fields type_, offset_width_, and attribute_width_.
    // Only ADD or COPY instruction should be meet.
    inline offset_t DecodeType(char *string) {
      if (*string >= kTypeBase) {
        type_ = COPY;
      } else if (*string > 0) {
        type_ = ADD;
      }
    #ifdef DEBUG_DELTA
      else {
        throw "[CompactInstruction::DecodeType] Invalid type to decode.";
      }
    #endif
      offset_width_ = (*string >> 3) & 0x07;
      attribute_width_ = *string & 0x07;
      return 1; // sizeof(int8_t)
    }
};

inline CompactInstruction::CompactInstruction(InstructionType type,
                                              offset_t offset,
                                              offset_t attribute)
    : offset_(offset), attribute_(attribute), type_(type){
  offset_width_ = GetWidth(offset);
  attribute_width_ = GetWidth(attribute);
}

inline InstructionType CompactInstruction::type() const {
  return InstructionType(type_);
}

inline void CompactInstruction::set_type(InstructionType type) {
  type_ = type;
}

inline offset_t CompactInstruction::offset() const {
  return offset_;
}

inline void CompactInstruction::set_offset(offset_t offset) {
  offset_ = offset;
  offset_width_ = GetWidth(offset);
}

inline offset_t CompactInstruction::attribute() const {
  return attribute_;
}

inline void CompactInstruction::set_attribute(offset_t attribute) {
  attribute_ = attribute;
  attribute_width_ = GetWidth(attribute);
}

inline void CompactInstruction::Reset(InstructionType type,
                                      offset_t offset,
                                      offset_t attribute) {
  set_offset(offset);
  set_attribute(attribute);
  set_type(type);
}

inline bool CompactInstruction::IsValid() {
  return type_ != INVALID;
}

inline void CompactInstruction::SetInvalid() {
  type_ = INVALID;
}

inline offset_t CompactInstruction::size() {
  return offset_width_ + attribute_width_ + 1; // sizeof(int8_t)
}


inline offset_t CompactInstruction::Write(char *string) {
  char *str_ptr = string;
  str_ptr += EncodeType(str_ptr);
  WriteOffset(offset_width_, offset_, str_ptr);
  str_ptr += offset_width_;
  WriteOffset(attribute_width_, attribute_, str_ptr);
  str_ptr += attribute_width_;
  return offset_t(str_ptr - string);
}

inline offset_t CompactInstruction::Read(char *string) {
  char *str_ptr = string;
  str_ptr += DecodeType(str_ptr);
  ReadOffset(offset_width_, str_ptr, offset_);
  str_ptr += offset_width_;
  ReadOffset(attribute_width_, str_ptr, attribute_);
  str_ptr += attribute_width_;
  return offset_t(str_ptr - string);
}

inline bool operator<(const int value, const CompactInstruction &instruction) {
  return value < instruction.offset_;
}

#endif // CINQUAIN_DELTA_INSTRUCTION_H_

