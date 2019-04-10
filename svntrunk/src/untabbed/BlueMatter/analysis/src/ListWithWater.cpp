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
ValList < int > AllWaterO;
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
PDB pdbG;
 
FILE *outfileG;

//void InitStuff(PDB &pdb, char *listname, double D)
void InitStuff(PDB &pdb, int res1, double D)
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


  SelectAType(pdb,"OH2",AllWaterO,AllWater);

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

//   Ret.Write("retinal.lst");

//   SelectAType(pdb,"C6",IList1,Ret); IList2.Copy(IList1);
//   SelectAType(pdb,"C5",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C18",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H18A",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H18B",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H18C",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C4",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H4A",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H4B",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C3",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H3A",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H3B",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C2",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H2A",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H2B",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C1",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);   
//   SelectAType(pdb,"C16",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H16A",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H16B",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H16C",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C17",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H17A",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H17B",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H17C",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   IList2.Write("ionone.lst");

//   SelectAType(pdb,"N16",IList1,Ret); IList2.Copy(IList1);
//   SelectAType(pdb,"H16",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C15",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H15",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C14",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H14",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C13",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C20",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H20A",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H20B",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H20C",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C12",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H12",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C11",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H11",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C10",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H10",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C9",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);   
//   SelectAType(pdb,"C19",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H19A",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H19B",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H19C",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C8",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H8",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"C7",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   SelectAType(pdb,"H7",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
//   IList2.Write("polyene.lst");

//   SelectAType(pdb,"N16",IList1,Ret); IList2.Copy(IList1);
//   SelectAType(pdb,"H16",IList1,Ret); IList3.Or(IList1,IList2);IList2.Copy(IList3);  
  //  IList2.Write("schiffp.lst");

  //  IList5.Copy(IList2);

  SelectResId(pdb,178,IList1,AllProtein);
  SelectAType(pdb,"CZ",Tyr178CZ,IList1);
  //  IList1.Write("Res178.lst");

  SelectResId(pdb,191,IList1,AllProtein);
  SelectAType(pdb,"CZ",Tyr191CZ,IList1);
  //  IList1.Write("Res191.lst");

  SelectResId(pdb,268,IList1,AllProtein);
  SelectAType(pdb,"CZ",Tyr268CZ,IList1);
  //  IList1.Write("Res268.lst");

  SelectResId(pdb,188,IList1,AllProtein);
  SelectAType(pdb,"CA",Gly188CA,IList1);
  //  IList1.Write("Res188.lst");

  SelectResId(pdb,114,IList1,AllProtein);
  SelectAType(pdb,"CA",Gly114CA,IList1);
  //  IList1.Write("Res114.lst");

  SelectResId(pdb,121,IList1,AllProtein);
  SelectAType(pdb,"CA",Gly121CA,IList1);
  //  IList1.Write("Res121.lst");

  SelectResId(pdb,118,IList1,AllProtein);
  SelectAType(pdb,"C",Thr118C,IList1);
  //  IList1.Write("Res118.lst");

  SelectResId(pdb,186,IList1,AllProtein);
  SelectAType(pdb,"CB",Ser186CB,IList1);
  //  IList1.Write("Res186.lst");

//   SelectResId(pdb,134,IList1,AllProtein);
//   IList1.Write("Res134.lst");

//   SelectResId(pdb,135,IList1,AllProtein);
//   IList1.Write("Res135.lst");

//   SelectResId(pdb,265,IList1,AllProtein);
//   IList1.Write("Res265.lst");

//   SelectResId(pdb,169,IList1,AllProtein);
//   IList1.Write("Res169.lst");

//   SelectResId(pdb,211,IList1,AllProtein);
//   IList1.Write("Res211.lst");

//   SelectResId(pdb,181,IList1,AllProtein);
//   IList1.Write("Res181.lst");

//   SelectResId(pdb,113,IList1,AllProtein);
//   IList1.Write("Res113.lst");

  SelectResId(pdb,res1,IList5,AllProtein);
  char name[256];
  sprintf(name,"Res%d.lst",res1);
  IList5.Write(name);

  //  IList5.Init(listname);




  outfileG = stdout;

  fprintf(outfileG, "#  Time\t nWaterOs_D%f \n",D);
}

void DoStuff(FrameHandler & fram, PDB &pdb,int nframe, double NetTimeStepInPs, double D)
{
  static int first = 1;
#define MIN_VAL 1e-6


  int nW = AllWaterO.Size();
  int nats = IList5.Size();
  int nnW=0;
  for(int i=0; i <nW;i++)
    {
      int ind1 = AllWaterO[i];
      for(int j = 0;j < nats;j++)
  {
    int ind2 = IList5[j];
    double d = AtomPairDistance(fram,ind1,ind2);
    if(d <= D)
      {
        //	      printf("Found Water %f from %d\n",d,ind2);
        nnW++;
        break;
      }
  }
    }


  fprintf(outfileG, "%d %d \n",nframe, nnW);

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
  //  Calc1r6Sum(fram, IList2, AllWater,Sr1r2);
  //  CalcDistMat(fram, IList2, IList3,DList1 );
  //  FindMinVal(DList1,min);
  //  IList2.OutputItems(stdout);
  // IList3.OutputItems(stdout);

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
    vector<XYZ> coors;


    if (argc < 8) {
        printf(" %s  BMTFileName PDBFileName <resid> <distance> <start> <stop> <skip>\n",argv[0]);
  printf(" Gives number of water O within distance of list atoms\n");
        return 1;
    }

    char *bmtname = argv[1];
    char *pdbname = argv[2];
    char listname[256];
    int res1 = atoi(argv[3]); 
    // strcpy(listname,argv[3]);
    double D = atof(argv[4]);
    int start = atoi(argv[5]);
    int stop = atoi(argv[6]);
    int skip = atoi(argv[7]);


    XYZ *coor;

    BMTReader bmt(bmtname);
    int natom = bmt.getNAtoms();

    XYZ box;
    BoundingBox myInitBox=bmt.getInitialBox();
    box = myInitBox.GetDimensionVector(); 
    double NetTimeStepInPs = bmt.getNetTimeStepInPicoSeconds();
    double InitTimeStepInNs = bmt.getInitialSimulationTimeInNanoSeconds();

    pdbG.Read(pdbname,natom);
    InitStuff(pdbG,res1,D);
    //    InitStuff(pdbG,listname,D);

    int readerr = bmt.ReadFrame();
    int nframe = -1;

    int WritePDB = 0;

    while (!readerr)
      {
        int ignore_frame = (nframe-start) % skip;
  
        if ( (nframe >= start) && (nframe <= stop) && (!ignore_frame) )
    {
      
      coor = bmt.getPositions();
      framG.SetPositions(coor);

      DoStuff(framG, pdbG, nframe, NetTimeStepInPs, D);
    }

  readerr = bmt.ReadFrame();
  nframe++;
      
      }
}

void mapIntoLine(double x, double &v, double a, double l)
    {
    double dx = x-a;
    int k = fabs(dx/l);
    v = k*l;
    if (dx < 0)
        v = -v - l;
    }
