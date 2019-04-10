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
  /* $Id    : fft3D.hpp
   Date   : 07/23/2003  11:41:51 
   Name   : Maria Eleftheriou, IBM T.J.W R.C.
   Last Modified: 
   By     :
*/

/**
 *  \addtogroup FFTS
 *  The FFT class computes the complex DFT of a 3D array. The data is
 *  distributed on a 3D processor mesh. It is templated on the type 
 *  of 1D FFT. The 1D FFT performs the 1D FFTs locally on each node.
 **/

// TO_DO
// need to write the _strideSrc and Trg
// test the code.

#ifndef __FFT3D_H_
#define __FFT3D_H_
#define FORWARD -1
#define REVERSE 1

#include <iostream>
#include "mpi.h"
#include <MPI/fft_one_dim_MPI.hpp>
#include <MPI/comm3DLayer_MPI.hpp>
#include <MPI/complex_MPI.hpp>
#include <MPI/rectangles_MPI.hpp>
#include <MPI/arrayBounds_MPI.hpp>

#define MAX(i, j) ( (i>j)?i:j )

template<class FFT_PLAN, int FWD_REV>
class FFT3D 
{
  enum{DIM=4};
  enum{DIR_OZ, DIR_ZY, DIR_YX, DIR_XO};

private:
  Comm3DLayer *_comm;
  FFTOneD *_fft[DIM-1];
 
  // data related   
  int _fftDir;
  int _strideXYZ[DIM-1];
  int _strideYXZ[DIM-1];
  int _strideZXY[DIM-1];
  int _strideYZX[DIM-1];

  int _strideR0, _strideR1, _strideR2;

  MPI_Comm _cart_comm;
  int _nFFTs[DIM];
  complex *_input;
  complex *_output;
  
  int _localNx[2*DIM];
  int _localNy[2*DIM];
  int _localNz[2*DIM];
  int _localPx[2*DIM], _localPy[2*DIM],_localPz[2*DIM];
 
  int _strideTrgA[DIM][DIM-1];
  int _strideSrcA[DIM][DIM-1];
  int _localSizeA[DIM][DIM-1];

  int _totalLocalSize[DIM];
  int ***_trgIndex[DIM-1];
  complex* _tmpSrc;
  complex* _tmpTrg;

  int* _sendCount[DIM];
  int* _recvCount[DIM];

  int* _sendDisp[DIM];
  int* _recvDisp[DIM];
 
  int* _sendCount_R[DIM];
  int* _recvCount_R[DIM];

  int* _sendDisp_R[DIM];
  int* _recvDisp_R[DIM];
 
  Rectangle** _sendIndex[DIM];
  Rectangle** _recvIndex[DIM];

  MatrixIndices *_ab;
  int _pX; int _pY; int _pZ;
  int _myPx; int _myPy; int _myPz,_myRank, _globalID;

  complex *_localDataIn;
  complex *_localDataTmp1;
  complex *_localDataTmp2;
  complex *_localDataOut;

  // private functions
  void AllocateMemory();
  void DeallocateMemory();
  void SetSendRecvIndecies();
  void DoLocalMapping(Point3D &pointMin, Point3D &pointMax, int x, int y, int z);

  void calcLocalIndexOZ(int dir);
  void calcLocalIndexZY(int dir);
  void calcLocalIndexYX(int dir);
  void calcLocalIndexXO(int dir);

  void ReorderData(int dir, int *srcStride, int *trgStride, int *localSize,
       complex* localDataSrc, complex* localDataTrg);
  void ReorderDoFFTs(int dir, int *srcStride, int *trgStride, int *localSize,
         complex* localDataSrc, complex* localDataTrg);

  void ReorderDoFFTs_R(int dir, int *srcStride, int *trgStride, int *localSize,
         complex* localDataSrc, complex* localDataTrg);
  
  inline void Redistribute(int dir, complex *localDataSrc, complex *localDataTrg);
  inline void Redistribute_R(int dir, complex *localDataSrc, complex *localDataTrg);

  void SetAllToAllIndexInPlane(int aDir, int srcP, int trgP);
  void SetAllToAllIndexInPlane2(int aDir, int srcP, int trgP);
  void Plan();

public:  

  typedef FFT_PLAN FFT_PLAN_IF;

  enum { P_X = FFT_PLAN::P_X };
  enum { P_Y = FFT_PLAN::P_Y };
  enum { P_Z = FFT_PLAN::P_Z };
  
  enum { GLOBAL_N_X = FFT_PLAN::GLOBAL_SIZE_X };
  enum { GLOBAL_N_Y = FFT_PLAN::GLOBAL_SIZE_Y };
  enum { GLOBAL_N_Z = FFT_PLAN::GLOBAL_SIZE_Z };
  
  enum { LOCAL_N_X = (GLOBAL_N_X / P_X) };
  enum { LOCAL_N_Y = (GLOBAL_N_Y / P_Y) };
  enum { LOCAL_N_Z = (GLOBAL_N_Z / P_Z) };
  
  void Init(FFT3D** ptr);
  void Init( int localNx,int localNy, int localNz, int pX, int pY, int pZ);
  void Finalize();

  inline void ZeroRealSpace()
  {
    int localSize = _localNx[0]*_localNy[0]*_localNz[0];

    for(int index = 0; index<localSize; index++)
      {
        _localDataIn[0].re = 0.0;
        _localDataIn[0].im = 0.0;
      }
  }

  inline double GetRealSpaceElement( int x, int y, int z );
  inline void PutRealSpaceElement( int x, int y, int z, double d );
  inline complex GetRecipSpaceElement( int kx, int ky, int kz );

  inline void PutRecipSpaceElement( int kx, int ky, int kz,complex value)
  {
    int index           = _strideR0*kx + _strideR1*ky + _strideR2*kz;
    _localDataIn[index] = value; 
  }

  inline void GetLocalDimensions( int& aLocalX, int& aLocalSizeX,
          int& aLocalY, int& aLocalSizeY,
          int& aLocalZ, int& aLocalSizeZ,
          int& aLocalRecipX, int& aLocalRecipSizeX,
          int& aLocalRecipY, int& aLocalRecipSizeY,
          int& aLocalRecipZ, int& aLocalSizeRecipZ );
  
