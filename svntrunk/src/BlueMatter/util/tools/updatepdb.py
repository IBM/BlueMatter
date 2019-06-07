#!/usr/bin/env python


from __future__ import print_function
import sys
import os
from dvsutils import *

if len(sys.argv) < 2:
    print()
    print('Usage: updatepdb.py dvsfile pdbname ') 
    sys.exit(1)


FName = sys.argv[1]
PDBName = sys.argv[2]
coords = []
prefix = []
suffix = []

pdbfile = open(PDBName,"r")
pdblines = pdbfile.readlines()
pdbfile.close()

coords = []
vels = []

flines = GetDVS(FName)
NLines = ReadPosVels( flines , coords, vels)
c = -1;
nl = len(pdblines)
for i in range(nl):
    line = pdblines[i]
    if( line.startswith("ATOM")):
	c+= 1
	pref =line[:30]
	suff = line[54:(len(line)-1)]
	vec = coords[c]
	print("%s%8.3f%8.3f%8.3f%s" % (pref,vec[0],vec[1], vec[2], suff))
    else:
	l = line[:(len(line)-1)]
	print(l)
sys.exit()


