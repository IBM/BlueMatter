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
 #ifndef _INCLUDED_CSGMODEL__
#define _INCLUDED_CSGMODEL__

#include <FFUtils.hpp>
#include <PoserObj.hpp>

#define MAX_TRIANGLES 10000

class Line
{
public:
  Vector m_pos;
  Vector m_axis;

  void Define(Vector &v1, Vector &v2);
  double PointProjection(Vector &p, Vector &proj);
  int ClosestPointBetweenLines(Line &L2, double &t1);
  void LineMark(double t, Vector &mark);

  Line & operator=(Line &ln)
  {m_pos = ln.m_pos; m_axis = ln.m_axis; return *this;}

  void Move(Matrix &R,Vector &T)
  {m_pos = (R / m_pos) + T; m_axis = R / m_axis;}
};

class Plane
{
public:
  Vector m_pos;
  Vector m_norm;

  Plane(){}
  double PointProjection(Vector &p0,Vector &proj);

  double D(){return m_pos / m_norm; }
  Plane & operator=(Plane &pln){m_pos = pln.m_pos; m_norm = pln.m_norm; return *this;}
  void Move(Matrix &R, Vector &T){m_pos = (R / m_pos) + T; m_norm = R / m_norm;}

};

class Triangle
{
public:
  Vector m_v1,m_v2,m_v3;
  void Define(Vector &v1, Vector &v2,Vector &v3){  m_v1 = v1;  m_v2 = v2; m_v3 = v3;}
  void Norm(Vector &norm);
  int IsPointProjectionInTriangle(Vector &p);
  int IsPointWithinRange(Vector &p,double range,Vector & closest_point, int bail_asap);
  Triangle & operator=(Triangle &tri){m_v1 = tri.m_v1;m_v2 = tri.m_v2;m_v3 = tri.m_v3;return *this;}

  void Move(Matrix &R, Vector &T){m_v1 = (R / m_v1) + T; m_v2 = (R / m_v2) + T; m_v3 = (R / m_v3) + T; }

};

class Sphere
{
public:
  Vector m_pos;
  double m_rad;
  Sphere *m_next;

  Sphere(){m_rad = 0;m_next = NULL;}
  void Define(Vector p, double r){m_pos = p; m_rad = r;}
  int Read(FILE *fPF);
  void Write(FILE *fPF);  
  void PointProjection(Vector &p, Vector &proj);
  int AddSurfacePoints(char *name,double height_thresh,int vertoffset, FILE *fPF);
  int DoesTriangleIntersect(Triangle &tri);  
  int RayIntersection(Line &L,double &r1,double &r2);
  int IsPointInside(Vector &v);

  Sphere & operator=(Sphere &s){m_pos = s.m_pos; m_rad = s.m_rad; return *this;}

  int DoSpheresIntersect(Sphere &s)
  {
    Vector v= s.m_pos - m_pos;
    double d = v/v;
    double r = s.m_rad + m_rad;
    if(r*r >= d)
      return 1;
    return 0;
  }

  void Move(Matrix &R, Vector &T){m_pos = R / m_pos + T;}

};

class Cylinder
{
public:
  Line m_line;
  double m_height, m_rad;
  Cylinder *m_next;

  Cylinder(){m_height = m_rad = 0;m_next = NULL;}
  void Define(Vector &base, Vector &top, double rad, double height);
  int DoesCapsuleIntersectTriangle(Triangle &tri);
  int DoesCapsuleIntersectCapsule(Cylinder &cyl);
  int DoesCapsuleIntersectSphere(Sphere &sph);
  Read(FILE *fPF);
  void Write(FILE *fPF);
  int AddSurfacePoints(char *name,int Nr, int Nslabs,int nvert_offset, FILE *fPF);

  Cylinder & operator=(Cylinder &s){m_line = s.m_line; m_rad = s.m_rad; m_height = s.m_height; return *this;}

  void Move(Matrix &R, Vector &T){m_line.Move(R,T);}


};

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////



