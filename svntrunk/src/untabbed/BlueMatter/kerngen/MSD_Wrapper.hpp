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
 // This file is meant to contain code that modifys how the
// msd.hpp file information is seen by the core.
#ifndef __MSD_WRAPPER_HPP__
#define __MSD_WRAPPER_HPP__

//#include <BlueMatter/msd.hpp>
//#include "msd.hpp"
////#define MSD_UdfInvocationTableSize 7

#include <BlueMatter/TupleDriverUdfBinder.hpp>

#if 0 // example invocation table from charmm enkp

const UdfInvocation MSD::UdfInvocationTable[] =
{
{ 7 , 9 , (int*) ImproperDihedralForce_sitesSiteTupleList, (void*) ImproperDihedralForce_paramsParamTupleList },
{ 2 , 131 , (int*) StdHarmonicAngleForce_sitesSiteTupleList, (void*) StdHarmonicAngleForce_paramsParamTupleList },
{ 1 , 76 , (int*) StdHarmonicBondForce_sitesSiteTupleList, (void*) StdHarmonicBondForce_paramsParamTupleList },
{ 3 , 144 , (int*) SwopeTorsionForce_sitesSiteTupleList, (void*) SwopeTorsionForce_paramsParamTupleList },
{ 4 , 54 , (int*) UreyBradleyForce_sitesSiteTupleList, (void*) UreyBradleyForce_paramsParamTupleList },
{ 36 , 177 , (int*)Pair14SiteList , (void*)Charge14PairTable },
{ 8 , 177 , (int*)Pair14SiteList , (void*)LJ14PairTable }
};

#endif

// This is a horrible way to turn on/off the bonded force driver along with all the dcls not then to be found in the msd.
#if MSD_UdfInvocationTableSize > 0
#define UDF_StdHarmonicBondForce_Enabled  1
#define UDF_StdHarmonicAngleForce_Enabled  1
#define UDF_UreyBradleyForce_Enabled  1
#define UDF_SwopeTorsionForce_Enabled  1
#define UDF_ImproperDihedralForce_Enabled  1
#define UDF_Coulomb14_Enabled 1
#define UDF_LennardJones14Force_Enabled 1
#endif


class ParamAccessor_Base
  {
  public:
    static void Init(){}
  };

template< int UdfInvocationInstance >
class ParamAccessor{};

