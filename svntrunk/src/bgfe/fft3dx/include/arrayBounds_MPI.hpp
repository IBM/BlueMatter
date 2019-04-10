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
/* $Id    : arrayBounds.hpp
   Date   : 07/22/2003  9:24:50 
   Author : Maria Eleftheriou
   Last Modified: 
   By :
   $ */

#include <math.h>
#include <MPI/rectangles_MPI.hpp>
#include <assert.h>
#include <mpi.h>
#include <iostream>

#ifndef __MATRIX_INDICES_H_
#define __MATRIX_INDICES_H_

#ifndef PKFXLOG_MATRIXINDICES
#define PKFXLOG_MATRIXINDICES (0)
#endif

class MatrixIndices
{
private:
  // stores the data distribution at each phase of the FFT
public:
  Rectangle ***_Ro;
  Rectangle ***_Rx;
  Rectangle ***_Ry;  
  Rectangle ***_Rz;  

  Point3D _PoMin, _PoMax; 
  Point3D _PxMin, _PxMax;
  Point3D _PyMin, _PyMax;
  Point3D _PzMin, _PzMax;

  unsigned _pX;
  unsigned _pY;
  unsigned _pZ;
 
  inline MatrixIndices(unsigned Nx,   unsigned Ny, unsigned Nz, 
		unsigned Px,   unsigned Py, unsigned Pz,
		unsigned myPx, unsigned myPy, unsigned myPz);

  inline ~MatrixIndices();
  inline Rectangle& getO(unsigned px, unsigned py, unsigned pz);
  inline Rectangle& getX(unsigned px, unsigned py, unsigned pz);
  inline Rectangle& getY(unsigned px, unsigned py, unsigned pz);
  
  inline Rectangle getZ(unsigned px, unsigned py, unsigned pz);
  
  inline Rectangle getO(unsigned px, unsigned py, unsigned pz)const;
  inline Rectangle getX(unsigned px, unsigned py, unsigned pz)const;
  inline Rectangle getY(unsigned px, unsigned py, unsigned pz)const;

  inline Rectangle calcSendOtoZdataDistribution(unsigned myPx, unsigned myPy,
					 unsigned myPz, unsigned tgrPx,
					 unsigned trgPy, unsigned trgPz);
  
  inline Rectangle calcSendZtoYdataDistribution(unsigned myPx,  unsigned myPy,
					 unsigned myPz, unsigned tgrPx,
					 unsigned trgPy, unsigned trgPz);
   
  inline Rectangle calcSendYtoXdataDistribution(unsigned myPx, unsigned myPy,
					 unsigned myPz, unsigned trgPx,
					 unsigned trgPy, unsigned trgPz);

  inline Rectangle calcSendXtoOdataDistribution(unsigned myPx, unsigned myPy, 
					 unsigned myPz, unsigned trgPx,
					 unsigned trgPy, unsigned trgPz);
  
   // calc the portion of my local data I recv from processor [trgPy,trgPz]
 
  inline Rectangle calcRecvZfromOdataDistribution(unsigned myPx, unsigned myPy, 
					   unsigned myPz, unsigned trgPx,
					   unsigned trgPy, unsigned trgPz);
  
  inline Rectangle calcRecvYfromZdataDistribution(unsigned myPx, unsigned myPy,
					   unsigned myPz, unsigned tgrPx,
					   unsigned trgPy, unsigned trgPz);
  
  inline Rectangle calcRecvXfromYdataDistribution(unsigned myPx, unsigned myPy, 
					   unsigned myPz, unsigned trgPx,
					   unsigned trgPy, unsigned trgPz);
  
  inline Rectangle calcRecvOfromXdataDistribution(unsigned myPx, unsigned myPy,
					   unsigned myPz, unsigned trgPx,
					   unsigned trgPy, unsigned trgPz);  
};

#define MIN(i, j) ( ((i)<(j))?(i):(j) )