void Line::Define(Vector &v1, Vector &v2)
{
  m_pos = v1;
  m_axis = v2 - v1;
  m_axis /= sqrt(m_axis / m_axis);
}
double Line::PointProjection(Vector &p, Vector &proj)
{
  double t = (p - m_pos) / m_axis;
  proj = t * m_axis + m_pos;
  return t;
}  
void Line::LineMark(double t, Vector &mark)
{
  mark = t * m_axis + m_pos;
}
int Line::ClosestPointBetweenLines(Line &L2, double &t1)
{

  Vector RC = L2.m_pos - m_pos;
  Vector &d2 = L2.m_axis;
  Vector D1CD2 = m_axis * d2; 
    
  double N = D1CD2 / D1CD2;
  if(fabs(N) < EPS) // parallel lines
    {
      // parallel lines
      t1 = 0.0;
      return 1;
    }
  else
    {
      t1 = RC[0] * (d2[1] * D1CD2[2] - d2[2] * D1CD2[1]) -
  RC[1] * (d2[0] * D1CD2[2] - d2[2] * D1CD2[0]) +
  RC[2] * (d2[0] * D1CD2[1] - d2[1] * D1CD2[0]);
      t1 /= N;
    }    
  return 0;
}

double Plane::PointProjection(Vector &p0,Vector &proj)
{
  Vector v = p0 - m_pos;
  double d = v / m_norm;
  proj = p0 - d * m_norm; 
  return d;
}


void Triangle::Norm(Vector &norm)
{
  norm = (m_v2 - m_v1) * (m_v3 - m_v1);
  norm /= sqrt(norm / norm);
}
int Triangle::IsPointProjectionInTriangle(Vector &p)
{
  // compute whether the projection of p lies inside a triangle given its verticies
  Vector c1,c2,c3;
  double t1,d;
    
  // For each vertex, test if point projection on side opposite vertex forms acute angle
  Line L1;
  L1.Define(m_v2,m_v3);
  t1  = L1.PointProjection(p,c1);
  c2 = m_v1 - c1;
  c3 = p - c1;
  d = c3 / c2;
  if (d < -EPS)
    return 0;

  L1.Define(m_v1,m_v2);
  t1  = L1.PointProjection(p,c1);
  c2 = m_v3 - c1;
  c3 = p - c1;
  d = c3 / c2;
  if (d < -EPS)
    return 0;

  L1.Define(m_v1,m_v3);
  t1  = L1.PointProjection(p,c1);
  c2 = m_v2 - c1;
  c3 = p - c1;
  d = c3 / c2;
  if (d < -EPS)
    return 0;    
  return 1;
} 

int Triangle::IsPointWithinRange(Vector &p,double range,Vector & closest_point, int bail_asap)
{
  Vector norm;

  // If bail_asap is true, return as soon as within range: the closest_point is not necessarily the closest.
  int withinrange = 0;
  double d,atleast;
  if(IsPointProjectionInTriangle(p))
    {
      Vector proj;
      Vector v = p - m_v1;
      Norm(norm);
      d = v / norm;
      proj = p - d * norm; 
      v -= proj;
      atleast = sqrt(p / p);
      withinrange =  (atleast <= range) ? 1 : 0;
      closest_point = proj;
      return withinrange;
    }

  double t1,t2,t3,t;
  Vector pc1,pc2,pc3;
  // Find distance of point to side 1;

  Line L1;
  Vector proj;
  L1.Define(m_v1,m_v2);
  t1 = L1.PointProjection(p,proj);
  if(t1 < 0.0) t1 = 0.0;
  proj = m_v2 - m_v1;  t2 = sqrt( proj / proj);
  if(t1 > t2) t1 = t2;
  L1.LineMark(t1,pc1);
  proj = p - pc1;
  t1 = sqrt( proj / proj);
  atleast = t1;
  closest_point = pc1;
  if(t1 < range && bail_asap)
    return 1;

  // Find distance of point to side 2
  L1.Define(m_v2,m_v3);
  t2 = L1.PointProjection(p,proj);
  if(t2 < 0.0) t2 = 0.0;
  proj = m_v3 - m_v2;  t = sqrt( proj / proj);
  if(t2 > t) t2 = t;
  L1.LineMark(t2,pc2);
  proj = p - pc2;
  t2 = sqrt( proj / proj);
  atleast = t2;
  closest_point = pc2;
  if(t2 < range && bail_asap)
    return 1;


  // Find distance of point to side 3
  L1.Define(m_v1,m_v3);
  t3 = L1.PointProjection(p,proj);
  if(t3 < 0.0) t3 = 0.0;
  proj = m_v3 - m_v1;  t = sqrt( proj / proj);
  if(t3 > t) t3 = t;
  L1.LineMark(t3,pc3);
  proj = p - pc3;
  t3 = sqrt( proj / proj);
  atleast = t3;
  closest_point = pc3;
  if(t3 < range && bail_asap)
    return 1;

  return 0;
}

