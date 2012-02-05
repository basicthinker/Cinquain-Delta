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
#include <memory.h>

#ifdef DEBUG_FINGERPRINT
#include <iostream>
using std::cerr;
using std::endl;
#endif

NumericalWindow::NumericalWindow(int width, Byte *string)
    : width_(width),
      window_head_(width - 1),
      fingerprint_(0) {
  weights_ = new Int[width_];
  InitWeights();
  window_symbols_ = new Byte[width_];
  if (string) {
    InitFingerprint(string);
  } else {
    memset(window_symbols_, 0, width_ * sizeof(Byte));
  }
}

NumericalWindow::~NumericalWindow() {
  delete window_symbols_;
}

void NumericalWindow::InitWeights() {
  weights_[0] = 1;
  for (int i = 1; i < width_; ++i) {
    weights_[i] = SymbolShift(weights_[i - 1]) % kPrime;
  }
  over_weight_ = SymbolShift(weights_[width_ - 1]) % kPrime;

#ifdef DEBUG_FINGERPRINT
  cerr << "Initialized weights:" << endl;
  for (int i = 0; i < width_; ++i) {
    cerr << " [" << i << "] = " << weights_[i];
  }
  cerr << " [" << width_ << "] = " << over_weight_;
  cerr << endl;
#endif
}


