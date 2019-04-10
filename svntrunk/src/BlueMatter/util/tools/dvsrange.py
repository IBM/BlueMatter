#!/usr/bin/env python


import sys
import os
from dvsutils import *

if len(sys.argv) < 2:
    print
    print 'Usage: dvsrange.py dvsfile'
    sys.exit(1)


FName = sys.argv[1]
coords = []
vels = []

flines = GetDVS(FName)
NLines = ReadPosVels( flines , coords, vels)

print "NLines = " + str(NLines)

min = Vec([coords[0][0],coords[0][1],coords[0][2]])
max = Vec([coords[0][0],coords[0][1],coords[0][2]])


for i in range(NLines):
      v = coords[i]
      nm = "coord %d :" % i
      for j in range(3):
	    if min[j] > v[j]:
	          min[j] = v[j]
#		  min.outputNm("Min: ")
#		  v.outputNm(nm)
	    if max[j] < v[j]:
	          max[j] = v[j]
#		  max.outputNm("Max: ")
#		  v.outputNm(nm)


box = max - min
min.outputNm("Min: ")
max.outputNm("Max: ")
box.outputNm("Box: ")      


sys.exit()
