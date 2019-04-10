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
 #define charge14scale  0.833333333333333
#define ChargeConversion  SciConst::AmberChargeConversion
#define TimeConversionFactor  SciConst::AmberTimeConversion


struct LennardJonesForce_Params
{
  double epsilon;
  double sigma;
};

struct StdHarmonicBondForce_Params
{
  double k;
  double r0;
};

struct StdHarmonicAngleForce_Params
{
  double k;
  double th0;
};

struct OPLSTorsionForce_Params
{
  double cosDelta;
  double k;
  double sinDelta;
  int n;
};

class MDProblemSpec
{
public:
static const UdfInvocation UdfInvocationTable[];
static const int UdfInvocationTableSize;

static const int SizeOfParams[];
static const SiteInfo SiteInformation[];
static const int SiteInformationSize;

static const int SiteIDtoSiteTypeMap[];
static const int SiteIDtoSiteTypeMapSize;

static const int SiteIDtoFragmentIDMap[];
static const int SiteIDtoFragmentIDMapSize;

static const LennardJonesForce_Params LJPairs[];
static const int LJPairsSize;

static const LennardJonesForce_Params LJ14Pairs[];
static const int LJ14PairsSize;

static const ExcludedPair14 Pair14List[];
static const int Pair14ListSize;

static const ExcludedPair14 ExcludedList[];
static const int ExcludedListSize;

static const IrreduciblePartition IrreduciblePartitionList[];
static const int IrreduciblePartitionListSize;

static const int IrreduciblePartitionCount;

static int StdHarmonicBondForce_sites[];
static  StdHarmonicBondForce_Params StdHarmonicBondForceParamTupleTable[];

static int StdHarmonicAngleForce_sites[];
static  StdHarmonicAngleForce_Params StdHarmonicAngleForceParamTupleTable[];

static int OPLSTorsionForce_sites[];
static  OPLSTorsionForce_Params OPLSTorsionForceParamTupleTable[];

};

/* 
The parameters are in the order of the structs above.
*/
StdHarmonicBondForce_Params MDProblemSpec::StdHarmonicBondForceParamTupleTable[] = {

{ 3.40000000E+02 , 1.09000000E+00} ,
{ 3.40000000E+02 , 1.09000000E+00} ,
{ 3.40000000E+02 , 1.09000000E+00} ,
{ 3.40000000E+02 , 1.09000000E+00} ,
{ 3.40000000E+02 , 1.09000000E+00} ,
{ 3.40000000E+02 , 1.09000000E+00} ,
{ 3.40000000E+02 , 1.09000000E+00} ,
{ 4.34000000E+02 , 1.01000000E+00} ,
{ 3.40000000E+02 , 1.09000000E+00} ,
{ 3.40000000E+02 , 1.09000000E+00} ,
{ 3.40000000E+02 , 1.09000000E+00} ,
{ 4.34000000E+02 , 1.01000000E+00} ,
{ 5.70000000E+02 , 1.22900000E+00} ,
{ 4.90000000E+02 , 1.33500000E+00} ,
{ 3.17000000E+02 , 1.52200000E+00} ,
{ 5.70000000E+02 , 1.22900000E+00} ,
{ 4.90000000E+02 , 1.33500000E+00} ,
{ 3.10000000E+02 , 1.52600000E+00} ,
{ 3.17000000E+02 , 1.52200000E+00} ,
{ 3.37000000E+02 , 1.44900000E+00} ,
{ 3.37000000E+02 , 1.44900000E+00} 
};

/* 
These are the sites.
*/
int MDProblemSpec::StdHarmonicBondForce_sites[] = {

1,2 ,
1,3 ,
0,1 ,
10,11 ,
10,12 ,
10,13 ,
8,9 ,
6,7 ,
18,19 ,
18,20 ,
18,21 ,
16,17 ,
4,5 ,
4,6 ,
1,4 ,
14,15 ,
14,16 ,
8,10 ,
8,14 ,
6,8 ,
16,18};

