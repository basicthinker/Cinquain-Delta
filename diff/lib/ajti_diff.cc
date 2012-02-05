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

CinquainEncoder::CinquainEncoder(int hashtable_length, int seed_length) 
    : hashtable_length_(hashtable_length),
      seed_length_(seed_length) {
  hashtable_r_ = new VersionedHashtable(hashtable_length_);
  hashtable_v_ = new VersionedHashtable(hashtable_length_);
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

void CinquainEncoder::Reset(Byte *reference, Byte *version) {
  hashtable_r_->Reset();
  hashtable_v_->Reset();
  window_r_->Reset(reference);
  window_v_->Reset(version);
  current_r_ = 0L;
  current_v_ = 0L;
  suffix_v_ = 0L;
}

void CinquainEncoder::Encode(Byte *string_r, long length_r, 
                             Byte *string_v, long length_v,
                             DiffOutput *output) {
  if (length_r < seed_length_ || length_v < seed_length_) return;
  
  // Following comments denote step numbers in the original paper
  Reset(string_r, string_v); // (1)(2)
  
  Int64 match_offset_r; // r_m
  Int64 match_offset_v; // v_m
  
  Int64 fingerprint_r = window_r_->GetFingerprint(); // (3)
  Int64 fingerprint_v = window_r_->GetFingerprint(); // (3)
  
  while ((current_r_ + seed_length_ < length_r) 
         && (current_v_ + seed_length_ < length_v)) {
    hashtable_r_->SetValue(fingerprint_r, current_r_); // (4.a)
    hashtable_v_->SetValue(fingerprint_v, current_v_); // (4.a)
    
    if (hashtable_v_->HasValue(fingerprint_r)) { // (4.b)
      match_offset_r = current_r_;
      hashtable_v_->GetValue(fingerprint_r, match_offset_v);
    }
    
    if (hashtable_r_->HasValue(fingerprint_v)) { // (4.b)
      hashtable_r_->GetValue(fingerprint_v, match_offset_r);
      match_offset_v = current_v_;
    }
    
    Byte *position_r = string_r + match_offset_r;
    Byte *position_v = string_v + match_offset_v;
    if (memcmp(position_r, position_v,
               seed_length_ * sizeof(Byte)) == 0) { // (5)
      
    } else { // (4.b)
      
    }
  }
}