MatrixIndices::MatrixIndices(unsigned Nx, unsigned Ny, unsigned Nz,
			     unsigned Px, unsigned Py, unsigned Pz,
			     unsigned myPx, unsigned myPy, unsigned myPz)
  :_pX(Px), _pY(Py), _pZ(Pz)
{  
  BegLogLine(PKFXLOG_MATRIXINDICES) 
    << "MatrixIndices::MatrixIndices Nx=" << Nx
    << " Ny=" << Ny
    << " Nz=" << Nz
    << " Px=" << Px
    << " Py=" << Py
    << " Pz=" << Pz
    << " myPx=" << myPx
    << " myPy=" << myPy
    << " myPz=" << myPz
    << EndLogLine ;
  _Ro = new Rectangle**[_pX];
  _Rx = new Rectangle**[_pX];
  _Ry = new Rectangle**[_pX];
  _Rz = new Rectangle**[_pX];
  
  for(int px=0; px<_pX; px++)
    {
      _Ro[px] = new Rectangle*[_pY];
      _Ry[px] = new Rectangle*[_pY];
      _Rz[px] = new Rectangle*[_pY];
      _Rx[px] = new Rectangle*[_pY];

      for(int py=0; py<_pY; py++)
	{
	  _Ro[px][py] = new Rectangle[_pZ];
	  _Ry[px][py] = new Rectangle[_pZ];
	  _Rz[px][py] = new Rectangle[_pZ];
	  _Rx[px][py] = new Rectangle[_pZ];
	}
    }
  
  unsigned localNx = (unsigned) ceil((double)Nx/(double)_pX);
  unsigned localNy = (unsigned) ceil((double)Ny/(double)_pY);
  unsigned localNz = (unsigned) ceil((double)Nz/(double)_pZ);

  unsigned nx = localNx;
  unsigned ny = localNy;
  unsigned nz = localNz;

  for(int px=0; px<_pX; px++)
    {
      ny = localNy;
      for(int py=0; py<_pY; py++)
	{
	  nz = localNz;
	  for(int pz=0; pz<_pZ; pz++)
	    {
	      Point3D point0(nx*px, ny*py, nz*pz);
	      
	      int p0X1 = nx*(px+1)-1;
	      int pOX2 = Nx -1;

	      int p0Y1 = ny*(py+1)-1;
	      int p0Y2 = Ny-1;
	  
	      int p1Z1 = nz*(pz+1)-1;
	      int p1Z2 = Nz-1;
	      
	      Point3D point1(MIN(p0X1, pOX2),MIN(p0Y1, p0Y2), MIN(p1Z1, p1Z2));
	  
	      _Ro[px][py][pz].setMin(point0);
	      _Ro[px][py][pz].setMax(point1);
	      _Ro[px][py][pz].setIsEmpty();

	    }
	}
    }
  
  // Data distribution in the Z-Dim phase  
  int nyx = (nx*ny);
  
  for(int px=0; px<_pX; px++)
    {
      for(int py=0; py<_pY; py++)
	{
	  for(int trgPz=0; trgPz<_pZ; trgPz++)
	    {
	      int deltaMinYX = floor((double)(nyx * trgPz)/(double)_pZ); 
	      int deltaMaxYX = floor((double)(nyx * (trgPz+1))/(double)_pZ)-1;   
	      int p0x = floor((double)deltaMinYX/(double)ny);
	      int p0y = deltaMinYX%ny;
	      int p0z = 0;

	      // get the lower point of the rectangle by using the 
	      // global indices of the array

	      int p1x = floor((double)deltaMaxYX/(double)ny);
	      int p1y = deltaMaxYX%ny;
	      int p1z = Nz-1;

	      Point3D point0(p0x+px*nx, p0y+py*ny, p0z);
	      Point3D point1(p1x+px*nx, p1y+py*ny, p1z);
	      
	      _Rz[px][py][trgPz].setMin(point0);
	      _Rz[px][py][trgPz].setMax(point1);
	      _Rz[px][py][trgPz].setIsEmpty();

	      int myRank;
#ifdef ARRAY_DBG 
	      MPI_Comm_rank( Platform::Topology::cart_comm, &myRank);
 	      if(myRank==0)
 		_Rz[px][py][trgPz].Print(px,py,trgPz);
#endif
 	    }
	}
    }

  //  Y distribution

  int nxz = (nx*nz);
  
  for(int px=0; px<_pX; px++)
    {
      for(int trgPy=0; trgPy<_pY; trgPy++)
	{
	  for(int pz=0; pz<_pZ; pz++)
	    {
	      int deltaMinXZ = floor((double)(nxz * trgPy)/(double)_pY); 
	      int deltaMaxXZ = floor((double)(nxz * (trgPy+1))/(double)_pY)-1;   
	      int p0z = floor((double)deltaMinXZ/(double)nx);
	      int p0x = deltaMinXZ%nx;
	      int p0y = 0;

	      // get the lower point of the rectangle bx using the 
	      // global indices of the arrax

	      int p1z = floor((double)deltaMaxXZ/(double)nx);
	      int p1x = deltaMaxXZ%nx;
	      int p1y = Ny-1;

	      Point3D point0(p0x+px*nx, p0y, p0z+pz*nz);
	      Point3D point1(p1x+px*nx, p1y, p1z+pz*nz);
	      
	      _Ry[px][trgPy][pz].setMin(point0);
	      _Ry[px][trgPy][pz].setMax(point1);
	      _Ry[px][trgPy][pz].setIsEmpty();
#ifdef ARRAY_DBG 
	      int myRank;
	      MPI_Comm_rank( Platform::Topology::cart_comm, &myRank );
	      if(myRank==0)
 		_Ry[px][trgPy][pz].Print(px,trgPy, pz);
#endif     
	    }
	}
    }

  // X data Distribution
  // Data distribution in the X-Dim phase  
  int nyz = (ny*nz);
  for(int trgPx=0; trgPx<_pX; trgPx++)
    {
      for(int py=0; py<_pY; py++)
	{
	  for(int pz=0; pz<_pZ; pz++)
	    {
	      int deltaMinYZ = floor((double)(nyz * trgPx)/(double)_pX); 
	      int deltaMaxYZ = floor((double)(nyz * (trgPx+1))/(double)_pX)-1;   
	      int p0x = 0;
	      int p0y = deltaMinYZ%ny;
	      int p0z = floor((double)deltaMinYZ/(double)ny);
	      
	      // get the lower point of the rectangle bz using the 
	      // global indices of the arraz
	      
	      int p1x = Nx-1;
	      int p1y = deltaMaxYZ%ny;
	      int p1z = floor((double)deltaMaxYZ/(double)ny);
	      
	      Point3D point0(p0x, p0y+py*ny, p0z+pz*nz);
	      Point3D point1(p1x, p1y+py*ny, p1z+pz*nz);
	      
	      _Rx[trgPx][py][pz].setMin(point0);
	      _Rx[trgPx][py][pz].setMax(point1);
	      _Rx[trgPx][py][pz].setIsEmpty();
#ifdef ARRAY_DBG 	      
	      int myRank;
 	      MPI_Comm_rank( Platform::Topology::cart_comm, &myRank);
 	      if(myRank==0)
 		_Rx[trgPx][py][pz].Print(trgPx, py, pz);
#endif
	    }
	}
    }
  
}