int Sphere::RayIntersection(Line &L,double &r1,double &r2)
{
  // sphere (p-pc)(p-pc) - r^2 = 0
  // ray p = d t + p0
  // (-b +/ sqrt(b2 -4ac))/2a;
  // a = d * d
  // b = 2d(p0-pc)
  // c = (p0-pc)(p0-pc) - r^2

  Vector p = L.m_pos - m_pos;
  double a = L.m_axis / L.m_axis;
  double b = 2.0 * (L.m_axis / p);
  double c = p / p - m_rad * m_rad;

  double dis = b*b - 4 * a * c;
  if(dis < 0)
    return 0;
  dis = sqrt(dis);
  b = -b;
  r1 = (b + dis)/ (2 * a);
  r2 = (b - dis)/ (2 * a);
  return 1;
}
int Sphere::Read(FILE *fPF)
{
  char bufS[256];
  fgets(bufS,256,fPF);
  while(!feof(fPF))
    {
      char token[32];
      double x,y,z;
      int n = sscanf(bufS,"%s %lf %lf %lf %lf",token,&x,&y,&z,&m_rad);
      if(!strcmp("SPHERE",token))
  { 
    assert(n == 5);
    m_pos = Vector(x,y,z);
    return 1;
  }
      fgets(bufS,256,fPF);
    }
  return 0;
}
void Sphere::Write(FILE *fPF)
{
  fprintf(fPF," SPHERE %f %f %f %f\n",m_pos[0],m_pos[1],m_pos[2],m_rad);
}

void Sphere::PointProjection(Vector &p, Vector &proj)
{
  Line L1;
  L1.Define(m_pos,p);
  L1.LineMark(m_rad,proj);  
}
int Sphere::IsPointInside(Vector &v)
{
  Vector p = v - m_pos;
  if (p / p < m_rad*m_rad)
    return 1;
  return 0;
}
void Cylinder::Define(Vector &base, Vector &top, double rad, double height)
{
  m_rad = rad;
  m_height = height;
  m_line.Define(base,top);
}
int Cylinder::Read(FILE *fPF)
{
  char bufS[256];
  fgets(bufS,256,fPF);
  while(!feof(fPF))
    {
      char token[32];
      double x,y,z,x1,y1,z1;
      int n = sscanf(bufS,"%s %lf %lf %lf %lf %lf %lf %lf ",token,&x,&y,&z,&x1,&y1,&z1,&m_rad);
      if(!strcmp("CYLINDER",token))
  { 
    assert(n == 8);
    m_line.m_pos = Vector(x,y,z);
    m_line.m_axis = Vector(x1,y1,z1) - m_line.m_pos;
    m_height = sqrt( m_line.m_axis / m_line.m_axis );
    m_line.m_axis /= m_height;	  
    return 1;
  }
      fgets(bufS,256,fPF);
    }
  return 0;
}
void Cylinder::Write(FILE *fPF)
  {
    Vector top;
    m_line.LineMark(m_height,top);
    fprintf(fPF," CYLINDER %f %f %f %f %f %f %f \n",
      m_line.m_pos[0],m_line.m_pos[1],m_line.m_pos[2],
      top[0],top[1],top[2],m_rad);
  }

