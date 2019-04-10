#!/usr/bin/env python

"""
  See MultiRunBase.py for description
"""

from Numeric import *
import os
import sys
import time
import re
from MultiRunBase import *

# Some globals
Vars = {}
AllVars = {}
Runs = []
AnalysisCommand = ''
RTPMaster = ''
IndependentVariable = 'OuterTimeStepInPicoSeconds'
DoSummary = 0


ListOnly = 0
RoundRobin = 1
argnum = 1
nargs = len(sys.argv)
while argnum < nargs:
  if sys.argv[argnum] == '-listonly':
    ListOnly = 1
    RoundRobin = 0
  elif sys.argv[argnum] == '-roundrobin':
    RoundRobin = 1
  elif sys.argv[argnum] == '-summarize':
    DoSummary = 1
  elif sys.argv[argnum] == '-killall':
    KillRuns()
    sys.exit(1)
  else:
    print "Command option %s not recognized" % sys.argv[argnum]
    print "Usage:  MultiRun.py [-listonly] [-roundrobin]"
    sys.exit(-1)
  argnum += 1

Res = []
f = open('RunResultsCurrent.txt')
Header = f.readline().strip()
while Header != '':
  l = f.readline().strip()
  rd = l.split()[0]
  rs = l.split()[1]
  h2 = f.readline().strip()
  v = f.readline().strip()
  rkey = "%s%s" % (rs, rd)
  Res.append(("Key %s %s" % (Header, h2), "%s %s %s" % (rkey, l, v)))
  blnk = f.readline()
  Header = f.readline().strip()

f.close()

Res.sort(lambda x,y: cmp(x[1], y[1]))

for v in Res:
  print "%s\n%s" % v

