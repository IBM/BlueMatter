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
 
#ifndef UDF_HELPERLEKNER_HPP
#define UDF_HELPERLEKNER_HPP



// Some warnings and information messages were expressed using cerr.
// Convert to log lines for BGL; probably should be expressed as log lines anyway

#include <math.h>
#include <values.h>
// GNU GSL isn't distributed with BlueMatter; look here if you would like it
//                      http://www.gnu.org/software/gsl/
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_sf_psi.h>
#include <gsl/gsl_sf_zeta.h>


struct LeknerIncludeInvR { enum { INCLUDE_INV_R = 1 }; };
struct LeknerExcludeInvR { enum { INCLUDE_INV_R = 0 }; };

template <class MDVM,class DynVarMgrIF,class RTG>
class UDF_HelperLekner
{
public:

    static double epsilon;
    static const int LargeN;

    static int Size1dim;
    static double *Uxy;
    static double *Uyz;
    static double *Uzx;
    static double *Uind1ind2[3];

    static double *cosp, *cosq, *sinp, *sinq;

    inline static void OneTimeInit( MDVM &aMDVM );

    inline static void InitUpq( MDVM &aMDVM );

    inline static int SelfEnergy ( double& phi );


    template<class IncludeInvR>
    inline static int LargeZpairwise( const double x, const double y, const double z,
                                      const double boxX_1, const double boxY_1, const double boxZ_1,
                                      const double boxX, const double boxY,
                                      const double *Upq,
                                      const double boxZ,
                                      const int Np, const int Nq,
                                      double& phi, double& fx, double& fy, double& fz );

    template<class IncludeInvR>
    inline static int SmallZpairwise( const double x, const double y, const double z,
                                      const double boxX_1, const double boxY_1, const double boxZ_1,
                                      const double *Upq,
                                      const double boxX, const double boxY, const double boxZ,
                                      const int smNp, const int smNq, const int smNK0, const int smNpsi,
                                      double& phi, double& fx, double& fy, double& fz );

    template<class IncludeInvR>
    inline static int pairwiseRotated( MDVM &aMDVM, const double chargeAxB );

    inline static double BesselK0( const double x ) { return gsl_sf_bessel_K0( x ); }
    inline static double BesselK1( const double x ) { return gsl_sf_bessel_K1( x ); }
    inline static double Psi( const double x ) { return gsl_sf_psi( x ); }
    inline static double Psi( const int n, const double x ) { return gsl_sf_psi_n( n, x ); }

    // convergence evaluators - calculate the number of terms needed
    inline static void NNforLargeZpairwise( const double z, const double boxX, const double boxY, double& dNx, double& dNy );
    const static double costLargeZpairwise;

    inline static void NNforSmallZpairwise( const double z, const double boxX, const double boxY, const double boxZ, double& dNx, double& dNy );
    const static double costSmallZpairwise;

    inline static void NNforSmallZpairwiseK0Sum( const double boxX, const double boxY_1, double& dN );
    const static double costSmallZpairwiseK0Sum;

    inline static void NNforSmallZpairwisePsiSum( const double fabsx, const double rho2, const double boxX, const double boxX_1, double& dN );
    const static double costSmallZpairwisePsiSum;
};

template <class MDVM,class DynVarMgrIF,class RTG>
int UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::Size1dim;

template <class MDVM,class DynVarMgrIF,class RTG>
double *UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::Uxy;

template <class MDVM,class DynVarMgrIF,class RTG>
double *UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::Uyz;

template <class MDVM,class DynVarMgrIF,class RTG>
double *UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::Uzx;

template <class MDVM,class DynVarMgrIF,class RTG>
double *UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::Uind1ind2[3];

template <class MDVM,class DynVarMgrIF,class RTG>
double *UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::cosp;

template <class MDVM,class DynVarMgrIF,class RTG>
double *UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::cosq;

template <class MDVM,class DynVarMgrIF,class RTG>
double *UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::sinp;

template <class MDVM,class DynVarMgrIF,class RTG>
double *UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::sinq;

template <class MDVM,class DynVarMgrIF,class RTG>
const int UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::LargeN = 100;

template <class MDVM,class DynVarMgrIF,class RTG>
double UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::epsilon;

template <class MDVM,class DynVarMgrIF,class RTG>
const double UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::costLargeZpairwise = 1;

template <class MDVM,class DynVarMgrIF,class RTG>
const double UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::costSmallZpairwise = 1;

template <class MDVM,class DynVarMgrIF,class RTG>
const double UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::costSmallZpairwiseK0Sum = 10;

template <class MDVM,class DynVarMgrIF,class RTG>
const double UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::costSmallZpairwisePsiSum = 20;