int Cylinder::DoesCapsuleIntersectCapsule(Cylinder &cyl)
{
  double d;
  Vector pt,tpt;
  int parallel = m_line.ClosestPointBetweenLines(cyl.m_line, d);
  
  if(parallel)
    {
      double d1,d2,d3;
      Vector RC,tp,cp,tpt;
      
      d2 = cyl.m_line.PointProjection(m_line.m_pos,tp);
      RC = m_line.m_pos - tp;
      d1 = RC / RC; // sqr dist of lines
      
      m_line.LineMark(m_height,tp);
      d3 = cyl.m_line.PointProjection(tp,tpt);
      
      if( fabs(d2) > fabs(d3)) // closest point to origin
  d2 = d3;
      if(d2 < 0.0)
  d = sqrt( d2*d2 + d1 );
      else if (d2 > cyl.m_height)
  {
    d2 -= cyl.m_height;
    d = sqrt( d2*d2 + d1 );
  }
      else
  d = sqrt(d1);
      
    }
  else
    {
      if(d < 0.0)d = 0;
      if(d > cyl.m_height)d = cyl.m_height;
      cyl.m_line.LineMark(d,tpt);
      
      
      Vector cp = tpt - m_line.m_pos;
      d = m_line.m_axis / cp;
      if(d < 0.0)d = 0;
      if(d > m_height)d = m_height;
      m_line.LineMark(d,pt);
      tpt -= pt;
      d = sqrt(tpt / tpt);
    }
  
  if(d < (m_rad + cyl.m_rad))
    { return 1;}
  else 
    return 0;
  
} 
int Cylinder::DoesCapsuleIntersectSphere(Sphere &sph)
{
  double d;
  Vector pt,tpt;
  
  d = m_line.PointProjection(sph.m_pos, pt);
  
  if(d < 0.0)d = 0;
  if(d > m_height)d = m_height;
  m_line.LineMark(d,tpt);
  
  
  Vector cp = tpt - sph.m_pos;
  d = sqrt(cp / cp);
  
  if(d < (m_rad + sph.m_rad))
    { return 1;}
  else 
    return 0;
  
} 
int Cylinder::DoesCapsuleIntersectTriangle(Triangle &tri)
{
  Vector &v1 = tri.m_v1;
  Vector &v2 = tri.m_v2;
  Vector &v3 = tri.m_v3;
  Vector base,top;
  base = m_line.m_pos;
  m_line.LineMark(m_height,top);
  Plane pln;
  pln.m_pos = v1; 
  tri.Norm(pln.m_norm);
  
  Vector baseproj, topproj;
  m_line.LineMark(m_height,top);
  pln.PointProjection(m_line.m_pos,baseproj);
  pln.PointProjection(top,topproj);
  
  int crosses_plane = ((base - baseproj) / (top - baseproj) < 0.0) ? 1 : 0;
  Vector tv = m_line.m_pos - baseproj;    
  double hb = sqrt(tv / tv);
  tv = top - topproj;
  double ht = sqrt(tv / tv);
  if(!crosses_plane && hb > m_rad && ht > m_rad)
    return 0;
  
  double d=0;
  Vector cp1,cp2;
  if(tri.IsPointWithinRange(base, d, cp1,1))
    return 1;
  if(tri.IsPointWithinRange(top, d, cp2,1))
    return 1;
  
  Vector cpd = cp2 - cp1;
  double sh = sqrt(cpd / cpd);
  cpd /= sh;
  
  Vector pt,cylp;
  Line L1;
  L1.Define(cp1,cp2);
  L1.ClosestPointBetweenLines(m_line, d);
  if(d < 0.0)d = 0;
  if(d > sh)d = sh;
  L1.LineMark(d,pt);
  
  cylp = pt - m_line.m_pos;
  
  d = m_line.m_axis / cylp;
  if(d < 0.0)d = 0;
  if(d > m_height)d = m_height;
  m_line.LineMark(d,cylp);
  
  cylp -= pt;
  d = sqrt(cylp / cylp); 
  if(d < m_rad)
    return 1;
  
  return 0;
  
}