  void DoFFT();
  //  For debugging
  void PrintVector(complex* data, int size, int myGlobalID,  int dir)
    {
      
      MPI_Comm comm;int dim=3;
      int coords[3]; //{-1,-1, -1};
      
      comm=(*_comm)._comm[dir];
      
      if(dir==0 || dir==3){ dim=1;} else {dim=2;} 
      
      MPI_Cart_coords(comm, myGlobalID, dim, coords);
      
      for(int i=0; i<size; i++)
  printf("Array[%d][ %d->(%d %d %d)][ %d] = %lf \n",dir, 
         myGlobalID, coords[0], coords[1], coords[2],i, data[i].re);
      
      MPI_Barrier(_cart_comm);
  }
};


template< class FFT_PLAN, int FWD_REV >
void FFT3D< FFT_PLAN, FWD_REV >::PutRealSpaceElement(int x, int y, int z,
                 double realInput)
{
  int index =  x*_strideXYZ[0]+y*_strideXYZ[1]+z*_strideXYZ[2];
  _localDataIn[index].re = realInput;
  _localDataIn[index].im = 0.;
}

template< class FFT_PLAN, int FWD_REV>
double FFT3D< FFT_PLAN, FWD_REV >::
GetRealSpaceElement( int x, int y, int z )
{
  if(FWD_REV == FORWARD)
    {
      int index = x*_strideXYZ[0]+y*_strideXYZ[1]+z*_strideXYZ[2];
      return _localDataIn[index].re;
    }
  else
    {
      //   int index = x*_strideR0+y*_strideR1+z*_strideR2;
      
      int index = x*_strideXYZ[0]+y*_strideXYZ[1]+z*_strideXYZ[2];
      assert(index <_localNx[0]*_localNy[0]*_localNz[0]);      
      return _localDataOut[index].re;
    }

}

template< class FFT_PLAN, int FWD_REV >
complex FFT3D< FFT_PLAN, FWD_REV >::
GetRecipSpaceElement( int kx, int ky, int kz )
{
  if(FWD_REV == FORWARD)
    {
      int index =kx*_strideR0+ky*_strideR1+kz*_strideR2;
      return _localDataOut[index];
    }
   
  // int index = kx*_strideXYZ[0]+ky*_strideXYZ[1]+kz*_strideXYZ[2];
       int index =kx*_strideR0+ky*_strideR1+kz*_strideR2;
 return _localDataIn[index];
}


template< class FFT_PLAN, int FWD_REV>
inline void FFT3D< FFT_PLAN, FWD_REV >::
GetLocalDimensions( int& aLocalX,      int& aLocalSizeX,
        int& aLocalY,      int& aLocalSizeY,
        int& aLocalZ,      int& aLocalSizeZ,
        int& aLocalRecipX, int& aLocalRecipSizeX,
        int& aLocalRecipY, int& aLocalRecipSizeY,
        int& aLocalRecipZ, int& aLocalRecipSizeZ )
{
  aLocalX = _myPx*_localNx[0];   aLocalSizeX = _localNx[0];
  aLocalY = _myPy*_localNy[0];   aLocalSizeY = _localNy[0];
  aLocalY = _myPz*_localNz[0];   aLocalSizeZ = _localNz[0];
  
  aLocalRecipX     = 0; 
  aLocalRecipSizeX = _localNx[0]*_pX;
  
  //returns the global indecies w aLocalRecip representing the first elm
  // check this is not assume of partitioning the y direction at all. 
  
  aLocalRecipY     = _localNy[3]*_myPy; 
  aLocalRecipSizeY = _localNy[3];
  aLocalRecipZ     = _localNz[3]*_myPx+_localNz[3]*_pX*_myPz;
  aLocalRecipSizeZ = _localNz[3];
}

template< class FFT_PLAN, int FWD_REV>
void  FFT3D<FFT_PLAN, FWD_REV>::Plan() 
{
  _fft[2] = new  FFTOneD(_localNx[0]*_pX, FWD_REV);  // 1D fft in the x-dim
  _fft[1] = new  FFTOneD(_localNy[0]*_pY, FWD_REV);  // 1D fft in the y-dim
  _fft[0] = new  FFTOneD(_localNz[0]*_pZ, FWD_REV);  // 1D fft in the y-dim

  _ab = new MatrixIndices(_localNx[0]*_pX, 
        _localNy[0]*_pY,
        _localNz[0]*_pZ,
        _pX,    _pY,  _pZ,
        _myPx, _myPy, _myPz); 
  
  int size = MAX(MAX(_pX,_pY),_pZ);
      
  for(int dir=0; dir<DIM; dir++)
    {
      // need to optimize the max_size by exact size
      _sendIndex[dir]  = new Rectangle*[size];
      _recvIndex[dir]  = new Rectangle*[size];
      
      for(int p=0; p<size; p++)
  {
    _sendIndex[dir][p]  = new Rectangle[size];
    _recvIndex[dir][p]  = new Rectangle[size];
  }
    }
}


