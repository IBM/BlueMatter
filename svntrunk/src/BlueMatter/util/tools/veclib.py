import math

class Vec:
	""" Class Vec with default constructor"""
	
	def __init__(self, v = [ 0, 0, 0]):
	    """ Initialize all vecs to zero """
	    self._v = v
	 
	def copy(self, v = [0,0,0]):
	    """ Element-wise copy """
	    self._v = v
	     
	def __getitem__( self, i):
	    """ allow array access for reading components """
	    return self._v[i]

	def __setitem__( self, i , val):
	    """ allow array access for write components """
	    self._v[i] = val

        def output( self):
	    """ bracketed component output """
	    print " [ %f, %f, %f ] " % (self._v[0], self._v[1], self._v[2])

        def outputNm( self, name):
	    """ bracketed component output """
	    print "%s:  [ %f, %f, %f ] " % (name, self._v[0], self._v[1], self._v[2])

	def __iadd__(self, v):
	    """ += operator """
	    self._v[0] += v[0]
	    self._v[1] += v[1]
	    self._v[2] += v[2]
	    return (self)

	def __add__(self, v):
	    """ + operator """
	    return (Vec([self._v[0] + v._v[0], \
		   self._v[1] + v._v[1], \
		   self._v[2] + v._v[2]]))

	def __isub__(self, v):
	    """ -= operator """
	    self._v[0] -= v[0]
	    self._v[1] -= v[1]
	    self._v[2] -= v[2]
	    return (self)

	def __sub__(self, v):
	    """ - operator """
	    return (Vec([self._v[0] - v._v[0], \
		   self._v[1] - v._v[1], \
		   self._v[2] - v._v[2]]))

	def __mul__(self, v):
	    """ * operator vector, scalar """
	    return (Vec([self._v[0] * v, \
		   self._v[1] * v, \
		   self._v[2] * v]))

	def __imul__(self, d):
	    """ *= operator vector, scalar """
	    self._v[0] *= d
	    self._v[1] *= d
	    self._v[2] *= d
	    return (self)

	def __div__(self, v):
	    """ / operator vector, scalar """
	    return (Vec([self._v[0] / v, \
		   self._v[1] / v, \
		   self._v[2] / v]))

	def __idiv__(self, d):
	    """ /= operator vector, scalar """
	    self._v[0] /= d
	    self._v[1] /= d
	    self._v[2] /= d
	    return (self)

	def dot(self, v):
	    """ Dot Product """
	    d = self._v[0] * v[0] + \
	    self._v[1] * v[1] + \
	    self._v[2] * v[2]
	    return d

	def SqLength(self):
	    """ Length squared """
	    d = self._v[0] * self._v[0] + \
	    self._v[1] * self._v[1] + \
	    self._v[2] * self._v[2] 
	    return d

	def Rotate(self, axis, theta):
	    """ Rotate about axis by theta """
	    s = math.sin(theta)
	    c = math.cos(theta)
	    t = 1 - c
	    x = axis[0]
	    y = axis[1]
	    z = axis[2]
	    nx = self._v[0] * (x * x * t + c) + \
		 self._v[1] * (x * y * t + s * z) + \
		 self._v[2] * (x * z * t - s * y)
	    ny = self._v[1] * (y * y * t + c) + \
		 self._v[0] * (x * y * t - s * z) + \
		 self._v[2] * (y * z * t + s * x)
	    nz = self._v[2] * (z * z * t + c) + \
		 self._v[1] * (z * y * t - s * x) + \
		 self._v[0] * (x * z * t + s * y)
	    self._v[0] = nx
	    self._v[1] = ny
	    self._v[2] = nz

	def Cross(self, A, B):
	    self._v[0] = A[1] * B[2] - A[2] * B[1]
	    self._v[1] = A[2] * B[0] - A[0] * B[2]
	    self._v[2] = A[0] * B[1] - A[1] * B[0]
