#!/usr/bin/env python


import sys
import os
from dvsutils import *


if len(sys.argv) < 2:
    print
    print 'Usage: shift_pos_in_dvs.py dvsfile idlist dx dy dz dvx dvy dvz outname'
    sys.exit(1)


FName = sys.argv[1]
IdList = sys.argv[2]
dpx = float(sys.argv[3])
dpy = float(sys.argv[4])
dpz = float(sys.argv[5])
dvx = float(sys.argv[6])
dvy = float(sys.argv[7])
dvz = float(sys.argv[8])
outname = sys.argv[9]

dp = Vec([dpx,dpz,dpz])
dv = Vec([dvx,dvz,dvz])

coords = []
vels = []

cmd = 'cat ' + IdList
ids = GetOutput(cmd)

flines = GetDVS(FName)
NLines = ReadPosVels( flines , coords, vels)

for i in range(len(ids)):
    idstr = ids[i].split()
    id = int(idstr[0])
    coords[id-1] += dp 
    vels[id-1] += dv

WriteDVSfile(coords,vels,outname)
sys.exit()
