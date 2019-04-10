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
 #ifndef __MSD_HPP__
#define __MSD_HPP__
#ifndef EXECUTABLEID 
#define EXECUTABLEID -1
#endif 
//File generated on: Mon Mar 24 16:33:54 EST 2003
//%database_name:mdsetup
//%source_id:12676
//%system_id:20922
//%ctp_id:12915
//%probspec_version:v1.0.0
//%probspec_tag:RTP file is comming in from db2
//%magic_number:0xFACEB0B4
#ifndef MSD_COMPILE_DATA_ONLY
#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#endif
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/BoundingBox.hpp>

#ifndef assert
#include <assert.h>
#endif
#ifndef MSD_COMPILE_DATA_ONLY
#include <BlueMatter/RunTimeGlobals.hpp>
#endif
#include <BlueMatter/UDF_Binding.hpp>
#include <BlueMatter/MSD_Prefix.hpp>
#define MSDDEF_DoWaterInit	0

#define MSDDEF_DoRespa	0

#define MSDDEF_DoVelocityResample	0

#define MSDDEF_DoTemperatureControl	0

#define MSDDEF_DoShakeRattleProtein	0

#define MSDDEF_DoPressureControl	0

#define MSDDEF_DoShakeRattleWater	1

#define MSDDEF_DoNVT	0

#define MSDDEF_DoShakeRattle	1

#define MSDDEF_DoFreeEnergy	0

#define MSDDEF_DoNSQCheck	0

#define MSDDEF_STDCOULOMB

#define charge14  1
#define charge14scale  1.0
#define ChargeConversion  SciConst::StdChargeConversion
#define lj14  1
#define lj14scale  1.0
#define TimeConversionFactor  SciConst::StdTimeConversion


class MSD
{
public:

enum { NSQ0 = 0, NSQ1 = 1, NSQ2 = 2};
enum { Exclusion = 0, Inclusion = 1};
enum { WATER = 0, AMINOACID = 1, OTHER=2};
enum { NORM = 0, COMB = 1};

static const int    SimTicksAtRespaLevel[];
static const int    RespaLevelByUDFCode[];
static const int    NumberOfRespaLevels;
static int    NumberOfSimTicks;
static const double ConvertedInnerTimeStep;
static const double IntegratorLevelConvertedTimeDelta[];
static const double SwitchLowerCutoffForShell[];

static const double SwitchDeltaForShell[];

static const int DoNonBondedAtRespaLevel[];

static const int RespaLevelOfFirstShell;
static const double ShakeTolerance;
static const int    ShakeMaxIterations;
static const double RattleTolerance;
static const int    RattleMaxIterations;
static const double EwaldAlpha;
static const double LeknerEpsilon;
static const int    EwaldKmax;
static const int    P3MEchargeAssignment;
static const int    P3MEdiffOrder;
static const double InnerTimeStepInPicoSeconds;
static const int    NumberOfInnerTimeSteps;
static const int       DoOuter;
static const double OuterTimeStepInPicoSeconds;
static int    NumberOfOuterTimeSteps;
static const int    NumberOfConstraints;
static int    SnapshotPeriodInOTS;
static const int    SimTicksPerTimeStep;
static int    EmitEnergyTimeStepModulo;
static const double    ExternalPressure;
static double    PressureControlPistonMass;
static double    PressureControlPistonInitialVelocity;
static int       VelocityResamplePeriodInOTS;
static int       VelocityResampleRandomSeed;
static double    VelocityResampleTargetTemperature;
static double    PressureControlTarget;
static const XYZ    P3MEinitSpacing;
static BoundingBox    BoundingBoxDef;
static const double    Lambda;

static const double TimeConversionFactorMsd;
static UdfInvocation UdfInvocationTable[];
static const int UdfInvocationTableSize;

static const int magicNumber;

static const int sourceId;

static const SiteInfo SiteInformation[];
static const int SiteInformationSize;

static const int SiteIDtoSiteTypeMap[];
static const int SiteIDtoSiteTypeMapSize;

static const UDF_Binding::NSQCoulomb_Params ChargeNsqParams[];
static const UDF_Binding::NSQLennardJones_Params LJNsqParams[];
static const int NumberOfNsqSiteTypes;

static const int NsqSiteIDtoSiteTypeMap[];
static const int NsqSiteIDtoSiteTypeMapSize;

static const int ExclusionList1_2_3_4[];
static const int ExclusionList1_2_3_4Size;

static const NsqUdfInvocation NsqInvocationTable[];
static const int NsqInvocationTableSize;

static const UDF_Binding::LennardJonesForce_Params LJ14PairTable[];
static const int LJ14PairTableSize;

static const UDF_Binding::Coulomb14_Params Charge14PairTable[];
static const int Charge14PairTableSize;

static const int Pair14SiteList[];
static const int Pair14SiteListSize;

static const IrreduciblePartitionType IrreduciblePartitionTypeList[];
static const int IrreduciblePartitionTypeListSize;

static const int IrreduciblePartitionCount;

static const IrreduciblePartitionMap IrreduciblePartitionIndexToTypeMap[];
static const int IrreduciblePartitionIndexToTypeMapSize;

static const int ExternalToInternalSiteIdMap[];
static const int ExternalToInternalSiteIdMapSize;

static const int InternalToExternalSiteIdMap[];
static const int InternalToExternalSiteIdMapSize;

inline static int GetNumberOfSimTicks();

static const int NumberOfEnergyReportingUDFs;

static const int Water3sites[];

};

