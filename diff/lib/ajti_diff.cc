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
//  ajti_diff.cc
//  Cinquain-Delta
//
//  Created by Jinglei Ren <jinglei.ren@gmail.com> on 2/4/12.
//

#include "ajti_diff.h"
#include <memory.h>
#include <algorithm>

using std::min;
using std::max;

CinquainEncoder::CinquainEncoder(int hashtable_length, int seed_length) 
    : seed_length_(seed_length) {
  hashtable_r_ = new IntHashtable(kPrime - 1);
  hashtable_v_ = new IntHashtable(kPrime - 1);
  window_r_ = new NumericalWindow(seed_length_);
  window_v_ = new NumericalWindow(seed_length_);
  current_r_ = 0L;
  current_v_ = 0L;
  suffix_v_ = 0L;
}

CinquainEncoder::~CinquainEncoder() {
  delete hashtable_r_;
  delete hashtable_v_;
  delete window_r_;
  delete window_v_;
}

void CinquainEncoder::Reset() {
  hashtable_r_->Reset();
  hashtable_v_->Reset();
  window_r_->Reset();
  window_v_->Reset();
  current_r_ = 0L;
  current_v_ = 0L;
  suffix_v_ = 0L;
}

void CinquainEncoder::Encode(Byte *string_r, const long length_r, 
                             Byte *string_v, const long length_v,
                             DiffOutput *output) {
  if (length_r < seed_length_ || length_v < seed_length_) return;
  
  // Following comments denote step numbers in the original paper
  Reset(); // (1)(2)
  
  uint64_t match_v; // v_m
  uint64_t match_r; // r_m
  long match_length; // l
  
  uint64_t fingerprint_v = 0;
  uint64_t fingerprint_r = 0;
  bool to_clear_window_v = true;
  bool to_clear_window_r = true;
  while ((current_v_ + seed_length_ <= length_v) 
         || (current_r_ + seed_length_ <= length_r)) { // (3)
    
    if (current_v_ + seed_length_ <= length_v) {
      fingerprint_v = to_clear_window_v ?
                      window_v_->Reset(string_v + current_v_)
                    : window_v_->Slide(string_v[current_v_ + seed_length_]);
    }
    
    if (current_r_ + seed_length_ <= length_r) {
      fingerprint_r = to_clear_window_r ?
                      window_r_->Reset(string_r + current_r_)
                    : window_r_->Slide(string_r[current_r_ + seed_length_]);
    }
    
    hashtable_v_->SetValue(fingerprint_v, current_v_); // (4.a)
    hashtable_r_->SetValue(fingerprint_r, current_r_); // (4.a)
    
    if (hashtable_v_->HasValue(fingerprint_r)) { // (4.b)
      match_r = current_r_;
      match_v = hashtable_v_->GetValue(fingerprint_r);
    } else if (hashtable_r_->HasValue(fingerprint_v)) {
      match_r = hashtable_r_->GetValue(fingerprint_v);
      match_v = current_v_;
    }
    
    Byte *position_r = string_r + match_r;
    Byte *position_v = string_v + match_v;
    if (memcmp(position_r, position_v,
               seed_length_ * sizeof(Byte)) == 0) {
      // Step (5)
      // Extend forwards
      long forward_length = seed_length_;
      long limit = min(length_r - match_r, length_v - match_v);
      while (forward_length < limit && 
             *(position_r + forward_length) == *(position_v + forward_length)) {
        ++forward_length;
      }
      // Extend backwards
      long backward_length = 0;
      limit = min(match_r, match_v);
      while (backward_length < limit &&
             *(position_r - backward_length - 1) == *(position_v - backward_length - 1)) {
        ++backward_length;
      }
      match_r -= backward_length;
      match_v -= backward_length;
      match_length = backward_length + forward_length;
      
      if (suffix_v_ <= match_v) { // (6)
        output->Apend(ADD, suffix_v_, match_v);
        output->Apend(COPY, match_v, match_v + match_length);
      } else {
        output->Correct(match_v, match_v + match_length);
      }
      suffix_v_ = match_v + match_length;
      
      if ((match_v += match_length) <= ++current_v_) {
        to_clear_window_v = false;
      } else {
        current_v_ = match_v;
        to_clear_window_v = true;
      }
      
      if ((match_r += match_length) <= ++current_r_) {
        to_clear_window_r = false;
      } else {
        current_r_ = match_r;
        to_clear_window_r = true;
      }
    } else { // (4.b)
      ++current_v_;
      ++current_r_;
      to_clear_window_v = false;
      to_clear_window_r = false;
    }
    
    // Step (8)
    if (suffix_v_ < length_v) {
      output->Apend(ADD, suffix_v_, length_v);
    }
    output->Flush();
  }
}

