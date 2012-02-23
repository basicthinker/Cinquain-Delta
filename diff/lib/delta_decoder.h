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
//  delta_decoder.h
//  Cinquain-Delta
//
//  Created by Jinglei Ren <jinglei.ren@gmail.com> on 2/22/12.
//

#ifndef CINQUAIN_DELTA_DECODER_H_
#define CINQUAIN_DELTA_DECODER_H_

#include <string.h>
#include "delta_instruction.h"

class CinquainDecoder {
  public:
    CinquainDecoder(char *&output);
    void Decode(const char *reference, const char *delta);
    offset_t GetVersionSize();
    ~CinquainDecoder();
  
  private:
    char *&output_;
    offset_t version_size_;
};

inline CinquainDecoder::CinquainDecoder(char *&output)
    : output_(output) {
  output_ = 0;
  version_size_ = 0;
}

inline offset_t CinquainDecoder::GetVersionSize() {
  return version_size_;
}

inline CinquainDecoder::~CinquainDecoder() {
  if (output_) {
    delete[] output_;
  }
}

#endif // CINQUAIN_DELTA_DECODER_H_
