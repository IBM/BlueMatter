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


ValList < int > DHAC2;
ValList < int > DHAC3;
ValList < int > DHAC4;
ValList < int > DHAC5;
ValList < int > DHAC6;
ValList < int > DHAC7;
ValList < int > DHAC8;
ValList < int > DHAC9;
ValList < int > DHAC10;
ValList < int > DHAC11;
ValList < int > DHAC12;
ValList < int > DHAC13;
ValList < int > DHAC14;
ValList < int > DHAC15;
ValList < int > DHAC16;
ValList < int > DHAC17;
ValList < int > DHAC18;
ValList < int > DHAC19;
ValList < int > DHAC20;
ValList < int > DHAC21;
ValList < int > DHAC22;

ValList < XYZ > *C2SeriesG;
ValList < XYZ > *C3SeriesG;
ValList < XYZ > *C4SeriesG;
ValList < XYZ > *C5SeriesG;
ValList < XYZ > *C6SeriesG;
ValList < XYZ > *C7SeriesG;
ValList < XYZ > *C8SeriesG;
ValList < XYZ > *C9SeriesG;
ValList < XYZ > *C10SeriesG;
ValList < XYZ > *C11SeriesG;
ValList < XYZ > *C12SeriesG;
ValList < XYZ > *C13SeriesG;
ValList < XYZ > *C14SeriesG;
ValList < XYZ > *C15SeriesG;
ValList < XYZ > *C16SeriesG;
ValList < XYZ > *C17SeriesG;
ValList < XYZ > *C18SeriesG;
ValList < XYZ > *C19SeriesG;
ValList < XYZ > *C20SeriesG;
ValList < XYZ > *C21SeriesG;
ValList < XYZ > *C22SeriesG;
ValList < XYZ > ProteinG;


ValList < double > DHAC2R;
ValList < double > DHAC3R;
ValList < double > DHAC4R;
ValList < double > DHAC5R;
ValList < double > DHAC6R;
ValList < double > DHAC7R;
ValList < double > DHAC8R;
ValList < double > DHAC9R;
ValList < double > DHAC10R;
ValList < double > DHAC11R;
ValList < double > DHAC12R;
ValList < double > DHAC13R;
ValList < double > DHAC14R;
ValList < double > DHAC15R;
ValList < double > DHAC16R;
ValList < double > DHAC17R;
ValList < double > DHAC18R;
ValList < double > DHAC19R;
ValList < double > DHAC20R;
ValList < double > DHAC21R;
ValList < double > DHAC22R;

double minSG = 0.0;
double maxSG = 100000.0;

FrameHandler framG;
ValList < double > MassesG;
PDB pdbG;
 
FILE *outfileG;

void InitStuff(PDB &pdb, int NFrames)
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

  SelectAType(pdb,"C2",DHAC2,AllDHA);
  SelectAType(pdb,"C3",DHAC3,AllDHA);
  SelectAType(pdb,"C4",DHAC4,AllDHA);
  SelectAType(pdb,"C5",DHAC5,AllDHA);
  SelectAType(pdb,"C6",DHAC6,AllDHA);
  SelectAType(pdb,"C7",DHAC7,AllDHA);
  SelectAType(pdb,"C8",DHAC8,AllDHA);
  SelectAType(pdb,"C9",DHAC9,AllDHA);
  SelectAType(pdb,"C10",DHAC10,AllDHA);
  SelectAType(pdb,"C11",DHAC11,AllDHA);
  SelectAType(pdb,"C12",DHAC12,AllDHA);
  SelectAType(pdb,"C13",DHAC13,AllDHA);
  SelectAType(pdb,"C14",DHAC14,AllDHA);
  SelectAType(pdb,"C15",DHAC15,AllDHA);
  SelectAType(pdb,"C16",DHAC16,AllDHA);
  SelectAType(pdb,"C17",DHAC17,AllDHA);
  SelectAType(pdb,"C18",DHAC18,AllDHA);
  SelectAType(pdb,"C19",DHAC19,AllDHA);
  SelectAType(pdb,"C20",DHAC20,AllDHA);
  SelectAType(pdb,"C21",DHAC21,AllDHA);
  SelectAType(pdb,"C22",DHAC22,AllDHA);

  int SeriesLen = DHAC2.Size();
  C2SeriesG = new ValList< XYZ >[SeriesLen];
  C3SeriesG = new ValList< XYZ >[SeriesLen];
  C4SeriesG = new ValList< XYZ >[SeriesLen];
  C5SeriesG = new ValList< XYZ >[SeriesLen];
  C6SeriesG = new ValList< XYZ >[SeriesLen];
  C7SeriesG = new ValList< XYZ >[SeriesLen];
  C8SeriesG = new ValList< XYZ >[SeriesLen];
  C9SeriesG = new ValList< XYZ >[SeriesLen];
  C10SeriesG = new ValList< XYZ >[SeriesLen];
  C11SeriesG = new ValList< XYZ >[SeriesLen];
  C12SeriesG = new ValList< XYZ >[SeriesLen];
  C13SeriesG = new ValList< XYZ >[SeriesLen];
  C14SeriesG = new ValList< XYZ >[SeriesLen];
  C15SeriesG = new ValList< XYZ >[SeriesLen];
  C16SeriesG = new ValList< XYZ >[SeriesLen];
  C17SeriesG = new ValList< XYZ >[SeriesLen];
  C18SeriesG = new ValList< XYZ >[SeriesLen];
  C19SeriesG = new ValList< XYZ >[SeriesLen];
  C20SeriesG = new ValList< XYZ >[SeriesLen];
  C21SeriesG = new ValList< XYZ >[SeriesLen];
  C22SeriesG = new ValList< XYZ >[SeriesLen];

  XYZ xxx;
  xxx.Zero();
  ProteinG.Init(NFrames,xxx);
  for(int i = 0 ; i < SeriesLen; i++)
    {
      C2SeriesG[i].Init(NFrames,xxx);
      C3SeriesG[i].Init(NFrames,xxx);
      C4SeriesG[i].Init(NFrames,xxx);
      C5SeriesG[i].Init(NFrames,xxx);
      C6SeriesG[i].Init(NFrames,xxx);
      C7SeriesG[i].Init(NFrames,xxx);
      C8SeriesG[i].Init(NFrames,xxx);
      C9SeriesG[i].Init(NFrames,xxx);
      C10SeriesG[i].Init(NFrames,xxx);
      C11SeriesG[i].Init(NFrames,xxx);
      C12SeriesG[i].Init(NFrames,xxx);
      C13SeriesG[i].Init(NFrames,xxx);
      C14SeriesG[i].Init(NFrames,xxx);
      C15SeriesG[i].Init(NFrames,xxx);
      C16SeriesG[i].Init(NFrames,xxx);
      C17SeriesG[i].Init(NFrames,xxx);
      C18SeriesG[i].Init(NFrames,xxx);
      C19SeriesG[i].Init(NFrames,xxx);
      C20SeriesG[i].Init(NFrames,xxx);
      C21SeriesG[i].Init(NFrames,xxx);
      C22SeriesG[i].Init(NFrames,xxx);
    }

}

