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

// Global setting
const int kWidth = 16;

static void FillString(int length, Byte *string);
static void PrintString(ostream &out, int length, Byte *string);

// Part One: Test for Sliding a Window
static void TestSlide(ostream &out) {
  // Config for parameters
  const int num_cases = 10000; // The number of random strings to generate for test.
  const int length = kWidth * 2; // The length of each string, only the last window
                          // has its fingerprint value checked after sliding.
  
  Byte *input_string = new Byte[length];
  FillString(length, input_string);
  RabinWindow *window = new NumericalWindow(kWidth, input_string); // Test constructor.
  
  for (int i = kWidth; i < length; ++i) {
    window->Slide(input_string[i]);
  }
  PrintString(out, kWidth, input_string + length - kWidth);
  out << "\t" << window->GetFingerprint() << endl;
  
  for (int i = 0; i < num_cases - 1; ++i) {
    FillString(length, input_string);
    window->Reset(input_string); // Test Reset(Byte *) function
    for (int j = kWidth; j < length; ++j) {
      window->Slide(input_string[j]);
    }
    PrintString(out, kWidth, input_string + length - kWidth);
    out << "\t" << window->GetFingerprint() << endl;
  }
  
  delete input_string;
  delete window;
}

// Part Two: Test for Extending a Window
static void TestExtend(ostream &out) {
  // Config for parameters
  const int num_cases = 10000; // The number of random strings to generate for test.
  const int length = kWidth * 4; // The length of each string.
  
  Byte *input_string = new Byte[length];
  FillString(length, input_string);
  RabinWindow *window = new NumericalWindow(kWidth, input_string); // Test constructor.
  
  for (int i = kWidth; i < length; ++i) {
    window->Extend(input_string[i]);
  }
  PrintString(out, length, input_string);
  out << "\t" << window->GetFingerprint() << endl;
  
  for (int i = 0; i < num_cases - 1; ++i) {
    FillString(length, input_string);
    window->Reset(input_string); // Test Reset(Byte *) function
    for (int j = kWidth; j < length; ++j) {
      window->Extend(input_string[j]);
    }
    PrintString(out, length, input_string);
    out << "\t" << window->GetFingerprint() << endl;
  }
  
  delete input_string;
  delete window;
}

// Part Three: Test for Collision
static void TestCollision(ostream &out) {
  // Config for parameters
  const int expected_collision_count = 100;
  const int num_cases = kPrime * expected_collision_count; 
    // The number of window values to generate for test.
  
  int *hash_counts = new int[kPrime];
  memset(hash_counts, 0, kPrime * sizeof(int));
  
  int length = num_cases + kWidth - 1; // NOT configurable
  Byte *input_string = new Byte[num_cases + kWidth - 1];
  FillString(length, input_string);
  
  RabinWindow *window = new NumericalWindow(kWidth, input_string);
  ++hash_counts[window->GetFingerprint()];
  
  for (int i = kWidth; i < length; ++i) {
    window->Slide(input_string[i]);
    ++hash_counts[window->GetFingerprint()];
  }
  
  sort(hash_counts, hash_counts + kPrime);
  
  out << "Expected collision count of a position: "
    << expected_collision_count << endl;
  out << "All unexpected collision counts:" << endl;
  
  int case_count = 0;
  int position_count = 0;
  for (int i = 0; i < kPrime; ++i) {
    case_count += hash_counts[i];
    if (hash_counts[i] != expected_collision_count) {
      ++position_count;
      out << "\t" << hash_counts[i];
    }
  }
  out << endl;
  
  out << "For " << case_count << " vlues," << endl;
  out << "there are " << position_count
    << " positions among " << kPrime << endl;
  out << "meeting unexpected numbers of collisions" << endl;
  out << "in range [" << hash_counts[0] << ", "
    << hash_counts[kPrime - 1] << "]." << endl;

  delete window;
  delete hash_counts;
  delete input_string;
}

int main(int argc, const char * argv[]) {  
  ofstream output_file;
  output_file.open("fingerprints.log");
  
  output_file << hex;
  output_file << kSymbolBitWidth << " bits per symbol" << endl;
  output_file << kPrime << " as the prime" << endl;
  
  TestExtend(output_file);
  TestSlide(output_file);
  
  TestCollision(cout);

  output_file.close();
  
  return 0;
}

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
