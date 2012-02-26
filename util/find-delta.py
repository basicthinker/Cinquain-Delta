#! /usr/bin/env python

import sys

if len(sys.argv) != 3:
  print "\nUSAGE: " + sys.argv[0] + " file-list-1 file-list-2"
  print "Notice: either file list should be sorted and have two columns separated by tab.\n"
  sys.exit(0)

name1 = sys.argv[1]
name2 = sys.argv[2]

file1 = open(name1, 'r')
file2 = open(name2, 'r')

try:
  line1 = file1.readline().split('\t')
  line2 = file2.readline().split('\t')
  while len(line1[0]) > 0 and len(line2[0]) > 0:
    if line1[0] == line2[0]:
      if line1[1] != line2[1]:
        print line1[0]
      line1 = file1.readline().split('\t')
      line2 = file2.readline().split('\t')
      continue
    elif line1[0] < line2[0]:
      line1 = file1.readline().split('\t')
    else:
      line2 = file2.readline().split('\t')

finally:
  file1.close()
  file2.close()
