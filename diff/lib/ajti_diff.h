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

#include "diff_output.h"
#include <cstdio>

typedef long long Int64;

class CinquainEncoder {
  public:
    CinquainEncoder(int hash_table_length);
    void Encode(char *base, char *version, DiffOutput *output);
    ~CinquainEncoder();
};

#endif // CINQUAIN_DELTA_AJTI_DIFF_H_