template< class FFT_PLAN, int FWD_REV>
void  FFT3D<FFT_PLAN, FWD_REV>::DoLocalMapping(Point3D &pointMin,
                 Point3D &pointMax, 
                 int x, int y, int z)
{
  // if not send to a spefic node then your rectangle is empty w coords
  // (1,1,1) (0,0,0)

  if(pointMax.x()-pointMin.x()<0 ||
     pointMax.y()-pointMin.y()<0 ||
     pointMax.z()-pointMin.z()<0)
    {
      pointMin.setX(1); pointMin.setY(1); pointMin.setZ(1);
      pointMax.setX(0); pointMax.setY(0); pointMax.setZ(0);
      return;
    }

  int xMin = pointMin.x() - _myPx*x;
  int yMin = pointMin.y() - _myPy*y;
  int zMin = pointMin.z() - _myPz*z;
 
  int xMax = pointMax.x() - _myPx*x;
  int yMax = pointMax.y() - _myPy*y;
  int zMax = pointMax.z() - _myPz*z;
 
  pointMin.setX(xMin); pointMin.setY(yMin); pointMin.setZ(zMin);
  pointMax.setX(xMax); pointMax.setY(yMax); pointMax.setZ(zMax);
}

  
template< class FFT_PLAN, int FWD_REV>
void  FFT3D<FFT_PLAN, FWD_REV>::calcLocalIndexOZ(int dir)
{
  assert(dir==DIR_OZ);
  int sumRecvX = 0;  int sumRecvY = 0; int sumRecvZ = 0;
  int myRank;

  MPI_Comm_rank(_cart_comm, &myRank);

  for(int trgPz=0; trgPz<_pZ; trgPz++)
    {
      //get global send indecies
      _sendIndex[dir][0][trgPz] = (*_ab).calcSendOtoZdataDistribution
  (_myPx, _myPy, _myPz, _myPx, _myPy, trgPz);
      
      Point3D sendMin = _sendIndex[dir][0][trgPz].min();
      Point3D sendMax = _sendIndex[dir][0][trgPz].max();
      
      DoLocalMapping(sendMin, sendMax, _localNx[0], _localNy[0], _localNz[0]);

      _sendIndex[dir][0][trgPz].setMin(sendMin);
      _sendIndex[dir][0][trgPz].setMax(sendMax);

      _recvIndex[dir][0][trgPz] = (*_ab).calcRecvZfromOdataDistribution
  (_myPx, _myPy, _myPz, _myPx, _myPy, trgPz);  
    
      Point3D recvMin = _recvIndex[dir][0][trgPz].min();
      Point3D recvMax = _recvIndex[dir][0][trgPz].max();  
      
       DoLocalMapping(recvMin, recvMax,_localNx[0], _localNy[0], 0);
     
      _recvIndex[dir][0][trgPz].setMin(recvMin);
      _recvIndex[dir][0][trgPz].setMax(recvMax);
      
      sumRecvX += (recvMax.x()-recvMin.x())+1;  
      sumRecvY += (recvMax.y()-recvMin.y())+1;  
      sumRecvZ += (recvMax.z()-recvMin.z())+1;  
    }  

  // set local sizes
  _localNx[dir+1] = sumRecvX/_pZ;
  _localNy[dir+1] = sumRecvY/_pZ;
  _localNz[dir+1] = sumRecvZ;
  _localPx[dir]=1;  _localPy[dir]=1;  _localPz[dir]=1;

  _localPx[dir+1] = 1;
  _localPy[dir+1] = 1;
  _localPz[dir+1] = _pZ;

  Point3D tmpMin = (*_ab)._Rz[_myPx][_myPy][_myPz].min();
  Point3D tmpMax = (*_ab)._Rz[_myPx][_myPy][_myPz].max();
  
  assert(_localNx[dir+1] == (tmpMax.x()-tmpMin.x()+1) &&
   _localNy[dir+1] == (tmpMax.y()-tmpMin.y()+1) &&
   _localNz[dir+1] == (tmpMax.z()-tmpMin.z()+1));
}


template<class FFT_PLAN, int FWD_REV>
void FFT3D<FFT_PLAN, FWD_REV>::calcLocalIndexZY(int dir)
{
  assert(dir==DIR_ZY);
  int myRank;
  MPI_Comm_rank(_cart_comm, &myRank);

  int sumRecvX = 0; int sumRecvY = 0; int sumRecvZ = 0;
  for(int trgPy=0; trgPy<_pY; trgPy++)
    {
      for(int trgPz=0; trgPz<_pZ; trgPz++)
  {
    _sendIndex[dir][trgPz][trgPy] = 
      (*_ab).calcSendZtoYdataDistribution(_myPx, _myPy, _myPz,
            _myPx, trgPy, trgPz);      
    
    Point3D sendMin = _sendIndex[dir][trgPz][trgPy].min();
    Point3D sendMax = _sendIndex[dir][trgPz][trgPy].max();
    
    DoLocalMapping(sendMin, sendMax, _localNx[0], _localNy[0], 0);
    
    _sendIndex[dir][trgPz][trgPy].setMin(sendMin);
    _sendIndex[dir][trgPz][trgPy].setMax(sendMax);
    
    _recvIndex[dir][trgPz][trgPy] = 
      (*_ab).calcRecvYfromZdataDistribution
      (_myPx, _myPy, _myPz, _myPx, trgPy, trgPz);
    
    Point3D recvMin = _recvIndex[dir][trgPz][trgPy].min();
    Point3D recvMax = _recvIndex[dir][trgPz][trgPy].max();
    
    DoLocalMapping(recvMin, recvMax, _localNx[0], 0, _localNz[0]);

    _recvIndex[dir][trgPz][trgPy].setMin(recvMin);
    _recvIndex[dir][trgPz][trgPy].setMax(recvMax);
    
    sumRecvX += recvMax.x()-recvMin.x()+1;  
    sumRecvY += recvMax.y()-recvMin.y()+1;  
    sumRecvZ += recvMax.z()-recvMin.z()+1;  
  }
    }
 
  
  Point3D *tmpMin = &((*_ab)._Ry[_myPx][_myPy][_myPz].min());
  Point3D *tmpMax = &((*_ab)._Ry[_myPx][_myPy][_myPz].max());

  _localNx[dir+1] = (tmpMax->x()-tmpMin->x()+1);
  _localNy[dir+1] = (tmpMax->y()-tmpMin->y()+1);
  _localNz[dir+1] = (tmpMax->z()-tmpMin->z()+1);
  
  if(_localNx[dir+1]<=0) _localNx[dir+1]=1;
  if(_localNy[dir+1]<=0) _localNy[dir+1]=1;
  if(_localNz[dir+1]<=0) _localNz[dir+1]=1;

  assert(_localNx[dir+1]>0 &&
   _localNy[dir+1]>0 &&
   _localNz[dir+1]>0);

  _localPx[dir+1] = 1;
  _localPy[dir+1] = _pY;
  _localPz[dir+1] = _pZ;
}


