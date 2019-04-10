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
   Last  modified : 05/30/2006  11:41:51 
   Name   : Maria Eleftheriou, IBM T.J.W R.C.
   Last Modified: 06/02/06 
   By     : ME
*/

/** *  \addtogroup FFTS
 *  The FFT class computes the complex DFT of a 3D array. The data is
 *  distributed on a 3D processor mesh. It is templated on the type 
 *  of 1D FFT. The 1D FFT performs the 1D FFTs locally on each node.
 **/

#ifndef __BGL3DFFT_H_
#define __BGL3DFFT_H_

#define FORWARD -1
#define REVERSE 1

#include <PrivateFFT.hpp>
// #include <complexTemplate.hpp>
#undef MAX
#define MAX(i, j) ( ((i)>(j))?(i):(j) )

#ifdef PK_BLADE_SPI
#define LOG_FFT_MPI 0
#endif

template<class FFT_PLAN, int FWD_REV, class T, class Tcomplex>
class BGL3DFFT 
{
public:  
  enum{ DIM = 4 };
  typedef FFT_PLAN FFT_PLAN_IF;
  PrivateFFT<T, Tcomplex> *privateFFT;
  
  enum { P_X = FFT_PLAN::P_X };
  enum { P_Y = FFT_PLAN::P_Y };
  enum { P_Z = FFT_PLAN::P_Z };
  
  void Init(BGL3DFFT** ptr, const int& globalNx, const int& globalNy, const int& globalNz,int subPx, int subPy, int subPz); 
  void Init(const int& localNx, const int& localNy, const int& localNz, const int& pX, const int& pY,
	 const int& pZ,int subPx, int subPy, int subPz, int compress);
  
  void Finalize(){delete privateFFT;} 

  inline void ZeroRealSpace();
  
  // Function calls to retrive data element by element.
  
  inline T GetRealSpaceElement(const int x, const int y, const int z ) const;
  inline void PutRealSpaceElement(const int x, const int y, const int z, T realInput);
  inline Tcomplex GetRecipSpaceElement(const int kx,const int ky, const int kz) const;
  inline void PutRecipSpaceElement( int kx, int ky, int kz,Tcomplex value);
  
  // Returns the original and the transform data distriutions
  void GetLocalDimensions( int& aLocalX, int& aLocalSizeX,
			   int& aLocalY, int& aLocalSizeY,
			   int& aLocalZ, int& aLocalSizeZ,
			   int& aLocalRecipX, int& aLocalRecipSizeX,
			   int& aLocalRecipY, int& aLocalRecipSizeY,
			   int& aLocalRecipZ, int& aLocalSizeRecipZ );
  
