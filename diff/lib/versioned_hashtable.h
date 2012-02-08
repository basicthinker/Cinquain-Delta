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
//  versioned_hashtable.h
//  Cinquain-Delta
//  
//  This class is intended for the one-pass encoding algorithm
//  as in the original paper. However, the encapsulating library
//  now chooses the correcting one-pass algorithm that does NOT 
//  require a versioned hashtable. NOTE that this class has NOT
//  undergone full testing.
//
//  THIS FILE CAN BE EXCLUDED FROM THE LIBRARY IF YOU WISH.
//
//  Created by Jinglei Ren <jinglei.ren@gmail.com> on 2/5/12.
//

#ifndef CINQUAIN_DELTA_VERSIONED_HASHTABLE_H_
#define CINQUAIN_DELTA_VERSIONED_HASHTABLE_H_

typedef long long Int64;

#include <memory.h>

class VersionedHashtable {
  public:
    explicit VersionedHashtable(int num_buckets);
  
    Int64 current_version();
    Int64 NewVersion();
  
    bool HasValue(Int64 key, Int64 version);
    bool HasValue(Int64 key);
  
    Int64 GetVersion(Int64 key);
  
    bool GetValue(Int64 key, Int64 version, Int64 &value);
    bool GetValue(Int64 key, Int64 &value) {
      return GetValue(key, current_version_, value);
    }
  
    void SetValue(Int64 key, Int64 value, Int64 version);
    void SetValue(Int64 key, Int64 value) {
      SetValue(key, value, current_version_);
    }
  
    bool TrySetValue(Int64 key, Int64 value, Int64 version);
    bool TrySetValue(Int64 key, Int64 value) {
      return TrySetValue(key, value, current_version_);
    }
  
    Int64 GetEntry(Int64 key);
    void SetEntry(Int64 key, Int64 entry);
    
    void Reset();
    ~VersionedHashtable();
  
    static const Int64 kVersionPlus = 0x0001000000000000;
    static const Int64 kVersionMask = 0xffff000000000000;
    static const Int64 kValueMask = 0x0000ffffffffffff;
  
  private:
    Int64 *hashtable_;
    const int num_buckets_;
    Int64 current_version_;
};


inline VersionedHashtable::VersionedHashtable(int num_buckets)
    : num_buckets_(num_buckets),
  current_version_(kVersionPlus) {
  hashtable_ = new Int64[num_buckets_];
  memset(hashtable_, 0, sizeof(Int64) * num_buckets_);
}

inline VersionedHashtable::~VersionedHashtable() {
  delete hashtable_;
}

inline void VersionedHashtable::Reset() {
  current_version_ = kVersionPlus;
  memset(hashtable_, 0, sizeof(Int64) * num_buckets_);
}

inline Int64 VersionedHashtable::current_version() {
  return current_version_;
}

inline Int64 VersionedHashtable::NewVersion() {
  current_version_ += kVersionPlus;
  return current_version_;
}

inline bool VersionedHashtable::HasValue(Int64 key, Int64 version) {
  return (hashtable_[key] & kVersionMask) == version;
}

inline bool VersionedHashtable::HasValue(Int64 key) {
  return HasValue(key, current_version_);
}

inline Int64 VersionedHashtable::GetVersion(Int64 key) {
  return hashtable_[key] & kVersionMask;
}

inline bool VersionedHashtable::GetValue(Int64 key, Int64 version, Int64 &value) {
  Int64 entry = hashtable_[key];
  if ((entry & kVersionMask) != version) return false;
  value = entry & kValueMask;
  return true;
}

inline void VersionedHashtable::SetValue(Int64 key, Int64 value, Int64 version) {
  hashtable_[key] = value | version;
}

inline bool VersionedHashtable::TrySetValue(Int64 key, Int64 value, Int64 version) {
  if (HasValue(key, version)) return false;
  SetValue(key, value, version);
  return true;
}

inline Int64 VersionedHashtable::GetEntry(Int64 key) {
  return hashtable_[key];
}

inline void VersionedHashtable::SetEntry(Int64 key, Int64 entry) {
  hashtable_[key] = entry;
}

#endif // CINQUAIN_DELTA_VERSIONED_HASHTABLE_H_
