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
 //File generated on: Sat Aug 09 14:28:17 EDT 2003
//%database_name:mdsetup
//%source_id:33124
//%system_id:50762
//%ctp_id:33279
//%impl_id:135
//%plt_id:1
//%probspec_version:v1.0.0
//%probspec_tag:RTP file is comming in from db2
//%magic_number:0xFACEB0B4



/** BEGIN_COMPILE_SCRIPT

#! /bin/ksh

rm -rf *.o

rm -rf *.a

# pick out install base
BG_INSTALL_BASE=`installbase.py`
export SHIPDATA_DIR=$BG_INSTALL_BASE/usr/opt/bluegene/
export SHIPDATA_INCLUDE=$SHIPDATA_DIR/include/
export SHIPDATA_LIB=$SHIPDATA_DIR/lib/
export SHIPDATA_SRC=$SHIPDATA_DIR/src/
export SHIPDATA_BIN=$SHIPDATA_DIR/bin/


export HW_INC_TOP=/gsa/watgsa/.home/h1/arayshu/bglsw/
export BLADE_INC=$HW_INC_TOP/blade/include


# First compiler pass ( compile the DATA ONLY )
xlC_r \
-I./ \
-I$SHIPDATA_INCLUDE \
-qpriority=1 \
-qalign=natural \
-DMSD_COMPILE_DATA_ONLY \
$1.cpp \
-c -o $1.data.o



# Second compiler pass ( compile the CODE ONLY )
echo 'xlC_r  -DARCH_HAS_FSEL  -DBLAKE  -DNDEBUG  -DNSQ_SINGLE_COMPUTE  -DPERFORMANCE_PERIODIC_IMAGE  -DPERFORMANCE_SWITCH  -DPK_AIX  -DPK_SMP  -DPK_XLC  -DPKFXLOG  -DSITE_SITE_CUTOFF  -DWATER_WATER_CUTOFF   -bmaxdata:0x40000000  -bmaxstack:0x10000000  -O2  -qalign=natural  -qarch=ppcgr  -qmaxmem=-1  -qnolm  -qpriority=0  -qtune=pwr3  -qalign=natural   -I. -I$SHIPDATA_INCLUDE  -DMSD_COMPILE_CODE_ONLY $1.data.o   $SHIPDATA_SRC/pk/exception.cpp $SHIPDATA_SRC/pk/fxlogger.cpp $SHIPDATA_SRC/pk/globject.cpp $SHIPDATA_SRC/pk/platform.cpp $SHIPDATA_SRC/pk/smp_reactor.cpp -L$SHIPDATA_LIB $SHIPDATA_LIB/libfftw.a $SHIPDATA_LIB/librfftw.a -lpthread  -c $1.cpp ;'
echo ''
xlC_r  -DARCH_HAS_FSEL  -DBLAKE  -DNDEBUG  -DNSQ_SINGLE_COMPUTE  -DPERFORMANCE_PERIODIC_IMAGE  -DPERFORMANCE_SWITCH  -DPK_AIX  -DPK_SMP  -DPK_XLC  -DPKFXLOG  -DSITE_SITE_CUTOFF  -DWATER_WATER_CUTOFF   -bmaxdata:0x40000000  -bmaxstack:0x10000000  -O2  -qalign=natural  -qarch=ppcgr  -qmaxmem=-1  -qnolm  -qpriority=0  -qtune=pwr3  -qalign=natural   -I. -I$SHIPDATA_INCLUDE  -DMSD_COMPILE_CODE_ONLY $1.data.o   $SHIPDATA_SRC/pk/exception.cpp $SHIPDATA_SRC/pk/fxlogger.cpp $SHIPDATA_SRC/pk/globject.cpp $SHIPDATA_SRC/pk/platform.cpp $SHIPDATA_SRC/pk/smp_reactor.cpp -L$SHIPDATA_LIB $SHIPDATA_LIB/libfftw.a $SHIPDATA_LIB/librfftw.a -lpthread  -c $1.cpp ;
echo 'xlC_r  -DARCH_HAS_FSEL  -DBLAKE  -DNDEBUG  -DNSQ_SINGLE_COMPUTE  -DPERFORMANCE_PERIODIC_IMAGE  -DPERFORMANCE_SWITCH  -DPK_AIX  -DPK_SMP  -DPK_XLC  -DPKFXLOG  -DSITE_SITE_CUTOFF  -DWATER_WATER_CUTOFF   -bmaxdata:0x40000000  -bmaxstack:0x10000000  -O2  -qalign=natural  -qarch=ppcgr  -qmaxmem=-1  -qnolm  -qpriority=0  -qtune=pwr3  -qalign=natural   -I. -I$SHIPDATA_INCLUDE  -DMSD_COMPILE_CODE_ONLY $1.data.o   $SHIPDATA_SRC/pk/exception.cpp $SHIPDATA_SRC/pk/fxlogger.cpp $SHIPDATA_SRC/pk/globject.cpp $SHIPDATA_SRC/pk/platform.cpp $SHIPDATA_SRC/pk/smp_reactor.cpp -L$SHIPDATA_LIB $SHIPDATA_LIB/libfftw.a $SHIPDATA_LIB/librfftw.a -lpthread  $1.o -o $1.smp.aix.exe
'
echo ''
xlC_r  -DARCH_HAS_FSEL  -DBLAKE  -DNDEBUG  -DNSQ_SINGLE_COMPUTE  -DPERFORMANCE_PERIODIC_IMAGE  -DPERFORMANCE_SWITCH  -DPK_AIX  -DPK_SMP  -DPK_XLC  -DPKFXLOG  -DSITE_SITE_CUTOFF  -DWATER_WATER_CUTOFF   -bmaxdata:0x40000000  -bmaxstack:0x10000000  -O2  -qalign=natural  -qarch=ppcgr  -qmaxmem=-1  -qnolm  -qpriority=0  -qtune=pwr3  -qalign=natural   -I. -I$SHIPDATA_INCLUDE  -DMSD_COMPILE_CODE_ONLY $1.data.o   $SHIPDATA_SRC/pk/exception.cpp $SHIPDATA_SRC/pk/fxlogger.cpp $SHIPDATA_SRC/pk/globject.cpp $SHIPDATA_SRC/pk/platform.cpp $SHIPDATA_SRC/pk/smp_reactor.cpp -L$SHIPDATA_LIB $SHIPDATA_LIB/libfftw.a $SHIPDATA_LIB/librfftw.a -lpthread  $1.o -o $1.smp.aix.exe



END_COMPILE_SCRIPT **/


#ifndef __MSD_HPP__
#define __MSD_HPP__
#ifndef EXECUTABLEID 
#define EXECUTABLEID -1
#endif 
#ifndef MSD_COMPILE_DATA_ONLY
#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <pk/pktrace_pk.hpp>  // Needed by the pk tracing instrumentation
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

#define MSDDEF_DoVelocityResample	1

#define MSDDEF_DoTemperatureControl	1

#define MSDDEF_DoShakeRattleProtein	1

#define MSDDEF_DoPressureControl	0

#define MSDDEF_DoShakeRattleWater	1

#define MSDDEF_DoNVT	1

#define MSDDEF_DoShakeRattle	1

#define MSDDEF_NOSE_HOOVER	0

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

static const int Rigid2Sites[];
static const int Rigid3Sites[];
static const int Rigid4Sites[];

static const UDF_Binding::ShakeGroup2_Params Rigid2Params[];
static const UDF_Binding::ShakeGroup3_Params Rigid3Params[];
static const UDF_Binding::ShakeGroup4_Params Rigid4Params[];

static const int ImproperDihedralForce_sitesSiteTupleList[];
static const UDF_Binding::ImproperDihedralForce_Params ImproperDihedralForce_paramsParamTupleList[];

static const int StdHarmonicAngleForce_sitesSiteTupleList[];
static const UDF_Binding::StdHarmonicAngleForce_Params StdHarmonicAngleForce_paramsParamTupleList[];

static const int StdHarmonicBondForce_sitesSiteTupleList[];
static const UDF_Binding::StdHarmonicBondForce_Params StdHarmonicBondForce_paramsParamTupleList[];

static const int SwopeTorsionForce_sitesSiteTupleList[];
static const UDF_Binding::SwopeTorsionForce_Params SwopeTorsionForce_paramsParamTupleList[];

static const int UreyBradleyForce_sitesSiteTupleList[];
static const UDF_Binding::UreyBradleyForce_Params UreyBradleyForce_paramsParamTupleList[];

};

#ifndef MSD_COMPILE_CODE_ONLY
// All molecules in their own fragment
const IrreduciblePartitionType MSD::IrreduciblePartitionTypeList[] = 
{
/*NONE*/ { 75 ,  MSD::OTHER, 0 , 35, 99999999.0 },
/*NONE*/ { 3 ,  MSD::WATER, 0 , 3, 1.1 * 0.9572 }
};

const int MSD::IrreduciblePartitionTypeListSize = 2;

const int MSD::IrreduciblePartitionCount = 8;

