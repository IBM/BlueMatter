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
 
#ifndef _EWALD_HPP_
#define _EWALD_HPP_

#include <BlueMatter/complex.hpp>

class EwaldIF
{
private:

    enum { MAXMEMORY = 50000000u }; // 50 MB

public:

    static double EwaldSelfEnergyPrefactor;

    // indexed by k_ind and particle id
    class tSiteData
    {
    public:

        // static
        int     AbsSiteId;
        double  Charge;

        // dynamic
        Complex Eikx1, Eiky1, Eikz1;
        Complex qEikx, Eikxy;
        XYZ     Force;
    };

    static tSiteData *SiteData;


    // these arrays have KindMax elements, indexed by k_ind
    static double *DampingFactor;
    static XYZ *LambDampK;
    static Complex *ReducedEwaldGlobalStructureFactor;
    // size - KStorageSize
    static Complex *EwaldGlobalStructureFactor;

    static int Kmax, Kmax2;
    static int ContiguousKIndMax;
    static XYZ Lambda; // 2 pi/init_box_size

    static int KStorageSize;
    static int KIndLocalStart, KIndLocalFinish;

    static int NumLocalSites;

    class tEikxyz
    {
    public:
        
        Complex *data;
        inline Complex *operator() ( int aKStorInd ) const { return data + aKStorInd * NumLocalSites; }
    };

    static tEikxyz Eikxyz;

    static inline void Init();  // this is done only once
    static inline void PrecomputeEiks();
    static inline void GetKSpaceEnergy( double& aEnergy, const int ContiguousKIndex );

private:

    static inline void LocalDistributor( const int total, const int containers, const int local_id, int& first, int& size );

    class getDamp
    {
    public:

        static inline void run();

    private:

        static int iKx2, iKxy2, Kind;
        static double Kx2, Kxy2;
        static double inv4alpha2, Volume;

        static inline void dampkz();
        static inline void dampky();
    };

    class initLambDampK
    {
    public:

        static inline void run();

    private:

        static int iKx, iKy, Kind;
        static int iKx2, iKxy2;

        static inline void initkz();
        static inline void initky();
    };
};

EwaldIF::tSiteData *EwaldIF::SiteData;
EwaldIF::tEikxyz EwaldIF::Eikxyz;

XYZ *EwaldIF::LambDampK;
Complex *EwaldIF::EwaldGlobalStructureFactor;
Complex *EwaldIF::ReducedEwaldGlobalStructureFactor;
double *EwaldIF::DampingFactor;
XYZ EwaldIF::Lambda;
int EwaldIF::KStorageSize;
int EwaldIF::KIndLocalStart;
int EwaldIF::KIndLocalFinish;
int EwaldIF::NumLocalSites;
int EwaldIF::Kmax;
int EwaldIF::ContiguousKIndMax;
int EwaldIF::Kmax2;
double EwaldIF::EwaldSelfEnergyPrefactor;
int EwaldIF::initLambDampK::iKx;
int EwaldIF::initLambDampK::iKy;
int EwaldIF::initLambDampK::Kind;
int EwaldIF::initLambDampK::iKx2;
int EwaldIF::initLambDampK::iKxy2;
int EwaldIF::getDamp::iKx2;
int EwaldIF::getDamp::iKxy2;
double EwaldIF::getDamp::Kx2;
int EwaldIF::getDamp::Kind;
double EwaldIF::getDamp::Kxy2;
double EwaldIF::getDamp::inv4alpha2;
double EwaldIF::getDamp::Volume;

inline
void EwaldIF
::LocalDistributor( const int total, const int containers, const int local_id, int& first, int& size )
{
    // creates most-even distribution like this:
    // |**|**|**|**|***|***|
    // |lesser_ids |
    // |   containers      |

    const int lesser_size = total / containers;
    const int lesser_ids = containers * ( lesser_size + 1 ) - total;
    if( local_id < lesser_ids )
    {
        size = lesser_size;
        first = lesser_size * local_id;
    } else {
        size = lesser_size + 1;
        first = lesser_size * lesser_ids + ( lesser_size + 1 ) * ( local_id - lesser_ids );
    }
}



