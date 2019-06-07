from __future__ import print_function
import otherestimate as other

import sys

if len(sys.argv) < 4:
    print(sys.argv[0], "meshSize atomCount targetTime")
    sys.exit(1)

cutoff = 10.0 # angstroms
meshSize=int(sys.argv[1])
atomCount=int(sys.argv[2])
target=float(sys.argv[3])

maxP=65000

p = 2
left=3
power5=0
gpul2=0
ia32=0
while (left > 0) and (p < maxP):
    # print p,
    t_power5=other.tsEstimate_power5blade(p, meshSize, atomCount, cutoff)[5]
    # print t_power5,
    if (power5==0) and (t_power5 < target):
        print("power5blade equivalent nodes =", p)
        left=left-1
        power5=1
    t_gpul2=other.tsEstimate_gpul2blade(p, meshSize, atomCount, cutoff)[5]
    # print t_gpul2,
    if (gpul2==0) and (t_gpul2 < target):
        print("gpul2blade equivalent nodes =", p)
        left=left-1
        gpul2=1
    t_ia32 = other.tsEstimate_ia32blade(p, meshSize, atomCount, cutoff)[5]
    # print t_ia32
    if (ia32==0) and (t_ia32 < target):
        print("ia32blade equivalent nodes =", p)
        left=left-1
        ia32=1
    p=p+1

