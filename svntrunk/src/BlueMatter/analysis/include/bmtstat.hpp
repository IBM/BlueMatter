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
 


#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
//#include <vector>
#include <string>
#include <BlueMatter/BMT.hpp>
#include <BlueMatter/BoundingBox.hpp>
#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/XYZ.hpp>
//#include <fstream>

using namespace std;

#define BUFFSIZE 2048
#define MAXATM   100000
#define PDBPREFIX 30
#define PDBSUFFIX 55
#define PDBWIDTH  80

class FrameHandler
{
  public:
  XYZ *mPos;
  double mTimestepInPs;
  int mNAtoms;
  
  
  FrameHandler(){mNAtoms = 0; mPos = NULL; mTimestepInPs = 0;}
  
  void Init(int natoms, double timestep )
    {
      mNAtoms = natoms;
      mTimestepInPs = timestep;
      mPos = NULL;      
    }

  void SetPositions(XYZ * pos)
    {
      mPos = pos;
    }

  void GetPosition(int index, XYZ & pos)
    {
      assert(index < mNAtoms);
      pos = mPos[index];
    }

  void SetPosition(int index, XYZ &pos)
    {
      assert(index < mNAtoms);
      mPos[index] = pos;      
    }
};

template < class T > class ValList
{
public:
  int mSize;
  int mAllocatedSize;
  T *mId;

  ValList()
    {
      mSize = 0;
      mId = NULL;
    }

  ~ValList()
    {
      Clean(mAllocatedSize+1);
    }

  void Clean(int size = 0)
    {
      
      if(mId != NULL && (size > mAllocatedSize))
	{
	  delete [] mId;
	  mId = NULL;
	  mAllocatedSize = 0;
	}
      mSize = 0;
    }
  int Init(int size, T initval = 0)
    {
      Clean(size);
      if(size > 0)
	{
	  if(size > mAllocatedSize)
	    {
	      assert(mId == NULL);  // bust memory leak attempts
	      mId = new T[size];
	      assert(mId != NULL);  
	      mAllocatedSize = size;
	    }
	  if(mId ==NULL)
	    {
	      char msg[100];
	      sprintf(msg,"Couldn't allocate ValList of size %d\n",size);
	      assert(msg == NULL);
	      return 0;
	    }
	  for(int i = 0;i < size; i++)
	    mId[i] = initval;
	}
      mSize = size;
      return mSize;
    }

  int Size(){ return mSize;}

  T & Get(int index)
    { 
      if(index >= mSize) 
	{char msg[256]; sprintf(msg,"ValList: Index %d out of bounds %d",index,mSize); assert(msg == NULL);}
      return mId[index];
    }

  void Set(int index, T & val)
    {
      if(index < mSize) 
	{ mId[index] = val;}
      else 
	{char msg[256]; sprintf(msg,"ValList: Index %d out of bounds %d",index,mSize); assert(msg == NULL);}
    }

  T & operator[] (int index) { return Get(index);}


  int ReadSize(FILE *inPF)
    {
      char buf[256];
      fgets(buf, 256,inPF);
      while (!feof(inPF))
	{
	  char tag[256];
	  int val;
	  sscanf(buf,"%s %d",tag,&val);
	  if ( !strcmp("LIST_SIZE",tag))
	    {
	      Init(val);
	      return val;
	    }
	  fgets(buf,256,inPF);
	}
      return 0;
    }

  int OutputSize(FILE *outPF)
    {
      return fprintf(outPF,"LIST_SIZE %d\n",mSize);      
    }
  int OutputItems(FILE *outPF)
    {
      int sz = sizeof(mId[0]);

      switch(sz)
	{
	case 4:
	  for(int i = 0 ; i < mSize; i++)
	    {
	      int ival = (int) mId[i];
	      fprintf(outPF,"%d\n",ival);
	    }
	  break;
        default:
	  for(int i = 0 ; i < mSize; i++)
	    {
	      double val = (double) mId[i];
	      fprintf(outPF,"%lf\n",val);
	    }
	  break;
	}
      return mSize;
    }


  int ReadItems(FILE *inPF)
    {
      char buf[256];
      int index = 0;
      int val;
      fgets(buf, 256,inPF);
      while (!feof(inPF))
	{
	  char tag[256];
	  double  val;
	  T tval;
	  if(1 == sscanf(buf,"%lf", &val))
	    {
	      tval = ( T ) val;
	      mId[index] = tval;
	      index++;
	    }
	  else
	    {
	      char msg[256]; 
	      sprintf(msg,"ValList: Could not read Index %d; expected %d items",index,mSize); 
	      assert(msg == NULL);
	    }
	  if(index >= mSize)
	    break;
	  fgets(buf,256,inPF);
	}
      return mSize;
    }
  int Init(FILE *inPF)
    {
      Clean();
      
      ReadSize(inPF);
      return ReadItems(inPF);

    }

  int Init(char *filename)
    {
      FILE * fPF = fopen(filename,"r");
      assert(fPF != NULL);
      int rc =  Init(fPF);
      fclose(fPF);
      return rc;
    }
  void Write(FILE *outPF)
    {
      int rc;
      rc = OutputSize(outPF);
      assert(rc);
      rc = OutputItems(outPF);
      assert(rc);
    }

  int Write(char *filename)
    {
      FILE *outPF = fopen(filename,"w");
      assert(outPF != NULL);
      Write(outPF);
      fclose(outPF);
      return 1;
    }

  int And(ValList < T > &L1, ValList < T > &L2)  
    {
      // Assumes list are sorted in ascending order

      assert(L1.mAllocatedSize == L2.mAllocatedSize);
      Init(L1.mAllocatedSize);
      int i = 0;
      int j = 0;
      int cnt = 0;
      while (i < L1.mSize && j < L2.mSize)
	{
	  if(L1[i] < L2[j])
	    i++;
	  if(L1[i] > L2[j])
	    j++;
	  if(L1[i] == L2[j])
	    { 
	      mId[cnt] = L1[i];
	      cnt++; i++; j++;
	    }
	}
      mSize = cnt;
      return cnt;
    }
  int Or(ValList < T > &L1, ValList < T > &L2)  
    {
      // Assumes list are sorted in ascending order

      assert(L1.mAllocatedSize == L2.mAllocatedSize);
      Init(L1.mAllocatedSize );
      int i = 0;
      int j = 0;
      int cnt = 0;
      while (i < L1.mSize && j < L2.mSize)
	{
	  if(L1[i] < L2[j])
	    {mId[cnt] = L1[i]; cnt++; i++;}
	  if(L1[i] > L2[j])
	    {mId[cnt] = L2[j]; cnt++; j++;}
	  if(L1[i] == L2[j])
	    { mId[cnt] = L1[i]; cnt++; i++; j++; }
	}
      while (i < L1.mSize)
	{mId[cnt] = L1[i]; cnt++; i++;}

      while (j < L2.mSize)
	{mId[cnt] = L2[j]; cnt++; j++;}

      mSize = cnt;
      return cnt;
    }

  int XOr(ValList < T > &L1, ValList < T > &L2)  
    {
      // Assumes list are sorted in ascending order

      assert(L1.mAllocatedSize == L2.mAllocatedSize);
      Init(L1.mAllocatedSize);
      int i = 0;
      int j = 0;
      int cnt = 0;
      while (i < L1.mSize && j < L2.mSize)
	{
	  if(L1[i] < L2[j])
	    {mId[cnt] = L1[i]; cnt++; i++;}
	  if(L1[i] > L2[j])
	    {mId[cnt] = L2[j]; cnt++; j++;}
	  if(L1[i] == L2[j])
	    {  i++; j++; }
	}
      while (i < L1.mSize)
	{mId[cnt] = L1[i]; cnt++; i++;}

      while (j < L2.mSize)
	{mId[cnt] = L2[j]; cnt++; j++;}

      mSize = cnt;
      return cnt;
    }

  int Copy(ValList < T > &L1)  
    {
      // Assumes list are sorted in ascending order

      Init(L1.Size());
      int i = 0;
      int cnt = 0;
      while (i < L1.mSize)
	{
	    {mId[cnt] = L1[i]; cnt++; i++;}
	}
      mSize = cnt;
      return cnt;
    }

};


