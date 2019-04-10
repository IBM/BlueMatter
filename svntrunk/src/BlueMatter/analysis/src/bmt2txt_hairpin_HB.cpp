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
 
#include <math.h>
#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <cstdio>
#include <string>
//#include <math.h>
#define BUFFSIZE 2048

#define BEGINTRAJ "TRAJECTORYFORMAT"

using namespace std ;




#define DSSP_FACTOR (332.0 * 0.42 * 0.2)

int ReportAlphaHbonds(FILE *alphyPF,int NStep);
int ReportHbonds(FILE *hairyPF,int nstep);
int ReportAllHBondPairs(FILE *hairyPF, int nstep, int dolist);
int ReportAllDADistances(FILE *hairyPF, int nstep);
double RadiusOfGyration();
double CoreRadiusOfGyration();
double EndEndCADist();
char InputFName[512];
char InputFNameStem[512];
void FindSidechainContacts(int nstep, FILE *f);
void BuildResidueTable();

class Crd
{
public:
  double d_c[3];
  Crd(){d_c[0] = d_c[1] = d_c[2] = 0.0;}
  Crd(double x, double y, double z){d_c[0] = x; d_c[1] = y; d_c[2] = z;}
  double & operator[](int i) {return d_c[i];}
  Crd & operator=( Crd const & c){for(int i = 0;i<3;i++ ){d_c[i] = c.d_c[i];} return *this;}
  Crd & operator+= (Crd const & c){for(int i = 0;i<3;i++ ){d_c[i] += c.d_c[i];} return *this;}
  Crd & operator-= (Crd const & c){for(int i = 0;i<3;i++ ){d_c[i] -= c.d_c[i];} return *this;}
  Crd const  operator+ (Crd const & c){return Crd(d_c[0] + c.d_c[0],d_c[1] + c.d_c[1],d_c[2] + c.d_c[2]);}
  Crd const  operator- (Crd const & c){return Crd(d_c[0] - c.d_c[0],d_c[1] - c.d_c[1],d_c[2] - c.d_c[2]);}
  Crd & operator*= (double  c){for(int i = 0;i<3;i++ ){d_c[i] *= c;} return *this;}
  Crd & operator/= (double  c){for(int i = 0;i<3;i++ ){d_c[i] /= c;} return *this;}
  double SQDist(Crd &c)
  { 
    double dx = d_c[0] - c.d_c[0];
    double dy = d_c[1] - c.d_c[1];
    double dz = d_c[2] - c.d_c[2];
    return dx * dx + dy * dy + dz * dz;
  }
  double SQLength()
  { 
    double dx = d_c[0] * d_c[0]; 
    double dy = d_c[1] * d_c[1]; 
    double dz = d_c[2] * d_c[2]; 

    return dx + dy + dz;
  }
};
class Dssp_Atom
{
public:
  int d_id,d_elenum;
  Crd d_crd;
  Dssp_Atom(){}
  Dssp_Atom(int id, int elenum, Crd & r)
  {
    d_id = id; d_elenum = elenum; 
    d_crd = r;
  }
  Dssp_Atom &operator=(Dssp_Atom const &A)
  {
    d_id = A.d_id; d_elenum = A.d_elenum;
    for(int i = 0;i<3;i++){d_crd[i] = A.d_crd.d_c[i];}
    return *this;
  }
  Crd & Coord(){return d_crd;}
};


class Dssp_HBond
{
public:
  int d_valid,d_res_NH,d_res_CO;
  Dssp_Atom d_C,d_N,d_O,d_H;
  double d_DON,d_DCH,d_DOH,d_DCN,d_E;
  static float d_MaxBondEnergy;

  Dssp_HBond()
  {
      d_valid = 0;
  }

  int Init(int NHres, int COres, 
    Dssp_Atom &N,Dssp_Atom &H,Dssp_Atom &C,Dssp_Atom &O)
  {
    SetNHats(NHres,N,H);
    SetCOats(COres,C,O);
    return Compute();
  }

  static void SetMaxBondEnergy(float e) {d_MaxBondEnergy = e;}

  int Test()
  {
// ATOM     14  N   GLU    42       6.800   4.356   4.772               -.5000
// ATOM     15  H   GLU    42       6.238   4.442   5.607                .3000
// ATOM    234  C   THR    55       3.773   4.868   7.288                .5000
// ATOM    235  O   THR    55       4.772   4.702   6.592               -.5000

    Crd cN(6.800 ,  4.356 ,  4.772);
    Crd cH(6.238 ,  4.442 ,  5.607);
    Crd cC(3.773 ,  4.868 ,  7.288);
    Crd cO(4.772 ,  4.702 ,  6.592);
    Dssp_Atom N(1,7,cN);
    Dssp_Atom H(2,1,cH);
    Dssp_Atom C(3,6,cC);
    Dssp_Atom O(4,8,cO);
    Init(0,0,N,H,C,O);
    Compute();
    return IsHBonded();
  }

  int SetCOats(int COres,Dssp_Atom &C,Dssp_Atom &O)
  {
    d_res_CO = COres;
    d_C = C;    d_O = O;
    return 1;
  }
  int SetNHats(int NHres,Dssp_Atom &N,Dssp_Atom &H)
  {
    d_res_CO = NHres;
    d_N = N;    d_H = H;
    return 1;
  }

  int Compute()
  {
    d_DON = d_O.Coord().SQDist(d_N.Coord());
    d_DCH = d_C.Coord().SQDist(d_H.Coord());
    d_DOH = d_O.Coord().SQDist(d_H.Coord());
    d_DCN = d_C.Coord().SQDist(d_N.Coord());
    d_DON = 1.0/sqrt(d_DON);
    d_DCH = 1.0/sqrt(d_DCH);
    d_DOH = 1.0/sqrt(d_DOH);
    d_DCN = 1.0/sqrt(d_DCN);
    d_E = DSSP_FACTOR * (d_DON + d_DCH - d_DCN - d_DOH);
    d_valid = (d_E < d_MaxBondEnergy) ? 1 : 0;
    return d_valid;
  }

  int IsHBonded(){ return d_valid;}
  double HBondVal(){ return d_E;}

  int ResNumCO(){return d_res_CO;}
  int ResNumNH(){return d_res_NH;}

};

float Dssp_HBond::d_MaxBondEnergy = -0.5;

#define NHBONDS 14

Crd * Coords;
Dssp_Atom Nats[NHBONDS],Hats[NHBONDS],Cats[NHBONDS],Oats[NHBONDS];
Dssp_HBond hbonds[NHBONDS];
FILE *hairyPF;
FILE *alphyPF;

char PDBFileName[256];
int  NProteinAtoms = 0;
int  NNonWaterAtoms = 0;
int  NWaters = 0;
int  NWaterAtoms = 0;
int  NAtoms = 0;
int  NoWaters = 0;
char id[256];