inline
void EwaldIF
::getDamp::dampkz()
{
    // kz = 0
    {
        const int iKxyz2 = iKxy2;
        if( iKxyz2 )
        {
            const double Kxyz2 = Kxy2;
            const double dampingFactor = 4.0 * Math::PI * exp( -Kxyz2*inv4alpha2 ) / ( Kxyz2*Volume );
            DampingFactor[ Kind ] = dampingFactor;
            Kind++;
        }
    }
    
    // kz > 0
    for( int kz = 1; ; kz++ )
    {
        const int iKxyz2 = iKxy2 + kz*kz;
        if( iKxyz2 > Kmax2 ) 
            break;
        
        const double Kz = Lambda.mZ * kz;
        const double Kxyz2 = Kxy2 + Kz*Kz;
        const double dampingFactor = 4.0 * Math::PI * exp( -Kxyz2*inv4alpha2 ) / ( Kxyz2*Volume );
        DampingFactor[ Kind ] = dampingFactor * 2;
        Kind++;
    }
}



inline
void EwaldIF
::getDamp::dampky()
{
    // ky = 0
    iKxy2 = iKx2;
    Kxy2 = Kx2;
    dampkz();
    
    // ky > 0
    for( int ky = 1; ; ky++ )
    {
        iKxy2 = iKx2 + ky*ky;
        if( iKxy2 > Kmax2 )
            break;

        const double Ky = Lambda.mY * ky;
        Kxy2 = Kx2 + Ky*Ky;
        dampkz();
    }

    // ky < 0    
    for( int ky = 1; ; ky++ )
    {
        iKxy2 = iKx2 + ky*ky;
        if( iKxy2 > Kmax2 )
            break;

        const double Ky = Lambda.mY * ky;
        Kxy2 = Kx2 + Ky*Ky;
        dampkz();
    }
}



inline
void EwaldIF
::getDamp::run()
{
    const XYZ& BoxSize = RTG.mBirthBoundingBoxDimensionVector;

    Lambda.mX = 2.0*Math::PI/BoxSize.mX;
    Lambda.mY = 2.0*Math::PI/BoxSize.mY;
    Lambda.mZ = 2.0*Math::PI/BoxSize.mZ;

    Volume = BoxSize.mX * BoxSize.mY * BoxSize.mZ;
    inv4alpha2 = 0.25 / sqr(  MSD_IF::GetEwaldAlpha() );

    //-----------------------------------------------------------------------
    //-- Damping factors are stored in a 1D array so that when used inside
    //-- a 3-loop over kx, ky, kz that each has a range of [-Kmax,Kmax],
    //-- it can be accesses using a cumulative index that is incremented
    //-- every time the next momentum is reached (f.e., every time kz is incremented)

    Kind = 0;

    // kx = 0
    Kx2 = 0;
    iKx2 = 0;
    dampky();

    // kx > 0
    for( int kx = 1; kx <= Kmax; kx++ )
    {
        const double Kx = Lambda.mX * kx;
        Kx2 = Kx*Kx;
        iKx2 = kx*kx;
        dampky();
    }

    // kx < 0
    for( int kx = 1; kx <= Kmax; kx++ )
    {
        const double Kx = Lambda.mX * kx;
        Kx2 = Kx*Kx;
        iKx2 = kx*kx;
        dampky();
    }
}


