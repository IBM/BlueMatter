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

 /* $Id    : fft3D.hpp
   Date   : 07/23/2003  11:41:51 
   Name   : Maria Eleftheriou, IBM T.J.W R.C.
   Last Modified: 
   By     :
*/

/** *  \addtogroup FFTS
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

#include "mpi.h"

#include <pk/platform.hpp>

#include <MPI/PrivateFFT_MPI.hpp>

#define MAX(i, j) ( (i>j)?i:j )

template<class FFT_PLAN, int FWD_REV, class T>
class FFT3D 
{
public:  
  enum{ DIM = 4 };
  typedef FFT_PLAN FFT_PLAN_IF;
  PrivateFFT<T> *privateFFT;

  enum { P_X = FFT_PLAN::P_X };
  enum { P_Y = FFT_PLAN::P_Y };
  enum { P_Z = FFT_PLAN::P_Z };
  
  FFT3D(){}
  void Init(FFT3D** ptr, const int& globalNx, const int& globalNy, const int& globalNz);  
  //void Init(FFT3D** ptr);
  void Init( int localNx,int localNy, int localNz, int pX, int pY, int pZ);
  void Initialize();

  ~FFT3D()
    {
      (*privateFFT).Finalize();
    }
  
  inline void ZeroRealSpace()
  {
    int localSize = (*privateFFT)._localNx[0]*(*privateFFT)._localNy[0]*(*privateFFT)._localNz[0];
    
    for(int index = 0; index<localSize; index++)
      {
        (*privateFFT)._localDataIn[index].re = 0.0;
        (*privateFFT)._localDataIn[index].im = 0.0;
      }
  }

  
  inline complexTemplate<T>* GetRealSpaceArray()
  {
    if(FWD_REV == FORWARD)
      {
  return (*privateFFT)._localDataIn;
      }
    else
      {
  return (*privateFFT)._localDataOut;
      }
  }

  inline T GetRealSpaceElement( int x, int y, int z )
  {
    if(FWD_REV == FORWARD)
      {
  int index = x*(*privateFFT)._strideXYZ[0]+y*(*privateFFT)._strideXYZ[1]+z*(*privateFFT)._strideXYZ[2];  
  return (*privateFFT)._localDataIn[index].re;
      }
    else
      {
  int index = x*(*privateFFT)._strideXYZ[0]+y*(*privateFFT)._strideXYZ[1]+z*(*privateFFT)._strideXYZ[2];
  assert(index <(*privateFFT)._localNx[0]*(*privateFFT)._localNy[0]*(*privateFFT)._localNz[0]);      
  return (*privateFFT)._localDataOut[index].re;
      }
    
  }

  inline void PutRealSpaceArray(T* inArray)
  {
    (*privateFFT)._localDataIn =(complexTemplate<T>) inArray;
  }

  inline void PutRealSpaceElement(int x, int y, int z,
          T realInput)
  {
    int index  =  x*(*privateFFT)._strideXYZ[0]
      +y*(*privateFFT)._strideXYZ[1]+z*(*privateFFT)._strideXYZ[2];
    
    (*privateFFT)._localDataIn[index].re = realInput;
    (*privateFFT)._localDataIn[index].im = 0.;
    
    assert(index <(*privateFFT)._localNx[0]*(*privateFFT)._localNy[0]*(*privateFFT)._localNz[0]);       
  }
  
  inline complexTemplate<T> GetRecipSpaceElement( int kx, int ky, int kz )
  {
    if(FWD_REV == FORWARD)
      {
  int index =kx*(*privateFFT)._strideR0+ky*(*privateFFT)._strideR1+kz*(*privateFFT)._strideR2;
  return (*privateFFT)._localDataOut[index];
      }
    else
      {
  
  int index =kx*(*privateFFT)._strideR0+ky*(*privateFFT)._strideR1+kz*(*privateFFT)._strideR2;
  return (*privateFFT)._localDataIn[index];
      }
  }
  
  inline T* GetRecipSpaceArray()
  {
    if(FWD_REV == FORWARD)
      {
  return (*privateFFT)._localDataOut;
      }
    else
      {
  return (*privateFFT)._localDataIn;
      }

  }

  inline void PutRecipSpaceElement( int kx, int ky, int kz,complexTemplate<T> value)
  {
    int index           = (*privateFFT)._strideR0*kx + (*privateFFT)._strideR1*ky + (*privateFFT)._strideR2*kz;
    (*privateFFT)._localDataIn[index] = value; 
  }

  
  inline void PutRecipSpaceArray(complexTemplate<T>* inArray)
  {
    (*privateFFT)._localDataIn = inArray; 
  }

  void GetLocalDimensions( int& aLocalX, int& aLocalSizeX,
         int& aLocalY, int& aLocalSizeY,
         int& aLocalZ, int& aLocalSizeZ,
         int& aLocalRecipX, int& aLocalRecipSizeX,
         int& aLocalRecipY, int& aLocalRecipSizeY,
         int& aLocalRecipZ, int& aLocalSizeRecipZ );
  
  void DoFFT(complexTemplate<T>* bglfftIn, complexTemplate<T>* bglfftOut) ; 
  void DoFFT();
  void PrintVector(complexTemplate<T>* data, int size, int myGlobalID,  int dir)
  {
    
    MPI_Comm comm;int dim=3;
    int coords[3]; //{-1,-1, -1};
    comm= (*privateFFT)._cart_comm; 
    comm=(*(*privateFFT)._comm)._comm[dir];
    if(dir==0 || dir==3){ dim=1;} else {dim=2;} 
    
    MPI_Cart_coords(comm, myGlobalID, dim, coords);
    
    for(int i=0; i<size; i++)
      printf("Array[%d][ %d->(%d %d %d)][ %d] = %lf \n",dir, 
       myGlobalID, coords[0], coords[1], coords[2],i, data[i].re);
    
  }

  

public:

    // accepts rectangle in simulation space - vmin and vmax are the corners of it
    // fills in the list of nodes where the effected mesh lives - idnum is num of entries returned
    int InitProcMesh( int grdvoxleft_X, int grdvoxleft_Y, int grdvoxleft_Z);
    inline void GetGridOwnerIDList( const iXYZ& vmin, const iXYZ& vmax, int idlist[], int& idnum );

private:

    // this facilitates GetGridOwnerIDList(...)


    int *subgridX;
    int *subgridY;
    int *subgridZ;
};




//in->tmp1->tmp2->out->in->tmp1->tmp2->out

template< class FFT_PLAN, int FWD_REV, class T>
inline void FFT3D<FFT_PLAN, FWD_REV, T>::GetLocalDimensions( int& aLocalX,
                int& aLocalSizeX,
                int& aLocalY, 
                int& aLocalSizeY,
                int& aLocalZ, 
                int& aLocalSizeZ,
                int& aLocalRecipX, 
                int& aLocalRecipSizeX,
                int& aLocalRecipY, 
                int& aLocalRecipSizeY,
                int& aLocalRecipZ, 
                int& aLocalRecipSizeZ )
{
  aLocalX = (*privateFFT)._myPx*(*privateFFT)._localNx[0];   aLocalSizeX = (*privateFFT)._localNx[0];
  aLocalY = (*privateFFT)._myPy*(*privateFFT)._localNy[0];   aLocalSizeY = (*privateFFT)._localNy[0];
  aLocalZ = (*privateFFT)._myPz*(*privateFFT)._localNz[0];   aLocalSizeZ = (*privateFFT)._localNz[0];

  aLocalRecipX     = 0;  
  aLocalRecipSizeX = (*privateFFT)._localNx[0]*(*privateFFT)._pX;
 
  int d = ((*privateFFT)._localNy[0]*(*privateFFT)._localNz[0]);
  int n1 = d*(*privateFFT)._myPx/(*privateFFT)._pX;
  int n2 = d*((*privateFFT)._myPx+1)/(*privateFFT)._pX-1;
  
  int j1 = n1%(*privateFFT)._localNy[0];
  int j2 = n2%(*privateFFT)._localNy[0];
 
  int k1 = n1/(*privateFFT)._localNy[0];
  int k2 = n2/(*privateFFT)._localNy[0];

  aLocalRecipY     = j1+(*privateFFT)._myPy*(*privateFFT)._localNy[0];
  aLocalRecipSizeY = j2-j1+1;

  aLocalRecipZ     = k1+(*privateFFT)._myPz*(*privateFFT)._localNz[0];
  aLocalRecipSizeZ = k2-k1+1;
}

 
template< class FFT_PLAN, int FWD_REV, class T>
void  FFT3D<FFT_PLAN, FWD_REV, T>::Init(FFT3D** ptr, const int& globalNx, const int& globalNy, const int& globalNz)
{
  
   privateFFT = new PrivateFFT<T>();

   Platform::Topology::GetDimensions( & (*privateFFT)._pX,  & (*privateFFT)._pY,  & (*privateFFT)._pZ );
   if( (*privateFFT)._pX == 0 )
   {
       (*privateFFT)._pX = FFT_PLAN::P_X;
       (*privateFFT)._pY = FFT_PLAN::P_Y;
       (*privateFFT)._pZ = FFT_PLAN::P_Z;
   }

  (*privateFFT)._localNx[0] = globalNx / (*privateFFT)._pX;
  (*privateFFT)._localNy[0] = globalNy / (*privateFFT)._pY;
  (*privateFFT)._localNz[0] = globalNz / (*privateFFT)._pZ;
  
 
  assert( ((((*privateFFT)._localNx[0]*(*privateFFT)._pX)%(*privateFFT)._pX)==0) &&
    ((((*privateFFT)._localNy[0]*(*privateFFT)._pY)%(*privateFFT)._pY)==0) && 
    ((((*privateFFT)._localNz[0]*(*privateFFT)._pZ)%(*privateFFT)._pZ)==0));
  
  Initialize();
}

template< class FFT_PLAN, int FWD_REV, class T>
void  FFT3D<FFT_PLAN, FWD_REV, T>::Initialize()
{
    (*privateFFT).Initialize( FWD_REV );
  
  
}



template< class FFT_PLAN, int FWD_REV, class T>
void  FFT3D<FFT_PLAN, FWD_REV, T>::Init(int globalNx, int globalNy, int globalNz,
      int pX, int pY, int pZ)
{
  privateFFT = new PrivateFFT<T>();
  (*privateFFT)._pX=pX; (*privateFFT)._pY=pY; (*privateFFT)._pZ=pZ;
 
  (*privateFFT)._localNx[0] = globalNx;
  (*privateFFT)._localNy[0] = globalNy;
  (*privateFFT)._localNz[0] = globalNz;
  
  assert( ((((*privateFFT)._localNx[0]*(*privateFFT)._pX)%(*privateFFT)._pX)==0) &&
    ((((*privateFFT)._localNy[0]*(*privateFFT)._pY)%(*privateFFT)._pY)==0) && 
    ((((*privateFFT)._localNz[0]*(*privateFFT)._pZ)%(*privateFFT)._pZ)==0));


  Initialize();
}

//template<class FFT_PLAN, int FWD_REV, class T>
//void FFT3D<FFT_PLAN,FWD_REV,T>::
//DoFFT(complexTemplate<T>* bglfftIn, complexTemplate<T>* bglfftOut)  
//   {
//     if(FWD_REV == FORWARD)
//       {
//        (*privateFFT).DoFwdFFT(bglfftIn, bglfftOut);
//       }
//     else
//       {
//         (*privateFFT).DoRevFFT(bglfftIn, bglfftOut);
//      }
//}
//
//template<class FFT_PLAN, int FWD_REV, class T> 
//void FFT3D<FFT_PLAN,FWD_REV,T>::DoFFT()
//{
//  if(FWD_REV == FORWARD)
//    {
//      (*privateFFT).DoFwdFFT();
//    }
//  else
//    {
//      (*privateFFT).DoRevFFT();
//    }
//}


template < class FFT_PLAN, int FWD_REV, typename T >
inline void FFT3D<FFT_PLAN,FWD_REV,T>::GetGridOwnerIDList( const iXYZ& vmin, const iXYZ& vmax, int idlist[], int& idnum )
{
//     const int GlobalXSize = FFT_PLAN::GLOBAL_SIZE_X;
//     const int GlobalYSize = FFT_PLAN::GLOBAL_SIZE_Y;
//     const int GlobalZSize = FFT_PLAN::GLOBAL_SIZE_Z;

    int procs_x, procs_y, procs_z; 
    Platform::Topology::GetDimensions( &procs_x, &procs_y, &procs_z );

    const int GlobalXSize = (*privateFFT)._localNx[0] * procs_x;
    const int GlobalYSize = (*privateFFT)._localNy[0] * procs_y;
    const int GlobalZSize = (*privateFFT)._localNz[0] * procs_z;

    iXYZ vmi = vmin;
    iXYZ vma = vmax;

    // map back into central cell
    if( vmi.mX < 0 ) vmi.mX += GlobalXSize;
    if( vmi.mY < 0 ) vmi.mY += GlobalYSize;
    if( vmi.mZ < 0 ) vmi.mZ += GlobalZSize;

    if( vma.mX >= GlobalXSize ) vma.mX -= GlobalXSize;
    if( vma.mY >= GlobalYSize ) vma.mY -= GlobalYSize;
    if( vma.mZ >= GlobalZSize ) vma.mZ -= GlobalZSize;

    int kxmin = procs_x - 1;
    while( subgridX[kxmin] > vmi.mX )
        kxmin--;

    int kxmax = procs_x - 1;
    while( subgridX[kxmax] > vma.mX )
        kxmax--;

    int kymin = procs_y - 1;
    while( subgridY[kymin] > vmi.mY )
        kymin--;

    int kymax = procs_y - 1;
    while( subgridY[kymax] > vma.mY )
        kymax--;

    int kzmin = procs_z - 1;
    while( subgridZ[kzmin] > vmi.mZ )
        kzmin--;

    int kzmax = procs_z - 1;
    while( subgridZ[kzmax] > vma.mZ )
        kzmax--;
    
    idnum = 0; 

    int kx = kxmin, kxx;
    do
    {
        kxx = kx % procs_x;
        int ky = kymin, kyy;
        do
        {
            kyy = ky % procs_y;
            int kz = kzmin, kzz;
            do
            {
                kzz = kz % procs_z;
                idlist[idnum++] = Platform::Topology::MakeRankFromCoords( kxx, kyy, kzz );
                kz++;
            } while( kzz != kzmax );
            ky++;
        } while( kyy != kymax );
        kx++;
    } while(  kxx != kxmax );
}

template < class FFT_PLAN, int FWD_REV, typename T >
inline int FFT3D<FFT_PLAN,FWD_REV,T>::InitProcMesh( int grdvoxleft_X, int grdvoxleft_Y, int grdvoxleft_Z )
{
    int i;

    int procs_x, procs_y, procs_z; 
    Platform::Topology::GetDimensions( &procs_x, &procs_y, &procs_z );

    int *subgr;
#ifdef PK_PARALLEL
    subgr = new int[procs_x*procs_y*procs_z];
    assert(subgr);
#endif

    int myproc_x, myproc_y, myproc_z;
    Platform::Topology::GetMyCoords( &myproc_x, &myproc_y, &myproc_z );

    subgridX = new int[procs_x];
    subgridY = new int[procs_y];
    subgridZ = new int[procs_z];
    assert( subgridX && subgridY && subgridZ );

    // X
#ifndef PK_PARALLEL
    subgr = subgridX;
#endif
    for( i=0; i<procs_x; i++ )
        subgr[i] = 0;
    if( myproc_y == 0 && myproc_z == 0 )
        subgr[ myproc_x ] = grdvoxleft_X;
#ifdef PK_PARALLEL
    Platform::Collective::INT_AllReduce_Sum( (int *) subgr, (int *) subgridX, procs_x );
#endif

    // Y
#ifndef PK_PARALLEL
    subgr = subgridY;
#endif
    for( i=0; i<procs_y; i++ )
        subgr[i] = 0;
    if( myproc_x == 0 && myproc_z == 0 )
        subgr[ myproc_y ] = grdvoxleft_Y;
#ifdef PK_PARALLEL
    Platform::Collective::INT_AllReduce_Sum( (int *) subgr, (int *) subgridY, procs_y );
#endif

    // Z
#ifndef PK_PARALLEL
    subgr = subgridZ;
#endif
    for( i=0; i<procs_z; i++ )
        subgr[i] = 0;
    if( myproc_x == 0 && myproc_y == 0 )
        subgr[ myproc_z ] = grdvoxleft_Z;
#ifdef PK_PARALLEL
    Platform::Collective::INT_AllReduce_Sum( (int *) subgr, (int *) subgridZ, procs_z );
#endif

#ifdef PK_PARALLEL
    delete [] subgr;
#endif

    return 0;
}

#if defined(FFT_COMPILE) || !defined(FFT_SEPARATE)
template<class FFT_PLAN, int FWD_REV, typename T>
 void FFT3D<FFT_PLAN,FWD_REV,T>::DoFFT()  
   {	
     static int sequence = 0 ;  
     if(FWD_REV == FORWARD)
       {
#if defined(SCALED_INTEGER_FFT_COMMS)
        if ( sequence > 0 )
        {
          (*privateFFT).DoFwdFFTIntegerComms();
        } else {
          (*privateFFT).DoFwdFFT() ;
        }
#else
        (*privateFFT).DoFwdFFT() ;
#endif                
       } 
     else
       {
#if defined(SCALED_INTEGER_FFT_COMMS)
        if ( sequence > 0 )
        {
          (*privateFFT).DoRevFFTIntegerComms();
        } else {
          (*privateFFT).DoRevFFT();
        }
#else
         (*privateFFT).DoRevFFT();
#endif
      }
      sequence += 1 ;
}
template<class FFT_PLAN, int FWD_REV, typename T>
 void FFT3D<FFT_PLAN,FWD_REV,T>::DoFFT(complexTemplate<T>* bglfftIn, complexTemplate<T>* bglfftOut)  
   {	
     static int sequence = 0 ;  
     if(FWD_REV == FORWARD)
       {
#if defined(SCALED_INTEGER_FFT_COMMS)
        if ( sequence > 0 )
        {
          (*privateFFT).DoFwdFFTIntegerComms(bglfftIn, bglfftOut);
        } else {
          (*privateFFT).DoFwdFFT(bglfftIn, bglfftOut) ;
        }
#else
        (*privateFFT).DoFwdFFT() ;
#endif                
       } 
     else
       {
#if defined(SCALED_INTEGER_FFT_COMMS)
        if ( sequence > 0 )
        {
          (*privateFFT).DoRevFFTIntegerComms(bglfftIn, bglfftOut);
        } else {
          (*privateFFT).DoRevFFT(bglfftIn, bglfftOut);
        }
#else
         (*privateFFT).DoRevFFT();
#endif
      }
      sequence += 1 ;
}

#if defined(FFT_COMPILE)
template void FFT3D<FFT_PLAN, PK_FORWARD,double>::DoFFT() ; 
template void FFT3D<FFT_PLAN, PK_REVERSE,double>::DoFFT() ; 
template void FFT3D<FFT_PLAN, PK_FORWARD,float>::DoFFT() ; 
template void FFT3D<FFT_PLAN, PK_REVERSE,float>::DoFFT() ; 
#endif

#endif

#endif