int
main(int argc, char **argv, char **envp)
{

    char buff[BUFFSIZE];

    if (argc < 2) {
        cout << "bmt2txt BMTFileName <id>" << endl;
        return 1;
    }

    strcpy(InputFName, argv[1]);
    strcpy(InputFNameStem, argv[1]);
    char *p = strstr(InputFNameStem, ".bmt");
    if (p)
	*p = '\0';
    if (argc > 2)
	strcpy(id, argv[2]);
    else
	strcpy(id, "0");

    // cout << "Using id " << id << endl;
    
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
	cerr << "bmt2txt fopen failed on file " << argv[1] << endl;
	exit(-1);
    }
    fgets(buff, BUFFSIZE, f);
    if (strncmp(buff, "BMT", 3)) {
	cerr << "bmt2txt error in file " << argv[1] << " File header doesn't have BMT and instead has " << buff << endl;
	exit(-1);
    }
    fgets(buff, BUFFSIZE, f);
    while (strncmp(buff, BEGINTRAJ, strlen(BEGINTRAJ))) {
	if      (1 == sscanf(buff, "STRUCTURE %s\n", PDBFileName))
	    ;
	else if (1 == sscanf(buff, "NUMNONWATERATOMS %d\n", &NNonWaterAtoms))
	    ;
	else if (1 == sscanf(buff, "NUMWATERS %d\n", &NWaters))
	    ;
	else if (1 == sscanf(buff, "NOWATERS %d\n", &NoWaters))
	    ;
	fgets(buff, BUFFSIZE, f);
    }

    if (NoWaters)
	NWaters = 0;

    NAtoms = NNonWaterAtoms + 3 * NWaters;

    NProteinAtoms = NNonWaterAtoms - 3;  // Hairpin specific - remove ions

    NWaterAtoms = 3 * NWaters;

    float MemPerStep = NAtoms*3*4/1.0E6;
    
    bool bailed = false;
    int NStepsExpected = 100000000;

    ///// Init globals for Hbond analysis
    Coords = new Crd[NAtoms + 1];
    int j,nat,cat,hat,oat;
#define NENERGYSTEPS 4
   FILE *hbond[NENERGYSTEPS];
   FILE *allbond[NENERGYSTEPS];
   FILE *allbondlist;
   FILE *alldalist;
   FILE *allsclist;
   //hairyPF = fopen("hairpin_hbond.txt","w");
    // alphyPF = fopen("hairpin_alpha.txt","w");
    // assert(hairyPF);
    /////
   char hbfname[256];
   char dafname[256];
   char scfname[256];

   for (int en=0; en<NENERGYSTEPS; en++) {
       float ev = 0.5+0.5*en;
       sprintf(hbfname, "%s_%03.1f.hb", InputFNameStem, ev);
       hbond[en] = fopen(hbfname, "w");
       // fprintf(hbond[en], "%s\n", argv[1]);
       sprintf(hbfname, "%s_%03.1f.hba", InputFNameStem, ev);
       allbond[en] = fopen(hbfname, "w");
   }
       
   sprintf(hbfname, "%s.hbl", InputFNameStem);
   allbondlist = fopen(hbfname, "w");
   sprintf(dafname, "%s.dal", InputFNameStem);
   alldalist = fopen(dafname, "w");
   sprintf(scfname, "%s.sc", InputFNameStem);
   allsclist = fopen(scfname, "w");
   BuildResidueTable();

    for (int NStep = 0; NStep < NStepsExpected; NStep++) {

        float x0, y0, z0, s;
        // _fmode = _O_BINARY;
        fread(&x0, sizeof(float), 1, f);
        fread(&y0, sizeof(float), 1, f);
        fread(&z0, sizeof(float), 1, f);
        fread(&s,  sizeof(float), 1, f);

        if (feof(f) || ferror(f)) {
            bailed = true;
            goto error;
        }
            
        unsigned short sx, sy, sz;
        float x, y, z;
        
        for (int i=1; i<=NAtoms; i++) {
            fread(&sx, sizeof(short), 1, f);
            fread(&sy, sizeof(short), 1, f);
            fread(&sz, sizeof(short), 1, f);

            Coords[i][0] = x0 + sx * s;
            Coords[i][1] = y0 + sy * s;
            Coords[i][2] = z0 + sz * s;
        }
	j = 0;

	//	hbonds[0].Test();
	//	fprintf(hairyPF," Test Value %10.4f %2d \n",hbonds[j].HBondVal(), hbonds[j].IsHBonded()); 
        for (int estep = 0; estep < NENERGYSTEPS; estep++) {
            float e = -0.5 - 0.5*estep;
            Dssp_HBond::SetMaxBondEnergy(e);
	    ReportAllHBondPairs(allbond[estep], NStep, 0);
            ReportHbonds(hbond[estep], NStep);
            if (!estep) {
                ReportAllHBondPairs(allbondlist, NStep, 1);
                ReportAllDADistances(alldalist, NStep);
	    }
        }
	// ReportAlphaHbonds(alphyPF,NStep);
	FindSidechainContacts(NStep, allsclist);
    }

    error:

    delete [] Coords;
    // fclose(hairyPF);
    // fclose(alphyPF);
    fclose(allbondlist);
    fclose(alldalist);
    fclose(allsclist);
    for (int i=0; i<NENERGYSTEPS; i++) {
        // fprintf(hbond[i], "END\n");
        fclose(hbond[i]);
        fclose(allbond[i]);
    }


    fclose(f);
}