// {start_site , ip_type, molecule_id, fragments_index_in_molecule} 
const IrreduciblePartitionMap MSD::IrreduciblePartitionIndexToTypeMap[] = 
{
{ 0, 0, 0, 0 },
{ 75, 1, 1, 0 },
{ 78, 1, 2, 0 },
{ 81, 1, 3, 0 },
{ 84, 1, 4, 0 },
{ 87, 1, 5, 0 },
{ 90, 1, 6, 0 },
{ 93, 1, 7, 0 }
};

const int MSD::IrreduciblePartitionIndexToTypeMapSize = 8;

const int MSD::ExternalToInternalSiteIdMap[] = 
{
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95};
const int MSD::ExternalToInternalSiteIdMapSize = 96;

const int MSD::InternalToExternalSiteIdMap[] = 
{
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95};
const int MSD::InternalToExternalSiteIdMapSize = 96;



const int MSD::Rigid2Sites[] = {
4,5,
10,11,
12,13,
15,16,
17,18,
19,20,
23,24,
30,31,
37,38,
39,40,
45,46,
47,48,
49,50,
51,52,
53,54,
57,58,
59,60};

const UDF_Binding::ShakeGroup2_Params MSD::Rigid2Params[] = {
{{ 1.0800}} ,
{{ 1.0800}} ,
{{ 1.0800}} ,
{{ 0.9600}} ,
{{ 1.0800}} ,
{{ 1.0800}} ,
{{ 0.9970}} ,
{{ 0.9970}} ,
{{ 0.9970}} ,
{{ 1.0800}} ,
{{ 1.0800}} ,
{{ 1.0800}} ,
{{ 1.0800}} ,
{{ 1.0800}} ,
{{ 1.0800}} ,
{{ 0.9970}} ,
{{ 1.0800}} };

const int MSD::Rigid3Sites[] = {
6,7,8,
25,26,27,
32,33,34,
41,42,43,
61,62,63,
64,65,66};

const UDF_Binding::ShakeGroup3_Params MSD::Rigid3Params[] = {
{{ 1.1110,1.1110}} ,
{{ 1.1110,1.1110}} ,
{{ 1.0800,1.0800}} ,
{{ 1.0800,1.0800}} ,
{{ 1.0800,1.0800}} ,
{{ 1.0800,1.0800}} };

const int MSD::Rigid4Sites[] = {
0,1,2,3,
68,69,70,71};

const UDF_Binding::ShakeGroup4_Params MSD::Rigid4Params[] = {
{{ 1.0400,1.0400,1.0400}} ,
{{ 1.0400,1.0400,1.0400}} };/*
These are the sites.
*/
const int MSD::ImproperDihedralForce_sitesSiteTupleList[] = {
21,4,23,22,
23,21,25,24,
28,25,30,29,
30,28,32,31,
35,32,37,36,
37,35,39,38,
55,39,57,56,
57,55,59,58,
72,59,74,73};

#define NUMBER_OF_ImproperDihedralForce_sites_SITE_TUPLES 9

/*
The parameters are in the order of the structs above.
*/
const UDF_Binding::ImproperDihedralForce_Params MSD::ImproperDihedralForce_paramsParamTupleList[] = {
{ 120.0000 , (0.0000*Math::Deg2Rad) },
{ 20.0000 , (0.0000*Math::Deg2Rad) },
{ 120.0000 , (0.0000*Math::Deg2Rad) },
{ 20.0000 , (0.0000*Math::Deg2Rad) },
{ 120.0000 , (0.0000*Math::Deg2Rad) },
{ 20.0000 , (0.0000*Math::Deg2Rad) },
{ 120.0000 , (0.0000*Math::Deg2Rad) },
{ 20.0000 , (0.0000*Math::Deg2Rad) },
{ 96.0000 , (0.0000*Math::Deg2Rad) } };

#define NUMBER_OF_ImproperDihedralForce_params_PARAM_TUPLES 9

/*
These are the sites.
*/
const int MSD::StdHarmonicAngleForce_sitesSiteTupleList[] = {
0,4,5,
0,4,6,
0,4,21,
1,0,2,
1,0,3,
1,0,4,
2,0,3,
2,0,4,
3,0,4,
4,6,7,
4,6,8,
4,6,9,
4,21,22,
4,21,23,
5,4,6,
5,4,21,
6,4,21,
6,9,10,
6,9,17,
7,6,8,
7,6,9,
8,6,9,
9,10,11,
9,10,12,
9,17,18,
9,17,19,
10,9,17,
10,12,13,
10,12,14,
11,10,12,
12,14,15,
12,14,19,
13,12,14,
14,15,16,
14,19,17,
14,19,20,
15,14,19,
17,19,20,
18,17,19,
21,23,24,
21,23,25,
22,21,23,
23,25,26,
23,25,27,
23,25,28,
24,23,25,
25,28,29,
25,28,30,
26,25,27,
26,25,28,
27,25,28,
28,30,31,
28,30,32,
29,28,30,
30,32,33,
30,32,34,
30,32,35,
31,30,32,
32,35,36,
32,35,37,
33,32,34,
33,32,35,
34,32,35,
35,37,38,
35,37,39,
36,35,37,
37,39,40,
37,39,41,
37,39,55,
38,37,39,
39,41,42,
39,41,43,
39,41,44,
39,55,56,
39,55,57,
40,39,41,
40,39,55,
41,39,55,
41,44,45,
41,44,51,
42,41,43,
42,41,44,
43,41,44,
44,45,46,
44,45,47,
44,51,52,
44,51,53,
45,44,51,
45,47,48,
45,47,49,
46,45,47,
47,49,50,
47,49,53,
48,47,49,
49,53,51,
49,53,54,
50,49,53,
51,53,54,
52,51,53,
55,57,58,
55,57,59,
56,55,57,
57,59,60,
57,59,61,
57,59,72,
58,57,59,
59,61,62,
59,61,63,
59,61,64,
59,72,73,
59,72,74,
60,59,61,
60,59,72,
61,59,72,
61,64,65,
61,64,66,
61,64,67,
62,61,63,
62,61,64,
63,61,64,
64,67,68,
65,64,66,
65,64,67,
66,64,67,
67,68,69,
67,68,70,
67,68,71,
69,68,70,
69,68,71,
70,68,71,
73,72,74};

#define NUMBER_OF_StdHarmonicAngleForce_sites_SITE_TUPLES 131

