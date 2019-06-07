from __future__ import print_function
import sys
from SquareMatrix import *

if len(sys.argv) < 2:
  print(sys.argv[0]+' matrix.txt niterations=100')
  print('Iterates square matrix and outputs running state vector')
  sys.exit(-1)

m = SquareMatrix()
m.markovMatrix(sys.argv[1])

m.state[0] = 1.0

NIterations = 100
if len(sys.argv)>2:
  NIterations = int(sys.argv[2])

for i in range(NIterations):
  #m.printStateDeltaScalar()
  m.printState()
  m.iterate()


