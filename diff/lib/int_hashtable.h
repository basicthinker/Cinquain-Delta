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

template <typename ValueType>
class IntHashtable {
  public:
    IntHashtable(unsigned int max_key);
    ~IntHashtable();
    void Reset();
  
    void SetValue(unsigned int key, const ValueType &value);
    bool HasValue(unsigned int key);
    const ValueType &GetValue(unsigned int key);
  
  private:
    const unsigned int num_buckets_;
    unsigned int * buckets_;
    
    static const unsigned int kNull = 0xffffffff;
};

template <typename ValueType>
inline IntHashtable<ValueType>::IntHashtable(unsigned int max_key)
    : num_buckets_(max_key + 1) {
  buckets_ = new ValueType[num_buckets_];
  Reset();
}

template <typename ValueType>
inline IntHashtable<ValueType>::~IntHashtable(){
  delete buckets_;
}

template <typename ValueType>
inline void IntHashtable<ValueType>::Reset() {
  memset(buckets_, 0xff, sizeof(unsigned int) * num_buckets_);
}

template <typename ValueType>
inline void IntHashtable<ValueType>::SetValue(unsigned int key, const ValueType &value) {
  buckets_[key] = value;
}

template <typename ValueType>
inline bool IntHashtable<ValueType>::HasValue(unsigned int key) {
  return buckets_[key] != kNull;
}

template <typename ValueType>
inline const ValueType &IntHashtable<ValueType>::GetValue(unsigned int key) {
  return buckets_[key];
}

#endif // CINQUAIN_DELTA_HASHTABLE_H_