template <class MDVM,class DynVarMgrIF,class RTG>
inline void UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::InitUpq( MDVM &aMDVM )
{
    XYZ dynamBoxDimVect_1 =  DynVarMgrIF::mDynamicBoxInverseDimensionVector;
             // aMDVM.GetDynamicBoxInverseDimensionVector();
    const double BoxSizeX_1 = dynamBoxDimVect_1.mX;
    const double BoxSizeY_1 = dynamBoxDimVect_1.mY;
    const double BoxSizeZ_1 = dynamBoxDimVect_1.mZ;

    double TwoPiBoxSizeX_1 = 2 * M_PI * BoxSizeX_1;
    double TwoPiBoxSizeY_1 = 2 * M_PI * BoxSizeY_1;
    double TwoPiBoxSizeZ_1 = 2 * M_PI * BoxSizeZ_1;

    int p, q;

    //--- p = 0 ---//

    for( q = 1; q <= LargeN; ++q )
    {
        double qx = TwoPiBoxSizeX_1 * q;
        double qy = TwoPiBoxSizeY_1 * q;
        double qz = TwoPiBoxSizeZ_1 * q;

        Uxy[q] = qy;
        Uyz[q] = qz;
        Uzx[q] = qx;
    }

    //--- p = -LargeN, ..., -1, 1, ..., LargeN ---//

    for( p = 1; p <= LargeN; ++p )
    {
        double px = TwoPiBoxSizeX_1 * p;
        double px2 = px * px;

        double py = TwoPiBoxSizeY_1 * p;
        double py2 = py * py;

        double pz = TwoPiBoxSizeZ_1 * p;
        double pz2 = pz * pz;

        int indp = p * Size1dim;

        //--- q = 0 ---//

        Uxy[indp] = px;
        Uyz[indp] = py;
        Uzx[indp] = pz;

        //--- q = -LargeN, ..., -1, 1, ..., LargeN ---//

        for( q = 1; q <= LargeN; ++q )
        {
            double qx2 = TwoPiBoxSizeX_1 * q;
            qx2 *= qx2;

            double qy2 = TwoPiBoxSizeY_1 * q;
            qy2 *= qy2;

            double qz2 = TwoPiBoxSizeZ_1 * q;
            qz2 *= qz2;

            int ind =  q + indp;

            double g;

            g = 1./sqrt( px2 + qy2 );
            Uxy[ind] = 1./g;

            g = 1./sqrt( py2 + qz2 );
            Uyz[ind] = 1./g;

            g = 1./sqrt( pz2 + qx2 );
            Uzx[ind] = 1./g;
        }
    }
}

template <class MDVM,class DynVarMgrIF,class RTG>
inline void
UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::OneTimeInit( MDVM &aMDVM )
{
    static int done = 0;
    if( ! done )
        done = 1;
    else
        return;

    //WARNING:: THIS CODE IS COMMENTED OUT DUE TO G++  
  ////////////////  epsilon = MSD_IF::GetLeknerEpsilon();

    Size1dim = LargeN + 1;
    int size = Size1dim * Size1dim;

//     Uxy = new double[ size ];
//     Uyz = new double[ size ];
//     Uzx = new double[ size ];
    pkNew( &Uxy, size );
    pkNew( &Uyz, size );
    pkNew( &Uzx, size );

//     cosp = new double[ Size1dim ];
//     cosq = new double[ Size1dim ];
//     sinp = new double[ Size1dim ];
//     sinq = new double[ Size1dim ];
    pkNew(& cosp, Size1dim );
    pkNew(& cosq, Size1dim );
    pkNew(& sinp, Size1dim );
    pkNew(& sinq, Size1dim );

    if( ! Uxy || ! Uyz || ! Uzx || ! cosp || ! cosq || ! sinp || ! sinq )
        exit( -1 );

    Uind1ind2[0] = Uyz;
    Uind1ind2[1] = Uzx;
    Uind1ind2[2] = Uxy;

    InitUpq( aMDVM );
}

template <class MDVM,class DynVarMgrIF,class RTG>
template<class IncludeInvR>
inline int UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::LargeZpairwise( const double x, const double y, const double z,
                                                   const double boxX_1, const double boxY_1, const double boxZ_1,
                                                   const double boxX, const double boxY,
                                                   const double *Upq, const double boxZ,
                                                   const int Np, const int Nq,
                                                   double& phi, double& fx, double& fy, double& fz )
{
    //cout << "LARGE 0000" << endl;

    int status = 0;

    phi = 0;
    fx = 0;
    fy = 0;
    fz = 0;

    double fabsz = fabs( z );
    double signz = ( z < 0 ) ? -1 : 1;

    double xUp1 = 2 * M_PI * x * boxX_1;
    double yUq1 = 2 * M_PI * y * boxY_1;

    // compute trigonometric functions;
    int p, q;

    cosp[ 0 ] = 1.;
    sinp[ 0 ] = 0.;
    for( p = 1; p <= Np; ++p )
    {
        cosp[ p ] = cos( xUp1 * p );
        sinp[ p ] = sin( xUp1 * p );
    }
    cosq[ 0 ] = 1.;
    sinq[ 0 ] = 0.;
    for( q = 1; q <= Nq; ++q )
    {
        cosq[ q ] = cos( yUq1 * q );
        sinq[ q ] = sin( yUq1 * q );
    }


    // q!=0, p!=0 terms
    for( p = 1; p <= Np; ++p )
        for( q = 1; q <= Nq; ++q )
        {
            double upq = Upq[p*Size1dim + q];
            double ExpUpqZ = exp( - upq * fabsz );
            double ExpUpqLz = exp( - upq * boxZ );
            double ExpUpqLzZ = ExpUpqLz / ExpUpqZ;
            double fourvalue = 4 * ( ExpUpqZ + ExpUpqLzZ ) / ( upq * ( 1 - ExpUpqLz ) );
            phi += fourvalue * cosp[p] * cosq[q];
            fx += p * fourvalue * sinp[ p ] * cosq[ q ];
            fy += q * fourvalue * cosp[ p ] * sinq[ q ];
            fz -= 4 * ( ExpUpqZ - ExpUpqLzZ ) / ( ExpUpqLz - 1 ) * cosp[ p ] * cosq[ q ];
        }

    // q == 0 terms
    for( p = 1; p <= Np; ++p )
    {
        double upq = Upq[ p * Size1dim ];
        double ExpUpqZ = exp( - upq * fabsz );
        double ExpUpqLz = exp( - upq * boxZ );
        double ExpUpqLzZ = ExpUpqLz / ExpUpqZ;
        double twovalue = 2 * ( ExpUpqZ + ExpUpqLzZ ) / ( upq * ( 1 - ExpUpqLz ) );
        phi += twovalue * cosp[p];
        fx += p * twovalue * sinp[ p ];
        fz -= 2 * ( ExpUpqZ - ExpUpqLzZ ) / ( ExpUpqLz - 1 ) * cosp[ p ];
    }

    // p == 0 terms
    for( q = 1; q <= Nq; ++q )
    {
        double upq = Upq[ q ];
        double ExpUpqZ = exp( - upq * fabsz );
        double ExpUpqLz = exp( - upq * boxZ );
        double ExpUpqLzZ = ExpUpqLz / ExpUpqZ;
        double twovalue = 2 * ( ExpUpqZ + ExpUpqLzZ ) / ( upq * ( 1 - ExpUpqLz ) );
        phi += twovalue * cosq[q];
        fy += q * twovalue * sinq[ q ];
        fz -= 2 * ( ExpUpqZ - ExpUpqLzZ ) / ( ExpUpqLz - 1 ) * cosq[ q ];
    }

    phi += boxZ_1 * z * z - fabsz;

    phi *= 2 * M_PI * boxX_1 * boxY_1;

    fx *= 2 * M_PI * boxX_1 * boxY_1 * ( 2 * M_PI * boxX_1 );
    fy *= 2 * M_PI * boxX_1 * boxY_1 * ( 2 * M_PI * boxY_1 );
    fz  = 2 * M_PI * boxX_1 * boxY_1 * ( signz * fz + signz - boxZ_1 * 2 * z );

    if( IncludeInvR::INCLUDE_INV_R == 0 )
    {
        // the evil sqrt
        double r_1 = 1/sqrt( x*x + y*y + z*z );
        double r_3 = r_1 * r_1 * r_1;
        phi -= r_1;
        fx -= x * r_3;
        fy -= y * r_3;
        fz -= z * r_3;
    }

    return status;
}




