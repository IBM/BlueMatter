import sys
from SquareMatrix import *

if len(sys.argv) < 2:
  print sys.argv[0]+' matrix.txt'
  print 'Calculates unit-eigenvalued eigenvector of square matrix'
  print 'Columns of input matrix should sum to unity'
  sys.exit(-1)

m = SquareMatrix()
m.markovMatrix(sys.argv[1])
#m.printUnitEigenvector()
for i in range(m.dimension):
  print i, abs(m.unitEigenvector[i])

sys.exit(0)

m.state[4] = 1.0

for i in range(2000):
  m.printStateDeltaScalar()
  m.iterate()


