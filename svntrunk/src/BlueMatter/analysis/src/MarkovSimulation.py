import sys
from SquareMatrix import *
from math import *
from random import *

#if len(sys.argv) < 1:
#  print 'MarkovSimulation.py fname'
#  sys.exit(-1)

m = SquareMatrix()

m.create(2)

nruns = 10
nsteps = 200

t1 = 30.0
t2 = 10.0

print "Specified lifetimes:"
print t1, t2

p1 = 1-1/t1*exp(1.0/t1)
p2 = 1-1/t2*exp(1.0/t2)

p12 = 1-p1
p21 = 1-p2

m.matrix[0,0] = p1
m.matrix[1,0] = p12
m.matrix[1,1] = p2
m.matrix[0,1] = p21

print
print "Exact transition matrix"
print m.matrix

m.findUnitEigenvector()

print
print "Steady state distribution (directly calculated unit-eigenvalued eigenvector):"
m.printUnitEigenvector()

pstart1 = m.unitEigenvector[0]

print

track = zeros((2,2))
lifecount = [0,0]

for i in range(nruns):
  current = 0
  if random() > pstart1:
    current = 1
  clist = ''
  prev = current
  inbin = 0
  for j in range(nsteps):
    clist += ['0','1'][current]
    track[current, current] += 1
    if random() > m.matrix[current, current]:
      current = 1 - current
    if prev != current:
      track[prev, current] += 1
      if inbin:
        lifecount[prev] += 1
      inbin = 1
    prev = current
  print clist
  print

#print track
#print
#print lifecount

t1est = float(track[0,0])/lifecount[0]
t2est = float(track[1,1])/lifecount[1]

print
print "Estimated lifetimes based on the %d runs of length %d:" % (nruns, nsteps)
print t1est, t2est

#mest = zeros((2,2), Float)
mest = SquareMatrix()
mest.create(2)
mest.matrix[0,0] = 1.0 - 1/t1est*exp(1/t1est)
mest.matrix[1,1] = 1.0 - 1/t2est*exp(1/t2est)
mest.matrix[1,0] = 1.0 - mest.matrix[0,0]
mest.matrix[0,1] = 1.0 - mest.matrix[1,1]

mest.findUnitEigenvector()

print
print "Estimated transition matrix:"
print mest.matrix
print
print "Estimated steady state distribution:"
mest.printUnitEigenvector()


sys.exit(0)





m.markovMatrix(sys.argv[1])

m.state[9] = 1.0

for i in range(20000):
  #m.printStateDeltaScalar()
  m.printState()
  m.iterate()


