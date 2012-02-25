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
//  diff_test.cpp
//  Cinquain-Delta
//
//  Created by Jinglei Ren <jinglei.ren@gmail.com> on 2/5/12.
//

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "diff_config.h"
#include "ajti_diff.h"
#include "delta_decoder.h"

// #define DEBUG_DIFF

int main (int argc, const char * argv[])
{
  if (argc < 3) {
    fprintf(stderr, "\nUsage: %s ReferenceFileName VersionFileName\n\n", argv[0]);
    return -1;
  }
  
  const int file_r = open(argv[1], O_RDONLY);
  const int file_v = open(argv[2], O_RDONLY);
  
  if (file_r == -1 || file_v == -1) {
    close(file_r);
    close(file_v);
    fprintf(stderr, "Error: failed to open specified files.\n");
    return -1;
  }
  
  struct stat file_stat_r;
  struct stat file_stat_v;
  
  if (fstat(file_r, &file_stat_r) == -1 || fstat(file_v, &file_stat_v) == -1) {
    close(file_r);
    close(file_v);
    fprintf(stderr, "Error: failed to get file sizes.\n");
    return -1;
  }
  
  // Timing begins.
  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  
  char *memory_r = (char *)mmap(NULL, file_stat_r.st_size,
                                            PROT_READ, MAP_PRIVATE, file_r, 0);
  char *memory_v = (char *)mmap(NULL, file_stat_v.st_size,
                                            PROT_READ, MAP_PRIVATE, file_v, 0);
  
  if (memory_r == MAP_FAILED || memory_v == MAP_FAILED) {
    munmap(memory_r, file_stat_r.st_size);
    munmap(memory_v, file_stat_v.st_size);
    close(file_r);
    close(file_v);
    fprintf(stderr, "Error: failed to map files to memory.\n");
    return -1;
  }
  
  CinquainEncoder encoder(kPrime, kSeedLength);
  char *memory_d;
  InMemoryOutput output(memory_r, memory_v, kInitNumInstructions, memory_d);
  encoder.Encode((Byte *)memory_r, (offset_t)file_stat_r.st_size,
                 (Byte *)memory_v, (offset_t)file_stat_v.st_size, output);
  
  // Timing ends.
  struct timeval end_time;
  gettimeofday(&end_time, NULL);
  double sec = end_time.tv_sec - start_time.tv_sec;
  double usec = (double)end_time.tv_usec - (double)start_time.tv_usec;
  fprintf(stdout, "%lld\t%d\t%lf",
          file_stat_v.st_size, output.GetDeltaSize(), sec + usec/1000000);
  
  char *memory_check;
  CinquainDecoder decoder(memory_check);
  decoder.Decode(memory_r, memory_d);
  
#ifdef DEBUG_DIFF
  // Print delta file
  fprintf(stdout, "Delta File (%d):\n", output.GetDeltaSize());
  offset_t *offset_ptr = (offset_t *)memory_d;
  fprintf(stdout, "end_instruction = %d\n", *offset_ptr);
  DeltaInstruction *instruction_ptr = (DeltaInstruction *)(memory_d + sizeof(offset_t));
  while ((char *)instruction_ptr - memory_d <= *offset_ptr) {
    switch (instruction_ptr->type()) {
      case ADD:
        fprintf(stdout, "[%d] ADD [%d]\n",
               instruction_ptr->offset(), instruction_ptr->attribute());
        break;
      case COPY:
        fprintf(stdout, "[%d] COPY [%d]\n",
               instruction_ptr->offset(), instruction_ptr->attribute());
        break;
      default:
        fprintf(stdout, "[%d]\n", instruction_ptr->offset());
        break;
    }
    ++instruction_ptr;
  }
#endif
  
  if (file_stat_v.st_size != decoder.GetVersionSize()) {
    fprintf(stderr, "Wrong: version file sizes do not match.\n");
  } else if (memcmp(memory_v, memory_check, decoder.GetVersionSize()) != 0) {
    fprintf(stderr, "Wrong: restored version file is not identical to the original.\n");
  } else {
    fprintf(stdout, "\tOK\n");
  }

  munmap(memory_r, file_stat_r.st_size);
  munmap(memory_v, file_stat_v.st_size);
  close(file_r);
  close(file_v);
  
  return 0;
}
