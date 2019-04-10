#!/usr/bin/env python

import sys
import os
from dvsutils import *


if len(sys.argv) < 6:
    print
    print 'Usage: transformdvs.py dvsfile x y z outputdvs'
    sys.exit(1)


FName = sys.argv[1]
FName2 = sys.argv[5]
tran = Vec( [float(sys.argv[2]),float(sys.argv[3]),float(sys.argv[4])])

coords = []
vels = []

flines = GetDVS(FName)
NLines = ReadPosVels(flines,coords,vels)

for i in range(NLines):
    coords[i] += tran

WriteDVSfile(coords,vels,FName2)

sys.exit()







