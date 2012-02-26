#! /usr/bin/env python

import sys
import shutil

if len(sys.argv) != 4:
  print "USAGE: " + sys.argv[0] + " delta-list target-dir postfix"
  quit()

file = open(sys.argv[1], 'r')
dir = sys.argv[2]
post = sys.argv[3]

try:
  for src in file:
    src = src[:-1]
    dst = dir + '/' + src.replace('/', '-')[1:] + '-' + post
    shutil.copyfile(src, dst);
finally:
  file.close()
