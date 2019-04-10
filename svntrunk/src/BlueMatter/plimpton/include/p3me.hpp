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
 #ifndef _P3ME_HPP_
#define _P3ME_HPP_

#define COMPLEX_TYPE_THINGY complex<FFT_TYPE>
#define COMPLEX_MEMBER_IM_THINGY(X) cimag(X)
#define COMPLEX_MEMBER_RE_THINGY(X) creal(X)

#ifndef PKFXLOG_P3MEFINDFIELD
#define PKFXLOG_P3MEFINDFIELD ( 0 )
#endif

#ifndef  PKFXLOG_P3MEASSIGNCHARGE
#define PKFXLOG_P3MEASSIGNCHARGE ( 0 )
#endif

#ifndef PKFXLOG_BOXSIZE
#define PKFXLOG_BOXSIZE ( 0 )
#endif

#ifndef PKFXLOG_REGRESS_UDF_RESULTS
#define PKFXLOG_REGRESS_UDF_RESULTS ( 0 )
#endif

#ifndef PKFXLOG_MESHED_CHARGE
#define PKFXLOG_MESHED_CHARGE ( 0 )
#endif

#ifndef PKFXLOG_AFTER_FWD_FFT
#define PKFXLOG_AFTER_FWD_FFT ( 0 )
#endif

#include <BlueMatter/UDF_Binding.hpp>


static TraceClient P3MEConvolveStart;
static TraceClient P3MEConvolveFinis;

static TraceClient P3MEReverseFFTStart;
static TraceClient P3MEReverseFFTFinis;

static TraceClient AssignChargeDetails1Start;
static TraceClient AssignChargeDetails1Finis;

static TraceClient AssignChargeDetails2Start;
static TraceClient AssignChargeDetails2Finis;

static TraceClient AssignChargeDetails3Start;
static TraceClient AssignChargeDetails3Finis;

static TraceClient AssignChargeDetails4Start;
static TraceClient AssignChargeDetails4Finis;

static TraceClient AssignChargeDetails5Start;
static TraceClient AssignChargeDetails5Finis;

static TraceClient AssignChargeDetails6Start;
static TraceClient AssignChargeDetails6Finis;


//-------------------------------------------------------------
//
//               FFT INTERFACE CLASS
//
//-------------------------------------------------------------


// this uses FFTW library
//#include <BlueMatter/fftw2p3me.hpp>
#ifdef log
#undef log
#endif
#define log Math::hlog

#ifdef exp
#undef exp
#endif
#define exp Math::hexp


#ifdef nearest
#undef nearest
#endif
#define nearest(x) ((int) ( x + 0.5))


#ifdef floor
#undef floor
#error Stop messing up with math functions!!!
#endif

// yuzh march-05: this floor function is incorrect and causes wrong results in the verlet list site selection
// #define floor(X) (((int)((X)+1000000000.0)) - 1000000000)



//------------------------------------------------------------------

// LANGUAGE:
// BOX stands for the element of volume that is periodically replicated
// GRID VOXEL stands for part of BOX
// GRID CELL stands for part of VOXEL of size spacing^3



//----------------------------------------------------------------------------------------//
//-      P3ME cache - to recycle the weights from AssignCharge to InterpolateFields      -//

template<class T>
class tFifoCache
{
public:

    void init( size_t aBufSize ) { sz = aBufSize; buf = new T[sz]; assert( buf != 0 ); reset(); }
    inline void reset() { rcnt = wcnt = 0; }
    inline void operator<<( const T& a ) { assert( wcnt >= 0 && wcnt < sz); buf[wcnt++] = a; }
    inline void operator>>( T& a ) { assert( rcnt >= 0 && rcnt < sz); a = buf[rcnt++]; }

private:

    T *buf ;//MEMORY_ALIGN( 5 );
    size_t sz; //MEMORY_ALIGN( 5 );

    int rcnt; //MEMORY_ALIGN(5);
    int wcnt; //MEMORY_ALIGN(5);
};

//------------------------------------------------------------------------------------------//


template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF >
class P3ME
{
public:


    static FFT2P3ME_FWD_IF* FFT_FWD_IF MEMORY_ALIGN(5);
    static FFT2P3ME_REV_IF* FFT_REV_IF MEMORY_ALIGN(5);
    static FFT2P3ME_REV_IF* FieldmX MEMORY_ALIGN(5);
    static FFT2P3ME_REV_IF* FieldmY MEMORY_ALIGN(5);
    static FFT2P3ME_REV_IF* FieldmZ MEMORY_ALIGN(5);


    // keeps the invariable part of the ewald self energy term
    static double EwaldSelfEnergyPrefactor ;// MEMORY_ALIGN(5);

    //--
    //-- class Grid contains global characteristics of the p3me grid in real space
    //--

    class Grid
    {
    public:

        static XYZ spacing ; //MEMORY_ALIGN(5);       // grid spacing after adjustment for optimization
        static XYZ spacing_1 ; // MEMORY_ALIGN(5);     // inverse spacing

        static XYZ boxSize ; // MEMORY_ALIGN(5);      // just a local copy of the BoundingBox (constant throught a simulation)
        static iXYZ iBoxSize ;//MEMORY_ALIGN(5);    // global size of the grid in units of spacing
        static XYZ iBoxSize_1 ;//MEMORY_ALIGN(5);   // == XYZ(1.0/iBoxSize.mX, 1.0/iBoxSize.mY, 1.0/iBoxSize.mZ)

        inline static void Init(int aGlobalFFTMeshX, int aGlobalFFTMeshY, int aGlobalFFTMeshZ);

    private:

        // this routine, now obsolete (march 3, 2005), was used to compute spacing that gave
        // optimal grid size in powers of 2, 3, ... for fftw performance
        inline static int opt_num_pts(double boxsz, double approx_spacing);
    };

    //--
    //-- class GridVoxel contains local node info of the portion of the grid that the processor owns
    //--

    class GridVoxel
    {
    public:

        static iXYZ iLeftCorner ;//MEMORY_ALIGN(5);  // in units of spacing
        static iXYZ iLocalGridSize ;//MEMORY_ALIGN(5); // in units of spacing. Note: in case of fftw varies from node to node
        static XYZ RelMinCorner ;//MEMORY_ALIGN(5); // left corner ratio to box size
        static XYZ RelMaxCorner ;//MEMORY_ALIGN(5); // right " " " "

        static int GridNotEmpty ;//MEMORY_ALIGN(5); // if one of the iLocalGridSize is zero - nothing to do in real space. Note: relevant for fftw
    };

    //--
    //-- class ParticleMesh - data and methods for the parts of p3me algorithm related to charge meshing and force/potential unmeshing
    //--

    class ParticleMesh
    {
    public:

        // charge assignment schemes
        // NGP - nearest grid point (1-point asignment)
        // CIC - charge in cloud (2-point asignment)
        // TSC - trianguar shaped cloud cloud (3-point asignment)
        // a hack is used to initialize numAssignPoints to (assignScheme+1)
        enum { NGP, CIC, TSC, A4P };
        static int assignScheme ;//MEMORY_ALIGN(5);    // assumes one of the enum values above
        static int numAssignPoints; //MEMORY_ALIGN(5); // number of nodes in each dimension that each charge is assigned to

        // each of these flags is set if the size of local real space mesh in that dimension is equial to the global mesh size
        // this flag was introduces after a round off error caused a particle to be dropped from the p3me verlet list march-3-05
        static iXYZ wrapped_dim ;//MEMORY_ALIGN(5);

        inline static void Init( int scheme, int difford );
        inline static XYZ GetVoxelAddExtent();
        inline static void InitVoxelAddExtent();  // computes VoxelAddExtent
        inline static void ZeroGridCharge();      // zeros real space grid
        inline static int  IsFragmentInRange( SourceIF& SourceFragmentIF ); // checks if a fragment can have sites local to this node

        inline static void GetLocalPaddedMeshDimensions( XYZ& vmin, XYZ& vmax ); // particles inside this box will contribute to local mesh

        // takes a box in real ("simulation") space, returns a list of node id's that it is shared by
        inline static void GetNodeIDList( const XYZ& vmin, const XYZ& vmax, int idlist[], int& idnum );

        // resets cache for the weight recycling between charge assignment and field interpolation
        inline static void ResetCache(){ intfifo.reset(); doublefifo.reset(); }

        // assignes a charged particle to the local grid
        inline static int AssignCharge( const XYZ& coor, const double charge, const int siteId, const int aSimTick );

        // main one, "does" p3me: calls forward fft(s), convolution functions, inverse fft(s)
        inline static void FindFields( XYZ& aVirial, const int aSimTick  );

        // applies fields on the grid to individual particles
        inline static int InterpolateFields( const XYZ& coor, const double charge, XYZ& force, double& energy, const int siteId, const int aSimTick );

    private:

        static XYZ VoxelAddExtent; //MEMORY_ALIGN(5); // due to charge assign to a range of cites - used to screan particles

        // cache to recycle weights between charge assignment step and field interpolation
        static tFifoCache<int> intfifo; //MEMORY_ALIGN(5);
        static tFifoCache<double> doublefifo; //MEMORY_ALIGN(5);

        // for FINITE difference method of computing forces from potentials
        // OBSOLETE (mar-3-05): has not been supported for parallel versions of the code
        // compute mesh electric field from mesh potential, for finite diff method only
        inline static void FiniteDiffEfield_X();
        inline static void FiniteDiffEfield_Y();
        inline static void FiniteDiffEfield_Z();
        inline static void FiniteDiffEfield();

        // finds the site(node) on the grid that serves as "reference" for mapping a particle onto the grid mesh
        inline static void FindReferenceSite( const XYZ& coord, iXYZ& refSite, XYZ& relDist );
        // computes weights based on the distance of a particle from the reference grid point
        inline static void GetWeights( double relDist, double w[] );
        // computes weights for force computation based on the distance of a particle from the reference grid point
        inline static void GetGradWeights( double relDist, double dw[] );
    };

    //--
    //-- class Differentiation - for methods to get forces from potentials
    //-- now (march-05) only keeps the differentiation method flag ANALYTICAL vs GRADIENT
    //-- the finite diff is obsolete and not implemented for parallel runs
    //--

    class Differentiation
    {
    public:

        // number of two-point differential operators used to approximate
        // a partial derivative
//         enum { GRADIENT = -1, ANALYTICAL = 0, FINITE_2PT = 1, FINITE_4PT = 2,
//                FINITE_6PT = 3, FINITE_8PT = 4, FINITE_10PT = 5, FINITE_12PT = 6 };
        static int diffOrder; //MEMORY_ALIGN(5);

        static double *coeff; //MEMORY_ALIGN(5);

        inline static void Init( int difforder ) { diffOrder = difforder; initcoeff(); }

    private:

        inline static void initcoeff();
        inline static void deletecoeff() { delete[] coeff; coeff = NULL; }
    };

    //--
    //-- class InKSpace - Green function initialization and convolution (and virial computation) in reciprocal space
    //--

    class InKSpace
    {
    public:

        static void Init();
        static void Convolute( XYZ& aVirial, const int aSimTick );
        static void ConvoluteKx();
        static void ConvoluteKy();
        static void ConvoluteKz();

        // these define the range of indices in k-space on the local node
        // THE ORDER IS STORAGE BASED. That is, if the fft result is transposed
        // (as in the case of mpi fftw), then *.mX gives the range for the first
        // index in the array, but the meaning of this first index is ky.
        // This should be taken into account when greenFun is initialized.
        // For the convolution, it does not matter in this case.
        static iXYZ iLeftCorner; //MEMORY_ALIGN(5);
        static iXYZ iLocalGridSize; //MEMORY_ALIGN(5);

        static int GridNotEmpty; //MEMORY_ALIGN(5); // if one of the iLocalGridSize is zero - nothing to do in real space

    private:

        // calculates sum_j c_j*sin(jkh)/kh
        inline static double Dfinite( double K_i, double aSpacing );

        inline static double Wsqr( double Kh );
        inline static double SumW2( double cosy, int ord );
        inline static void  SetGopt();

        static t3D<double>  greenFun; //MEMORY_ALIGN(5);

        static t1D<double>  SumDmuWVcX; //MEMORY_ALIGN(5);
        static t1D<double>  SumDmuWVcY; //MEMORY_ALIGN(5);
        static t1D<double>  SumDmuWVcZ; //MEMORY_ALIGN(5);

        static t1D<double>  SumWVcX;// MEMORY_ALIGN(5);
        static t1D<double>  SumWVcY;// MEMORY_ALIGN(5);
        static t1D<double>  SumWVcZ;// MEMORY_ALIGN(5);
    };

    inline static void PreInit(int aGlobalFFTMeshX, int aGlobalFFTMeshY, int aGlobalFFTMeshZ);

    inline static void Init();

    inline static void InitEwaldSelfEnergy();

    inline static void RunForwardFFT();

    inline static int HasFFTHeaderChecksumChanged();
};

//template<class SourceIF, class ResultIF, class FFT2P3MEInterface>
template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF >
FFT2P3ME_FWD_IF* P3ME<SourceIF, ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::FFT_FWD_IF;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF >
FFT2P3ME_REV_IF* P3ME<SourceIF, ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::FFT_REV_IF;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF >
FFT2P3ME_REV_IF* P3ME<SourceIF, ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::FieldmX;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF >
FFT2P3ME_REV_IF* P3ME<SourceIF, ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::FieldmY;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF >
FFT2P3ME_REV_IF* P3ME<SourceIF, ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::FieldmZ;

// template<class SourceIF, class ResultIF, class FFT2P3MEInterface>
// FFT2P3MEInterface P3ME<SourceIF, ResultIF, FFT2P3MEInterface>::FieldmX;

// template<class SourceIF, class ResultIF, class FFT2P3MEInterface>
// FFT2P3MEInterface P3ME<SourceIF, ResultIF, FFT2P3MEInterface>::FieldmY;

