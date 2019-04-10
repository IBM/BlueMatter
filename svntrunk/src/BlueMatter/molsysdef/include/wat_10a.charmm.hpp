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
#define charge14  1
#define charge14scale  1.0
#define lj14  1
#define lj14scale  1.0
#define ChargeConversion  SciConst::StdChargeConversion
#define TimeConversionFactor  SciConst::StdTimeConversion


class MSD
{
public:

enum { NSQ0 = 0, NSQ1 = 1, NSQ2 = 2};
enum { Exclusion = 0, Inclusion = 1};
enum { NON_WATER = 0, WATER = 1};
enum { NORM = 0, COMB = 1};
enum { DoFreeEnergy = 0 };
enum { LongRangeForceMethod = PreMSD::STDCOULOMB };
enum { DoNVT = 0 };
enum { DoShakeRattle = 0 };
enum { DoPressureControl = 1 };

static const int    SimTicksAtRespaLevel[];
static const int    RespaLevelByUDFCode[];
static const int    NumberOfRespaLevels;
static const int    RespaLevelNonBonded;
static const double ShakeTolerance;
static const int    NumberOfShakeIterations;
static const double RattleTolerance;
static const int    NumberOfRattleIterations;
static const double EwaldAlpha;
static const int    EwaldKmax;
static const int    P3MEchargeAssignment;
static const int    P3MEdiffOrder;
static const double InnerTimeStepInPicoSeconds;
static const int    NumberOfInnerTimeSteps;
static const int       DoOuter;
static const double OuterTimeStepInPicoSeconds;
static const int    NumberOfOuterTimeSteps;
static const int    NumberOfCOMConstraints;
static const int    SnapshotPeriodInOTS;
static const int    SimTicksPerTimeStep;
static const int    EmitEnergyTimeStepModulo;
static const double    PressureControlPistonMass;
static const double    PressureControlTarget;
static const double    PressureControlPistonInitialVelocity;
static const int       VelocityResamplePeriodInOTS;
static const double    VelocityResampleTargetTemperature;
static const double    SwitchLowerCutoff;
static const double    SwitchDelta;
static const XYZ    P3MEinitSpacing;
static const BoundingBox    BoundingBoxDef;
static const double    Lambda;

static const double TimeConversionFactorMsd;
static UdfInvocation UdfInvocationTable[];
static const int UdfInvocationTableSize;

static const SiteInfo SiteInformation[];
static const int SiteInformationSize;

static const int SiteIDtoSiteTypeMap[];
static const int SiteIDtoSiteTypeMapSize;

static const UDF_Binding::NSQCoulomb_Params ChargeNsqParams[];
static const UDF_Binding::NSQLennardJones_Params LJNsqParams[];
static const int NumberOfNsqSiteTypes;

static const int NsqSiteIDtoSiteTypeMap[];
static const int NsqSiteIDtoSiteTypeMapSize;

static const SiteTuplePair ExclusionList1_2_3_4[];
static const int ExclusionList1_2_3_4Size;

static const NsqUdfInvocation NsqInvocationTable[];
static const int NsqInvocationTableSize;

static const UDF_Binding::LennardJonesForce_Params LJ14PairTable[];
static const int LJ14PairTableSize;

static const UDF_Binding::StdChargeForce_Params Charge14PairTable[];
static const int Charge14PairTableSize;

static const int Pair14SiteList[];
static const int Pair14SiteListSize;

static const Partition PartitionList[];
static const int PartitionListSize;

static const int PartitionCount;

};


const double MSD::ShakeTolerance = 0.0;

const double MSD::EwaldAlpha = 0.0;

const double MSD::VelocityResampleTargetTemperature = 300.0;

const double MSD::PressureControlPistonInitialVelocity = 0.0;

const int MSD::NumberOfOuterTimeSteps = 10;

const double MSD::PressureControlPistonMass = .001;

const int MSD::NumberOfRattleIterations = 0;

const double MSD::PressureControlTarget = .0001;

const XYZ MSD::P3MEinitSpacing = {0,0,0};

const double MSD::RattleTolerance = 0.0;

const double MSD::Lambda = 0.0;

const int MSD::NumberOfShakeIterations = 0;

const int MSD::P3MEdiffOrder = 0;

const double MSD::OuterTimeStepInPicoSeconds = 0.001;