class PDB
{
    // clean this up to avoid fixed size buffers
public:
    int mNAtoms;
    char buff[BUFFSIZE];
    char pdbtxt[MAXATM][PDBWIDTH], pdbsuf[MAXATM][PDBWIDTH];

  PDB(){ mNAtoms = 0;}

  int Read(char *pdbname,int natom = 0)
    {

      char buff[BUFFSIZE];
      
      // Read the pdb file
      // Keep only ATOM lines
      FILE *f = fopen(pdbname, "r");
      assert(f != NULL);
      fgets(buff, BUFFSIZE, f);
      mNAtoms = 0;
      while (!feof(f) && !ferror(f)) 
        {
	  if (!strncmp(buff, "ATOM", 4)) 
            {
	      buff[PDBWIDTH-1] = '\0';
	      buff[PDBPREFIX] = '\0';
	      strcpy(pdbtxt[mNAtoms], buff);
	      strcpy(pdbsuf[mNAtoms], &buff[PDBSUFFIX]);
	      mNAtoms++;
            }
	  fgets(buff, BUFFSIZE, f);
        }
      fclose(f);
      
      if (natom > 0 && natom != mNAtoms)
        {
	  printf("Num of atoms in PDB (%d) and BMT (%d) doesn't match",
		mNAtoms, natom);
	  assert(natom == mNAtoms);
        }    
      return 1;
    }