#ifndef MSD_COMPILE_CODE_ONLY
// All molecules in their own fragment
const IrreduciblePartitionType MSD::IrreduciblePartitionTypeList[] = 
{
/*NONE*/ { 3 ,  MSD::WATER, 0, 1.1 * 0.9572 }
};

const int MSD::IrreduciblePartitionTypeListSize = 1;

const int MSD::IrreduciblePartitionCount = 2;

// {start_site , ip_type, molecule_id, fragments_index_in_molecule} 
const IrreduciblePartitionMap MSD::IrreduciblePartitionIndexToTypeMap[] = 
{
{ 0, 0, 0, 0 },
{ 3, 0, 1, 0 }
};

const int MSD::IrreduciblePartitionIndexToTypeMapSize = 2;

const int MSD::ExternalToInternalSiteIdMap[] = 
{
0,1,2,3,4,5};
const int MSD::ExternalToInternalSiteIdMapSize = 6;

const int MSD::InternalToExternalSiteIdMap[] = 
{
0,1,2,3,4,5};
const int MSD::InternalToExternalSiteIdMapSize = 6;

const double MSD::TimeConversionFactorMsd = TimeConversionFactor;

const double MSD::ShakeTolerance = 1.0E-8;

const double MSD::EwaldAlpha = 0.0;

double MSD::VelocityResampleTargetTemperature = 0.0;

double MSD::PressureControlPistonInitialVelocity = 0.0;

int MSD::NumberOfOuterTimeSteps = 10;

int MSD::VelocityResampleRandomSeed = 51579;

double MSD::PressureControlPistonMass = 0.0004 * 25.0 / ( cbrt((( 12.4 )-(  -12.4 ))*(( 12.4 )-( -12.4 ))*(( 12.4  )-( -12.4 ))) );

double MSD::PressureControlTarget = 1.0;

const double MSD::RattleTolerance = 1.0E-8;

const XYZ MSD::P3MEinitSpacing = {.5,.5,.5};

const double MSD::Lambda = 0.0;

const int MSD::P3MEdiffOrder = 2;

const double MSD::OuterTimeStepInPicoSeconds = 0.001;

int MSD::SnapshotPeriodInOTS = 1;

const int MSD::ShakeMaxIterations = 1000;

const double MSD::LeknerEpsilon = 0.0;

const int MSD::DoOuter = 1;

int MSD::EmitEnergyTimeStepModulo = 1;

int MSD::VelocityResamplePeriodInOTS = 0;

const double MSD::InnerTimeStepInPicoSeconds = -1.0;

const int MSD::EwaldKmax = 0;

const int MSD::P3MEchargeAssignment = 4;

const double MSD::ExternalPressure = 1.0;

BoundingBox MSD::BoundingBoxDef = { -12.4 , -12.4 , -12.4 , 12.4 , 12.4 , 12.4 };

const int MSD::RattleMaxIterations = 1000;

const int MSD::NumberOfInnerTimeSteps = -1;

const int MSD::SimTicksAtRespaLevel[] = {
1};

const int MSD::SimTicksPerTimeStep = 1;

const int MSD::RespaLevelOfFirstShell=0;
const int MSD::RespaLevelByUDFCode[] = {
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0
};
const int MSD::NumberOfRespaLevels = 1;