/* 
The parameters are in the order of the structs above.
*/
StdHarmonicAngleForce_Params MDProblemSpec::StdHarmonicAngleForceParamTupleTable[] = {

{ 3.00000000E+01 , (120*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 3.50000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 3.50000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 3.50000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 3.00000000E+01 , (120*Math::Deg2Rad)} ,
{ 3.50000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 3.50000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 3.50000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 3.00000000E+01 , (118.039999885408*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 3.50000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 3.50000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 3.50000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 3.00000000E+01 , (118.039999885408*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (109.5*Math::Deg2Rad)} ,
{ 8.00000000E+01 , (122.9*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (121.9*Math::Deg2Rad)} ,
{ 8.00000000E+01 , (120.4*Math::Deg2Rad)} ,
{ 7.00000000E+01 , (116.6*Math::Deg2Rad)} ,
{ 8.00000000E+01 , (122.9*Math::Deg2Rad)} ,
{ 5.00000000E+01 , (121.9*Math::Deg2Rad)} ,
{ 6.30000000E+01 , (111.1*Math::Deg2Rad)} ,
{ 8.00000000E+01 , (120.4*Math::Deg2Rad)} ,
{ 7.00000000E+01 , (116.6*Math::Deg2Rad)} ,
{ 8.00000000E+01 , (109.7*Math::Deg2Rad)} ,
{ 6.30000000E+01 , (110.1*Math::Deg2Rad)} 
};

/* 
These are the sites.
*/
int MDProblemSpec::StdHarmonicAngleForce_sites[] = {

4,6,7 ,
3,1,4 ,
2,1,3 ,
2,1,4 ,
0,1,2 ,
0,1,3 ,
0,1,4 ,
14,16,17 ,
12,10,13 ,
11,10,12 ,
11,10,13 ,
9,8,10 ,
9,8,14 ,
8,10,11 ,
8,10,12 ,
8,10,13 ,
7,6,8 ,
6,8,9 ,
20,18,21 ,
19,18,20 ,
19,18,21 ,
17,16,18 ,
16,18,19 ,
16,18,20 ,
16,18,21 ,
5,4,6 ,
4,6,8 ,
1,4,5 ,
1,4,6 ,
15,14,16 ,
14,16,18 ,
10,8,14 ,
8,14,15 ,
8,14,16 ,
6,8,10 ,
6,8,14};

/* 
The parameters are in the order of the structs above.
*/
OPLSTorsionForce_Params MDProblemSpec::OPLSTorsionForceParamTupleTable[] = {

{ cos(0*Math::Deg2Rad) , 2.00000000E+00 , sin(0*Math::Deg2Rad) , 1.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 2.50000000E+00 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 2.50000000E+00 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 2.00000000E+00 , sin(0*Math::Deg2Rad) , 1.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 2.50000000E+00 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 1.55555556E-01 , sin(0*Math::Deg2Rad) , 3.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 1.55555556E-01 , sin(0*Math::Deg2Rad) , 3.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 1.55555556E-01 , sin(0*Math::Deg2Rad) , 3.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 1.55555556E-01 , sin(0*Math::Deg2Rad) , 3.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 1.55555556E-01 , sin(0*Math::Deg2Rad) , 3.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 1.55555556E-01 , sin(0*Math::Deg2Rad) , 3.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 2.50000000E+00 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 1.55555556E-01 , sin(0*Math::Deg2Rad) , 3.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 1.55555556E-01 , sin(0*Math::Deg2Rad) , 3.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 1.55555556E-01 , sin(0*Math::Deg2Rad) , 3.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 2.50000000E+00 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 5.30000000E-01 , sin(0*Math::Deg2Rad) , 1.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 1.50000000E-01 , sin(180*Math::Deg2Rad) , 3.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 5.00000000E-01 , sin(180*Math::Deg2Rad) , 4.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 2.00000000E-01 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 2.50000000E+00 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 2.50000000E+00 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 7.00000000E-02 , sin(0*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(0*Math::Deg2Rad) , 1.00000000E-01 , sin(0*Math::Deg2Rad) , 4.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 2.50000000E+00 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 7.50000000E-01 , sin(180*Math::Deg2Rad) , 1.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 1.35000000E+00 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 4.00000000E-01 , sin(180*Math::Deg2Rad) , 4.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 1.00000000E+00 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 1.00000000E+00 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 1.05000000E+01 , sin(180*Math::Deg2Rad) , 2.00000000E+00} ,
{ cos(180*Math::Deg2Rad) , 1.05000000E+01 , sin(180*Math::Deg2Rad) , 2.00000000E+00} 
};

/* 
These are the sites.
*/
int MDProblemSpec::OPLSTorsionForce_sites[] = {

5,4,6,7 ,
5,4,6,7 ,
1,4,6,7 ,
15,14,16,17 ,
15,14,16,17 ,
13,10,8,14 ,
12,10,8,14 ,
11,10,8,14 ,
9,8,10,11 ,
9,8,10,12 ,
9,8,10,13 ,
8,14,16,17 ,
6,8,10,11 ,
6,8,10,12 ,
6,8,10,13 ,
5,4,6,8 ,
4,6,8,10 ,
4,6,8,10 ,
4,6,8,10 ,
4,6,8,14 ,
1,4,6,8 ,
15,14,16,18 ,
10,8,14,16 ,
10,8,14,16 ,
8,14,16,18 ,
6,8,14,16 ,
6,8,14,16 ,
6,8,14,16 ,
4,8,6,7 ,
14,18,16,17 ,
1,6,4,5 ,
8,16,14,15};

/* 
UdfCode, size of tupleList, siteTupleListPtr, paramTupleListPtr
*/

const UdfInvocation MDProblemSpec::UdfInvocationTable[] = 
{
{ 1 , 21 , (int*) StdHarmonicBondForce_sites , (void*)StdHarmonicBondForceParamTupleTable },
{ 2 , 36 , (int*) StdHarmonicAngleForce_sites , (void*)StdHarmonicAngleForceParamTupleTable },
{ 5 , 32 , (int*) OPLSTorsionForce_sites , (void*)OPLSTorsionForceParamTupleTable }
};

const int MDProblemSpec::UdfInvocationTableSize = 3;

const int MDProblemSpec::SizeOfParams[] = 
{
sizeof( StdHarmonicBondForce_Params ),
sizeof( StdHarmonicAngleForce_Params ),
sizeof( OPLSTorsionForce_Params )};

/* 
Indexed by site type  = { mass, halfInverseMass, charge, lj_type, lj_14_type };
*/

const SiteInfo MDProblemSpec::SiteInformation[] = 
{
{ 1.008 , 1.0/(2.0 * 1.008) , (    0.0603*ChargeConversion) , 0 , 0 },
{ 1.008 , 1.0/(2.0 * 1.008) , (    0.0823*ChargeConversion) , 6 , 6 },
{ 1.008 , 1.0/(2.0 * 1.008) , (    0.0976*ChargeConversion) , 6 , 6 },
{ 1.008 , 1.0/(2.0 * 1.008) , (    0.1123*ChargeConversion) , 0 , 0 },
{ 1.008 , 1.0/(2.0 * 1.008) , (    0.2719*ChargeConversion) , 5 , 5 },
{ 12.01 , 1.0/(2.0 * 12.01) , (    0.0337*ChargeConversion) , 1 , 1 },
{ 12.01 , 1.0/(2.0 * 12.01) , (    0.5972*ChargeConversion) , 2 , 2 },
{ 12.01 , 1.0/(2.0 * 12.01) , (    0.5973*ChargeConversion) , 2 , 2 },
{ 12.01 , 1.0/(2.0 * 12.01) , (   -0.1490*ChargeConversion) , 1 , 1 },
{ 12.01 , 1.0/(2.0 * 12.01) , (   -0.1825*ChargeConversion) , 1 , 1 },
{ 12.01 , 1.0/(2.0 * 12.01) , (   -0.3662*ChargeConversion) , 1 , 1 },
{ 14.01 , 1.0/(2.0 * 14.01) , (   -0.4157*ChargeConversion) , 4 , 4 },
{ 16 , 1.0/(2.0 * 16) , (   -0.5679*ChargeConversion) , 3 , 3 }
};

const int MDProblemSpec::SiteInformationSize = 13;

/* 
Mapping from SiteId to site type.
*/

const int MDProblemSpec::SiteIDtoSiteTypeMap[] = 
{
3, 
10, 
3, 
3, 
6, 
12, 
11, 
4, 
5, 
1, 
9, 
0, 
0, 
0, 
7, 
12, 
11, 
4, 
8, 
2, 
2, 
2};

const int MDProblemSpec::SiteIDtoSiteTypeMapSize = 22;

/* 
Every site is in its own fragment
*/

const int MDProblemSpec::SiteIDtoFragmentIDMap[] = 
{
0, 
1, 
2, 
3, 
4, 
5, 
6, 
7, 
8, 
9, 
10, 
11, 
12, 
13, 
14, 
15, 
16, 
17, 
18, 
19, 
20, 
21};

const int MDProblemSpec::SiteIDtoFragmentIDMapSize = 22;

/* 
An array of LJ Pairs
*/

const LennardJonesForce_Params MDProblemSpec::LJPairs[] = 
{
{0.0157000000262363 , 2.64953278726022},
{0.0414436968131308 , 3.02460114765057},
{0.109399999915728 , 3.39966950845074},
{0.0367450679577815 , 3.02460114621455},
{0.0969969070897875 , 3.39966950874072},
{0.0860000001283588 , 3.39966950794483},
{0.0574195093096127 , 2.80472734282863},
{0.151571765093351 , 3.17979570475948},
{0.134387499166528 , 3.17979570537529},
{0.209999999841822 , 2.95992190164469},
{0.0516623653716887 , 2.94976565593296},
{0.136374484426501 , 3.32483401597941},
{0.120913192035741 , 3.32483401597788},
{0.188944436449698 , 3.1049602120171},
{0.169999999917667 , 3.24999852403104},
{0.0156999999538954 , 1.85930562583042},
{0.0414436967992371 , 2.23437398498119},
{0.0367450677667984 , 2.23437398514522},
{0.0574195087769978 , 2.01450018167939},
{0.0516623656705171 , 2.15953849182152},
{0.0157000000042192 , 1.06907846172052},
{0.0156999999806046 , 2.56044291633196},
{0.041443696532826 , 2.93551127765429},
{0.0367450677213321 , 2.93551127615256},
{0.0574195088474234 , 2.71563747302357},
{0.0516623650350694 , 2.86067578549896},
{0.0156999999963813 , 1.77021575302184},
{0.0157000000984614 , 2.47135304264217}
};
const int MDProblemSpec::LJPairsSize = 28;

/* 
An array of LJ 1-4 Pairs
*/

const LennardJonesForce_Params MDProblemSpec::LJ14Pairs[] = 
{
{0.00785000001311814 , 2.64953278726022},
{0.0207218484065654 , 3.02460114765057},
{0.0546999999578639 , 3.39966950845074},
{0.0183725339788908 , 3.02460114621455},
{0.0484984535448938 , 3.39966950874072},
{0.0430000000641794 , 3.39966950794483},
{0.0287097546548063 , 2.80472734282863},
{0.0757858825466753 , 3.17979570475948},
{0.067193749583264 , 3.17979570537529},
{0.104999999920911 , 2.95992190164469},
{0.0258311826858444 , 2.94976565593296},
{0.0681872422132505 , 3.32483401597941},
{0.0604565960178706 , 3.32483401597788},
{0.0944722182248489 , 3.1049602120171},
{0.0849999999588335 , 3.24999852403104},
{0.00784999997694769 , 1.85930562583042},
{0.0207218483996185 , 2.23437398498119},
{0.0183725338833992 , 2.23437398514522},
{0.0287097543884989 , 2.01450018167939},
{0.0258311828352586 , 2.15953849182152},
{0.00785000000210962 , 1.06907846172052},
{0.00784999999030228 , 2.56044291633196},
{0.020721848266413 , 2.93551127765429},
{0.0183725338606661 , 2.93551127615256},
{0.0287097544237117 , 2.71563747302357},
{0.0258311825175347 , 2.86067578549896},
{0.00784999999819067 , 1.77021575302184},
{0.00785000004923071 , 2.47135304264217}
};

const int MDProblemSpec::LJ14PairsSize = 28;

const ExcludedPair14 MDProblemSpec::Pair14List[] = 
{
{ 0 , 5 },
{ 0 , 6 },
{ 1 , 7 },
{ 1 , 8 },
{ 2 , 5 },
{ 2 , 6 },
{ 3 , 5 },
{ 3 , 6 },
{ 4 , 9 },
{ 4 , 10 },
{ 4 , 14 },
{ 5 , 7 },
{ 5 , 8 },
{ 6 , 11 },
{ 6 , 12 },
{ 6 , 13 },
{ 6 , 15 },
{ 6 , 16 },
{ 7 , 9 },
{ 7 , 10 },
{ 7 , 14 },
{ 8 , 17 },
{ 8 , 18 },
{ 9 , 11 },
{ 9 , 12 },
{ 9 , 13 },
{ 9 , 15 },
{ 9 , 16 },
{ 10 , 15 },
{ 10 , 16 },
{ 11 , 14 },
{ 12 , 14 },
{ 13 , 14 },
{ 14 , 19 },
{ 14 , 20 },
{ 14 , 21 },
{ 15 , 17 },
{ 15 , 18 },
{ 17 , 19 },
{ 17 , 20 },
{ 17 , 21 }
};

const int MDProblemSpec::Pair14ListSize = 41;

const ExcludedPair14 MDProblemSpec::ExcludedList[] = 
{
{ 0 , 1 },
{ 0 , 2 },
{ 0 , 3 },
{ 0 , 4 },
{ 1 , 2 },
{ 1 , 3 },
{ 1 , 4 },
{ 1 , 5 },
{ 1 , 6 },
{ 2 , 3 },
{ 2 , 4 },
{ 3 , 4 },
{ 4 , 5 },
{ 4 , 6 },
{ 4 , 7 },
{ 4 , 8 },
{ 5 , 6 },
{ 6 , 7 },
{ 6 , 8 },
{ 6 , 9 },
{ 6 , 10 },
{ 6 , 14 },
{ 7 , 8 },
{ 8 , 9 },
{ 8 , 10 },
{ 8 , 11 },
{ 8 , 12 },
{ 8 , 13 },
{ 8 , 14 },
{ 8 , 15 },
{ 8 , 16 },
{ 9 , 10 },
{ 9 , 14 },
{ 10 , 11 },
{ 10 , 12 },
{ 10 , 13 },
{ 10 , 14 },
{ 11 , 12 },
{ 11 , 13 },
{ 12 , 13 },
{ 14 , 15 },
{ 14 , 16 },
{ 14 , 17 },
{ 14 , 18 },
{ 15 , 16 },
{ 16 , 17 },
{ 16 , 18 },
{ 16 , 19 },
{ 16 , 20 },
{ 16 , 21 },
{ 17 , 18 },
{ 18 , 19 },
{ 18 , 20 },
{ 18 , 21 },
{ 19 , 20 },
{ 19 , 21 },
{ 20 , 21 }
};

const int MDProblemSpec::ExcludedListSize = 57;

// All sites/molecules in one fragment

const IrreduciblePartition MDProblemSpec::IrreduciblePartitionList[] = 
{
{ 0 , 0 , 22}
};

const int MDProblemSpec::IrreduciblePartitionListSize = 1;

const int MDProblemSpec::IrreduciblePartitionCount = 1;