template <class MDVM,class DynVarMgrIF,class RTG>
inline int UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::SelfEnergy ( double& phi )
{
    const double x = 0, y = 0, z = 0;
    const double *Upq = Uxy;

    XYZ dynamBoxDimVect_1 =  RTG::mDynamicBoxInverseDimensionVector;
    const double boxX_1 = dynamBoxDimVect_1.mX;
    const double boxY_1 = dynamBoxDimVect_1.mY;
    const double boxZ_1 = dynamBoxDimVect_1.mZ;

    XYZ dynamBoxDimVect =  RTG::mDynamicBoxDimensionVector;
    const double boxX = dynamBoxDimVect.mX;
    const double boxY = dynamBoxDimVect.mY;
    const double boxZ = dynamBoxDimVect.mZ;

    double dsmNp, dsmNq;
    NNforSmallZpairwise( fabs(z), boxX, boxY, boxZ, dsmNp, dsmNq );
    int smNp = int(dsmNp);
    int smNq = int(dsmNq);
    //cerr << " ====== boxX " << boxX << " boxY " << boxY << " boxZ " << boxZ << endl;
    //cerr << " smNp = " << smNp << " smNq = " << smNq << endl; //////////////////$$$$$$$$#########???????

    double dsmNK0;
    NNforSmallZpairwiseK0Sum( boxX, boxY_1, dsmNK0 );
    int smNK0 = int( dsmNK0 );

//    int smNpsi = 0;

    //cout << "SMALL" << endl;
    int status = 0;

    phi = 0;
//    fx = 0;
//    fy = 0;
//    fz = 0;

    double z2 = z * z;

    double xUp1 = 2 * M_PI * x * boxX_1;
    double yUq1 = 2 * M_PI * y * boxY_1;

    //---------------------------------------------------------------------------
    //-  first - sum with exponentials. This one here should converge very fast.
    //-  the structure of this block is similar to the one in LargeZpairwise so
    //-  should be modified at the same time with it

    // determine how many terms to keep for a given accuracy using analytic formula
    int Np = smNp, Nq = smNq;
    //NNforSmallZpairwise( fabs(z), boxX, boxY, boxZ, Np, Nq );
    //
    //cout << "NNforSmallZpairwise===> Np = " << Np << " Nq = " << Nq << endl;

    if( Np > Size1dim || Nq > Size1dim )
    {
      BegLogLine( 1 )
        << "WARNING: not enough space in cosp, sinp, ... arrays for exponential terms"
        << EndLogLine ;
        //exit( 1 );
        Np = Nq = Size1dim - 1; // correct to avoid exceeding array size, -1 since use <= below
    }


    // compute trigonometric functions;
    int p, q;

    cosp[ 0 ] = 1.;
    sinp[ 0 ] = 0.;
    for( p = 1; p <= Np; ++p )
    {
        cosp[ p ] = cos( xUp1 * p );
        sinp[ p ] = sin( xUp1 * p );
    }
    cosq[ 0 ] = 1.;
    sinq[ 0 ] = 0.;
    for( q = 1; q <= Nq; ++q )
    {
        cosq[ q ] = cos( yUq1 * q );
        sinq[ q ] = sin( yUq1 * q );
    }

    // q!=0, p!=0 terms
    for( p = 1; p <= Np; ++p )
        for( q = 1; q <= Nq; ++q )
        {
            double upq = Upq[p*Size1dim + q];
            double ExpUpqZ = exp( upq * z );
            double ExpUpqLz = exp( upq * boxZ );
            double ExpUpqZ_1 = 1 / ExpUpqZ;
            double fourvalue = 4 * ( ExpUpqZ + ExpUpqZ_1 ) / ( upq * ( ExpUpqLz - 1 ) );
            phi += fourvalue * cosp[p] * cosq[q];
//            fx += p * fourvalue * sinp[ p ] * cosq[ q ];
//            fy += q * fourvalue * cosp[ p ] * sinq[ q ];
//            fz -= 4 * ( ExpUpqZ - ExpUpqZ_1 ) / ( ExpUpqLz - 1 ) * cosp[ p ] * cosq[ q ];
        }

    // q == 0 terms
    for( p = 1; p <= Np; ++p )
    {
        double upq = Upq[p*Size1dim];
        double ExpUpqZ = exp( upq * z );
        double ExpUpqLz = exp( upq * boxZ );
        double ExpUpqZ_1 = 1 / ExpUpqZ;
        double twovalue = 2 * ( ExpUpqZ + ExpUpqZ_1 ) / ( upq * ( ExpUpqLz - 1 ) );
        phi += twovalue * cosp[p];
//        fx += p * twovalue * sinp[ p ];
//        fz -= 2 * ( ExpUpqZ - ExpUpqZ_1 ) / ( ExpUpqLz - 1 ) * cosp[ p ];
    }

    // p == 0 terms
    for( q = 1; q <= Nq; ++q )
    {
        double upq = Upq[q];
        double ExpUpqZ = exp( upq * z );
        double ExpUpqLz = exp( upq * boxZ );
        double ExpUpqZ_1 = 1 / ExpUpqZ;
        double twovalue = 2 * ( ExpUpqZ + ExpUpqZ_1 ) / ( upq * ( ExpUpqLz - 1 ) );
        phi += twovalue * cosq[q];
//        fy += q * twovalue * sinq[ q ];
//        fz -= 2 * ( ExpUpqZ - ExpUpqZ_1 ) / ( ExpUpqLz - 1 ) * cosq[ q ];
    }


    //--------------------------------------------------------------------------
    //--- (some) stand alone terms

//    double ExpUqZ = exp( - 2 * M_PI * boxY_1 * z );
//    double CosUqY = cos( 2 * M_PI * boxY_1 * y );
//    double SinUqY = sin( 2 * M_PI * boxY_1 * y );
//
//    double divfact = 1./ ( 1 - 2*CosUqY*ExpUqZ + ExpUqZ*ExpUqZ );
//
//    phi += boxZ_1 * z2 - z;
//    fz -= boxZ_1 * 2 * z - 1;
//    fz += 2 * ( CosUqY - ExpUqZ ) * ExpUqZ * divfact;
//
    phi *= 2 * M_PI * boxY_1;
//    fx *= 2 * M_PI * boxY_1;
//    fy *= 2 * M_PI * boxY_1;
//    fz *= boxY_1;
//
//    // here goes rho squared
//    double rho2 = y*y + z2;
//    double rho_1 = 1 / sqrt( rho2 );
//    double rho_2 = rho_1 * rho_1;
//
//    phi += log( rho2 * boxX_1 * boxX_1 * 1./4. * divfact );
    phi += 2 * log( boxX_1 * boxY / 4. / M_PI );  // replaces divergent expression above
//    fy += 2 * SinUqY * ExpUqZ * divfact;
//    // the (y/rho2) term in fy comes later
//
//    //----
//    fy *= boxY_1;

    //-------------------------------------------------------------------------------
    //-- BesselK terms (converge fast)

    // the result, NN, corresponds to the product p*l
    // but usually this is not the largest sum, so the simplest thing to do is to
    // use it as the boundary for both p and l separately, or
    // at least to use something like a romb structure with p+l=NN being the boundary

    int NN = smNK0;
    //NNforSmallZpairwiseK0Sum( boxX, boxY_1, NN );
    //cout << "NNforSmallZpairwise bessel ===> NN = " << NN << endl;

    if( NN > Size1dim )
    {
      BegLogLine( 1 )
        << "WARNING: not enough space in cosp, sinp, ... arrays for bessel summation"
        << EndLogLine ;
        //exit( 1 );
        NN = Size1dim - 1;  // correct to avoid exceeding array size, -1 because below use <= for the limits
    }

    // initialize the rest of the arrays, here initialize only arrays for p, not q - only need them
    for( p = Np + 1; p <= NN; ++p )
    {
        cosp[ p ] = cos( xUp1 * p );
        sinp[ p ] = sin( xUp1 * p );
    }

    for( int l = 1; l <= NN; ++l )
    {
        double lboxY = l * boxY;
        double ylp = y + lboxY;
        double ylm = y - lboxY;
        double rholp_1 = 1/sqrt( ylp*ylp + z2 );
        double rholm_1 = 1/sqrt( ylm*ylm + z2 );
        double rholp = 1 / rholp_1;
        double rholm = 1 / rholm_1;

        for( p = 1; p <= NN - l; ++p )
        {
            double up = Upq[p*Size1dim];
            double uprholp = up * rholp;
            double uprholm = up * rholm;
            double bepm = BesselK0( uprholp ) + BesselK0( uprholm );
            double be1p = BesselK1( uprholp );
            double be1m = BesselK1( uprholm );
            
            phi += 4 * bepm * cosp[p];
//            fx += 4 * p * bepm * sinp[p];
            double commfact = boxX_1 * 4 * p * cosp[p];
            double be1prholp_1 = be1p * rholp_1;
            double be1mrholm_1 = be1m * rholm_1;
//            fy += commfact * ( be1prholp_1 * ylp + be1mrholm_1 * ylm );
            //cout << "delta fy for l="<<l<<" p="<<p<<" "<<commfact * ( be1prholp_1 * ylp + be1mrholm_1 * ylm )<<endl;
//            fz += commfact * ( be1prholp_1 * z + be1mrholm_1 * z );
            //cout << "delta fz for l="<<l<<" p="<<p<<" "<<commfact * ( be1prholp_1 * z + be1mrholm_1 * z )<<endl;
        }
    }

    // for fx, fy this is from u_p, u_q after differentiation
//    fx *= 2 * M_PI;
//    fy *= 2 * M_PI;
//    fz *= 2 * M_PI;

    //------------------------------------
    // other stand alone terms
//    fy -= 2 * y * rho_2;
//    fz -= 2 * z * rho_2;

    //cout << "before phi sum " << phi << " " << fx << " " << fy << " " << fz << endl;
    //////////





    //-------------------------------------------------------------------------------
    // Here is a problematic sum of polygamma functions that converge really slowly

    double argp = 1 + x * boxX_1;
    double argm = 1 - x * boxX_1;

    // first - term for n=0

    phi -= Psi( argp ) + Psi( argm );
//    fx += Psi( 1, argp ) - Psi( 1, argm );

    // now - the rest of the sum

//    double rhoux2 = rho2 * boxX_1 * boxX_1;
//    double rhoux2n = 1;
//    double twonfact = 1;
//    double yrho_2 = y * rho_2;
//    double zrho_2 = z * rho_2;
//
//    // these are for the Binomial
//    double term = -0.5;
//    double factorial = 1;
//    double product = 1;
//
//    NN = smNpsi;
//    //NNforSmallZpairwisePsiSum( fabs(x), rho_1, boxX, boxX_1, NN ); ///???
//    //cout << "NNforSmallZpairwise Psi ===> NN = " << NN << endl;
//
//    for( int n = 1; n <= NN; ++n )
//    {
//        int two_n = 2 * n;
//        rhoux2n *= rhoux2;
//        twonfact *= two_n * (two_n - 1);
//
//        // binomial
//        factorial *= n;
//        product *= term;
//        term -= 1;
//        double binom = product / factorial;
//
//        double fact = binom * rhoux2n / twonfact;
//        double psip = Psi( two_n, argp );
//        double psim = Psi( two_n, argm );
//        double phiterm = fact * ( psip + psim );
//        phi -= phiterm;
//        double psip1 = Psi( two_n + 1, argp );
//        double psim1 = Psi( two_n + 1, argm );
//        double fxterm = fact * ( psip1 - psim1 );
//        fx += fxterm;
//        double phitermtwo_n = phiterm * two_n;
//        double fyterm = phitermtwo_n * yrho_2;
//        fy += fyterm;
//        double fzterm = phitermtwo_n * zrho_2;
//        fz += fzterm;
//    }

    // the final touch
    phi *= boxX_1;
//    fx *= boxX_1 * boxX_1;
//    fy *= boxX_1;
//    fz *= boxX_1;

//    if( IncludeInvR::INCLUDE_INV_R )
//    {
//        // the evil sqrt
//        double r_1 = 1/sqrt( x*x + y*y + z2 );
//        double r_3 = r_1 * r_1 * r_1;
//       phi += r_1;
//        fx += x * r_3;
//        fy += y * r_3;
//        fz += z * r_3;
//    }

    return status;
}




