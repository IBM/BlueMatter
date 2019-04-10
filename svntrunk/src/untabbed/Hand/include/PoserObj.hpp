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
 
#ifndef _INCLUDED_POSEROBJ__
#define _INCLUDED_POSEROBJ__
#include <stdio.h>
#include <stdlib.h>


#include <FFUtils.hpp>

#define MAX_LEN 6
#define EPS 1e-7

class PoserObject;

class Polygon
{
public:
  int vis;
  int parent;
  int m_n;
  int m_vert[MAX_LEN];
  int m_norm[MAX_LEN];


  Polygon(){vis = 1; parent = m_n = 0;}
  Polygon(char *config,Vector *vertlist)
  {
    Config(config,vertlist);
  }
  void Config(char *config, Vector *vertlist)
  {
#define CHARLEN 64
    char v[12][CHARLEN];
    char temp[CHARLEN];
    int n = sscanf(config,"%s %s %s %s %s %s %s %s",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7]);
    assert(n>0);
    m_n = n;
    for(int i =0;i<n;i++)
      {
  char *vS =  v[i];
  int cnt = 0; int nl[2];
  int len = strlen(v[i]);
  int j = 0;
  char * cPC = v[i];
  while(j < len)
    {
      if(cPC[j] == '/')
        {
    cPC[j] = '\0';
    nl[cnt] = j+1;
    cnt++;
        }
      j++;
    }
  sscanf(cPC,"%d",&m_vert[i]);
  if(cnt == 2)
    {
      sscanf(&cPC[ nl[1] ],"%d",&m_norm[i]);
    }
      }	
  }
  Polygon & operator=(Polygon &p)
  {
    vis = p.vis;
    parent = p.parent;
    m_n = p.m_n;
    for(int i =0; i < m_n; i++)
      {
  m_vert[i] = p.m_vert[i];
  m_norm[i] = p.m_norm[i];
      }
    return *this;
  }
  int CheckSides(Vector *vert,double min)
  {
    Vector v;
    double m = min * min;
    for(int i = 0;i<m_n;i++)
      {
  int j = (i == m_n - 1) ? 0 : i+1;
  v = vert[ m_vert[i] ] - vert[ m_vert[j] ];
  if ( v / v < m)
    {
      v = vert[ m_vert[i] ];
      printf(" vert %d = %d < %f %f %f > \n", m_vert[i],m_vert[j], v[0],v[1],v[2]);
      assert (0);
      return 0;
    }
      }
    return 1;
  }
  int Vert(int i)
  {
    assert( i >=0);
    assert( i < m_n);
    return m_vert[i];
  }
  int Norm(int i)
  {
    assert( i >=0);
    assert( i < m_n);
    return m_norm[i];
  }
  int N()
  {
    return m_n;
  }
  void Center(Vector *vert,Vector &c)
  {
    c = Vector(0.0,0.0,0.0);
    for(int i = 0;i<m_n;i++)
      {
  c += vert[ m_vert[i] ];
      }
    c /= (double) m_n;
  }
  void PolygonNorm(Vector *verts,Vector &n )
  {
    Vector v1 = verts[m_vert[1]] - verts[m_vert[0]];
    Vector v2 = verts[m_vert[2]] - verts[m_vert[0]];
    n = v1 * v2;
    n /= sqrt(n / n);
    return; 
  }
  void SetNormDirection(Vector *verts,Vector &axis)
  {
    Vector norm;
    PolygonNorm(verts,norm);
    if(norm / axis < 0.0)
      {
  int temp = m_vert[1]; m_vert[1] = m_vert[2]; m_vert[2] = temp;	
  temp = m_norm[1]; m_norm[1] = m_norm[2]; m_norm[2] = temp;	
      }

  }

  int DoesLineIntersect(Vector &O1, Vector &axis,Vector *verts, double &t, Vector &p)
  {
    Vector n;
    PolygonNorm(verts,n);

    double Vd = axis / n;
    if(fabs(Vd) < EPS)
      return 0;

    int inside = 1;
    for(int i = 1; i<= m_n; i++)
      {
  int j = (i == m_n) ? i : 0;
  Vector V0 = O1 - verts[ m_vert[i-1] ];
  Vector t = verts[ m_vert[j] ] - verts[ m_vert[i-1] ];
  double proj = (n * t) / V0;
  if(proj < 0.0)
    return 0;
      }

    Vector V0 = verts[ m_vert[0] ];
    double D = V0 / n;
    double D2 =  -((n / O1) + D);
    t = D2 / Vd;
    p = O1 + (t * axis);
    return 1;
  }
  double SqDistToPoint(Vector &p, Vector *verts)
  {
    Vector n;
    PolygonNorm(verts,n);
    int closest = 0;
    Vector V0 = p - verts[ m_vert[0] ];
    double bestd = V0 / V0;
    int inside = 1;
    for(int i = 1; i<= m_n; i++)
      {
  int j = (i == m_n) ? i : 0;
  V0 = p - verts[ m_vert[i-1] ];
  Vector t = verts[ m_vert[j] ] - verts[ m_vert[i-1] ];
  double proj = (n * t) / V0;
  if(proj < 0.0)
    inside = 0;
  double d = V0 / V0;
  if (d < bestd)
    {
      bestd = d;
      closest = i-1;
    }
      }
    if(inside)
      {
  Vector V0 = p - verts[ m_vert[0] ];
  double projn = n / V0;
  return projn * projn;
      }
    else
      {
  // point projection is outside of polygon
  // Find closest vertex, and determine closest point on either edge segment
  // return sqdist of the closest of two.
  Vector BV = verts[ m_vert[closest] ];
  int j = (closest - 1) > 0 ? closest : m_n-1;
  Vector BV1 = verts[ m_vert[ j ] ] - BV;
  int k = (closest + 1) < m_n ? closest : 0;
  Vector BV2 = verts[ m_vert[ k ] ] - BV;
  double h1 = sqrt(BV1 / BV1);	BV1 /= h1;
  double h2 = sqrt(BV2 / BV2);	BV2 /= h2;
  Vector P = p - BV;
  double t1 = P / BV1;
  double t2 = P / BV2;
  if(t1 < 0.0) t1 = 0.;
  if(t1 > h1) t1 = h1;
  if(t2 < 0.0) t2 = 0.;
  if(t2 > h2) t2 = h2;
  Vector P1 = p - (BV + (t1 * BV1));
  Vector P2 = p - (BV + (t2 * BV2));
  double d1 = P1 / P1;
  double d2 = P2 / P2;
  return ( d1 < d2 ) ? d1 : d2;
      }
  }

  void WritePovray(Vector *vert,FILE *fPF, int query = 0)
  {
    for(int i = 0 ; i < m_n; i++)
      {
  int j = (i == 0) ? m_n-1 : i-1;
  int vi = m_vert[i];
  int vj = m_vert[j];
  Vector v0 = vert[vi];
  Vector v1 = vert[vj];
  if(query)
    {
      fprintf( fPF, " cylinder { < %f , %f, %f> , < %f , %f,  %f>,  QCYL_RAD()  QMAT() }\n", 
         v0[0],v0[1],v0[2],v1[0],v1[1],v1[2]);
      
      fprintf( fPF, " sphere { < %f , %f, %f> ,  QPOINT_RAD()  QMATY() }\n",
         v0[0],v0[1],v0[2]);
    }
  else
    {
      fprintf( fPF, " cylinder { < %f , %f, %f> , < %f , %f,  %f>,  CYL_RAD()  MAT() }\n", 
         v0[0],v0[1],v0[2],v1[0],v1[1],v1[2]);
      
      fprintf( fPF, " sphere { < %f , %f, %f> ,  POINT_RAD()  MATY() }\n",
         v0[0],v0[1],v0[2]);
    }
      }

  }

};