int ReportHbonds(FILE *hairyPF,int NStep)
{ 
  int nat,hat,cat,oat;
  static int HBats[NHBONDS][4] = 
  {//  nat , hat, cat , oat
    {14 , 15 , 234 , 235},
    {222 , 223 , 27 , 28},
    {53 , 54 , 204 , 205},
    {192 , 193 , 65 , 66},
    {88 , 89 , 170 , 171},
    {158 , 159 , 98 , 99},
    {136 , 137 , 110 , 111},
    { 112, 113, 134 ,135 },
    { 100, 101, 156, 157},
    { 172, 173, 86, 87},
    { 67, 68, 190, 191},
    { 206, 207, 51, 52},
    { 29, 30, 220, 221},
    { 236, 237, 12, 13}
  };

  static int SaltA = 152;
  static int SaltB[] = {25, 96, 108, 247};  // each has +1 also
  const int NSalts = sizeof(SaltB)/sizeof(int);
  int IonA = 257; 
  const int NIons = 3;
  int Salts[NSalts];
  int Ions[NIons];
  // fprintf(hairyPF,"%5d",NStep);
  double rg = RadiusOfGyration();
  double crg = CoreRadiusOfGyration();
  // double eed = EndEndCADist();
  fprintf(hairyPF,"%s %d %8.5f %8.5f", id, NStep, crg, rg);

  Crd pA = Coords[SaltA-1];

  // This finds salt bridges between SaltA and all SaltB's
  for (int p=0; p<NSalts; p++) {
      Salts[p] = 0;
      Crd pB0 = Coords[SaltB[p]-1];
      Crd pB1 = Coords[SaltB[p]];
      double d0 = pA.SQDist(pB0);
      double d1 = pA.SQDist(pB1);
      if (d0 < 4.0*4.0 || d1 < 4.0*4.0)
	Salts[p] = 1;
   }

  // This finds ion pairs between the 3 ions and the SaltB's
  for (int q=0; q<NIons; q++) {
      Crd pA = Coords[IonA+q-1];
      for (int p=0; p<NSalts; p++) {
	  Ions[p] = 0;
	  Crd pB0 = Coords[SaltB[p]-1];
	  Crd pB1 = Coords[SaltB[p]];
	  double d0 = pA.SQDist(pB0);
	  double d1 = pA.SQDist(pB1);
	  if (d0 < 4.0*4.0 || d1 < 4.0*4.0)
	    Ions[p] = 1;
       }
  }


  for(int j = 0;j< 6; j++)
    {
      nat = HBats[j][0];
      hat = HBats[j][1];
      cat = HBats[j][2];
      oat = HBats[j][3];
      Nats[j] = Dssp_Atom(nat,7,Coords[nat]);	Hats[j] = Dssp_Atom(hat,1,Coords[hat]);
      Cats[j] = Dssp_Atom(cat,6,Coords[cat]);	Oats[j] = Dssp_Atom(oat,8,Coords[oat]);
      hbonds[j].Init(0,0,Nats[j],Hats[j],Cats[j],Oats[j]);
      hbonds[j].Compute();
      fprintf(hairyPF, " %d", hbonds[j].IsHBonded());
      // fprintf(hairyPF,"  %5.3f %1d ",hbonds[j].HBondVal(), hbonds[j].IsHBonded()); 
    }
  fprintf(hairyPF, " %d %d %d %d %d %d %d %d", Salts[0], Salts[1], Salts[2], Salts[3], Ions[0], Ions[1], Ions[2], Ions[3]);
  fprintf(hairyPF,"\n");
  return 1;
}

// assumes H is N+1 and O is C+1
struct HBResidue {
  int ID;
  int N;
  int C;
};

int ReportAllHBondPairs(FILE *hairyPF,int NStep, int dolist)
{ 
  int nat,hat,cat,oat;
  static HBResidue res[] = 
  {//  resnum, n, c
      {41, 7, 12},
      {42, 14, 27},
      {43, 29, 51},
      {44, 53, 65},
      {45, 67, 86},
      {46, 88, 98},
      {47, 100, 110},
      {48, 112, 120},
      {49, 122, 134},
      {50, 136, 156},
      {51, 158, 170},
      {52, 172, 190},
      {53, 192, 204},
      {54, 206, 220},
      {55, 222, 234},
      {56, 236, 249}
  };

  int nres = sizeof(res)/sizeof(HBResidue);

  Dssp_Atom N, H, C, O;
  Dssp_HBond hb;

  char smallbuff[256];
  char linebuff[1024];
  *linebuff = '\0';
  int count=0;
  for (int i=0; i<nres; i++) {
      int n = res[i].N;
      int h = n+1;
      N=Dssp_Atom(n, 7, Coords[n]);
      H=Dssp_Atom(h, 1, Coords[h]);
      for (int j=0; j<nres; j++) {
          if (abs(i-j)<3)
              continue;
          int c = res[j].C;
          int o = c+1;
          C=Dssp_Atom(c, 6, Coords[c]);
          O=Dssp_Atom(o, 8, Coords[o]);
          hb.Init(0, 0, N, H, C, O);
          hb.Compute();
          if (hb.IsHBonded()) {
              if (dolist) {
                  fprintf(hairyPF, "%s %d %d %d %8.5f\n", id, NStep, i, j, hb.HBondVal());
              } else {
                  sprintf(smallbuff, " %d:%d", res[i].ID, res[j].ID);
                  strcat(linebuff, smallbuff);
                  count++;
              }
          }
      }
  }
          
  if (!dolist) {
      double rg = RadiusOfGyration();
      double crg = CoreRadiusOfGyration();
      fprintf(hairyPF,"%s %d %f %f %3d %s\n", id, NStep, rg, crg, count, linebuff);
  }

  return 1;
}


int ReportAllDADistances(FILE *hairyPF,int NStep)
{ 
  int nat, oat;
  static HBResidue res[] = 
  {//  resnum, n, c
      {41, 7, 12},
      {42, 14, 27},
      {43, 29, 51},
      {44, 53, 65},
      {45, 67, 86},
      {46, 88, 98},
      {47, 100, 110},
      {48, 112, 120},
      {49, 122, 134},
      {50, 136, 156},
      {51, 158, 170},
      {52, 172, 190},
      {53, 192, 204},
      {54, 206, 220},
      {55, 222, 234},
      {56, 236, 249}
  };

  int nres = sizeof(res)/sizeof(HBResidue);

  Dssp_Atom N, H, C, O;
  Dssp_HBond hb;

  char smallbuff[256];
  char linebuff[1024];
  *linebuff = '\0';
  int count=0;
  for (int i=0; i<nres; i++) {
      int n = res[i].N;
      int h = n+1;
      N=Dssp_Atom(n, 7, Coords[n]);
      // H=Dssp_Atom(h, 1, Coords[h]);
      for (int j=0; j<nres; j++) {
          if (abs(i-j)<3)
              continue;
          int c = res[j].C;
          int o = c+1;
          // C=Dssp_Atom(c, 6, Coords[c]);
          O=Dssp_Atom(o, 8, Coords[o]);
	  double d = sqrt(N.d_crd.SQDist(O.d_crd));
          if (d<8) {
                  fprintf(hairyPF, "%s %d %d %d %8.5f\n", id, NStep, i, j, d);
	  }
      }
  }
          
  return 1;
}