  void DoFFT(
      Tcomplex * bglfftIn, Tcomplex *bglfftOut
  ) ; 
  void DoFFT();
  int GetPzPyMod(){ _return (  ((*private)._myPz+(*private)._myPy)% ((*private)._pStep[0]) );}
  // These fields  are for phase_4: CHECK do we still need them. 

  
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


template<class FFT_PLAN, int FWD_REV, class T, class Tcomplex>
inline void BGL3DFFT<FFT_PLAN, FWD_REV, T, Tcomplex>::ZeroRealSpace()
{
  int localSize = (*privateFFT).getLocalNx(0)*(*privateFFT).getLocalNy(0)*(*privateFFT).getLocalNz(0);
  
  for(int index = 0; index<localSize; index++)
    {
      (*privateFFT)._localDataIn[index] = 0.0 ;
    }
}
//in->tmp1->tmp2->out->in->tmp1->tmp2->out

template< class FFT_PLAN, int FWD_REV, class T, class Tcomplex>
inline void BGL3DFFT<FFT_PLAN, FWD_REV, T, Tcomplex>::GetLocalDimensions( int& aLocalX,
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
  aLocalX = (*privateFFT)._myPx*(*privateFFT).getLocalNx(0);   aLocalSizeX = (*privateFFT).getLocalNx(0);
  aLocalY = (*privateFFT)._myPy*(*privateFFT).getLocalNy(0);   aLocalSizeY = (*privateFFT).getLocalNy(0);
  aLocalZ = (*privateFFT)._myPz*(*privateFFT).getLocalNz(0);   aLocalSizeZ = (*privateFFT).getLocalNz(0);
 
  //aLocalRecipX     = 0;  
  //aLocalRecipSizeX = (*privateFFT).getLocalNx(0)*(*privateFFT)._pX;
  
  int myPx = (*privateFFT)._myPx;
  int myPy = (*privateFFT)._myPy;
  int myPz = (*privateFFT)._myPz;

  		   
  aLocalRecipY = (*privateFFT)._ab->_Rx[myPx][myPy][myPz].min().y();

  aLocalRecipSizeY = (*privateFFT)._ab->_Rx[myPx][myPy][myPz].max().y()-(*privateFFT)._ab->_Rx[myPx][myPy][myPz].min().y()+1;
  
  aLocalRecipZ = (*privateFFT)._ab->_Rx[myPx][myPy][myPz].min().z();

  aLocalRecipSizeZ = (*privateFFT)._ab->_Rx[myPx][myPy][myPz].max().z()-(*privateFFT)._ab->_Rx[myPx][myPy][myPz].min().z()+1;
  
  aLocalRecipX = (*privateFFT)._ab->_Rx[myPx][myPy][myPz].min().x();
  
  aLocalRecipSizeX = (*privateFFT)._ab->_Rx[myPx][myPy][myPz].max().x()-(*privateFFT)._ab->_Rx[myPx][myPy][myPz].min().x()+1;
}

 
template< class FFT_PLAN, int FWD_REV, class T, class Tcomplex>
void  BGL3DFFT<FFT_PLAN, FWD_REV, T, Tcomplex>::Init(BGL3DFFT** ptr, const int& globalNx, const int& globalNy, const int& globalNz,
					int subPx, int subPy, int subPz)
{
  
  privateFFT = new PrivateFFT<T,Tcomplex>();
  
  Platform::Topology::GetDimensions( & (*privateFFT)._pX,  & (*privateFFT)._pY,  & (*privateFFT)._pZ );
  
  if( (*privateFFT)._pX == 0 )
    {
      (*privateFFT)._pX = FFT_PLAN::P_X;
      (*privateFFT)._pY = FFT_PLAN::P_Y;
      (*privateFFT)._pZ = FFT_PLAN::P_Z;
    }

  if ( subPx == -1 ) subPx = (*privateFFT)._pX ;
  if ( subPy == -1 ) subPy = (*privateFFT)._pY ;
  if ( subPz == -1 ) subPz = (*privateFFT)._pZ ;
  if((subPx > (*privateFFT)._pX ) || (subPy > (*privateFFT)._pY ) || (subPz > (*privateFFT)._pZ ) )
    {
      fprintf(stderr," Error: subProcessor mesh can't be larger than the processor mesh");
      exit(3);
    }
  
  if( (subPx==0) || ( subPy==0) || ( subPz==0) )
    {
      (*privateFFT)._pStep[0] = 1 ;
      (*privateFFT)._pStep[1] = 1 ;
      (*privateFFT)._pStep[2] = 1 ;
    }
  else 
    {
      if( ((*privateFFT)._pX %subPx !=0) || ((*privateFFT)._pY%subPy !=0) || ( (*privateFFT)._pZ%subPz !=0) )
	{
	  fprintf(stderr," Error: subProcessor mesh has to be power of two");
	  exit(4);
	}
      
      (*privateFFT)._pStep[0] = (*privateFFT)._pX /subPx;
      (*privateFFT)._pStep[1] = (*privateFFT)._pY /subPy;
      (*privateFFT)._pStep[2] = (*privateFFT)._pZ /subPz;
    }
   


  (*privateFFT).setLocalNx(0, globalNx/(*privateFFT)._pX);
  (*privateFFT).setLocalNy(0, globalNy/(*privateFFT)._pY);
  (*privateFFT).setLocalNz(0, globalNz/(*privateFFT)._pZ);
   
  if( ((((*privateFFT).getLocalNx(0)*(*privateFFT)._pX)%(*privateFFT)._pX)!=0) ||
      ((((*privateFFT).getLocalNy(0)*(*privateFFT)._pY)%(*privateFFT)._pY)!=0) || 
      ((((*privateFFT).getLocalNz(0)*(*privateFFT)._pZ)%(*privateFFT)._pZ)!=0))
    {
      fprintf(stderr," Error: All processor should have data at the original distribution");
      exit(2);
    }
  
  (*privateFFT).Initialize( FWD_REV );
}


template< class FFT_PLAN, int FWD_REV, class T, class Tcomplex>
void  BGL3DFFT<FFT_PLAN, FWD_REV, T,Tcomplex>::Init(const int& globalNx, const int& globalNy, const int& globalNz,
					const int& pX, const int& pY, const int& pZ, 
					int subPx, int subPy, int subPz, int compress)
{
  privateFFT = new PrivateFFT<T,Tcomplex>();
  (*privateFFT)._pX=pX; (*privateFFT)._pY=pY; (*privateFFT)._pZ=pZ;
  
  if ( subPx == -1 ) subPx = (*privateFFT)._pX ;
  if ( subPy == -1 ) subPy = (*privateFFT)._pY ;
  if ( subPz == -1 ) subPz = (*privateFFT)._pZ ;
  if((subPx > (*privateFFT)._pX ) || (subPy > (*privateFFT)._pY ) || (subPz > (*privateFFT)._pZ ) )
    {
      fprintf(stderr," Error: subProcessor mesh can't be larger than the processor mesh");
      exit(3);
    }
  
  if( (subPx==0) || ( subPy==0) || ( subPz==0) )
    {
      (*privateFFT)._pStep[0] = 1;
      (*privateFFT)._pStep[1] = 1;
      (*privateFFT)._pStep[2] = 1;
    }
  else 
    {
      if( ((*privateFFT)._pX %subPx !=0) || ((*privateFFT)._pY%subPy !=0) || ( (*privateFFT)._pZ%subPz !=0) )
	{
	  fprintf(stderr," Error: subProcessor mesh has to be power of two");
	  exit(4);
	}
      
      
      (*privateFFT)._pStep[0] = (*privateFFT)._pX /subPx;
      (*privateFFT)._pStep[1] = (*privateFFT)._pY /subPy;
      (*privateFFT)._pStep[2] = (*privateFFT)._pZ /subPz;
    }

  
  int nxy = (globalNx/(*privateFFT)._pX)*(globalNy/(*privateFFT)._pY);
  int nxz = (globalNx/(*privateFFT)._pX)*(globalNz/(*privateFFT)._pZ);
  int nyz = (globalNy/(*privateFFT)._pY)*(globalNz/(*privateFFT)._pZ);

  // printf(" before my steps... %d %d %d \n",
  //		  (*privateFFT)._pStep[0], (*privateFFT)._pStep[1], (*privateFFT)._pStep[2]);

  int _pX = (*privateFFT)._pX;
  int _pY = (*privateFFT)._pY;
  int _pZ = (*privateFFT)._pZ;

  (*privateFFT)._pStep[0] = ((nyz>=_pX)?(*privateFFT)._pStep[0]:_pX/nyz );
  (*privateFFT)._pStep[1] = ((nxz>=_pY)?(*privateFFT)._pStep[1]:_pY/nxz );
  (*privateFFT)._pStep[2] = ((nxy>=_pZ)?(*privateFFT)._pStep[2]:_pZ/nxy );
 /*  if((nyz > _pX || nxz > _pY || nxy > _pZ ) &&  compress==1) */
   {

   (*privateFFT)._pStep[0] = 2 ;
  (*privateFFT)._pStep[1] = 2;
  (*privateFFT)._pStep[2] =2; 
}
 //printf("fft3d::Init before my steps... %d %d %d \n",
  //		  (*privateFFT)._pStep[0], (*privateFFT)._pStep[1], (*privateFFT)._pStep[2]);
  (*privateFFT).setLocalNx(0, globalNx/pX);
  (*privateFFT).setLocalNy(0, globalNy/pY);
  (*privateFFT).setLocalNz(0, globalNz/pZ);
  if( ((((*privateFFT).getLocalNx(0)*(*privateFFT)._pX)%(*privateFFT)._pX)!=0) ||
      ((((*privateFFT).getLocalNy(0)*(*privateFFT)._pY)%(*privateFFT)._pY)!=0) || 
      ((((*privateFFT).getLocalNz(0)*(*privateFFT)._pZ)%(*privateFFT)._pZ)!=0))
    {
      fprintf(stderr," Error: All processor should have data at the original distribution");
      exit(2);
    }
  
  (*privateFFT).Initialize( FWD_REV );
}

template<class FFT_PLAN, int FWD_REV, class T, class Tcomplex>
void BGL3DFFT<FFT_PLAN,FWD_REV,T,Tcomplex>::
DoFFT(
    Tcomplex *bglfftIn, Tcomplex *bglfftOut
    )  
{
  if(FWD_REV == FORWARD)
    {
      (*privateFFT).DoFwdFFT(bglfftIn, bglfftOut);
    }
  else
    {
      (*privateFFT).DoRevFFT(bglfftIn, bglfftOut);
    }
}

template<class FFT_PLAN, int FWD_REV, class T, class Tcomplex> 
void BGL3DFFT<FFT_PLAN,FWD_REV,T,Tcomplex>::DoFFT()
{
  if(FWD_REV == FORWARD)
    {
      (*privateFFT).DoFwdFFT();
    }
  else
    {
      (*privateFFT).DoRevFFT();
    }
}


template<class FFT_PLAN, int FWD_REV, class T, class Tcomplex>
inline T BGL3DFFT<FFT_PLAN, FWD_REV, T, Tcomplex>::GetRealSpaceElement( int x, int y, int z ) const
{
  if(FWD_REV == FORWARD)
    {
      int index = x*(*privateFFT)._strideXYZ[0]+y*(*privateFFT)._strideXYZ[1]+z*(*privateFFT)._strideXYZ[2];  
      return creal((*privateFFT)._localDataIn[index]) ;
      
    }
  else
    {
      int index = x*(*privateFFT)._strideXYZ[0]+y*(*privateFFT)._strideXYZ[1]+z*(*privateFFT)._strideXYZ[2];
      return  creal((*privateFFT)._localDataOut[index]) ;
    }
}


template<class FFT_PLAN, int FWD_REV, class T, class Tcomplex>
inline Tcomplex BGL3DFFT<FFT_PLAN, FWD_REV, T, Tcomplex>::GetRecipSpaceElement( const int kx, const int ky, const int kz ) const
{
  int index =kx*(*privateFFT)._strideR0+ky*(*privateFFT)._strideR1+kz*(*privateFFT)._strideR2;
  
  if(FWD_REV == FORWARD){ return (*privateFFT)._localDataOut[index]; }
  else                  { return (*privateFFT)._localDataIn[index];  }
}


template<class FFT_PLAN, int FWD_REV, class T, class Tcomplex>
inline void BGL3DFFT<FFT_PLAN, FWD_REV, T, Tcomplex>::PutRecipSpaceElement( int kx, int ky, int kz, Tcomplex value)
{
  int index  = (*privateFFT)._strideR0*kx + (*privateFFT)._strideR1*ky + (*privateFFT)._strideR2*kz;
  (*privateFFT)._localDataIn[index] = value; 
}

template<class FFT_PLAN, int FWD_REV, class T, class Tcomplex>
inline void BGL3DFFT<FFT_PLAN, FWD_REV, T, Tcomplex>::PutRealSpaceElement(int x, int y, int z, T realInput)
{
  int index = x*(*privateFFT)._strideXYZ[0]+y*(*privateFFT)._strideXYZ[1]+z*(*privateFFT)._strideXYZ[2];
  (*privateFFT)._localDataIn[index] = realInput;
}


template < class FFT_PLAN, int FWD_REV, class T, class Tcomplex >
inline int BGL3DFFT<FFT_PLAN,FWD_REV,T,Tcomplex>::InitProcMesh( int grdvoxleft_X, int grdvoxleft_Y, int grdvoxleft_Z )
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


template < class FFT_PLAN, int FWD_REV, typename T, class Tcomplex >
inline void BGL3DFFT<FFT_PLAN,FWD_REV,T,Tcomplex>::GetGridOwnerIDList( const iXYZ& vmin, const iXYZ& vmax, int idlist[], int& idnum )
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


#if defined(FFT_COMPILE)
//template void BGL3DFFT<FFT_PLAN, PK_FORWARD,double,double complex>::DoFFT() ; 
//template void BGL3DFFT<FFT_PLAN, PK_REVERSE,double,double complex>::DoFFT() ; 
//template void BGL3DFFT<FFT_PLAN, PK_FORWARD,float,float complex>::DoFFT() ; 
//template void BGL3DFFT<FFT_PLAN, PK_REVERSE,float,float complex>::DoFFT() ;
template void BGL3DFFT<FFT_PLAN, PK_FORWARD,double,complex<double> >::DoFFT() ; 
template void BGL3DFFT<FFT_PLAN, PK_REVERSE,double,complex<double> >::DoFFT() ; 
template void BGL3DFFT<FFT_PLAN, PK_FORWARD,float,complex<float> >::DoFFT() ; 
template void BGL3DFFT<FFT_PLAN, PK_REVERSE,float,complex<float> >::DoFFT() ;
#endif



#endif
