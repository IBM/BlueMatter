# command line args:
# clock speed in Hz
# computational rate in FLOPS/cycle/node
# computational efficiency
# node network bandwidth in bits/cycle/node
# network latency in cycles/packet
# mesh size n (going to size nxnxn FFT)

from otherestimate import *

import sys


if len(sys.argv) < 9:
    print sys.argv[0],"clockspeed(Hz) compRate(FLOPS/cycle) compEff BW(bits/cycle) latency(cycles/packet) meshSize atomCount processorCount"
    sys.exit(1)
clock=float(sys.argv[1])
compRate=float(sys.argv[2])
eff=float(sys.argv[3])
bw=float(sys.argv[4])
latency=float(sys.argv[5])
meshSize=float(sys.argv[6])
atomCount=float(sys.argv[7])
procs = float(sys.argv[8])
# fix cutoff at 10 Angstroms
cutoff=10

platform=OtherPlatform(clock, compRate, eff, bw, latency)
print '%12.0e %12.3e' % (procs, tsEstimate_gp(procs, platform, meshSize, atomCount, cutoff)[5])