int ReportAlphaHbonds(FILE *alphyPF,int NStep)
{

#define NRES 16
  static int Nres[NRES][2] =
  {
    {7,41},
    {14,42},
    {29,43},
    {53,44},
    {67,45},
    {88,46},
    {100,47},
    {112,48},
    {122,49},
    {136,50},
    {158,51},
    {172,52},
    {192,53},
    {206,54},
    {222,55},
    {236,56}
  };
  static int Cres[NRES][2] =
  {
    {12,41},
    {27,42},
    {51,43},
    {65,44},
    {86,45},
    {98,46},
    {110,47},
    {120,48},
    {134,49},
    {156,50},
    {170,51},
    {190,52},
    {204,53},
    {220,54},
    {234,55},
    {249,56}
  };
  int j,nat1,hat1,oat1,cat1,nat2,hat2,oat2,cat2;
  fprintf(alphyPF,"%5d",NStep);
  Dssp_Atom Nat1,Hat1,Cat1,Oat1,Nat2,Hat2,Cat2,Oat2;
  Dssp_HBond hbd1,hbd2;

  int alphacounts[3];

  for(int k = 0; k < 3; k++)
    {
      alphacounts[k] = 0;

      for (int i = 0; i< NRES - 3 - k;i++)
	{
	  nat1 = Nres[i][0];
	  hat1 = Nres[i][0] + 1;
	  cat1 = Cres[i][0];
	  oat1 = Cres[i][0] + 1;
	  
	  j = i+3 + k;

	  nat2 = Nres[j][0];
	  hat2 = Nres[j][0] + 1;
	  cat2 = Cres[j][0];
	  oat2 = Cres[j][0] + 1;

	  Nat1 = Dssp_Atom(nat1,7,Coords[nat1]);    Hat1 = Dssp_Atom(hat1,1,Coords[hat1]);
	  Cat1 = Dssp_Atom(cat1,6,Coords[cat1]);    Oat1 = Dssp_Atom(oat1,8,Coords[oat1]);
	  Nat2 = Dssp_Atom(nat2,7,Coords[nat2]);    Hat2 = Dssp_Atom(hat2,1,Coords[hat2]);
	  Cat2 = Dssp_Atom(cat2,6,Coords[cat2]);    Oat2 = Dssp_Atom(oat2,8,Coords[oat2]);
	  hbd1.Init(0,0,Nat1,Hat1,Cat2,Oat2);
	  hbd1.Compute();
	  hbd2.Init(0,0,Nat2,Hat2,Cat1,Oat1);
	  hbd2.Compute();
	  fprintf(alphyPF,"  %5.3f %5.3f   ",hbd1.HBondVal(),hbd2.HBondVal() );
	  
	  alphacounts[k] += hbd1.IsHBonded() + hbd2.IsHBonded();
	}
      fprintf(alphyPF," %10d ", alphacounts[k]);
    }

  fprintf(alphyPF,"\n");
  return 1;


  return 1;
}

// This is a list of 1-based residue heavy side chain atoms
// The correspondence with residue is in the next array
static int HSCAtoms[] = {
  18,
  21,
  24,
  25,
  26,
  33,
  36,
  37,
  39,
  41,
  42,
  44,
  46,
  48,
  50,
  57,
  59,
  63,
  71,
  74,
  75,
  77,
  79,
  80,
  82,
  84,
  92,
  95,
  96,
  97,
 104,
 107,
 108,
 109,
 116,
 126,
 128,
 132,
 140,
 143,
 146,
 149,
 152,
 162,
 164,
 168,
 176,
 179,
 180,
 182,
 184,
 186,
 188,
 196,
 198,
 202,
 210,
 212,
 216,
 226,
 228,
 232,
 240,
 243,
 246,
 247,
 248
};

// This is the residue number of the above atoms.  Res 42 has 5 heavy side chain atoms
static int HSCResidues[] = {
 42,
 42,
 42,
 42,
 42,
 43,
 43,
 43,
 43,
 43,
 43,
 43,
 43,
 43,
 43,
 44,
 44,
 44,
 45,
 45,
 45,
 45,
 45,
 45,
 45,
 45,
 46,
 46,
 46,
 46,
 47,
 47,
 47,
 47,
 48,
 49,
 49,
 49,
 50,
 50,
 50,
 50,
 50,
 51,
 51,
 51,
 52,
 52,
 52,
 52,
 52,
 52,
 52,
 53,
 53,
 53,
 54,
 54,
 54,
 55,
 55,
 55,
 56,
 56,
 56,
 56,
 56
};

static int NHSCAtoms = sizeof(HSCResidues)/sizeof(int);

struct HSC {
    int *Atoms;
    int NAtoms;
    int ResNum;
};

static int NHSCRes = 0;

HSC HSCRes[100];

void BuildResidueTable()
{
    int currentres = HSCResidues[0];
    int natoms = 0;
    HSCRes[0].Atoms = &HSCAtoms[0];
    HSCRes[0].ResNum = HSCResidues[0];
    for (int i=0; i<NHSCAtoms; i++) {
	if (HSCResidues[i] != currentres) {
	    HSCRes[NHSCRes++].NAtoms = natoms;
	    currentres = HSCResidues[i];
	    HSCRes[NHSCRes].ResNum = currentres;
	    HSCRes[NHSCRes].Atoms = &HSCAtoms[i];
	    natoms = 1;
	} else {
	    natoms++;
	}
    }
    HSCRes[NHSCRes++].NAtoms = natoms;
    // for (int j=0; j<NHSCRes; j++) {
    //	cout << j << " " << HSCRes[j].Atoms[0] << " " << HSCRes[j].NAtoms << " " << HSCRes[j].ResNum << endl;
    // }
}

void FindSidechainContacts(int nstep, FILE *f)
{
    HSC *rA, *rB;
    for (int i=0; i<NHSCRes; i++) {
	rA = &HSCRes[i];
	for (int j=i+1; j<NHSCRes; j++) {
	    rB = &HSCRes[j];
	    int nc=0;
	    double strength = 0;
	    double dmin = 10000000.0;
	    for (int p=0; p<rA->NAtoms; p++) {
		Crd a = Coords[rA->Atoms[p]-1];
		for (int q=0; q<rB->NAtoms; q++) {
		    Crd b = Coords[rB->Atoms[q]-1];
		    double d = a.SQDist(b);
		    if (d < dmin)
			dmin = d;
		    if (d < 4.0*4.0) {
			nc++;
			strength += 1.0/d;
		    }
		}
	    }
	    if (nc > 0) {
		//cout << nstep << " " << i << ":" << j << " " << nc << endl;
		fprintf(f, "%s %d %d %d %d %f\n", id, nstep, HSCRes[i].ResNum, HSCRes[j].ResNum, nc, float(nc)/(HSCRes[i].NAtoms*HSCRes[j].NAtoms));
	    }
	}
    }
}

double RadiusOfGyration()
{
  int n = NProteinAtoms;
  Crd cm,v,zero;
  double rg = 0.0;
  for (int i=1; i<=n; i++) 
    {
      cm += Coords[i];
    }
  cm /= n;
  for (int i=1; i<=n; i++) 
    {
      v = Coords[i] - cm;
      rg += v.SQLength();
    }
  rg = sqrt(rg/n);
  return rg;
}

