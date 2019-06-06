from __future__ import print_function
import bglestimate as bgl
import sys
import math


if len(sys.argv) < 5:
    print(sys.argv[0],"atomCount px py pz")
    sys.exit(1)
atomCount=int(sys.argv[1])
px=int(sys.argv[2])
py=int(sys.argv[3])
pz=int(sys.argv[4])

print("# atom count = ",atomCount)
boxSide=math.pow(atomCount/bgl.waterAtomDensity,0.33333333)
print("# corresponding to a water box with side = ",boxSide,"Angstroms")

procMesh=(px,py,pz)

print("processor mesh =", procMesh)
vDecomp=bgl.VolumeDecomp(procMesh, atomCount)
print("cell radius =", vDecomp.cellRadius())
print("site count =", vDecomp.siteCount(10))