template< class FFT_PLAN, int FWD_REV>
void FFT3D<FFT_PLAN, FWD_REV>::calcLocalIndexYX(int dir)
{
  assert(dir == DIR_YX);
  int sumRecvX = 0; int sumRecvY = 0; int sumRecvZ = 0;
  int myRank;
  MPI_Comm_rank(_cart_comm, &myRank);

  for(int trgPx=0; trgPx<_pX; trgPx++)
    for(int trgPy=0; trgPy<_pY; trgPy++)
      {
  _sendIndex[dir][trgPy][trgPx] =
    (*_ab).calcSendYtoXdataDistribution
    (_myPx, _myPy, _myPz, trgPx, trgPy, _myPz);
  
  Point3D sendMin = _sendIndex[dir][trgPy][trgPx].min();
  Point3D sendMax = _sendIndex[dir][trgPy][trgPx].max();
  
        DoLocalMapping(sendMin, sendMax,_localNx[0], 0, _localNz[0]);
  
  _sendIndex[dir][trgPy][trgPx].setMin(sendMin);
  _sendIndex[dir][trgPy][trgPx].setMax(sendMax);
  
  _recvIndex[dir][trgPy][trgPx] = 
    (*_ab).calcRecvXfromYdataDistribution
    (_myPx, _myPy, _myPz, trgPx, trgPy, _myPz);
  
  Point3D recvMin = _recvIndex[dir][trgPy][trgPx].min();
  Point3D recvMax = _recvIndex[dir][trgPy][trgPx].max();

  DoLocalMapping(recvMin, recvMax, 0, _localNy[0], _localNz[0]);	

  _recvIndex[dir][trgPy][trgPx].setMin(recvMin);
  _recvIndex[dir][trgPy][trgPx].setMax(recvMax);
  //_recvIndex[dir][trgPy][trgPx].Print(_myPz+(_myPy+_myPx*_pY)*_pZ);

  sumRecvX += recvMax.x()-recvMin.x()+1;  
  sumRecvY += recvMax.y()-recvMin.y()+1;  
  sumRecvZ += recvMax.z()-recvMin.z()+1;  
      }
 
  Point3D tmpMin = ((*_ab)._Rx[_myPx][_myPy][_myPz].min());
  Point3D tmpMax = ((*_ab)._Rx[_myPx][_myPy][_myPz].max());
      
  _localNx[dir+1] = (tmpMax.x()-tmpMin.x()+1);
  _localNy[dir+1] = (tmpMax.y()-tmpMin.y()+1);
  _localNz[dir+1] = (tmpMax.z()-tmpMin.z()+1);
  
  //check
  if(_localNx[dir+1]<=0) _localNx[dir+1]=1;
  if(_localNy[dir+1]<=0) _localNy[dir+1]=1;
  if(_localNz[dir+1]<=0) _localNz[dir+1]=1;
  assert(_localNx[dir+1]>0 &&
   _localNy[dir+1]>0 &&
   _localNz[dir+1]>0);  
}


template< class FFT_PLAN, int FWD_REV>
void FFT3D<FFT_PLAN, FWD_REV>::SetSendRecvIndecies()
{
  calcLocalIndexOZ(DIR_OZ);
  calcLocalIndexZY(DIR_ZY);
  calcLocalIndexYX(DIR_YX);

  _nFFTs[0] = (_localNx[0]*_localNy[0])/_pZ;
  _nFFTs[1] = (_localNx[0]*_localNz[0])/_pY;
  _nFFTs[2] = (_localNz[0]*_localNy[0])/_pX;
}

template<class FFT_PLAN, int FWD_REV>
void FFT3D<FFT_PLAN, FWD_REV>::SetAllToAllIndexInPlane(int aDir, int srcP, int trgP)
{
  int dir = aDir;
  int xMin, xMax, yMin, yMax, zMin, zMax;
  int maxDim = 3;
  int coords[2];
  int rank;
  int commSize;
  
  // this function returns the coords of the sub communicator.
  
  int sendCount = 0;
  int sendDisp  = 0;
  
  for(int trgP1=0; trgP1<srcP; trgP1++)    
    for(int trgP0=0; trgP0<trgP; trgP0++)        
      {
  xMin = _sendIndex[dir][trgP1][trgP0].min().x();
  xMax = _sendIndex[dir][trgP1][trgP0].max().x();
  
  yMin = _sendIndex[dir][trgP1][trgP0].min().y();
  yMax = _sendIndex[dir][trgP1][trgP0].max().y();
  
  zMin = _sendIndex[dir][trgP1][trgP0].min().z();
  zMax = _sendIndex[dir][trgP1][trgP0].max().z();
  
  coords[0] = trgP0;
  coords[1] = trgP1;
  
        (*_comm).getSubRank(dir%(DIM+1), coords, &rank);	 
  sendDisp += sendCount;
  
  _sendDisp[dir][rank]    = sendDisp;
  _sendDisp_R[dir][rank]  = sendDisp;
  sendCount               = 2*(xMax-xMin+1) * (yMax-yMin+1) * (zMax-zMin+1);
  assert(sendCount >= 0);
  _sendCount[dir][rank]   = sendCount;
  _sendCount_R[dir][rank] = sendCount;
      }
  
  int recvCount = 0;
  int recvDisp  = 0;
  
  for(int srcP0=0; srcP0<trgP; srcP0++)        	
    for(int srcP1=0; srcP1<srcP; srcP1++)       
      {
  xMin = _recvIndex[dir][srcP1][srcP0].min().x();
  xMax = _recvIndex[dir][srcP1][srcP0].max().x();
  
  yMin = _recvIndex[dir][srcP1][srcP0].min().y();
  yMax = _recvIndex[dir][srcP1][srcP0].max().y();
  
  zMin = _recvIndex[dir][srcP1][srcP0].min().z();
  zMax = _recvIndex[dir][srcP1][srcP0].max().z();
  
  int rank;
  
  coords[0] = srcP0;
  coords[1] = srcP1;
    
  (*_comm).getSubRank(dir%(DIM+1), coords, &rank);
  
  recvDisp               += recvCount;
  _recvDisp[dir][rank]   += recvDisp;
  _recvDisp_R[dir][rank] += recvDisp;

  recvCount               = 2*(xMax-xMin+1)*(yMax-yMin+1)*(zMax-zMin+1);
  _recvCount[dir][rank]   = recvCount;
  _recvCount_R[dir][rank] = recvCount;	
      }
  
  return;
}
  

