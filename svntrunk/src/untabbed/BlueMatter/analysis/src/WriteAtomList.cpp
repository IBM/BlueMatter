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
 
#include <BlueMatter/bmtstat.hpp>


///////////////////////////////
///////  Working global arrays

ValList < int > ResGroup1;
ValList < int > ResGroup2;

ValList < int > IList1;
ValList < int > IList2;
ValList < int > IList3;
ValList < int > IList4;
ValList < int > IList5;
ValList < int > AllAtoms;
ValList < int > AllLipid;
ValList < int > AllWater;
ValList < int > AllIons;
ValList < int > AllDHA;
ValList < int > AllSTEA;
ValList < int > AllCHOL;
ValList < int > AllPEGL;
ValList < int > AllPCGL;
ValList < int > AllProtein;

ValList < int > Ret;
ValList < int > RetC12;
ValList < int > RetC14;
ValList < int > RetC15;
ValList < int > RetC19;
ValList < int > RetC20;

ValList < int > Tyr178CZ;
ValList < int > Tyr191CZ;
ValList < int > Tyr268CZ;

ValList < int > Gly188CA;
ValList < int > Gly114CA;
ValList < int > Gly121CA;

ValList < int > Thr118C;
ValList < int > Ser186CB;



ValList < double > DList1;
ValList < double > DList2;
ValList < double > DList3;
ValList < double > DList4;
ValList < double > DList5;

int res1G,res2G;

FrameHandler framG;
ValList < double > MassesG;
 
FILE *outfileG;

void InitStuff(PDB &pdb)
{
  int na = pdb.mNAtoms;
  AllAtoms.Init(na);
  for(int i=0;i< na; i++)
    AllAtoms.Set(i,i);
 
  SelectResName(pdb,"TIP3",AllWater,AllAtoms);
  SelectResName(pdb,"SOD",IList2,AllAtoms);
  SelectResName(pdb,"CLA",IList1,AllAtoms);
  AllIons.Or(IList1,IList2); 

  SelectResName(pdb,"DHA",AllDHA,AllAtoms);
  SelectResName(pdb,"STEA",AllSTEA,AllAtoms);
  SelectResName(pdb,"CHOL",AllCHOL,AllAtoms);
  SelectResName(pdb,"PEGL",AllPEGL,AllAtoms);
  SelectResName(pdb,"PCGL",AllPCGL,AllAtoms);


  // Ops for AllLipid
  IList1.Or(AllDHA,AllSTEA);
  IList2.Or(IList1,AllCHOL);
  IList1.Or(AllPCGL,AllPEGL);
  AllLipid.Or(IList1,IList2);

  // Ops for AllProtein : note, protein is everything else
  IList1.And(AllWater,AllAtoms);
  IList2.Or(IList1,AllLipid);
  IList1.Or(IList2,AllIons);
  AllProtein.XOr(AllAtoms,IList1);

  SelectResId(pdb,296,Ret,AllProtein);
  SelectAType(pdb,"C20",RetC20,Ret);
  SelectAType(pdb,"C19",RetC19,Ret);
  SelectAType(pdb,"C15",RetC15,Ret);
  SelectAType(pdb,"C14",RetC14,Ret);
  SelectAType(pdb,"C12",RetC12,Ret);

  SelectResId(pdb,178,IList1,AllProtein);
  SelectAType(pdb,"CZ",Tyr178CZ,IList1);

  SelectResId(pdb,191,IList1,AllProtein);
  SelectAType(pdb,"CZ",Tyr191CZ,IList1);

  SelectResId(pdb,268,IList1,AllProtein);
  SelectAType(pdb,"CZ",Tyr268CZ,IList1);


  SelectResId(pdb,188,IList1,AllProtein);
  SelectAType(pdb,"CA",Gly188CA,IList1);

  SelectResId(pdb,114,IList1,AllProtein);
  SelectAType(pdb,"CA",Gly114CA,IList1);

  SelectResId(pdb,121,IList1,AllProtein);
  SelectAType(pdb,"CA",Gly121CA,IList1);

  SelectResId(pdb,118,IList1,AllProtein);
  SelectAType(pdb,"C",Thr118C,IList1);

  SelectResId(pdb,186,IList1,AllProtein);
  SelectAType(pdb,"CB",Ser186CB,IList1);

  outfileG = stdout;

}

void DoStuff( PDB &pdb)
{
  static int first = 1;
#define MIN_VAL 1e-6
  //  Get res 1 and res 2 into ilist 1 and 2


  int res1 = IList1[0];
printf("selecting res %d\n",res1);

  SelectResId(pdb,res1,IList2,AllProtein);
printf("# Res %d\n", res1);
  IList2.OutputItems(stdout);


  //////////////////
  //                              PREVIOUS EXAMPLES
  //  CalcDistMat(fram, RetC20, Tyr268CZ,DList1 );
  //  for(int i=0;i< RetC20.Size(); i++)
  //    pdb.WriteRecord(fram,RetC20[i],stdout);
  //  for(int i=0;i< Tyr268CZ.Size(); i++)
  //    pdb.WriteRecord(fram,Tyr268CZ[i],stdout);
  // GetWaterZProfile( fram, pdb, AllWater,DList1);
  // for(int i=0;i<DList1.Size();i++)
  //   fprintf(outfileG,"%f ",DList1[i]);
  // fprintf(outfileG,"\n");

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////
////////                                      Blunt Main Loop Over Frames, calling InitStuff and DoStuff
////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int
main(int argc, char **argv, char **envp)
{


    if (argc < 3) {
        printf("%s PDBFileName <resid> \n ",argv[0]);

        return 1;
    }

    char *pdbname = argv[1];

    int res1 = atoi(argv[2]);

    printf("Reading pdb file |%s|\n",pdbname);
    PDB pdbG;

    pdbG.Read(pdbname);

    InitStuff(pdbG);

    IList1.Init(1);
    IList1.Set(0,res1);


    
    DoStuff( pdbG);
  
}

void mapIntoLine(double x, double &v, double a, double l)
    {
    double dx = x-a;
    int k = fabs(dx/l);
    v = k*l;
    if (dx < 0)
        v = -v - l;
    }