const int MSD::DoNonBondedAtRespaLevel[] = {
1
};
const double MSD::SwitchLowerCutoffForShell[] = {
9.0
};
const double MSD::SwitchDeltaForShell[] = {
1.0
};
const double MSD::ConvertedInnerTimeStep = 0.0010/MSD::TimeConversionFactorMsd ;

const double MSD::IntegratorLevelConvertedTimeDelta[] = {
MSD::ConvertedInnerTimeStep * 1
};
#endif

inline int MSD::GetNumberOfSimTicks()
{
int simTicks = ( NumberOfOuterTimeSteps * 1 );
return simTicks;
}

#define MSD_GetNumberOfSimTicks_Method
#ifndef MSD_COMPILE_CODE_ONLY

/*
Indexed by site type  = { mass, halfInverseMass, charge, lj_type, lj_14_type };
*/

const SiteInfo MSD::SiteInformation[] = 
{
{ 1.00800 , 1.0/(2.0 * 1.00800) , 0.4170 * ChargeConversion },
{ 15.9994 , 1.0/(2.0 * 15.9994) , -0.8340 * ChargeConversion }
};
const int MSD::SiteInformationSize = 2;

const int MSD::Pair14SiteList[] = 
{ 
-1,-1
};

const int MSD::Pair14SiteListSize = 1;

const UDF_Binding::Coulomb14_Params MSD::Charge14PairTable[] = 
{ 
{ -1 }
};

const int MSD::Charge14PairTableSize = 1;

const UDF_Binding::LennardJonesForce_Params MSD::LJ14PairTable[] = 
{ 
{ -1 , -1 }
};

const int MSD::LJ14PairTableSize = 1;

/*
 UdfCode, size of tupleList, siteTupleListPtr, paramTupleListPtr
*/

UdfInvocation MSD::UdfInvocationTable[] = 
{
{ -1, 0, NULL, NULL, MSD::NORM, -1 }
};
const int MSD::UdfInvocationTableSize = 0;

const UDF_Binding::NSQCoulomb_Params MSD::ChargeNsqParams[] = 
{
{ 0.4170 * ChargeConversion },
{ -0.8340 * ChargeConversion }
};
const int MSD::NumberOfNsqSiteTypes = 2;

const UDF_Binding::NSQLennardJones_Params MSD::LJNsqParams[] = 
{
{ 0.0 , 0.0 * SciConst::SixthRootOfTwo },
{ 0.1521 , 3.1506 * SciConst::SixthRootOfTwo }
};
const int MSD::NsqSiteIDtoSiteTypeMap[] = 
{
1,
0,
0,
1,
0,
0
};
const int MSD::NsqSiteIDtoSiteTypeMapSize = 6;

const int MSD::ExclusionList1_2_3_4[] = 
{
0 , 1,
0 , 2,
1 , 2,
3 , 4,
3 , 5,
4 , 5
};
const int MSD::ExclusionList1_2_3_4Size = 6;

const NsqUdfInvocation MSD::NsqInvocationTable[] = 
{
{ 31, -1, MSD::NSQ2, 6, MSD::Exclusion, (SiteTuplePair *)ExclusionList1_2_3_4, (void *)ChargeNsqParams  } ,
{ 30, 32, MSD::NSQ1, 6, MSD::Exclusion, (SiteTuplePair *)ExclusionList1_2_3_4,  (void *)LJNsqParams } 
};
const int MSD::NsqInvocationTableSize = 2;

const int MSD::SiteIDtoSiteTypeMap[] = 
{
1,
0,
0,
1,
0,
0
};
const int MSD::SiteIDtoSiteTypeMapSize = 6;

const int MSD::Water3sites[] = {
1,0,2,
4,3,5};



const int MSD::NumberOfConstraints =9;

const int MSD::magicNumber = 0xFACEB0B4;

const int MSD::sourceId = 12676;

MSD MolecularSystemDef;

#endif // MSD_COMPILE_CODE_ONLY

// This is the common area, defines go here...
#define MSD_RTP_VelocityResampleTargetTemperature_NONCONST  
#define MSD_RTP_PressureControlPistonInitialVelocity_NONCONST  
#define MSD_RTP_VelocityResampleRandomSeed_NONCONST  
#define PAIR14_SITE_LIST_SIZE 1
#define MSD_RTP_NumberOfOuterTimeSteps_NONCONST  
#define EXCLUSION_LIST1_2_3_4 6
#define MSD_RTP_VelocityResamplePeriodInOTS_NONCONST  
#define MSD_RTP_PressureControlTarget_NONCONST  
#define NUMBER_OF_RESPA_LEVELS 1
#define MSD_RTP_SnapshotPeriodInOTS_NONCONST  
#define NUMBER_OF_FRAGMENTS 2
#define NUMBER_OF_NSQ_SITE_TYPES 2
#define MSD_RTP_BoundingBoxDef_NONCONST  
#define MSD_RTP_PressureControlPistonMass_NONCONST  
#define NUMBER_OF_SITES 6
#define MSD_RTP_EmitEnergyTimeStepModulo_NONCONST  
#define NUMBER_OF_SHELLS 1

