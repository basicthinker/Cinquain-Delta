#! /usr/bin/env python

"""
Check a sorted file whose each line has a file name and MD5 value.

According to the file name by ASCII ascending order.
"""

import sys

if len(sys.argv) != 2:
  print "\nUSAGE: " + sys.argv[0] + " [file-name]"
  print "NOTICE: the file should have two columns separated by tab.\n"
  quit()

name = sys.argv[1]

file = open(name, 'r')
count = 0
try:
  line1 = "".split('\t')
  line2 = file.readline().split('\t')
  while len(line2[0]) > 0:
    if line1[0] >= line2[0]:
      print line1[0]
      print line2[0]
      print
      count += 1
    line1 = line2
    line2 = file.readline().split('\t')
  print count
finally:
  file.close()