template <class MDVM,class DynVarMgrIF,class RTG>
template <class IncludeInvR>
inline int UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::SmallZpairwise ( const double x, const double y, const double z,
                                          const double boxX_1, const double boxY_1, const double boxZ_1,
                                          const double *Upq,
                                          const double boxX, const double boxY, const double boxZ,
                                          const int smNp, const int smNq, const int smNK0, const int smNpsi,
                                          double& phi, double& fx, double& fy, double& fz )
{
    //cout << "SMALL" << endl;
    int status = 0;

    phi = 0;
    fx = 0;
    fy = 0;
    fz = 0;

    double z2 = z * z;

    double xUp1 = 2 * M_PI * x * boxX_1;
    double yUq1 = 2 * M_PI * y * boxY_1;

    //---------------------------------------------------------------------------
    //-  first - sum with exponentials. This one here should converge very fast.
    //-  the structure of this block is similar to the one in LargeZpairwise so
    //-  should be modified at the same time with it

    // determine how many terms to keep for a given accuracy using analytic formula
    int Np = smNp, Nq = smNq;
    //NNforSmallZpairwise( fabs(z), boxX, boxY, boxZ, Np, Nq );
    //
    //cout << "NNforSmallZpairwise===> Np = " << Np << " Nq = " << Nq << endl;

    if( Np > Size1dim || Nq > Size1dim )
    {
      BegLogLine( 1 )
        << "WARNING: not enough space in cosp, sinp, ... arrays for exponential terms"
        << EndLogLine ;
        //exit( 1 );
        Np = Nq = Size1dim - 1; // correct to avoid exceeding array size, -1 since use <= below
    }


    // compute trigonometric functions;
    int p, q;

    cosp[ 0 ] = 1.;
    sinp[ 0 ] = 0.;
    for( p = 1; p <= Np; ++p )
    {
        cosp[ p ] = cos( xUp1 * p );
        sinp[ p ] = sin( xUp1 * p );
    }
    cosq[ 0 ] = 1.;
    sinq[ 0 ] = 0.;
    for( q = 1; q <= Nq; ++q )
    {
        cosq[ q ] = cos( yUq1 * q );
        sinq[ q ] = sin( yUq1 * q );
    }

    // q!=0, p!=0 terms
    for( p = 1; p <= Np; ++p )
        for( q = 1; q <= Nq; ++q )
        {
            double upq = Upq[p*Size1dim + q];
            double ExpUpqZ = exp( upq * z );
            double ExpUpqLz = exp( upq * boxZ );
            double ExpUpqZ_1 = 1 / ExpUpqZ;
            double fourvalue = 4 * ( ExpUpqZ + ExpUpqZ_1 ) / ( upq * ( ExpUpqLz - 1 ) );
            phi += fourvalue * cosp[p] * cosq[q];
            fx += p * fourvalue * sinp[ p ] * cosq[ q ];
            fy += q * fourvalue * cosp[ p ] * sinq[ q ];
            fz -= 4 * ( ExpUpqZ - ExpUpqZ_1 ) / ( ExpUpqLz - 1 ) * cosp[ p ] * cosq[ q ];
        }

    // q == 0 terms
    for( p = 1; p <= Np; ++p )
    {
        double upq = Upq[p*Size1dim];
        double ExpUpqZ = exp( upq * z );
        double ExpUpqLz = exp( upq * boxZ );
        double ExpUpqZ_1 = 1 / ExpUpqZ;
        double twovalue = 2 * ( ExpUpqZ + ExpUpqZ_1 ) / ( upq * ( ExpUpqLz - 1 ) );
        phi += twovalue * cosp[p];
        fx += p * twovalue * sinp[ p ];
        fz -= 2 * ( ExpUpqZ - ExpUpqZ_1 ) / ( ExpUpqLz - 1 ) * cosp[ p ];
    }

    // p == 0 terms
    for( q = 1; q <= Nq; ++q )
    {
        double upq = Upq[q];
        double ExpUpqZ = exp( upq * z );
        double ExpUpqLz = exp( upq * boxZ );
        double ExpUpqZ_1 = 1 / ExpUpqZ;
        double twovalue = 2 * ( ExpUpqZ + ExpUpqZ_1 ) / ( upq * ( ExpUpqLz - 1 ) );
        phi += twovalue * cosq[q];
        fy += q * twovalue * sinq[ q ];
        fz -= 2 * ( ExpUpqZ - ExpUpqZ_1 ) / ( ExpUpqLz - 1 ) * cosq[ q ];
    }


    //--------------------------------------------------------------------------
    //--- (some) stand alone terms

    double ExpUqZ = exp( - 2 * M_PI * boxY_1 * z );
    double CosUqY = cos( 2 * M_PI * boxY_1 * y );
    double SinUqY = sin( 2 * M_PI * boxY_1 * y );

    double divfact = 1./ ( 1 - 2*CosUqY*ExpUqZ + ExpUqZ*ExpUqZ );

    phi += boxZ_1 * z2 - z;
    fz -= boxZ_1 * 2 * z - 1;
    fz += 2 * ( CosUqY - ExpUqZ ) * ExpUqZ * divfact;

    phi *= 2 * M_PI * boxY_1;
    fx *= 2 * M_PI * boxY_1;
    fy *= 2 * M_PI * boxY_1;
    fz *= boxY_1;

    // here goes rho squared
    double rho2 = y*y + z2;
    double rho_1 = 1 / sqrt( rho2 );
    double rho_2 = rho_1 * rho_1;

    phi += log( rho2 * boxX_1 * boxX_1 * 1./4. * divfact );
    fy += 2 * SinUqY * ExpUqZ * divfact;
    // the (y/rho2) term in fy comes later

    //----
    fy *= boxY_1;

    //-------------------------------------------------------------------------------
    //-- BesselK terms (converge fast)

    // the result, NN, corresponds to the product p*l
    // but usually this is not the largest sum, so the simplest thing to do is to
    // use it as the boundary for both p and l separately, or
    // at least to use something like a romb structure with p+l=NN being the boundary

    int NN = smNK0;
    //NNforSmallZpairwiseK0Sum( boxX, boxY_1, NN );
    //cout << "NNforSmallZpairwise bessel ===> NN = " << NN << endl;

    if( NN > Size1dim )
    {
      BegLogLine( 1 )
        << "WARNING: not enough space in cosp, sinp, ... arrays for bessel summation"
        << EndLogLine ;
        //exit( 1 );
        NN = Size1dim - 1;  // correct to avoid exceeding array size, -1 because below use <= for the limits
    }

    // initialize the rest of the arrays, here initialize only arrays for p, not q - only need them
    for( p = Np + 1; p <= NN; ++p )
    {
        cosp[ p ] = cos( xUp1 * p );
        sinp[ p ] = sin( xUp1 * p );
    }

    for( int l = 1; l <= NN; ++l )
    {
        double lboxY = l * boxY;
        double ylp = y + lboxY;
        double ylm = y - lboxY;
        double rholp_1 = 1/sqrt( ylp*ylp + z2 );
        double rholm_1 = 1/sqrt( ylm*ylm + z2 );
        double rholp = 1 / rholp_1;
        double rholm = 1 / rholm_1;

        for( p = 1; p <= NN - l; ++p )
        {
            double up = Upq[p*Size1dim];
            double uprholp = up * rholp;
            double uprholm = up * rholm;
            double bepm = BesselK0( uprholp ) + BesselK0( uprholm );
            double be1p = BesselK1( uprholp );
            double be1m = BesselK1( uprholm );

            phi += 4 * bepm * cosp[p];
            fx += 4 * p * bepm * sinp[p];
            double commfact = boxX_1 * 4 * p * cosp[p];
            double be1prholp_1 = be1p * rholp_1;
            double be1mrholm_1 = be1m * rholm_1;
            fy += commfact * ( be1prholp_1 * ylp + be1mrholm_1 * ylm );
            //cout << "delta fy for l="<<l<<" p="<<p<<" "<<commfact * ( be1prholp_1 * ylp + be1mrholm_1 * ylm )<<endl;
            fz += commfact * ( be1prholp_1 * z + be1mrholm_1 * z );
            //cout << "delta fz for l="<<l<<" p="<<p<<" "<<commfact * ( be1prholp_1 * z + be1mrholm_1 * z )<<endl;
        }
    }

    // for fx, fy this is from u_p, u_q after differentiation
    fx *= 2 * M_PI;
    fy *= 2 * M_PI;
    fz *= 2 * M_PI;

    //------------------------------------
    // other stand alone terms
    fy -= 2 * y * rho_2;
    fz -= 2 * z * rho_2;

    //cout << "before phi sum " << phi << " " << fx << " " << fy << " " << fz << endl;
    //////////





    //-------------------------------------------------------------------------------
    // Here is a problematic sum of polygamma functions that converge really slowly

    double argp = 1 + x * boxX_1;
    double argm = 1 - x * boxX_1;

    // first - term for n=0

    phi -= Psi( argp ) + Psi( argm );
    fx += Psi( 1, argp ) - Psi( 1, argm );

    // now - the rest of the sum

    double rhoux2 = rho2 * boxX_1 * boxX_1;
    double rhoux2n = 1;
    double twonfact = 1;
    double yrho_2 = y * rho_2;
    double zrho_2 = z * rho_2;

    // these are for the Binomial
    double term = -0.5;
    double factorial = 1;
    double product = 1;

    NN = smNpsi;
    //NNforSmallZpairwisePsiSum( fabs(x), rho_1, boxX, boxX_1, NN ); ///???
    //cout << "NNforSmallZpairwise Psi ===> NN = " << NN << endl;

    for( int n = 1; n <= NN; ++n )
    {
        int two_n = 2 * n;
        rhoux2n *= rhoux2;
        twonfact *= two_n * (two_n - 1);

        // binomial
        factorial *= n;
        product *= term;
        term -= 1;
        double binom = product / factorial;

        double fact = binom * rhoux2n / twonfact;
        double psip = Psi( two_n, argp );
        double psim = Psi( two_n, argm );
        double phiterm = fact * ( psip + psim );
        phi -= phiterm;
        double psip1 = Psi( two_n + 1, argp );
        double psim1 = Psi( two_n + 1, argm );
        double fxterm = fact * ( psip1 - psim1 );
        fx += fxterm;
        double phitermtwo_n = phiterm * two_n;
        double fyterm = phitermtwo_n * yrho_2;
        fy += fyterm;
        double fzterm = phitermtwo_n * zrho_2;
        fz += fzterm;
    }

    // the final touch
    phi *= boxX_1;
    fx *= boxX_1 * boxX_1;
    fy *= boxX_1;
    fz *= boxX_1;

    if( IncludeInvR::INCLUDE_INV_R )
    {
        // the evil sqrt
        double r_1 = 1/sqrt( x*x + y*y + z2 );
        double r_3 = r_1 * r_1 * r_1;
        phi += r_1;
        fx += x * r_3;
        fy += y * r_3;
        fz += z * r_3;
    }

    return status;
}


