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

typedef NumericalWindow RabinWindow; // one of the above

#ifdef DEBUG_FINGERPRINT
#include <iostream>
using std::cerr;
using std::endl;
#endif

class RabinWindowInterface {
  public:
    virtual Int Extend(Byte next_symbol) = 0;
    // Slides one step from the provided position
    virtual Int Slide(Byte *string) = 0;
    virtual Int GetFingerprint() = 0;
    virtual Int Reset(Byte* string = 0) = 0;
    virtual ~RabinWindowInterface() {};
};

/* Implementing class(es) */

class NumericalWindow : public RabinWindowInterface {
  public:
    explicit NumericalWindow(int width, Byte *string = 0);

    Int Extend(Byte next_symbol);
    Int Slide(Byte *string);
    Int GetFingerprint();

    Int Reset(Byte* string = 0);

    ~NumericalWindow();
  
    static Int SymbolShift(Int value);

  private:
    void InitWeights();
    void InitFingerprint(Byte *string);

    const int width_;
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

inline Int NumericalWindow::SymbolShift(Int value) {
  return (value << kSymbolBitWidth) + value;
}

inline Int NumericalWindow::Extend(Byte next_symbol) {
  fingerprint_ = (SymbolShift(fingerprint_) + next_symbol) % kPrime;
  
#ifdef DEBUG_FINGERPRINT
  cerr << "(+) one-step extended fingerprint = " << fingerprint_ << endl;
#endif
  
  return fingerprint_;
}

inline Int NumericalWindow::Slide(Byte *string) {
  Byte next_symbol = *(string + width_);
  ++window_head_;
  window_head_ %= width_;
  fingerprint_ += kPrime; // Prevent negative integers
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
  } else {
    memset(window_symbols_, 0, width_ * sizeof(Byte));
  }
  return fingerprint_;
}

#endif /* CINQUAIN_RABIN_FINGERPRINT_H_ */

