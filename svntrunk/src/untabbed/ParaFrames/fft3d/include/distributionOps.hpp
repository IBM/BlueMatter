/* Copyright 2001, 2019 IBM Corporation
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the 
 * following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
 * following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 /*#####################################################################
#                        I.B.M. CONFIDENTIAL                        #
#      (c) COPYRIGHT IBM CORP. 2001, 2003 ALL RIGHTS RESERVED.      #
#                                                                   #
#                LICENSED MATERIALS-PROPERTY OF IBM.                #
#                                                                   #
#####################################################################
*/
//  
//  IBM T.J.W  R.C.
//  Last Modified : 05/30/2006
//  Name : Maria Eleftheriou
//  This code provides some basic operations of data distributions
//

#ifndef __DISTRIBUTION_OPS_H_
#define __DISTRIBUTION_OPS_H_

#include <iostream>
#include <assert.h>

#if defined(OCO_SRC)
#define MAYBE_INLINE
#else
#define MAYBE_INLINE inline
#endif


class Distribution3DPoint
{
  enum{DIM=3};
private:
  int _p[DIM];

public:
  MAYBE_INLINE Distribution3DPoint();
  MAYBE_INLINE Distribution3DPoint(const Distribution3DPoint& p);
  MAYBE_INLINE Distribution3DPoint(const int x, const int y, const int z);
  
  inline int x() const{return _p[0];}
  inline int y() const{return _p[1];}
  inline int z() const{return _p[2];}

  MAYBE_INLINE int& x();
  MAYBE_INLINE int& y();
  MAYBE_INLINE int& z();
 
  MAYBE_INLINE Distribution3DPoint  operator+=(const Distribution3DPoint& p);
  MAYBE_INLINE Distribution3DPoint  operator-=(const Distribution3DPoint& p);
  MAYBE_INLINE Distribution3DPoint  operator-(const Distribution3DPoint& p);
  MAYBE_INLINE Distribution3DPoint &operator=(const Distribution3DPoint& p);
  
  void setX(const int x){ _p[0] = x;}
  void setY(const int y){ _p[1] = y;}
  void setZ(const int z){ _p[2] = z;}
  
  void Print(int i=0)
    {
      printf(" Distribution3DPoint[%d] = (x,y,z)=(%d,%d,%d) \n",i, x(), y(), z());
    }

};

class DistributionOps
{
private: 
  bool _isEmpty;
  // we only need the left lower and right higher points of the
  // rectangle.
  Distribution3DPoint _p0, _p1;

public:
  MAYBE_INLINE DistributionOps();
  MAYBE_INLINE DistributionOps(const DistributionOps& distributionOps);
  MAYBE_INLINE DistributionOps(const Distribution3DPoint& min, const Distribution3DPoint& max);
  
  
  MAYBE_INLINE const Distribution3DPoint& getLowerLeftPoint() const;
  MAYBE_INLINE const Distribution3DPoint& getUpperRightPoint()const;
 
 // Sets the distribution to be empty set.
  MAYBE_INLINE void setIsEmpty();
  // Check if two distributions overlap.
  MAYBE_INLINE bool isOverlap(const DistributionOps& r) const;
  // Find the overlaping region of two distributions.
  MAYBE_INLINE DistributionOps intersect(const DistributionOps&p);
  // Check if the distribution is an empty set
  inline const bool isEmpty() const{return _isEmpty;}
  
  // returns the end points of the data distribution.
  inline Distribution3DPoint& min(){ return _p0;} 
  inline Distribution3DPoint& max(){ return _p1;}
  inline Distribution3DPoint min() const{ return _p0;}
  inline Distribution3DPoint max() const{ return _p1;}
  
  // sets the endpoints of a given data distribution.
  inline void setMin(const Distribution3DPoint& p0)
  {
    _p0.setX(p0.x());
    _p0.setY(p0.y());
    _p0.setZ(p0.z());
  }

  inline void setMax(const Distribution3DPoint& p1)
  {
    _p1.setX(p1.x());
    _p1.setY(p1.y());
    _p1.setZ(p1.z());
  }
  
  // This is intented for debugging. It returns the info about a given distribution. 
  inline void Print(unsigned int i=0, unsigned int j=0, unsigned k=0) const
    {
      if(_isEmpty == true)
  {
    fprintf(stdout, "this is an empty distributionOps \n");
    // If the distribution is empty it shouldn't have any points.
    if(_p1.x()> _p0.x() && _p1.y()-_p0.y()>0 &&  _p1.z()> _p0.z())
      {
        fprintf(stderr, "Inconsistency: The distribution is not empty as claimed\n");
      }
  }
      
      // Print the distribution info.
      fprintf(stdout, "\n DistributionOps[%d %d %d]:  LowerLeftPoint(%d %d %d) UpperRightPoint(%d %d %d) Status %d \n",
        i,j,k, _p0.x(), _p0.y(), _p0.z(), _p1.x(), _p1.y(), _p1.z(), _isEmpty);
    }
  