// template<class SourceIF, class ResultIF, class FFT2P3MEInterface>
// FFT2P3MEInterface P3ME<SourceIF, ResultIF, FFT2P3MEInterface>::FieldmZ;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
double P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::EwaldSelfEnergyPrefactor;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
XYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::Grid::spacing;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
XYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::Grid::spacing_1;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
XYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::Grid::boxSize;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
iXYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::Grid::iBoxSize;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
XYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::Grid::iBoxSize_1;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
iXYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::GridVoxel::iLocalGridSize;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
XYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::GridVoxel::RelMinCorner;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
XYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::GridVoxel::RelMaxCorner;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
iXYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::GridVoxel::iLeftCorner;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
int P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::GridVoxel::GridNotEmpty;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
int P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::ParticleMesh::assignScheme;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
int P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::ParticleMesh::numAssignPoints;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
iXYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::ParticleMesh::wrapped_dim;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
XYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::ParticleMesh::VoxelAddExtent;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
tFifoCache<int> P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::ParticleMesh::intfifo;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
tFifoCache<double> P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::ParticleMesh::doublefifo;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
int P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::Differentiation::diffOrder;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
double* P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::Differentiation::coeff;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
iXYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::InKSpace::iLeftCorner;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
iXYZ P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::InKSpace::iLocalGridSize;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
t3D<double> P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::InKSpace::greenFun;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
t1D<double> P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::InKSpace::SumDmuWVcX;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
t1D<double> P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::InKSpace::SumDmuWVcY;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
t1D<double> P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::InKSpace::SumDmuWVcZ;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
t1D<double> P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::InKSpace::SumWVcX;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
t1D<double> P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::InKSpace::SumWVcY;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
t1D<double> P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::InKSpace::SumWVcZ;

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
int P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>::InKSpace::GridNotEmpty;



template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
int P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::HasFFTHeaderChecksumChanged()
{
  unsigned fwdCheckSum = FFT_FWD_IF->GetHeadersChecksum();
  unsigned revCheckSum = FFT_REV_IF->GetHeadersChecksum();

  int fwdFFTHeaderChange = FFT_FWD_IF->HasHeaderChecksumChanged();
  int revFFTHeaderChange = FFT_REV_IF->HasHeaderChecksumChanged();

  BegLogLine( 0 )
    << "HasFFTHeaderChecksumChanged():: "
    << " fwdFFTHeaderChange=" << fwdFFTHeaderChange
    << " revFFTHeaderChange=" << revFFTHeaderChange
    << " fwdFFTHeaderCheckSum=" << (int) fwdCheckSum
    << " revFFTHeaderCheckSum=" << (int) revCheckSum
    << EndLogLine;

  return ( fwdFFTHeaderChange && revFFTHeaderChange );
}


template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::RunForwardFFT()
{
  FFT_FWD_IF->DoFFT();
}

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::InitEwaldSelfEnergy()
{
    // selfenergy prefactor
    {
        EwaldSelfEnergyPrefactor = 0.;

        for ( int AbsSiteId = 0;
              AbsSiteId < DynVarMgrIF.GetNumberOfSites();
              AbsSiteId++ )

            if( DynVarMgrIF.IsSiteLocal( AbsSiteId ) )
            {
                double cite_charge = MSD_IF::GetSiteInfo( AbsSiteId ).charge;
                EwaldSelfEnergyPrefactor -= cite_charge * cite_charge;
            }

        EwaldSelfEnergyPrefactor *= Math::InvSqrtPI;
    }
}

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::Init()
{
    iXYZ FFTMeshCoords;
    FFTMeshCoords.mX = FFT_PLAN::GLOBAL_SIZE_X;
    FFTMeshCoords.mY = FFT_PLAN::GLOBAL_SIZE_Y;
    FFTMeshCoords.mZ = FFT_PLAN::GLOBAL_SIZE_Z;
    FFTMeshCoords.ReOrderAbs( RTG.mBoxIndexOrder );
    int FFTMeshX = FFTMeshCoords.mX;
    int FFTMeshY = FFTMeshCoords.mY;
    int FFTMeshZ = FFTMeshCoords.mZ;

    // Grid setup
    {
        // initial value for spacing to be read from RTP file
        /// XYZ gridspacing0 = MSD_IF::GetP3MEinitSpacing();

        Grid::Init( FFTMeshX, FFTMeshY, FFTMeshZ );
    }

    // class Differentiation
    {
        int difforder = MSD_IF::GetP3MEdiffOrder();
        Differentiation::Init( difforder );
    }

    // particle mesh
//     {
//         ParticleMesh::Init( MSD_IF::GetP3MEchargeAssignment(), MSD_IF::GetP3MEdiffOrder() );
//     }

    PreInit( FFTMeshX, FFTMeshY, FFTMeshZ );
}



template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::PreInit( int aGlobalFFTMeshX, int aGlobalFFTMeshY, int aGlobalFFTMeshZ )
{
    // //
    //FFT_IF.Init( Grid::iBoxSize.mX, Grid::iBoxSize.mY, Grid::iBoxSize.mZ );
    int FFTMeshX = aGlobalFFTMeshX;
    int FFTMeshY = aGlobalFFTMeshY;
    int FFTMeshZ = aGlobalFFTMeshZ;

    pkNew( &FFT_FWD_IF, 1, __FILE__, __LINE__, 1 );

    BegLogLine( 0 )
      << "P3ME::Init() "
      << " FFT_FWD_IF=" << (void *) FFT_FWD_IF
      << " &FFT_FWD_IF=" << (void *) &FFT_FWD_IF
      << EndLogLine;

    FFT_FWD_IF->Init( &FFT_FWD_IF, FFTMeshX, FFTMeshY, FFTMeshZ, -1, -1, -1 );

    FFT_FWD_IF->GetLocalDimensions( GridVoxel::iLeftCorner.mX, GridVoxel::iLocalGridSize.mX,
                                    GridVoxel::iLeftCorner.mY, GridVoxel::iLocalGridSize.mY,
                                    GridVoxel::iLeftCorner.mZ, GridVoxel::iLocalGridSize.mZ,
                                    InKSpace::iLeftCorner.mX,  InKSpace::iLocalGridSize.mX,
                                    InKSpace::iLeftCorner.mY,  InKSpace::iLocalGridSize.mY,
                                    InKSpace::iLeftCorner.mZ,  InKSpace::iLocalGridSize.mZ
                                    );
    FFT_FWD_IF->InitProcMesh( GridVoxel::iLeftCorner.mX, GridVoxel::iLeftCorner.mY, GridVoxel::iLeftCorner.mZ);

//     FFT_REV_IF.GetLocalDimensions( GridVoxel::iLeftCorner.mX, GridVoxel::iLocalGridSize.mX,
//                                    GridVoxel::iLeftCorner.mY, GridVoxel::iLocalGridSize.mY,
//                                    GridVoxel::iLeftCorner.mZ, GridVoxel::iLocalGridSize.mZ,
//                                    InKSpace::iLeftCorner.mX,  InKSpace::iLocalGridSize.mX,
//                                    InKSpace::iLeftCorner.mY,  InKSpace::iLocalGridSize.mY,
//                                    InKSpace::iLeftCorner.mZ,  InKSpace::iLocalGridSize.mZ
//                                    );

    GridVoxel::RelMinCorner.mX = GridVoxel::iLeftCorner.mX * Grid::iBoxSize_1.mX;
    GridVoxel::RelMinCorner.mY = GridVoxel::iLeftCorner.mY * Grid::iBoxSize_1.mY;
    GridVoxel::RelMinCorner.mZ = GridVoxel::iLeftCorner.mZ * Grid::iBoxSize_1.mZ;

    GridVoxel::RelMaxCorner.mX = GridVoxel::RelMinCorner.mX + GridVoxel::iLocalGridSize.mX * Grid::iBoxSize_1.mX;
    GridVoxel::RelMaxCorner.mY = GridVoxel::RelMinCorner.mY + GridVoxel::iLocalGridSize.mY * Grid::iBoxSize_1.mY;
    GridVoxel::RelMaxCorner.mZ = GridVoxel::RelMinCorner.mZ + GridVoxel::iLocalGridSize.mZ * Grid::iBoxSize_1.mZ;


    double low[3];
    double high[3];

    low[ 0 ] = GridVoxel::iLeftCorner.mX * Grid::spacing.mX;
    low[ 1 ] = GridVoxel::iLeftCorner.mY * Grid::spacing.mY;
    low[ 2 ] = GridVoxel::iLeftCorner.mZ * Grid::spacing.mZ;

    high[ 0 ] = ( GridVoxel::iLeftCorner.mX + GridVoxel::iLocalGridSize.mX ) * Grid::spacing.mX;
    high[ 1 ] = ( GridVoxel::iLeftCorner.mY + GridVoxel::iLocalGridSize.mY ) * Grid::spacing.mY;
    high[ 2 ] = ( GridVoxel::iLeftCorner.mZ + GridVoxel::iLocalGridSize.mZ ) * Grid::spacing.mZ;

    double Volume = ( high[ 0 ] - low[ 0 ] )
      * ( high[ 1 ] - low[ 1 ] )
      * ( high[ 2 ] - low[ 2 ] );

    BegLogLine( 0 )
      << "P3ME->low: { "
      << low[ 0 ] << " "
      << low[ 1 ] << " "
      << low[ 2 ] << " }"
      << EndLogLine;

    BegLogLine( 0 )
      << "P3ME->high: { "
      << high[ 0 ] << " "
      << high[ 1 ] << " "
      << high[ 2 ] << " }"
      << " P3MEVolume= " << Volume
      << EndLogLine;

    GridVoxel::GridNotEmpty =
        GridVoxel::iLocalGridSize.mX * GridVoxel::iLocalGridSize.mY * GridVoxel::iLocalGridSize.mZ;
    InKSpace::GridNotEmpty =
        InKSpace::iLocalGridSize.mX * InKSpace::iLocalGridSize.mY * InKSpace::iLocalGridSize.mZ;

    pkNew(&FFT_REV_IF, 1, __FILE__, __LINE__, 1);
    FFT_REV_IF->Init( &FFT_REV_IF, FFTMeshX, FFTMeshY, FFTMeshZ, -1, -1, -1 );

    if( Differentiation::diffOrder == PreMSD::ANALYTICAL )
      {
      pkNew(&FieldmX, 1, __FILE__, __LINE__, 1 );
      FieldmX->Init( &FieldmX, FFTMeshX, FFTMeshY, FFTMeshZ, -1, -1, -1 );

      pkNew(&FieldmY, 1, __FILE__, __LINE__, 1 );
      FieldmY->Init( &FieldmY, FFTMeshX, FFTMeshY, FFTMeshZ, -1, -1, -1 );

      pkNew(&FieldmZ, 1, __FILE__, __LINE__, 1 );
      FieldmZ->Init( &FieldmZ, FFTMeshX, FFTMeshY, FFTMeshZ, -1, -1, -1 );
      }

    {
        ParticleMesh::Init( MSD_IF::GetP3MEchargeAssignment(), MSD_IF::GetP3MEdiffOrder() );
    }

    {
        ParticleMesh::wrapped_dim.mX = ( GridVoxel::iLocalGridSize.mX + ParticleMesh::numAssignPoints > Grid::iBoxSize.mX );
        ParticleMesh::wrapped_dim.mY = ( GridVoxel::iLocalGridSize.mY + ParticleMesh::numAssignPoints > Grid::iBoxSize.mY );
        ParticleMesh::wrapped_dim.mZ = ( GridVoxel::iLocalGridSize.mZ + ParticleMesh::numAssignPoints > Grid::iBoxSize.mZ );
    }

    // InKSpace class
    {
        InKSpace::Init();
    }
}

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::Grid
::Init(int aGlobalFFTMeshX, int aGlobalFFTMeshY, int aGlobalFFTMeshZ)
{
    // just a local copy here for convenience
    boxSize = RTG.mBirthBoundingBoxDimensionVector;

    //----- SetOptimalGridSpacing
    //    XYZ spacing0 = MSD_IF::GetP3MEinitSpacing();

//     opt_num_pts( boxSize.mX, spacing0.mX );
//     printf("opt_num_pts: %d\n",  opt_num_pts( boxSize.mX, spacing0.mX )  );

//     iBoxSize.mX = FFT2P3ME_FWD_IF::FFT_PLAN_IF::GLOBAL_SIZE_X;
//     iBoxSize.mY = FFT2P3ME_FWD_IF::FFT_PLAN_IF::GLOBAL_SIZE_Y;
//     iBoxSize.mZ = FFT2P3ME_FWD_IF::FFT_PLAN_IF::GLOBAL_SIZE_Z;

    iBoxSize.mX = aGlobalFFTMeshX;
    iBoxSize.mY = aGlobalFFTMeshY;
    iBoxSize.mZ = aGlobalFFTMeshZ;

    BegLogLine( PKFXLOG_BOXSIZE )
        << "P3ME::Grid::Init:: "
        << " iBoxSize " << iBoxSize
        << EndLogLine;

    spacing.mX = boxSize.mX / iBoxSize.mX;
    spacing.mY = boxSize.mY / iBoxSize.mY;
    spacing.mZ = boxSize.mZ / iBoxSize.mZ;

    spacing_1.mX = 1./spacing.mX;
    spacing_1.mY = 1./spacing.mY;
    spacing_1.mZ = 1./spacing.mZ;

    iBoxSize_1.mX = 1./iBoxSize.mX;
    iBoxSize_1.mY = 1./iBoxSize.mY;
    iBoxSize_1.mZ = 1./iBoxSize.mZ;
}



