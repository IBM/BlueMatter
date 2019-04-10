#!/usr/bin/env python


import sys
import os
from dvsutils import *


if len(sys.argv) < 4:
    print
    print 'Usage: mergedvs.py dvsfile1 dvsfile2 mergeddvsfile'
    sys.exit(1)


FName = sys.argv[1]
FName2 = sys.argv[2]
FName3 = sys.argv[3]

flines = GetDVS(FName)
flines += GetDVS(FName2)

coords = []
vels = []

TLines = ReadPosVels( flines , coords, vels)

WriteDVSfile(coords,vels,FName3)

sys.exit(0)







