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

#include <iostream>
#include <math.h>
#include <assert.h>

#include <distributionOps.hpp>
#ifndef PK_BLADE_SPI
#include <mpi.h>
#endif

#ifndef __MATRIX_INDICES_H_
#define __MATRIX_INDICES_H_

#ifndef PKFXLOG_ARRAYBOUNDS
#define PKFXLOG_ARRAYBOUNDS (0)
#endif

#ifndef PKFXLOG_MATRIXINDICES
#define PKFXLOG_MATRIXINDICES (0)
#endif


class MatrixIndices
{
private:
  // stores the data distribution at each phase of the FFT
public:
  DistributionOps ***_Ro;
  DistributionOps ***_Rx;
  DistributionOps ***_Ry;  
  DistributionOps ***_Rz;  

  Distribution3DPoint _PoMin, _PoMax; 
  Distribution3DPoint _PxMin, _PxMax;
  Distribution3DPoint _PyMin, _PyMax;
  Distribution3DPoint _PzMin, _PzMax;
  
  unsigned _pX;
  unsigned _pY;
  unsigned _pZ;
 
  MatrixIndices(unsigned Nx,   unsigned Ny, unsigned Nz, 
		unsigned Px,   unsigned Py, unsigned Pz,
		unsigned myPx, unsigned myPy, unsigned myPz);
  
  ~MatrixIndices();
  inline DistributionOps& getO(unsigned px, unsigned py, unsigned pz);
  inline DistributionOps& getX(unsigned px, unsigned py, unsigned pz);
  inline DistributionOps& getY(unsigned px, unsigned py, unsigned pz);  
  inline DistributionOps getZ(unsigned px, unsigned py, unsigned pz);
  
  inline DistributionOps getO(unsigned px, unsigned py, unsigned pz)const;
  inline DistributionOps getX(unsigned px, unsigned py, unsigned pz)const;
  inline DistributionOps getY(unsigned px, unsigned py, unsigned pz)const;
  //inline DistributionOps getZ(unsigned px, unsigned py, unsigned pz)const;
  
  DistributionOps calcSendOtoZdataDistribution(unsigned myPx, unsigned myPy,
						unsigned myPz, unsigned tgrPx,
						unsigned trgPy, unsigned trgPz);
  
  DistributionOps calcSendZtoYdataDistribution(unsigned myPx,  unsigned myPy,
						unsigned myPz, unsigned tgrPx,
						unsigned trgPy, unsigned trgPz);
  
  DistributionOps calcSendYtoXdataDistribution(unsigned myPx, unsigned myPy,
						unsigned myPz, unsigned trgPx,
						unsigned trgPy, unsigned trgPz);
  
  DistributionOps calcSendXtoOdataDistribution(unsigned myPx, unsigned myPy, 
						unsigned myPz, unsigned trgPx,
						unsigned trgPy, unsigned trgPz);
  
  // calc the portion of my local data I recv from processor [trgPy,trgPz]
  
  DistributionOps calcRecvZfromOdataDistribution(unsigned myPx, unsigned myPy, 
						  unsigned myPz, unsigned trgPx,
						  unsigned trgPy, unsigned trgPz);
  
  DistributionOps calcRecvYfromZdataDistribution(unsigned myPx, unsigned myPy,
						  unsigned myPz, unsigned tgrPx,
						  unsigned trgPy, unsigned trgPz);
  
  DistributionOps calcRecvXfromYdataDistribution(unsigned myPx, unsigned myPy, 
						  unsigned myPz, unsigned trgPx,
						  unsigned trgPy, unsigned trgPz);
  
  DistributionOps calcRecvOfromXdataDistribution(unsigned myPx, unsigned myPy,
						  unsigned myPz, unsigned trgPx,
						  unsigned trgPy, unsigned trgPz);  
  void zeroDataDistributions(DistributionOps ***Rq);
};

#define MINScalar(i, j) ( ((i)<(j))?(i):(j) )

#if defined(FFT_COMPILE) || !defined(FFT_SEPARATE)