// rotate everything to have z - the largest coordinate on the value of which it is then decided
// if to use smallZ or largeZ function.
//
// it is clear that the convergence of the series in largeZ is completely determined by z - since
// x and y enter though trigonometric functions.
//

template <class MDVM,class DynVarMgrIF,class RTG>
template <class IncludeInvR>
inline int UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::pairwiseRotated( MDVM &aMDVM, const double chargeAxB )
{
    XYZ vAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB );

    const double x = vAB.mX;//##### = -12.375;
    const double y = vAB.mY;//##### = -3.106;
    const double z = vAB.mZ;//###### = -12.375;

    XYZ dynamBoxDimVect = aMDVM.GetDynamicBoxDimensionVector();
    const double BoxSizeX = dynamBoxDimVect.mX;
    const double BoxSizeY = dynamBoxDimVect.mY;
    const double BoxSizeZ = dynamBoxDimVect.mZ;

    XYZ dynamBoxDimVect_1 = aMDVM.GetDynamicBoxInverseDimensionVector();
    const double BoxSizeX_1 = dynamBoxDimVect_1.mX;
    const double BoxSizeY_1 = dynamBoxDimVect_1.mY;
    const double BoxSizeZ_1 = dynamBoxDimVect_1.mZ;

    //----- rotation
    // first of all, rotate to make z - the largest coord
    // here also make sure that 1st and 2nd coords come up to be xy, xz, or yz to match Uxy, Uzx, Uyz

    const double xyz[3] = { x, y, z };
    int perm[3] = { 0, 1, 2 }; // so x == xyz[0], etc
    {
        double fabsx = fabs(x), fabsy = fabs(y), fabsz = fabs(z);
        if( fabsx >= fabsy && fabsx > fabsz ) // x is the largest
        {
            int perm2 = perm[2];
            perm[2] = perm[0];
            perm[0] = perm[1];
            perm[1] = perm2;
        }
        else if( fabsy >= fabsx && fabsy > fabsz ) // else if y is the largest
        {
            int perm2 = perm[2];
            perm[2] = perm[1];
            perm[1] = perm[0];
            perm[0] = perm2;
        }
    }

    // select Upq
    double *Upq = Uind1ind2[perm[2]];

    // these should be initialized every time we start a new time step
    const double box[3] = { BoxSizeX, BoxSizeY, BoxSizeZ };
    const double box_1[3] = { BoxSizeX_1, BoxSizeY_1, BoxSizeZ_1 };

    const double boxX = box[perm[0]];
    const double boxY = box[perm[1]];
    const double boxZ = box[perm[2]];

    const double boxX_1 = box_1[perm[0]];
    const double boxY_1 = box_1[perm[1]];
    const double boxZ_1 = box_1[perm[2]];

    const double xx = xyz[perm[0]];
    const double yy = xyz[perm[1]];
    const double zz = xyz[perm[2]];


    //---- cost of calculations

    const double fabszz = fabs(zz);
    const double fabsxx = fabs(xx);

    // determine how many terms to keep for a given accuracy using analytic formula

    double dlrgNp, dlrgNq;
    NNforLargeZpairwise( fabszz, boxX, boxY, dlrgNp, dlrgNq );

    double dsmNp, dsmNq;
    NNforSmallZpairwise( fabszz, boxX, boxY, boxZ, dsmNp, dsmNq );

    double dsmNK0;
    NNforSmallZpairwiseK0Sum( boxX, boxY_1, dsmNK0 );

    double dsmNpsi;
    double rho2 = yy*yy + zz*zz;
    NNforSmallZpairwisePsiSum( fabsxx, rho2, boxX, boxX_1, dsmNpsi );

    double costLarge = dlrgNq*dlrgNp*costLargeZpairwise;
    double costSmall = dsmNq*dsmNp*costSmallZpairwise + dsmNK0*dsmNK0/2*costSmallZpairwiseK0Sum + dsmNpsi*costSmallZpairwisePsiSum;

    double fxx, fyy, fzz;

    //cout << "xx=" << xx << " yy=" << yy << " zz=" << zz << endl;
    //cout << "boxX_1=" << boxX_1 << " boxY_1=" << boxY_1 << " boxZ_1=" << boxZ_1 << " boxX=" << boxX << " boxY=" << boxY << " boxZ=" << boxZ << endl;

    //cout << "Upq=" << Upq << " Uxy=" << Uxy << endl;

    double phi; // potential

    int status;

    if(  costSmall < costLarge )
    {
        //cout << "CALLING SMALL" << endl;
        //cout << int(dsmNp) << " " <<  int(dsmNq) << " " <<  int(dsmNK0) << " " << int(dsmNpsi) << endl;
        //for(int c=0; c<5000; c++) //////////////
        //////status = SmallZpairwise<IncludeInvR>( xx, yy, zz, boxX_1, boxY_1, boxZ_1, Upq, boxX, boxY, boxZ, int(dsmNp), int(dsmNq), int(dsmNK0), int(dsmNpsi), phi, fxx, fyy, fzz );
        status = SmallZpairwise<IncludeInvR>( xx, yy, zz, boxX_1, boxY_1, boxZ_1, Upq, boxX, boxY, boxZ, 2*int(dsmNp), 2*int(dsmNq), 2*int(dsmNK0), 2*int(dsmNpsi), phi, fxx, fyy, fzz );
    }
    else
    {
        //cout << "CALLING LARGE" << endl;
        //cout << int(dlrgNp) << " " << int(dlrgNq) << endl;
        //for(int c=0; c<5000; c++) //////////////
        //status = LargeZpairwise<IncludeInvR>( xx, yy, zz, boxX_1, boxY_1, boxZ_1, boxX, boxY, Upq, boxZ, int(dlrgNp), int(dlrgNq), phi, fxx, fyy, fzz );
        status = LargeZpairwise<IncludeInvR>( xx, yy, zz, boxX_1, boxY_1, boxZ_1, boxX, boxY, Upq, boxZ, 2*int(dlrgNp), 2*int(dlrgNq), phi, fxx, fyy, fzz );
    }

    //cout << "PHI="<<phi << " fx=" << fxx << " fy=" << fyy << " fz=" << fzz << endl; ///////

    //----- report the results
    aMDVM.ReportEnergy( phi * chargeAxB );

    //cout << "x= " <<x<<" y= "<<y<<" z= "<<z<< " Qab= " <<chargeAxB<<" E= "<<phi*(57.7422)<<endl;///######

    double fx, fy, fz;
    double *fi[3] = { &fx, &fy, &fz };
    *fi[perm[0]] = fxx;
    *fi[perm[1]] = fyy;
    *fi[perm[2]] = fzz;

    XYZ force;
    force.mX = fx;
    force.mY = fy;
    force.mZ = fz;
    force *= chargeAxB;
    aMDVM.ReportForce( MDVM::SiteA, force );
    aMDVM.ReportForce( MDVM::SiteB, -force );

    return status;
}


