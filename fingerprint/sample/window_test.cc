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
 * window_test.cc
 *
 *  Created on: Feb 1, 2012
 *      Author: Jinglei Ren <jinglei.ren@gmail.com>
 */

// The Rabin fingerprint lib should be in INCLUDE path.
#include "rabin_fingerprint.h"
#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

static void FillString(int length, Byte *string) {
  long max_value = 1 << kSymbolBitWidth;
  for (int i = 0; i < length; ++i) {
    string[i] = rand() % max_value;
  }
}

static void PrintString(ostream &out, int length, Byte *string) {
  out << length << " { ";
  for (int i = 0; i < length; ++i) {
    out << (Int)string[i] << " ";
  }
  out << "}";
}

int main() {
  const int width = 16;
  
  ofstream file;
  file.open("fingerprints.log");
  
  file << hex;
  file << kSymbolBitWidth << " bits per symbol" << endl;
  file << kPrime << " as the prime" << endl;
  
  /* Part One: Test for Sliding a Window */
  
  int length = width * 2;
  Byte *string = new Byte[length];
  FillString(length, string);
  RabinWindow *window = new NumericalWindow(width, string); // Test constructor.
  
  for (int i = width; i < length; ++i) {
    window->Slide(string[i]);
  }
  PrintString(file, width, string + length - width);
  file << "\t" << window->GetFingerprint() << endl;
  
  for (int i = 0; i < 9999; ++i) {
    FillString(length, string);
    window->Reset(string); // Test Reset(Byte *) function
    for (int j = width; j < length; ++j) {
      window->Slide(string[j]);
    }
    PrintString(file, width, string + length - width);
    file << "\t" << window->GetFingerprint() << endl;
  }

  delete string;
  delete window;
  
  /* Part Two: Test for Extending a Windows */
  
  length = width * 4;
  string = new Byte[length];
  FillString(length, string);
  window = new NumericalWindow(width, string); // Test constructor.
  
  for (int i = width; i < length; ++i) {
    window->Extend(string[i]);
  }
  PrintString(file, length, string);
  file << "\t" << window->GetFingerprint() << endl;
  
  for (int i = 0; i < 9999; ++i) {
    FillString(length, string);
    window->Reset(string); // Test Reset(Byte *) function
    for (int j = width; j < length; ++j) {
      window->Extend(string[j]);
    }
    PrintString(file, length, string);
    file << "\t" << window->GetFingerprint() << endl;
  }
  
  delete string;
  delete window;

  file.close();


}

