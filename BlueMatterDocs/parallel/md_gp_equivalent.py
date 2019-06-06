from __future__ import print_function
# find equivalent node count by crudest method possible (stepping up from 1 node)
import fft_tree;
# command line args:
# clock speed in Hz
# computational rate in FLOPS/cycle/node
# computational efficiency
# node network bandwidth in bits/cycle/node
# network latency in cycles/packet
# mesh size n (going to size nxnxn FFT)
# target run time (seconds)
import sys

if len(sys.argv) < 8:
    print(sys.argv[0],"clockspeed(Hz) compRate(FLOPS/cycle) compEff BW(bits/cycle) latency(cycles/packet) meshSize targetRunTime")
    sys.exit(1)
clock=float(sys.argv[1])
compRate=float(sys.argv[2])
eff=float(sys.argv[3])
bw=float(sys.argv[4])
latency=float(sys.argv[5])
meshSize=float(sys.argv[6])
target=float(sys.argv[7])

p=1
maxProcs=10000
fft=fft_tree.FFTestimate(clock, compRate, eff, bw, latency)

while fft.commTime(p, meshSize)+fft.compTime(p, meshSize) > target:
    if p > maxProcs:
        print("No equivalent node count found for p <=",maxProcs)
        break;
    p=p+1
print("node count=",p)
print("target time=",target)
print("estimated time for ",p,"nodes = ", fft.commTime(p,meshSize)+fft.compTime(p,meshSize))