// this function give an extimate of the number of terms to use in the LargeZpairwise() function
// it does not have to be absolutely precise, so if there is a fast approximation of the log() expression
// here, that should be used
// BTW, this function gives a very good estimate in the whole range of parameters where I tested it.

template <class MDVM,class DynVarMgrIF,class RTG>
inline void UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::NNforLargeZpairwise( const double fabsz, const double boxX, const double boxY, double& dNx, double& dNy )
{
    double R = -log( fabsz * epsilon ) / ( 2 * M_PI * fabsz );
    dNx = boxX * R + 0.5;
    dNy = boxY * R + 0.5;
}

// this function give an extimate of the number of terms to use in the first sum in SmallZpairwise()
// (similar to the one in LargeZpairwise())
// it does not have to be absolutely precise, so if there is a fast approximation of the log() expression
// here, that should be used
// NOTE: z is small, usually smaller than 0.3, so it can be approximated to be just 0.
// In that case the number of terms can be calculated ones to save the log() calculation
//
// Also a very good estimate of the number of terms needed.


template <class MDVM,class DynVarMgrIF,class RTG>
inline void UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::NNforSmallZpairwise( const double z, const double boxX, const double boxY, const double boxZ, double& dNx, double& dNy )
{
    double R = -log( (boxZ - z) * epsilon ) / ( 2 * M_PI * (boxZ - z) );
    dNx = ceil( boxX * R );
    dNy = ceil( boxY * R );
}

