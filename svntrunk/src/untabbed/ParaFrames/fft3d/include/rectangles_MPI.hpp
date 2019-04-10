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
//  Date : 06/02/2003
//  Name : Maria Eleftheriou
//  This code calculates the intersection of two overlapping rectangles
//

#ifndef __RECTANGLES_H_
#define __RECTANGLES_H_
#include <iostream>
#include <assert.h>
class Point3D
{
  enum{DIM=3};
private:
  int _p[DIM];

public:
  inline Point3D();
  inline Point3D(const Point3D& p);
  inline Point3D(const int x, const int y, const int z);
  
  int x() const{return _p[0];}
  int y() const{return _p[1];}
  int z() const{return _p[2];}

  inline int& x();
  inline int& y();
  inline int& z();
 
  inline Point3D operator+=(const Point3D& p);
  inline Point3D operator-=(const Point3D& p);
  inline Point3D operator-(const Point3D& p);
  inline Point3D &operator=(const Point3D& p);
  
  void setX(const int x){ _p[0] = x;}
  void setY(const int y){ _p[1] = y;}
  void setZ(const int z){ _p[2] = z;}
  
  void Print(int i=0)
    {
      printf(" Point3D[%d] = (x,y,z)=(%d,%d,%d) \n",i, x(), y(), z());
    }

};

class Rectangle
{
private: 
  bool _isEmpty;
  // we only need the left lower and right higher points of the
  // rectangle.
  Point3D _p0, _p1;

public:
  inline Rectangle();
  inline Rectangle(const Rectangle& rectangle);
  inline Rectangle(const Point3D& min, const Point3D& max);
  
  // These methods return the corner of rectangles
  inline const Point3D& getLowerLeftPoint() const;
  inline const Point3D& getUpperRightPoint()const;
  inline void setIsEmpty();
  inline bool overlaps(const Rectangle& r) const;
  inline Rectangle intersect(const Rectangle&p);   
  inline const bool isEmpty() const{return _isEmpty;}
  
  inline Point3D& min(){ return _p0;}
  inline Point3D& max(){ return _p1;}

  inline Point3D min() const{ return _p0;}
  inline Point3D max() const{ return _p1;}

  inline void setMin(const Point3D& p0){
    _p0.setX(p0.x());
    _p0.setY(p0.y());
    _p0.setZ(p0.z());
  }
  inline void setMax(const Point3D& p1){
    _p1.setX(p1.x());
    _p1.setY(p1.y());
    _p1.setZ(p1.z());}
  

  inline void Print(unsigned int i=0, unsigned int j=0, unsigned k=0) const
    {
      if(_isEmpty == true)
  {
    printf("this is an empty rectangle \n");
      
    if(_p1.x()> _p0.x() && _p1.y()-_p0.y()>0 &&  _p1.z()> _p0.z())
      {
        printf("ophs ..: Error the triangle should not be empty\n");
      }
  }
      
      printf("\n Rectangle[%d %d %d]:  LowerLeftPoint(%d %d %d) UpperRightPoint(%d %d %d) Status %d \n"
       , i,j,k, _p0.x(), _p0.y(), _p0.z(), _p1.x(), _p1.y(), _p1.z(), _isEmpty);
    
  
    }
  
  inline Rectangle operator -= (const Rectangle& r)
    {
      _p0 -= r.min();
      _p1 -= r.max();
      return *this;
    }
  
};



Point3D::Point3D()
{ 
  _p[0] = _p[1] = _p[2] = 0;
}

Point3D::Point3D(const Point3D& p)
{
  _p[0] = p.x();
  _p[1] = p.y();
  _p[2] = p.z();
}

Point3D::Point3D(const int x, const int y, const int z)
{
  _p[0] = x; 
  _p[1] = y; 
  _p[2] = z;
}	

int& Point3D::x()
{
  return _p[0];
}

int& Point3D::y()
{
  return _p[1];
}

int& Point3D::z()
{
  return _p[2];
}

Point3D Point3D::operator += (const Point3D& p)
{
  _p[0] += p.x();
  _p[1] += p.y();
  _p[2] += p.z();
  return *this;
}

Point3D Point3D::operator-=(const Point3D& p)
{
  _p[0] -= p.x();
  _p[1] -= p.y();
  _p[2] -= p.z();
  return *this;
}

Point3D Point3D::operator-(const Point3D& p0)
{
  return Point3D(_p[0] - p0.x(), _p[1]- p0.y(), _p[2] - p0.z());
}

Point3D& Point3D::operator = (const Point3D& p)
{
  _p[0] = p.x();
  _p[1] = p.y();
  _p[2] = p.z();
  return *this;
}
 
Rectangle::Rectangle():_isEmpty(true){};

Rectangle::Rectangle(const Rectangle& r)
{
  _isEmpty = r.isEmpty();
  _p0 = r.min();
  _p1 = r.max();
};

Rectangle::Rectangle(const Point3D& min, const Point3D& max):
  _isEmpty(false), _p0(min), _p1(max)
{
  // A rectangle is empty if its width or its height is less than zero
  
  if(_p1.x() < _p0.x() || _p1.y()-_p0.y()<0 || _p1.z()< _p0.z())
    {
      _isEmpty=true;
    }
}  

void Rectangle::setIsEmpty()
{
  if(_p1.x() < _p0.x() || _p1.y()<_p0.y() ||  _p1.z()< _p0.z())
    {
      _isEmpty=true;
    }
  else
    {
      _isEmpty = false;
    }
}

// These methods return the corner of rectangles
const Point3D& Rectangle::getLowerLeftPoint() const{ return _p0;}
const Point3D& Rectangle::getUpperRightPoint()const{ return _p1;}

bool Rectangle::overlaps(const Rectangle& r) const
{
  if(!_isEmpty && !r.isEmpty())
    {
      if(   r._p0.x() <= _p1.x() 
   && r._p0.y() <= _p1.y() 
   && r._p0.z() <= _p1.z()
   && r._p1.x() >= _p0.x() 
   && r._p1.y() >= _p0.y() 
   && r._p1.z() >= _p0.z()) 
  {
    return true; 
  }
      
      return false;
    }
  
  return false;
}

Rectangle Rectangle::intersect(const Rectangle& r)
{ 
  int xmin = 0;
  int ymin = 0;
  int zmin = 0;
  int xmax = 0;
  int ymax = 0;
  int zmax = 0;
 
  if(overlaps(r))
    {
      if(r.min().x()>_p0.x()) {xmin = r.min().x();}
      else {xmin = _p0.x();}

      if(r.min().y()>_p0.y()) {ymin = r.min().y();}
      else {ymin = _p0.y();}
      
      if(r.min().z()>_p0.z()) {zmin = r.min().z();}
      else {zmin = _p0.z();}
      
      if(r.max().x()<_p1.x()) {xmax = r.max().x();}
      else {xmax = _p1.x();}
       
      if(r.max().y()<_p1.y()) {ymax = r.max().y();}
      else {ymax = _p1.y();}
      
      if(r.max().z()<_p1.z()) {zmax = r.max().z();}
      else  {zmax = _p1.z();}
      
      return Rectangle(Point3D(xmin, ymin, zmin), Point3D(xmax, ymax, zmax));
    }
  
  return Rectangle(Point3D(1, 0, 0),Point3D(0, 0, 0));
}



#endif