template<class FFT_PLAN, int FWD_REV>
void FFT3D<FFT_PLAN, FWD_REV>::SetAllToAllIndexInPlane2(int aDir, int srcP, int trgP)
{
  int dir = aDir;
  int xMin, xMax, yMin, yMax, zMin, zMax;
  int maxDim = 3;
  int coords[2];
  int rank;
  int commSize;
  
  // this function returns the coords of the sub communicator.  
  int sendCount = 0;
  int sendDisp  = 0;
  

  for(int trgP0=0; trgP0<trgP; trgP0++)   
    for(int trgP1=0; trgP1<srcP; trgP1++)      
     {
  xMin = _sendIndex[dir][trgP1][trgP0].min().x();
  xMax = _sendIndex[dir][trgP1][trgP0].max().x();
  
  yMin = _sendIndex[dir][trgP1][trgP0].min().y();
  yMax = _sendIndex[dir][trgP1][trgP0].max().y();
  
  zMin = _sendIndex[dir][trgP1][trgP0].min().z();
  zMax = _sendIndex[dir][trgP1][trgP0].max().z();

  coords[0] = trgP0;
  coords[1] = trgP1;
    
        (*_comm).getSubRank(dir%(DIM+1), coords, &rank); 
  sendDisp              += sendCount;
  _sendDisp_R[dir][rank] = sendDisp;
  sendCount              = 2*(xMax-xMin+1)*(yMax-yMin+1)*(zMax-zMin+1);	
       
  assert(sendCount>=0);
  _sendCount_R[dir][rank] = sendCount;

      }

   
  int recvCount = 0;
  int recvDisp  = 0;

  for(int srcP0=0; srcP0<trgP; srcP0++)
    for(int srcP1=0; srcP1<srcP; srcP1++)  
      
      {
  xMin = _recvIndex[dir][srcP1][srcP0].min().x();
  xMax = _recvIndex[dir][srcP1][srcP0].max().x();
  
  yMin = _recvIndex[dir][srcP1][srcP0].min().y();
  yMax = _recvIndex[dir][srcP1][srcP0].max().y();
  
  zMin = _recvIndex[dir][srcP1][srcP0].min().z();
  zMax = _recvIndex[dir][srcP1][srcP0].max().z();
  
  int rank;
  
  coords[0] = srcP0;
  coords[1] = srcP1;
    
  (*_comm).getSubRank(dir%(DIM+1), coords, &rank);
  
  recvDisp               += recvCount;
  _recvDisp_R[dir][rank] += recvDisp;

  recvCount               = 2*(xMax-xMin+1)*(yMax-yMin+1)*(zMax-zMin+1);
  _recvCount_R[dir][rank] = recvCount;

//      printf(" recvCount--- %d %d %d %d %d %d\n", xMax, xMin, yMax, yMin,zMax, zMin);
// 	printf("====== _recvCount[%d][%d] = %d \n", dir, rank, _recvCount[dir][rank]);
// 	printf("====== _recvDisp[%d][%d] = %d \n", dir, rank, _recvDisp[dir][rank]);
      }
 
    return;
} 

template< class FFT_PLAN, int FWD_REV>
void  FFT3D<FFT_PLAN, FWD_REV>::AllocateMemory()
{
  int commSize;
 
  for(int dir=0; dir<DIM; dir++)
    {  
      (*_comm).getSubSize(dir, &commSize);

      assert((dir==0 && commSize ==_pZ)||
       (dir==1 && commSize ==(_pZ*_pY))||
       (dir==2 && commSize ==(_pX*_pY))||
       (dir==3 && commSize ==_pX));

      _sendCount[dir] = new int[commSize];
      _recvCount[dir] = new int[commSize];
      
      _sendDisp[dir]  = new int[commSize];
      _recvDisp[dir]  = new int[commSize];

      _sendCount_R[dir] = new int[commSize];
      _recvCount_R[dir] = new int[commSize];
       
      _sendDisp_R[dir]  = new int[commSize];
      _recvDisp_R[dir]  = new int[commSize];
    }

 
  int totalLocal      = _localNx[0]*_localNy[0]*_localNz[0];
  int maxLocal        = MAX(_localNx[0], MAX(_localNy[0], _localNz[0]));
  int maxLocalSize    = (MAX(_pX,MAX(_pY, _pZ)))*totalLocal;
  int maxLocalTmpSize = (MAX(_pX,MAX(_pY, _pZ)))*maxLocal;

 
  _tmpSrc = new complex[maxLocalTmpSize];        
  _tmpTrg = new complex[maxLocalTmpSize];  
        
  _localDataIn   = new complex[maxLocalSize];
  _localDataOut  = new complex[maxLocalSize];
  _localDataTmp1 = new complex[maxLocalSize];
  _localDataTmp2 = new complex[maxLocalSize];
}