int Cylinder::AddSurfacePoints(char *name,int Nr, int Nslabs,int nvert_offset, FILE *fPF)
{
  int N = Nr * (Nslabs + 1);
  Vector vert[MAX_TRIANGLES],v;
  Polygon face[MAX_TRIANGLES];

  int NP = 2 * N;

  
  int nv,nf;
  nv = nf = 0;
  double drad = 2 * M_PI / Nr;
  double dh = m_height / Nslabs;
  Vector p;
  p[2] = 0;
  for(int j = 0 ; j <= Nslabs; j++)
    {
      double h = j*dh;
      for(int i = 0 ; i < Nr; i++)
  {
    double ang = i * drad;
    //if(j % 2 == 0)
    ang += (j - 1) * 0.5  * drad;
    p[0] = cos(ang);
    p[1] = sin(ang);
    nv++;
    vert[nv] = p * m_rad;
    vert[nv][2] = h;
  }
    }
  for(int j = 0 ; j < Nslabs; j++)
    {
      int v1,v2,v3;
      for(int i = 0 ; i < Nr; i++)
  {
    v1 = i + j * Nr;
    //	  v2 = (i+1 == Nr) ? j*Nr + 1: j*Nr + i + 1;
    v2 = (i+1 == Nr) ? j*Nr : j*Nr + i + 1;
    v3 = i + (j+1)*Nr;
    nf++;
    face[nf].m_n = 3;
    face[nf].m_vert[0] = v1 + 1;
    face[nf].m_vert[1] = v2 + 1;
    face[nf].m_vert[2] = v3 + 1;
    face[nf].CheckSides(vert,EPS);

    v1 = ( i + 1 == Nr) ? (j + 1)*Nr : v3 + 1;
    nf++;
    face[nf].m_n = 3;
    face[nf].m_vert[0] = v2 + 1;
    face[nf].m_vert[1] = v3 + 1;
    face[nf].m_vert[2] = v1 + 1;
    face[nf].CheckSides(vert,EPS);

  }
    }

  Vector p1,p2,p3;

  p3 = m_line.m_axis;
  p1 = Vector(1.0,0.0,0.0);
  if((fabs(p3 / p1) - 1.0) < EPS )
    p1 = Vector(1.0,1.0,0.0);
    
  p2 = p3 * p1; 
  p2 /= sqrt( p2 / p2 );
  p1 = p2 * p3;

  Matrix R = Matrix(p1[0],p1[1],p1[2],p2[0],p2[1],p2[2],p3[0],p3[1],p3[2]);

  for(int i = 1 ;i <= nv; i++)
    {
      Vector p =  vert[i] / R + m_line.m_pos;
      fprintf(fPF,"v %f %f %f\n",p[0],p[1],p[2]);
    }
  if(strlen(name) > 0)
    fprintf(fPF,"g %s\n",name);
  for(int i = 1 ;i <= nf; i++)
    {
      fprintf(fPF,"f %d %d %d\n",
        face[i].m_vert[0] + nvert_offset,
        face[i].m_vert[1] + nvert_offset,
        face[i].m_vert[2] + nvert_offset);
    }
  return nv;
}

#define NORMAL_FOLLOW_FACET 1
#define NORMAL_FOLLOW_EDGE 1