double CoreRadiusOfGyration()
{
#if 0
  // This is the list of ALL atoms on the 4 core residues, rather than all heavy sidechain atoms
  static int coreats[] =
  {29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,172,173,174,175,176,177,178,179,180, 
   181,182,183,184,185,186,187,188,189,190,191,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,
   206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221};
#endif

  static int coreats[] =
  { 33, 36, 37, 39, 41, 42, 44, 46, 48, 50,
    71, 74, 75, 77, 79, 80, 82, 84,
    176, 179, 180, 182, 184, 186, 188, 
    210, 212, 216 };


  static int ncoreats = sizeof (coreats) / sizeof (int);
  Crd cm,v,zero;
  double rg = 0.0;
  for (int i=0; i<ncoreats; i++) 
    {
      cm += Coords[coreats[i]];
    }
  cm /= ncoreats;
  for (int i=0; i< ncoreats; i++) 
    {
      v = Coords[coreats[i]] - cm;
      rg += v.SQLength();
    }

  rg = sqrt(rg/ncoreats);
  return rg;
}

double EndEndCADist()
{
  int a1 = 9,a2 = 238;
  Crd v = Coords[a1] - Coords[a2];
  double r = sqrt(v.SQLength());
  return r;
}

///////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////
///         Least Squares stuff
///////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////

#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

class MVError {
	private:
		int index;
	public:
		MVError(int indx, char* amsg):index(indx) {
			cerr << amsg << endl;
			cerr << "Out of range: " << index << endl;
		};
};

class Matrix;

class Vector {
	
	public:

		Vector();
		Vector(const double&, const double&, const double&);
		Vector(const Vector&);

		// vector addition
		friend class Vector operator+(const Vector&, const Vector&);
		// unary +
		friend class Vector operator+(const Vector&);
		// vector subtraction
		friend class Vector operator-(const Vector&, const Vector&);
		// unary -
		friend class Vector operator-(const Vector&); 
		// cross product
		friend class Vector operator*(const Vector&, const Vector&); 
		// dot product
		friend double operator/(const Vector&, const Vector&);
		// scalar-vector product
		friend class Vector operator*(const double&, const Vector&);
		// scalar-vector product
		friend class Vector operator*(const Vector&, const double&);
		// scalar-vector product
		friend class Vector operator/(const Vector&, const double&);

		// Assignment
		const Vector& operator=(const Vector&);
		// Accumulate
		const Vector& operator+=(const Vector&);
		// Accumulate neg.
		const Vector& operator-=(const Vector&);
		// Accumulate prod.
		const Vector& operator*=(const double&);
		// Accumulate prod.
		const Vector& operator/=(const double&);

		// outer product
		friend class Matrix operator&(const Vector&, const Vector&);
		// inner product
		friend class Vector operator/(const Matrix&, const Vector&);
		// inner product
		friend class Vector operator/(const Vector&, const Matrix&);

		friend class Matrix;

		// Matrix Addition
		friend class Matrix operator+(const Matrix&, const Matrix&);
		// Unary +
		friend class Matrix operator+(const Matrix&);
		// Matrix Subtraction
		friend class Matrix operator-(const Matrix&, const Matrix&);
		// Unary -
		friend class Matrix operator-(const Matrix&); 
		friend class Matrix operator/(const Matrix&, const Matrix&); 
		// Scalar-matrix product
		friend class Matrix operator*(const Matrix&, const double&); 
		// Scalar-matrix product
		friend class Matrix operator*(const double&, const Matrix&); 
	
		friend class Matrix VX1(const Vector&);

		double& operator[](int i) 
		{
			if (i < 0 || i >= 3) {
				// cerr << form("index = %d out of range.", i) << endl;
				assert(0);
				return component[0];
			} else {
				return component[i];
			}
		};
		const double& operator[](int i) const 
		{
			if (i < 0 || i >= 3) {
				// cerr << form("index = %d out of range.", i) << endl;
				assert(0);
				return component[0];
			} else {
				return component[i];
			}
		};

	private:

		double component[3];

};

class Matrix {

	public:

		Matrix();
		Matrix(const double&, const double&, const double&,
			const double&, const double&, const double&,
			const double&, const double&, const double&);
		Matrix(const Matrix&);

		// Matrix Addition
		friend class Matrix operator+(const Matrix&, const Matrix&);
		// Unary +
		friend class Matrix operator+(const Matrix&);
		// Matrix Subtraction
		friend class Matrix operator-(const Matrix&, const Matrix&);
		// Unary -
		friend class Matrix operator-(const Matrix&); 
		// outer product
		friend class Matrix operator&(const Vector&, const Vector&); 
		// inner product
		friend class Vector operator/(const Matrix&, const Vector&); 
		// inner product
		friend class Vector operator/(const Vector&, const Matrix&); 
		// inner product
		friend class Matrix operator/(const Matrix&, const Matrix&); 
		// Scalar-matrix product
		friend class Matrix operator*(const Matrix&, const double&); 
		// Scalar-matrix product
		friend class Matrix operator/(const Matrix&, const double&); 
		// Scalar-matrix product
		friend class Matrix operator*(const double&, const Matrix&); 

		// Transpose
		Matrix operator!()const;

		// Trace
		friend double Tr(const Matrix& m);

		// Determinant
		friend double Det(const Matrix& m);

		// Assignment
		const Matrix& operator=(const Matrix& m2);
		// Accumulate
		const Matrix& operator+=(const Matrix& m2);
		// neg Accumulate
		const Matrix& operator-=(const Matrix& m2);
		// Accumulate
		const Matrix& operator/=(const double& m2);
		// Accumulate
		const Matrix& operator*=(const double& m2);

		friend class Matrix VX1(const Vector&);

		friend class Vector;

		Vector& operator[](int i) {
			if (i < 0 || i >= 3) {
				// cerr << form("index = %d out of range.", i) << endl;
				assert(0);
				return component[0];
			} else {
				return component[i];
			}
		};

		const Vector& operator[](int i) const 
		{
			if (i < 0 || i >= 3) {
				// cerr << form("index = %d out of range.", i) << endl;
				assert(0);
				return component[0];
			} else {
				return component[i];
			}
		};

		void Diag(double[3], Vector[3]) const;

	private:
		Vector component[3];
};

