from __future__ import print_function
# generate table containing time/timestep in seconds

import bglestimate as bgl
import sys
import math

clock=700.0e6
cutoff=10.0 # angstroms

if len(sys.argv) < 3:
    print(sys.argv[0],"meshSize atomCount <clock = 700e6>")
    sys.exit(1)

if len(sys.argv) > 3:
    clock = float(sys.argv[3])
    
meshSize=int(sys.argv[1])
atomCount=int(sys.argv[2])

print("# estimates of wall clock time/time step (seconds) for molecular dynamics")
print("# using clock frequency = ", clock, "Hertz")
print("column 1: processor count")
print("column 2: px")
print("column 3: py")
print("column 4: pz")
print("column 5: time(position globalization)")
print("column 6: time(global force reduction)")
print("column 7: speedup(position globalization)")
print("column 8: speedup(global force reduction)")

singleNode_gp = bgl.tsEstimate_singleNode_gp((meshSize, meshSize, meshSize), atomCount, 10)
singleNode_gfr = bgl.tsEstimate_singleNode_gfr((meshSize, meshSize, meshSize), atomCount, 10)
procs = 1
p = (1,1,1)
unity = 1.0
print('%12d %4d %4d %4d' % (procs,p[0],p[1],p[2]), end=' ')
print('%12.3e' % (float(singleNode_gp[5])/clock), end=' ')
print('%12.3e' % (float(singleNode_gfr[6])/clock), end=' ')
print('%12.3e' % (unity), end=' ')
print('%12.3e' % (unity))

for p in ((2,2,2),(2,4,4),(4,4,4),(8,4,4),(8,8,4),(8,8,8),(8,8,16),(8,16,16),(16,16,16),(16,16,32),(16,32,32)):
    procs=p[0]*p[1]*p[2]
    gpTS = bgl.tsEstimate_gp((p[0],p[1],p[2]), (meshSize,meshSize,meshSize),atomCount, 10)
    gfrTS = bgl.tsEstimate_gfr((p[0],p[1],p[2]), (meshSize,meshSize,meshSize),atomCount, 10)
    print('%12d %4d %4d %4d' % (procs,p[0],p[1],p[2]), end=' ')
    print('%12.3e' % (float(gpTS[5])/clock), end=' ')
    print('%12.3e' % (float(gfrTS[6])/clock), end=' ')
    print('%12.3e' % (float(singleNode_gp[5])/float(gpTS[5])), end=' ')
    print('%12.3e' % (float(singleNode_gfr[6])/float(gfrTS[6])))
