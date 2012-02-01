/*
 *
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
 * rabin.h
 *
 *  Created on: Feb 1, 2012
 *      Author: Jinglei Ren <jinglei.ren@gmail.com>
 */

#ifndef CINQUAIN_RABIN_FINGERPRINT_H_
#define CINQUAIN_RABIN_FINGERPRINT_H_

#include "rabin_config.h"

class Window {
  public:
    virtual Int Extend(Byte next_symbol) = 0;
    virtual Int Slide(Byte next_symbol) = 0;
    virtual ~Window() {};
};

#endif /* CINQUAIN_RABIN_FINGERPRINT_H_ */