// This is used for static allocation  only.
template< class FFT_PLAN, int FWD_REV>
void  FFT3D<FFT_PLAN, FWD_REV>::Init(FFT3D** ptr)
{
  _pX = FFT_PLAN::P_X;
  _pY = FFT_PLAN::P_Y;
  _pZ = FFT_PLAN::P_Z;
  
  _localNx[0] = FFT_PLAN::GLOBAL_SIZE_X / _pX;
  _localNy[0] = FFT_PLAN::GLOBAL_SIZE_Y / _pY;
  _localNz[0] = FFT_PLAN::GLOBAL_SIZE_Z / _pZ;
    
  assert( (((_localNx[0]*_pX)%_pX)==0) &&
    (((_localNy[0]*_pY)%_pY)==0) && 
    (((_localNz[0]*_pZ)%_pZ)==0));
 
  int dimSz[DIM-1] = {_pX, _pY, _pZ};

  int periods[DIM-1]  = {0, 0, 0};  // logical array, specifying whether the
 
  assert(_localNx[0]*_localNy[0] >= _pZ);
  assert(_localNx[0]*_localNz[0] >= _pY);
  assert(_localNz[0]*_localNy[0] >= _pX);
  
  // grid is periodic, in each dim.   
  int reorder = 1;                   // ranking is not reordered.
 
  MPI_Cart_create(MPI_COMM_WORLD, DIM-1, dimSz,periods,reorder, &_cart_comm);
  _comm = new Comm3DLayer(_pX, _pY, _pZ, _cart_comm);
  
  (*_comm).getCartXCoord(&_myPx);
  (*_comm).getCartYCoord(&_myPy);
  (*_comm).getCartZCoord(&_myPz);
  
  MPI_Comm_rank(_cart_comm, &_globalID);
  MPI_Comm_rank(MPI_COMM_WORLD, &_myRank);    
 
  Plan();
  SetSendRecvIndecies();

  AllocateMemory();
 
  SetAllToAllIndexInPlane(DIR_OZ,   1, _pZ);
  SetAllToAllIndexInPlane(DIR_ZY, _pZ, _pY);
  SetAllToAllIndexInPlane(DIR_YX, _pY, _pX);
 
  _strideYZX[0] = 1;
  _strideYZX[1] = _localNx[0]*_localNz[0];
  _strideYZX[2] = _localNx[0];

  _strideZXY[0] = _localNy[0];
  _strideZXY[1] = 1;
  _strideZXY[2] = _localNx[0]*_localNy[0];
  
  _strideXYZ[0] = _localNy[0]*_localNz[0];
  _strideXYZ[1] = _localNz[0];
  _strideXYZ[2] =  1;

  _strideYXZ[0] = _localNx[0]*_localNz[0];
  _strideYXZ[1] = _localNz[0];
  _strideYXZ[2] =  1;

  _strideR0     = 1; 
  _strideR1     = _localNx[0]*_pX;
  _strideR2     =_strideR1*_localNy[3];

  int commSize=-1;
  MPI_Comm_size((*_comm)._comm[0], &commSize);
  _strideSrcA[0][2] = 1;
  _strideSrcA[0][1] = _localNz[0]*_nFFTs[0];
  _strideSrcA[0][0] = _localNz[0];
   
  _strideTrgA[0][2] = _nFFTs[0];
  _strideTrgA[0][1] = _nFFTs[0]*_localNz[0];
  _strideTrgA[0][0] = 1;

  _localSizeA[0][2] = _localNz[0];
  _localSizeA[0][1] =  commSize;
  _localSizeA[0][0] = _nFFTs[0];

  commSize=-1;
  MPI_Comm_size((*_comm)._comm[1], &commSize);

  _strideSrcA[1][2] =  1;
  _strideSrcA[1][1] = _localNy[0]*_nFFTs[1]; 
  _strideSrcA[1][0] = _localNy[0];
 
  _strideTrgA[1][2] = _nFFTs[1];
  _strideTrgA[1][1] = _nFFTs[1]*_localNy[0];
  _strideTrgA[1][0] = 1;
  
  _localSizeA[1][2] = _localNy[0];
  _localSizeA[1][1] = _pY;
  _localSizeA[1][0] = _nFFTs[1];

  _strideSrcA[2][2] = 1;
  _strideSrcA[2][1] =  _localNx[2]*_nFFTs[2]; 
  _strideSrcA[2][0] =  _localNx[2];
  
  _strideTrgA[2][2] = (_nFFTs[2])*_pX;
  _strideTrgA[2][1] = (_nFFTs[2]);
  _strideTrgA[2][0] = 1;
 
  _localSizeA[2][2] = _localNx[2];
  _localSizeA[2][1] = _pX*(_localNx[0]/_localNx[2]);
  _localSizeA[2][0] = _nFFTs[2];

  int xSize = MAX(MAX(_localSizeA[0][0],_localSizeA[1][0]),_localSizeA[2][0]);
  int ySize = MAX(MAX(_localSizeA[0][1],_localSizeA[1][1]),_localSizeA[2][1]);
  int zSize = MAX(MAX(_localSizeA[0][2],_localSizeA[1][2]),_localSizeA[2][2]);

  for(int d=0; d<3; d++)
    {
      _trgIndex[d] = new int**[xSize];
        
    for(int i=0; i<xSize; i++)
    {
    _trgIndex[d][i] = new int*[ySize];
    for(int j=0; j<ySize; j++)
      {
        _trgIndex[d][i][j] = new int[zSize];
      }
    
    } 
    }
  
  for(int d=0; d<3; d++)
    {
      int localNx    = _localSizeA[d][0];
      int localNy    = _localSizeA[d][1];
      int localNz    = _localSizeA[d][2];
      
      int trgStride0 = _strideTrgA[d][0];
      int trgStride1 = _strideTrgA[d][1];
      int trgStride2 = _strideTrgA[d][2];

      int counter=0;

      if(( _localNx[0]*_localNy[0]*_localNz[0] )!= (localNx*localNy*localNz))
      {
  printf("DIR %d  %d %d %d %d %d %d %d %d \n", d,
         _localNx[0],_localNy[0],_localNz[0],
          localNx, localNy, localNz,
         _localNx[0]*_localNy[0]*_localNz[0],
          localNx*localNy*localNz);
      } 
      assert((_localNx[0]*_localNy[0]*_localNz[0]) == (localNx*localNy*localNz)); 
        
      for(int i=0; i<localNx; i++)
  {
    for(int j=0; j<localNy; j++)
      {
        for(int k=0; k<localNz; k++)
    {
      
      if(d==0){
        _trgIndex[d][i][j][k] = 
          i*trgStride0+j*trgStride1+k*trgStride2;
        assert(_trgIndex[d][i][j][k] < _localNx[0]*_localNy[0]*_localNz[0]) ;
      }
      
      if(d==2)
        {
          _trgIndex[d][i][j][k] = counter; counter++;
          assert(_trgIndex[d][i][j][k] < _localNx[0]*_localNy[0]*_localNz[0]) ;
        }

      if(d==1)
        {
          int  trgIndex = i*trgStride0+j*trgStride1+k*trgStride2;
          int howMany = (_localNx[d]*_localNz[d+1])/(_pZ*_pY);

          int ii    = i     % _localNx[d];
          int alpha = i     / _localNx[d];
          int kk    = alpha % _localNz[d+1];
          int pZ    = alpha / _localNz[d+1];
  
          trgIndex  = j*trgStride1+(((kk)*_localNy[d]+k))
      *_localNx[d+1]+ (pZ*_localNx[d]+ii);
          
          _trgIndex[d][i][j][k] =  trgIndex;

          assert(_trgIndex[d][i][j][k] <( _localNx[0]*_localNy[0]*_localNz[0])) ;
   
        }
    }
      }
        }
    }
  
  MPI_Barrier(_cart_comm);  
}