int Sphere::AddSurfacePoints(char *name,double height_thresh, int nvert_offset, FILE *fPF )
{

  Vector vert[MAX_TRIANGLES],v;
  Polygon face[MAX_TRIANGLES];
  double d,h,d1,d2;
  Line L1;
    
  int nv = 0;
  int nf = 0;
  double h2 = height_thresh * height_thresh * m_rad * m_rad;
  // set basis points as tetrahedron

  Vector p1,p2,p3,p4;
  vert[1] = Vector(1.0,1.0,1.0) + m_pos;
  vert[2] = Vector(-1.0,-1.0,1.0) + m_pos;
  vert[3] = Vector(-1.0,1.0,-1.0) + m_pos;
  vert[4] = Vector(1.0,-1.0,-1.0) + m_pos;

  nv = 4;

  face[1].m_n = 3; face[1].m_vert[0] = 1; face[1].m_vert[1] = 3; face[1].m_vert[2] = 2; face[1].vis=1;
  face[2].m_n = 3; face[2].m_vert[0] = 2; face[2].m_vert[1] = 3; face[2].m_vert[2] = 4;  face[2].vis=1;
  face[3].m_n = 3; face[3].m_vert[0] = 1; face[3].m_vert[1] = 4; face[3].m_vert[2] = 3;  face[3].vis=1;
  face[4].m_n = 3; face[4].m_vert[0] = 1; face[4].m_vert[1] = 2; face[4].m_vert[2] = 4;  face[4].vis=1;
  
  nf = 4;


  for(int i = 1; i <= nv; i++)
    {
      PointProjection(vert[i],vert[i]);
    }
  int flg = 0;
  do
    {
      nf += flg;
      flg = 0;
      //////////////////////
      //   subdivide facets
      // 
      for(int i = 1; i <= nf; i++)
  {
    if(face[i].vis == 0)
      continue;
    assert(face[i].vis > 0);
    Vector cent;
    face[i].Center(vert,cent);

    if(NORMAL_FOLLOW_FACET)
      {
        L1.m_pos = cent;
        face[i].PolygonNorm(vert,L1.m_axis);
        d = (L1.m_pos - m_pos) / L1.m_axis;
        //	      assert(d > 0.0);
        if(d < 0.0)
    L1.m_axis *= -1.0;
        RayIntersection(L1,d1,d2);
        L1.LineMark(d1,v);
        d = d1*d1;
      }
    else
      {
        PointProjection(cent,v);
        Vector vv = v - cent;
        d = vv / vv;
      }
    if( d > h2) // projection is over threshold => subdivide
      {
        nv++;
        assert(nv < MAX_TRIANGLES);
        assert(nf + flg < MAX_TRIANGLES - 2);
        vert[nv] = v;
        int nf1 = nf + flg + 1;
        int nf2 = nf + flg + 2;
        int nf3 = nf + flg + 3;

        face[nf1].m_vert[1] = face[i].m_vert[0];
        face[nf1].m_vert[2] = face[i].m_vert[1];

        face[nf2].m_vert[1] = face[i].m_vert[1];
        face[nf2].m_vert[2] = face[i].m_vert[2];

        face[nf3].m_vert[1] = face[i].m_vert[2];
        face[nf3].m_vert[2] = face[i].m_vert[0];

        for(int kk = nf1; kk<=nf3; kk++)
    {
      face[kk].m_n = 3;
      face[kk].vis = 1;
      face[kk].parent = i;
      face[kk].m_vert[0] = nv;
      face[kk].SetNormDirection(vert,L1.m_axis);
    }

        face[i].vis = 0;
        flg += 3;
      }
  }

      //////////////////////
      //   subdivide edges
      // 
      // each pair of faces shares an edge that needs to be tested
      int nnf = nf + flg;
      for(int fc1 = 1; fc1 <= nnf-1; fc1++)
  {
    if(face[fc1].vis == 0)
      continue;
    assert(face[fc1].vis > 0);
    for(int fc2 = fc1+1; fc2 <= nnf; fc2++)
      {
        if(face[fc2].vis == 0)
    continue;
        assert(face[fc2].vis > 0);
        if(face[fc1].parent == face[fc2].parent)   // not cool
    continue;
        // find vertex pair in common
        int v1,v2,ind1,ind2,tind1,tind2;
        v1 = -1;
        for(int k=0;k<face[fc1].m_n;k++)
    {
      ind1 = k;
      for(int m=0;m<face[fc2].m_n;m++)
      {
        tind1 = m;
        if(face[fc1].m_vert[k] == face[fc2].m_vert[m])
          { v1 = face[fc2].m_vert[m]; break;}
      }
      if(v1 > -1)break;
    }
        if(v1 == -1)
    continue;

        v2 = -1;
        for(int k=ind1+1;k<face[fc1].m_n;k++)
    {
      ind2 = k;
      for(int m=0;m<face[fc2].m_n;m++)
      {
        tind2 = m;
        if(face[fc1].m_vert[k] == face[fc2].m_vert[m])
          { v2 = face[fc2].m_vert[m];  break; }
      }
      if(v2 > -1)break;
    }
        if(v2 == -1)
    continue;

        // faces fc1 and fc2 share edge with verts v1 and v2
        Vector lc = 0.5 * (vert[v1] + vert[v2]);
        Vector lcproj;

        if(NORMAL_FOLLOW_EDGE)
    {
                L1.m_pos = lc;
          face[fc1].PolygonNorm(vert,L1.m_axis);
          //      d = (L1.m_pos - m_pos) / L1.m_axis;
          // assert(d > 0.0);
          face[fc2].PolygonNorm(vert,lcproj);
          //d = (L1.m_pos - m_pos) / lcproj;
          //assert(d > 0.0);

          L1.m_axis += lcproj;
          L1.m_axis /= sqrt(L1.m_axis / L1.m_axis);
          //d = (L1.m_pos - m_pos) / L1.m_axis;
          //assert(d > 0.0);

          RayIntersection(L1,d1,d2);
          d = d1*d1;
          L1.LineMark(d1,lcproj);
    }
        else
    {
      PointProjection(lc,lcproj);
      v = lcproj - lc;
      d = v / v;
    }
        if(d > h2) // height is > thresh => subdivide
    {
      nv++;
      assert(nv < MAX_TRIANGLES);
      assert(nf + flg < MAX_TRIANGLES - 2);
      vert[nv] = lcproj;

      int nf1 = nf + flg + 1;
      int nf2 = nf + flg + 2;
      int nf3 = nf + flg + 3;
      int nf4 = nf + flg + 4;

      face[nf1] = face[fc1];
      face[nf2] = face[fc1];
      face[nf1].m_vert[ind1] = nv;
      face[nf2].m_vert[ind2] = nv;

      face[nf3] = face[fc2];
      face[nf4] = face[fc2];
      face[nf3].m_vert[tind1] = nv;
      face[nf4].m_vert[tind2] = nv;

      int par =  (fc2 << 16) + fc1;
      for(int kk = nf1; kk <= nf4; kk++)
        {
          face[kk].vis = 1;
          face[kk].parent = par;
          face[kk].SetNormDirection(vert,L1.m_axis);
        }

            face[fc1].vis = 0;
      face[fc2].vis = 0;
          
      flg += 4;
      
    }

      }
  }

    }while (flg);
  for(int i = 1 ;i <= nv; i++)
    {
      fprintf(fPF,"v %f %f %f\n",vert[i][0],vert[i][1],vert[i][2]);
    }
  if(strlen(name) > 0)
    fprintf(fPF,"g %s\n",name);
  for(int i = 1 ;i <= nf; i++)
    {
      if(face[i].vis > 0)
  fprintf(fPF,"f %d %d %d\n",
        face[i].m_vert[0] + nvert_offset,
        face[i].m_vert[1] + nvert_offset,
        face[i].m_vert[2] + nvert_offset);
    }

  return nv;
}  

int Sphere::DoesTriangleIntersect(Triangle &tri)
{
  Vector cp;
  return tri.IsPointWithinRange(m_pos,m_rad,cp, 1);
}


#endif