template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::GetLocalPaddedMeshDimensions( XYZ& vmin, XYZ& vmax )
{
    vmin.mX = ( GridVoxel::iLeftCorner.mX * Grid::spacing.mX - VoxelAddExtent.mX ) / DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
    vmin.mY = ( GridVoxel::iLeftCorner.mY * Grid::spacing.mY - VoxelAddExtent.mY ) / DynVarMgrIF.mBirthBoxAbsoluteRatio.mY;
    vmin.mZ = ( GridVoxel::iLeftCorner.mZ * Grid::spacing.mZ - VoxelAddExtent.mZ ) / DynVarMgrIF.mBirthBoxAbsoluteRatio.mZ;

    vmax.mX = ( ( GridVoxel::iLeftCorner.mX + GridVoxel::iLocalGridSize.mX - 1 ) * Grid::spacing.mX + VoxelAddExtent.mX ) / DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
    vmax.mY = ( ( GridVoxel::iLeftCorner.mY + GridVoxel::iLocalGridSize.mY - 1 ) * Grid::spacing.mY + VoxelAddExtent.mY ) / DynVarMgrIF.mBirthBoxAbsoluteRatio.mY;
    vmax.mZ = ( ( GridVoxel::iLeftCorner.mZ + GridVoxel::iLocalGridSize.mZ - 1 ) * Grid::spacing.mZ + VoxelAddExtent.mZ ) / DynVarMgrIF.mBirthBoxAbsoluteRatio.mZ;
}



template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::GetNodeIDList( const XYZ& vmin, const XYZ& vmax, int idlist[], int& idnum )
{
    assert( vmax.mX >= vmin.mX && vmax.mY >= vmin.mY && vmax.mZ >= vmin.mZ );

    XYZ vmin_sc;
    vmin_sc.mX = vmin.mX * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
    vmin_sc.mY = vmin.mY * DynVarMgrIF.mBirthBoxAbsoluteRatio.mY;
    vmin_sc.mZ = vmin.mZ * DynVarMgrIF.mBirthBoxAbsoluteRatio.mZ;

    XYZ vmax_sc;
    vmax_sc.mX = vmax.mX * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
    vmax_sc.mY = vmax.mY * DynVarMgrIF.mBirthBoxAbsoluteRatio.mY;
    vmax_sc.mZ = vmax.mZ * DynVarMgrIF.mBirthBoxAbsoluteRatio.mZ;

    const XYZ mi = vmin_sc - ParticleMesh::VoxelAddExtent;
    iXYZ imi;
    imi.mX = int( ceil( mi.mX * Grid::spacing_1.mX ) );
    imi.mY = int( ceil( mi.mY * Grid::spacing_1.mY ) );
    imi.mZ = int( ceil( mi.mZ * Grid::spacing_1.mZ ) );

    const XYZ ma = vmax_sc + ParticleMesh::VoxelAddExtent;
    iXYZ ima;
    ima.mX = int( floor( ma.mX * Grid::spacing_1.mX ) );
    ima.mY = int( floor( ma.mY * Grid::spacing_1.mY ) );
    ima.mZ = int( floor( ma.mZ * Grid::spacing_1.mZ ) );

    FFT_FWD_IF->GetGridOwnerIDList( imi, ima, idlist, idnum );
}





template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
int P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::Grid
::opt_num_pts( double boxsize, double grdSp )
{
  // double min = ceil(boxsize/grdSp);
    double min = ((boxsize + grdSp - 1) / grdSp);
    int a, b, c, d;
    int amin, bmin, cmin, dmin;
    int meshPts;
    double lnmin, rema, remb, remc;
    double excess, minexcess;
    static const double ln2 = log(2.), ln3 = log(3.), ln5 = log(5.), ln7 = log(7.);
    static const double ln2_1 = 1.0/ln2, ln3_1 = 1.0/ln3,
        ln5_1 = 1.0/ln5, ln7_1 = 1.0/ln7;

    lnmin = log(min);

    amin = (int)floor(lnmin*ln2_1) + 1;
    bmin = cmin = dmin = 0;
    minexcess = amin*ln2 - lnmin;

    for( a = 0; a <= (int)floor(lnmin*ln2_1) + 1; a++ )
    {
        rema = lnmin - a*ln2;
        for( b = 0; b<=(int)floor(rema*ln3_1)+1; b++ )
        {
            remb = rema - b*ln3;
            for( c = 0; c <= (int)floor(remb*ln5_1)+1; c++ )
            {
                remc = remb - c*ln5;
                d = (int)floor(remc*ln7_1) + 1;
                excess = d*ln7 - remc;
                if (excess<minexcess) {
                    amin = a;
                    bmin = b;
                    cmin = c;
                    dmin = d;
                    minexcess = excess;
                }
            }
        }
    }

    meshPts = 1;
    for( a = 0; a < amin; a++ )
        meshPts *= 2;
    for( b = 0; b < bmin; b++ )
        meshPts *= 3;
    for( c = 0; c < cmin; c++ )
        meshPts *= 5;
    for( d = 0; d < dmin; d++ )
        meshPts *= 7;

    return meshPts;
}







template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF, ResultIF,  FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::Init( int numassignpoints, int difford )
{
    numAssignPoints = numassignpoints;
    assignScheme = numAssignPoints - 1;

#ifndef  CACHE_OFF
    intfifo.init( (numassignpoints+1) * 3 * NUMBER_OF_SITES ); //##### TOO BIG!!!!!!!!!
    doublefifo.init( numassignpoints * 2 * 3 * NUMBER_OF_SITES ); //##### TOO BIG!!!!!!!!!
#endif
}


template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::InitVoxelAddExtent()
{
    VoxelAddExtent.mX = 0.5*numAssignPoints*Grid::iBoxSize_1.mX*DynVarMgrIF.mDynamicBoxDimensionVector.mX;
    VoxelAddExtent.mY = 0.5*numAssignPoints*Grid::iBoxSize_1.mY*DynVarMgrIF.mDynamicBoxDimensionVector.mY;
    VoxelAddExtent.mZ = 0.5*numAssignPoints*Grid::iBoxSize_1.mZ*DynVarMgrIF.mDynamicBoxDimensionVector.mZ;
}

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
XYZ P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::GetVoxelAddExtent()
{
    return VoxelAddExtent;
}

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::ZeroGridCharge()
{
    FFT_FWD_IF->ZeroRealSpace();
}


template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
int P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::IsFragmentInRange( SourceIF& SourceFragmentIF )
{
    if( ! GridVoxel::GridNotEmpty )
        return 0;

    int SourceIrreducibleFragmentId = SourceFragmentIF.GetIrreducibleFragmentId();

    const int TagAtomId = MSD_IF::GetTagAtomIndex( SourceIrreducibleFragmentId );
    const XYZ TagAtomCoor = *SourceFragmentIF.GetPositionPtr( TagAtomId );
    const double FragmentExtent = MSD_IF::GetFragmentExtent( SourceIrreducibleFragmentId );

    //------------ X plane --------------
    {
        const double box_1 = DynVarMgrIF.mDynamicBoxInverseDimensionVector.mX;
        const double buff = VoxelAddExtent.mX + FragmentExtent;

        const double voxmin = GridVoxel::RelMinCorner.mX;
        const double voxmax = GridVoxel::RelMaxCorner.mX;

        const double rat = TagAtomCoor.mX * box_1;
        const double RelFragMapped = rat - floor(rat); // map tag atom into central box

        const double ratb = buff * box_1;

        const double RelLeft = RelFragMapped - ratb;
        const double RelRight = RelFragMapped + ratb;

        //      ...] o--o [  ]      [...
        // or   ...]      [  ] o--o [...

        if( ( RelRight < voxmin && RelLeft > voxmax - 1.0 )
            || ( RelLeft > voxmax && RelRight < voxmin + 1.0 ) )
        {
            return 0;
        }




// the code below does not work if fragment extent is defined bigger than the box size

//         const double box_1 = DynVarMgrIF.mDynamicBoxInverseDimensionVector.mX;
//         const double buff = VoxelAddExtent.mX + FragmentExtent;

//         const double voxmin = GridVoxel::RelMinCorner.mX;
//         const double voxmax = GridVoxel::RelMaxCorner.mX;

//         const double FragMax = TagAtomCoor.mX + buff;
//         const double FragMin = TagAtomCoor.mX - buff;

//         // map it to the central cell and convert to relative to boxsize
//         const double rat1 = FragMax * box_1;
//         const double RelFragMax = rat1 - floor(rat1);
//         const double rat2 = FragMin * box_1;
//         const double RelFragMin = rat2 - floor(rat2);

//         // now - return (0) if
//         //       o--o  [   ]            case (1)
//         // OR    ---o  [   ]  o---      case (2)
//         // OR          [   ]  o--o      case (3)
//         // otherwise the fragment can be important
//         // here Min---Max
//         //
//         // "bad" (need to compute) if
//         //             [ o-o ]
//         // OR    ------[-o   ]  o--
//         // OR    --o   [   o-]-----
//         // OR EVEN ----[-o o-]-----

//         // case (1) or (2)
//         if( ( RelFragMax < voxmin && ( RelFragMin < RelFragMax || RelFragMin > voxmax ) )
//             // case (3)
//             || ( RelFragMin > voxmax && RelFragMin < RelFragMax ) )
//         {
//             return 0;
//         }
    }

    //------------ Y plane --------------
    {
        const double box_1 = DynVarMgrIF.mDynamicBoxInverseDimensionVector.mY;
        const double buff = VoxelAddExtent.mY + FragmentExtent;

        const double voxmin = GridVoxel::RelMinCorner.mY;
        const double voxmax = GridVoxel::RelMaxCorner.mY;

        const double rat = TagAtomCoor.mY * box_1;
        const double RelFragMapped = rat - floor(rat); // map tag atom into central box

        const double ratb = buff * box_1;

        const double RelLeft = RelFragMapped - ratb;
        const double RelRight = RelFragMapped + ratb;

        //      ...] o--o [  ]      [...
        // or   ...]      [  ] o--o [...

        if( ( RelRight < voxmin && RelLeft > voxmax - 1.0 )
            || ( RelLeft > voxmax && RelRight < voxmin + 1.0 ) )
        {
            return 0;
        }



// the code below does not work if fragment extent is defined bigger than the box size

//         const double box_1 = DynVarMgrIF.mDynamicBoxInverseDimensionVector.mY;
//         const double buff = VoxelAddExtent.mY + FragmentExtent;

//         const double voxmin = GridVoxel::RelMinCorner.mY;
//         const double voxmax = GridVoxel::RelMaxCorner.mY;

//         const double FragMax = TagAtomCoor.mY + buff;
//         const double FragMin = TagAtomCoor.mY - buff;

//         // map it to the central cell and convert to relative to boxsize
//         const double rat1 = FragMax * box_1;
//         const double RelFragMax = rat1 - floor(rat1);
//         const double rat2 = FragMin * box_1;
//         const double RelFragMin = rat2 - floor(rat2);

//         // now - return (0) if
//         //       o--o  [   ]            case (1)
//         // OR    ---o  [   ]  o---      case (2)
//         // OR          [   ]  o--o      case (3)
//         // otherwise the fragment can be important
//         // here Min---Max
//         //
//         // "bad" (need to compute) if
//         //             [ o-o ]
//         // OR    ------[-o   ]  o--
//         // OR    --o   [   o-]-----
//         // OR EVEN ----[-o o-]-----

//         // case (1) or (2)
//         if( ( RelFragMax < voxmin && ( RelFragMin < RelFragMax || RelFragMin > voxmax ) )
//             // case (3)
//             || ( RelFragMin > voxmax && RelFragMin < RelFragMax ) )
//         {
//             return 0;
//         }
    }

    //------------ Z plane --------------
    {
        const double box_1 = DynVarMgrIF.mDynamicBoxInverseDimensionVector.mZ;
        const double buff = VoxelAddExtent.mZ + FragmentExtent;

        const double voxmin = GridVoxel::RelMinCorner.mZ;
        const double voxmax = GridVoxel::RelMaxCorner.mZ;

        const double rat = TagAtomCoor.mZ * box_1;
        const double RelFragMapped = rat - floor(rat); // map tag atom into central box

        const double ratb = buff * box_1;

        const double RelLeft = RelFragMapped - ratb;
        const double RelRight = RelFragMapped + ratb;

        //      ...] o--o [  ]      [...
        // or   ...]      [  ] o--o [...

        if( ( RelRight < voxmin && RelLeft > voxmax - 1.0 )
            || ( RelLeft > voxmax && RelRight < voxmin + 1.0 ) )
        {
            return 0;
        }




// the code below does not work if fragment extent is defined bigger than the box size

//         const double box_1 = DynVarMgrIF.mDynamicBoxInverseDimensionVector.mZ;
//         const double buff = VoxelAddExtent.mZ + FragmentExtent;

//         const double voxmin = GridVoxel::RelMinCorner.mZ;
//         const double voxmax = GridVoxel::RelMaxCorner.mZ;

//         const double FragMax = TagAtomCoor.mZ + buff;
//         const double FragMin = TagAtomCoor.mZ - buff;

//         // map it to the central cell and convert to relative to boxsize
//         const double rat1 = FragMax * box_1;
//         const double RelFragMax = rat1 - floor(rat1);
//         const double rat2 = FragMin * box_1;
//         const double RelFragMin = rat2 - floor(rat2);

//         // now - return (0) if
//         //       o--o  [   ]            case (1)
//         // OR    ---o  [   ]  o---      case (2)
//         // OR          [   ]  o--o      case (3)
//         // otherwise the fragment can be important
//         // here Min---Max
//         //
//         // "bad" (need to compute) if
//         //             [ o-o ]
//         // OR    ------[-o   ]  o--
//         // OR    --o   [   o-]-----
//         // OR EVEN ----[-o o-]-----

//         // case (1) or (2)
//         if( ( RelFragMax < voxmin && ( RelFragMin < RelFragMax || RelFragMin > voxmax ) )
//             // case (3)
//             || ( RelFragMin > voxmax && RelFragMin < RelFragMax ) )
//         {
//             return 0;
//         }
    }

    return 1;
}


