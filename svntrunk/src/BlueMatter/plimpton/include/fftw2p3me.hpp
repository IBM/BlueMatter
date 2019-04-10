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
 
#ifndef _FFTW2P3ME_HPP_
#define _FFTW2P3ME_HPP_

//-------------------------------------------------------------
//
//               FFT INTERFACE CLASS
//
//                USES FFTW LIBRARY
//
//-------------------------------------------------------------


// macro FFTW_MPI_SLAB_WORLD should be defined to go to the parallel world

// to use multiple threads - define macro FFTW_THREADED and run in a uniprocessor mode
// that implies from what I can see now - either true uniprocessor (PK_MPI_ALL undefined)
// or FFT_IN_ONE_TASK_ONLY defined, which switches off FFTW_MPI_SLAB_WORLD

#ifdef PK_MPI_ALL
#ifndef FFT_IN_ONE_TASK_ONLY
  #define FFTW_MPI_SLAB_WORLD
#endif
#endif

#ifdef FFTW_MPI_SLAB_WORLD
  #include <BlueMatter/rfftw_mpi.h>
  typedef rfftwnd_mpi_plan fftw_plan_type;

  // NOTE: THIS CAN BE CHANGED HERE (BY COMMENTING OUT) TO HAVE NORMAL ORDER INSTEAD
  #define MPI_USE_TRANSPOSED_ORDER

#else

  #ifdef FFTW_THREADED
    #include <BlueMatter/rfftw_threads.h>
  #else
    #include <BlueMatter/rfftw.h>
  #endif

  typedef rfftwnd_plan fftw_plan_type;

#endif

template < class FFT_PLAN, int FWD_REV >
class FFTW2P3ME
{
public:
  
    typedef FFT_PLAN FFT_PLAN_IF;
  
    //    int Init( int aGlobalXsize, int aGlobalYsize, int aGlobalZsize );
    int Init( FFTW2P3ME** aStaticThisPtr, int aGlobalXsize, int aGlobalYsize, int aGlobalZsize );
    void GetLocalDimensions( int& aLocalX, int& aLocalSizeX, 
                             int& aLocalY, int& aLocalSizeY, 
                             int& aLocalZ, int& aLocalSizeZ, 
                             int& aLocalRecipX, int& aLocalRecipSizeX, 
                             int& aLocalRecipY, int& aLocalRecipSizeY, 
                             int& aLocalRecipZ, int& aLocalSizeRecipZ );
    
//     void ForwardTransform();
//     void InverseTransform();
    
    void DoFFT();

    inline void ZeroRealSpace();
    inline double GetRealSpaceElement( int x, int y, int z );
    inline void PutRealSpaceElement( int x, int y, int z, double );
    inline complex GetRecipSpaceElement( int kx, int ky, int kz );
    inline void PutRecipSpaceElement( int kx, int ky, int kz, complex );
    
private:
    
    int localX, localY, localZ, localXsize, localYsize, localZsize, 
        localRecipX, localRecipY, localRecipZ,
        localRecipXsize, localRecipYsize, localRecipZsize;

    int total_local_size; // this is the allocation size for the mpi fftw version, cdata

    t3D<double> rdata;
    t3D<complex> cdata;
    
    fftw_plan_type plan;
    //    fftw_plan_type plan;

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

   int GlobalXsize;
   int GlobalYsize;
   int GlobalZsize;
};

