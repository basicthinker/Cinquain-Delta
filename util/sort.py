#! /usr/bin/env python

"""
Sort lines of a file according to the 1st column separated by tab.
"""

import sys

if len(sys.argv) != 2:
  print "USAGE: " + sys.argv[0] + " [file-name]"
  quit()

file = open(sys.argv[1], 'r');
lines = []

try:
  for line in file:
    lines.append(line.split('\t')[0])
  lines.sort()
  for line in lines:
    print line
finally:
  file.close()