void AtomVector(FrameHandler &fram, int a1, int a2, XYZ &v)
{
  XYZ p1,p2;
  fram.GetPosition(a1,p1);
  fram.GetPosition(a2,p2);
  v = p2 - p1; 
  
}

void CHVector(FrameHandler &fram, int index, ValList < int > &inds, ValList < XYZ > *coors, int nframe,double minS, double maxS)
{
  XYZ v;
  int id = inds[index];
  double sum = 0.0;
  for(int i = 0;i < AllProtein.Size(); i++)
    {
      int ind = AllProtein[i];
      AtomVector(fram, id , ind, v);
      double r = v.Length();
      double r3 = r * r * r;
      double r6 = 1.0 / (r3 * r3);
      sum += r6;
    }
  //  printf("CHVector id %d %f min %f max %f\n",id,sum,minS,maxS);
  if ( sum > minS && sum < maxS)
    {
      AtomVector(fram, id , id+1, v);
    }
  else
    v.Zero();

  coors[index].Set(nframe,v);

  if( 0 &&  index == 0)
    printf("frame %d index %d id1 %d CHVector %f %f %f\n",nframe, index, id, v.mX,v.mY,v.mZ);
}
void DoStuff(FrameHandler & fram, ValList < double > &Masses, PDB &pdb, int nframe)
{
  static int first = 1;

  //  CalcDistMat(fram, RetC20, Tyr268CZ,DList1 );
  //  for(int i=0;i< RetC20.Size(); i++)
  //    pdb.WriteRecord(fram,RetC20[i],stdout);
  //  for(int i=0;i< Tyr268CZ.Size(); i++)
  //    pdb.WriteRecord(fram,Tyr268CZ[i],stdout);

  printf("Frame %d\n",nframe);


  for(int i = 0;i < DHAC2.Size(); i++)
    {

      //methylene C 2 3 6 9 12 15 18 21 
      CHVector(fram,i,DHAC2,C2SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC3,C3SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC4,C4SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC5,C5SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC6,C6SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC7,C7SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC8,C8SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC9,C9SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC10,C10SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC11,C11SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC12,C12SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC13,C13SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC14,C14SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC15,C15SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC16,C16SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC17,C17SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC18,C18SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC19,C19SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC20,C20SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC21,C21SeriesG,nframe, minSG, maxSG);
      CHVector(fram,i,DHAC22,C22SeriesG,nframe, minSG, maxSG);
    }

  //  for(int i=0;i<DList1.Size();i++)
  //    fprintf(outfileG,"%f ",DList1[i]);
  //  fprintf(outfileG,"\n");

}