template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::FindReferenceSite( const XYZ& coord, iXYZ& refSite, XYZ& relDist )
{
    // in case of odd numAssignPoints find the closest grid site;
    // in case of even numAssignPoints find the reference corner of the
    // cell that contains the charge

    XYZ relCoord;
    relCoord.mX = coord.mX * ( Grid::spacing_1.mX * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX );
    relCoord.mY = coord.mY * ( Grid::spacing_1.mY * DynVarMgrIF.mBirthBoxAbsoluteRatio.mY );
    relCoord.mZ = coord.mZ * ( Grid::spacing_1.mZ * DynVarMgrIF.mBirthBoxAbsoluteRatio.mZ );

    if( numAssignPoints % 2 ) {
#if PK_GCC
        refSite.mX = int(floor(relCoord.mX + 0.5));
        refSite.mY = int(floor(relCoord.mY + 0.5));
        refSite.mZ = int(floor(relCoord.mZ + 0.5));
#else
        refSite.mX = int(nearest(relCoord.mX));
        refSite.mY = int(nearest(relCoord.mY));
        refSite.mZ = int(nearest(relCoord.mZ));
#endif
    }
    else
    {
        refSite.mX = int(floor(relCoord.mX));
        refSite.mY = int(floor(relCoord.mY));
        refSite.mZ = int(floor(relCoord.mZ));
    }

    // calculate the relative distance from that site

    relDist.mX = relCoord.mX - refSite.mX;
    relDist.mY = relCoord.mY - refSite.mY;
    relDist.mZ = relCoord.mZ - refSite.mZ;
}



template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::GetWeights( double relDist, double weight[] )
{
    switch( assignScheme )
    {
    case NGP:
        weight[0] = 1.0;
        break;

    case CIC:
        weight[0] = 1.0 - relDist;
        weight[1] = relDist;
        break;

    case TSC:
    {
        const double inv8 = 0.125;
        const double tworeldist = 2.* relDist;
        weight[0] = sqr(1.0-tworeldist)*inv8;
        weight[1] = 0.75 - sqr(relDist);
        weight[2] = sqr(1.0+tworeldist)*inv8;
        break;
    }

    case A4P:
    {
        const double inv48 = 1./48.;
        const double d = relDist - 0.5;
        const double d2 = d * d;
        const double d3 = d2 * d;
        const double ad2 = 12.0*d2;
        const double ad2add1 = ad2 + 1.0;
        const double ad2m23 = 23.0 - ad2;
        const double eightd3 = 8.0*d3;
        const double t4d3 = 24.0*d3;
        const double sixd = 6.0*d;
        const double td = 30.0*d;
        weight[0] = (- sixd + ad2add1 - eightd3) * inv48;
        weight[1] = (- td + ad2m23 + t4d3) * inv48;
        weight[2] = (td + ad2m23 - t4d3) * inv48;
        weight[3] = (sixd + ad2add1 + eightd3) * inv48;
        break;
    }

    default:
        assert(0);
    }
}




template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void  P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::GetGradWeights( double relDist, double gradweight[] )
{
    switch( assignScheme )
    {
    case NGP:
        assert(0);

    case CIC:
        gradweight[0] = -1.0;
        gradweight[1] = 1.0;
        break;

    case TSC:
        gradweight[0] = -(1.0-2.0*relDist)/2.0;
        gradweight[1] = -2.*relDist;
        gradweight[2] = (1.0+2.0*relDist)/2.0;
        break;

    case A4P:
    {
        const double d = relDist - 0.5;
        const double d_2 = 0.5*d;
        const double d2_2 = 0.5*d*d;
        const double d2a = 3*d2_2;

        gradweight[0] = - 0.125 + d_2 - d2_2;
        gradweight[1] = - 0.625 - d_2 + d2a;
        gradweight[2] =   0.625 - d_2 - d2a;
        gradweight[3] =   0.125 + d_2 + d2_2;

        break;
    }

    default:
        assert(0);
    }
}



