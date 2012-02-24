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

#include "diff_config.h"
#include "ajti_diff.h"
#include "delta_decoder.h"


int main (int argc, const char * argv[])
{
  if (argc < 3) {
    printf("\nUsage: ./delta ReferenceFileName VersionFileName\n\n");
    return -1;
  }
  
  const int file_r = open(argv[1], O_RDONLY);
  const int file_v = open(argv[2], O_RDONLY);
  
  if (file_r == -1 || file_v == -1) {
    close(file_r);
    close(file_v);
    printf("Error: failed to open specified files.\n");
    return -1;
  }
  
  struct stat file_stat_r;
  struct stat file_stat_v;
  
  if (fstat(file_r, &file_stat_r) == -1 || fstat(file_v, &file_stat_v) == -1) {
    close(file_r);
    close(file_v);
    printf("Error: failed to get file sizes.\n");
    return -1;
  }
  
  char *memory_r = (char *)mmap(NULL, file_stat_r.st_size,
                                            PROT_READ, MAP_PRIVATE, file_r, 0);
  char *memory_v = (char *)mmap(NULL, file_stat_v.st_size,
                                            PROT_READ, MAP_PRIVATE, file_v, 0);
  
  if (memory_r == MAP_FAILED || memory_v == MAP_FAILED) {
    munmap(memory_r, file_stat_r.st_size);
    munmap(memory_v, file_stat_v.st_size);
    close(file_r);
    close(file_v);
    printf("Error: failed to map files to memory.\n");
    return -1;
  }
  
  CinquainEncoder encoder(kPrime, kSeedLength);
  char *memory_d;
  InMemoryOutput output(memory_r, memory_v, kInitNumInstructions, memory_d);
  encoder.Encode((Byte *)memory_r, (offset_t)file_stat_r.st_size,
                 (Byte *)memory_v, (offset_t)file_stat_v.st_size, output);

  // Print delta file
  printf("Delta File (%d):\n", output.GetDeltaSize());
  offset_t *offset_ptr = (offset_t *)memory_d;
  printf("end_instruction = %d\n", *offset_ptr);
  DeltaInstruction *instruction_ptr = (DeltaInstruction *)(memory_d + sizeof(offset_t));
  while ((char *)instruction_ptr - memory_d <= *offset_ptr) {
    switch (instruction_ptr->type()) {
      case ADD:
        printf("[%d] ADD [%d]\n",
               instruction_ptr->offset(), instruction_ptr->attribute());
        break;
      case COPY:
        printf("[%d] COPY [%d]\n",
               instruction_ptr->offset(), instruction_ptr->attribute());
        break;
      default:
        printf("[%d]\n", instruction_ptr->offset());
        break;
    }
    ++instruction_ptr;
  }
  
  char *memory_check;
  CinquainDecoder decoder(memory_check);
  decoder.Decode(memory_r, memory_d);
  
  // Print restored version file
  printf("Restored Version (%d):\n", decoder.GetVersionSize());
  for (char *symbol_ptr = memory_check;
       symbol_ptr - memory_check < decoder.GetVersionSize(); ++symbol_ptr) {
    printf("%c", *symbol_ptr);
  }
  printf("\n");
  
  if (file_stat_v.st_size != decoder.GetVersionSize()) {
    printf("Wrong: version file sizes do not match.\n");
  } else if (memcmp(memory_v, memory_check, decoder.GetVersionSize()) != 0) {
    printf("Wrong: restored version file is not identical to the original.\n");
  } else {
    printf("Encoding/Decoding: OK.\n");
  }

  munmap(memory_r, file_stat_r.st_size);
  munmap(memory_v, file_stat_v.st_size);
  close(file_r);
  close(file_v);
  
  return 0;
}
