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
//  hashtable.h
//  Cinquain-Delta
//
//  Created by Jinglei Ren <jinglei.ren@gamil.com> on 2/8/12.
//

#ifndef CINQUAIN_DELTA_INT_HASHTABLE_H_
#define CINQUAIN_DELTA_INT_HASHTABLE_H_

typedef unsigned long long uint64_t;

class IntHashtable {
  public:
    IntHashtable(uint64_t max_key);
    ~IntHashtable();
    void Reset();
  
    void SetValue(uint64_t key, uint64_t value);
    bool HasValue(uint64_t key);
    uint64_t GetValue(uint64_t key);
  
  private:
    const uint64_t num_buckets_;
    uint64_t * buckets_;
    
    static const uint64_t kNull = 0xffffffffffffffff;
};

inline IntHashtable::IntHashtable(uint64_t max_key)
    : num_buckets_(max_key + 1) {
  buckets_ = new uint64_t[num_buckets_];
  Reset();
}

inline IntHashtable::~IntHashtable(){
  delete buckets_;
}

inline void IntHashtable::Reset() {
  memset(buckets_, 0xff, sizeof(uint64_t) * num_buckets_);
}

inline void IntHashtable::SetValue(uint64_t key, uint64_t value) {
  buckets_[key] = value;
}

inline bool IntHashtable::HasValue(uint64_t key) {
  return buckets_[key] != kNull;
}

inline uint64_t IntHashtable::GetValue(uint64_t key) {
  return buckets_[key];
}

#endif // CINQUAIN_DELTA_HASHTABLE_H_