  void WriteRecord(FrameHandler &fram, int record, FILE *fPF)
    {
      int i = record;
       XYZ *coor = fram.mPos;
       fprintf(fPF, "%s%8.3f%8.3f%8.3f %s\n", pdbtxt[i], 
		  coor[i].mX, coor[i].mY, coor[i].mZ ,pdbsuf[i]);
       
    }
	    
  void Write(char *seedname,int nframe, double t, FrameHandler &fram) 
    {

      int natom = fram.mNAtoms;
      assert(natom == mNAtoms);


      char currentpdbname[1024]; 
      sprintf(currentpdbname,"%s.%05d.pdb",seedname,nframe);
      FILE *fPF =fopen(currentpdbname, "r");
      assert(fPF != NULL);

      fprintf(fPF,"REMARK T(%d) = %lf\n",nframe, t);
      
      for (int i=0; i<mNAtoms; i++)
	{
	  WriteRecord(fram, i, fPF);
	} 
      fprintf(fPF,"END\n");
      fclose(fPF);
 
    }

  int GetPDBField(int id, int offset, int length, char *fld)
    {
      assert(id < mNAtoms);
      char buff[100];
      //          1         2         3         4         5         6         7
      //012345678901234567890123456789012345678901234567890123456789012345678901234567890
      //ATOM      1  N   MET     1       1.683 -19.843 -24.797  1.00   .00      RHOD
      //ATOM  43222  H2  TIP3  7400     15.765  15.340  25.303  1.00   .00      BULK

      if(offset < 30)
	{
	  for(int i = 0 ; i<length ; i++)
	    buff[i] = pdbtxt[id][i + offset];
	  buff[length] = '\0';
	}
      else
	{
	  for(int i = 0 ; i<length ; i++)
	    buff[i] = pdbsuf[id][i + offset - 55];
	  buff[length] = '\0';
	}
      sscanf(buff,"%s",fld);
      return 1;
    }

  int ResName(int id, char *txt)
    {return GetPDBField(id,17,4,txt);}

  int ResId(int id, int &resid)
    {char txt[10]; GetPDBField(id,23,5,txt); sscanf(txt,"%d",&resid); return resid;}

  int Seg(int id, char *txt)
    {return GetPDBField(id,72,6,txt);}