inline Vector operator+(const Vector& v1, const Vector& v2)
{
	return Vector(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
}

inline Vector operator+(const Vector& v)
{
	return Vector(v[0], v[1], v[2]);
}

inline Vector operator-(const Vector& v1, const Vector& v2)
{
	return Vector(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
}

inline Vector operator-(const Vector& v)
{
	return Vector(-v[0], -v[1], -v[2]);
}

inline Vector operator*(const Vector& v1, const Vector& v2)
{
	return Vector(
		v1[1] * v2[2] - v1[2] * v2[1],
		v1[2] * v2[0] - v1[0] * v2[2],
		v1[0] * v2[1] - v1[1] * v2[0]
	);
}

inline Vector operator*(const Vector& v, const double& d)
{
	return Vector(d * v[0], d * v[1], d * v[2]);
}

inline Vector operator/(const Vector& v, const double& d)
{
	return Vector(v[0] / d, v[1] / d, v[2] / d);
}

inline Vector operator*(const double& d, const Vector& v)
{
	return Vector(d * v[0], d * v[1], d * v[2]);
}

inline double operator/(const Vector& v1, const Vector& v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

inline const Vector& Vector::operator=(const Vector& v2)
{
	(*this)[0] = v2[0]; (*this)[1] = v2[1]; (*this)[2] = v2[2];
	return *this;
}

inline const Vector& Vector::operator+=(const Vector& v2)
{
	(*this)[0] += v2[0]; (*this)[1] += v2[1]; (*this)[2] += v2[2];
	return *this;
}

inline const Vector& Vector::operator*=(const double& d)
{
	(*this)[0] *= d; (*this)[1] *= d; (*this)[2] *= d;
	return *this;
}

inline const Vector& Vector::operator/=(const double& d)
{
	(*this)[0] /= d; (*this)[1] /= d; (*this)[2] /= d;
	return *this;
}

inline const Vector& Vector::operator-=(const Vector& v2)
{
	(*this)[0] -= v2[0]; (*this)[1] -= v2[1]; (*this)[2] -= v2[2];
	return *this;
}


inline double mag(const Vector& v)
{
	return sqrt(v/v);
}

inline Matrix VX1(const Vector& v) 
{
	return Matrix(
		0.0, - v[2], v[1],
		v[2], 0.0, - v[0],
		-v[1], v[0], 0.0
	);
}

ostream &operator << (ostream &s, const Matrix &m);

ostream &operator << (ostream &s, const Vector &v);

const Vector X(1.0, 0.0, 0.0);
const Vector Y(0.0, 1.0, 0.0);
const Vector Z(0.0, 0.0, 1.0);
const Matrix I = (X&X) + (Y&Y) + (Z&Z);
#endif



// -------------------------------------------------------------
#ifndef __DIAG_HPP__
#define __DIAG_HPP__

void eigsrt(double* const d, double * const * const v, const int& n);

void tred2(double * const * const a, double * const d, double * const e,
	const int& n);

int tqli(double * const d, double * const e, double * const * const z,
	const int& n);

void Eigen(double * const * const a, double* const e, double * const * const v,
	const int& n);

void Svd(double * const * const a, double * const * const u,
	double * const * const s, double * const * const v,
	const int& m, const int& n);

#endif

// ---------------------------------------------
#ifndef __MATINV_HPP__
#define  __MATINV_HPP__

void ludcmp(double * const * const a, int * const indx, double& d,
	const int& n);
void lubksb(double * const * const a, int * const indx, double * const b, 
	const int& n);
void matinv(double * const * const a, double * const * const y, 
	double& d, const int& n);

#endif


#define EPS 1.0e-6


int 
lstSqAlign(const Vector x[], const Vector y[], const int& n, Matrix& R, Vector& d);


///////////////////////////////////////////////////////////////////

Vector::Vector(){component[0] = component[1] = component[2] = 0.0;}

Vector::Vector(const double& x, const double& y, const double& z) 
{
	component[0] = x; component[1] = y; component[2] = z;
}

Vector::Vector(const Vector& v) {
	component[0] = v[0]; component[1] = v[1]; component[2] = v[2];
}


Matrix::Matrix()
{
};

Matrix::Matrix(
	const double& xx, const double& xy, const double& xz, 
	const double& yx, const double& yy, const double& yz, 
	const double& zx, const double& zy, const double& zz)
{
	component[0] = Vector(xx, xy, xz);
	component[1] = Vector(yx, yy, yz);
	component[2] = Vector(zx, zy, zz);
};

Matrix::Matrix(const Matrix& m)
{
	component[0][0] = m[0][0];
	component[0][1] = m[0][1];
	component[0][2] = m[0][2];

	component[1][0] = m[1][0];
	component[1][1] = m[1][1];
	component[1][2] = m[1][2];
	
	component[2][0] = m[2][0];
	component[2][1] = m[2][1]; 
	component[2][2] = m[2][2];
}


Vector operator/(const Vector& v, const Matrix& m) 
{
	Vector vRes;
	int i, j;
	for (i = 0; i < 3; i++) {
		for (vRes[i] = 0.0, j = 0; j < 3; j++) {
			vRes[i] += v[j] * m[j][i];
		}
	}
	return vRes;
}

Vector operator/(const Matrix& m, const Vector& v)
{
	Vector vRes;
	int i, j;
	for (i = 0; i < 3; i++) {
		for (vRes[i] = 0.0, j = 0; j < 3; j++) {
			vRes[i] += m[i][j] * v[j];
		}
	}
	return vRes;
}

Matrix operator+(const Matrix& m1, const Matrix& m2)
{
	Matrix mRes;
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			mRes[i][j] = m1[i][j] + m2[i][j];
		}
	}
	return mRes;
}

Matrix operator+(const Matrix& m)
{
	Matrix mRes;
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			mRes[i][j] = m[i][j];
		}
	}
	return mRes;
}

Matrix operator-(const Matrix& m1, const Matrix& m2)
{
	Matrix mRes;
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			mRes[i][j] = m1[i][j] - m2[i][j];
		}
	}
	return mRes;
}

Matrix operator-(const Matrix& m)
{
	Matrix mRes;
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			mRes[i][j] = - m[i][j];
		}
	}
	return mRes;
}

Matrix operator/(const Matrix& m1, const Matrix& m2)
{
	Matrix mRes;
	int i, j, k;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			for (mRes[i][j] = 0.0, k = 0; k < 3; k++) {
				mRes[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
	return mRes;
}

Matrix operator*(const Matrix& m, const double& d) 
{
	Matrix mRes;
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			mRes[i][j] = d * m[i][j];
		}
	}
	return mRes;
}

Matrix operator/(const Matrix& m, const double& d) 
{
	Matrix mRes;
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			mRes[i][j] = m[i][j] / d;
		}
	}
	return mRes;
}

Matrix operator*(const double& d, const Matrix& m) 
{
	Matrix mRes;
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			mRes[i][j] = d * m[i][j];
		}
	}
	return mRes;
}

const Matrix& 
Matrix::operator=(const Matrix& m2) 
{
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			(*this)[i][j] = m2[i][j];
		}
	}
	return (*this);
}

const Matrix& 
Matrix::operator+=(const Matrix& m2) 
{
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			(*this)[i][j] += m2[i][j];
		}
	}
	return (*this);
}