void  MatrixIndices::zeroDataDistributions(DistributionOps ***Rq)
{
  Distribution3DPoint point0(1,1,1);
  Distribution3DPoint point1(0,0,0);
  
  for(int px=0; px<_pX; px++)
    {
      for(int py=0; py<_pY; py++)
	{
	  
	  for(int pz=0; pz<_pZ; pz++)
	    {
	      Rq[px][py][pz].setMin(point0);
	      Rq[px][py][pz].setMax(point1);
	      Rq[px][py][pz].setIsEmpty();
	      
	      if(Rq[px][py][pz].isEmpty()!=true) 
		{
		  fprintf(stderr, "Inconsistency: This should be an empty distribution \n");
		}
	    }
	}
    }
}



MatrixIndices::MatrixIndices(unsigned Nx, unsigned Ny, unsigned Nz,
			     unsigned Px, unsigned Py, unsigned Pz,
			     unsigned myPx, unsigned myPy, unsigned myPz)
  :_pX(Px), _pY(Py), _pZ(Pz)
{  
  // Create data distributions for the initial and the 3 compute phases of the 3D-FFT.
  
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
  
  _Ro = new DistributionOps**[_pX];
  _Rx = new DistributionOps**[_pX];
  _Ry = new DistributionOps**[_pX];
  _Rz = new DistributionOps**[_pX];
  
  for(int px=0; px<_pX; px++)
    {
      _Ro[px] = new DistributionOps*[_pY];
      _Ry[px] = new DistributionOps*[_pY];
      _Rz[px] = new DistributionOps*[_pY];
      _Rx[px] = new DistributionOps*[_pY];

      for(int py=0; py<_pY; py++)
	{
	  _Ro[px][py] = new DistributionOps[_pZ];
	  _Ry[px][py] = new DistributionOps[_pZ];
	  _Rz[px][py] = new DistributionOps[_pZ];
	  _Rx[px][py] = new DistributionOps[_pZ];
	}
    }
  // Ni is the global FFT data dimension along a single axis.
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
	      Distribution3DPoint point0(nx*px, ny*py, nz*pz);
	      
	      int p0X1 = nx*(px+1)-1;
	      int pOX2 = Nx -1;

	      int p0Y1 = ny*(py+1)-1;
	      int p0Y2 = Ny-1;
	  
	      int p1Z1 = nz*(pz+1)-1;
	      int p1Z2 = Nz-1;
	      
	      Distribution3DPoint point1(MINScalar(p0X1, pOX2),MINScalar(p0Y1, p0Y2), MINScalar(p1Z1, p1Z2));
	  
	      _Ro[px][py][pz].setMin(point0);
	      _Ro[px][py][pz].setMax(point1);
	      _Ro[px][py][pz].setIsEmpty();
	    }
	}
    }
  
  // Data distribution in the Z-Dim phase  
  int nyx = (nx*ny);
 
  zeroDataDistributions(_Rz);
  int pZstep=( nyx<_pZ)? (_pZ/nyx):1;
 
  // Construct the Z-data distribution
  for(int px=0; px<_pX; px++)
    {
      for(int py=0; py<_pY; py++)
	{
	  for(int trgPz=((px+py)%pZstep); trgPz<_pZ; trgPz += pZstep)
	    {
	      int localTrgPz = trgPz/pZstep;
	      int deltaMinYX = floor((double)(nyx * trgPz/pZstep)/(double)(_pZ/pZstep)); 
	      int deltaMaxYX = floor((double)(nyx * (trgPz/pZstep+1))/(double)(_pZ/pZstep))-1;   
	      
	      int p0x = floor((double)deltaMinYX/(double)ny);
	      int p0y = deltaMinYX%ny;
	      int p0z = 0;
	      
	      // get the lower point of the data distribution by using the 
	      // global indices of the array
	      
	      int p1x = floor((double)deltaMaxYX/(double)ny);
	      int p1y = deltaMaxYX%ny;
	      int p1z = Nz-1;

	      Distribution3DPoint point0(p0x+px*nx, p0y+py*ny, p0z);
	      Distribution3DPoint point1(p1x+px*nx, p1y+py*ny, p1z);
	      
	      _Rz[px][py][trgPz].setMin(point0);
	      _Rz[px][py][trgPz].setMax(point1);
	      _Rz[px][py][trgPz].setIsEmpty();
	    }
	}
    }

  // 
  //  Y distribution
  //
  
  int nxz = (nx*nz);
  zeroDataDistributions(_Ry);
  int pYstep = ( nxz<_pY)? (_pY/nxz) :1;
  // Construct the Y data distribution
  for(int px=0; px<_pX; px++)
    {
      for(int pz=0; pz<_pZ; pz++)
       {
	 for(int trgPy=(px+pz)%pYstep; trgPy<_pY; trgPy += pYstep)
	    {
	      int deltaMinXZ = floor((double)(nxz * (trgPy/pYstep))/(double)(_pY/pYstep)); 
	      int deltaMaxXZ = floor((double)(nxz * ((trgPy/pYstep)+1))/(double)(_pY/pYstep))-1;   
	      
	      int p0z = floor((double)deltaMinXZ/(double)nx);
	      int p0x = deltaMinXZ%nx;
	      int p0y = 0;
	      
	      // get the lower point of the rectangle bx using the 
	      // global indices of the arrax

	      int p1z = floor((double)deltaMaxXZ/(double)nx);
	      int p1x = deltaMaxXZ%nx;
	      int p1y = Ny-1;

	      Distribution3DPoint point0(p0x+px*nx, p0y, p0z+pz*nz);
	      Distribution3DPoint point1(p1x+px*nx, p1y, p1z+pz*nz);
	      
	      _Ry[px][trgPy][pz].setMin(point0);
	      _Ry[px][trgPy][pz].setMax(point1);
	      _Ry[px][trgPy][pz].setIsEmpty();
	    }
       }
    }
   
  // X data Distribution
  // Construct the X data distribution  
  int nyz = (ny*nz);
  int pXstep = (nyz<_pX)?(_pX/nyz):1;
  zeroDataDistributions(_Rx);
  for(int py=0; py<_pY; py++)
    {
      for(int pz=0; pz<_pZ; pz++)
	{
	  for(int trgPx=(py+pz)%pXstep; trgPx<_pX; trgPx+= pXstep)
	    {
	      int deltaMinYZ = floor((double)(nyz * (trgPx/pXstep))/(double)(_pX/pXstep)); 
	      int deltaMaxYZ = floor((double)(nyz * ((trgPx/pXstep)+1))/(double)(_pX/pXstep))-1;   
	      
	      int p0x = 0;
	      int p0y = deltaMinYZ%ny;
	      int p0z = floor((double)deltaMinYZ/(double)ny);
	      
	      // get the lower point of the Z-distribution using the 
	      // global indices of the arraz
	      
	      int p1x = Nx-1;
	      int p1y = deltaMaxYZ%ny;
	      int p1z = floor((double)deltaMaxYZ/(double)ny);
	      
	      Distribution3DPoint point0(p0x, p0y+py*ny, p0z+pz*nz);
	      Distribution3DPoint point1(p1x, p1y+py*ny, p1z+pz*nz);
	      
	      _Rx[trgPx][py][pz].setMin(point0);
	      _Rx[trgPx][py][pz].setMax(point1);
	      _Rx[trgPx][py][pz].setIsEmpty();
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
#endif

DistributionOps& MatrixIndices::
getO(unsigned px, unsigned py, unsigned pz)
{
  assert(px<_pX || py<_pY || pz<_pZ );
  return _Ro[px][py][pz];
}

DistributionOps& MatrixIndices::
getY(unsigned px, unsigned py, unsigned pz) 
{
  assert(px<_pX || py<_pY || pz<_pZ );
  return _Ry[px][py][pz];
}

DistributionOps MatrixIndices::
 getZ(unsigned px, unsigned py, unsigned pz)
{
  assert(px<_pX || py<_pY || pz<_pZ );
  return  DistributionOps(_Rz[px][py][pz]);
}

DistributionOps MatrixIndices::
getO(unsigned px, unsigned py, unsigned pz) const
{
  assert(px<_pX || py<_pY || pz<_pZ );
  return _Ro[px][py][pz];
}
  
DistributionOps MatrixIndices::
getY(unsigned px, unsigned py, unsigned pz)const
{
  assert(px<_pX || py<_pY || pz<_pZ );
  return _Ry[px][py][pz];
}

#if defined(FFT_COMPILE) || !defined(FFT_SEPARATE)

DistributionOps MatrixIndices::
calcSendOtoZdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
 			     unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
	BegLogLine(PKFXLOG_ARRAYBOUNDS)
	  << "calcSendOtoZdataDistribution myPx=" << myPx
	  << " _pX=" << _pX
	  << " trgPx=" << trgPx
	  << " myPy=" << myPy
	  << " _pY=" << _pY
	  << " trgPy=" << trgPy
	  << " myPz=" << myPz
	  << " _pZ=" << _pZ
	  << " trgPz=" << trgPz
	  << EndLogLine ;
	
  assert(myPx<_pX || myPy <_pY || myPz <_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
  
 // intersection of my current distribution with the initial data distribution before FFT is performe     
  return DistributionOps(_Ro[myPx][myPy][myPz].intersect(_Rz[trgPx][trgPy][trgPz]));
}

DistributionOps MatrixIndices::
calcSendZtoYdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			     unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  assert(myPx<_pX || myPy <_pY || myPz <_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
  // intersection of my current distribution with the distribution with the Y data distribution before FFT is performe     
  return DistributionOps(_Rz[myPx][myPy][myPz].intersect(_Ry[trgPx][trgPy][trgPz]));
}

DistributionOps MatrixIndices::
calcSendYtoXdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			     unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  assert(myPx<_pX || myPy <_pY || myPz <_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
  // intersection of my current distribution with the distribution
  // before FFT is perform     
  return DistributionOps(_Ry[myPx][myPy][myPz].intersect(_Rx[trgPx][trgPy][trgPz]));
}

DistributionOps MatrixIndices::
calcSendXtoOdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			     unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  assert(myPx<_pX || myPy <_pY || myPz <_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
  // intersection of my current distribution with the distribution
  // before FFT is performe     
  return DistributionOps(_Rx[myPx][myPy][myPz].intersect(_Ro[trgPx][trgPy][trgPz]));
}


DistributionOps MatrixIndices::
calcRecvZfromOdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			       unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  assert(myPx<_pX || myPy<_pY || myPz<_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
  
  return DistributionOps(_Ro[trgPx][trgPy][trgPz].intersect(_Rz[myPx][myPy][myPz]));
}

DistributionOps MatrixIndices::
calcRecvYfromZdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			       unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  
  assert(myPx<_pX || myPy <_pY || myPz <_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
 
  return DistributionOps(_Rz[trgPx][trgPy][trgPz].intersect(_Ry[myPx][myPy][myPz]));
}

DistributionOps MatrixIndices::
calcRecvXfromYdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			       unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  
  assert(myPx<_pX || myPy <_pY || myPz <_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
 
  return DistributionOps(_Ry[trgPx][trgPy][trgPz].intersect(_Rx[myPx][myPy][myPz]));
}


DistributionOps MatrixIndices::
calcRecvOfromXdataDistribution(unsigned myPx, unsigned myPy,  unsigned myPz,
			       unsigned trgPx, unsigned trgPy, unsigned trgPz)
{
  assert(myPx<_pX || myPy<_pY || myPz<_pZ);
  assert(trgPx<_pX || trgPy<_pY || trgPz<_pZ);
  
  return DistributionOps(_Rx[trgPx][trgPy][trgPz].intersect(_Ro[myPx][myPy][myPz]));
}

#endif


#endif