  int AType(int id, char*txt)
    {return GetPDBField(id,12,5,txt);}
    


};


//////////////////////////////
///     Operations

///  Calc Distance Matrix
int CalcDistMat(FrameHandler &frm, ValList <int> & lst1, ValList <int> & lst2, ValList < double > & res )
{

  int s1 = lst1.Size();
  int s2 = lst2.Size();
  int N = s1 * s2;

  res.Init(N);


  int ind = 0;
  for(int i = 0 ; i < s1 ; i++)
    {
      XYZ v1;
      int p1 = lst1[i];
      frm.GetPosition(p1,v1);
      for(int j = 0 ; j < s2; j++)
	{
	  XYZ v2;
	  int p2 = lst2[j];
	  frm.GetPosition(p2,v2);
	  double r = v1.Distance(v2);
	  res.Set(ind,r);
	  ind++;
	  assert(ind < N);

	}
    }
  return N;
}
int Calc1r6Sum(FrameHandler &frm, ValList <int> & lst1, ValList <int> & lst2,double &res )
{

  int s1 = lst1.Size();
  int s2 = lst2.Size();
  int N = s1 * s2;
  double sum = 0.0;

 
  int ind = 0;
  for(int i = 0 ; i < s1 ; i++)
    {
      XYZ v1;
      int p1 = lst1[i];
      frm.GetPosition(p1,v1);
      for(int j = 0 ; j < s2; j++)
	{
	  XYZ v2;
	  int p2 = lst2[j];
	  frm.GetPosition(p2,v2);
	  double r = v1.Distance(v2);
	  double r3 = r * r * r;
	  double r6 = 1.0 / (r3 * r3);
	  sum += r6;
	  ind++;
	  assert(ind < N);

	}
    }
  res = sum;
  return N;
}

template < class T > 
int FindMinVal(ValList < T > & lst, T & minval)
{
  int  s = lst.Size();
  int best = 0;
  minval = lst[0];
  for(int i = 1; i < s; i++)
    {
      T val = lst[i];
      if ( minval > val)
	{ minval = val; best = i; }
    }
  return best;
}


template < class T > 
int FindMaxVal(ValList < T > & lst, T & maxval)
{
  int  s = lst.Size();
  int best = 0;
  maxval = lst[0];
  for(int i = 1; i < s; i++)
    {
      T val = lst[i];
      if ( maxval < val)
	{ maxval = val; best = i; }
    }
  return best;
}

int PrintDistBetweenLists(FrameHandler &fram,ValList < int > &lst1, ValList < int > &lst2, ValList < double > &res)
{
  for(int i = 0; i < lst1.Size() ; i++)
    {
      XYZ v1,v2;
      double r;
      fram.GetPosition(lst1[i],v1);
      fram.GetPosition(lst2[i],v2);
      r = v1.Distance(v2);
      printf("Dist from %d to %d is %f\n",lst1[i],lst2[i],r);
      res.Set(i,r);
    }
  res.Write(stdout);
  return 1;
}

double AtomPairDistance(FrameHandler &fram, int a1, int a2)
{
  XYZ v1,v2;
  double r;
  fram.GetPosition(a1,v1);
  fram.GetPosition(a2,v2);
  r = v1.Distance(v2);
  return r;
}
double FindShortestDistance(FrameHandler & fram, ValList < int > &lst1, ValList < int > &lst2, ValList < double > &distlst,  FILE *out = NULL)
{
  CalcDistMat(fram,lst1, lst2, distlst);
  double min;
  FindMinVal(distlst, min);
  if (out != NULL)
    {fprintf(out,"Mindist: %lf\n",min );}
  return min;
}

int SelectResName(PDB & pdb,char *txt, ValList < int > &lst, ValList < int > &srclst)
{
  int na = srclst.Size();
  lst.Init(na,-1);
  int cnt=0;
  for(int i=0;i < na;i++)
    {
      char buff[16];
      int ind = srclst[i];
      pdb.ResName(ind,buff);
      if(!strcmp(buff,txt))
	{
	  lst.Set(cnt,ind);
	  cnt++;
	}
    }
  lst.mSize = cnt;
  return cnt;
}

