#!/usr/bin/env python

from __future__ import print_function
from xreadlines import *
import sys
import os
import time
#import gui_thread
#time.sleep(3)
#from chaco.wxplt import *
#time.sleep(3)
import FFT
from Numeric import *

"""
if len(sys.argv) < 3:
    print
    print 'Usage: lipid_life.py fname Number of protons'
    print '   or: MDPlot.py -    to read from stdin'
    sys.exit(1)
"""

fname = sys.argv[1]
###################   Constants
# permeability
mu0 = 1.2566371e-6
# nuclear magneton
mun = 5.050787e-27
# magnetic moments
mu1H = 2.79284
mu2H = 0.85743
mu13C = 0.70241
# plank
h = 6.626076e-34
# C--H bond distance
r = 1.11e-10
# Larmor frequencies
omegaH = 498.13e6
omega2H = 76.465866e6
omegaC = 125.72e6

# number of protons
n = float(sys.argv[2])
########  (nT1)^-1 = 4pi/10 * Tconst^2 * (J(omegaH - omegaC) + 3J(omegaC) + 6J(omegaH + omegaC))
Tconst = mu0*mun*mun*mu2H*mu13C*2.0/h/r/r/r

omega1 =  -omega2H + omegaC
omega3 = omegaC
omega6 = omega2H + omegaC

print("reading file %s " % fname)

#allx = ()
#ally = ()
#xy = ()
#allxy = []


t = []
Ct = []

file = open(fname)
for l in xreadlines(file):
    s = l.split()
# assume timesteps are expressed in ps, convert to s
    t.append(float(s[0]) * 1e-12)
    Ct.append(float(s[1]))

dt = t[1] - t[0] 
      


J1 = 0
J3 = 0
J6 = 0
npts = len(t)
for i in range(npts):
    J1 += Ct[i] * cos(omega1 * t[i]) * dt
    J3 += 3 * Ct[i] * cos(omega3 * t[i]) * dt
    J6 += 6 * Ct[i] * cos(omega6 * t[i]) * dt
    file.close()
#  allx += (array(x)[:,NewAxis],)
#  ally += (array(y)[:,NewAxis],)
#  xy += (array(x)[:,NewAxis],array(y)[:,NewAxis])
#  allxy.append(allx[-1])
#  allxy.append(ally[-1])



rT1 = n * 4.0 * 3.14159 / 10.0 * Tconst * Tconst * ( J1 + J3 + J6)
T1 = 1.0 / rT1
print(" J(H - C) = %g   3J(C) = %g    6J(H + C) = %g " % (J1,J3,J6))
print(" T1 = %f " % T1)



#test = 4*3.14159/10.0 * Tconst * Tconst
#print test

sys.exit(0)

#print x[0], y[0]
#print x[1], y[1]
#print len(x)

#z=FFT.fft(y)
#xx=arange(len(z))
#for i in range(len(z)):
#	print " %f %f " % (x[i],z[i].real)

#plot(xx, hstack((real(z)[:,NewAxis],imag(z)[:,NewAxis])))
#plot(allx, ally)
#plot(xy)
#plot(((allx[0],ally[0]),(allx[1], ally[1])))
#plot(allx[0], ally[0],'ro',allx[0], ally[1],'bo')
#plot(allxy)
#plot(allxy[0], allxy[1], allxy[2], allxy[3], allxy[4], allxy[5])
#title('hello')
#plot(xx,real(z), xx, imag(z), xx, y)

#time.sleep(1000)
#import msvcrt
#msvcrt.getch()
#close('all')