class Group
{
public:
  char name[256];
  int start;
  int len;
  Vector m_O;
  Group(){start = len = 0; name[0] = '\0';}
  int operator==(Group &g)
  {
    return (0 == strcmp(name,g.name));
  }
  int operator==(char *str)
  {
    return (0 == strcmp(name,str));
  }
  Group & operator=(Group &g)
  {
    strcpy(name,g.name);
    start = g.start;
    len = g.len;
    return *this;
  }

  void SetOrigin (Vector &v)
  {
    m_O = v;
  }
  int Move(PoserObject *ref, Matrix &R, Vector &D, PoserObject *cur);
  int Move(PoserObject *ref, Matrix &R0, Vector &D0, Matrix &R, Vector &D, PoserObject *cur);
};

class PoserObject
{
public:
  Vector * m_vert;
  Vector * m_norm;
  Polygon * m_face;
  Group *m_group;
  int m_nverts,m_nnorms,m_ngroups,m_nfaces;

  PoserObject( )
  {
    m_nverts = m_nnorms = m_nfaces = m_ngroups = 0;
    m_vert = m_norm = NULL;
    m_face = NULL;
    m_group = NULL;
  }
  int Move(Matrix &R, Vector &T)
  {
    for(int i = 1; i <= m_nverts; i++)
      {
  m_vert[i] = ( R / m_vert[i]) + T;
      }
    for(int i = 1; i <= m_nnorms; i++)
      {
  m_norm[i] = R / m_norm[i];
      }
    return 1;
  }
  void operator= (PoserObject &obj)
  {
    if(m_nverts < obj.m_nverts)
      {
  if(m_nverts>0)
    delete [] m_vert;
  m_vert = new Vector[obj.m_nverts + 1];	
      }
    m_nverts = obj.m_nverts;
    for(int i = 1; i <= m_nverts; i++)
      {
  m_vert[i] = obj.m_vert[i];
      }

    if(m_nnorms < obj.m_nnorms)
      {
  if(m_nnorms>0)
    delete [] m_norm;
  m_norm = new Vector[obj.m_nnorms + 1];	
      }
    m_nnorms = obj.m_nnorms;
    for(int i = 1; i <= m_nnorms; i++)
      {
  m_norm[i] = obj.m_norm[i];
      }

    if(m_nfaces < obj.m_nfaces)
      {
  if(m_nfaces>0)
    delete [] m_face;
  m_face = new Polygon[obj.m_nfaces + 1];	
      }
    m_nfaces = obj.m_nfaces;
    for(int i = 1; i <= m_nfaces; i++)
      {
  m_face[i] = obj.m_face[i];
      }

    if(m_ngroups < obj.m_ngroups)
      {
  if(m_ngroups>0)
    delete [] m_group;
  m_group = new Group[obj.m_ngroups + 1];	
      }
    m_ngroups = obj.m_ngroups;
    for(int i = 1; i <= m_ngroups; i++)
      {
  m_group[i] = obj.m_group[i];
      }
  }
  PoserObject(FILE *fPF)
  {
    char bufS[256];
    m_nverts = m_nnorms = m_ngroups = m_nfaces = 0;
    DetermineCounts(fPF);
    m_vert = new Vector[m_nverts + 1];
    if(m_nnorms > 0)
      m_norm = new Vector[m_nnorms + 1];
    else
      m_norm = NULL;
    m_face = new Polygon[m_nfaces + 1];
    m_group = new Group[m_ngroups + 1];

    int nv,nf,nn,ng;
    nv = nf = nn = ng = 0;
    double x,y,z;
    rewind(fPF);

    fgets(bufS,256,fPF);
    while(!feof(fPF))
      {
  char* c = bufS;
  switch(*c)
    {
    case ' ':
    case '#':
      break;
    case 'f':
      nf++;  // first face is 1
      m_face[nf].Config(&c[1],m_vert); 
      break;
    case 'g':	    
      m_group[ng].len = nf - m_group[ng].start;
      ng++; 
      m_group[ng].start = nf + 1; 
      sscanf(&c[1],"%s",m_group[ng].name);
      break;
    case 'v':
      switch(c[1])
        {
        case ' ':
    sscanf(&c[2],"%lf %lf %lf",&x,&y,&z);
    nv++; // use first vertex is 1
    m_vert[nv] = Vector(x,y,z);
    break;
        case 'n':
// 		sscanf(&c[2],"%lf %lf %lf",&x,&y,&z);
// 		nn++; // first norm is 1
// 		m_norm[nn] = Vector(x,y,z);
    break;
        case 't': break;
        }
    }
  fgets(bufS,256,fPF);
      }
    m_group[ng].len = nf - m_group[ng].start; 

    if(m_nnorms == 0) // determine normals as the average normal from each polygon
      {	
  Vector nrm;
  m_nnorms = m_nverts;
  m_norm = new Vector[m_nnorms + 1];
  for(int i=1;i<= m_nfaces; i++)
    {
      m_face[i].PolygonNorm(m_vert,nrm);
      for(int j=0;j<m_face[i].N();j++)
        {
    int k = m_face[i].Vert(j);
    m_norm[k] += nrm;
    m_face[i].m_norm[j] = k;
        }
    }
  for(int i=1;i<= m_nverts; i++)
    {
      m_norm[i] /= sqrt( m_norm[i] / m_norm[i] );
    }
      }

  }

