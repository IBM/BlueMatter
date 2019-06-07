from __future__ import print_function
from Numeric import *
import sys

class SquareMatrix:
  'Square matrix class with import and eigenvector support.'

  def __init__(self):
    self.dimension = 0
    self.fileName = ''
    self.unitEigenvector = None
    self.matrix = None
    self.state = None

  def create(self, n):
    self.dimension = n
    self.matrix = zeros((n,n), Float)
    self.state = zeros(n, Float)

  def load(self, fname):
    self.fileName = fname
    try:
      f = open(fname)
      l = f.readline()
      toks = l.split()
      ntoks = len(toks)
      self.create(ntoks)
      for i in range(ntoks):
        for j in range(ntoks):
          self.matrix[i,j] = float(toks[j])
        l = f.readline()
        toks = l.split()
      f.close()
    except:
      print('Error opening or reading file ', fname)
      sys.exit(-1)

  def normalizeColumns(self):
    for j in range(self.dimension):
      sum = 0.0
      for i in range(self.dimension):
        sum += self.matrix[i,j]
      if sum == 0.0:
        sum = 1.0
      for k in range(self.dimension):
        self.matrix[k,j] /= sum

  def findUnitEigenvector(self):
    import LinearAlgebra
    evecs = LinearAlgebra.eigenvectors(self.matrix)
    self.eigenValues  = evecs[0]
    self.eigenVectors = evecs[1]
    closest = -1
    best = 1.0E+10
    for i in range(self.dimension):
      sep = abs(self.eigenValues[i]-1)
      if sep<best:
        best = sep
        closest = i
    e = self.eigenVectors[closest]
    self.unitEigenvalue = self.eigenValues[closest]
    sum = 0.0
    for i in range(self.dimension):
      sum += e[i]
    for j in range(self.dimension):
      e[j] /= sum
    self.unitEigenvector = e

  def setState(self, v):
    if len(v) != self.dimension:
      print('State vector dimension does not match matrix')
      sys.exit(-1)
    self.state = v

  def iterate(self):
    self.state = matrixmultiply(self.matrix, self.state)

  def printState(self):
    for i in range(self.dimension):
      print(self.state[i], " ", end=' ')
    print()

  def printStateDeltaVector(self):
    for i in range(self.dimension):
      # self.normalizeState()
      print(self.state[i]-abs(self.unitEigenvector[i]), " ", end=' ')
    print()

  def printStateDeltaScalar(self):
    sum = 0.0
    for i in range(self.dimension):
      sum += (self.state[i]-abs(self.unitEigenvector[i]))**2
    print(sqrt(sum))

  def normalizeState(self):
    sum = 0.0
    for i in range(self.dimension):
      sum += self.state[i]
    if (sum != 0):
      sum = 1
    for i in range(self.dimension):
      self.state[i] /= sum

  def dumpColumnSum(self):
    for j in range(self.dimension):
      sum = 0.0
      for i in range(self.dimension):
        sum += self.matrix[i,j]
      print(sum)

  def markovMatrix(self, fname):
    self.load(fname)
    self.normalizeColumns()
    self.findUnitEigenvector()
    if abs(abs(self.unitEigenvalue)-1) > 1.0E-3:
      print('unit eigenvalue looks off - value is ', self.unitEigenvalue)
      sys.exit(-1)

  def printUnitEigenvector(self):
    for i in range(self.dimension):
      print(abs(self.unitEigenvector[i]))

    

def test():
  'Test function for SquareMatrix module'
  m = SquareMatrix()
  m.load('mtest.txt')
  m.dumpColumnSum()
  m.normalizeColumns()
  print(m.matrix)
  print()
  m.dumpColumnSum()
  print()
  #print m.matrix
  # print
  m.findUnitEigenvector()
  print(m.unitEigenvalue)
  print()
  for i in range(m.dimension):
    print(abs(m.unitEigenvector[i]))

if __name__ == '__main__':
  test()