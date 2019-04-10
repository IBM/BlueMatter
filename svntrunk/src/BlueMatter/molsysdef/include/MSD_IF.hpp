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
 #ifndef _MSD_IF_HPP_
#define _MSD_IF_HPP_

#if defined(PK_BLADE)


/* Perform a binary search for KEY in BASE which has NMEMB elements
   of SIZE bytes each.  The comparisons are done by (*COMPAR)().  */
extern "C" {
    void *
    bsearch (const void *key, const void *base, unsigned int  nmemb, unsigned int  size,
             int (*compar) (const void *, const void *)) ;
} ; 

#endif
#include <BlueMatter/MDVM_BaseIF.hpp>
#include <BlueMatter/UDF_Binding.hpp>

#ifndef PKFXLOG_IRREDUCIBLEFRAGMENT
#define PKFXLOG_IRREDUCIBLEFRAGMENT  (0)
#endif

#if PK_GCC // this is big trouble if this file really needs to include the MSD completely
class MSD;
class SiteInfo;
class UdfInvocation;
class NsqUdfInvocation;
#endif

class MSD_IF  ///_BASE
  {
  private:
  static SiteInfo* expandedSiteInfoTable;

  // NsqInvocationTableIndex to Nsq1Table Index mapping
  static int*      nsqIndexToNsq1IndexMap;

  // A table of pointers to 2 dimensional tables of Nsq1 params
  static void**     nsq1Tables;

  struct UdfInUse
    {
    unsigned useBit : 1;
    char     udfDriver;
    };

  static void**    listBasedCombinedParams;
  static int       listBasedCombinedParamsSize;

  public:
  enum { LISTBASEDDRIVER=0, NSQDRIVER = 1 };
  static UdfInUse UdfsInUse[ UDF_Binding::UDF_MAX_CODE + 1 ];
  static int      udfCodeToIndexMap[ UDF_Binding::UDF_MAX_CODE + 1 ];
  static int workOrderSize;

  static
  inline
  void
  CheckMagicNumber()
    {
    int magicNumber = 0xFACEB0B5;
    if( MSD::magicNumber != magicNumber )
      PLATFORM_ABORT("ERROR:: Magic numbers do not match");
    //// SingletonPtr =  &MolecularSystemDef ;
    }

  /**********************************************************************
   * Here lie the MSD field accessors
   **********************************************************************/

  static
  inline
  double
  GetGuardZoneWidth()
    {
    // This is meant to return the width of the cutoff guard zone.
#if MSDDEF_DoPressureControl
    return( 0.0 );
#else
    return( 0.5 );
#endif
    }

#ifdef MSDDEF_DoSimpleRespa // YUZH: should probably be removed once MSD has SimpleRespaRatio
  static
  inline
  int
  GetSimpleRespaRatio()
    {
    return MSD::SimpleRespaRatio;
    }
#endif

  static
  inline
  int
  GetVelocityResampleRandomSeed()
    {
    return MSD::VelocityResampleRandomSeed;
    }

#if MSDDEF_ReplicaExchange
  static
  inline
  int
  GetSwapPeriodOTS()
    {
    return MSD::SwapPeriodOTS;
    }

  static
  inline
  int
  GetReplicaExchangeRandomSeed()
    {
    return MSD::ReplicaExchangeRandomSeed;
    }

  static
  inline
  int
  GetNumOfTemperatureAttempts()
    {
    return MSD::NumOfTemperatureAttempts;
    }
#endif

  static
  inline
  BoundingBox
  GetBoundingBox()
    {
    return MSD::BoundingBoxDef;
    }

  static
  inline
  int
  GetDoOuter()
    {
    return MSD::DoOuter;
    }

  static
  inline
  int*
  GetSimTicksAtRespaLevel()
    {
    return (int *) MSD::SimTicksAtRespaLevel;
    }

  static
  inline
  double
  GetNoseHooverMassFactor()
    {
    return MSD::NoseHooverMassFactor;  
    }

  static
  inline
  int
  GetNumberOfNHRespaLevels()
    {
    return MSD::NumberOfNoseHooverRespaLevels;  
    }

  static
  inline
  int
  GetNumberOfRespaLevels()
    {
    return MSD::NumberOfRespaLevels;
    }

  static
  inline
  double
  GetOuterTimeStepInPicoSeconds()
    {
    return MSD::OuterTimeStepInPicoSeconds;
    }

  static
  inline
  double
  GetInnerTimeStepInPicoSeconds()
    {
    return MSD::InnerTimeStepInPicoSeconds;
    }

  static
  inline
  double
  GetTimeConversionFactorMsd()
    {
    return MSD::TimeConversionFactorMsd;
    }

  static
  inline
  double
  GetSwitchLowerCutoffForShell( const int aShellId )
    {
    return MSD::SwitchLowerCutoffForShell[ aShellId ];
    }

  static
  inline
  double
  GetSwitchDeltaForShell( const int aShellId )
    {
    return MSD::SwitchDeltaForShell[ aShellId ];
    }

  static
  inline
  int
  GetRespaLevelOfFirstShell()
  {
  return MSD::RespaLevelOfFirstShell;
  }

static
inline
int
GetNumberOfEnergyReportingUDFs()
  {
  return MSD::NumberOfEnergyReportingUDFs;
  }

static
inline
int
GetShakeMaxIterations()
  {
  return MSD::ShakeMaxIterations;
  }

static
inline
int
GetRattleMaxIterations()
  {
  return MSD::RattleMaxIterations;
  }

static
inline
double
GetShakeTolerance()
  {
  return MSD::ShakeTolerance;
  }

static
inline
double
GetRattleTolerance()
  {
  return MSD::RattleTolerance;
  }

  static
  inline
  int
  GetNumberOfConstraints()
  {
  return MSD::NumberOfConstraints;
  }

static
inline
int
GetNumberOfInnerTimeSteps()
  {
  return MSD::NumberOfInnerTimeSteps;
  }

static
inline
int
GetNumberOfOuterTimeSteps()
  {
  return MSD::NumberOfOuterTimeSteps;
  }

static
inline
int
GetSourceId()
  {
  return MSD::sourceId;
  }

static
inline
int
DoNonBondedAtRespaLevel( int aRespaLevel )
  {
  assert( aRespaLevel >= 0 && aRespaLevel < NUMBER_OF_RESPA_LEVELS );
  return MSD::DoNonBondedAtRespaLevel[ aRespaLevel ];
  }

static
inline
int
DoKSpaceAtRespaLevel( int aRespaLevel )
  {
  assert( aRespaLevel >= 0 && aRespaLevel < NUMBER_OF_RESPA_LEVELS );
  return MSD::DoKSpaceAtRespaLevel[ aRespaLevel ];
  }

static
inline
double
GetPressureControlTarget()
  {
  return MSD::PressureControlTarget;
  }

static
inline
double
GetPressureControlPistonInitialVelocity()
  {
  return MSD::PressureControlPistonInitialVelocity;
  }

static
inline
double
GetPressureControlPistonMass()
  {
  return MSD::PressureControlPistonMass;
  }

static
inline
int
GetNumberOfSimTicks()
  {
  return MSD::GetNumberOfSimTicks();
  }

static
inline
double
GetVelocityResampleTargetTemperature()
  {
  return MSD::VelocityResampleTargetTemperature;
  }

static
inline
int
GetSnapshotPeriodInOTS()
  {
  return MSD::SnapshotPeriodInOTS;
  }

static
inline
int
GetVelocityResamplePeriodInOTS()
  {
  return MSD::VelocityResamplePeriodInOTS;
  }

static
inline
int*
GetRespaLevelByUDFCode()
  {
  return (int *)MSD::RespaLevelByUDFCode;
  }

static
inline
double
GetEwaldAlpha()
  {
  return MSD::EwaldAlpha;
  }

static
inline
double
GetConvertedInnerTimeStep()
  {
  return MSD::ConvertedInnerTimeStep;
  }

static
inline
double
GetLeknerEpsilon()
  {
  return MSD::LeknerEpsilon;
  }

static
inline
double
GetExernalPressure()
  {
  return MSD::ExternalPressure;
  }

static
inline
int
GetEmitEnergyTimeStepModulo()
  {
  return MSD::EmitEnergyTimeStepModulo;
  }

static
inline
int
GetEwaldKmax()
  {
  return MSD::EwaldKmax;
  }

static
inline
int
GetP3MEchargeAssignment()
  {
  return MSD::P3MEchargeAssignment;
  }

static
inline
int
GetP3MEdiffOrder()
  {
  return MSD::P3MEdiffOrder;
  }

static
inline
int
GetSimTicksPerTimeStep()
  {
  return MSD::SimTicksPerTimeStep;
  }

static
inline
double*
GetIntegratorLevelConvertedTimeDelta()
  {
  return (double *) MSD::IntegratorLevelConvertedTimeDelta;
  }

/**********************************************************************/

static
inline
int
GetWorkOrderSize()
  {
  return workOrderSize;
  }

static
inline
int
GetNsqInvocationTableSize()
  {
  return MSD::NsqInvocationTableSize;
  }

static
inline
int
GetSiteTypeCount()
  {
  return MSD::SiteIDtoSiteTypeMapSize;
  }

  static
  inline
  int
  GetNsqTypesCount()
  {
  return MSD::NumberOfNsqSiteTypes;
  }

static
inline
int
GetNsqTypeForIndex( const int index )
  {
  assert (index >= 0 && index <= GetNsqInvocationTableSize());
  return MSD::NsqInvocationTable[ index ].nsqType;
  }

static
inline
int
GetNsqCombinationCodeForIndex( const int index )
  {
  assert (index >= 0 && index <= GetNsqInvocationTableSize());
  return MSD::NsqInvocationTable[ index ].combinationCode;
  }

static
inline
int
GetUdfCodeForIndex( const int index )
  {
  assert (index >= 0 && index <= GetNsqInvocationTableSize());
  return MSD::NsqInvocationTable[ index ].udfCode;
  }

static
inline
int
GetNsqSiteTypeForSiteId( const int siteId )
  {
  return MSD::NsqSiteIDtoSiteTypeMap[ siteId ];
  }

private:

static
inline
int
GetNSQ1UdfCount()
  {
  int count = 0;
  for (int i=0; i < GetNsqInvocationTableSize(); i++)
    {
    if ( GetNsqTypeForIndex( i ) == MSD::NSQ1 )
      count++;
    }
  return count;
  }

public:

  static
  inline
  void
  Init()
  {
  expandedSiteInfoTable = NULL;
  nsqIndexToNsq1IndexMap = NULL;
  nsq1Tables = NULL;
  }

static
inline
void *
GetNSQ2ParamForIndexAtSiteId(const int index, const SiteId aAbsSiteId)
  {
  assert (index >= 0 && index <= GetNsqInvocationTableSize());

  int siteType = GetNsqSiteTypeForSiteId( aAbsSiteId );
  int udfCode  = GetUdfCodeForIndex ( index );
  int paramSize = UDF_Binding::GetParamSizeByUDFCode( udfCode );

  char* paramArrayAddr = (char *) MSD::NsqInvocationTable[ index ].paramTupleList;

  return (void *) (paramArrayAddr + paramSize*siteType);
  }

static
inline
void *
GetNSQ1ParamForIndexAtSiteIds(const int index, const SiteId aAbsSiteId1, const SiteId aAbsSiteId2)
  {
  int nsq1Index = nsqIndexToNsq1IndexMap[ index ];

  int udfCode  = GetUdfCodeForIndex ( index );

  int paramSize = UDF_Binding::GetParamSizeByUDFCode( udfCode );

  int numberOfNsqTypes = GetNsqTypesCount();

  int siteType1 = GetNsqSiteTypeForSiteId( aAbsSiteId1 );

  int siteType2 = GetNsqSiteTypeForSiteId( aAbsSiteId2 );

  // NOTE: For now this is implemented as a full T**2 table, in the future it
  // might be worth to keep it a 1/2*T**2 table

  char* paramTableAddr = (char *) nsq1Tables[ nsq1Index ];

  // Indexing a 2-dimensional array
  return (void *) ( paramTableAddr + paramSize * (siteType1 * numberOfNsqTypes + siteType2 ) );
  }

static
inline
NsqUdfInvocation*
GetNsqInvocationTable()
  {
  return (NsqUdfInvocation *) MSD::NsqInvocationTable;
  }

static
inline
int
GetNsqSiteTypeMapSize()
  {
  return MSD::SiteIDtoSiteTypeMapSize;
  }

static
inline
int
IsUDFInUse( const int driverFlag, const int udfCode )
  {
  assert(udfCode >= 0 && udfCode <= UDF_Binding::UDF_MAX_CODE);
  int rc = (UdfsInUse[ udfCode ].useBit && (UdfsInUse[ udfCode ].udfDriver == driverFlag));
  return( rc );
  }


static
inline
int
GetNumberOfUDFsInUse()
  {
  int count = 0;
  for( int i=0; i < UDF_Binding::UDF_MAX_CODE + 1; i++ )
    {
    if( UdfsInUse[i].useBit )
      {
      count++;
      }
    }
  return count;
  }

static
inline
SiteInfo
GetSiteInfo( const SiteId aAbsSiteId )
  {
  int index = MSD::SiteIDtoSiteTypeMap[ aAbsSiteId ];
  SiteInfo rc =  MSD::SiteInformation[ index ];
  return( rc );
  }

static
inline
double
GetMass( SiteId aAbsSiteId )
  {
  double rc = GetSiteInfo( aAbsSiteId ).mass;
  return( rc );
  }

static
inline
double
GetHalfInverseMass( SiteId aAbsSiteId )
  {
  double rc = GetSiteInfo( aAbsSiteId ).halfInverseMass;
  return( rc );
  }

static
inline
void
Finalize()
  {
  }

static
inline
int
IsPair14( const SiteId aAbsSiteId1,
          const SiteId aAbsSiteId2 )
  {

  if( aAbsSiteId1 == aAbsSiteId2 )
    {
    return 0;
    }

  SiteTuplePair key;

  if( aAbsSiteId1 < aAbsSiteId2 )
    {
    key.site1 = aAbsSiteId1;
    key.site2 = aAbsSiteId2;
    }
  else
    {
    key.site1 = aAbsSiteId2;
    key.site2 = aAbsSiteId1;
    }

  return (0);
  }

static
inline
int
IsExcluded( const SiteId aAbsSiteId1,
            const SiteId aAbsSiteId2 )
  {

  if( aAbsSiteId1 == aAbsSiteId2 )
    {
    return 1;
    }

  SiteTuplePair key;

  if( aAbsSiteId1 < aAbsSiteId2 )
    {
    key.site1 = aAbsSiteId1;
    key.site2 = aAbsSiteId2;
    }
  else
    {
    key.site1 = aAbsSiteId2;
    key.site2 = aAbsSiteId1;
    }

  return (1);
  }


static
inline
int
AreSitesInExcIncList( unsigned aAbsSiteId1, unsigned aAbsSiteId2 )
  {
  SiteTuplePair key;
      
  if( aAbsSiteId1 < aAbsSiteId2 )
    {
    key.site1 = aAbsSiteId1;
    key.site2 = aAbsSiteId2;
    }
  else
    {
    key.site1 = aAbsSiteId2;
    key.site2 = aAbsSiteId1;
    }
      
  SiteTuplePair* result = (SiteTuplePair *) bsearch(&key,
                                                    MSD::ExclusionList1_2_3_4,
                                                    MSD::ExclusionList1_2_3_4Size,
                                                    sizeof(SiteTuplePair),
                                                    SiteTuplePair::compare);
  
  return ( result != NULL );
  }
    
static
inline
int
AreSitesInExcIncList( const int index, unsigned aAbsSiteId1, unsigned aAbsSiteId2 )
  {

  if( MSD::NsqInvocationTable[ index ].incExcList == NULL )
    {
    return 0;
    }

  SiteTuplePair key;

  if( aAbsSiteId1 < aAbsSiteId2 )
    {
    key.site1 = aAbsSiteId1;
    key.site2 = aAbsSiteId2;
    }
  else
    {
    key.site1 = aAbsSiteId2;
    key.site2 = aAbsSiteId1;
    }

  SiteTuplePair* result = (SiteTuplePair *) bsearch(&key,
                                                    MSD::NsqInvocationTable[ index ].incExcList,
                                                    MSD::NsqInvocationTable[ index ].size,
                                                    sizeof(SiteTuplePair),
                                                    SiteTuplePair::compare);

  return ( result != NULL );
  }


  // Check if an Site in A is excluded from non-bond calculations a site in B
  // Pick the appropriate functional form for the non-bond interaction
  static
  inline
  unsigned
  GetInteractionCodeMaskNsq( unsigned aAbsoluteSiteIdA,
                             unsigned aAbsoluteSiteIdB )
  {
  unsigned Mask = 0u;

  int nsqInvocationTableSize = MSD_IF::GetNsqInvocationTableSize();
  assert(nsqInvocationTableSize <= 8); // For now we can only support up to 8 nsq udf codes

  unsigned tempBit = 0u;
  for (int i=0; i < nsqInvocationTableSize; i++)
    {
    if( AreSitesInExcIncList(i, aAbsoluteSiteIdA, aAbsoluteSiteIdB) )
      {
      // If the pair is in the exclusion siteTupleList then
      tempBit = MSD::NsqInvocationTable[i].incExcType;
      }
    else
      {
      // If incExcType is MSD::Exclusion (i.e. 0) then we include
      // the sites if they are not in the SiteTupleList
      // If incExcType is MSD::Inclusion (i.e. 1) and the pair is not in the
      // siteTupleList then we do not include the sites

      tempBit = !(MSD::NsqInvocationTable[i].incExcType);
      }

    tempBit = tempBit << i;
    Mask |= tempBit;

    }

  return( Mask );
  }

static
inline
int
GetNumberOfSites ()
  {
  int rc = MSD::SiteIDtoSiteTypeMapSize;
  return ( rc );
  }

static
inline
int
GetIrreducibleFragmentCount()
  {
  // This assert makes sure that we don't have multiple entries per partition which is not yet handled.
  assert( MSD::IrreduciblePartitionIndexToTypeMapSize == MSD::IrreduciblePartitionCount );
  int rc = MSD::IrreduciblePartitionCount;
  return( rc );
  }


static
inline
int
GetFragmentTypeId( FragId aIrreducibleFragmentNumber )
  {
  BegLogLine( PKFXLOG_IRREDUCIBLEFRAGMENT )
    << " GetFragmentTypeId() "
    << " aIrreducibleFragmentNumber=" << aIrreducibleFragmentNumber
    << EndLogLine;

  assert( aIrreducibleFragmentNumber < MSD::IrreduciblePartitionIndexToTypeMapSize );
  // This assert makes sure that we don't have multiple entries per partition which is not yet handled.
  assert( MSD::IrreduciblePartitionIndexToTypeMapSize == MSD::IrreduciblePartitionCount );

  int rc = MSD::IrreduciblePartitionIndexToTypeMap[ aIrreducibleFragmentNumber ].fragmentType;

  BegLogLine( PKFXLOG_IRREDUCIBLEFRAGMENT )
    << " GetFragmentTypeId() "
    << " fragmentTypeId=" << rc
    << EndLogLine;

  return( rc );
  }

static
inline
int
GetIrreducibleFragmentMemberSiteCount( FragId aIrreducibleFragmentNumber )
  {

  int fragmentType = GetFragmentTypeId( aIrreducibleFragmentNumber );

  int rc = MSD::IrreduciblePartitionTypeList[ fragmentType ].numSites;

  BegLogLine( PKFXLOG_IRREDUCIBLEFRAGMENT )
    << " GetIrreducibleFragmentMemberSiteCount() "
    << " NumberOfSites=" << rc
    << EndLogLine;

  return( rc );
  }

static
inline
int
GetFragmentConstraintCount( FragId aIrreducibleFragmentNumber )
  {

  int fragmentType = GetFragmentTypeId( aIrreducibleFragmentNumber );

  int rc = MSD::IrreduciblePartitionTypeList[ fragmentType ].numConstraints;

  BegLogLine( PKFXLOG_IRREDUCIBLEFRAGMENT )
    << " GetIrreducibleFragmentMemberSiteCount() "
    << " NumberOfSites=" << rc
    << EndLogLine;

  return( rc );
  }



static
inline
int
GetIrreducibleFragmentWaterStatus( FragId aIrreducibleFragmentNumber )
  {

  int fragmentType = GetFragmentTypeId( aIrreducibleFragmentNumber );

  int rc = MSD::IrreduciblePartitionTypeList[ fragmentType ].moleculeType == MSD::WATER ;

  return( rc );
  }

static
inline
int
GetTagAtomIndex( FragId aIrreducibleFragmentNumber )
  {

  int fragmentType = GetFragmentTypeId( aIrreducibleFragmentNumber );

  int rc = MSD::IrreduciblePartitionTypeList[ fragmentType ].tagAtomIndex;
  return( rc );
  }

static
inline
double
GetFragmentExtent( FragId aIrreducibleFragmentNumber )
  {

  int fragmentType = GetFragmentTypeId( aIrreducibleFragmentNumber );

  double rc = MSD::IrreduciblePartitionTypeList[ fragmentType ].extent;
  return( rc );
  }

static
inline
int
GetMoleculeTypeId( FragId aIrreducibleFragmentNumber )
  {

  int fragmentType = GetFragmentTypeId( aIrreducibleFragmentNumber );

  int rc = MSD::IrreduciblePartitionTypeList[ fragmentType ].moleculeType;

  return( rc );
  }

static
inline
int
GetMoleculeId( FragId aIrreducibleFragmentNumber )
  {
  assert( aIrreducibleFragmentNumber < MSD::IrreduciblePartitionIndexToTypeMapSize );
  // This assert makes sure that we don't have multiple entries per partition which is not yet handled.
  assert( MSD::IrreduciblePartitionIndexToTypeMapSize == MSD::IrreduciblePartitionCount );

  int rc = MSD::IrreduciblePartitionIndexToTypeMap[ aIrreducibleFragmentNumber ].moleculeId;

  return( rc );
  }

static
inline
int
GetFragmentCountForMolecule( int aMoleculeId )
  {
  int numberOfFragments = 0;

  for(int i=0; i < GetIrreducibleFragmentCount(); i++)
    {
    int moleculeId = GetMoleculeId( i );
    if(moleculeId == aMoleculeId)
      numberOfFragments++;
    }

  return numberOfFragments;
  }

static
inline
int
GetFragmentIndexInMolecule( FragId aIrreducibleFragmentNumber )
  {
  int moleculeId = GetMoleculeId( aIrreducibleFragmentNumber );
  int counter=-1;

  for(int i=0; i <= aIrreducibleFragmentNumber; i++)
    {
    int moleculeIdSearch = GetMoleculeId( i );
    if( moleculeId == moleculeIdSearch )
      counter++;
    }

  return counter;
  }

static
inline
int
GetFragmentIndexInMoleculeFromMSD( FragId aIrreducibleFragmentNumber )
  {
  assert( aIrreducibleFragmentNumber < MSD::IrreduciblePartitionIndexToTypeMapSize );

  int rc = MSD::IrreduciblePartitionIndexToTypeMap[ aIrreducibleFragmentNumber ].fragmentIndexInMol;
  return ( rc );
  }

static
inline
SiteId
GetIrreducibleFragmentMemberSiteId( FragId aIrreducibleFragmentNumber, int aSiteNumber )
  {

  int fragmentType = GetFragmentTypeId( aIrreducibleFragmentNumber );

  assert(  aSiteNumber < MSD::IrreduciblePartitionTypeList[ fragmentType ].numSites );

  SiteId rc = MSD::IrreduciblePartitionIndexToTypeMap[ aIrreducibleFragmentNumber ].startSiteId + aSiteNumber;
  return( rc );
  }

  static
  inline
  SiteId
  GetIrreducibleFragmentFirstSiteId( FragId aIrreducibleFragmentNumber )
  {

  int fragmentType = GetFragmentTypeId( aIrreducibleFragmentNumber );

  SiteId rc = MSD::IrreduciblePartitionIndexToTypeMap[ aIrreducibleFragmentNumber ].startSiteId ;
  return( rc );
  }

static
inline
SiteId
GetInternalSiteIdForExternalSiteId( SiteId aExternalSiteId )
  {
  assert( aExternalSiteId < MSD::ExternalToInternalSiteIdMapSize );
  SiteId rc = MSD::ExternalToInternalSiteIdMap[ aExternalSiteId ];
  return rc;
  }

static
inline
SiteId
GetExternalSiteIdForInternalSiteId( int aInternalSiteId )
  {
  assert( aInternalSiteId < MSD::InternalToExternalSiteIdMapSize );
  SiteId rc = MSD::InternalToExternalSiteIdMap[ aInternalSiteId ];
  return rc;
  }
};


#if !defined(LEAVE_SYMS_EXTERN)
SiteInfo* MSD_IF::expandedSiteInfoTable = NULL;
int       MSD_IF::udfCodeToIndexMap[ UDF_Binding::UDF_MAX_CODE + 1 ];

int*    MSD_IF::nsqIndexToNsq1IndexMap = NULL;
void**  MSD_IF::nsq1Tables = NULL;

MSD_IF::UdfInUse MSD_IF::UdfsInUse[ UDF_Binding::UDF_MAX_CODE + 1 ];

void**   MSD_IF::listBasedCombinedParams = NULL;
int      MSD_IF::listBasedCombinedParamsSize = 0;
#endif

#endif
