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
//  ajti_diff.h
//  Cinquain-Delta
//
//  Created by Jinglei Ren <jinglei.ren@gmail.com> on 2/4/12.
//

#ifndef CINQUAIN_DELTA_AJTI_DIFF_H_
#define CINQUAIN_DELTA_AJTI_DIFF_H_

#include "rabin_fingerprint.h"
#include "int_hashtable.h"
#include "diff_output.h"

class CinquainEncoder {
  public:
    CinquainEncoder(int hashtable_length, int seed_length);
    void Encode(Byte *reference, const long length_r,
                Byte *version, const long length_v,
                DiffOutput *output);
    ~CinquainEncoder();
  
  private:
    void Reset();

    // Following comments denote symbols in the original paper
    const int seed_length_; // p
    IntHashtable *hashtable_r_; // H_R
    IntHashtable *hashtable_v_; // H_V
    RabinWindow *window_r_; // F_R
    RabinWindow *window_v_; // F_V
    long current_r_; // r_c
    long current_v_; // v_c
    long suffix_v_; // v_s
};

#endif // CINQUAIN_DELTA_AJTI_DIFF_H_
