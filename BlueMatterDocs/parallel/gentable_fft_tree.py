from __future__ import print_function
# generate table of total time for 3D FFT versus node count given system params
import fft_tree;
# command line args:
# clock speed in Hz
# computational rate in FLOPS/cycle/node
# computational efficiency
# node network bandwidth in bits/cycle/node
# network latency in cycles/packet
# mesh size n (going to size nxnxn FFT)
import sys

if len(sys.argv) < 7:
    print(sys.argv[0],"clockspeed(Hz) compRate(FLOPS/cycle) compEff BW(bits/cycle) latency(cycles/packet) meshSize <maxNode=64000>")
    sys.exit(1)
clock=float(sys.argv[1])
compRate=float(sys.argv[2])
eff=float(sys.argv[3])
bw=float(sys.argv[4])
latency=float(sys.argv[5])
meshSize=float(sys.argv[6])
maxNode=64000
if len(sys.argv) > 7:
    maxNode=int(sys.argv[7])

print("# machine parameters:")
print("# clock speed:", str(clock))
print("# computation rate (FLOPS/cycle/node): ", str(compRate))
print("# computational efficiency:", str(eff))
print("# node bandwidth (bits/cycle):", str(bw))
print("# packet latency (cycles/packet):", str(latency))
print("# problem mesh size:", str(meshSize))
print("#")
print("# output columns:")
print("# column 1: node count")
print("# column 2: computation time")
print("# column 3: communication time")
print("# column 4: total time")
print("# column 5: parallel efficiency")
print("# column 6: packet size (bytes)")
fft=fft_tree.FFTestimate(clock, compRate, eff, bw, latency)
uniTime=fft.compTime(1,meshSize)
print("# uniTime =", uniTime)
p = 2
while p <= maxNode:
    tcomm=fft.commTime(p,meshSize)
    tcomp=fft.compTime(p,meshSize)
    tTotal=tcomm+tcomp
    parEff=uniTime/(float(p)*(tTotal))
#    print uniTime,tTotal,parEff
    print('%12d %12.3e %12.3e %12.3e %12.3e %12.3e' % (p, tcomp, tcomm, tcomp+tcomm, parEff, fft.packetSize(p,meshSize)))
    p=p*2