//-------------------
// AssignCharge(...) - assigns a single charge to the local grid points
// a charge is assigned to a cube numAssignPoints^3 with weights that add up to 1.0
// a node can own a subcube of the above cube, and is also can be "wrapped" due to
// periodic b.c.
// icellx[inx],... keep the local subcube (grid point locations),
//    wx[inx],... - keep weights and dwx[inx],... - weights for differentiation (to find forces)
// all of this info is saved in intfifo and doublefifo arrays to avoid recomputing them
// later in the findfield step

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
int P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::AssignCharge( const XYZ& coor, const double charge, const int siteId, const int aSimTick )
{
    AssignChargeDetails3Start.HitOE( PKTRACE_ASSIGN_CHARGE_DETAILS,
                             "P3ME_AssignChargeDetails3_Control",
                             Platform::Topology::GetAddressSpaceId(),
                             AssignChargeDetails3Start );

    XYZ reldist;

    //-------- X

#define SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE 10

    int indmap_x[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
    int icellx[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
    int inx = 0;
    {
        // this is coor of particle minus voxeladdextent
        double relcoor = ( coor.mX - VoxelAddExtent.mX ) * DynVarMgrIF.mDynamicBoxInverseDimensionVector.mX;

        // now it's inside central cell but is in units of boxsize
        relcoor -= floor( relcoor );

        // now should be scaled in units of spacing
        relcoor *= Grid::iBoxSize.mX;

        const double startcell = ceil( relcoor );

        reldist.mX = relcoor + 1.0 - startcell;
        if( /*odd*/ numAssignPoints & 1 ) reldist.mX -= 0.5;

        const int iStartCell = int( startcell );
        const int gofor = min( numAssignPoints, Grid::iBoxSize.mX - iStartCell );
        const int lcorner = GridVoxel::iLeftCorner.mX;
        const int size = GridVoxel::iLocalGridSize.mX;

        int i;
        int iCell = iStartCell - lcorner;
        for( i = 0; i < gofor; i++ )
        {
            if( iCell >= 0 && iCell < size )
            {
            assert( inx >= 0 && inx < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
                indmap_x[inx] = i;
                icellx[inx] = iCell;
                inx++;
            }
            iCell++;
        }

        iCell -= Grid::iBoxSize.mX;
        for( ; i < numAssignPoints; i++ )
        {
            if( iCell >= 0 && iCell < size )
            {
            assert( inx >= 0 && inx < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
                indmap_x[inx] = i;
                icellx[inx] = iCell;
                inx++;
            }
            iCell++;
        }
    }

    AssignChargeDetails3Finis.HitOE( PKTRACE_ASSIGN_CHARGE_DETAILS,
                             "P3ME_AssignChargeDetails3_Control",
                             Platform::Topology::GetAddressSpaceId(),
                             AssignChargeDetails3Finis );

    if( inx == 0 )
        return 0;

    AssignChargeDetails4Start.HitOE( PKTRACE_ASSIGN_CHARGE_DETAILS,
                             "P3ME_AssignChargeDetails4_Control",
                             Platform::Topology::GetAddressSpaceId(),
                             AssignChargeDetails4Start );

    //-------- Y

    int indmap_y[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
    int icelly[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
    int iny = 0;
    {
        // this is coor of particle minus voxeladdextent
        double relcoor = ( coor.mY - VoxelAddExtent.mY ) * DynVarMgrIF.mDynamicBoxInverseDimensionVector.mY;

        // now it's inside central cell but is in units of boxsize
        relcoor -= floor( relcoor );

        // now should be scaled in units of spacing
        relcoor *= Grid::iBoxSize.mY;

        const double startcell = ceil( relcoor );

        reldist.mY = relcoor + 1.0 - startcell;
        if( /*odd*/ numAssignPoints & 1 ) reldist.mY -= 0.5;

        const int iStartCell = int( startcell );
        const int gofor = min( numAssignPoints, Grid::iBoxSize.mY - iStartCell );
        const int lcorner = GridVoxel::iLeftCorner.mY;
        const int size = GridVoxel::iLocalGridSize.mY;

        int i;
        int iCell = iStartCell - lcorner;
        for( i = 0; i < gofor; i++ )
        {
            if( iCell >= 0 && iCell < size )
            {
            assert( iny >= 0 && iny < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
                indmap_y[iny] = i;
                icelly[iny] = iCell;
                iny++;
            }
            iCell++;
        }

        iCell -= Grid::iBoxSize.mY;
        for( ; i < numAssignPoints; i++ )
        {
            if( iCell >= 0 && iCell < size )
            {
            assert( iny >= 0 && iny < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
                indmap_y[iny] = i;
                icelly[iny] = iCell;
                iny++;
            }
            iCell++;
        }
    }

    AssignChargeDetails4Finis.HitOE( PKTRACE_ASSIGN_CHARGE_DETAILS,
                             "P3ME_AssignChargeDetails4_Control",
                             Platform::Topology::GetAddressSpaceId(),
                             AssignChargeDetails4Finis );

    if( iny == 0 )
        return 0;

    AssignChargeDetails5Start.HitOE( PKTRACE_ASSIGN_CHARGE_DETAILS,
                             "P3ME_AssignChargeDetails5_Control",
                             Platform::Topology::GetAddressSpaceId(),
                             AssignChargeDetails5Start );

    //----- Z

    int indmap_z[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
    int icellz[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
    int inz = 0;
    {
        // this is coor of particle minus voxeladdextent
        double relcoor = ( coor.mZ - VoxelAddExtent.mZ ) * DynVarMgrIF.mDynamicBoxInverseDimensionVector.mZ;

        // now it's inside central cell but is in units of boxsize
        relcoor -= floor( relcoor );

        // now should be scaled in units of spacing
        relcoor *= Grid::iBoxSize.mZ;

        const double startcell = ceil( relcoor );

        reldist.mZ = relcoor + 1.0 - startcell;
        if( /*odd*/ numAssignPoints & 1 ) reldist.mZ -= 0.5;

        const int iStartCell = int( startcell );
        const int gofor = min( numAssignPoints, Grid::iBoxSize.mZ - iStartCell );
        const int lcorner = GridVoxel::iLeftCorner.mZ;
        const int size = GridVoxel::iLocalGridSize.mZ;

        int i;
        int iCell = iStartCell - lcorner;
        for( i = 0; i < gofor; i++ )
        {
            if( iCell >= 0 && iCell < size )
            {
            assert( inz >= 0 && inz < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
                indmap_z[inz] = i;
                icellz[inz] = iCell;
                inz++;
            }
            iCell++;
        }

        iCell -= Grid::iBoxSize.mZ;
        for( ; i < numAssignPoints; i++ )
          {
          if( iCell >= 0 && iCell < size )
            {
            assert( inz >= 0 && inz < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
            indmap_z[inz] = i;
            icellz[inz] = iCell;
            inz++;
            }
          iCell++;
          }
    }

    AssignChargeDetails5Finis.HitOE( PKTRACE_ASSIGN_CHARGE_DETAILS,
                             "P3ME_AssignChargeDetails5_Control",
                             Platform::Topology::GetAddressSpaceId(),
                             AssignChargeDetails5Finis );

    if( inz == 0 )
        return 0;

    //-------- done XYZ

    AssignChargeDetails6Start.HitOE( PKTRACE_ASSIGN_CHARGE_DETAILS,
                             "P3ME_AssignChargeDetails6_Control",
                             Platform::Topology::GetAddressSpaceId(),
                             AssignChargeDetails6Start );

    // postpone the expensive weight calculation until here - by now we know
    // that there will be some charge assignment to the local grid

    double wx[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
    double wy[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
    double wz[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
    {
        double w[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
        int i;
        GetWeights( reldist.mX, w );
        for( i = 0; i < inx; i++ )
          {
          assert( i >= 0 && i < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
          assert( indmap_x[ i ] >= 0 && indmap_x[ i ] < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
          wx[ i ] = w[ indmap_x[ i ] ];
          }

        GetWeights( reldist.mY, w );
        for( i = 0; i < iny; i++ )
          {
          assert( i >= 0 && i < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
          assert( indmap_y[ i ] >= 0 && indmap_y[ i ] < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
          wy[ i ] = w[ indmap_y[ i ] ];
          }

        GetWeights( reldist.mZ, w );
        for( i = 0; i < inz; i++ )
          {
          assert( i >= 0 && i < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
          assert( indmap_z[ i ] >= 0 && indmap_z[ i ] < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
          wz[ i ] = w[ indmap_z[ i ] ];
          }
    }

    double dwx[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
    double dwy[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
    double dwz[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
    if( Differentiation::diffOrder == PreMSD::GRADIENT )
    {
        double dw[ SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE ];
        int i;
        GetGradWeights( reldist.mX, dw );
        for( i = 0; i < inx; i++ )
          {
          assert( i >= 0 && i < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
          assert( indmap_x[ i ] >= 0 && indmap_x[ i ] < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
          dwx[i] = dw[indmap_x[i]];
          }

        GetGradWeights( reldist.mY, dw );
        for( i = 0; i < iny; i++ )
          {
          assert( i >= 0 && i < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
          assert( indmap_y[ i ] >= 0 && indmap_y[ i ] < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
          dwy[i] = dw[indmap_y[i]];
          }

        GetGradWeights( reldist.mZ, dw );
        for( i = 0; i < inz; i++ )
          {
          assert( i >= 0 && i < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
          assert( indmap_z[ i ] >= 0 && indmap_z[ i ] < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
          dwz[ i ] = dw[ indmap_z[ i ] ];
          }
    }

#ifndef CACHE_OFF
    // cache the weights and subcube dimensions to reuse them in the field interpolation part
    {
        intfifo << inx;
        intfifo << iny;
        intfifo << inz;
        int i;
        for( i = 0; i < inx; i++ )
        {
            assert( i >= 0 && i < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
            intfifo << icellx[i];
            doublefifo << wx[i];
        }
        for( i = 0; i < iny; i++ )
        {
            assert( i >= 0 && i < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
            intfifo << icelly[i];
            doublefifo << wy[i];
        }
        for( i = 0; i < inz; i++ )
        {
            assert( i >= 0 && i < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
            intfifo << icellz[i];
            doublefifo << wz[i];
        }
        if( Differentiation::diffOrder == PreMSD::GRADIENT )
        {
            for( i = 0; i < inx; i++ )
              {
              assert( i >= 0 && i < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
              doublefifo << dwx[i];
              }

            for( i = 0; i < iny; i++ )
              {
              assert( i >= 0 && i < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
              doublefifo << dwy[i];
              }

            for( i = 0; i < inz; i++ )
              {
              assert( i >= 0 && i < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
              doublefifo << dwz[i];
              }
        }
    }
#endif

    for( int ix = 0; ix < inx; ix++ )
    {
    assert( ix >= 0 && ix < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
        const double weightx = wx[ ix ];
        for( int iy = 0; iy < iny; iy++ )
        {
        assert( iy >= 0 && iy < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
            const double weightxy = weightx * wy[ iy ];
            for( int iz = 0; iz < inz; iz++ )
            {
            assert( iz >= 0 && iz < SIZE_OF_ARRAYS_IN_ASSIGN_CHARGE );
                const double weightxyz = weightxy * wz[ iz ];
                // finally, assign the charge
		// This logline puts out the weight and charge weight for the current site on each grid point
		BegLogLine(PKFXLOG_MESHED_CHARGE)
		  << FormatString("MESHED_CHARGE %6d ")      << aSimTick
		  << FormatString("Site: %6d ")              << siteId
		  << FormatString("xi yi zi: %6d ")          << icellx[ix] + GridVoxel::iLeftCorner.mX
		  << FormatString("%6d ")                    << icelly[iy] + GridVoxel::iLeftCorner.mY
		  << FormatString("%6d ")                    << icellz[iz] + GridVoxel::iLeftCorner.mZ
		  << " weight: "                             << weightxyz
		  << " charge: "                             << charge*weightxyz
		  << EndLogLine;
                const double rse = FFT_FWD_IF->GetRealSpaceElement( icellx[ ix ], icelly[ iy ], icellz[ iz ] ) + charge * weightxyz;
                FFT_FWD_IF->PutRealSpaceElement( icellx[ ix ], icelly[ iy ], icellz[ iz ], rse );
            }
        }
    }

    AssignChargeDetails6Finis.HitOE( PKTRACE_ASSIGN_CHARGE_DETAILS,
                             "P3ME_AssignChargeDetails6_Control",
                             Platform::Topology::GetAddressSpaceId(),
                             AssignChargeDetails6Finis );

    return 1;
}



template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::FiniteDiffEfield_X()
{
//////// NOTICE: CODE BROKEN !!!!!!!!!


    int leftcnr = GridVoxel::iLeftCorner.mX;
    int rightcnr = leftcnr + GridVoxel::iLocalGridSize.mX;

    FieldmX->ZeroRealSpace();

    for( int ord = 1; ord <= Differentiation::diffOrder; ord++ )
    {
        double factx = Differentiation::coeff[ord-1]/(2*ord*Grid::spacing.mX);

        for( int ix = 0; ix < GridVoxel::iLocalGridSize.mX; ix++ )
        {
            int x1 = (ix - ord + Grid::iBoxSize.mX)%Grid::iBoxSize.mX;
            if( x1 >= leftcnr && x1 < rightcnr )
                for( int iy = 0; iy < GridVoxel::iLocalGridSize.mY; iy++ )
                    for( int iz = 0; iz < GridVoxel::iLocalGridSize.mZ; iz++ )
                        FieldmX->PutRealSpaceElement(
                            ix, iy, iz,
                            FieldmX->GetRealSpaceElement(ix,iy,iz)
                            + FFT_REV_IF->GetRealSpaceElement( x1, iy, iz ) * factx
                            );

            int x2 = (ix + ord)%Grid::iBoxSize.mX;
            if( x2 >= leftcnr && x2 < rightcnr )
                for( int iy = 0; iy < GridVoxel::iLocalGridSize.mY; iy++ )
                    for( int iz = 0; iz < GridVoxel::iLocalGridSize.mZ; iz++ )
                        FieldmX->PutRealSpaceElement(
                            ix, iy, iz,
                            FieldmX->GetRealSpaceElement(ix,iy,iz)
                            - FFT_REV_IF->GetRealSpaceElement( x1, iy, iz ) * factx
                            );
        }
    }
}



template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::FiniteDiffEfield_Y()
{
//////// NOTICE: CODE BROKEN !!!!!!!!!

    int leftcnr = GridVoxel::iLeftCorner.mY;
    int rightcnr = leftcnr + GridVoxel::iLocalGridSize.mY;

    FieldmY->ZeroRealSpace();

    for( int ord = 1; ord <= Differentiation::diffOrder; ord++ )
    {
        double facty = Differentiation::coeff[ord-1]/(2*ord*Grid::spacing.mY);

        for( int iy = 0; iy < GridVoxel::iLocalGridSize.mY; iy++ )
        {
            int y1 = (iy - ord + Grid::iBoxSize.mY)%Grid::iBoxSize.mY;
            if( y1 >= leftcnr && y1 < rightcnr )
                for( int ix = 0; ix < GridVoxel::iLocalGridSize.mX; ix++ )
                    for( int iz = 0; iz < GridVoxel::iLocalGridSize.mZ; iz++ )
                        FieldmY->PutRealSpaceElement(
                            ix, iy, iz,
                            FieldmY->GetRealSpaceElement(ix,iy,iz)
                            + FFT_REV_IF->GetRealSpaceElement( ix, y1, iz ) * facty
                            );

            int y2 = (iy + ord)%Grid::iBoxSize.mY;
            if( y2 >= leftcnr && y2 < rightcnr )
                for( int ix = 0; ix < GridVoxel::iLocalGridSize.mX; ix++ )
                    for( int iz = 0; iz < GridVoxel::iLocalGridSize.mZ; iz++ )
                        FieldmY->PutRealSpaceElement(
                            ix, iy, iz,
                            FieldmY->GetRealSpaceElement(ix,iy,iz)
                            - FFT_REV_IF->GetRealSpaceElement( ix, y1, iz ) * facty
                            );
        }
    }
}



template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::FiniteDiffEfield_Z()
{
//////// NOTICE: CODE BROKEN !!!!!!!!!


    int leftcnr = GridVoxel::iLeftCorner.mZ;
    int rightcnr = leftcnr + GridVoxel::iLocalGridSize.mZ;

    FieldmZ->ZeroRealSpace();

    for( int ord = 1; ord <= Differentiation::diffOrder; ord++ )
    {
        double factz = Differentiation::coeff[ord-1]/(2*ord*Grid::spacing.mZ);

        for( int iz = 0; iz < GridVoxel::iLocalGridSize.mZ; iz++ )
        {
            int z1 = (iz - ord + Grid::iBoxSize.mZ)%Grid::iBoxSize.mZ;
            if( z1 >= leftcnr && z1 < rightcnr )
                for( int iy = 0; iy < GridVoxel::iLocalGridSize.mY; iy++ )
                    for( int ix = 0; ix < GridVoxel::iLocalGridSize.mX; ix++ )
                        FieldmY->PutRealSpaceElement(
                            ix, iy, iz,
                            FieldmZ->GetRealSpaceElement(ix,iy,iz)
                            + FFT_REV_IF->GetRealSpaceElement( ix, iy, z1 ) * factz
                            );

            int z2 = (iz + ord)%Grid::iBoxSize.mZ;
            if( z2 >= leftcnr && z2 < rightcnr )
                for( int iy = 0; iy < GridVoxel::iLocalGridSize.mY; iy++ )
                    for( int ix = 0; ix < GridVoxel::iLocalGridSize.mX; ix++ )
                        FieldmY->PutRealSpaceElement(
                            ix, iy, iz,
                            FieldmZ->GetRealSpaceElement(ix,iy,iz)
                            + FFT_REV_IF->GetRealSpaceElement( ix, iy, z1 ) * factz
                            );
        }
    }
}



template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::FiniteDiffEfield()
{
    if( ! GridVoxel::GridNotEmpty )
        return;

    FiniteDiffEfield_X();
    FiniteDiffEfield_Y();
    FiniteDiffEfield_Z();
}




template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::FindFields( XYZ& aVirial, const int aSimTick )
{

   BegLogLine( PKFXLOG_P3MEFINDFIELD )
    << "Before InKSpace::Convolute();"
    << EndLogLine;

   //EmitTimeStamp( aSimTick, Tag::KSpaceConvolve_Begin, 1 );


   P3MEConvolveStart.HitOE( PKTRACE_KSPACE_CONVOLVE,
                            "KSpaceConvolve",
                            Platform::Topology::GetAddressSpaceId(),
                            P3MEConvolveStart );

   InKSpace::Convolute( aVirial, aSimTick );

   aVirial *= MSD::SimpleRespaRatio;

   P3MEConvolveFinis.HitOE( PKTRACE_KSPACE_CONVOLVE,
                            "KSpaceConvolve",
                            Platform::Topology::GetAddressSpaceId(),
                            P3MEConvolveFinis );

   //EmitTimeStamp( aSimTick, Tag::KSpaceConvolve_End, 1 );


   BegLogLine( PKFXLOG_P3MEFINDFIELD )
    << "After InKSpace::Convolute();"
    << EndLogLine;

    if( Differentiation::diffOrder == PreMSD::ANALYTICAL )
    {
        // uses cdata to initialize gridFieldmX
        InKSpace::ConvoluteKx();

        BegLogLine( PKFXLOG_P3MEFINDFIELD )
          << "After ConvoluteKx();"
          << EndLogLine;

        // get x component of mesh electric field
        FieldmX->DoFFT();

        BegLogLine( PKFXLOG_P3MEFINDFIELD )
          << "After InverseFFT1();"
          << EndLogLine;

        // uses cdata to initialize gridFieldmY
        InKSpace::ConvoluteKy();
        // get y component of mesh electric field
        FieldmY->DoFFT();

        BegLogLine( PKFXLOG_P3MEFINDFIELD )
          << "After InverseFFT2();"
          << EndLogLine;

        // uses cdata to initialize gridFieldmZ
        InKSpace::ConvoluteKz();
        // get z component of mesh electric field
        FieldmZ->DoFFT();

        BegLogLine( PKFXLOG_P3MEFINDFIELD )
          << "After InverseFFT3();"
          << EndLogLine;
    }

    // this is to get the mesh potential

    P3MEReverseFFTStart.HitOE( PKTRACE_KSPACE_REV_FFT,
                               "KSpaceRevFFT",
                               Platform::Topology::GetAddressSpaceId(),
                               P3MEReverseFFTStart );

    FFT_REV_IF->DoFFT();

    P3MEReverseFFTFinis.HitOE( PKTRACE_KSPACE_REV_FFT,
                               "KSpaceRevFFT",
                               Platform::Topology::GetAddressSpaceId(),
                               P3MEReverseFFTFinis );

    BegLogLine( PKFXLOG_P3MEFINDFIELD )
      << "After InverseFFT4();"
      << EndLogLine;

    if( Differentiation::diffOrder > 0 )
    {
        FiniteDiffEfield();
        BegLogLine( PKFXLOG_P3MEFINDFIELD )
          << "After FiniteDiffEfield();"
          << EndLogLine;
    }
}



//-----------------------------------------------------
// InterpolateFields(..) - computes potentials and (fields) forces on particles
// after the inverse fft call. The function is structured very similarly to AssignCharge(..)
// and in fact if the cache is on the weights are not recalculated but recycled from the
// AssignCharge step.

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
int P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::ParticleMesh
::InterpolateFields( const XYZ& coor,
		     const double charge,
		     XYZ& force,
		     double& energy,
		     const int siteId,
		     const int aSimTick )
{
#ifdef CACHE_OFF

    XYZ reldist;

    //-------- X

    int indmap_x[10];
    int icellx[10];
    int inx = 0;
    {
        // this is coor of particle minus voxeladdextent
        double relcoor = ( coor.mX - VoxelAddExtent.mX ) * DynVarMgrIF.mDynamicBoxInverseDimensionVector.mX;

        // now it's inside central cell but is in units of boxsize
        relcoor -= floor( relcoor );

        // now should be scaled in units of spacing
        relcoor *= Grid::iBoxSize.mX;

        const double startcell = ceil( relcoor );

        reldist.mX = relcoor + 1.0 - startcell;
        if( /*odd*/ numAssignPoints & 1 ) reldist.mX -= 0.5;

        const int iStartCell = int( startcell );
        const int gofor = min( numAssignPoints, Grid::iBoxSize.mX - iStartCell );
        const int lcorner = GridVoxel::iLeftCorner.mX;
        const int size = GridVoxel::iLocalGridSize.mX;

        int i;
        int iCell = iStartCell - lcorner;
        for( i = 0; i < gofor; i++ )
        {
            if( iCell >= 0 && iCell < size )
            {
                indmap_x[inx] = i;
                icellx[inx] = iCell;
                inx++;
            }
            iCell++;
        }

        iCell -= Grid::iBoxSize.mX;
        for( ; i < numAssignPoints; i++ )
        {
            if( iCell >= 0 && iCell < size )
            {
                indmap_x[inx] = i;
                icellx[inx] = iCell;
                inx++;
            }
            iCell++;
        }
    }

    if( inx == 0 )
        return 0;

    //-------- Y

    int indmap_y[10];
    int icelly[10];
    int iny = 0;
    {
        // this is coor of particle minus voxeladdextent
        double relcoor = ( coor.mY - VoxelAddExtent.mY ) * DynVarMgrIF.mDynamicBoxInverseDimensionVector.mY;

        // now it's inside central cell but is in units of boxsize
        relcoor -= floor( relcoor );

        // now should be scaled in units of spacing
        relcoor *= Grid::iBoxSize.mY;

        const double startcell = ceil( relcoor );

        reldist.mY = relcoor + 1.0 - startcell;
        if( /*odd*/ numAssignPoints & 1 ) reldist.mY -= 0.5;

        const int iStartCell = int( startcell );
        const int gofor = min( numAssignPoints, Grid::iBoxSize.mY - iStartCell );
        const int lcorner = GridVoxel::iLeftCorner.mY;
        const int size = GridVoxel::iLocalGridSize.mY;

        int i;
        int iCell = iStartCell - lcorner;
        for( i = 0; i < gofor; i++ )
        {
            if( iCell >= 0 && iCell < size )
            {
                indmap_y[iny] = i;
                icelly[iny] = iCell;
                iny++;
            }
            iCell++;
        }

        iCell -= Grid::iBoxSize.mY;
        for( ; i < numAssignPoints; i++ )
        {
            if( iCell >= 0 && iCell < size )
            {
                indmap_y[iny] = i;
                icelly[iny] = iCell;
                iny++;
            }
            iCell++;
        }
    }

    if( iny == 0 )
        return 0;

    //----- Z

    int indmap_z[10];
    int icellz[10];
    int inz = 0;
    {
        // this is coor of particle minus voxeladdextent
        double relcoor = ( coor.mZ - VoxelAddExtent.mZ ) * DynVarMgrIF.mDynamicBoxInverseDimensionVector.mZ;

        // now it's inside central cell but is in units of boxsize
        relcoor -= floor( relcoor );

        // now should be scaled in units of spacing
        relcoor *= Grid::iBoxSize.mZ;

        const double startcell = ceil( relcoor );

        reldist.mZ = relcoor + 1.0 - startcell;
        if( /*odd*/ numAssignPoints & 1 ) reldist.mZ -= 0.5;

        const int iStartCell = int( startcell );
        const int gofor = min( numAssignPoints, Grid::iBoxSize.mZ - iStartCell );
        const int lcorner = GridVoxel::iLeftCorner.mZ;
        const int size = GridVoxel::iLocalGridSize.mZ;

        int i;
        int iCell = iStartCell - lcorner;
        for( i = 0; i < gofor; i++ )
        {
            if( iCell >= 0 && iCell < size )
            {
                indmap_z[inz] = i;
                icellz[inz] = iCell;
                inz++;
            }
            iCell++;
        }

        iCell -= Grid::iBoxSize.mZ;
        for( ; i < numAssignPoints; i++ )
        {
            if( iCell >= 0 && iCell < size )
            {
                indmap_z[inz] = i;
                icellz[inz] = iCell;
                inz++;
            }
            iCell++;
        }
    }

    if( inz == 0 )
        return 0;

    //-------- done XYZ

    double wx[10];
    double wy[10];
    double wz[10];
    {
        double w[10];
        int i;
        GetWeights( reldist.mX, w );
        for( i = 0; i < inx; i++ )
            wx[i] = w[indmap_x[i]];
        GetWeights( reldist.mY, w );
        for( i = 0; i < iny; i++ )
            wy[i] = w[indmap_y[i]];
        GetWeights( reldist.mZ, w );
        for( i = 0; i < inz; i++ )
            wz[i] = w[indmap_z[i]];
    }

    double dwx[10];
    double dwy[10];
    double dwz[10];
    if( Differentiation::diffOrder == PreMSD::GRADIENT )
    {
        double dw[10];
        int i;
        GetGradWeights( reldist.mX, dw );
        for( i = 0; i < inx; i++ )
            dwx[i] = dw[indmap_x[i]];
        GetGradWeights( reldist.mY, dw );
        for( i = 0; i < iny; i++ )
            dwy[i] = dw[indmap_y[i]];
        GetGradWeights( reldist.mZ, dw );
        for( i = 0; i < inz; i++ )
            dwz[i] = dw[indmap_z[i]];
    }

#else

    int icellx[10];
    int icelly[10];
    int icellz[10];

    int inx = 0;
    int iny = 0;
    int inz = 0;

    double wx[10];
    double wy[10];
    double wz[10];

    double dwx[10];
    double dwy[10];
    double dwz[10];

    {
        intfifo >> inx;
        intfifo >> iny;
        intfifo >> inz;

        int i;
        for( i = 0; i < inx; i++ )
        {
            intfifo >> icellx[i];
            doublefifo >> wx[i];
        }
        for( i = 0; i < iny; i++ )
        {
            intfifo >> icelly[i];
            doublefifo >> wy[i];
        }
        for( i = 0; i < inz; i++ )
        {
            intfifo >> icellz[i];
            doublefifo >> wz[i];
        }
        if( Differentiation::diffOrder == PreMSD::GRADIENT )
        {
            for( i = 0; i < inx; i++ )
                doublefifo >> dwx[i];
            for( i = 0; i < iny; i++ )
                doublefifo >> dwy[i];
            for( i = 0; i < inz; i++ )
                doublefifo >> dwz[i];
        }
    }

#endif

    XYZ efield = { 0., 0., 0. };
    double potential = 0.;

    for( int ix = 0; ix < inx; ix++ )
    {
        const double weightx = wx[ix];
        for( int iy = 0; iy < iny; iy++ )
        {
            const double weighty = wy[iy];
            const double weightxy = weightx * weighty;
            for( int iz = 0; iz < inz; iz++ )
            {
                const double weightz = wz[iz];
                const double weightxyz = weightxy * weightz;

#if PKFXLOG_REGRESS_UDF_RESULTS
		XYZ dEField;
                double dpotential = weightxyz * FFT_REV_IF->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] );
		potential += dpotential;
#else
                potential += weightxyz * FFT_REV_IF->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] );
#endif
                if( Differentiation::diffOrder == PreMSD::GRADIENT )
                {
#if PKFXLOG_REGRESS_UDF_RESULTS
		  dEField.mX = -dwx[ix]*weighty*weightz * FFT_REV_IF->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] )*Grid::spacing_1.mX;
		  dEField.mY = -weightx*dwy[iy]*weightz * FFT_REV_IF->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] )*Grid::spacing_1.mY;
		  dEField.mZ = -weightx*weighty*dwz[iz] * FFT_REV_IF->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] )*Grid::spacing_1.mZ;
		  
		  efield.mX +=dEField.mX;
		  efield.mY +=dEField.mY;
		  efield.mZ +=dEField.mZ;
#else
		  efield.mX -= dwx[ix]*weighty*weightz * FFT_REV_IF->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] )*Grid::spacing_1.mX;
		  efield.mY -= weightx*dwy[iy]*weightz * FFT_REV_IF->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] )*Grid::spacing_1.mY;
		  efield.mZ -= weightx*weighty*dwz[iz] * FFT_REV_IF->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] )*Grid::spacing_1.mZ;
#endif
                }
                else
                {
#if PKFXLOG_REGRESS_UDF_RESULTS
		  dEField.mX = weightxyz * FieldmX->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] );
		  dEField.mY = weightxyz * FieldmY->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] );
		  dEField.mZ = weightxyz * FieldmZ->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] );
		  
		  efield.mX +=dEField.mX;
		  efield.mY +=dEField.mY;
		  efield.mZ +=dEField.mZ;
#else
		  efield.mX += weightxyz * FieldmX->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] );
		  efield.mY += weightxyz * FieldmY->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] );
		  efield.mZ += weightxyz * FieldmZ->GetRealSpaceElement( icellx[ix], icelly[iy], icellz[iz] );
#endif
                }

#if PKFXLOG_REGRESS_UDF_RESULTS
		// convert to energy
		dpotential = dpotential* 0.5 * charge * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
		// convert deltas into actual forces
		dEField.mX = dEField.mX*charge * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
		dEField.mY = dEField.mY*charge * DynVarMgrIF.mBirthBoxAbsoluteRatio.mY * DynVarMgrIF.mBirthBoxAbsoluteRatio.mY;
		dEField.mZ = dEField.mZ*charge * DynVarMgrIF.mBirthBoxAbsoluteRatio.mZ * DynVarMgrIF.mBirthBoxAbsoluteRatio.mZ;

		BegLogLine(PKFXLOG_REGRESS_UDF_RESULTS)
		  << FormatString("UDFREG % 6d ") << aSimTick
		  << FormatString(" % 2d ")       << UDF_Binding::EwaldKSpaceForce_Code
		  << FormatString(" S0 %5d ")     << siteId
		  << dpotential << " "
		  << "F0 " << dEField
		  << EndLogLine;
#endif
            }
        }
    }

    force.mX = charge * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX * efield.mX;
    force.mY = charge * DynVarMgrIF.mBirthBoxAbsoluteRatio.mY * DynVarMgrIF.mBirthBoxAbsoluteRatio.mY * efield.mY;
    force.mZ = charge * DynVarMgrIF.mBirthBoxAbsoluteRatio.mZ * DynVarMgrIF.mBirthBoxAbsoluteRatio.mZ * efield.mZ;
    energy = 0.5 * charge * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX * potential;

    return 1;
}




template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::Differentiation
::initcoeff()
{
    if( diffOrder > 0 )
    {
      // coeff = new double[diffOrder];
        pkNew( &coeff, diffOrder, __FILE__, __LINE__ );
        assert( coeff );

        // in principle, can compute the coeffitients for a general case here,
        // but for simplicity just type them in for the lowest order operators

        switch( diffOrder )
        {
        case 1:

            coeff[0] = 1.0;
            break;

        case 2:

            coeff[0] = 4./3.;
            coeff[1] = -1./3.;
            break;

        case 3:

            coeff[0] = 3./2.;
            coeff[1] = -3./5.;
            coeff[2] = 1./10.;
            break;

        case 4:

            coeff[0] = 8./5.;
            coeff[1] = -4./5.;
            coeff[2] = 8./35.;
            coeff[3] = -1./35.;
            break;

        case 5:

            coeff[0] = 5./3.;
            coeff[1] = -20./21.;
            coeff[2] = 5./14.;
            coeff[3] = -5./63.;
            coeff[4] = 1./126.;
            break;

        case 6:

            coeff[0] = 12./7.;
            coeff[1] = -15./14.;
            coeff[2] = 10./21.;
            coeff[3] = -1./7.;
            coeff[4] = 2./77.;
            coeff[5] = -1./462.;
            break;

        default:

            assert(0);
        }
    }
}




#if defined(P3ME_COMPILE) || !defined(P3ME_SEPARATE)
template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::InKSpace
::Init()
{
    if( ! InKSpace::GridNotEmpty )
        return;

    // allocate space
    // the dimensioned are of momentum range, NOT STORAGE
    greenFun.Init( InKSpace::iLocalGridSize.mX, InKSpace::iLocalGridSize.mY, InKSpace::iLocalGridSize.mZ );
    SumDmuWVcX.Init( InKSpace::iLocalGridSize.mX );
    SumDmuWVcY.Init( InKSpace::iLocalGridSize.mY );
    SumDmuWVcZ.Init( InKSpace::iLocalGridSize.mZ );
    SumWVcX.Init( InKSpace::iLocalGridSize.mX );
    SumWVcY.Init( InKSpace::iLocalGridSize.mY );
    SumWVcZ.Init( InKSpace::iLocalGridSize.mZ );

    // initialize greenFun array
    SetGopt();
}


//------------------------------------------------------------------------
// Convolute(..) - convolution in the reciprocal space of greens functions with charges
// also computes virial contribution from the k-space part

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::InKSpace
::Convolute( XYZ& aVirial, const int aSimTick )
{
#if MSDDEF_DoComputeVirial || MSDDEF_DoPressureControl
    XYZ Virial;
    Virial.Zero();

    int kx, ky, kz;
    double localKzStart, localKzStop; // for the loops below the two special cases

    double dKx = 2.*M_PI*Grid::iBoxSize_1.mX*Grid::spacing_1.mX;
    double dKy = 2.*M_PI*Grid::iBoxSize_1.mY*Grid::spacing_1.mY;
    double dKz = 2.*M_PI*Grid::iBoxSize_1.mZ*Grid::spacing_1.mZ;

    double InvAlpha2 = 1.0/( MSD_IF::GetEwaldAlpha() * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX );
    InvAlpha2 *= InvAlpha2;
    const double Inv2Alpha2 = InvAlpha2/2.;

    // kz has range 0..Grid::iBoxSize.mZ/2, for larger kz (corresponding to negative momenta)
    // need to map into that range.
    // kz==0 is not mapped sinse (-0)==0, also Grid::iBoxSize.mZ/2 itself has a negative
    // counterpart only for odd box size. This is why need the special cases of kz below.

    if( InKSpace::iLeftCorner.mZ == 0 ) // check if Kz==0 is local to this node
    {
        localKzStart = 1; // skip Kz==0

        kz = 0;
        for( kx = 0; kx < InKSpace::iLocalGridSize.mX; kx++ )
        {
            int KKKx = kx + InKSpace::iLeftCorner.mX;
            if( KKKx >= Grid::iBoxSize.mX/2 )
                KKKx -= Grid::iBoxSize.mX;
            double Kx2 = dKx*KKKx*DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
            Kx2 *= Kx2;
            for( ky = 0; ky < InKSpace::iLocalGridSize.mY; ky++ )
            {
                double gf = greenFun( kx, ky, kz );
                if( gf ) // to avoid 0/0 case when v==0 and SD==0
                {
                    int KKKy = ky + InKSpace::iLeftCorner.mY;
                    if( KKKy >= Grid::iBoxSize.mY/2 )
                        KKKy -= Grid::iBoxSize.mY;
                    double Ky2 = dKy*KKKy*DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
                    Ky2 *= Ky2;
                    const double K2 = Kx2 + Ky2;

                    const double v = norm( FFT_FWD_IF->GetRecipSpaceElement(kx,ky,kz) ) * gf;

                    const double SDx = SumDmuWVcX( kx ) * SumWVcY( ky ) * SumWVcZ( kz );
                    const double SDy = SumWVcX( kx ) * SumDmuWVcY( ky ) * SumWVcZ( kz );
                    const double SDz = SumWVcX( kx ) * SumWVcY( ky ) * SumDmuWVcZ( kz );
                    const double two_SD = 2.0/( SDx + SDy + SDz );
                    const double m1mk2_6al2 = -1.0 - K2*(1./6.)*InvAlpha2;
                    Virial.mX += v * ( m1mk2_6al2 + two_SD*SDx + Kx2*Inv2Alpha2 );
                    Virial.mY += v * ( m1mk2_6al2 + two_SD*SDy + Ky2*Inv2Alpha2  );
                    Virial.mZ += v * ( m1mk2_6al2 + two_SD*SDz );
//                    Virial.mX += v * ( -1.0 + 2*SDx/SD + ( Kx2/2. - K2/6. )*InvAlpha2 );
//                    Virial.mY += v * ( -1.0 + 2*SDy/SD + ( Ky2/2. - K2/6. )*InvAlpha2  );
//                    Virial.mZ += v * ( -1.0 + 2*SDz/SD + (        - K2/6. )*InvAlpha2  );
                }
            }
        }
    }
    else
    {
        localKzStart = 0; // Kz==0 is not local, so start from kz=0
    }

    kz = Grid::iBoxSize.mZ/2;
    if( ( kz > Grid::iBoxSize.mZ - Grid::iBoxSize.mZ/2 - 1 ) // is there a momentum that does not have "-K"
        && ( kz - InKSpace::iLeftCorner.mZ < InKSpace::iLocalGridSize.mZ ) ) // is it local to the node
    {
        localKzStop = kz - InKSpace::iLeftCorner.mZ;  // cut out momenta equal or above boxsize/2

        double Kz2 = (Grid::iBoxSize.mZ/2)*dKz*DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
        Kz2 *= Kz2;
        kz -= InKSpace::iLeftCorner.mZ; // adjust for local indexing
        for( kx = 0; kx < InKSpace::iLocalGridSize.mX; kx++ )
        {
            int KKKx = kx + InKSpace::iLeftCorner.mX;
            if( KKKx >= Grid::iBoxSize.mX/2 )
                KKKx -= Grid::iBoxSize.mX;
            double Kx2 = dKx*KKKx*DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
            Kx2 *= Kx2;
            for( ky = 0; ky < InKSpace::iLocalGridSize.mY; ky++ )
            {
                int KKKy = ky + InKSpace::iLeftCorner.mY;
                if( KKKy >= Grid::iBoxSize.mY/2 )
                    KKKy -= Grid::iBoxSize.mY;
                double Ky2 = dKy*KKKy*DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
                Ky2 *= Ky2;
                const double K2 = Kx2 + Ky2 + Kz2;

                const double v = norm( FFT_FWD_IF->GetRecipSpaceElement(kx,ky,kz) ) * greenFun(kx,ky,kz);

                const double SDx = SumDmuWVcX( kx ) * SumWVcY( ky ) * SumWVcZ( kz );
                const double SDy = SumWVcX( kx ) * SumDmuWVcY( ky ) * SumWVcZ( kz );
                const double SDz = SumWVcX( kx ) * SumWVcY( ky ) * SumDmuWVcZ( kz );
                const double two_SD = 2.0/( SDx + SDy + SDz );
                const double m1mk2_6al2 = -1.0 - K2*(1./6.)*InvAlpha2;
                Virial.mX += v * ( m1mk2_6al2 + two_SD*SDx + Kx2*Inv2Alpha2 );
                Virial.mY += v * ( m1mk2_6al2 + two_SD*SDy + Ky2*Inv2Alpha2 );
                Virial.mZ += v * ( m1mk2_6al2 + two_SD*SDz + Kz2*Inv2Alpha2 );
//                Virial.mX += v * ( -1.0 + 2*SDx/SD + ( Kx2/2. - K2/6. )*InvAlpha2 );
//                Virial.mY += v * ( -1.0 + 2*SDy/SD + ( Ky2/2. - K2/6. )*InvAlpha2 );
//                Virial.mZ += v * ( -1.0 + 2*SDz/SD + ( Kz2/2. - K2/6. )*InvAlpha2 );
            }
        }
    }
    else
    {
        // keep the boxsize/2 momentum or the available local range not exceeding boxsize/2
        localKzStop = min( kz - InKSpace::iLeftCorner.mZ + 1, InKSpace::iLocalGridSize.mZ );
    }

    Virial = Virial/2.; // the contributions from above momenta count only once since "-K" for them does not exist

    // the rest of kz are mapped into negative momenta, which simply doubles virial contribution
    for( kz = localKzStart; kz < localKzStop; kz++ )
    {
        double Kz2 = dKz*( kz + InKSpace::iLeftCorner.mZ )*DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
        Kz2 *= Kz2;
        for( kx = 0; kx < InKSpace::iLocalGridSize.mX; kx++ )
        {
            int KKKx = kx + InKSpace::iLeftCorner.mX;
            if( KKKx >= Grid::iBoxSize.mX/2 )
                KKKx -= Grid::iBoxSize.mX;
            double Kx2 = dKx*KKKx*DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
            Kx2 *= Kx2;
            for( ky = 0; ky < InKSpace::iLocalGridSize.mY; ky++ )
            {
                int KKKy = ky + InKSpace::iLeftCorner.mY;
                if( KKKy >= Grid::iBoxSize.mY/2 )
                    KKKy -= Grid::iBoxSize.mY;
                double Ky2 = dKy*KKKy*DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;

                Ky2 *= Ky2;
                const double K2 = Kx2 + Ky2 + Kz2;

                const double v = norm( FFT_FWD_IF->GetRecipSpaceElement(kx,ky,kz) ) * greenFun(kx,ky,kz);

                const double SDx = SumDmuWVcX( kx ) * SumWVcY( ky ) * SumWVcZ( kz );
                const double SDy = SumWVcX( kx ) * SumDmuWVcY( ky ) * SumWVcZ( kz );
                const double SDz = SumWVcX( kx ) * SumWVcY( ky ) * SumDmuWVcZ( kz );
                const double two_SD = 2.0/( SDx + SDy + SDz );
                const double m1mk2_6al2 = -1.0 - K2*(1./6.)*InvAlpha2;
                Virial.mX += v * ( m1mk2_6al2 + two_SD*SDx + Kx2*Inv2Alpha2 );
                Virial.mY += v * ( m1mk2_6al2 + two_SD*SDy + Ky2*Inv2Alpha2 );
                Virial.mZ += v * ( m1mk2_6al2 + two_SD*SDz + Kz2*Inv2Alpha2 );
            }
        }
    }

    Virial *= DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;

    aVirial -= Virial;

#endif // MSDDEF_DoComputeVirial

    int kxLimit = InKSpace::iLocalGridSize.mX ;
    int kyLimit = InKSpace::iLocalGridSize.mY ;
    int kzLimit = InKSpace::iLocalGridSize.mZ ;

    BegLogLine( 0 )
      << aSimTick 
      << " kxLimit: " << kxLimit
      << " kyLimit: " << kyLimit
      << " kzLimit: " << kzLimit
      << EndLogLine;

    for( int kx = 0; kx < kxLimit; kx++ )
    {
        for( int ky = 0; ky < kyLimit; ky++ )
        {
#if 0
        COMPLEX_TYPE_THINGY updatedRecipelements[ kzLimit ];
        for( int kz0 = 0; kz0 < kzLimit; kz0++ )
            {
            	double greenVal = greenFun( kx, ky, kz0 ) ;
            	COMPLEX_TYPE_THINGY recipElement = FFT_FWD_IF->GetRecipSpaceElement(kx,ky,kz0) ;
		BegLogLine(PKFXLOG_AFTER_FWD_FFT)
		  << FormatString("AFTER_FWD_FFT %6d ") << aSimTick
		  << FormatString(" kx ky kz: %6d ")    << kx + InKSpace::iLeftCorner.mX
		  << FormatString("%6d ")               << ky + InKSpace::iLeftCorner.mY
		  << FormatString("%6d ")               << kz0 + InKSpace::iLeftCorner.mZ
		  << FormatString(" re: ")              << recipElement.re
		  << FormatString(" im: ")              << recipElement.im
		  << FormatString(" greenVal: ")        << greenVal
		  << EndLogLine;
            	updatedRecipelements[kz0].re=recipElement.re*greenVal ;
            	updatedRecipelements[kz0].im=recipElement.im*greenVal ;
//                updatedRecipelements[kz0]=FFT_FWD_IF->GetRecipSpaceElement(kx,ky,kz0) * greenFun( kx, ky, kz0 );
            }
            for( int kz1 = 0; kz1 < kzLimit; kz1++ )
            {
                FFT_REV_IF->PutRecipSpaceElement(kx,ky,kz1,updatedRecipelements[kz1]);
            }
#else            
            for( int kz = 0; kz < kzLimit; kz++ )
            {
                FFT_REV_IF->PutRecipSpaceElement(kx, ky, kz, FFT_FWD_IF->GetRecipSpaceElement(kx,ky,kz) * static_cast<FFT_TYPE>( greenFun( kx, ky, kz ) ) );
            }
#endif   
        }
    }         
}



template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::InKSpace
::ConvoluteKx()
{
    int kx;
    double dK = 2.*M_PI*Grid::iBoxSize_1.mX*Grid::spacing_1.mX;

    // range [0,L/2] interpret as positive momenta
    int kxleft1 = InKSpace::iLeftCorner.mX;
    int kxright1 = min( InKSpace::iLeftCorner.mX + InKSpace::iLocalGridSize.mX, Grid::iBoxSize.mX/2 + 1 );
    double Kx = kxleft1 * dK;
    for( kx = kxleft1; kx < kxright1; kx++ )
    {
        for( int ky = 0; ky < InKSpace::iLocalGridSize.mY; ky++ )
            for( int kz = 0; kz < InKSpace::iLocalGridSize.mZ; kz++ )
            {
                COMPLEX_TYPE_THINGY c = static_cast<FFT_TYPE>( Kx ) * FFT_REV_IF->GetRecipSpaceElement( kx - InKSpace::iLeftCorner.mX, ky, kz );
//                FieldmX->PutRecipSpaceElement( kx - InKSpace::iLeftCorner.mX, ky, kz, COMPLEX_TYPE_THINGY_WITH_CONSTRUCTOR( imag(c), -real(c) ));
//                FieldmX->PutRecipSpaceElement( kx - InKSpace::iLeftCorner.mX, ky, kz, COMPLEX_TYPE_THINGY_WITH_CONSTRUCTOR( COMPLEX_MEMBER_IM_THINGY(c), -COMPLEX_MEMBER_RE_THINGY(c) ));
                FieldmX->PutRecipSpaceElement( kx - InKSpace::iLeftCorner.mX, ky, kz, complex<FFT_TYPE>(0.0,-1.0)*c);
            }
        Kx += dK;
    }

    // range [L/2+1, L-1] map to [- (L-1)/2, -1]
    int kxleft2 = max( InKSpace::iLeftCorner.mX, Grid::iBoxSize.mX/2 + 1 );
    int kxright2 = InKSpace::iLeftCorner.mX + InKSpace::iLocalGridSize.mX;
    Kx = ( -( Grid::iBoxSize.mX - 1 )/2 + kxleft2 - Grid::iBoxSize.mX/2 - 1 )* dK;

    for( kx = kxleft2; kx < kxright2; kx++ )
    {
        for( int ky = 0; ky < InKSpace::iLocalGridSize.mY; ky++ )
            for( int kz = 0; kz < InKSpace::iLocalGridSize.mZ; kz++ )
            {
                COMPLEX_TYPE_THINGY c = static_cast<FFT_TYPE>( Kx ) * FFT_REV_IF->GetRecipSpaceElement( kx - InKSpace::iLeftCorner.mX, ky, kz );
//                FieldmX->PutRecipSpaceElement( kx - InKSpace::iLeftCorner.mX, ky, kz, COMPLEX_TYPE_THINGY_WITH_CONSTRUCTOR( imag(c), -real(c) ));
//                FieldmX->PutRecipSpaceElement( kx - InKSpace::iLeftCorner.mX, ky, kz, COMPLEX_TYPE_THINGY_WITH_CONSTRUCTOR( COMPLEX_MEMBER_IM_THINGY(c), -COMPLEX_MEMBER_RE_THINGY(c) ));
                FieldmX->PutRecipSpaceElement( kx - InKSpace::iLeftCorner.mX, ky, kz, complex<FFT_TYPE>(0.0,-1.0)*c);
            }
        Kx += dK;
    }
}






template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::InKSpace
::ConvoluteKy()
{
    int ky;
    double dK = 2.*M_PI*Grid::iBoxSize_1.mY*Grid::spacing_1.mY;

    // range [0,L/2] interpret as positive momenta
    int kyleft1 = InKSpace::iLeftCorner.mY;
    int kyright1 = min( InKSpace::iLeftCorner.mY + InKSpace::iLocalGridSize.mY, Grid::iBoxSize.mY/2 + 1 );
    double Ky = kyleft1 * dK;
    for( ky = kyleft1; ky < kyright1; ky++ )
    {
        for( int kx = 0; kx < InKSpace::iLocalGridSize.mX; kx++ )
            for( int kz = 0; kz < InKSpace::iLocalGridSize.mZ; kz++ )
            {
                COMPLEX_TYPE_THINGY c = static_cast<FFT_TYPE>( Ky ) * FFT_REV_IF->GetRecipSpaceElement( kx, ky - InKSpace::iLeftCorner.mY, kz );
//               FieldmY->PutRecipSpaceElement( kx, ky - InKSpace::iLeftCorner.mY, kz, COMPLEX_TYPE_THINGY_WITH_CONSTRUCTOR( COMPLEX_MEMBER_IM_THINGY(c), -COMPLEX_MEMBER_RE_THINGY(c) ));
                FieldmY->PutRecipSpaceElement( kx, ky - InKSpace::iLeftCorner.mY, kz, complex<FFT_TYPE>(0.0,-1.0)*c);
            }
        Ky += dK;
    }

    // range [L/2+1, L-1] map to [- (L-1)/2, -1]
    int kyleft2 = max( InKSpace::iLeftCorner.mY, Grid::iBoxSize.mY/2 + 1 );
    int kyright2 = InKSpace::iLeftCorner.mY + InKSpace::iLocalGridSize.mY;
    Ky = ( -( Grid::iBoxSize.mY - 1 )/2 + kyleft2 - Grid::iBoxSize.mY/2 - 1 )* dK;

    for( ky = kyleft2; ky < kyright2; ky++ )
    {
        for( int kx = 0; kx < InKSpace::iLocalGridSize.mX; kx++ )
            for( int kz = 0; kz < InKSpace::iLocalGridSize.mZ; kz++ )
            {
                COMPLEX_TYPE_THINGY c = static_cast<FFT_TYPE>( Ky ) * FFT_REV_IF->GetRecipSpaceElement( kx, ky - InKSpace::iLeftCorner.mY, kz );
//                FieldmY->PutRecipSpaceElement( kx, ky - InKSpace::iLeftCorner.mY, kz, COMPLEX_TYPE_THINGY_WITH_CONSTRUCTOR( COMPLEX_MEMBER_IM_THINGY(c), -COMPLEX_MEMBER_RE_THINGY(c) ));
                FieldmY->PutRecipSpaceElement( kx, ky - InKSpace::iLeftCorner.mY, kz, complex<FFT_TYPE>(0.0,-1.0)*c);
            }
        Ky += dK;
    }
}






template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::InKSpace
::ConvoluteKz()
{
    int kz;
    double dK = 2.*M_PI*Grid::iBoxSize_1.mZ*Grid::spacing_1.mZ;

    // range [0,L/2] interpret as positive momenta
    int kzleft1 = InKSpace::iLeftCorner.mZ;
    int kzright1 = min( InKSpace::iLeftCorner.mZ + InKSpace::iLocalGridSize.mZ, Grid::iBoxSize.mZ/2 + 1 );
    double Kz = kzleft1 * dK;
    for( kz = kzleft1; kz < kzright1; kz++ )
    {
        for( int kx = 0; kx < InKSpace::iLocalGridSize.mX; kx++ )
            for( int ky = 0; ky < InKSpace::iLocalGridSize.mY; ky++ )
            {
                COMPLEX_TYPE_THINGY c = static_cast<FFT_TYPE>( Kz ) * FFT_REV_IF->GetRecipSpaceElement( kx, ky, kz - InKSpace::iLeftCorner.mZ );
//                FieldmZ->PutRecipSpaceElement( kx, ky, kz - InKSpace::iLeftCorner.mZ, COMPLEX_TYPE_THINGY_WITH_CONSTRUCTOR( COMPLEX_MEMBER_IM_THINGY(c), -COMPLEX_MEMBER_RE_THINGY(c) ));
                FieldmZ->PutRecipSpaceElement( kx, ky, kz - InKSpace::iLeftCorner.mZ, complex<FFT_TYPE>(0.0,-1.0)*c);
            }
        Kz += dK;
    }

    // range [L/2+1, L-1] map to [- (L-1)/2, -1]
    int kzleft2 = max( InKSpace::iLeftCorner.mZ, Grid::iBoxSize.mZ/2 + 1 );
    int kzright2 = InKSpace::iLeftCorner.mZ + InKSpace::iLocalGridSize.mZ;
    Kz = ( -( Grid::iBoxSize.mZ - 1 )/2 + kzleft2 - Grid::iBoxSize.mZ/2 - 1 )* dK;

    for( kz = kzleft2; kz < kzright2; kz++ )
    {
        for( int kx = 0; kx < InKSpace::iLocalGridSize.mX; kx++ )
            for( int ky = 0; ky < InKSpace::iLocalGridSize.mY; ky++ )
            {
                COMPLEX_TYPE_THINGY c = static_cast<FFT_TYPE>( Kz ) * FFT_REV_IF->GetRecipSpaceElement( kx, ky, kz - InKSpace::iLeftCorner.mZ );
//                FieldmZ->PutRecipSpaceElement( kx, ky, kz - InKSpace::iLeftCorner.mZ, COMPLEX_TYPE_THINGY_WITH_CONSTRUCTOR( COMPLEX_MEMBER_IM_THINGY(c), -COMPLEX_MEMBER_RE_THINGY(c) ));
                FieldmZ->PutRecipSpaceElement( kx, ky, kz - InKSpace::iLeftCorner.mZ, complex<FFT_TYPE>(0.0,-1.0)*c);
            }
        Kz += dK;
    }
}


#endif



template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
double P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::InKSpace
::Dfinite( double K_i, double aSpacing )
{
    double D_J = 0.;

    for( int j = 1; j <= Differentiation::diffOrder; j++ )
    {
        double dj = sin(j*K_i)/(j*aSpacing);
        D_J += dj * Differentiation::coeff[j-1];
    }

    return D_J;
}





template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
double P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::InKSpace
::Wsqr( double KxH )
{
    double arg = 0.5 * KxH;
    double W = sin(arg)/arg;
    W *= W;
    double Wp = 1.;
    for( int i = 0; i < ParticleMesh::numAssignPoints; i++ )
        Wp *= W;

    return Wp;
}




// analytical result for the infinite sum in the denomenator for several cases of P
// obtained with the help of Mathematica

template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
double P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::InKSpace
::SumW2( double cosy, int ord )
{
    double result;

    switch( ord )
    {
    case 1:
        result = 1.;
        break;

    case 2:
        result = 1/3.*(2.+cosy);
        break;

    case 3:
        result = 1/30.*(16. + 13.*cosy + cosy*cosy);
        break;

    case 4:
        result = 1/630.*(272. + 297.*cosy + 60.*cosy*cosy + cosy*cosy*cosy);
        break;

    default:
        // not implemented, but use Mathematica to easily get more results
        assert(0);
    }

    return result;
}


//---------------------------------------------------------------------------------------
// SetGopt - pre-computes (optimized for accuracy) greens functions based on the paper
// "How to mesh up Ewald sums (I): A theoretical and numerical comparison of various
// particle mesh routines", Markus deserno, christian holm, April 27, 1998


template<class SourceIF, class ResultIF, class FFT2P3ME_FWD_IF, class FFT2P3ME_REV_IF>
void P3ME<SourceIF,ResultIF, FFT2P3ME_FWD_IF, FFT2P3ME_REV_IF>
::InKSpace
::SetGopt()
{
    // NOTE: here kx, ky, kz will be indices for physical momenta

    int lftX = InKSpace::iLeftCorner.mX, rtX = lftX + InKSpace::iLocalGridSize.mX;
    int lftY = InKSpace::iLeftCorner.mY, rtY = lftY + InKSpace::iLocalGridSize.mY;
    int lftZ = InKSpace::iLeftCorner.mZ, rtZ = lftZ + InKSpace::iLocalGridSize.mZ;

    // for k=(0,0,0) set Gopt to zero, see below why
    if( InKSpace::iLeftCorner.mX == 0 && InKSpace::iLeftCorner.mY == 0 && InKSpace::iLeftCorner.mZ == 0 )
        greenFun( 0, 0, 0 ) = 0.;

    double V = Grid::boxSize.mX * Grid::boxSize.mY * Grid::boxSize.mZ;

    for( int kx = lftX; kx < rtX; kx++ )
    {
        double knX = 2*kx*M_PI*Grid::iBoxSize_1.mX;
        double cosknX = cos( knX );
        SumWVcX( kx - lftX ) = SumW2( cosknX, ParticleMesh::numAssignPoints );
        SumDmuWVcX( kx - lftX ) = (1.-cosknX)*SumW2( cosknX, ParticleMesh::numAssignPoints - 1 )*Grid::spacing_1.mX*Grid::spacing_1.mX;
    }

    for( int ky = lftY; ky < rtY; ky++ )
    {
        double knY = 2*ky*M_PI*Grid::iBoxSize_1.mY;
        double cosknY = cos( knY );
        SumWVcY( ky - lftY ) = SumW2( cosknY, ParticleMesh::numAssignPoints );
        SumDmuWVcY( ky - lftY ) = (1.-cosknY)*SumW2( cosknY, ParticleMesh::numAssignPoints - 1 )*Grid::spacing_1.mY*Grid::spacing_1.mY;
    }

    for( int kz = lftZ; kz < rtZ; kz++ )
    {
        double knZ = 2*kz*M_PI*Grid::iBoxSize_1.mZ;
        double cosknZ = cos( knZ );
        SumWVcZ( kz - lftZ ) = SumW2( cosknZ, ParticleMesh::numAssignPoints );
        SumDmuWVcZ( kz - lftZ ) = (1.-cosknZ)*SumW2( cosknZ, ParticleMesh::numAssignPoints - 1 )*Grid::spacing_1.mZ*Grid::spacing_1.mZ;
    }

    XYZ dft;
    for( int kx = lftX; kx < rtX; kx++ )
    {
        // symmkx: "symmetric" momentum
        // range [0,L/2] interpret as positive momenta
        // range [L/2+1, L-1] map to [- (L-1)/2, -1]
        int symmkx = ( kx > Grid::iBoxSize.mX/2 ) ? ( kx - Grid::iBoxSize.mX ) : kx;
        double knX = 2*symmkx*M_PI*Grid::iBoxSize_1.mX;
        //double cosknX = cos( knX );
        double sumW2x, sumW2x1;
        sumW2x = SumWVcX( kx - lftX );
        if( Differentiation::diffOrder > 0 )
            dft.mX = Dfinite( knX, Grid::spacing.mX );
        else
            sumW2x1 = SumDmuWVcX( kx - lftX );
        double WsqrX = ( kx ? Wsqr( knX ) : 1.0 );

        for( int ky = lftY; ky < rtY; ky++ )
        {
            // symmky: "symmetric" momentum
            // range [0,L/2] interpret as positive momenta
            // range [L/2+1, L-1] map to [- (L-1)/2, -1]
            int symmky = ( ky > Grid::iBoxSize.mY/2 ) ? ( ky - Grid::iBoxSize.mY ) : ky;
            double knY = 2*symmky*M_PI*Grid::iBoxSize_1.mY;
            double cosknY = cos( knY );
            double sumW2y, sumW2y1;
            sumW2y = SumWVcY( ky - lftY );
            double sumW2xy = sumW2x * sumW2y;
            if( Differentiation::diffOrder > 0 )
                dft.mY = Dfinite( knY, Grid::spacing.mY );
            else
                sumW2y1 = SumDmuWVcY( ky - lftY );
            double WsqrY = ( ky ? Wsqr( knY ) : 1.0 );

            for( int kz = lftZ; kz < rtZ; kz++ )
            {
                // for k={0,0,0} Gopt is singular, 4\pi / k^2
                // later it is multiplied by charge(k) which is zero for neutral systems;
                // this is the term that depends on the boundary conditions;
                // here set Gopt to zero just to have it initialized.
                // Boundary conditions dependent term can be
                // added separately, same as for Ewald without PME
                if( kx == 0 && ky == 0 && kz == 0 )
                    continue;

                // not understood: for discreet case (diffOrder>0) can get Dft=0
                // since sin(2*pi/L*kx*j) can be zero when kx/L=1/2
                // set Gopt to 0 in this case;
                // the only plausability argument is that for such large K they are suppressed
                // by exp(-k^2/4alpha^2) anyway
                if( Differentiation::diffOrder > 0 && (2*kx == Grid::iBoxSize.mX || kx == 0)
                    && (2*ky == Grid::iBoxSize.mY || ky == 0) && (2*kz == Grid::iBoxSize.mZ || kz == 0) )
                {
                    greenFun( kx - lftX, ky - lftY, kz - lftZ ) = 0.;
                    continue;
                }

                // symmkz: "symmetric" momentum
                // range [0,L/2] interpret as positive momenta
                // range [L/2+1, L-1] map to [- (L-1)/2, -1]
                int symmkz = ( kz > Grid::iBoxSize.mZ/2 ) ? ( kz - Grid::iBoxSize.mZ ) : kz;
                double knZ = 2*symmkz*M_PI*Grid::iBoxSize_1.mZ;
                double cosknZ = cos( knZ );
                double sumW2z, sumW2z1;
                sumW2z = SumWVcZ( kz - lftZ );
                double sumW2xyz = sumW2xy * sumW2z;
                if( Differentiation::diffOrder > 0 )
                    dft.mZ = Dfinite( knZ, Grid::spacing.mZ );
                else
                    sumW2z1 = SumDmuWVcZ( kz - lftZ );
                double WsqrZ = ( kz ? Wsqr( knZ ) : 1.0 );

                // when use symmetric momentum scheme, do not need to sum over
                // alias momenta different by 2\pi/h since they are suppressed
                // by a large exponential factor that goes to 0 as the grid spacing
                // goes to zero
                double numerat;
                {
                    double Wxyz2 = WsqrX * WsqrY * WsqrZ;

                    XYZ Ksp;
                    Ksp.mX = knX * Grid::spacing_1.mX;
                    Ksp.mY = knY * Grid::spacing_1.mY;
                    Ksp.mZ = knZ * Grid::spacing_1.mZ;
                    double K2 = Ksp*Ksp;
#if 1  // try to trick gcc by hiding a divide by zero
                    double spooge = MSD_IF::GetEwaldAlpha() * MSD_IF::GetEwaldAlpha();
                    double fact = Wxyz2 * exp(-K2/(4*spooge));
#else
                    double fact = Wxyz2 * exp(-K2/(4*MSD_IF::GetEwaldAlpha()*MSD_IF::GetEwaldAlpha()));
#endif

                    numerat = 4.*M_PI*fact;
                    if( Differentiation::diffOrder > 0 )
                        numerat *= ( Ksp*dft ) / K2;
                }

                double sumW2Dft2;
                if( Differentiation::diffOrder > 0 )
                {
                    double dft2 = dft*dft;
                    sumW2Dft2 = sumW2xyz * dft2;
                }
                else
                {
                    // the infinite sum in the denomenator can be expressed in the following way
                    sumW2Dft2 =
                        sumW2x1*sumW2y*sumW2z
                        + sumW2x*sumW2y1*sumW2z
                        + sumW2x*sumW2y*sumW2z1;
                    sumW2Dft2 *= 2.;
                }

                // need to divide by cell volume for correct charge density calculation
                // and by the total number of grid points in the box for fftw;
                // combining -> just divide by total volume of the box

                greenFun( kx - lftX, ky - lftY, kz - lftZ )
                    = numerat / ( sumW2xyz * sumW2Dft2 * V );
            }
        }
    }
}


#endif
