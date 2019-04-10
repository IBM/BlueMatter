
import string
import sys
import os
from veclib import Vec

MAX_BOND_DIST = 1.7 * 1.7


class Atom:

      def __init__(self):
	  self.d_id = 0
	  self.d_elenum = 0
	  self.d_crd = Vec( [0,0,0] )
	  self.d_natch = 0
	  self.d_atch = [0,0,0,0]
	  
      def __init__(self, id, ele, crd):
	  self.d_id = id
	  self.d_elenum = ele
	  self.d_crd = crd
	  self.d_natch = 0
	  self.d_atch = [0,0,0,0]

      def __setitem__( self, itm):
	  self.d_id = itm.d_id
	  self.d_elenum = itm.d_elenum
	  self.d_crd = itm.d_crd
	  self.d_natch = itm.d_natch
	  self.d_atch = itm.d_atch
	  return(self)

      def Id(self):
          return (self.d_id)

      def Element(self):
          return (self.d_elenum)
       
      def Crd(self):
	  return (self.d_crd)

      def Natch(self):
	  return (self.d_natch)

      def Atch(self, a):
	  if(a >= self.d_natch):
	       print "Atch: natch = %d; passed %d" % (self.d_natch, a)
	       sys.exit(-1)
	  return (self.d_atch[a])

      def AddBond(self, id):
	  self.d_atch[self.d_natch] = id
	  self.d_natch += 1;
	  return(self.d_natch)

      def IsWithinBondingRange( self, at ):
	  d = at.d_crd - self.d_crd
	  r = d.SqLength()
	  if( r < MAX_BOND_DIST ):
	      return(1)
	  return(0)


      def Output(self):
	  print "Atom %d ( %d ): [ %f %f %f ] atch ( %d ) [ %d %d %d %d ]" % (self.d_id, self.d_elenum, self.d_crd[0], self.d_crd[1], self.d_crd[2], self.d_natch, self.d_atch[0], self.d_atch[1], self.d_atch[2], self.d_atch[3])