const int MSD::SnapshotPeriodInOTS = 5;

const int MSD::DoOuter = 1;

const int MSD::EmitEnergyTimeStepModulo = 1;

const int MSD::NumberOfCOMConstraints = 3;

const int MSD::VelocityResamplePeriodInOTS = 2;

const double MSD::InnerTimeStepInPicoSeconds = 0.0;

const int MSD::EwaldKmax = 0;

const int MSD::P3MEchargeAssignment = 0;

const BoundingBox MSD::BoundingBoxDef = { -12.0,-12.0,-12.0,12.0,12.0,12.0 };

const double MSD::SwitchLowerCutoff = 1.0E100;

const double MSD::SwitchDelta = 1.0E100;

const int MSD::NumberOfInnerTimeSteps = 0;

const int MSD::SimTicksAtRespaLevel[] = {
1,
2,
4};

const int MSD::SimTicksPerTimeStep = 4;

const int MSD::RespaLevelByUDFCode[] = {
-1,
0,
0,
1,
0,
1,
1,
1,
2,
2,
0,
0,
0,
0,
-1,
-1,
-1,
0,
0,
-1,
0,
2,
2,
2,
2,
2,
0,
0,
0,
0,
2,
2,
-1,
-1,
2,
2,
2,
-1,
-1,
-1,
-1,
-1
};
const int MSD::NumberOfRespaLevels = 3;

const int MSD::RespaLevelNonBonded = 2;


/*
Indexed by site type  = { mass, halfInverseMass, charge, lj_type, lj_14_type };
*/

const SiteInfo MSD::SiteInformation[] = 
{
{ 15.9994 , 1.0/(2.0 * 15.9994) , (-0.834*ChargeConversion) , -1 , 0 },
{ 1.00800 , 1.0/(2.0 * 1.00800) , (0.417*ChargeConversion) , -1 , 1 }
};
const int MSD::SiteInformationSize = 2;

const int MSD::Pair14SiteList[] = 
{ 
-1,-1
};

const int MSD::Pair14SiteListSize = 1;

const UDF_Binding::StdChargeForce_Params MSD::Charge14PairTable[] = 
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
{ (-0.834*ChargeConversion) },
{ (0.417*ChargeConversion) }
};
const int MSD::NumberOfNsqSiteTypes = 2;

const UDF_Binding::NSQLennardJones_Params MSD::LJNsqParams[] = 
{
{ -0.15207259450172 , 1.76824595565275 },
{ 0 , 0 }
};
const int MSD::NsqSiteIDtoSiteTypeMap[] = 
{
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1
};
const int MSD::NsqSiteIDtoSiteTypeMapSize = 99;

const SiteTuplePair MSD::ExclusionList1_2_3_4[] = 
{
{ 0 , 1 },
{ 0 , 2 },
{ 1 , 2 },
{ 3 , 4 },
{ 3 , 5 },
{ 4 , 5 },
{ 6 , 7 },
{ 6 , 8 },
{ 7 , 8 },
{ 9 , 10 },
{ 9 , 11 },
{ 10 , 11 },
{ 12 , 13 },
{ 12 , 14 },
{ 13 , 14 },
{ 15 , 16 },
{ 15 , 17 },
{ 16 , 17 },
{ 18 , 19 },
{ 18 , 20 },
{ 19 , 20 },
{ 21 , 22 },
{ 21 , 23 },
{ 22 , 23 },
{ 24 , 25 },
{ 24 , 26 },
{ 25 , 26 },
{ 27 , 28 },
{ 27 , 29 },
{ 28 , 29 },
{ 30 , 31 },
{ 30 , 32 },
{ 31 , 32 },
{ 33 , 34 },
{ 33 , 35 },
{ 34 , 35 },
{ 36 , 37 },
{ 36 , 38 },
{ 37 , 38 },
{ 39 , 40 },
{ 39 , 41 },
{ 40 , 41 },
{ 42 , 43 },
{ 42 , 44 },
{ 43 , 44 },
{ 45 , 46 },
{ 45 , 47 },
{ 46 , 47 },
{ 48 , 49 },
{ 48 , 50 },
{ 49 , 50 },
{ 51 , 52 },
{ 51 , 53 },
{ 52 , 53 },
{ 54 , 55 },
{ 54 , 56 },
{ 55 , 56 },
{ 57 , 58 },
{ 57 , 59 },
{ 58 , 59 },
{ 60 , 61 },
{ 60 , 62 },
{ 61 , 62 },
{ 63 , 64 },
{ 63 , 65 },
{ 64 , 65 },
{ 66 , 67 },
{ 66 , 68 },
{ 67 , 68 },
{ 69 , 70 },
{ 69 , 71 },
{ 70 , 71 },
{ 72 , 73 },
{ 72 , 74 },
{ 73 , 74 },
{ 75 , 76 },
{ 75 , 77 },
{ 76 , 77 },
{ 78 , 79 },
{ 78 , 80 },
{ 79 , 80 },
{ 81 , 82 },
{ 81 , 83 },
{ 82 , 83 },
{ 84 , 85 },
{ 84 , 86 },
{ 85 , 86 },
{ 87 , 88 },
{ 87 , 89 },
{ 88 , 89 },
{ 90 , 91 },
{ 90 , 92 },
{ 91 , 92 },
{ 93 , 94 },
{ 93 , 95 },
{ 94 , 95 },
{ 96 , 97 },
{ 96 , 98 },
{ 97 , 98 }
};
const int MSD::ExclusionList1_2_3_4Size = 99;