#ifdef UDF_StdHarmonicBondForce_Enabled
template<>
class ParamAccessor<1> : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::StdHarmonicBondForce_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::StdHarmonicBondForce_paramsParamTupleList[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_StdHarmonicBondForce,
                sizeof(  MSD::StdHarmonicBondForce_paramsParamTupleList ) / sizeof( MSD::StdHarmonicBondForce_paramsParamTupleList[0]),
                MSD::StdHarmonicBondForce_sitesSiteTupleList,
                ParamAccessor<1> > UDF1_Bound;
#endif

#ifdef  UDF_StdHarmonicAngleForce_Enabled
class AngleParamAccessor : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::StdHarmonicAngleForce_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::StdHarmonicAngleForce_paramsParamTupleList[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_StdHarmonicAngleForce,
                sizeof(  MSD::StdHarmonicAngleForce_paramsParamTupleList ) / sizeof( MSD::StdHarmonicAngleForce_paramsParamTupleList[0]),
                MSD::StdHarmonicAngleForce_sitesSiteTupleList,
                AngleParamAccessor> UDF2_Bound;
#endif

///----------------------------------------------------------------

#ifdef UDF_UreyBradleyForce_Enabled
class UreyBradleyParamAccessor  : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::UreyBradleyForce_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::UreyBradleyForce_paramsParamTupleList[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_UreyBradleyForce,
                sizeof(  MSD::UreyBradleyForce_paramsParamTupleList ) / sizeof( MSD::UreyBradleyForce_paramsParamTupleList[0]),
                MSD::UreyBradleyForce_sitesSiteTupleList,
                UreyBradleyParamAccessor> UreyBradleyUDF_Bound;
#endif


///----------------------------------------------------------------

#ifdef  UDF_ImproperDihedralForce_Enabled
class ImproperDihedralParamAccessor  : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::ImproperDihedralForce_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::ImproperDihedralForce_paramsParamTupleList[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_ImproperDihedralForce,
                sizeof(  MSD::ImproperDihedralForce_paramsParamTupleList ) / sizeof( MSD::ImproperDihedralForce_paramsParamTupleList[0]),
                MSD::ImproperDihedralForce_sitesSiteTupleList,
                ImproperDihedralParamAccessor> ImproperDihedralUDF_Bound;
#endif

///----------------------------------------------------------------

#ifdef  UDF_SwopeTorsionForce_Enabled
class SwopeTorsionParamAccessor  : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::SwopeTorsionForce_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::SwopeTorsionForce_paramsParamTupleList[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_SwopeTorsionForce,
                sizeof(  MSD::SwopeTorsionForce_paramsParamTupleList ) / sizeof( MSD::SwopeTorsionForce_paramsParamTupleList[0]),
                MSD::SwopeTorsionForce_sitesSiteTupleList,
                SwopeTorsionParamAccessor> SwopeTorsionUDF_Bound;

#endif


///----------------------------------------------------------------

#ifdef  UDF_Coulomb14_Enabled
class Coulomb14ParamAccessor  : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::StdChargeForce_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::Charge14PairTable[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_Coulomb14,
                sizeof(  MSD::Charge14PairTable ) / sizeof( MSD::Charge14PairTable[0]),
                MSD::Pair14SiteList,
                Coulomb14ParamAccessor> Coulomb14UDF_Bound;
#endif


#ifdef UDF_LennardJones14Force_Enabled
template<class Combiner>
class LennardJones14ParamAccessor   : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::LennardJonesForce_Params ParamType;

//    static ParamType* combinedParameterArray;

    static
    void
    Init()
    {
      int numberOfSiteTuples =  MSD::Pair14SiteListSize;
      int paramSize          = sizeof(ParamType);

      ParamType* combinedParameterArray = NULL;
      GetParam( -1, &combinedParameterArray );

      for (int i = 0; i < numberOfSiteTuples; i++)
        {

          int site1 = MSD::Pair14SiteList[ 2*i ];
          int site2 = MSD::Pair14SiteList[ 2*i + 1 ];

          int siteType1 = MSD::SiteIDtoSiteTypeMap[ site1 ];
          int siteType2 = MSD::SiteIDtoSiteTypeMap[ site2 ];
          Combiner::Combine<ParamType>( MSD::LJ14PairTable[ siteType1 ],  MSD::LJ14PairTable[ siteType2 ], combinedParameterArray[ i ] );
        }
    }

    static
    ParamType
    GetParam( int i , ParamType **SecretParameterArrayReturnField = (ParamType **) 0xFFFFFFFF )
      {
      static ParamType CombinedParameterArray[  MSD::Pair14SiteListSize ];
      if( SecretParameterArrayReturnField != (ParamType **)0xFFFFFFFF )
        {
        *SecretParameterArrayReturnField = CombinedParameterArray;
        return( CombinedParameterArray[ 0 ] );
        }
      return( CombinedParameterArray[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_LennardJonesForce,
                MSD::Pair14SiteListSize,
                MSD::Pair14SiteList,
                LennardJones14ParamAccessor<LennardJonesCHARMMCombiner > > LennardJones14UDF_Bound;

#endif



///----------------------------------------------------------------

class  NSQCoulomb_ParamsAccessor  : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::NSQCoulomb_Params ParamType;

    static
    ParamType
    GetParam( int aAbsSiteId )
      {
      // contain the map to compressed parameter table in this method
      int ParamSiteType =  MSD::NsqSiteIDtoSiteTypeMap[ aAbsSiteId ];
      // could assert array bounds here - even compile time assert!
      return( MSD::ChargeNsqParams[ ParamSiteType ] );
      }
  };

///typedef Binder< UDF_LennardJonesForce,
///                sizeof(  MSD::LJ14PairTable ) / sizeof( MSD::LJ14PairTable[0]),
///                MSD::Pair14SiteList,
///                LennardJonesParamAccessor> LennardJonesUDF_Bound;




///----------------------------------------------------------------

class LennardJonesParamAccessor  : public ParamAccessor_Base
  {
  public:

    typedef  UDF_Binding::NSQLennardJones_Params ParamType;

    static
    ParamType
    GetParam( int aAbsSiteId )
      {
      int ParamSiteType =  MSD::NsqSiteIDtoSiteTypeMap[ aAbsSiteId ];
      return(  MSD::LJNsqParams[ ParamSiteType ] );
      }
  };

class NSQEwaldParamAccessor : public ParamAccessor_Base 
  {
  public:

    typedef  UDF_Binding::NSQEwaldRealSpace_Params ParamType;
  
    static
    ParamType
    GetParam( int aAbsSiteId )
      {
      int ParamSiteType =  MSD::NsqSiteIDtoSiteTypeMap[ aAbsSiteId ];
      return(  MSD::ChargeNsqParams[ ParamSiteType ] );
      }
  };


#include "IFP_Binder.hpp"
typedef IFP_Binder<UDF_NSQLennardJonesForce,  LennardJonesParamAccessor> UDF_LennardJonesForce_Bound;

template<class UDF0_Bound, class UDF1_Bound = NULL_IFP_Binder>
class IrreducibleFragmentPair;

#ifdef MSDDEF_STDCOULOMB
typedef IFP_Binder<UDF_NSQCoulomb,  NSQEwald_ParamsAccessor> UDF_NSQCoulomb_Bound;
typedef UDF_NSQCoulomb_Bound UDF_NSQChargeRealSpace_Bound ;

typedef IrreducibleFragmentPair
        < UDF_NSQCoulomb_Bound,
          UDF_LennardJonesForce_Bound
        > GeneralRealSpaceIFP;
#endif

#ifdef MSDDEF_FRC
typedef IFP_Binder<UDF_NSQCoulomb,  NSQEwald_ParamsAccessor> UDF_NSQCoulomb_Bound;
typedef UDF_NSQCoulomb_Bound UDF_NSQChargeRealSpace_Bound ;

typedef IrreducibleFragmentPair
        < UDF_NSQCoulomb_Bound,
          UDF_LennardJonesForce_Bound
        > GeneralRealSpaceIFP;
#endif

#ifdef MSDDEF_EWALD

typedef IFP_Binder<UDF_NSQEwaldRealSpace,  NSQEwaldParamsAccessor> UDF_NSQEwaldRealSpace_Bound;
typedef IFP_Binder<UDF_NSQEwaldCorrection,  NSQEwaldParamsAccessor> UDF_NSQEwaldCorrection_Bound;

typedef UDF_NSQEwaldRealSpace_Bound UDF_NSQChargeRealSpace_Bound ;

typedef IrreducibleFragmentPair
        < UDF_NSQEwaldRealSpace_Bound,
    UDF_NSQEwaldCorrection_Bound,
          UDF_LennardJonesForce_Bound
        > GeneralRealSpaceIFP;
#endif

#ifndef DoPressureControl
  typedef LocalFragmentDirectDriver
          < UDF_UpdateVelocity,
//////  aMDVM.SetCenterOfMassForce( XYZ::ZERO_VALUE() );
            UDF_ZeroForce
          > MSD_TYPED_LFD_UpdateVelocityFirstHalfStepDriver; // This name is expected by code in main loop
#else
  typedef LocalFragmentDirectDriver
          < UDF_UpdateVelocityFirstHalfStepNPT,
            UDF_ZeroForce
          > MSD_TYPED_LFD_UpdateVelocityFirstHalfStepDriver; // This name is expected by code in main loop

#endif

#ifndef DoPressureControl
  typedef LocalFragmentDirectDriver
          < UDF_UpdateVelocity
          > MSD_TYPED_LFD_UpdateVelocitySecondHalfStepDriver; // This name is expected by code in main loop
#else
  typedef LocalFragmentDirectDriver
          < UDF_UpdateVelocitySecondHalfStepNPT
          > MSD_TYPED_LFD_UpdateVelocitySecondHalfStepDriver; // This name is expected by code in main loop
#endif

typedef LocalFragmentDirectDriver
        < UDF_KineticEnergy
        > MSD_TYPED_LFD_UpdateKineticEnergyDriver;  // This name is expected in the main loop

typedef LocalFragmentDirectDriver
        < UDF_UpdateCenterOfMass
        > MSD_TYPED_LFD_UpdateCenterOfMassDriver;  // This name is expected in the main loop

typedef LocalFragmentDirectDriver
        < UDF_UpdateCenterOfMass,
          UDF_UpdateFragmentKineticEnergy
        > MSD_TYPED_LFD_UpdateCenterOfMassAndFragmentKineticEnergyDriver;  // This name is expected in the main loop

typedef LocalFragmentDirectDriver
        < UDF_UpdateFragmentKineticEnergy
        > MSD_TYPED_LFD_UpdateFragmentKineticEnergyDriver;  // This name is expected in the main loop

#ifndef DoPressureControl
  typedef LocalFragmentDirectDriver
          < UDF_UpdatePosition
          > MSD_TYPED_LFD_UpdatePositionDriver;  // This name is expected in the main loop
#else
  typedef LocalFragmentDirectDriver
          < UDF_UpdatePositionNPT
          > MSD_TYPED_LFD_UpdatePositionDriver;  // This name is expected in the main loop
#endif

typedef SiteTupleDriver
        <
          
        > MSD_TYPED_STD_WaterInit;

#if MSD_UdfInvocationTableSize
typedef SiteTupleDriver
        <  MDVM,        // THIS SHOULDN'T BE PASSED IN BUT RATHER SHOULD BE DECIDED BY THE IMPLEMENTATION OF THE DRIVER
           UDF1_Bound,
           UDF2_Bound,
           SwopeTorsionUDF_Bound,
           UreyBradleyUDF_Bound,
           ImproperDihedralUDF_Bound,
           LennardJones14UDF_Bound,
           Coulomb14UDF_Bound
        > MSD_TYPED_STD_BondedForceDriver;  // This name is expected by code in main loop
#else
typedef SiteTupleDriver
        <  MDVM        // THIS SHOULDN'T BE PASSED IN BUT RATHER SHOULD BE DECIDED BY THE IMPLEMENTATION OF THE DRIVER
        > MSD_TYPED_STD_BondedForceDriver;  // This name is expected by code in main loop
#endif
#endif
#endif
