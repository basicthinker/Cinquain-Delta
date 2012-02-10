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
//  diff_output.h
//  Cinquain-Delta
//
//  Created by Jinglei Ren <jinglei.ren@gmail.com> on 2/4/12.
//

#ifndef CINQUAIN_DELTA_DIFF_OUTPUT_H_
#define CINQUAIN_DELTA_DIFF_OUTPUT_H_

typedef unsigned int offset_t;

class DiffOutputInterface;

typedef DiffOutputInterface DiffOutput;

enum DiffInstruction {
  ADD,
  COPY
};

class DiffOutputInterface {
  public:
    virtual void Append(DiffInstruction instruction,
                       const offset_t begin, const offset_t end) = 0;
    virtual offset_t Correct(const offset_t begin, const offset_t end) = 0;
    virtual void Flush() = 0;
    virtual ~DiffOutputInterface() {}
};


/* Implementing classes */

class InMemoryOutput : public DiffOutputInterface {
  public:
    InMemoryOutput(const int init_size);
    void Append(DiffInstruction instruction, const offset_t begin, const offset_t end);
    offset_t Correct(const offset_t begin, const offset_t end);
    void Flush();
    ~InMemoryOutput();
  
  private:
    typedef struct {
      offset_t offset_v;
      offset_t compound;
    } Instruction;
};

#endif // CINQUAIN_DELTA_DIFF_OUTPUT_H_