// This is used for dynamic allocation only.
template< class FFT_PLAN, int FWD_REV>
void  FFT3D<FFT_PLAN, FWD_REV>::Init(int globalNx, int globalNy, int globalNz,
             int pX, int pY, int pZ)
{
  _pX = pX; _pY = pY; _pZ = pZ;
  
  _localNx[0] = globalNx;
  _localNy[0] = globalNy;
  _localNz[0] = globalNz;
    
  assert( (((_localNx[0]*_pX)%_pX)==0) &&
    (((_localNy[0]*_pY)%_pY)==0) && 
    (((_localNz[0]*_pZ)%_pZ)==0));
 
  int dimSz[DIM-1] = {_pX, _pY, _pZ};

  int periods[DIM-1]  = {0, 0, 0};  // logical array, specifying whether the
 
  assert(_localNx[0]*_localNy[0] >= _pZ);
  assert(_localNx[0]*_localNz[0] >= _pY);
  assert(_localNz[0]*_localNy[0] >= _pX);
  
  // grid is periodic, in each dim.   
  int reorder = 1;                   // ranking is not reordered.
 
  MPI_Cart_create(MPI_COMM_WORLD, DIM-1, dimSz,periods,reorder, &_cart_comm);
  _comm = new Comm3DLayer(_pX, _pY, _pZ, _cart_comm);
  
  (*_comm).getCartXCoord(&_myPx);
  (*_comm).getCartYCoord(&_myPy);
  (*_comm).getCartZCoord(&_myPz);
  
  MPI_Comm_rank(_cart_comm, &_globalID);
  MPI_Comm_rank(MPI_COMM_WORLD, &_myRank);    
 
  Plan();
  SetSendRecvIndecies();

  AllocateMemory();
 
  SetAllToAllIndexInPlane(DIR_OZ,   1, _pZ);
  SetAllToAllIndexInPlane(DIR_ZY, _pZ, _pY);
  SetAllToAllIndexInPlane(DIR_YX, _pY, _pX);
 
  _strideYZX[0] = 1;
  _strideYZX[1] = _localNx[0]*_localNz[0];
  _strideYZX[2] = _localNx[0];

  _strideZXY[0] = _localNy[0];
  _strideZXY[1] = 1;
  _strideZXY[2] = _localNx[0]*_localNy[0];
  
  _strideXYZ[0] = _localNy[0]*_localNz[0];
  _strideXYZ[1] = _localNz[0];
  _strideXYZ[2] =  1;

  _strideYXZ[0] = _localNx[0]*_localNz[0];
  _strideYXZ[1] = _localNz[0];
  _strideYXZ[2] =  1;

  _strideR0     = 1; 
  _strideR1     = _localNx[0]*_pX;
  _strideR2     =_strideR1*_localNy[3];

  int commSize=-1;
  MPI_Comm_size((*_comm)._comm[0], &commSize);
  _strideSrcA[0][2] = 1;
  _strideSrcA[0][1] = _localNz[0]*_nFFTs[0];
  _strideSrcA[0][0] = _localNz[0];
   
  _strideTrgA[0][2] = _nFFTs[0];
  _strideTrgA[0][1] = _nFFTs[0]*_localNz[0];
  _strideTrgA[0][0] = 1;

  _localSizeA[0][2] = _localNz[0];
  _localSizeA[0][1] =  commSize;
  _localSizeA[0][0] = _nFFTs[0];

  commSize=-1;
  MPI_Comm_size((*_comm)._comm[1], &commSize);

  _strideSrcA[1][2] =  1;
  _strideSrcA[1][1] = _localNy[0]*_nFFTs[1]; 
  _strideSrcA[1][0] = _localNy[0];
 
  _strideTrgA[1][2] = _nFFTs[1];
  _strideTrgA[1][1] = _nFFTs[1]*_localNy[0];
  _strideTrgA[1][0] = 1;
  
  _localSizeA[1][2] = _localNy[0];
  _localSizeA[1][1] = _pY;
  _localSizeA[1][0] = _nFFTs[1];

  _strideSrcA[2][2] = 1;
  _strideSrcA[2][1] =  _localNx[2]*_nFFTs[2]; 
  _strideSrcA[2][0] =  _localNx[2];
  
  _strideTrgA[2][2] = (_nFFTs[2])*_pX;
  _strideTrgA[2][1] = (_nFFTs[2]);
  _strideTrgA[2][0] = 1;
 
  _localSizeA[2][2] = _localNx[2];
  _localSizeA[2][1] = _pX*(_localNx[0]/_localNx[2]);
  _localSizeA[2][0] = _nFFTs[2];

  int xSize = MAX(MAX(_localSizeA[0][0],_localSizeA[1][0]),_localSizeA[2][0]);
  int ySize = MAX(MAX(_localSizeA[0][1],_localSizeA[1][1]),_localSizeA[2][1]);
  int zSize = MAX(MAX(_localSizeA[0][2],_localSizeA[1][2]),_localSizeA[2][2]);

  for(int d=0; d<3; d++)
    {
      _trgIndex[d] = new int**[xSize];
        
    for(int i=0; i<xSize; i++)
    {
    _trgIndex[d][i] = new int*[ySize];
    for(int j=0; j<ySize; j++)
      {
        _trgIndex[d][i][j] = new int[zSize];
      }
    
    } 
    }
  
  for(int d=0; d<3; d++)
    {
      int localNx    = _localSizeA[d][0];
      int localNy    = _localSizeA[d][1];
      int localNz    = _localSizeA[d][2];
      
      int trgStride0 = _strideTrgA[d][0];
      int trgStride1 = _strideTrgA[d][1];
      int trgStride2 = _strideTrgA[d][2];

      int counter=0;

      if(( _localNx[0]*_localNy[0]*_localNz[0] )!= (localNx*localNy*localNz))
      {
  printf("DIR %d  %d %d %d %d %d %d %d %d \n", d,
         _localNx[0],_localNy[0],_localNz[0],
          localNx, localNy, localNz,
         _localNx[0]*_localNy[0]*_localNz[0],
          localNx*localNy*localNz);
      } 
      assert((_localNx[0]*_localNy[0]*_localNz[0]) == (localNx*localNy*localNz)); 
        
      for(int i=0; i<localNx; i++)
  {
    for(int j=0; j<localNy; j++)
      {
        for(int k=0; k<localNz; k++)
    {
      
      if(d==0){
        _trgIndex[d][i][j][k] = 
          i*trgStride0+j*trgStride1+k*trgStride2;
        assert(_trgIndex[d][i][j][k] < _localNx[0]*_localNy[0]*_localNz[0]) ;
      }
      
      if(d==2)
        {
          _trgIndex[d][i][j][k] = counter; counter++;
          assert(_trgIndex[d][i][j][k] < _localNx[0]*_localNy[0]*_localNz[0]) ;
        }

      if(d==1)
        {
          int  trgIndex = i*trgStride0+j*trgStride1+k*trgStride2;
          int howMany = (_localNx[d]*_localNz[d+1])/(_pZ*_pY);

          int ii    = i     % _localNx[d];
          int alpha = i     / _localNx[d];
          int kk    = alpha % _localNz[d+1];
          int pZ    = alpha / _localNz[d+1];
  
          trgIndex  = j*trgStride1+(((kk)*_localNy[d]+k))
      *_localNx[d+1]+ (pZ*_localNx[d]+ii);
          
          _trgIndex[d][i][j][k] =  trgIndex;

          assert(_trgIndex[d][i][j][k] <( _localNx[0]*_localNy[0]*_localNz[0])) ;
   
        }
    }
      }
        }
    }
  
  MPI_Barrier(_cart_comm);  
}

template< class FFT_PLAN, int FWD_REV>
void FFT3D<FFT_PLAN, FWD_REV>::Redistribute(int dir, complex *localDataSrc, 
          complex *localDataTrg)
{
  MPI_Alltoallv((void *)localDataSrc, _sendCount[dir], 
    _sendDisp[dir], MPI_DOUBLE,
    (void*)localDataTrg, _recvCount[dir], _recvDisp[dir],
    MPI_DOUBLE,(*_comm)._comm[dir]);
}