int SelectResId(PDB & pdb,int qres, ValList < int > &lst, ValList < int > &srclst )
{
  int na = srclst.Size();
  lst.Init(na,-1);
  int cnt=0;
  for(int i=0;i < na;i++)
    {
      int resid;
      int ind = srclst[i];
      pdb.ResId(ind,resid);
      if(resid == qres)
	{
	  lst.Set(cnt,ind);
	  cnt++;
	}
    }
  lst.mSize = cnt;
  return cnt;
}


int SelectSeg(PDB & pdb,char *txt, ValList < int > &lst,  ValList < int > &srclst)
{
  int na = srclst.Size();
  lst.Init(na,-1);
  int cnt=0;
  for(int i=0;i < na;i++)
    {
      char buff[16];
      int ind = srclst[i];
      pdb.Seg(ind,buff);
      if(!strcmp(buff,txt))
	{
	  lst.Set(cnt,ind);
	  cnt++;
	}
    }
  lst.mSize = cnt;
  return cnt;
}

int SelectAType(PDB & pdb,char *txt, ValList < int > &lst, ValList < int > &srclst)
{
  int na = srclst.Size();
  lst.Init(na,-1);
  int cnt=0;
  for(int i=0;i < na;i++)
    {
      char buff[16];
      int ind = srclst[i];
      pdb.AType(ind,buff);
      if(!strcmp(buff,txt))
	{
	  lst.Set(cnt,ind);
	  cnt++;
	}
    }
  lst.mSize = cnt;
  return cnt;
}


void WriteListPDB(FrameHandler &fram, PDB & pdb, ValList < int > &lst, char *remark,FILE *fPF)
{
  int na = lst.Size();

  fprintf(fPF,"REMARK %s\n",remark);
  for(int i = 0 ; i < na; i++)
    {
      int ind = lst[i];
      pdb.WriteRecord(fram,ind,fPF);
    }

}
 

void GetWaterZProfile(FrameHandler &fram, PDB & pdb, ValList < int > &waterlst, double spacing, double min, double max, ValList < double > &hist)
{
#define HISTSPACING spacing
#define HISTMIN min
#define HISTMAX max
#define HISTSIZE ( (int) ((HISTMAX - HISTMIN)/ HISTSPACING + 1))
  // 
  // returns z profile of waterlst in raw count, and total in index 0
  //

  hist.Init(HISTSIZE,0.0);
  int len = waterlst.Size();
  for(int i = 0; i < len; i+=3)
    {
      XYZ v1;
      int p1 = waterlst[i] - 1;
      fram.GetPosition(p1,v1);
      int index = (int ) ( (v1.mZ - HISTMIN)/HISTSPACING ) ;
      if (index < 0 )
	index = 0;
      if (index >= HISTSIZE)
	index = HISTSIZE - 1;
      double val = hist[index] + 1.0;
      hist.Set(index,val);            
    }
  //  hist.Set(0,len);
}


void GetWaterBoxCount(FrameHandler &fram, PDB & pdb, ValList < int > &waterlst, XYZ & min, XYZ & max, double &count)
{

  // returns the water count in the box

  count = 0;
  int len = waterlst.Size();
  for(int i = 0; i < len; i+=3)
    {
      XYZ v1;
      int p1 = waterlst[i] - 1;
      fram.GetPosition(p1,v1);
      if( v1.mX < min.mX )continue;
      if( v1.mY < min.mY )continue;
      if( v1.mZ < min.mZ )continue;
      if( v1.mX > max.mX )continue;
      if( v1.mY > max.mY )continue;
      if( v1.mZ > max.mZ )continue;
      count++;
      //      printf(" pos [ %f %f %f ] min [ %f %f %f ] max [ %f %f %f ] count %f \n" , v1.mX,v1.mY,v1.mZ,min.mX,min.mY,min.mZ,max.mX,max.mY,max.mZ,count);      
    }
}