  inline DistributionOps operator -= (const DistributionOps& r)
  {
    _p0 -= r.min();
    _p1 -= r.max();
    return *this;
  }
  
};

#if !defined(OCO_SRC)

Distribution3DPoint::Distribution3DPoint()
{
  for(int i=0; i<3; i++) _p[i]=0;
}

Distribution3DPoint::Distribution3DPoint(const Distribution3DPoint& p)
{
  _p[0]=p.x(); _p[1]=p.y(); _p[2]=p.z();
}

Distribution3DPoint::Distribution3DPoint(const int x, const int y, const int z)
{
  _p[0]=x; _p[1]=y; _p[2]=z;
}

int& Distribution3DPoint::x()
{
  return _p[0];
}

int& Distribution3DPoint::y()
{
  return _p[1];
}

int& Distribution3DPoint::z()
{
  return _p[2];
}

Distribution3DPoint Distribution3DPoint::operator += (const Distribution3DPoint& p)
{
  _p[0] += p.x();
  _p[1] += p.y();
  _p[2] += p.z();
  return *this;
}

Distribution3DPoint Distribution3DPoint::operator-=(const Distribution3DPoint& p)
{
  _p[0] -= p.x();
  _p[1] -= p.y();
  _p[2] -= p.z();
  return *this;
}

Distribution3DPoint Distribution3DPoint::operator-(const Distribution3DPoint& p0)
{
  return Distribution3DPoint(_p[0] - p0.x(), _p[1]- p0.y(), _p[2] - p0.z());
}

Distribution3DPoint& Distribution3DPoint::operator = (const Distribution3DPoint& p)
{
  _p[0] = p.x();
  _p[1] = p.y();
  _p[2] = p.z();
  return *this;
}
 
DistributionOps::DistributionOps():_isEmpty(true){};

DistributionOps::DistributionOps(const DistributionOps& r)
{
  _isEmpty = r.isEmpty();
  _p0 = r.min();
  _p1 = r.max();
};

DistributionOps::DistributionOps(const Distribution3DPoint& min, const Distribution3DPoint& max):
  _p0(min), _p1(max)
{
  // Distributions are empty when their widths or heights are less than zero.
  _isEmpty=(_p1.x() < _p0.x() || _p1.y()-_p0.y()<0 || _p1.z()< _p0.z())?true:false;
}  

void DistributionOps::setIsEmpty()
{
  _isEmpty=(_p1.x() < _p0.x() || _p1.y()<_p0.y() ||  _p1.z()< _p0.z())?true:false;
}

// These methods return the corner of rectangles
const Distribution3DPoint& DistributionOps::getLowerLeftPoint() const{ return _p0;}
const Distribution3DPoint& DistributionOps::getUpperRightPoint()const{ return _p1;}

// Check if my distribution overlaps with distribution r. Return true if overlaps, false otherwise.
bool DistributionOps::isOverlap(const DistributionOps& r) const
{
  //Check if distributions are empty. If not check if there is an overlap.
  if(!_isEmpty && !r.isEmpty())
    {
      if(   r._p0.x() <= _p1.x() && r._p0.y() <= _p1.y() && r._p0.z() <= _p1.z()
      && r._p1.x() >= _p0.x() && r._p1.y() >= _p0.y() && r._p1.z() >= _p0.z()) 
  {
    return true; 
  }
      
      return false;
    }
  
  return false;
}

// Return the overlapping region. It might be empty.
DistributionOps DistributionOps::intersect(const DistributionOps& r)
{ 
  int xmin, ymin, zmin;
  int xmax, ymax, zmax;

  // Check if the two distributions overlap. 
  if(isOverlap(r))
    {
      xmin = (r.min().x()>_p0.x()) ? r.min().x() : _p0.x();
      ymin = (r.min().y()>_p0.y()) ? r.min().y() : _p0.y();
      zmin = (r.min().z()>_p0.z()) ? r.min().z() : _p0.z();
      xmax = (r.max().x()<_p1.x()) ? r.max().x() : _p1.x();
      ymax = (r.max().y()<_p1.y()) ? r.max().y() : _p1.y();
      zmax = (r.max().z()<_p1.z()) ? r.max().z() : _p1.z();
      // Return the overlapping region of the two distributions. 
      return DistributionOps(Distribution3DPoint(xmin, ymin, zmin), Distribution3DPoint(xmax, ymax, zmax));
    }
  
  return DistributionOps(Distribution3DPoint(1, 0, 0), Distribution3DPoint(0, 0, 0));
}

#endif

#endif
