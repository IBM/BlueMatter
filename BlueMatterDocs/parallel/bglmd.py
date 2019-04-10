# drive bglestimate.py
import bglestimate as bgl
import sys
import math

clock=700.0e6
cutoff=10.0 # angstroms

if len(sys.argv) < 6:
    print sys.argv[0], "atomCount meshSize px py pz"
    sys.exit(1)
atomCount=int(sys.argv[1])
meshSize=int(sys.argv[2])
px=int(sys.argv[3])
py=int(sys.argv[4])
pz=int(sys.argv[5])

chargeMesh=(meshSize, meshSize, meshSize)
procMesh=(px, py, pz)

p=px*py*pz

print '%12d %4d %4d %4d %12.3e' % (p, px, py, pz, bgl.tsEstimate_gp(procMesh,\
                                                                    chargeMesh,\
                                                                    atomCount,\
                                                                    cutoff)[5]/clock)