  int DetermineCounts(FILE *fPF)
  {
    char bufS[256];
    fgets(bufS,256,fPF);
    while(!feof(fPF))
      {
  char* c = bufS;
  switch(*c)
    {
    case ' ':
    case '#':
      break;
    case 'f':
      m_nfaces++; break;
    case 'g':
      m_ngroups++; break;
    case 'v':
      switch(c[1])
        {
        case ' ':
    m_nverts++; break;
        case 'n':
    m_nnorms++; break;
        case 't': break;
        default:
    printf("Line unparsible: |%s|\n",bufS);
    assert(0);
        }
    }
  fgets(bufS,256,fPF);
      }
    return 1;
  }

  Vector & Vert(int i)
  {
    assert(i >= 0);
    assert(i < m_nverts);
    return m_vert[i];
  }
  Vector & Norm(int i)
  {
    assert(i >= 0);
    assert(i < m_nnorms);
    return m_norm[i];
  }
  Polygon & Poly(int i)
  {
    assert(i >= 0);
    assert(i < m_nfaces);    
    return m_face[i];
  }

  void Scale(Vector &scl)
  {
    for(int i = 0 ; i < m_nverts ; i++)
      {
  m_vert[i] = Vector(m_vert[i][0] *  scl[0], 
         m_vert[i][1] *  scl[1], 
         m_vert[i][2] *  scl[2]);
      }
  }