// an estimator for the number of terms in the BesselK0 sum.
// it actually gives an estimate on the product l*p, so ideally only l*p<=NN should be summed.
// the formula in the thesis works well numerically, though I do not understand the origin of the division by 100 inside
// the logarithm there, especially because the derivation of the formula seems to have a mistake

template <class MDVM,class DynVarMgrIF,class RTG>
inline void UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::NNforSmallZpairwiseK0Sum( const double boxX, const double boxY_1, double& dN )
{
    dN = ceil( - log( epsilon * boxX / 100 ) / ( 2 * M_PI ) * boxX * boxY_1 );
}

// an estimator for the number of terms to use in the psi-function summation.
// this estimator is not very accurate and it usually underestimates the number.
// will need to work on it to have a better formula.
// the original f-la had 1/2 not 1/6 - why did i change it ?

template <class MDVM,class DynVarMgrIF,class RTG>
inline void UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::NNforSmallZpairwisePsiSum( const double fabsx, const double rho2, const double boxX, const double boxX_1, double& dN )
{
    double tmp = 1 - boxX_1 * fabsx;
    tmp *= tmp;
    double expr = boxX_1 * boxX_1 * rho2 / tmp;
    if( expr >= 1.0 ) // negative dN sign problem work around
        dN = MAXDOUBLE;
    else
        dN = ceil( 2 * (1/1.6) * log( epsilon * boxX ) / log( expr ) );
}

#endif
