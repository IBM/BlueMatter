# "scalability" estimates for 3d fft using torus communication
import bglestimate as bgl
import sys
import math

clock=700e6
if len(sys.argv) > 2:
    clock=float(sys.argv[2])
if len(sys.argv) < 2:
    print sys.argv[0],"meshSize <clock=700e6>"
    sys.exit(1)
meshSize=int(sys.argv[1])
chargeMesh=(meshSize, meshSize, meshSize)
print "# meshSize = ", meshSize
print "# procCount px py pz fftComm fftComp fftTotal(cycles) fftTotal(sec.)"
#for p in ((2,2,24), (4,4,6),(8,8,9), (16,8,9), (32,32,64)):
for p in ((2,2,2),(2,4,4),(4,4,4),(8,4,4),(8,8,4),(8,8,8),(8,8,16),(8,16,16),(16,16,16),(16,16,32),(16,32,32),(32,32,32),(32,32,64)):
    procs=p[0]*p[1]*p[2]
    if procs > meshSize*meshSize:
        continue
    fft=bgl.RealFFT3D(p, chargeMesh)
    print '%12d %4d %4d %4d' % (procs,p[0],p[1],p[2]),
    print '%12.3e %12.3e %12.3e %12.3e' % (fft.commTime(), fft.compTime(), fft.commTime()+fft.compTime(), (fft.commTime()+fft.compTime())/clock)