const Matrix& 
Matrix::operator*=(const double& d) 
{
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			(*this)[i][j] *= d;
		}
	}
	return (*this);
}

const Matrix& 
Matrix::operator/=(const double& d) 
{
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			(*this)[i][j] /= d;
		}
	}
	return (*this);
}

const Matrix& 
Matrix::operator-=(const Matrix& m2) 
{
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			(*this)[i][j] -= m2[i][j];
		}
	}
	return (*this);
}

Matrix operator&(const Vector& v1, const Vector& v2)
{
	int i, j;
	Matrix mRes;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			mRes[i][j] = v1[i] * v2[j];
		}
	}
	return mRes;
}
			
Matrix 
Matrix::operator!()const
{
	Matrix mRes;
	int i, j;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			mRes[i][j] = (*this)[j][i];
		}
	}
	return mRes;
}

double
Tr(const Matrix& m)
{
	double mRes;
	int i, j;

	for (mRes = 0.0, i = 0; i < 3; i++) {
		mRes += m[i][i];
	}
	return mRes;
}

double Det(const Matrix& m)
{
  // * is cross product, / is scalar product
  return (m[0] * m[1]) / m[2];
}

void 
Matrix::Diag(double eigvals[3], Vector eigvecs[3]) const
{
	static double d[3];
	static double e[3];
	static double a0[3], a1[3], a2[3];
	static double* a[3] = {a0, a1, a2};
	int i, j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			a[i][j] = (*this)[i][j];

	tred2(a, d, e, 3);
	tqli(d, e, a, 3);
	eigsrt(d, a, 3);

	for (i = 0; i < 3; i++) {
		for (eigvals[i] = d[i], j = 0; j < 3; j++) {
			eigvecs[i][j] = a[j][i];
		}
	}
}
//--------------------------------------------------
// << operator for Matrix
ostream &operator << (ostream &s, const Matrix &m)
{
  s.setf(ios::fixed,ios::floatfield);
  for (int i=0; i<3; i++) 
    for (int j=0; j<3; j++) 
      s << m[i][j] << ((j==2)?'\n':' ');
  return s;
}

// << operator for Vector
ostream &operator << (ostream &s, const Vector &v)
{
  s.setf(ios::fixed,ios::floatfield);
  s << v[0] << ' ' << v[1] << ' ' << v[2];
  return s;
}


#define MAX_ITTER 1000
#define __COMPLAIN_ABOUT_ITTER__

/****************************************************
**	eigsrt sorts the n eigenvalues d[]
**	and n eigenvectors v[][].
****************************************************/

void eigsrt(double * const d, double * const * const v, const int& n)
{
	int		i, j, k;
	double	p;

	for (i = 0; i < n - 1; i++) {

		for (k = i, p = d[i], j = i + 1; j < n; j++) 
			if (d[j] <= p) 
				p = d[k = j];

		if (k != i) 
			for (d[k] = d[i], d[i] = p, j = 0; j < n; j++) 
				p = v[j][i], v[j][i] = v[j][k], v[j][k] = p;

	}
}

void eigsrt2(double * const d, double * const * const v, const int& n)
{
	int		i, j, k;
	double	p;

	for (i = 0; i < n - 1; i++) {

		for (k = i, p = d[i], j = i + 1; j < n; j++) 
			if (d[j] >= p) 
				p = d[k = j];

		if (k != i) 
			for (d[k] = d[i], d[i] = p, j = 0; j < n; j++) 
				p = v[j][i], v[j][i] = v[j][k], v[j][k] = p;

	}
}


/********************************************************
**	tred2 effects the Housholder tri-diagonalization of
**	nXn matrix a[][].  a[][] is replaced by the orthogonal
**	matrix q[][] that effects the transformation on a,
**	d[] returns the diagonal components, and e[] returns
**	the off-diagonal components with e[0] = 0.0.
********************************************************/

void tred2(double * const * const a, double * const d, double * const e, 
	const int& n)
{
	int		i, j, k, l;
	double	h, scale,
	f, g, hh;

	for (i = n - 1; i >= 1; i--) { 

		l = i - 1; 
		h = scale = 0.0;

		if (l > 0) {

			for (k = 0; k <= l; k++) 
				scale += fabs(a[i][k]);

			if (scale == 0.0) {

				e[i] = a[i][l];

			} else {

				for (k = 0; k <= l; k++) {
					a[i][k] /= scale;
					h += a[i][k] * a[i][k];
				}

				f = a[i][l];
				g = ((f > 0.0) ? (-sqrt(h)) : sqrt(h)); 
				e[i] = scale * g;
				h -= f * g;
				a[i][l] = f - g;

				for (f = 0.0, j = 0; j <= l; j++) {

					a[j][i] = a[i][j] / h;

					for (g = 0.0, k = 0; k <= j; k++)
						g += a[j][k] * a[i][k];

					for (k = j + 1; k <= l; k++)
						g += a[k][j] * a[i][k];

					e[j] = g / h;
					f += e[j] * a[i][j];

				}

				for (hh = f / (h + h), j = 0; j <= l; j++) {

					f = a[i][j];
					g = e[j] - hh * f;
					e[j] = g;

					for (k = 0; k <= j; k++)
						a[j][k] += - f * e[k] - g * a[i][k];

				}
				
			}

		} else {

			e[i] = a[i][l];

		}

		d[i] = h;

	}

	e[0] = d[0] = 0.0;

	for (i = 0; i < n; i++) {

		l = i - 1;

		if (d[i] != 0.0) {

			for (j = 0; j <= l; j++) {

				for (g = 0.0, k = 0; k <= l; k++)
					g+= a[i][k] * a[k][j];

				for (k = 0; k <= l; k++)
					a[k][j] -= g * a[k][i];

			}
		}

		d[i] = a[i][i];
		a[i][i] = 1.0;

		for (j = 0; j <= l; j++)
			a[i][j] = a[j][i] = 0.0;
	}

	return;
}

/************************************************************
**	tqli computes eigenvalues and eigenvectors of
**	a real symmetric matrix.
**
**	d[] and e[] come from tred2, z[][] input contains the
**	results stored in a[][] that came from tred2, and
**	returns the eigenvectors of what was in a[][] in input
**	to tred2.
************************************************************/