#ifndef MSD_COMPILE_DATA_ONLY

extern MSD MolecularSystemDef;

RunTimeGlobals<NUMBER_OF_SHELLS> RTG;

/****************************************************************************
 *  Payload logic 
 * 
 * The confusion is due to our commitment to support both phase 1 and phase 2 
 ****************************************************************************/ 
#include <math.h>
#define EXPECTED_PAYLOAD_SUM ( 0xffffffff >> 1 ) // max unsigned int divided by 2 
#define UDF_PAYLOAD          ( rint( EXPECTED_PAYLOAD_SUM / Platform::Topology::GetAddressSpaceCount() ) )
#define EmitEnergyDistributed( TimeStep, UdfCode, Energy ) \ 
  ED_Emit_UDFd1( TimeStep, UdfCode, UDF_PAYLOAD,  Energy) 
#define EmitEnergyUni(TimeStep, UdfCode, Energy) ED_Emit_UDFd1( TimeStep, UdfCode, EXPECTED_PAYLOAD_SUM,  Energy)
#ifdef PK_MPI_ALL
// If we're in phase 2 then all the energy emits are distributed.
// If we're in phase 1 then only some are distributed. So hence the UniContext distinction.
#ifdef PK_MPI_PHASE2
#define EmitEnergy(TimeStep, UdfCode, Energy, UniContext) EmitEnergyDistributed( TimeStep, UdfCode, Energy )
#else
#define EmitEnergy(TimeStep, UdfCode, Energy, UniContext) \ 
{ \ 
if( UniContext == 1 ) \ 
  EmitEnergyUni( TimeStep, UdfCode, Energy ); \ 
else \ 
  EmitEnergyDistributed( TimeStep, UdfCode, Energy ); \ 
} 
#endif 
#else
#define EmitEnergy(TimeStep, UdfCode, Energy, UniContext) EmitEnergyUni( TimeStep, UdfCode, Energy )
#endif
/**********************************************************************/
#include <BlueMatter/UDF_BindingWrapper.hpp>

#include <BlueMatter/DynamicVariableManager.hpp>

typedef TDynamicVariableManager< NUMBER_OF_SITES,
                                 NUMBER_OF_FRAGMENTS,
                                 NUMBER_OF_RESPA_LEVELS
                               > DynamicVariableManager;
DynamicVariableManager DynVarMgrIF;

#include <BlueMatter/MDVM_IF.hpp>

#include <BlueMatter/LocalTupleListDriver.hpp>

#include <BlueMatter/LocalFragmentDirectDriver.hpp>

// This file is meant to contain code that modifys how the
// msd.hpp file information is seen by the core.

#include <BlueMatter/TupleDriverUdfBinder.hpp>



class ParamAccessor_Base
  {
  public:
    static void Init(){}
  };

template< int UdfInvocationInstance >
class ParamAccessor{};

// This is needed for EwaldCorrection and LeknerCorrection
template<class Combiner>
class Coulomb1_2_3_4ParamAccessor   : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::NSQCoulomb_Params ParamType;

//    static ParamType* combinedParameterArray;

    static
    void
    Init()
    {
      int numberOfSiteTuples =  EXCLUSION_LIST1_2_3_4;
      int paramSize          = sizeof(ParamType);

      ParamType* combinedParameterArray = NULL;
      GetParam( -1, &combinedParameterArray );

      for (int i = 0; i < numberOfSiteTuples; i++)
        {
          int site1 = MSD::ExclusionList1_2_3_4[ 2*i ];
          int site2 = MSD::ExclusionList1_2_3_4[ 2*i + 1 ];

          int siteType1 = MSD::SiteIDtoSiteTypeMap[ site1 ];
          int siteType2 = MSD::SiteIDtoSiteTypeMap[ site2 ];
          
          ParamType charge1;
          ParamType charge2;
          charge1.ChargeAxB = MSD::SiteInformation[ siteType1 ].charge;
          charge2.ChargeAxB = MSD::SiteInformation[ siteType2 ].charge;
          
          Combiner::Combine<ParamType>( charge1 ,  
                                        charge2, 
                                        combinedParameterArray[ i ] );
        }
    }

    static
    ParamType
    GetParam( int i , ParamType **SecretParameterArrayReturnField = (ParamType **) 0xFFFFFFFF )
      {
      static ParamType CombinedParameterArray[ EXCLUSION_LIST1_2_3_4 ];
      if( SecretParameterArrayReturnField != (ParamType **)0xFFFFFFFF )
        {
        *SecretParameterArrayReturnField = CombinedParameterArray;
        return( CombinedParameterArray[ 0 ] );
        }
      return( CombinedParameterArray[ i ] );
      }
  };




