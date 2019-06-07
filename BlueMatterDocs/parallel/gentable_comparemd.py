from __future__ import print_function
# generates table containing estimated wall clock time/timestep for
# BG/L and other platforms

import sys
import otherestimate as other
import bglestimate as bgl

cutoff=10 #angstroms

if len(sys.argv) < 3:
    print(sys.argv[0], "meshSize atomCount")
    sys.exit(1)

meshSize=int(sys.argv[1])
atomCount=int(sys.argv[2])
bglClock=700.0e6

print("# estimates of time step duration using parallelization by globalizing positions")
print("# column 1: processor count")
print("# column 2: px")
print("# column 3: py")
print("# column 4: pz")
print("# column 5: time(bgl)")
print("# column 6: time(power5blade)")
print("# column 7: time(gpul2blade)")
print("# column 8: time(ia32blade)")

for p in ((2,2,2),(2,2,4),(2,4,4),(4,4,4),(8,4,4),(8,8,4),(8,8,8),(8,8,16),(8,16,16),(16,16,16),(16,16,32),(16,32,32)):
    procs=p[0]*p[1]*p[2]
    bgl_ts=bgl.tsEstimate_gp((p[0],p[1],p[2]),\
                             (meshSize,meshSize,meshSize),\
                             atomCount,\
                             cutoff)
    print('%12d %4d %4d %4d %12.3e %12.3e %12.3e %12.3e' %\
          (procs, p[0], p[1], p[2],\
           bgl_ts[5]/bglClock,\
           other.tsEstimate_power5blade(procs, meshSize, atomCount, cutoff)[5],\
           other.tsEstimate_gpul2blade(procs, meshSize, atomCount, cutoff)[5],\
           other.tsEstimate_ia32blade(procs, meshSize, atomCount, cutoff)[5]))

           