  int GetGroup(char *name, Group & grp)
  {
    
    for(int i = 1; i <= m_ngroups; i++)
      {
  if( m_group[i] == name )
    {
      grp =  m_group[i];
      return 1;
    }
      }
    return 0;
  }


  int FindNextVertexWithinRangeOfPoint(int start,double rad,Vector &p,int &vnum, Vector &v)
  {
    Vector p1;
    double rr = rad * rad;
    for(int i = start; i <= m_nverts; i++)
      {
  double r = m_vert[i] / m_vert[i];
  if(r <= rr)
    {
      v = m_vert[i]; 
      vnum = i;
      return 1;
    }
      }
    return 0;
  }
  int FindNextFaceWithinRangeOfPoint(int start,double rad,Vector &p,int &pnum, Polygon &poly)
  {
    Vector p1;
    double rr = rad * rad;
    for(int i = start; i <= m_nverts; i++)
      {
  double r = m_face[i].SqDistToPoint(p,m_vert);
  if(r <= rr)
    {
      poly = m_face[i]; 
      pnum = i;
      return 1;
    }
      }
    return 0;
  }
  int FindNextFaceIntersection(int start,Vector &O1,Vector &axis,int &pnum, Polygon &poly,Vector &intersect)
  {
    Vector p1;
    for(int i = start; i <= m_nverts; i++)
      {
  double t;
  if( m_face[i].DoesLineIntersect(O1,axis,m_vert, t, intersect))
    {
      poly = m_face[i]; 
      pnum = i;
      return 1;
    }
      }
    return 0;
  }

  void WritePovrayVerts(FILE *fPF)
  {
    for(int j = 1; j <= m_nverts; j++)
      WritePovrayVert(j,fPF);
  }
  void WritePovrayFaces(Group &g,FILE *fPF)
  {
    for(int j = g.start; j <= g.start + g.len; j++)
      WritePovrayFace(j,fPF);
  }
  void WritePovrayVert(int j,FILE *fPF)
  {
    fprintf( fPF, " sphere { < %f , %f, %f> ,  POINT_RAD()  MATX() }\n",
       m_vert[j][0],m_vert[j][1],m_vert[j][2] );
    
  }
  void WritePovrayFaces(FILE *fPF,int query = 0)
  {
    for(int j = 1; j <= m_nfaces; j++)
      WritePovrayFace(j,fPF,query);
  }  
  void WritePovrayFace(int j,FILE *fPF, int query = 0)
  {
    m_face[j].WritePovray(m_vert,fPF, query);
  }

  void Scale(double scl)
  {
    for(int j = 1; j <= m_nverts; j++)
      m_vert[j] *= scl;
  }
};


int Group::Move(PoserObject *ref, Matrix &R, Vector &D, PoserObject *cur)
{
  if(ref == NULL)
    return 0;
  if(cur == NULL)
    return 0;
  for(int i = start; i <= start + len; i++)
    {
      Polygon *face = &cur->m_face[i];
      for(int j = 0 ; j < face->m_n; j++)
  {
    int k = face->m_vert[j];
    int m = face->m_norm[j];
    cur->m_vert[k] = ( (ref->m_vert[k] - m_O) / R) + D;
    cur->m_norm[m] = ref->m_norm[m] / R;
  }
    }    
  return len;
}

int Group::Move(PoserObject * ref,Matrix &R0, Vector &D0, Matrix &R, Vector &D, PoserObject *cur)
{
  if(cur == NULL)
    return 0;
  Matrix Rc;
  Rc = (!R0) / R;
  for(int i = start; i <= start + len; i++)
    {
      Polygon *face = &cur->m_face[i];
      for(int j = 0 ; j < face->m_n; j++)
  {
    int k = face->m_vert[j];
    int m = face->m_norm[j];
    cur->m_vert[k] = ( (ref->m_vert[k] - D0) / Rc) + D;
    cur->m_norm[m] = ref->m_norm[m] / Rc;
  }
    }    
  return len;
}


#endif