int
tqli(double * const d, double * const e, double * const * const z, 
	const int& n)
{
	int		i, k, l, m;
	int		itter, iflag, iflag2;
	double	dd, r, g, s, c, p, f, b;

	for (i = 1; i < n; i++) 
		e[i-1] = e[i];

	e[n-1] = 0.0;

	for (l = 0; l < n; l++) {

		itter = 0;

		do {

			iflag2 = 0;

			for (iflag = 0, m = l; m < n - 1; m++) {

				dd = fabs(d[m]) + fabs(d[m + 1]);

				if (fabs(e[m]) + dd == dd) {
					iflag = 1; break;
				}

			}

			if (!iflag)
				m = n - 1;

			if (m != l) {

				if (itter >= MAX_ITTER) {
				  // #ifdef __COMPLAIN_ABOUT_ITTER__
					// cerr << form("itter = %d > MAX_ITTER = %d in tqli",
					// 	itter, MAX_ITTER) << endl;
					//#endif
					return 0;
				}

				itter++;

				g = (d[l + 1] - d[l]) / (2.0 * e[l]);
				r = sqrt(g * g + 1.0);
				g = d[m] - d[l] + e[l] / (g + ((g > 0.0) ? r : (-r)));
				s = c = 1.0; p = 0.0;

				for (i = m - 1; i >= l; i--) {

					f = s * e[i]; b = c * e[i];

					if (fabs(f) >= fabs(g)) {

						c = g / f;
						r = sqrt(c * c + 1.0);
						e[i + 1] = f * r;
						s = 1.0 / r;
						c *= s;

					} else {

						s = f / g;
						r = sqrt(s * s + 1.0);
						e[i + 1] = g * r; 
						c = 1.0 / r;
						s *= c;

					}
					
					g = d[i + 1] - p;
					r = (d[i] - g) * s + 2.0 * c * b;
					p = s * r;
					d[i + 1] = g + p;
					g = c * r - b;

					for (k = 0; k < n; k++) {

						f = z[k][i + 1];
						z[k][i + 1] = s * z[k][i] + c * f;
						z[k][i] = c * z[k][i] - s * f;

					}

				}

				d[l] -= p; e[l] = g; e[m] = 0.0;
				iflag2 = 1;
			} 

		} while (iflag2);

	}
	return 1;
}

/*************************************************************************
**
**	Given a[.][.] an n x n matrix, compute e[.], an array of eigenvalues
**	of length n, and v[.][.], an array of vectors n x n containing the
**	eigenvectors (backwards by some conventions).
**
*************************************************************************/

void Eigen(double * const * const a, double * const e, 
	double * const * const v, const int& n)
{
	double** c;
	double* c1;
	double* c2;
	int i, j;

	c = new double*[n];
	c1 = new double[n];
	c2 = new double[n];
	for (i = 0; i < n; i++) {
		c[i] = new double[n];
		for (j = 0; j < n; j++) {
			c[i][j] = a[i][j];
		}
	}

	tred2(c, c1, c2, n);
	if(!tqli(c1, c2, c, n))
	  {assert(0);}
	eigsrt(c1, c, n);

	for (i = 0; i < n; i++) {
		e[i] = c1[i];
		for (j = 0; j < n; j++) {
			v[j][i] = c[i][j];
		}
	}

	for (i = 0; i < n; i++)
		delete[] c[i];
	delete[] c;
	delete[] c1;
	delete[] c2;

}

static void Eigen2(double * const * const a, double * const e,
	double * const * const v, const int& n)
{
	double** c;
	double* c1;
	double* c2;
	int i, j;

	c = new double*[n];
	c1 = new double[n];
	c2 = new double[n];
	for (i = 0; i < n; i++) {
		c[i] = new double[n];
		for (j = 0; j < n; j++) {
			c[i][j] = a[i][j];
		}
	}

	tred2(c, c1, c2, n);
	if(!tqli(c1, c2, c, n))
	  {  assert(0);	  }
	eigsrt2(c1, c, n);

	for (i = 0; i < n; i++) {
		e[i] = c1[i];
		for (j = 0; j < n; j++) {
			v[j][i] = c[j][i];
		}
	}

	for (i = 0; i < n; i++)
		delete[] c[i];
	delete[] c;
	delete[] c1;
	delete[] c2;

}

/*******************************************************************
**
**	Svd returns u(mxn), s(nxn) diagonal, v(nxn) orthogonal, 
**	with uTu = 1 (nxn) given a(mxn).
**
**	Corresponds to matlab's SVD(x, 0).
**
*******************************************************************/

void Svd(double * const * const a, double * const * const u, 
	double * const * const s, double * const * const v,
	const int& m, const int& n)
{
  int i, j, k;

  double** c = new double*[n];
  for (i = 0; i < n; i++) {
    c[i] = new double[n];
  }

  for (i = 0; i < n; i++) {
    for (j = i; j < n; j++) {
      for (c[i][j] = 0.0, k = 0; k < m; k++) {
	c[i][j] += a[k][i] * a[k][j];
      }
      c[j][i] = c[i][j];
    }
  }

  double* sd = new double[n];
  Eigen2(c, sd, v, n);
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      s[i][j] = 0.0;
    }
    s[i][i] = ((sd[i] >= 0.0) ? sqrt(sd[i]) : 0.0);
  }

  for (i = 0; i < n; i++) {
    sd[i] = ((s[i][i] == 0.0) ? 0.0 : 1.0 / s[i][i]);
  }

  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      for (u[i][j] = 0.0, k = 0; k < n; k++) {
	u[i][j] += a[i][k] * v[k][j] * sd[j];
      }
    }
  }
  
  for (i = 0; i < n; i++) delete[] c[i];
  delete[] sd, c;
}



/**************************************************************************
**  The following routines compute least squares estimates of poses or
**  rotation matrices from corresponding points or from arrays of
**	(possibly affine) matrices.
**************************************************************************/


int 
lstSqAlign(const Vector x[], const Vector y[], const int& n, 
Matrix& R, Vector& d)
{
	double Hi[3][3];
	double * H[3] = {Hi[0], Hi[1], Hi[2]};

	double Ui[3][3];
	double * U[3] = {Ui[0], Ui[1], Ui[2]};

	double Si[3][3];
	double * S[3] = {Si[0], Si[1], Si[2]};

	double Vi[3][3];
	double * V[3] = {Vi[0], Vi[1], Vi[2]};

	int i, j, k, l;

	Vector *x0 = new Vector[n];
	Vector xM;
	Vector *y0 = new Vector[n];
	Vector yM;

	for (yM = xM = Vector(0.0, 0.0, 0.0), i = 0; i < n; i++) {
		xM += x[i];
		yM += y[i];
	}
	xM /= (double)n;
	yM /= (double)n;

	for (i = 0; i < n; i++) {
		x0[i] = x[i] - xM;
		y0[i] = y[i] - yM;
	}

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			for (H[i][j] = 0.0, k = 0; k < n; k++) {
				H[i][j] += x0[k][i] * y0[k][j];
			}
		}
	}

	
	Svd(H, U, S, V, 3, 3);

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			for (R[i][j] = 0.0, k = 0; k < 3; k++) {
				R[i][j] += V[i][k] * U[j][k];
			}
		}
	}

	d = yM - R / xM;

	delete[] x0;
	delete[] y0;

	return 1;
}