int AddCHCorrelation(int n, ValList <int> &idmap, ValList <XYZ> *series, ValList <double> &corr, ValList <int> &count)
{

   
printf("AddCHCorrelation n= %d\n",n);
  for(int dt = 0; dt < n;dt++)
    {
      double dtsum = corr[dt];
      int dtcount = count[dt];

      double sum = 0;
      int cnt = 0;
      int nn = n-dt;
      for(int i = 0;i < nn ; i++)
  {
    int nlips = idmap.Size();
    for(int a=0;a < nlips;a++)
      {
        XYZ x0,xt;
        int ind = idmap[a];

        x0 = series[a][i];
        xt = series[a][i + dt];
        double r0 = x0.Length();
        double rt = xt.Length();
        if ( (r0 < 0.1) || (rt < 0.1) )
    {
      //  printf(" len %f %f\n",r0,rt);
      continue;
    }
        else
    {
      // printf(" len %f %f\n",r0,rt);
    }

        x0 *= 1.0/r0;
        xt *= 1.0/rt;
        
        double c = x0.mX * xt.mX + x0.mY * xt.mY + x0.mZ * xt.mZ;
        // printf("\t\tdt %d i %d a %d ind %d r0 %f rt %f c %f\n",dt,i,a,ind,r0,rt,c);
        sum += c;
        cnt++;
      }
  }
      //      sum /= count;
      //	sum /= n;
      corr[dt] = dtsum + sum;
      count[dt] = dtcount + cnt;
      printf(" corr[%d] = %f count %d\n",dt,corr[dt],count[dt]);
    }      
  return count[0];
}
void CorrelateDHACH2Vecs(int n , ValList <double> &corr, ValList <int> &count)
{
  corr.Init(n,0);
  count.Init(n,0);
  printf("Now correlating CHVcs\n");

      //methylene C 2 3 6 9 12 15 18 21 
  AddCHCorrelation(n, DHAC2,C2SeriesG,corr, count);
  AddCHCorrelation(n, DHAC3,C3SeriesG,corr, count);
  AddCHCorrelation(n, DHAC6,C6SeriesG,corr, count);
  AddCHCorrelation(n, DHAC9,C9SeriesG,corr, count);
  AddCHCorrelation(n, DHAC12,C12SeriesG,corr, count);
  AddCHCorrelation(n, DHAC15,C15SeriesG,corr, count);
  AddCHCorrelation(n, DHAC18,C18SeriesG,corr, count);
  AddCHCorrelation(n, DHAC21,C21SeriesG,corr, count);
    
  // Normalize corr 
  for(int i = 0;i < corr.Size(); i++)
    {
      corr[i] = corr[i] / count[i];
    }
  return ;
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


    if (argc < 3) {
        cout << "bmtstat BMTFileName PDBFileName OutputSeed <start> <stop> <skip> <minSG> <maxSG>"
             << endl;
        return 1;
    }

    char *bmtname = argv[1];
    char *pdbname = argv[2];
    char massname[256];
    strcpy(massname,pdbname);
    strcat(massname,".mass");
    char *seedname = argv[3];
    int start = atoi(argv[4]);
    int stop = atoi(argv[5]);
    int skip = atoi(argv[6]);
    minSG = atof(argv[7]);
    maxSG = atof(argv[8]);

    printf("# %s S [ %f : %f ] \n", bmtname,minSG,maxSG);
    int NFrames = (stop - start)/skip + 1;

    XYZ *coor;

    BMTReader bmt(bmtname);
    int natom = bmt.getNAtoms();

    XYZ box;
    BoundingBox myInitBox=bmt.getInitialBox();
    box = myInitBox.GetDimensionVector(); 
    double NetTimeStepInPs = bmt.getNetTimeStepInPicoSeconds();
    double InitTimeStepInNs = bmt.getInitialSimulationTimeInNanoSeconds();

    pdbG.Read(pdbname,natom);
    //    MassesG.Init(massname);
    InitStuff(pdbG,NFrames);


    int readerr = bmt.ReadFrame();
    int nframe = -1;

    int WritePDB = 0;

    int actual_frame = 0;
    while (!readerr)
      {
        int ignore_frame = (nframe-start) % skip;
  
        if ( (nframe >= start) && (nframe <= stop) && (!ignore_frame) )
    {
      
      coor = bmt.getPositions();
      framG.SetPositions(coor);

      DoStuff(framG, MassesG, pdbG, actual_frame);
      actual_frame++;
    }

  readerr = bmt.ReadFrame();
  nframe++;
        if (nframe > stop)
    break;
      }

  ValList <double> corr; 
  ValList <int> count;

  corr.Init(actual_frame,0);
  count.Init(actual_frame,0);
  CorrelateDHACH2Vecs(actual_frame ,corr, count);

  FILE *out = fopen(seedname,"w");
  fprintf(out,"# CH2 correlation N = %d NetTimeinPs = %f, skip = %d\n",count[0], NetTimeStepInPs, skip);

  for(int i=0;i < actual_frame;i++)
    {
      double t = NetTimeStepInPs * skip * i;
      fprintf(out,"%f %g %d\n",t,corr[i], count[i]);
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