class  NSQCoulombParamsAccessor  : public ParamAccessor_Base
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



///----------------------------------------------------------------

class NSQLennardJonesParamAccessor  : public ParamAccessor_Base
  {
  public:

    typedef  UDF_Binding::NSQLennardJones_Params ParamType;

    static
    ParamType
    GetParamByIndex( int aIndex )
    {
    return(  MSD::LJNsqParams[ aIndex ] );
    }


    static
    ParamType
    GetParam( int aAbsSiteId )
      {
      int ParamSiteType =  MSD::NsqSiteIDtoSiteTypeMap[ aAbsSiteId ];
      return(  MSD::LJNsqParams[ ParamSiteType ] );
      }
  };


#include <BlueMatter/IFP_Binder.hpp>
typedef IFP_Binder<UDF_NSQCHARMMLennardJonesForce,  NSQLennardJonesParamAccessor> UDF_LennardJonesForce_Bound;

typedef IFP_Binder<UDF_NSQCoulomb,  NSQCoulombParamsAccessor> UDF_NSQCoulomb_Bound;

typedef UdfGroup
        < UDF_NSQCoulomb_Bound,
          UDF_LennardJonesForce_Bound
        > NsqUdfGroup;

  typedef LocalFragmentDirectDriver
          < UDF_UpdateVelocity,
            UDF_ZeroForce
          > MSD_TYPED_LFD_UpdateVelocityFirstHalfStepDriver; // This name is expected by code in main loop
  typedef LocalFragmentDirectDriver
          < UDF_UpdateVelocity
          > MSD_TYPED_LFD_UpdateVelocitySecondHalfStepDriver; // This name is expected by code in main loop

typedef LocalFragmentDirectDriver
        < UDF_KineticEnergy
        > MSD_TYPED_LFD_UpdateKineticEnergyDriver;  // This name is expected in the main loop
  typedef LocalFragmentDirectDriver
          < UDF_UpdatePosition
          > MSD_TYPED_LFD_UpdatePositionDriver;  // This name is expected in the main loop

typedef SiteTupleDriver
        <  MDVM         > MSD_TYPED_STD_BondedForceDriver;  // This name is expected by code in main loop
typedef TupleDriverUdfBinder <UDF_WaterTIP3PInit,
                              2,
                              MSD::Water3sites,
                              NULL_ParamAccessor > WaterInitUDF_Bound;

typedef TupleDriverUdfBinder <UDF_WaterTIP3PShake,
                              2,
                              MSD::Water3sites,
                              NULL_ParamAccessor > WaterShakeUDF_Bound;

typedef TupleDriverUdfBinder <UDF_WaterTIP3PRattle,
                              2,
                              MSD::Water3sites,
                              NULL_ParamAccessor > WaterRattleUDF_Bound;

#include <BlueMatter/MDVM_UVP_IF.hpp>

typedef SiteTupleDriver
        <  MDVM_UVP
,        WaterInitUDF_Bound
        > MSD_TYPED_STD_GroupInitDriver;  // This name is expected by code in main loop

typedef SiteTupleDriver
        <  MDVM_UVP
,        WaterShakeUDF_Bound
        > MSD_TYPED_STD_GroupShakeDriver;  // This name is expected by code in main loop

typedef SiteTupleDriver
        <  MDVM_UVP
,        WaterRattleUDF_Bound
        > MSD_TYPED_STD_GroupRattleDriver;  // This name is expected by code in main loop

#endif

#ifndef MSD_COMPILE_CODE_ONLY

const int MSD::NumberOfEnergyReportingUDFs = 3;

#endif

#ifndef MSD_COMPILE_DATA_ONLY



#include <BlueMatter/p2.hpp>

#endif
#endif
