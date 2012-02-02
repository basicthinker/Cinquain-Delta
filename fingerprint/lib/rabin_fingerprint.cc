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
 * rabin_fingerprint.cc
 *
 *  Created on: Feb 1, 2012
 *      Author: Jinglei Ren <jinglei.ren@gmail.com>
 */

#include "rabin_fingerprint.h"

#ifdef DEBUG_FINGERPRINT
#include <iostream>
using std::cerr;
using std::endl;
#endif

inline NumericalWindow::NumericalWindow(int width, Byte *string)
    : width_(width), window_head_(width - 1), fingerprint_(0) {
  window_symbols_ = new Byte[width_];
  weights_ = new Int[width_];
  InitWeights();
  if (string) {
    InitFingerprint(string);
  }
}

inline NumericalWindow::~NumericalWindow() {
  delete window_symbols_;
}

inline void NumericalWindow::InitWeights() {
  weights_[0] = 1;
  for (int i = 1; i < width_; ++i) {
    weights_[i] = (weights_[i - 1] << kSymbolBitWidth) % kPrime;
  }
  over_weight_ = (weights_[width_ - 1] << kSymbolBitWidth) % kPrime;

#ifdef DEBUG_FINGERPRINT
  cerr << "Initialized weights:" << endl;
  for (int i = 0; i < width_; ++i) {
    cerr << "\t[" << i << "] = " << weights_[i];
  }
  cerr << "\t[" << width_ << "] = " << over_weight_;
  cerr << endl;
#endif
}

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
  fingerprint_ = ((fingerprint_ << kSymbolBitWidth) + next_symbol) % kPrime;

#ifdef DEBUG_FINGERPRINT
  cerr << "(+) one-step extended fingerprint = " << fingerprint_ << endl;
#endif

  return fingerprint_;
}

inline Int NumericalWindow::Slide(Byte next_symbol) {
  ++window_head_;
  window_head_ %= width_;
  fingerprint_ = ((fingerprint_ << kSymbolBitWidth) + next_symbol
      - window_symbols_[window_head_] * over_weight_) % kPrime;
  window_symbols_[window_head_] = next_symbol;

#ifdef DEBUG_FINGERPRINT
  cerr << "[+] one-step slid fingerprint = " << fingerprint_ << endl;
#endif

  return fingerprint_;
}

inline void NumericalWindow::Reset() {
  window_head_ = width_ - 1;
  fingerprint_ = 0;
}

inline void NumericalWindow::Reset(Byte* string) {
  Reset();
  InitFingerprint(string);
}

