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

/*
 * rabin.h
 *
 *  Created on: Feb 1, 2012
 *      Author: Jinglei Ren <jinglei.ren@gmail.com>
 */

#ifndef CINQUAIN_RABIN_FINGERPRINT_H_
#define CINQUAIN_RABIN_FINGERPRINT_H_

#include "rabin_config.h"
#include <memory.h>

class NumericalWindow;
class SimpleWindow;

typedef SimpleWindow RabinWindow; // one of the above

#ifdef DEBUG_FINGERPRINT
#include <iostream>
using std::cerr;
using std::endl;
#endif

Int SymbolShift(Int value);

class RabinWindowInterface {
  public:
    virtual Int Extend(Byte next_symbol) = 0;
  
    // Slides one step TO the provided position.
    // New window begines with the *string byte, with its fingerprint returned.
    virtual Int Slide(Byte *string) = 0;
  
    virtual Int GetFingerprint() = 0;
    virtual Int Reset(Byte* string = 0) = 0;
    virtual bool CheckCollision(const Byte *a, const Byte *b) = 0;
    virtual ~RabinWindowInterface() {};
};

inline Int SymbolShift(Int value) {
  return (value << kSymbolBitWidth); // non-normal radix can be used
                                     // SimpleWindow only supports normal radix.
}

/* Implementing class(es) */

class NumericalWindow : public RabinWindowInterface {
  public:
    explicit NumericalWindow(int width, Byte *string = 0);

    Int Extend(Byte next_symbol);
    Int Slide(Byte *string);
    Int GetFingerprint();

    Int Reset(Byte* string = 0);

    ~NumericalWindow();
  
    bool CheckCollision(const Byte *a, const Byte *b);

  private:
    void InitWeights();
    void InitFingerprint(Byte *string);

    const int width_; // number of Bytes
    Byte *window_symbols_;
    int window_head_;
    Int *weights_;
    Int over_weight_;
    Int fingerprint_;
};

inline void NumericalWindow::InitFingerprint(Byte *string) {
  fingerprint_ = 0;
  Byte next_symbol;
  for (window_head_ = 0; window_head_ < width_; ++window_head_) {
    next_symbol = window_symbols_[window_head_] = string[window_head_];
    fingerprint_ += (next_symbol * weights_[width_ - window_head_ - 1]) 
      % kPrime;
  }
  window_head_ = width_ - 1;
  fingerprint_ %= kPrime;
  
#ifdef DEBUG_FINGERPRINT
  cerr << "Initialized fingerprint = " << fingerprint_ << endl;
#endif
}

inline Int NumericalWindow::Extend(Byte next_symbol) {
  fingerprint_ = (SymbolShift(fingerprint_) + next_symbol) % kPrime;
  
#ifdef DEBUG_FINGERPRINT
  cerr << "(+) one-step extended fingerprint = " << fingerprint_ << endl;
#endif
  
  return fingerprint_;
}

inline Int NumericalWindow::Slide(Byte *string) {
  Byte next_symbol = *(string + width_ - 1);
  ++window_head_;
  window_head_ %= width_;
  fingerprint_ += kPrime; // prevents negative integers
                          // in the following operations
  fingerprint_ = (SymbolShift(fingerprint_) + next_symbol
                  - window_symbols_[window_head_] * over_weight_) % kPrime;
  window_symbols_[window_head_] = next_symbol;
  
#ifdef DEBUG_FINGERPRINT
  cerr << "[+] one-step slid fingerprint = " << fingerprint_ << endl;
#endif
  
  return fingerprint_;
}

inline Int NumericalWindow::GetFingerprint() {
  return fingerprint_;
}

inline Int NumericalWindow::Reset(Byte* string) {
  window_head_ = width_ - 1;
  fingerprint_ = 0;
  if (string) {
    InitFingerprint(string);
    return fingerprint_;
  } else {
    memset(window_symbols_, 0, width_ * sizeof(Byte));
    return kPrime;
  }
}

inline bool NumericalWindow::CheckCollision(const Byte *a, const Byte *b) {
  return memcmp(a, b, width_ * sizeof(Byte)) != 0;
}

// Fast implementation of window semantics by integer module
class SimpleWindow : public RabinWindowInterface {
  public:
    explicit SimpleWindow(int width, Byte *string = 0);
    Int Extend(Byte next_symbol);
    Int Slide(Byte *string);
    Int GetFingerprint();
    Int Reset(Byte* string = 0);
    bool CheckCollision(const Byte *a, const Byte *b);
  
  private:
    uint64_t fingerprint_;
  
};

inline SimpleWindow::SimpleWindow(int width, Byte *string) {
  if (width != 8) {
    throw "[SimpleWindow::SimpleWindow] Invalid width (!= 8 bytes).";
  }
  if (string) {
    Slide(string);
  }
}

inline Int SimpleWindow::Extend(Byte next_symbol) {
  fingerprint_ = ((fingerprint_ << kSymbolBitWidth) + next_symbol) % kPrime;
  return (Int)fingerprint_;
}

inline Int SimpleWindow::Slide(Byte *string) {
  fingerprint_ = *((uint64_t *)string) % kPrime;
  
#ifdef DEBUG_FINGERPRINT
  cerr << std::hex << *((uint64_t *)string) << endl;
#endif
  
  return (Int)fingerprint_;
}

inline Int SimpleWindow::GetFingerprint() {
  return (Int)fingerprint_;
}

inline Int SimpleWindow::Reset(Byte *string) {
  if (string) {
    return Slide(string);
  } else {
    return kPrime; // dnotes success
  }
}

inline bool SimpleWindow::CheckCollision(const Byte *a, const Byte *b) {
  return *((uint64_t *)a) != *((uint64_t *)b);
}

#endif /* CINQUAIN_RABIN_FINGERPRINT_H_ */