inline
void EwaldIF
::initLambDampK::initkz()
{
    // iKz = 0
    {
        const int iKxyz2 = iKxy2;
        if( iKxyz2 )
        {
            LambDampK[Kind].mX = iKx * DampingFactor[Kind] * Lambda.mX;
            LambDampK[Kind].mY = iKy * DampingFactor[Kind] * Lambda.mY;
            LambDampK[Kind].mZ = 0;
            Kind++;
        }
    }
    
    for( int iKz = 1; ; iKz++ )
    {
        const int iKxyz2 = iKxy2 + iKz*iKz;
        if( iKxyz2 > Kmax2 )
            break;
        
        LambDampK[Kind].mX = iKx * DampingFactor[Kind] * Lambda.mX;
        LambDampK[Kind].mY = iKy * DampingFactor[Kind] * Lambda.mY;
        LambDampK[Kind].mZ = iKz * DampingFactor[Kind] * Lambda.mZ;
        Kind++;
    }
}




inline
void EwaldIF
::initLambDampK::initky()
{
    iKy = 0;
    iKxy2 = iKx2;
    initkz();
    
    for( iKy = 1; ; iKy++ )
    {
        iKxy2 = iKx2 + iKy*iKy;
        if( iKxy2 > Kmax2 )
            break;
        initkz();
    }
    
    for( iKy = -1; ; iKy-- )
    {
        iKxy2 = iKx2 + iKy*iKy;
        if( iKxy2 > Kmax2 )
            break;
        initkz();
    }
}


inline
void EwaldIF
::initLambDampK::run()
{
    Kind = 0;

    // kx = 0
    iKx = 0;
    iKx2 = 0;
    initky();
    
    // kx > 0
    for( iKx = 1; iKx <= Kmax; iKx++ )
    {
        iKx2 = iKx*iKx;
        initky();
    }
    
    // kx < 0
    for( iKx = -1; iKx >= -Kmax; iKx-- )
    {
        iKx2 = iKx*iKx;
        initky();
    }
}




inline
void EwaldIF
::Init()
{
    //------------------------------------------------------------------------------------
    // this part is to prepare storage of damping factors
    // keep all eight octants in memory to have easy assess to them in the main loop

    Kmax =  MSD_IF::GetEwaldKmax();
    Kmax2 = Kmax * Kmax;

    // find ContiguousKIndMax; order is not important - just counting

    ContiguousKIndMax = 0;
    for( int kx = -Kmax; kx <= Kmax; kx++ )
    {
        const int iKx2 = kx*kx;
        for ( int ky = -Kmax; ky <= Kmax; ky++ )
        {
            const int iKxy2 = iKx2 + ky*ky;
            for ( int kz = 0; kz <= Kmax; kz++ )
            {
                const int iKxyz2 = iKxy2 + kz*kz;
                if( iKxyz2 && iKxyz2 <= Kmax2 ) // the sphere condition
                    ContiguousKIndMax++;
            }
        }
    }

    // DampingFactor - allocate, initialize
    {
        pkNew( &DampingFactor, ContiguousKIndMax );
        assert( DampingFactor != NULL );
        
        getDamp::run();
    }

    // need these for load balance assignments in k-space and real space
    const int addrnum = Platform::Topology::GetAddressSpaceCount();
    const int addrid = Platform::Topology::GetAddressSpaceId();

    
    // load assignment in k-space
    {
        const int k_total = ContiguousKIndMax;
        int k_locsize;
        LocalDistributor( k_total, addrnum, addrid, KIndLocalStart, k_locsize );
        KIndLocalFinish = KIndLocalStart + k_locsize;
    }

    // load assignment in real space
    {
        // select which sites to make local
        
        const int numberOfSites = DynVarMgrIF.GetNumberOfSites();
        int loc_start;
        LocalDistributor( numberOfSites, addrnum, addrid, loc_start, NumLocalSites );

        // allocate memory
        pkNew( &SiteData, NumLocalSites );
        assert( SiteData != NULL );

        // gather static info about local sites
        for( int ContiguosSiteIndex = 0; ContiguosSiteIndex < NumLocalSites; ContiguosSiteIndex++ )
        {
            const AbsSiteId = ContiguosSiteIndex + loc_start;
            SiteData[ ContiguosSiteIndex ].AbsSiteId = AbsSiteId;
            SiteData[ ContiguosSiteIndex ].Charge = MSD_IF::GetSiteInfo( AbsSiteId ).charge;
        }
    }

    // selfenergy prefactor
    {
        EwaldSelfEnergyPrefactor = 0.;

        for( int ContiguosSiteIndex = 0; ContiguosSiteIndex < NumLocalSites; ContiguosSiteIndex++ )
        {
            const double charge = SiteData[ ContiguosSiteIndex ].Charge;
            EwaldSelfEnergyPrefactor -= charge * charge;
        }
        
        EwaldSelfEnergyPrefactor *= Math::InvSqrtPI * MSD_IF::GetEwaldAlpha();
    }

    //---------- allocate memory ----------------
    {
        // allocate and initialize LambDampK[]
        pkNew( &LambDampK, ContiguousKIndMax * sizeof( LambDampK[0] ) / sizeof( double ) );
        assert( LambDampK != NULL );
        initLambDampK::run();

        pkNew( &ReducedEwaldGlobalStructureFactor, 2*ContiguousKIndMax );
        assert( ReducedEwaldGlobalStructureFactor != NULL );

        // set allocation size for Eikxyz and EwaldGlobalStructureFactor
        // maximum size is ContiguousKIndMax
        {
            const int numberOfSites = DynVarMgrIF.GetNumberOfSites();
            const int max_local_sites = ( numberOfSites + addrnum - 1 ) / addrnum;
            const int mem_per_k_ind = max_local_sites * sizeof( Eikxyz.data[0] );
            KStorageSize = MAXMEMORY / mem_per_k_ind;
            KStorageSize = min( ContiguousKIndMax, KStorageSize );
        }

#ifdef PK_PARALLEL
        pkNew( &EwaldGlobalStructureFactor, 2*KStorageSize );
        assert( EwaldGlobalStructureFactor != NULL );
#else
        EwaldGlobalStructureFactor = ReducedEwaldGlobalStructureFactor;
#endif
        
        // storage for Eikxyz's

        pkNew( &Eikxyz.data, 2 * NumLocalSites * KStorageSize );
        assert( Eikxyz.data != NULL );
    }
}