template< class FFT_PLAN, int FWD_REV>
void FFT3D<FFT_PLAN, FWD_REV>::Redistribute_R(int dir, complex *localDataSrc, 
          complex *localDataTrg)
{
  MPI_Alltoallv((void *)localDataSrc, _recvCount_R[dir], 
    _recvDisp_R[dir],      MPI_DOUBLE,
    (void*)localDataTrg,   _sendCount_R[dir], _sendDisp_R[dir],
    MPI_DOUBLE,(*_comm)._comm[dir]);		
}




template< class FFT_PLAN, int FWD_REV>
void FFT3D<FFT_PLAN, FWD_REV>::ReorderDoFFTs(int dir, int *srcStride, int *trgStride,
         int *localSize,
               complex* localDataSrc, complex* localDataTrg)
{
  int localNx = localSize[0];
  int localNy = localSize[1];
  int localNz = localSize[2];
  
  int srcStride0 = srcStride[0];
  int srcStride1 = srcStride[1];
  int srcStride2 = srcStride[2];
  
  // int nFFTs = _nFFTs[dir];

  FFTOneD *fft = _fft[dir];
  int trgIndex=0, srcIndex=0;

  //CHECK  This and the printf should be completely removed  
  int globalID = -1; 
  MPI_Comm_rank(_cart_comm, &globalID);

  int counter = 0;
  
  int ***trgIndexTmp = _trgIndex[dir];

  for(int i=0; i<localNx; i++)
    {
      for(int j=0; j<localNy; j++)
  { 
    for(int k=0; k<localNz; k++)
      {
        srcIndex = i*srcStride0+j*srcStride1+k*srcStride2;
       

        _tmpSrc[k+j*localNz].re=localDataSrc[srcIndex].re;
        _tmpSrc[k+j*localNz].im=localDataSrc[srcIndex].im;
        assert(srcIndex < _localNx[0]*_localNy[0]*_localNz[0]);
        
      
      } 
  }

      (*fft).fftInternal(_tmpSrc, _tmpTrg);
      //_tmpTrg = _tmpSrc;
      
             
      for(int j=0; j<localNy; j++)
  {
    for(int k=0; k<localNz; k++)
      {
              trgIndex = _trgIndex[dir][i][j][k];
        
        localDataTrg[trgIndex].re = _tmpTrg[k+j*localNz].re;
        localDataTrg[trgIndex].im = _tmpTrg[k+j*localNz].im;

        assert(trgIndex < _localNx[0]*_localNy[0]*_localNz[0]); 

      }
    
  }
      
    }
    
}


//in->tmp1->tmp2->out->in->tmp1->tmp2->out

template< class FFT_PLAN, int FWD_REV>
void FFT3D<FFT_PLAN, FWD_REV>::DoFFT()  
{	
  static int iterCounter =0;
  iterCounter++;
  static double now;
  
  if(iterCounter==1) now = MPI_Wtime();
  static iter = 0;
  iter++; 
  int globalID = -1; 
   
  if(FWD_REV == FORWARD)
    {
      int dir=0;
      Redistribute(dir, _localDataIn, _localDataTmp2);  
      ReorderDoFFTs(dir, _strideSrcA[dir], _strideTrgA[dir], _localSizeA[dir],
        _localDataTmp2, _localDataTmp1);

      dir=1;
      Redistribute(dir, _localDataTmp1, _localDataOut);  
      ReorderDoFFTs(dir, _strideSrcA[dir], _strideTrgA[dir], _localSizeA[dir],
        _localDataOut, _localDataTmp2);


      dir=2;
      Redistribute(dir, _localDataTmp2, _localDataTmp1);  
      ReorderDoFFTs(dir, _strideSrcA[dir], _strideTrgA[dir], _localSizeA[dir],
        _localDataTmp1, _localDataOut);
    }
  else
    {
      int  dir=2; 
      
      ReorderDoFFTs_R(dir, _strideTrgA[dir], _strideSrcA[dir], _localSizeA[dir],
          _localDataIn, _localDataTmp2);
      Redistribute_R(dir, _localDataTmp2, _localDataTmp1);  
 
      dir=1;  
      ReorderDoFFTs_R(dir, _strideTrgA[dir], _strideSrcA[dir], _localSizeA[dir],
          _localDataTmp1, _localDataOut);
      Redistribute_R(dir, _localDataOut, _localDataTmp2);
  
      dir=0;   
      ReorderDoFFTs_R(dir, _strideTrgA[dir], _strideSrcA[dir], _localSizeA[dir],
          _localDataTmp2, _localDataTmp1);

      Redistribute_R(dir, _localDataTmp1, _localDataOut);  
   
    }
  
}



template< class FFT_PLAN, int FWD_REV>
void FFT3D<FFT_PLAN, FWD_REV>::ReorderDoFFTs_R(int dir, int *srcStride, int *trgStride,
         int *localSize,
               complex* localDataSrc, complex* localDataTrg)
{
  int localNx = localSize[0];
  int localNy = localSize[1];
  int localNz = localSize[2];
  
 
  int trgStride0 = trgStride[0];
  int trgStride1 = trgStride[1];
  int trgStride2 = trgStride[2];

  int nFFTs = _nFFTs[dir];

  FFTOneD *fft = _fft[dir];
  int trgIndex=0, srcIndex=0;
  
  int globalID = -1; 
  MPI_Comm_rank(_cart_comm, &globalID);
 
  int counter = 0;
  int ***srcIndexDir =  _trgIndex[dir];  

  for(int i=0; i<localNx; i++)
    {
      for(int j=0; j<localNy; j++)
  { 
    for(int k=0; k<localNz; k++)
      {

        srcIndex = srcIndexDir[i][j][k];
        _tmpSrc[k+j*localNz].re = localDataSrc[srcIndex].re;
        _tmpSrc[k+j*localNz].im = localDataSrc[srcIndex].im;
      
      }	      
      
  }
           
      (*fft).fftInternal(_tmpSrc, _tmpTrg);
      //  _tmpTrg = _tmpSrc;
      
      for(int j=0; j<localNy; j++)
  {
    for(int k=0; k<localNz; k++)
      {
        trgIndex = i*trgStride0+j*trgStride1+k*trgStride2;        
        localDataTrg[trgIndex].re = _tmpTrg[k+j*localNz].re;
        localDataTrg[trgIndex].im = _tmpTrg[k+j*localNz].im;
      } 
  }
    }
}


#endif