MatrixIndices::~MatrixIndices()
{

  for(int py=0; py<_pY; py++)
    {
      for(int pz=0; pz<_pZ; pz++)
	{
	  delete [] _Ro[py][pz];
	  delete [] _Ry[py][pz];
	  delete [] _Rz[py][pz];
	}
      delete [] _Ro[py];
      delete [] _Ry[py];
      delete [] _Rz[py];
    }
  
  delete [] _Ro;
  delete [] _Ry;
  delete [] _Rz;
}

Rectangle& MatrixIndices::
getO(unsigned px, unsigned py, unsigned pz)
{
  assert(px<_pX || py<_pY || pz<_pZ );
  return _Ro[px][py][pz];
}

Rectangle& MatrixIndices::
getY(unsigned px, unsigned py, unsigned pz) 
{
  assert(px<_pX || py<_pY || pz<_pZ );
  return _Ry[px][py][pz];
}

Rectangle MatrixIndices::
 getZ(unsigned px, unsigned py, unsigned pz)
{
  assert(px<_pX || py<_pY || pz<_pZ );
  return  Rectangle(_Rz[px][py][pz]);
}

Rectangle MatrixIndices::
getO(unsigned px, unsigned py, unsigned pz) const
{
  assert(px<_pX || py<_pY || pz<_pZ );
  return _Ro[px][py][pz];
}
  
Rectangle MatrixIndices::
getY(unsigned px, unsigned py, unsigned pz)const
{
  assert(px<_pX || py<_pY || pz<_pZ );
  return _Ry[px][py][pz];
}

Rectangle MatrixIndices::
calcSendOtoZdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
 			     unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  assert(myPx<_pX || myPy <_pY || myPz <_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
  
 // intersection of my current distribution with the distribution
 // before FFT is performe     
  return Rectangle(_Ro[myPx][myPy][myPz].intersect(_Rz[trgPx][trgPy][trgPz]));
}

Rectangle MatrixIndices::
calcSendZtoYdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			     unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  assert(myPx<_pX || myPy <_pY || myPz <_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
  // intersection of my current distribution with the distribution
  // before FFT is performe     
  return Rectangle(_Rz[myPx][myPy][myPz].intersect(_Ry[trgPx][trgPy][trgPz]));
}

Rectangle MatrixIndices::
calcSendYtoXdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			     unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  assert(myPx<_pX || myPy <_pY || myPz <_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
  // intersection of my current distribution with the distribution
  // before FFT is perform     
  return Rectangle(_Ry[myPx][myPy][myPz].intersect(_Rx[trgPx][trgPy][trgPz]));
}

Rectangle MatrixIndices::
calcSendXtoOdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			     unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  assert(myPx<_pX || myPy <_pY || myPz <_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
  // intersection of my current distribution with the distribution
  // before FFT is performe     
  return Rectangle(_Rx[myPx][myPy][myPz].intersect(_Ro[trgPx][trgPy][trgPz]));
}


Rectangle MatrixIndices::
calcRecvZfromOdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			       unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  assert(myPx<_pX || myPy<_pY || myPz<_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
  
  return Rectangle(_Ro[trgPx][trgPy][trgPz].intersect(_Rz[myPx][myPy][myPz]));
}

Rectangle MatrixIndices::
calcRecvYfromZdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			       unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  
  assert(myPx<_pX || myPy <_pY || myPz <_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
 
  return Rectangle(_Rz[trgPx][trgPy][trgPz].intersect(_Ry[myPx][myPy][myPz]));
}

Rectangle MatrixIndices::
calcRecvXfromYdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			       unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  
  assert(myPx<_pX || myPy <_pY || myPz <_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
 
  return Rectangle(_Ry[trgPx][trgPy][trgPz].intersect(_Rx[myPx][myPy][myPz]));
}


Rectangle MatrixIndices::
calcRecvOfromXdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			       unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  assert(myPx<_pX || myPy<_pY || myPz<_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
  
  return Rectangle(_Rx[trgPx][trgPy][trgPz].intersect(_Ro[myPx][myPy][myPz]));
}


#endif