//int FFTW2P3ME::Init( int aGlobalXsize, int aGlobalYsize, int aGlobalZsize )
template < class FFT_PLAN, int FWD_REV >
int FFTW2P3ME< FFT_PLAN, FWD_REV >::Init( FFTW2P3ME** aStaticThisPtr, int aGlobalXsize, int aGlobalYsize, int aGlobalZsize )
{

//   int GlobalXsize = FFT_PLAN::GLOBAL_SIZE_X;
//   int GlobalYsize = FFT_PLAN::GLOBAL_SIZE_Y;
//   int GlobalZsize = FFT_PLAN::GLOBAL_SIZE_Z;
   GlobalXsize = aGlobalXsize;
   GlobalYsize = aGlobalYsize;
   GlobalZsize = aGlobalZsize;

#ifdef FFTW_THREADED
    fftw_threads_init();
#endif

//--------------  CreatePlan();

#ifdef FFTW_MPI_SLAB_WORLD
    plan = rfftw3d_mpi_create_plan( MPI_COMM_WORLD,
#else  // works for threaded too
    plan = rfftw3d_create_plan( 
#endif
        GlobalXsize, GlobalYsize, GlobalZsize, 
        ( FWD_REV == PK_FORWARD ) ? FFTW_REAL_TO_COMPLEX : FFTW_COMPLEX_TO_REAL, 
        FFTW_IN_PLACE | FFTW_MEASURE );
                                           
// #ifdef FFTW_MPI_SLAB_WORLD
//     inversePlan = rfftw3d_mpi_create_plan( MPI_COMM_WORLD,
// #else  // works for threaded too
//     inversePlan = rfftw3d_create_plan(
// #endif
//         GlobalXsize, GlobalYsize, GlobalZsize, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE | FFTW_MEASURE );

//-------------- end of  CreatePlan();
    
#ifdef FFTW_MPI_SLAB_WORLD
    rfftwnd_mpi_local_sizes( FFTW2P3ME<FFT_PLAN,FWD_REV>::plan, 
                             &localXsize, &localX,
                             &localRecipYsize, &localRecipY,
                             &total_local_size );
    
    localY = 0;
    localYsize = GlobalYsize;
    localZ = 0;
    localZsize = GlobalZsize;

    localRecipX  = localX;
    localRecipXsize = localXsize;
    localRecipZ = localZ;
    localRecipZsize = localZsize;

    // if transposed order - then x reciprocal is 0..GlobalSizeZ
    // and reciprocal y is from the function call
    // else if direct order - then x reciprocal is the same as for real space
    // while y is not from the function call but 0..GlobalSizeY
    // the meaning of X and Y is geometrical, the word transposed is related
    // to how it is stored in memory, which is taken care of when accessing it
  #ifdef MPI_USE_TRANSPOSED_ORDER
    localRecipX = 0;
    localRecipXsize = GlobalXsize;
  #else
    localRecipY = 0;
    localRecipYsize = GlobalYsize;
  #endif
#else
    localX = 0;
    localXsize = GlobalXsize;
    localY = 0;
    localYsize = GlobalYsize;
    localZ = 0;
    localZsize = GlobalZsize;

    localRecipX = 0;
    localRecipXsize = GlobalXsize;
    localRecipY = 0;
    localRecipYsize = GlobalYsize;
    localRecipZ = 0;
    localRecipZsize = GlobalZsize;

    total_local_size = localXsize * localYsize * 2*(localZsize/2+1);
#endif

#if defined( PK_BGL )
    double *ptr1;// = new double[total_local_size];
    pkNew( &ptr1, total_local_size, __FILE__, __LINE__ );
    void* ptr = (void *) ptr1;
#else
    void* ptr = new double[total_local_size]; 
#endif
    assert( ptr );
    if( !ptr )
        return -1;
   

    rdata.Init( localXsize, localYsize, 2*( localZsize/2 + 1 ), ptr );
#ifdef MPI_USE_TRANSPOSED_ORDER
    // same dimensions in x and y dirs as rdata, just transposed
    // the range is determined by localRecip?size
    cdata.Init( localRecipYsize, localRecipXsize, localRecipZsize/2 + 1, ptr );
#else
    cdata.Init( localRecipXsize, localRecipYsize, localRecipZsize/2 + 1, ptr );
#endif
    
    return 0;
}


template < class FFT_PLAN, int FWD_REV >
void FFTW2P3ME< FFT_PLAN, FWD_REV >::GetLocalDimensions( int& aLocalX, int& aLocalSizeX, 
                                                         int& aLocalY, int& aLocalSizeY, 
                                                         int& aLocalZ, int& aLocalSizeZ, 
                                                         int& aLocalRecipX, int& aLocalRecipSizeX, 
                                                         int& aLocalRecipY, int& aLocalRecipSizeY, 
                                                         int& aLocalRecipZ, int& aLocalRecipSizeZ )
{
    aLocalX = localX;
    aLocalSizeX = localXsize;
    aLocalY = localY;
    aLocalSizeY = localYsize;
    aLocalZ = localZ;
    aLocalSizeZ = localZsize;

    aLocalRecipX = localRecipX;
    aLocalRecipSizeX = localRecipXsize;
    aLocalRecipY = localRecipY;
    aLocalRecipSizeY = localRecipYsize;
    aLocalRecipZ = localRecipZ;
    aLocalRecipSizeZ = localRecipZsize/2+1;
}

template < class FFT_PLAN, int FWD_REV >
void FFTW2P3ME< FFT_PLAN, FWD_REV >::DoFFT()
{
    if( FWD_REV == PK_FORWARD )
      {
    
#ifdef FFTW_MPI_SLAB_WORLD
  #ifdef MPI_USE_TRANSPOSED_ORDER
    rfftwnd_mpi( plan, 1, rdata, 0, FFTW_TRANSPOSED_ORDER );
  #else
    rfftwnd_mpi( plan, 1, rdata, 0, FFTW_NORMAL_ORDER );
  #endif
#else
  #ifdef FFTW_THREADED
    rfftwnd_threads_one_real_to_complex( FFTW_NTHREADS, plan, rdata, 0 /*(fftw_complex*)(Complex*)cdata*/ );

// //char* fftwWisdomStr = fftw_export_wisdom_to_string();
// //cout << fftwWisdomStr << endl;
// //fftw_free(fftwWisdomStr);

  #else
    rfftwnd_one_real_to_complex( plan, rdata, 0 /*(fftw_complex*)(Complex*)cdata*/ );
  #endif
#endif
      }
    else
      {
// template < class FFT_PLAN, int FWD_REV >
// void FFTW2P3ME<FFT_PLAN,FWD_REV>::InverseTransform()
// {
#ifdef FFTW_MPI_SLAB_WORLD
  #ifdef MPI_USE_TRANSPOSED_ORDER
    rfftwnd_mpi( plan, 1, (fftw_real*)(Complex*)cdata, 0, FFTW_TRANSPOSED_ORDER );
  #else
    rfftwnd_mpi( plan, 1, (fftw_real*)(Complex*)cdata, 0, FFTW_NORMAL_ORDER );
  #endif
#else
  #ifdef FFTW_THREADED
    rfftwnd_threads_one_complex_to_real( FFTW_NTHREADS, plan, (fftw_complex*)(Complex*)cdata, 0 /*data*/ );
  #else
    rfftwnd_one_complex_to_real( plan, (fftw_complex*)(Complex*)cdata, 0 /*data*/ );
  #endif
#endif
      }
}

template < class FFT_PLAN, int FWD_REV >
inline void FFTW2P3ME<FFT_PLAN,FWD_REV>::ZeroRealSpace()
{
    rdata.Zero();
}

template < class FFT_PLAN, int FWD_REV >
inline double FFTW2P3ME<FFT_PLAN,FWD_REV>::GetRealSpaceElement( int x, int y, int z )
{
    return rdata( x, y, z );
}

template < class FFT_PLAN, int FWD_REV >
inline void FFTW2P3ME<FFT_PLAN,FWD_REV>::PutRealSpaceElement( int x, int y, int z, double d )
{
    rdata( x, y, z ) = d;
}

template < class FFT_PLAN, int FWD_REV >
inline complex FFTW2P3ME<FFT_PLAN,FWD_REV>::GetRecipSpaceElement( int x, int y, int z )
{
#ifdef MPI_USE_TRANSPOSED_ORDER
    return cdata( y, x, z );
#else
    return cdata( x, y, z );
#endif
}

template < class FFT_PLAN, int FWD_REV >
inline void FFTW2P3ME<FFT_PLAN,FWD_REV>::PutRecipSpaceElement( int x, int y, int z, complex c )
{
#ifdef MPI_USE_TRANSPOSED_ORDER
    cdata( y, x, z ) = c;
#else
    cdata( x, y, z ) = c;
#endif
}

template < class FFT_PLAN, int FWD_REV >
inline void FFTW2P3ME<FFT_PLAN,FWD_REV>::GetGridOwnerIDList( const iXYZ& vmin, const iXYZ& vmax, int idlist[], int& idnum )
{
//     const int GlobalXSize = FFT_PLAN::GLOBAL_SIZE_X;
//     const int GlobalYSize = FFT_PLAN::GLOBAL_SIZE_Y;
//     const int GlobalZSize = FFT_PLAN::GLOBAL_SIZE_Z;

//    int GlobalXsize = aGlobalXsize;
//    int GlobalYsize = aGlobalYsize;
//    int GlobalZsize = aGlobalZsize;

    iXYZ vmi = vmin;
    iXYZ vma = vmax;

    // map back into central cell
    if( vmi.mX < 0 ) vmi.mX += GlobalXsize;
    if( vmi.mY < 0 ) vmi.mY += GlobalYsize;
    if( vmi.mZ < 0 ) vmi.mZ += GlobalZsize;

    if( vma.mX >= GlobalXsize ) vma.mX -= GlobalXsize;
    if( vma.mY >= GlobalYsize ) vma.mY -= GlobalYsize;
    if( vma.mZ >= GlobalZsize ) vma.mZ -= GlobalZsize;

    int procs_x, procs_y, procs_z; 
    Platform::Topology::GetDimensions( &procs_x, &procs_y, &procs_z );

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

template < class FFT_PLAN, int FWD_REV >
inline int FFTW2P3ME<FFT_PLAN,FWD_REV>::InitProcMesh( int grdvoxleft_X, int grdvoxleft_Y, int grdvoxleft_Z )
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


#endif