inline
void EwaldIF
::PrecomputeEiks()
{
    const double scaledLambdaX = Lambda.mX * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
    const double scaledLambdaY = Lambda.mY * DynVarMgrIF.mBirthBoxAbsoluteRatio.mY;
    const double scaledLambdaZ = Lambda.mZ * DynVarMgrIF.mBirthBoxAbsoluteRatio.mZ;

    for( int ContiguousSiteIndex = 0; ContiguousSiteIndex < NumLocalSites; ContiguousSiteIndex++ )
    {
        const AbsSiteId = SiteData[ ContiguousSiteIndex ].AbsSiteId;
        const XYZ SitePosition = DynVarMgrIF.GetPosition( AbsSiteId );

        const double argx =  SitePosition.mX * scaledLambdaX;
        SiteData[ ContiguousSiteIndex ].Eikx1 = Complex( cos( argx ), -sin( argx ) );

        const double argy =  SitePosition.mY * scaledLambdaY;
        SiteData[ ContiguousSiteIndex ].Eiky1 = Complex( cos( argy ), -sin( argy ) );

        const double argz =  SitePosition.mZ * scaledLambdaZ;
        SiteData[ ContiguousSiteIndex ].Eikz1 = Complex( cos( argz ), -sin( argz ) );
    }
}




inline
void EwaldIF
::GetKSpaceEnergy( double& aEnergy, const int ContiguousKIndex )
{
    const double scaledDampingFactor = DampingFactor[ ContiguousKIndex ] * DynVarMgrIF.mBirthBoxAbsoluteRatio.mX;
    const double gsre = ReducedEwaldGlobalStructureFactor[ ContiguousKIndex ].re;
    const double gsim = ReducedEwaldGlobalStructureFactor[ ContiguousKIndex ].im;
    const double Energy_kind = 0.5 * scaledDampingFactor * ( gsre*gsre + gsim*gsim );

    aEnergy += Energy_kind;
}


#endif
