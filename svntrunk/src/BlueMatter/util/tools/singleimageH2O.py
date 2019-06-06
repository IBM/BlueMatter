#!/usr/bin/env python

from __future__ import print_function
import sys
import os
from dvsutils import *


if len(sys.argv) < 9:
    print()
    print('Usage: singleimageH2O.py dvsfile minx miny minz maxx maxy maxz outputdvs')
    print('NOTE: this assumes file of water!!!')
    sys.exit(1)

FName = sys.argv[1]
FName2 = sys.argv[8]
min = Vec([ float(sys.argv[2]), float(sys.argv[3]), float(sys.argv[4])]);
max = Vec([ float(sys.argv[5]), float(sys.argv[6]), float(sys.argv[7])]);

box = max - min

coords = []
vels = []

flines = GetDVS(FName)
NLines = ReadPosVels(flines, coords, vels)

i = 0
while i < NLines:
      for j in range(3):
	    while min[j] > coords[i][j]:
		  coords[i][j] += box[j]
		  coords[i+1][j] += box[j]
		  coords[i+2][j] += box[j]
	    while max[j] <= coords[i][j]:
		  coords[i][j] -= box[j]
		  coords[i+1][j] -= box[j]
		  coords[i+2][j] -= box[j]
      i += 3

WriteDVSfile(coords, vels, FName2)

sys.exit(0)







