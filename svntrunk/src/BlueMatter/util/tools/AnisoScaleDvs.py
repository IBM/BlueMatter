#!/usr/bin/env python

from __future__ import print_function
import sys
import os
from dvsutils import *
from atlib import *

if len(sys.argv) < 6:
    print()
    print('Usage: AnisoScaleDvs.py dvsfile sx sy sz outputdvs')
    sys.exit(1)


FName = sys.argv[1]
FName2 = sys.argv[5]
scl = Vec( [float(sys.argv[2]),float(sys.argv[3]),float(sys.argv[4])])

coords = []
vels = []


flines = GetDVS(FName)
NLines = ReadPosVels(flines,coords,vels)

for i in range(NLines):
    coords[i][0] = coords[i][0] * scl[0]
    coords[i][1] = coords[i][1] * scl[1]
    coords[i][2] = coords[i][2] * scl[2]

ats = []

#for i in range(NLines):
#    ats += [ Atom(i,0,coords[i])  ]

#for i in range(NLines):
#    j = i + 1
#    while j < NLines:
#	if ats[i].IsWithinBondingRange(ats[j]):
#	   ats[i].AddBond(j)
#	   ats[j].AddBond(i)
#	j += 1
#    print "Cooking connections for atom %d" % (i)

#for i in range(NLines):
#    ats[i].Output()

WriteDVSfile(coords,vels,FName2)

sys.exit()







