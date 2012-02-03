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
 * test.cc
 *
 *  Created on: Feb 1, 2012
 *      Author: Jinglei Ren <jinglei.ren@gmail.com>
 */

// The Rabin fingerprint lib should be in INCLUDE path.
#include "rabin_fingerprint.h"
#include <cstdlib>
#include <iostream>

using namespace std;

void FillString(int length, Byte *string) {
  long max_value = 1 << kSymbolBitWidth;
  for (int i = 0; i < length; ++i) {
    string[i] = rand() % max_value;
  }
}

void PrintString(int length, Byte *string) {
  cout << "{ ";
  for (int i = 0; i < length; ++i) {
    cout << hex << (Int)string[i] << " ";
  }
  cout << "}";
}

int main() {
  const int width = 16;
  const int length = width * 2;

  Byte string[length];
  FillString(length, string);

  RabinWindow *window = new NumericalWindow(width, string);
  for (int i = width; i < length; ++i) {
    window->Slide(string[i]);
  }

  PrintString(width, string + length - width);
  cout << "\t" << dec << window->GetFingerprint() << endl;

  delete window;
}