const NsqUdfInvocation MSD::NsqInvocationTable[] = 
{
{ 31, -1, MSD::NSQ2, 99, MSD::Exclusion, (SiteTuplePair *)ExclusionList1_2_3_4, (void *)ChargeNsqParams  } ,
{ 30, 32, MSD::NSQ1, 99, MSD::Exclusion, (SiteTuplePair *)ExclusionList1_2_3_4,  (void *)LJNsqParams } 
};
const int MSD::NsqInvocationTableSize = 2;

const int MSD::SiteIDtoSiteTypeMap[] = 
{
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1,
0,
1,
1
};
const int MSD::SiteIDtoSiteTypeMapSize = 99;

const double MSD::TimeConversionFactorMsd = TimeConversionFactor;
// All molecules in their own fragment
const Partition MSD::PartitionList[] = 
{
{ 0 , 0 , 3 ,  MSD::WATER },
{ 1 , 3 , 3 ,  MSD::WATER },
{ 2 , 6 , 3 ,  MSD::WATER },
{ 3 , 9 , 3 ,  MSD::WATER },
{ 4 , 12 , 3 ,  MSD::WATER },
{ 5 , 15 , 3 ,  MSD::WATER },
{ 6 , 18 , 3 ,  MSD::WATER },
{ 7 , 21 , 3 ,  MSD::WATER },
{ 8 , 24 , 3 ,  MSD::WATER },
{ 9 , 27 , 3 ,  MSD::WATER },
{ 10 , 30 , 3 ,  MSD::WATER },
{ 11 , 33 , 3 ,  MSD::WATER },
{ 12 , 36 , 3 ,  MSD::WATER },
{ 13 , 39 , 3 ,  MSD::WATER },
{ 14 , 42 , 3 ,  MSD::WATER },
{ 15 , 45 , 3 ,  MSD::WATER },
{ 16 , 48 , 3 ,  MSD::WATER },
{ 17 , 51 , 3 ,  MSD::WATER },
{ 18 , 54 , 3 ,  MSD::WATER },
{ 19 , 57 , 3 ,  MSD::WATER },
{ 20 , 60 , 3 ,  MSD::WATER },
{ 21 , 63 , 3 ,  MSD::WATER },
{ 22 , 66 , 3 ,  MSD::WATER },
{ 23 , 69 , 3 ,  MSD::WATER },
{ 24 , 72 , 3 ,  MSD::WATER },
{ 25 , 75 , 3 ,  MSD::WATER },
{ 26 , 78 , 3 ,  MSD::WATER },
{ 27 , 81 , 3 ,  MSD::WATER },
{ 28 , 84 , 3 ,  MSD::WATER },
{ 29 , 87 , 3 ,  MSD::WATER },
{ 30 , 90 , 3 ,  MSD::WATER },
{ 31 , 93 , 3 ,  MSD::WATER },
{ 32 , 96 , 3 ,  MSD::WATER }
};

const int MSD::PartitionListSize = 33;

const int MSD::PartitionCount = 33;



MSD MolecularSystemDef;

#endif
