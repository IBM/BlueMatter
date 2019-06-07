#!/usr/bin/env python

from __future__ import print_function
from string import *
from runresults import *
import sys

data = RunResults(sys.argv[1])

all = []

RunSets = data.items()
RunSets.sort()

def ParseRun(run):
  ns = 0
  rt = 0
  ut = 0
  st = 0
  runset = ''
  desc = ''
  mn = ''
  dir = ''
  for i in run.items():
    if i[0] == 'Step':
      ns = int(i[1])
    elif i[0] == 'RunSet':
      runset = i[1]
    elif i[0] == 'RealTime':
      rt = float(i[1])
    elif i[0] == 'UserTime':
      ut = float(i[1])
    elif i[0] == 'SystemTime':
      st = float(i[1])
    elif i[0] == 'MachineName':
      mn = i[1]
    elif i[0] == 'DirName':
      dir = i[1]

  for p in run.params():
    if p[0] != 'RunSet' and p[0] != 'DirName' and p[0] != 'MachineName':
      desc = "%s %s" % (desc, p[1])
    
  return {'NSteps':ns, 'RunSet':runset, 'MachineName':mn, 'RealTime':rt, 'UserTime':ut, 'SystemTime':st, 'Desc':desc, 'Dir':dir}


print("\n%9s %15s %8s %9s %9s %9s %15s %8s %9s %9s %9s %9s %s" % ("s/ts", "Machine1", "NSteps1", "RealTime1",
       "UserTime1", "SysTime1", "Machine2", "NSteps2", "RealTime2", "UserTime2", "SysTime2", "Dir1", " Description"))

for r in RunSets:
  runs = r[1]
  if len(runs) != 2:
    print("Runset has %d items - Must have 2 per runset\n" % len(r.items()))
    print("Make sure there are two runs per runset, each with different number of steps.\n")
    print("Also, set NumberOfOuterTimeSteps as a the IndependentVariable in the MultiRun.rtp file.\n")
    sys.exit(-1)

  r0 = ParseRun(runs[0])
  r1 = ParseRun(runs[1])

  if r0['Desc'] != r1['Desc']:
    print("Descriptions don't match:\n%s\n%s\n" % (r0['Desc'], r1['Desc']))
    sys.exit(-1)
  
  dt = r1['UserTime'] - r0['UserTime']
  ns = r1['NSteps'] - r0['NSteps']
  tps = dt/ns
  print("%9.3f %15s %8d %9.2f %9.2f %9.2f %15s %8d %9.2f %9.2f %9.2f %9s %s" % (tps,
         r0['MachineName'], r0['NSteps'], r0['RealTime'], r0['UserTime'], r0['SystemTime'],
         r1['MachineName'], r1['NSteps'], r1['RealTime'], r1['UserTime'], r1['SystemTime'], r0['Dir'], r0['Desc']))



