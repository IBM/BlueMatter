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
 #ifndef _INCLUDED_PROBSPECMACS_ 
#define  _INCLUDED_PROBSPECMACS_ 
#define TYPE_REFERENCES 1



///////////////////////
//   class declaration
/////
#define STRUCTUREDEFINITION(x)   sprintf(tbufS,"\n%s\n",x);strcat(fPF,tbufS);
#define OPEN_BLOCK(x) sprintf(tbufS,"\n%s = \n{\n\t",x);strcat(fPF,tbufS);
#define OPEN_CLASS(x) sprintf(tbufS,"\n%s\n{\n\t",x);strcat(fPF,tbufS);
#define BLOCKSTR(a1) sprintf(tbufS,"{%s}",a1);strcat(fPF,tbufS);
#define BLOCK1(a1) sprintf(tbufS,"{%4d}",a1);strcat(fPF,tbufS);
#define BLOCK2(a1,a2) sprintf(tbufS,"{%4d,%4d}",a1,a2);strcat(fPF,tbufS);
#define BLOCK2X(a1,a2) sprintf(tbufS,"{%4d,  0x%x}",a1,a2);strcat(fPF,tbufS);
#define BLOCK3(a1,a2,a3) sprintf(tbufS,"{%4d,%4d,%4d}",a1,a2,a3);strcat(fPF,tbufS);
#define BLOCK3X(a1,a2,a3) sprintf(tbufS,"{%4d,%4d,  0x%x}",a1,a2,a3);strcat(fPF,tbufS);
#define BLOCK4(a1,a2,a3,a4) sprintf(tbufS,"{%4d,%4d,%4d,%4d}",a1,a2,a3,a4);strcat(fPF,tbufS);
#define BLOCK5(a1,a2,a3,a4,a5) sprintf(tbufS,"{%4d,%4d,%4d,%4d,%4d}",a1,a2,a3,a4,a5);strcat(fPF,tbufS);
#define END_BLOCK sprintf(tbufS,"\n};\n");strcat(fPF,tbufS);

//////////////////////
//   Misc formating
//////
#define LINE strcat(fPF,"\n")
#define CHECK(arg) strstr(bufS,arg)
#define COMA strcat(fPF," , ")
#define TAB strcat(fPF,"\t");

////////////////////////////
//    File I/O and Buffering
//////
#define STRNG(str) {char b[256]; sprintf(b,"%s",str); strcat(fPF,b);}
#define LINEIN fgets(bufS,1000,fPF);
#define BUFSTR(str) sscanf(bufS,"%s",str);
#define BUFINT(str) sscanf(bufS," %d",&str);
#define SCANSTR(str) fscanf(fPF," %s",str);
#define SCANumberInt(str) fscanf(fPF," %d",&str);
#define SCANumberInt2(s1,s2) fscanf(fPF," %d %d",&s1,&s2);
#define SCANumberInt3(s1,s2,s3) fscanf(fPF," %d %d %d",&s1,&s2,&s3);
#define SCANumberInt4(s1,s2,s3,s4) fscanf(fPF," %d %d %d %d",&s1,&s2,&s3,&s4);
#define SCANDBL(str) fscanf(fPF,"%lf",&str);
#define SCANDBL2(s1,s2) fscanf(fPF," %lf %lf",&s1,&s2);
#define SCANDBL3(s1,s2,s3) fscanf(fPF," %lf %lf %lf",&s1,&s2,&s3);
#define SCANDBL4(s1,s2,s3,s4) fscanf(fPF," %lf %lf %lf %lf",&s1,&s2,&s3,&s4);

///////////////////////////
//   String concatenation
//////
#define CATSTR(str) sprintf(tbufS,"%s",str);strcat(fPF,tbufS);
#define CATDELIM sprintf(tbufS,",\n\t");strcat(fPF,tbufS);
#define CATINT(str) sprintf(tbufS,"%4d",str);;strcat(fPF,tbufS);
#define CATSITE(s0,s1,s2) sprintf(tbufS,"{%2d,%2d,%4d}",s0,s1,s2);;strcat(fPF,tbufS);
#define CATINT2(s1,s2) sprintf(tbufS,"%4d %4d",s1,s2);;strcat(fPF,tbufS);
#define CATINT3(s1,s2,s3) sprintf(tbufS,"%4d %4d",s1,s2);;strcat(fPF,tbufS);
#define CATINT4(s1,s2,s3,s4) sprintf(tbufS,"%4d %4d %4d %4d",s1,s2,s3,s4);strcat(fPF,tbufS);
#define CATDBL(str) sprintf(tbufS,"%lf",&str);;strcat(fPF,tbufS);
#define CATDBL2(s1,s2) sprintf(tbufS,"%lf %lf",s1,s2);;strcat(fPF,tbufS);
#define CATDBL3(s1,s2,s3) sprintf(tbufS,"%lf %lf %lf",s1,s2,s3);;strcat(fPF,tbufS);
#define CATDBL4(s1,s2,s3,s4) sprintf(tbufS,"%lf %lf %lf %lf",s1,s2,s3,s4);strcat(fPF,tbufS);