/*
The parameters are in the order of the structs above.
*/
const UDF_Binding::StdHarmonicAngleForce_Params MSD::StdHarmonicAngleForce_paramsParamTupleList[] = {
{ 51.500 , (107.5000*Math::Deg2Rad) },
{ 67.700 , (110.0000*Math::Deg2Rad) },
{ 43.700 , (110.0000*Math::Deg2Rad) },
{ 44.000 , (109.5000*Math::Deg2Rad) },
{ 44.000 , (109.5000*Math::Deg2Rad) },
{ 30.000 , (109.50*Math::Deg2Rad) },
{ 44.000 , (109.5000*Math::Deg2Rad) },
{ 30.000 , (109.50*Math::Deg2Rad) },
{ 30.000 , (109.50*Math::Deg2Rad) },
{ 33.430 , (110.10*Math::Deg2Rad) },
{ 33.430 , (110.10*Math::Deg2Rad) },
{ 51.800 , (107.5000*Math::Deg2Rad) },
{ 80.000 , (121.0000*Math::Deg2Rad) },
{ 80.000 , (116.5000*Math::Deg2Rad) },
{ 35.000 , (111.0000*Math::Deg2Rad) },
{ 50.000 , (109.5000*Math::Deg2Rad) },
{ 52.000 , (108.0000*Math::Deg2Rad) },
{ 45.800 , (122.3000*Math::Deg2Rad) },
{ 45.800 , (122.3000*Math::Deg2Rad) },
{ 35.500 , (109.00*Math::Deg2Rad) },
{ 49.300 , (107.5000*Math::Deg2Rad) },
{ 49.300 , (107.5000*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 40.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 40.000 , (120.00*Math::Deg2Rad) },
{ 40.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 40.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 45.200 , (120.0000*Math::Deg2Rad) },
{ 40.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 65.000 , (108.0000*Math::Deg2Rad) },
{ 40.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 45.200 , (120.0000*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 34.000 , (123.0000*Math::Deg2Rad) },
{ 50.000 , (120.0000*Math::Deg2Rad) },
{ 80.000 , (122.5000*Math::Deg2Rad) },
{ 48.000 , (108.0000*Math::Deg2Rad) },
{ 48.000 , (108.0000*Math::Deg2Rad) },
{ 50.000 , (107.0000*Math::Deg2Rad) },
{ 35.000 , (117.0000*Math::Deg2Rad) },
{ 80.000 , (121.0000*Math::Deg2Rad) },
{ 80.000 , (116.5000*Math::Deg2Rad) },
{ 36.000 , (115.0000*Math::Deg2Rad) },
{ 50.000 , (109.5000*Math::Deg2Rad) },
{ 50.000 , (109.5000*Math::Deg2Rad) },
{ 34.000 , (123.0000*Math::Deg2Rad) },
{ 50.000 , (120.0000*Math::Deg2Rad) },
{ 80.000 , (122.5000*Math::Deg2Rad) },
{ 48.000 , (108.0000*Math::Deg2Rad) },
{ 48.000 , (108.0000*Math::Deg2Rad) },
{ 50.000 , (107.0000*Math::Deg2Rad) },
{ 35.000 , (117.0000*Math::Deg2Rad) },
{ 80.000 , (121.0000*Math::Deg2Rad) },
{ 80.000 , (116.5000*Math::Deg2Rad) },
{ 36.000 , (115.0000*Math::Deg2Rad) },
{ 50.000 , (109.5000*Math::Deg2Rad) },
{ 50.000 , (109.5000*Math::Deg2Rad) },
{ 34.000 , (123.0000*Math::Deg2Rad) },
{ 50.000 , (120.0000*Math::Deg2Rad) },
{ 80.000 , (122.5000*Math::Deg2Rad) },
{ 48.000 , (108.0000*Math::Deg2Rad) },
{ 70.000 , (113.5000*Math::Deg2Rad) },
{ 50.000 , (107.0000*Math::Deg2Rad) },
{ 35.000 , (117.0000*Math::Deg2Rad) },
{ 33.430 , (110.10*Math::Deg2Rad) },
{ 33.430 , (110.10*Math::Deg2Rad) },
{ 51.800 , (107.5000*Math::Deg2Rad) },
{ 80.000 , (121.0000*Math::Deg2Rad) },
{ 80.000 , (116.5000*Math::Deg2Rad) },
{ 35.000 , (111.0000*Math::Deg2Rad) },
{ 50.000 , (109.5000*Math::Deg2Rad) },
{ 52.000 , (108.0000*Math::Deg2Rad) },
{ 45.800 , (122.3000*Math::Deg2Rad) },
{ 45.800 , (122.3000*Math::Deg2Rad) },
{ 35.500 , (109.00*Math::Deg2Rad) },
{ 49.300 , (107.5000*Math::Deg2Rad) },
{ 49.300 , (107.5000*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 40.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 40.000 , (120.00*Math::Deg2Rad) },
{ 40.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 40.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 40.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 40.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 30.000 , (120.00*Math::Deg2Rad) },
{ 34.000 , (123.0000*Math::Deg2Rad) },
{ 50.000 , (120.0000*Math::Deg2Rad) },
{ 80.000 , (122.5000*Math::Deg2Rad) },
{ 48.000 , (108.0000*Math::Deg2Rad) },
{ 70.000 , (113.5000*Math::Deg2Rad) },
{ 50.000 , (107.0000*Math::Deg2Rad) },
{ 35.000 , (117.0000*Math::Deg2Rad) },
{ 33.430 , (110.10*Math::Deg2Rad) },
{ 33.430 , (110.10*Math::Deg2Rad) },
{ 58.350 , (113.50*Math::Deg2Rad) },
{ 40.000 , (118.00*Math::Deg2Rad) },
{ 40.000 , (118.00*Math::Deg2Rad) },
{ 35.000 , (111.0000*Math::Deg2Rad) },
{ 50.000 , (109.5000*Math::Deg2Rad) },
{ 52.000 , (108.0000*Math::Deg2Rad) },
{ 26.500 , (110.10*Math::Deg2Rad) },
{ 26.500 , (110.10*Math::Deg2Rad) },
{ 58.000 , (114.5000*Math::Deg2Rad) },
{ 35.500 , (109.00*Math::Deg2Rad) },
{ 26.500 , (110.10*Math::Deg2Rad) },
{ 26.500 , (110.10*Math::Deg2Rad) },
{ 34.000 , (95.0000*Math::Deg2Rad) },
{ 35.500 , (109.00*Math::Deg2Rad) },
{ 46.100 , (111.3000*Math::Deg2Rad) },
{ 46.100 , (111.3000*Math::Deg2Rad) },
{ 46.100 , (111.3000*Math::Deg2Rad) },
{ 46.100 , (111.3000*Math::Deg2Rad) },
{ 46.100 , (111.3000*Math::Deg2Rad) },
{ 35.500 , (108.40*Math::Deg2Rad) },
{ 35.500 , (108.40*Math::Deg2Rad) },
{ 35.500 , (108.40*Math::Deg2Rad) },
{ 100.000 , (124.00*Math::Deg2Rad) } };

#define NUMBER_OF_StdHarmonicAngleForce_params_PARAM_TUPLES 131

/*
These are the sites.
*/
const int MSD::StdHarmonicBondForce_sitesSiteTupleList[] = {
0,4,
6,4,
9,6,
10,9,
12,10,
12,14,
14,19,
15,14,
17,9,
19,17,
21,4,
21,23,
22,21,
23,25,
28,25,
28,30,
29,28,
30,32,
35,32,
35,37,
36,35,
37,39,
41,39,
44,41,
45,44,
47,45,
49,47,
49,53,
51,44,
53,51,
55,39,
55,57,
56,55,
57,59,
61,59,
64,61,
67,64,
68,67,
72,59,
72,73,
72,74};

#define NUMBER_OF_StdHarmonicBondForce_sites_SITE_TUPLES 41

/*
The parameters are in the order of the structs above.
*/
const UDF_Binding::StdHarmonicBondForce_Params MSD::StdHarmonicBondForce_paramsParamTupleList[] = {
{ 200.000 , 1.4800 },
{ 222.500 , 1.5380 },
{ 230.000 , 1.4900 },
{ 305.000 , 1.3750 },
{ 305.000 , 1.3750 },
{ 305.000 , 1.3750 },
{ 305.000 , 1.3750 },
{ 334.300 , 1.4110 },
{ 305.000 , 1.3750 },
{ 305.000 , 1.3750 },
{ 250.000 , 1.4900 },
{ 370.000 , 1.3450 },
{ 620.000 , 1.2300 },
{ 320.000 , 1.4300 },
{ 250.000 , 1.4900 },
{ 370.000 , 1.3450 },
{ 620.000 , 1.2300 },
{ 320.000 , 1.4300 },
{ 250.000 , 1.4900 },
{ 370.000 , 1.3450 },
{ 620.000 , 1.2300 },
{ 320.000 , 1.4300 },
{ 222.500 , 1.5380 },
{ 230.000 , 1.4900 },
{ 305.000 , 1.3750 },
{ 305.000 , 1.3750 },
{ 305.000 , 1.3750 },
{ 305.000 , 1.3750 },
{ 305.000 , 1.3750 },
{ 305.000 , 1.3750 },
{ 250.000 , 1.4900 },
{ 370.000 , 1.3450 },
{ 620.000 , 1.2300 },
{ 320.000 , 1.4300 },
{ 222.500 , 1.5380 },
{ 222.500 , 1.5300 },
{ 198.000 , 1.8180 },
{ 240.000 , 1.8160 },
{ 200.000 , 1.5220 },
{ 525.000 , 1.2600 },
{ 525.000 , 1.2600 } };

#define NUMBER_OF_StdHarmonicBondForce_params_PARAM_TUPLES 41

/*
These are the sites.
*/
const int MSD::SwopeTorsionForce_sitesSiteTupleList[] = {
0,4,6,7,
0,4,6,8,
0,4,6,9,
0,4,21,23,
1,0,4,5,
1,0,4,6,
1,0,4,21,
2,0,4,5,
2,0,4,6,
2,0,4,21,
3,0,4,5,
3,0,4,6,
3,0,4,21,
4,6,9,10,
4,6,9,17,
4,21,23,24,
4,21,23,25,
4,21,23,25,
5,4,6,7,
5,4,6,8,
5,4,6,9,
6,4,21,22,
6,9,10,11,
6,9,10,12,
6,9,17,18,
6,9,17,19,
7,6,4,21,
8,6,4,21,
9,6,4,21,
9,10,12,13,
9,10,12,14,
9,17,19,14,
9,17,19,20,
10,9,17,18,
10,9,17,19,
10,12,14,15,
10,12,14,19,
11,10,9,17,
11,10,12,13,
11,10,12,14,
12,10,9,17,
12,14,15,16,
12,14,19,17,
12,14,19,20,
13,12,14,15,
13,12,14,19,
14,19,17,18,
15,14,19,17,
15,14,19,20,
16,15,14,19,
18,17,19,20,
21,23,25,28,
22,21,23,24,
22,21,23,25,
23,25,28,30,
25,28,30,31,
25,28,30,32,
25,28,30,32,
28,30,32,35,
29,28,30,31,
29,28,30,32,
30,32,35,37,
32,35,37,38,
32,35,37,39,
32,35,37,39,
35,37,39,41,
35,37,39,55,
36,35,37,38,
36,35,37,39,
37,39,41,42,
37,39,41,43,
37,39,41,44,
37,39,55,57,
39,41,44,45,
39,41,44,51,
39,55,57,58,
39,55,57,59,
39,55,57,59,
40,39,41,42,
40,39,41,43,
40,39,41,44,
41,39,55,56,
41,44,45,46,
41,44,45,47,
41,44,51,52,
41,44,51,53,
42,41,39,55,
43,41,39,55,
44,41,39,55,
44,45,47,48,
44,45,47,49,
44,51,53,49,
44,51,53,54,
45,44,51,52,
45,44,51,53,
45,47,49,50,
45,47,49,53,
46,45,44,51,
46,45,47,48,
46,45,47,49,
47,45,44,51,
47,49,53,51,
47,49,53,54,
48,47,49,50,
48,47,49,53,
49,53,51,52,
50,49,53,51,
50,49,53,54,
52,51,53,54,
55,57,59,61,
55,57,59,72,
56,55,57,58,
56,55,57,59,
57,59,61,62,
57,59,61,63,
57,59,61,64,
57,59,72,73,
57,59,72,74,
59,61,64,65,
59,61,64,66,
59,61,64,67,
60,59,61,62,
60,59,61,63,
60,59,61,64,
60,59,72,73,
60,59,72,74,
61,59,72,73,
61,59,72,74,
61,64,67,68,
61,64,67,68,
62,61,59,72,
62,61,64,65,
62,61,64,66,
62,61,64,67,
63,61,59,72,
63,61,64,65,
63,61,64,66,
63,61,64,67,
64,61,59,72,
64,67,68,69,
64,67,68,70,
64,67,68,71,
65,64,67,68,
66,64,67,68};

#define NUMBER_OF_SwopeTorsionForce_sites_SITE_TUPLES 144

/*
The parameters are in the order of the structs above.
*/
const UDF_Binding::SwopeTorsionForce_Params MSD::SwopeTorsionForce_paramsParamTupleList[] = {
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.6000 , sin(0.00*Math::Deg2Rad) , 1 },
{ cos(0.00*Math::Deg2Rad) , 0.1000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(180.00*Math::Deg2Rad) , 0.2300 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 0.2300 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 1.6000 , sin(0.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 1.4000 , sin(0.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.0400 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 2.4000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 0.9900 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 0.9900 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 2.4000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 0.2000 , sin(180.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 0.6000 , sin(0.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 1.6000 , sin(0.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 0.2000 , sin(180.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 0.6000 , sin(0.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 1.6000 , sin(0.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 1.8000 , sin(0.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 0.2000 , sin(180.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.6000 , sin(0.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 0.2300 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 0.2300 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 1.6000 , sin(0.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 1.4000 , sin(0.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.0400 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 2.4000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 3.1000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 2.4000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 4.2000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 2.4000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 2.4000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 1.8000 , sin(0.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 0.2000 , sin(180.00*Math::Deg2Rad) , 1 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(180.00*Math::Deg2Rad) , 2.5000 , sin(180.00*Math::Deg2Rad) , 2 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(180.00*Math::Deg2Rad) , 0.0500 , sin(180.00*Math::Deg2Rad) , 6 },
{ cos(180.00*Math::Deg2Rad) , 0.0500 , sin(180.00*Math::Deg2Rad) , 6 },
{ cos(0.00*Math::Deg2Rad) , 0.1950 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1950 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1950 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(180.00*Math::Deg2Rad) , 0.0500 , sin(180.00*Math::Deg2Rad) , 6 },
{ cos(180.00*Math::Deg2Rad) , 0.0500 , sin(180.00*Math::Deg2Rad) , 6 },
{ cos(180.00*Math::Deg2Rad) , 0.0500 , sin(180.00*Math::Deg2Rad) , 6 },
{ cos(180.00*Math::Deg2Rad) , 0.0500 , sin(180.00*Math::Deg2Rad) , 6 },
{ cos(180.00*Math::Deg2Rad) , 0.2400 , sin(180.00*Math::Deg2Rad) , 1 },
{ cos(0.00*Math::Deg2Rad) , 0.3700 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1950 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1950 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.0100 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1950 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.1950 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.0100 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2000 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2800 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2800 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2800 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2800 , sin(0.00*Math::Deg2Rad) , 3 },
{ cos(0.00*Math::Deg2Rad) , 0.2800 , sin(0.00*Math::Deg2Rad) , 3 } };

#define NUMBER_OF_SwopeTorsionForce_params_PARAM_TUPLES 144

/*
These are the sites.
*/
const int MSD::UreyBradleyForce_sitesSiteTupleList[] = {
1,4,
2,4,
3,4,
4,7,
4,8,
7,8,
9,11,
9,12,
9,18,
9,19,
10,17,
10,13,
10,14,
11,12,
12,19,
13,14,
14,17,
14,20,
17,20,
18,19,
39,42,
39,43,
42,43,
44,46,
44,47,
44,52,
44,53,
45,51,
45,48,
45,49,
46,47,
47,50,
47,53,
48,49,
49,51,
49,54,
50,53,
51,54,
52,53,
59,62,
59,63,
59,64,
59,73,
59,74,
61,65,
61,66,
62,63,
62,64,
63,64,
65,66,
69,70,
69,71,
70,71,
73,74};

#define NUMBER_OF_UreyBradleyForce_sites_SITE_TUPLES 54

/*
The parameters are in the order of the structs above.
*/
const UDF_Binding::UreyBradleyForce_Params MSD::UreyBradleyForce_paramsParamTupleList[] = {
{ 20.00 , 2.07400 },
{ 20.00 , 2.07400 },
{ 20.00 , 2.07400 },
{ 22.53 , 2.17900 },
{ 22.53 , 2.17900 },
{ 5.40 , 1.80200 },
{ 22.00 , 2.15250 },
{ 35.00 , 2.41620 },
{ 22.00 , 2.15250 },
{ 35.00 , 2.41620 },
{ 35.00 , 2.41620 },
{ 22.00 , 2.15250 },
{ 35.00 , 2.41620 },
{ 22.00 , 2.15250 },
{ 35.00 , 2.41620 },
{ 22.00 , 2.15250 },
{ 35.00 , 2.41620 },
{ 22.00 , 2.15250 },
{ 22.00 , 2.15250 },
{ 22.00 , 2.15250 },
{ 22.53 , 2.17900 },
{ 22.53 , 2.17900 },
{ 5.40 , 1.80200 },
{ 22.00 , 2.15250 },
{ 35.00 , 2.41620 },
{ 22.00 , 2.15250 },
{ 35.00 , 2.41620 },
{ 35.00 , 2.41620 },
{ 22.00 , 2.15250 },
{ 35.00 , 2.41620 },
{ 22.00 , 2.15250 },
{ 22.00 , 2.15250 },
{ 35.00 , 2.41620 },
{ 22.00 , 2.15250 },
{ 35.00 , 2.41620 },
{ 22.00 , 2.15250 },
{ 22.00 , 2.15250 },
{ 22.00 , 2.15250 },
{ 22.00 , 2.15250 },
{ 22.53 , 2.17900 },
{ 22.53 , 2.17900 },
{ 11.16 , 2.56100 },
{ 50.00 , 2.38800 },
{ 50.00 , 2.38800 },
{ 22.53 , 2.17900 },
{ 22.53 , 2.17900 },
{ 5.40 , 1.80200 },
{ 22.53 , 2.17900 },
{ 22.53 , 2.17900 },
{ 5.40 , 1.80200 },
{ 5.40 , 1.80200 },
{ 5.40 , 1.80200 },
{ 5.40 , 1.80200 },
{ 70.00 , 2.22500 } };

#define NUMBER_OF_UreyBradleyForce_params_PARAM_TUPLES 54

const double MSD::TimeConversionFactorMsd = TimeConversionFactor;

const double MSD::ShakeTolerance = 1.0E-10;

const double MSD::EwaldAlpha = 0.0;

double MSD::VelocityResampleTargetTemperature = 0.0;

double MSD::PressureControlPistonInitialVelocity = 0.0;

int MSD::NumberOfOuterTimeSteps = 50;

int MSD::VelocityResampleRandomSeed = 51579;

double MSD::PressureControlPistonMass = 0.0004 * 25.0 / ( cbrt(((0)-( 0))*((0)-(0))*((0 )-(0))) );

double MSD::PressureControlTarget = 1.0;

const double MSD::RattleTolerance = 1.0E-10;

const double MSD::Lambda = 0.0;

const int MSD::P3MEdiffOrder = 2;

const double MSD::OuterTimeStepInPicoSeconds = 0.001;

int MSD::SnapshotPeriodInOTS = 1000;

const int MSD::ShakeMaxIterations = 1000;

const double MSD::LeknerEpsilon = 0.0;

const int MSD::DoOuter = 1;

int MSD::EmitEnergyTimeStepModulo = 1;

int MSD::VelocityResamplePeriodInOTS = 0;

const double MSD::InnerTimeStepInPicoSeconds = -1.0;

const int MSD::EwaldKmax = 0;

const int MSD::P3MEchargeAssignment = 4;

const double MSD::ExternalPressure = 1.0;

BoundingBox MSD::BoundingBoxDef = {0,0,0,0,0,0};

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
1.0E100
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
{ 12.0110 , 1.0/(2.0 * 12.0110) , (-.140000*ChargeConversion) },
{ 12.0110 , 1.0/(2.0 * 12.0110) , (-.180000*ChargeConversion) },
{ 12.0110 , 1.0/(2.0 * 12.0110) , (-.200000E-01*ChargeConversion) },
{ 12.0110 , 1.0/(2.0 * 12.0110) , (-.220000*ChargeConversion) },
{ 12.0110 , 1.0/(2.0 * 12.0110) , (.210000*ChargeConversion) },
{ 12.0110 , 1.0/(2.0 * 12.0110) , (.700000E-01*ChargeConversion) },
{ 1.00800 , 1.0/(2.0 * 1.00800) , (.100000*ChargeConversion) },
{ 1.00800 , 1.0/(2.0 * 1.00800) , (.900000E-01*ChargeConversion) },
{ 1.00800 , 1.0/(2.0 * 1.00800) , (.115000*ChargeConversion) },
{ 1.00800 , 1.0/(2.0 * 1.00800) , (.310000*ChargeConversion) },
{ 1.00800 , 1.0/(2.0 * 1.00800) , (.330000*ChargeConversion) },
{ 1.00800 , 1.0/(2.0 * 1.00800) , (.430000*ChargeConversion) },
{ 12.0110 , 1.0/(2.0 * 12.0110) , (-.115000*ChargeConversion) },
{ 12.0110 , 1.0/(2.0 * 12.0110) , (.000000E+00*ChargeConversion) },
{ 12.0110 , 1.0/(2.0 * 12.0110) , (.110000*ChargeConversion) },
{ 12.0110 , 1.0/(2.0 * 12.0110) , (.340000*ChargeConversion) },
{ 12.0110 , 1.0/(2.0 * 12.0110) , (.510000*ChargeConversion) },
{ 15.9990 , 1.0/(2.0 * 15.9990) , (-.510000*ChargeConversion) },
{ 15.9990 , 1.0/(2.0 * 15.9990) , (-.670000*ChargeConversion) },
{ 15.9990 , 1.0/(2.0 * 15.9990) , (-.540000*ChargeConversion) },
{ 14.0070 , 1.0/(2.0 * 14.0070) , (-.470000*ChargeConversion) },
{ 14.0070 , 1.0/(2.0 * 14.0070) , (-.300000*ChargeConversion) },
{ 32.0600 , 1.0/(2.0 * 32.0600) , (-.900000E-01*ChargeConversion) },
{ 1.00800 , 1.0/(2.0 * 1.00800) , 0.4170 * ChargeConversion },
{ 15.9994 , 1.0/(2.0 * 15.9994) , -0.8340 * ChargeConversion }
};
const int MSD::SiteInformationSize = 25;


const int MSD::Pair14SiteList[] = 
{
0 , 7,
0 , 8,
0 , 9,
0 , 22,
0 , 23,
1 , 5,
1 , 6,
1 , 21,
2 , 5,
2 , 6,
2 , 21,
3 , 5,
3 , 6,
3 , 21,
4 , 10,
4 , 17,
4 , 24,
4 , 25,
5 , 7,
5 , 8,
5 , 9,
5 , 22,
5 , 23,
6 , 11,
6 , 12,
6 , 18,
6 , 19,
6 , 22,
6 , 23,
7 , 10,
7 , 17,
7 , 21,
8 , 10,
8 , 17,
8 , 21,
9 , 13,
9 , 14,
9 , 20,
9 , 21,
10 , 15,
10 , 18,
10 , 19,
11 , 13,
11 , 14,
11 , 17,
12 , 16,
12 , 17,
12 , 20,
13 , 15,
13 , 19,
14 , 18,
15 , 17,
15 , 20,
16 , 19,
18 , 20,
21 , 26,
21 , 27,
21 , 28,
22 , 24,
22 , 25,
23 , 29,
23 , 30,
24 , 26,
24 , 27,
24 , 28,
25 , 31,
25 , 32,
26 , 29,
26 , 30,
27 , 29,
27 , 30,
28 , 33,
28 , 34,
28 , 35,
29 , 31,
29 , 32,
30 , 36,
30 , 37,
31 , 33,
31 , 34,
31 , 35,
32 , 38,
32 , 39,
33 , 36,
33 , 37,
34 , 36,
34 , 37,
35 , 40,
35 , 41,
35 , 55,
36 , 38,
36 , 39,
37 , 42,
37 , 43,
37 , 44,
37 , 56,
37 , 57,
38 , 40,
38 , 41,
38 , 55,
39 , 45,
39 , 51,
39 , 58,
39 , 59,
40 , 42,
40 , 43,
40 , 44,
40 , 56,
40 , 57,
41 , 46,
41 , 47,
41 , 52,
41 , 53,
41 , 56,
41 , 57,
42 , 45,
42 , 51,
42 , 55,
43 , 45,
43 , 51,
43 , 55,
44 , 48,
44 , 49,
44 , 54,
44 , 55,
45 , 50,
45 , 52,
45 , 53,
46 , 48,
46 , 49,
46 , 51,
47 , 51,
47 , 54,
48 , 50,
48 , 53,
49 , 52,
50 , 51,
50 , 54,
52 , 54,
55 , 60,
55 , 61,
55 , 72,
56 , 58,
56 , 59,
57 , 62,
57 , 63,
57 , 64,
57 , 73,
57 , 74,
58 , 60,
58 , 61,
58 , 72,
59 , 65,
59 , 66,
59 , 67,
60 , 62,
60 , 63,
60 , 64,
60 , 73,
60 , 74,
61 , 68,
61 , 73,
61 , 74,
62 , 65,
62 , 66,
62 , 67,
62 , 72,
63 , 65,
63 , 66,
63 , 67,
63 , 72,
64 , 69,
64 , 70,
64 , 71,
64 , 72,
65 , 68,
66 , 68
};

const int MSD::Pair14SiteListSize = 177;

const UDF_Binding::Coulomb14_Params MSD::Charge14PairTable[] = 
{ 
{ (-.300000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (-.300000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (-.300000*ChargeConversion) * (.000000E+00*ChargeConversion) * charge14scale },
{ (-.300000*ChargeConversion) * (-.510000*ChargeConversion) * charge14scale },
{ (-.300000*ChargeConversion) * (-.470000*ChargeConversion) * charge14scale },
{ (.330000*ChargeConversion) * (.100000*ChargeConversion) * charge14scale },
{ (.330000*ChargeConversion) * (-.180000*ChargeConversion) * charge14scale },
{ (.330000*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (.330000*ChargeConversion) * (.100000*ChargeConversion) * charge14scale },
{ (.330000*ChargeConversion) * (-.180000*ChargeConversion) * charge14scale },
{ (.330000*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (.330000*ChargeConversion) * (.100000*ChargeConversion) * charge14scale },
{ (.330000*ChargeConversion) * (-.180000*ChargeConversion) * charge14scale },
{ (.330000*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (.210000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.210000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.210000*ChargeConversion) * (.310000*ChargeConversion) * charge14scale },
{ (.210000*ChargeConversion) * (-.200000E-01*ChargeConversion) * charge14scale },
{ (.100000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.100000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.100000*ChargeConversion) * (.000000E+00*ChargeConversion) * charge14scale },
{ (.100000*ChargeConversion) * (-.510000*ChargeConversion) * charge14scale },
{ (.100000*ChargeConversion) * (-.470000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (-.510000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (-.470000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (.000000E+00*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.000000E+00*ChargeConversion) * (.110000*ChargeConversion) * charge14scale },
{ (.000000E+00*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.000000E+00*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (-.115000*ChargeConversion) * (-.540000*ChargeConversion) * charge14scale },
{ (-.115000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (-.115000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (.110000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (-.115000*ChargeConversion) * (.430000*ChargeConversion) * charge14scale },
{ (-.115000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (-.115000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (-.540000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.110000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (-.540000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (-.540000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.430000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.510000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.510000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.510000*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (-.510000*ChargeConversion) * (.310000*ChargeConversion) * charge14scale },
{ (-.510000*ChargeConversion) * (-.200000E-01*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (-.510000*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (-.470000*ChargeConversion) * charge14scale },
{ (.310000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.310000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.310000*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (-.200000E-01*ChargeConversion) * (.310000*ChargeConversion) * charge14scale },
{ (-.200000E-01*ChargeConversion) * (-.200000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.510000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.470000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.510000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.470000*ChargeConversion) * charge14scale },
{ (.510000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.510000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.510000*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (-.510000*ChargeConversion) * (.310000*ChargeConversion) * charge14scale },
{ (-.510000*ChargeConversion) * (-.200000E-01*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (-.510000*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (-.470000*ChargeConversion) * charge14scale },
{ (.310000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.310000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.310000*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (-.200000E-01*ChargeConversion) * (.310000*ChargeConversion) * charge14scale },
{ (-.200000E-01*ChargeConversion) * (.700000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.510000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.470000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.510000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.470000*ChargeConversion) * charge14scale },
{ (.510000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.510000*ChargeConversion) * (-.180000*ChargeConversion) * charge14scale },
{ (.510000*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (-.510000*ChargeConversion) * (.310000*ChargeConversion) * charge14scale },
{ (-.510000*ChargeConversion) * (.700000E-01*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (.000000E+00*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (-.510000*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (-.470000*ChargeConversion) * charge14scale },
{ (.310000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.310000*ChargeConversion) * (-.180000*ChargeConversion) * charge14scale },
{ (.310000*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (.700000E-01*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.700000E-01*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.700000E-01*ChargeConversion) * (.310000*ChargeConversion) * charge14scale },
{ (.700000E-01*ChargeConversion) * (.700000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.000000E+00*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.510000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.470000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (-.510000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (-.470000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (.000000E+00*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.000000E+00*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.000000E+00*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.000000E+00*ChargeConversion) * (.510000*ChargeConversion) * charge14scale },
{ (-.115000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (-.115000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (-.115000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (-.115000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (-.115000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (-.115000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (-.115000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.115000*ChargeConversion) * (.115000*ChargeConversion) * charge14scale },
{ (.510000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.510000*ChargeConversion) * (-.180000*ChargeConversion) * charge14scale },
{ (.510000*ChargeConversion) * (.340000*ChargeConversion) * charge14scale },
{ (-.510000*ChargeConversion) * (.310000*ChargeConversion) * charge14scale },
{ (-.510000*ChargeConversion) * (.700000E-01*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (-.140000*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (-.670000*ChargeConversion) * charge14scale },
{ (-.470000*ChargeConversion) * (-.670000*ChargeConversion) * charge14scale },
{ (.310000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.310000*ChargeConversion) * (-.180000*ChargeConversion) * charge14scale },
{ (.310000*ChargeConversion) * (.340000*ChargeConversion) * charge14scale },
{ (.700000E-01*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.700000E-01*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.700000E-01*ChargeConversion) * (-.900000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.140000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.670000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.670000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (-.220000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (-.670000*ChargeConversion) * charge14scale },
{ (-.180000*ChargeConversion) * (-.670000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.900000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.340000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.900000E-01*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (.340000*ChargeConversion) * charge14scale },
{ (-.140000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (-.140000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (-.140000*ChargeConversion) * (.900000E-01*ChargeConversion) * charge14scale },
{ (-.140000*ChargeConversion) * (.340000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.220000*ChargeConversion) * charge14scale },
{ (.900000E-01*ChargeConversion) * (-.220000*ChargeConversion) * charge14scale }
};

const int MSD::Charge14PairTableSize = 177;

const UDF_Binding::LennardJonesForce_Params MSD::LJ14PairTable[] = 
{ 
{ +0.010000 , 2*1.900000 },
{ +0.010000 , 2*1.900000 },
{ +0.010000 , 2*1.900000 },
{ +0.010000 , 2*1.900000 },
{ +0.010000 , 2*1.900000 },
{ +0.010000 , 2*1.900000 },
{ +0.022000 , 2*1.320000 },
{ +0.022000 , 2*1.320000 },
{ +0.030000 , 2*1.358200 },
{ +0.046000 , 2*0.224500 },
{ +0.046000 , 2*0.224500 },
{ +0.046000 , 2*0.224500 },
{ +0.070000 , 2*1.992400 },
{ +0.070000 , 2*1.992400 },
{ +0.070000 , 2*1.992400 },
{ +0.070000 , 2*2.000000 },
{ +0.110000 , 2*2.000000 },
{ +0.120000 , 2*1.400000 },
{ +0.120000 , 2*1.700000 },
{ +0.152100 , 2*1.770000 },
{ +0.200000 , 2*1.550000 },
{ +0.200000 , 2*1.850000 },
{ +0.450000 , 2*2.000000 },
{ 0.0 , 0.0 },
{ 0.0 , 0.0 }};

const int MSD::LJ14PairTableSize = 25;

/* 
UdfCode, size of tupleList, siteTupleListPtr, paramTupleListPtr, CombinationRule, ToCombine
*/

UdfInvocation MSD::UdfInvocationTable[] = 
{
{ 7 , 9 , (int*) ImproperDihedralForce_sitesSiteTupleList, (void*) ImproperDihedralForce_paramsParamTupleList,  MSD::NORM,  -1  },
{ 2 , 131 , (int*) StdHarmonicAngleForce_sitesSiteTupleList, (void*) StdHarmonicAngleForce_paramsParamTupleList,  MSD::NORM,  -1  },
{ 1 , 41 , (int*) StdHarmonicBondForce_sitesSiteTupleList, (void*) StdHarmonicBondForce_paramsParamTupleList,  MSD::NORM,  -1  },
{ 3 , 144 , (int*) SwopeTorsionForce_sitesSiteTupleList, (void*) SwopeTorsionForce_paramsParamTupleList,  MSD::NORM,  -1  },
{ 4 , 54 , (int*) UreyBradleyForce_sitesSiteTupleList, (void*) UreyBradleyForce_paramsParamTupleList,  MSD::NORM,  -1  },
{ 36 , 177 , (int*)Pair14SiteList , (void*)Charge14PairTable, MSD::NORM, -1 },
{ 8 , 177 , (int*)Pair14SiteList , (void*)LJ14PairTable, MSD::COMB, 32 }
};

const int MSD::UdfInvocationTableSize = 7;

const UDF_Binding::NSQCoulomb_Params MSD::ChargeNsqParams[] = 
{
{ (.210000*ChargeConversion) },
{ (.700000E-01*ChargeConversion) },
{ (.100000*ChargeConversion) },
{ (.900000E-01*ChargeConversion) },
{ (.115000*ChargeConversion) },
{ (.310000*ChargeConversion) },
{ (.330000*ChargeConversion) },
{ (.430000*ChargeConversion) },
{ (-.140000*ChargeConversion) },
{ (-.180000*ChargeConversion) },
{ (-.200000E-01*ChargeConversion) },
{ (-.115000*ChargeConversion) },
{ (.000000E+00*ChargeConversion) },
{ (.110000*ChargeConversion) },
{ (.340000*ChargeConversion) },
{ (-.220000*ChargeConversion) },
{ (.510000*ChargeConversion) },
{ (-.510000*ChargeConversion) },
{ (-.670000*ChargeConversion) },
{ (-.540000*ChargeConversion) },
{ (-.300000*ChargeConversion) },
{ (-.470000*ChargeConversion) },
{ (-.900000E-01*ChargeConversion) },
{ 0.4170 * ChargeConversion },
{ -0.8340 * ChargeConversion }
};
const int MSD::NumberOfNsqSiteTypes = 25;

const UDF_Binding::NSQLennardJones_Params MSD::LJNsqParams[] = 
{
{ +0.020000 , 2*2.275000 },
{ +0.020000 , 2*2.275000 },
{ +0.022000 , 2*1.320000 },
{ +0.022000 , 2*1.320000 },
{ +0.030000 , 2*1.358200 },
{ +0.046000 , 2*0.224500 },
{ +0.046000 , 2*0.224500 },
{ +0.046000 , 2*0.224500 },
{ +0.055000 , 2*2.175000 },
{ +0.055000 , 2*2.175000 },
{ +0.055000 , 2*2.175000 },
{ +0.070000 , 2*1.992400 },
{ +0.070000 , 2*1.992400 },
{ +0.070000 , 2*1.992400 },
{ +0.070000 , 2*2.000000 },
{ +0.080000 , 2*2.060000 },
{ +0.110000 , 2*2.000000 },
{ +0.120000 , 2*1.700000 },
{ +0.120000 , 2*1.700000 },
{ +0.152100 , 2*1.770000 },
{ +0.200000 , 2*1.850000 },
{ +0.200000 , 2*1.850000 },
{ +0.450000 , 2*2.000000 },
{ 0.0 , 0.0 * SciConst::SixthRootOfTwo },
{ 0.1521 , 3.1506 * SciConst::SixthRootOfTwo }
};
const int MSD::NsqSiteIDtoSiteTypeMap[] = 
{
20,
6,
6,
6,
0,
2,
9,
3,
3,
12,
11,
4,
11,
4,
13,
19,
7,
11,
4,
11,
4,
16,
17,
21,
5,
10,
3,
3,
16,
17,
21,
5,
10,
3,
3,
16,
17,
21,
5,
1,
3,
9,
3,
3,
12,
11,
4,
11,
4,
11,
4,
11,
4,
11,
4,
16,
17,
21,
5,
1,
3,
9,
3,
3,
8,
3,
3,
22,
15,
3,
3,
3,
14,
18,
18,
24,
23,
23,
24,
23,
23,
24,
23,
23,
24,
23,
23,
24,
23,
23,
24,
23,
23,
24,
23,
23
};
const int MSD::NsqSiteIDtoSiteTypeMapSize = 96;

const int MSD::ExclusionList1_2_3_4[] = 
{
0 , 1,
0 , 2,
0 , 3,
0 , 4,
0 , 5,
0 , 6,
0 , 7,
0 , 8,
0 , 9,
0 , 21,
0 , 22,
0 , 23,
1 , 2,
1 , 3,
1 , 4,
1 , 5,
1 , 6,
1 , 21,
2 , 3,
2 , 4,
2 , 5,
2 , 6,
2 , 21,
3 , 4,
3 , 5,
3 , 6,
3 , 21,
4 , 5,
4 , 6,
4 , 7,
4 , 8,
4 , 9,
4 , 10,
4 , 17,
4 , 21,
4 , 22,
4 , 23,
4 , 24,
4 , 25,
5 , 6,
5 , 7,
5 , 8,
5 , 9,
5 , 21,
5 , 22,
5 , 23,
6 , 7,
6 , 8,
6 , 9,
6 , 10,
6 , 11,
6 , 12,
6 , 17,
6 , 18,
6 , 19,
6 , 21,
6 , 22,
6 , 23,
7 , 8,
7 , 9,
7 , 10,
7 , 17,
7 , 21,
8 , 9,
8 , 10,
8 , 17,
8 , 21,
9 , 10,
9 , 11,
9 , 12,
9 , 13,
9 , 14,
9 , 17,
9 , 18,
9 , 19,
9 , 20,
9 , 21,
10 , 11,
10 , 12,
10 , 13,
10 , 14,
10 , 15,
10 , 17,
10 , 18,
10 , 19,
11 , 12,
11 , 13,
11 , 14,
11 , 17,
12 , 13,
12 , 14,
12 , 15,
12 , 16,
12 , 17,
12 , 19,
12 , 20,
13 , 14,
13 , 15,
13 , 19,
14 , 15,
14 , 16,
14 , 17,
14 , 18,
14 , 19,
14 , 20,
15 , 16,
15 , 17,
15 , 19,
15 , 20,
16 , 19,
17 , 18,
17 , 19,
17 , 20,
18 , 19,
18 , 20,
19 , 20,
21 , 22,
21 , 23,
21 , 24,
21 , 25,
21 , 26,
21 , 27,
21 , 28,
22 , 23,
22 , 24,
22 , 25,
23 , 24,
23 , 25,
23 , 26,
23 , 27,
23 , 28,
23 , 29,
23 , 30,
24 , 25,
24 , 26,
24 , 27,
24 , 28,
25 , 26,
25 , 27,
25 , 28,
25 , 29,
25 , 30,
25 , 31,
25 , 32,
26 , 27,
26 , 28,
26 , 29,
26 , 30,
27 , 28,
27 , 29,
27 , 30,
28 , 29,
28 , 30,
28 , 31,
28 , 32,
28 , 33,
28 , 34,
28 , 35,
29 , 30,
29 , 31,
29 , 32,
30 , 31,
30 , 32,
30 , 33,
30 , 34,
30 , 35,
30 , 36,
30 , 37,
31 , 32,
31 , 33,
31 , 34,
31 , 35,
32 , 33,
32 , 34,
32 , 35,
32 , 36,
32 , 37,
32 , 38,
32 , 39,
33 , 34,
33 , 35,
33 , 36,
33 , 37,
34 , 35,
34 , 36,
34 , 37,
35 , 36,
35 , 37,
35 , 38,
35 , 39,
35 , 40,
35 , 41,
35 , 55,
36 , 37,
36 , 38,
36 , 39,
37 , 38,
37 , 39,
37 , 40,
37 , 41,
37 , 42,
37 , 43,
37 , 44,
37 , 55,
37 , 56,
37 , 57,
38 , 39,
38 , 40,
38 , 41,
38 , 55,
39 , 40,
39 , 41,
39 , 42,
39 , 43,
39 , 44,
39 , 45,
39 , 51,
39 , 55,
39 , 56,
39 , 57,
39 , 58,
39 , 59,
40 , 41,
40 , 42,
40 , 43,
40 , 44,
40 , 55,
40 , 56,
40 , 57,
41 , 42,
41 , 43,
41 , 44,
41 , 45,
41 , 46,
41 , 47,
41 , 51,
41 , 52,
41 , 53,
41 , 55,
41 , 56,
41 , 57,
42 , 43,
42 , 44,
42 , 45,
42 , 51,
42 , 55,
43 , 44,
43 , 45,
43 , 51,
43 , 55,
44 , 45,
44 , 46,
44 , 47,
44 , 48,
44 , 49,
44 , 51,
44 , 52,
44 , 53,
44 , 54,
44 , 55,
45 , 46,
45 , 47,
45 , 48,
45 , 49,
45 , 50,
45 , 51,
45 , 52,
45 , 53,
46 , 47,
46 , 48,
46 , 49,
46 , 51,
47 , 48,
47 , 49,
47 , 50,
47 , 51,
47 , 53,
47 , 54,
48 , 49,
48 , 50,
48 , 53,
49 , 50,
49 , 51,
49 , 52,
49 , 53,
49 , 54,
50 , 51,
50 , 53,
50 , 54,
51 , 52,
51 , 53,
51 , 54,
52 , 53,
52 , 54,
53 , 54,
55 , 56,
55 , 57,
55 , 58,
55 , 59,
55 , 60,
55 , 61,
55 , 72,
56 , 57,
56 , 58,
56 , 59,
57 , 58,
57 , 59,
57 , 60,
57 , 61,
57 , 62,
57 , 63,
57 , 64,
57 , 72,
57 , 73,
57 , 74,
58 , 59,
58 , 60,
58 , 61,
58 , 72,
59 , 60,
59 , 61,
59 , 62,
59 , 63,
59 , 64,
59 , 65,
59 , 66,
59 , 67,
59 , 72,
59 , 73,
59 , 74,
60 , 61,
60 , 62,
60 , 63,
60 , 64,
60 , 72,
60 , 73,
60 , 74,
61 , 62,
61 , 63,
61 , 64,
61 , 65,
61 , 66,
61 , 67,
61 , 68,
61 , 72,
61 , 73,
61 , 74,
62 , 63,
62 , 64,
62 , 65,
62 , 66,
62 , 67,
62 , 72,
63 , 64,
63 , 65,
63 , 66,
63 , 67,
63 , 72,
64 , 65,
64 , 66,
64 , 67,
64 , 68,
64 , 69,
64 , 70,
64 , 71,
64 , 72,
65 , 66,
65 , 67,
65 , 68,
66 , 67,
66 , 68,
67 , 68,
67 , 69,
67 , 70,
67 , 71,
68 , 69,
68 , 70,
68 , 71,
69 , 70,
69 , 71,
70 , 71,
72 , 73,
72 , 74,
73 , 74,
75 , 76,
75 , 77,
76 , 77,
78 , 79,
78 , 80,
79 , 80,
81 , 82,
81 , 83,
82 , 83,
84 , 85,
84 , 86,
85 , 86,
87 , 88,
87 , 89,
88 , 89,
90 , 91,
90 , 92,
91 , 92,
93 , 94,
93 , 95,
94 , 95
};
const int MSD::ExclusionList1_2_3_4Size = 405;

const NsqUdfInvocation MSD::NsqInvocationTable[] = 
{
{ 31, -1, MSD::NSQ2, 405, MSD::Exclusion, (SiteTuplePair *)ExclusionList1_2_3_4, (void *)ChargeNsqParams  } ,
{ 30, 32, MSD::NSQ1, 405, MSD::Exclusion, (SiteTuplePair *)ExclusionList1_2_3_4,  (void *)LJNsqParams } 
};
const int MSD::NsqInvocationTableSize = 2;

const int MSD::SiteIDtoSiteTypeMap[] = 
{
21,
10,
10,
10,
4,
6,
1,
7,
7,
13,
12,
8,
12,
8,
14,
19,
11,
12,
8,
12,
8,
16,
17,
20,
9,
2,
7,
7,
16,
17,
20,
9,
2,
7,
7,
16,
17,
20,
9,
5,
7,
1,
7,
7,
13,
12,
8,
12,
8,
12,
8,
12,
8,
12,
8,
16,
17,
20,
9,
5,
7,
1,
7,
7,
0,
7,
7,
22,
3,
7,
7,
7,
15,
18,
18,
24,
23,
23,
24,
23,
23,
24,
23,
23,
24,
23,
23,
24,
23,
23,
24,
23,
23,
24,
23,
23
};
const int MSD::SiteIDtoSiteTypeMapSize = 96;

const int MSD::Water3sites[] = {
76,75,77,
79,78,80,
82,81,83,
85,84,86,
88,87,89,
91,90,92,
94,93,95};



const int MSD::NumberOfConstraints =56;

const int MSD::magicNumber = 0xFACEB0B4;

const int MSD::sourceId = 33124;

MSD MolecularSystemDef;

#endif // MSD_COMPILE_CODE_ONLY

// This is the common area, defines go here...
#define NUMBER_OF_SITES 96
#define MSD_RTP_BoundingBoxDef_NONCONST  
#define MSD_RTP_PressureControlTarget_NONCONST  
#define EXCLUSION_LIST1_2_3_4 405
#define MSD_RTP_PressureControlPistonInitialVelocity_NONCONST  
#define NUMBER_OF_FRAGMENTS 8
#define NUMBER_OF_SITES_PER_THERMOSTAT 3
#define MSD_RTP_VelocityResamplePeriodInOTS_NONCONST  
#define MSD_RTP_VelocityResampleTargetTemperature_NONCONST  
#define MSD_RTP_EmitEnergyTimeStepModulo_NONCONST  
#define NUMBER_OF_NSQ_SITE_TYPES 25
#define PAIR14_SITE_LIST_SIZE 177
#define MSD_RTP_VelocityResampleRandomSeed_NONCONST  
#define MSD_RTP_NumberOfOuterTimeSteps_NONCONST  
#define NUMBER_OF_SHELLS 1
#define MSD_RTP_SnapshotPeriodInOTS_NONCONST  
#define MSD_RTP_PressureControlPistonMass_NONCONST  
#define NUMBER_OF_RESPA_LEVELS 1

#ifndef MSD_COMPILE_DATA_ONLY

extern MSD MolecularSystemDef;

struct RTG_PARAMETERS
{
  enum { NUM_OF_SHELLS               = NUMBER_OF_SHELLS }; 
  enum { NUM_OF_THERMOSTATS          = 2 }; 
  enum { NUM_OF_FRAGMENTS            = NUMBER_OF_FRAGMENTS }; 
  enum { NUM_OF_SITES_PER_THERMOSTAT = 1 }; 
};

RunTimeGlobals< RTG_PARAMETERS > RTG;

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
#ifdef PK_PARALLEL
// If we're in phase 2 then all the energy emits are distributed.
// If we're in phase 1 then only some are distributed. So hence the UniContext distinction.
#ifdef PK_PHASE2
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
#define EmitTimeStamp(TimeStep, TypeTag, ToEmit)           \ 
 if( ToEmit )                                              \ 
 if( Platform::Topology::GetAddressSpaceId() == 0 ) \ 
   {                                                       \ 
   TimeValue TimeNow = Platform::Clock::GetTimeValue();    \ 
   ED_Emit_ControlTimeStamp(TimeStep, TypeTag, Platform::Topology::GetAddressSpaceId(), \ 
                            TimeNow.Seconds, TimeNow.Nanoseconds );                     \ 
   } 
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

class ImproperDihedralForceAccessor : public ParamAccessor_Base
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
                9,
                MSD::ImproperDihedralForce_sitesSiteTupleList,
                ImproperDihedralForceAccessor > ImproperDihedralForceUDF_Bound;

class StdHarmonicAngleForceAccessor : public ParamAccessor_Base
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
                131,
                MSD::StdHarmonicAngleForce_sitesSiteTupleList,
                StdHarmonicAngleForceAccessor > StdHarmonicAngleForceUDF_Bound;

class StdHarmonicBondForceAccessor : public ParamAccessor_Base
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
                41,
                MSD::StdHarmonicBondForce_sitesSiteTupleList,
                StdHarmonicBondForceAccessor > StdHarmonicBondForceUDF_Bound;

class SwopeTorsionForceAccessor : public ParamAccessor_Base
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
                144,
                MSD::SwopeTorsionForce_sitesSiteTupleList,
                SwopeTorsionForceAccessor > SwopeTorsionForceUDF_Bound;

class UreyBradleyForceAccessor : public ParamAccessor_Base
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
                54,
                MSD::UreyBradleyForce_sitesSiteTupleList,
                UreyBradleyForceAccessor > UreyBradleyForceUDF_Bound;

class Coulomb14Accessor : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::Coulomb14_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::Charge14PairTable[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_Coulomb14,
                177,
                MSD::Pair14SiteList,
                Coulomb14Accessor > Coulomb14UDF_Bound;

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
      int numberOfSiteTuples =  PAIR14_SITE_LIST_SIZE;
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
      static ParamType CombinedParameterArray[ PAIR14_SITE_LIST_SIZE ];
      if( SecretParameterArrayReturnField != (ParamType **)0xFFFFFFFF )
        {
        *SecretParameterArrayReturnField = CombinedParameterArray;
        return( CombinedParameterArray[ 0 ] );
        }
      return( CombinedParameterArray[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_LennardJones14Force,
                PAIR14_SITE_LIST_SIZE,
                MSD::Pair14SiteList,
                LennardJones14ParamAccessor<LennardJones14CHARMMCombiner > > LennardJones14UDF_Bound;




///----------------------------------------------------------------

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
        <  MDVM ,
           ImproperDihedralForceUDF_Bound,
           StdHarmonicAngleForceUDF_Bound,
           StdHarmonicBondForceUDF_Bound,
           SwopeTorsionForceUDF_Bound,
           UreyBradleyForceUDF_Bound,
           Coulomb14UDF_Bound,
           LennardJones14UDF_Bound
        > MSD_TYPED_STD_BondedForceDriver;  // This name is expected by code in main loop
typedef TupleDriverUdfBinder <UDF_WaterTIP3PInit,
                              7,
                              MSD::Water3sites,
                              NULL_ParamAccessor > WaterInitUDF_Bound;

typedef TupleDriverUdfBinder <UDF_WaterTIP3PShake,
                              7,
                              MSD::Water3sites,
                              NULL_ParamAccessor > WaterShakeUDF_Bound;

typedef TupleDriverUdfBinder <UDF_WaterTIP3PRattle,
                              7,
                              MSD::Water3sites,
                              NULL_ParamAccessor > WaterRattleUDF_Bound;


class RigidShake2Accessor : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::ShakeGroup2_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::Rigid2Params[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_ShakeGroup2,
                              17,
                              MSD::Rigid2Sites,
                              RigidShake2Accessor > RigidShake2UDF_Bound;


class RigidShake3Accessor : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::ShakeGroup3_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::Rigid3Params[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_ShakeGroup3,
                              6,
                              MSD::Rigid3Sites,
                              RigidShake3Accessor > RigidShake3UDF_Bound;


class RigidShake4Accessor : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::ShakeGroup4_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::Rigid4Params[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_ShakeGroup4,
                              2,
                              MSD::Rigid4Sites,
                              RigidShake4Accessor > RigidShake4UDF_Bound;


class RigidRattle2Accessor : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::ShakeGroup2_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::Rigid2Params[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_RattleGroup2,
                              17,
                              MSD::Rigid2Sites,
                              RigidRattle2Accessor > RigidRattle2UDF_Bound;


class RigidRattle3Accessor : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::ShakeGroup3_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::Rigid3Params[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_RattleGroup3,
                              6,
                              MSD::Rigid3Sites,
                              RigidRattle3Accessor > RigidRattle3UDF_Bound;


class RigidRattle4Accessor : public ParamAccessor_Base
  {
  public:

    typedef UDF_Binding::ShakeGroup4_Params ParamType;

    static
    ParamType
    GetParam( int i )
      {
      return( MSD::Rigid4Params[ i ] );
      }
  };

typedef TupleDriverUdfBinder< UDF_RattleGroup4,
                              2,
                              MSD::Rigid4Sites,
                              RigidRattle4Accessor > RigidRattle4UDF_Bound;

#include <BlueMatter/MDVM_UVP_IF.hpp>

typedef SiteTupleDriver
        <  MDVM_UVP
,        WaterInitUDF_Bound
        > MSD_TYPED_STD_GroupInitDriver;  // This name is expected by code in main loop

typedef SiteTupleDriver
        <  MDVM_UVP
,        WaterShakeUDF_Bound
,        RigidShake2UDF_Bound
,        RigidShake3UDF_Bound
,        RigidShake4UDF_Bound
        > MSD_TYPED_STD_GroupShakeDriver;  // This name is expected by code in main loop

typedef SiteTupleDriver
        <  MDVM_UVP
,        WaterRattleUDF_Bound
,        RigidRattle2UDF_Bound
,        RigidRattle3UDF_Bound
,        RigidRattle4UDF_Bound
        > MSD_TYPED_STD_GroupRattleDriver;  // This name is expected by code in main loop

#endif

#ifndef MSD_COMPILE_CODE_ONLY

const int MSD::NumberOfEnergyReportingUDFs = 11;

#endif

#ifndef MSD_COMPILE_DATA_ONLY

#include <BlueMatter/MSD_IF.hpp>

#include <BlueMatter/RunTimeGlobalsInit.hpp>

#include <pk/AtomicOps.hpp>
#include <pk/ThreadCreate.hpp>
#include <pk/Barrier.hpp>
using namespace std ;
#include <BlueMatter/PeriodicImage.hpp>

#include <BlueMatter/RandomVelocity.hpp>

XYZ  *StartingVelocities;
XYZ  *StartingPositions;
int NoseHooverRestart;
#include <BlueMatter/DVSFile.hpp>
#include <BlueMatter/IFP.hpp>
#include <BlueMatter/NSQ.hpp>
typedef IFP< NsqUdfGroup,
                         UDF_SwitchFunction,
                         UDF_SwitchFunctionForce,
                         NUMBER_OF_SHELLS,
                      1
                       > IFP_TYPE; 

typedef NSQAnchor<
                  DynamicVariableManager,
                  IFP_TYPE
                 > PNBE_TYPE;

PNBE_TYPE PNBE;


#include <BlueMatter/p2.hpp>

#endif
#endif