////////////////////////
//    Fragspec type
//////
#define INIT_COUNT(cls,cnt,val) sprintf(bufS,"const int %s::%s = %d;\n",cls,cnt,val);strcat(fPF,bufS);
#define SITETYPESPEC(id) sprintf(bufS,"{%4d}",id);strcat(fPF,bufS);
#define SITESPEC(ms,cg,eps,rmin,eps14,rmin14) sprintf(bufS,"{ %lf, %lf * mChargeConversion, %lf, %lf, %lf, %lf}",ms,cg,eps,rmin,eps14,rmin14);strcat(fPF,bufS);
#define OPLSSITESPEC(ms,cg,eps,rmin,eps14,rmin14) sprintf(bufS,"{ %lf, %lf * mChargeConversion, %lf, %16.12f, %lf, %16.12f}",ms,cg,eps,rmin,eps14,rmin14);strcat(fPF,bufS);

#define LJSPEC(a1,a2)                                                             \
{                                                                                 \
    strstream a1SS; a1SS.precision(32); a1SS << a1 << endl;           \
    strstream a2SS; a2SS.precision(32); a2SS << a2 << endl;           \
    char *a1S = a1SS.str();                                           \
    char *a2S = a2SS.str();                                           \
    sprintf(bufS,"{  %s  ,  %s  }",a1S,a2S);strcat(fPF,bufS);         \
    delete a1S, a2S;                                                  \
}
#define LJ14SPEC(a1,a2) LJSPEC(a1,a2)

#define BONDSPEC(a1,a2,typ) { char A1S[10],A2S[10]; \
  if(a1<NumberInternalSites){sprintf(A1S,"{0,%4d}",a1);}else{sprintf(A1S,"{1,%4d}",a1 - NumberInternalSites);}\
  if(a2<NumberInternalSites){sprintf(A2S,"{0,%4d}",a2);}else{sprintf(A2S,"{1,%4d}",a2 - NumberInternalSites);}\
  sprintf(bufS,"{ %s, %s, %4d }",A1S,A2S,typ);strcat(fPF,bufS); }
#define ANGLESPEC(a1,a2,a3,typ) { char A1S[10],A2S[10],A3S[10]; \
  if(a1<NumberInternalSites){sprintf(A1S,"{0,%4d}",a1);}else{sprintf(A1S,"{1,%4d}",a1 - NumberInternalSites);}\
  if(a2<NumberInternalSites){sprintf(A2S,"{0,%4d}",a2);}else{sprintf(A2S,"{1,%4d}",a2 - NumberInternalSites);}\
  if(a3<NumberInternalSites){sprintf(A3S,"{0,%4d}",a3);}else{sprintf(A3S,"{1,%4d}",a3 - NumberInternalSites);}\
  sprintf(bufS,"{ %s, %s, %s, %4d }",A1S,A2S,A3S,typ);strcat(fPF,bufS); }
#define TORSIONSPEC(a1,a2,a3,a4,typ) { char A1S[10],A2S[10],A3S[10],A4S[10]; \
  if(a1<NumberInternalSites){sprintf(A1S,"{0,%4d}",a1);}else{sprintf(A1S,"{1,%4d}",a1 - NumberInternalSites);}\
  if(a2<NumberInternalSites){sprintf(A2S,"{0,%4d}",a2);}else{sprintf(A2S,"{1,%4d}",a2 - NumberInternalSites);}\
  if(a3<NumberInternalSites){sprintf(A3S,"{0,%4d}",a3);}else{sprintf(A3S,"{1,%4d}",a3 - NumberInternalSites);}\
  if(a4<NumberInternalSites){sprintf(A4S,"{0,%4d}",a4);}else{sprintf(A4S,"{1,%4d}",a4 - NumberInternalSites);}\
  sprintf(bufS,"{ %s, %s, %s, %s, %4d }",A1S,A2S,A3S,A4S,typ);strcat(fPF,bufS); }

#define BONDTYPESPEC(a1,a2) sprintf(bufS,"{ %lf, %lf }",a1,a2);strcat(fPF,bufS);
#define ANGLETYPESPEC(a1,a2) sprintf(bufS,"{ %lf, %lf * (M_PI/180.0) }",a1,a2);strcat(fPF,bufS);
#define UBTYPESPEC(a1,a2) sprintf(bufS,"{ %lf, %lf }",a1,a2);strcat(fPF,bufS);
#define TORSIONTYPESPEC(a1,a2,a3) sprintf(bufS,"{ %lf, %d, %lf * (M_PI/180.0) }",a1,a2,a3);strcat(fPF,bufS);
#define IMPROPERTYPESPEC(a1,a2,a3) sprintf(bufS,"{ %lf, %d, %lf * (M_PI/180.0) }",a1,a2,a3);strcat(fPF,bufS);
#define EXCLUDESITEPAIR(a1,a2,a3) if(a2>=0){sprintf(bufS,"{0,%4d}, {0,%4d}",a1,a3);} \
              else{sprintf(bufS,"{0,%4d}, {1,%4d}",a1,a3-1);}strcat(fPF,bufS);





#define XREC1(str) {char b[256]; sprintf(b,"<%s>",str);strcat(fPF,b);}
#define XREC0(str) {char b[256]; sprintf(b,"</%s>\n",str);strcat(fPF,b);}

#define XSFIELD(name,val) {char bb[256];sprintf(bb,"<%s>%s</%s>",name,val,name);strcat(fPF,bb);}
#define XIFIELD(name,val) {char bb[256];sprintf(bb,"<%s>%d</%s>",name,val,name);strcat(fPF,bb);}
#define XDFIELD(name,val) {char bb[256];sprintf(bb,"<%s>%lf</%s>",name,val,name);strcat(fPF,bb);}

#endif
