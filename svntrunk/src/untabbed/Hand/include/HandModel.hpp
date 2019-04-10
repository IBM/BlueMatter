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
 #ifndef __INCLUDED_HANDMODEL__
#define __INCLUDED_HANDMODEL__

#include <bvh.hpp>
#include <CSGModel.hpp>



#define WRITE_FEATURE_POV PV.write_features
#define WRITE_SELECT1_POV PV.write_selected1
#define WRITE_SELECT2_POV PV.write_selected2
#define WRITE_SELECT3_POV PV.write_selected3
#define WRITE_EXPOSED_POV PV.write_exposed
#define PV_OUT_FILE PV.fPF

/////////////////////////////////////////////////////////////////////

class Hand
{
public:
  int m_model_id;
  BVH m_bvh;
  PoserObject m_pobj;
  PoserObject m_pcur;
  int *m_exposedface;
  int *m_selected1;
  int *m_selected2;
  int *m_selected3;
  int *m_exposedvert;
  Vector *m_facecenter;
  Sphere *m_selection1;
  Sphere *m_selection2;
  Sphere *m_selection3;
  int m_nsel1,m_nsel2,m_nsel3;

  Hand()
  {
    m_nsel1 = m_nsel2 = m_nsel3 = 0;
    m_selection1 = m_selection2 = m_selection3 = NULL; 
    m_selected1 = m_selected2 = m_selected3 = NULL; 
    m_exposedvert = m_exposedface = NULL; m_facecenter = NULL;m_model_id = 0;
  }
  Hand & operator=(Hand &hnd);
  int Id(){return m_model_id;}
  ReadBVHDef(FILE *fPF); 
  ReadSurfDef(FILE *fPF);
  BVHJoint * GetJoint(char *name);

  int Move(Matrix &R, Vector &T);  
  void SetModelId(int id){ m_model_id = id; }
  int ModelId(){return m_model_id;}
  int ReadState(FILE *bvh,FILE *pos,int state);
  int Write(FILE *fPF){m_bvh.WriteState(m_model_id,fPF);return 1;}
  int SetState(int state);
  int AddStates(int nstates)  {    return m_bvh.AddStates(nstates);  }
  int CopyState(int src, int dest){ return m_bvh.CopyState(src,dest);  }
  int CurlFinger(char *name,int state,double deg);
  int UniformGrasp(int state,double deg);
  
  int DefineSurface( char *name, double resfact);  
  int AddJointCyl(BVHJoint *doob1,double rad1,double slabres,int Nr, int npoint_offset,FILE *fPF);

  int DoesTriangleIntersectHand(Vector &v1, Vector &v2, Vector &v3);
  int ExposedVertex(int vertex);
  int ExposedFace(int facecent);

  void AddSelection1Sphere(Sphere *sph)  {sph->m_next = m_selection1; m_selection1 = sph;}
  void AddSelection2Sphere(Sphere *sph)  {sph->m_next = m_selection2; m_selection2 = sph;}
  void AddSelection3Sphere(Sphere *sph)  {sph->m_next = m_selection3; m_selection3 = sph;}
  int SelectedFace(Vector &center, int selectnum);
  WritePovray(FILE fPF,int query);
  int DoHandsIntersect(Hand *hnd);

};



////////////////////////////////////////////////////////////////////////////////////////////////////////

BVHJoint *Hand::GetJoint(char *name)
{
  return m_bvh.GetJoint(name);
}

int Hand::ReadBVHDef(FILE *fPF)
{
  return m_bvh.Read(fPF);
}
int Hand::ReadSurfDef(FILE *fPF)
{
  PoserObject p(fPF);
  m_pobj = p;
  m_pcur = p;
  return 1;
}

int Hand::ReadState(FILE *bvh,FILE *pos,int state)
{
  ReadBVHDef(bvh);
  m_bvh.SetState(0);
  ReadSurfDef(pos);
  SetState(0);    
  m_bvh.AttachObjs(&m_pobj, &m_pcur);
  return 1;
}

int Hand::AddJointCyl(BVHJoint *doob1,double rad1,double slabres,int Nr, int npoint_offset,FILE *fPF)
{
  int np = npoint_offset;
  BVHJoint *doob2;
  Cylinder cyl;
  for(int i=0;i<doob1->m_njoints;i++)
    {
      doob2 = &doob1->m_joint[i];
      Vector v = doob1->m_pos - doob2->m_pos;
      double h = sqrt(v / v);
      cyl.Define(doob1->m_pos,doob2->m_pos,rad1,h);
      doob1->AddCylinder(cyl);
      int n = h / slabres;
      np += cyl.AddSurfacePoints("",Nr,n,np,fPF);
    }
  return np - npoint_offset;
}


int Hand::DefineSurface(char *name,double resfact = 1.0)
{
  BVHJoint * doob1,*doob2,*palmthumb,*thumb,*index,*middle,*ring,*pinky,*thumbmid;
  Vector p,v;

  FILE *fPF = fopen(name,"w");

  int Nr = 4 * resfact; // 10;
  // hthresh 0.07, rad 0.5
  //  double height_thresh = 0.1;
  double height_thresh = 0.2;
  double rad = 0.5;
  double h,slabres = 1.2*rad/resfact ; //0.4;
  double prad = 1.2*rad;
  double frad = 0.9*rad;
  double rad1 = rad;
  double rad2 = 1.6*rad;

    int n;
  int np = 0;
  Sphere sph;
  Cylinder cyl;

  doob1 = GetJoint("Wrist");  assert(doob1);
  sph.Define(doob1->m_pos,rad2);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("Wrist",height_thresh,np,fPF);
  np += AddJointCyl(doob1,prad,slabres,Nr,np,fPF);


  doob1 = GetJoint("PalmCenter");  assert(doob1);
  sph.Define(doob1->m_pos,prad);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("PalmCenter",height_thresh,np,fPF);
  np += AddJointCyl(doob1,prad,slabres,Nr,np,fPF);


  doob1 = GetJoint("PalmOut");  assert(doob1);
  sph.Define(doob1->m_pos,rad2);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("PalmOut",height_thresh,np,fPF);
  np += AddJointCyl(doob1,prad,slabres,Nr,np,fPF);

  doob1 = GetJoint("Pinky");  assert(doob1);
  pinky = doob1;
  sph.Define(doob1->m_pos,prad);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("Pinky",height_thresh,np,fPF);
  np += AddJointCyl(doob1,frad,slabres,Nr,np,fPF);

  doob1 = GetJoint("PinkyMid");  assert(doob1);
  sph.Define(doob1->m_pos,frad);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("PinkyMid",height_thresh,np,fPF);
  np += AddJointCyl(doob1,frad,slabres,Nr,np,fPF);

  doob1 = GetJoint("PinkyTip");  assert(doob1);
  sph.Define(doob1->m_pos,frad);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("PinkyTip",height_thresh,np,fPF);
  np += AddJointCyl(doob1,frad,slabres,Nr,np,fPF);
  sph.Define(doob1->m_joint[0].m_pos,frad);
  doob1->m_joint[0].AddSphere(sph);
  np += sph.AddSurfacePoints("",height_thresh,np,fPF);

  doob1 = GetJoint("Ring");  assert(doob1);
  ring = doob1;
  sph.Define(doob1->m_pos,prad);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("Ring",height_thresh,np,fPF);
  np += AddJointCyl(doob1,rad1,slabres,Nr,np,fPF);

  v = (Vector(-1.0,0.0,0.0) / ring->m_R);
  h = sqrt(v / v);
  v += pinky->m_pos;
  cyl.Define(ring->m_pos,v,rad1,h);
  doob1->AddCylinder(cyl);
  n = h / slabres;
  np += cyl.AddSurfacePoints("",Nr,n,np,fPF);
  

  doob1 = GetJoint("RingMid");  assert(doob1);
  sph.Define(doob1->m_pos,rad1);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("RingMid",height_thresh,np,fPF);
  np += AddJointCyl(doob1,rad1,slabres,Nr,np,fPF);

  doob1 = GetJoint("RingTip");  assert(doob1);
  sph.Define(doob1->m_pos,rad1);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("RingTip",height_thresh,np,fPF);
  np += AddJointCyl(doob1,rad1,slabres,Nr,np,fPF);
  sph.Define(doob1->m_joint[0].m_pos,rad1);
  doob1->m_joint[0].AddSphere(sph);
  np += sph.AddSurfacePoints("",height_thresh,np,fPF);


  doob1 = GetJoint("Middle");  assert(doob1);
  middle = doob1;
  sph.Define(doob1->m_pos,prad);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("Middle",height_thresh,np,fPF);
  np += AddJointCyl(doob1,rad1,slabres,Nr,np,fPF);

  v = (Vector(-1.0,0.0,0.0) / middle->m_R);
  h = sqrt(v / v);
  v += middle->m_pos;
  cyl.Define(middle->m_pos,v,prad,h);
  doob1->AddCylinder(cyl);
  n = h / slabres;
  np += cyl.AddSurfacePoints("",Nr,n,np,fPF);


  doob1 = GetJoint("MiddleMid");  assert(doob1);
  sph.Define(doob1->m_pos,rad1);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("MiddleMid",height_thresh,np,fPF);
  np += AddJointCyl(doob1,rad1,slabres,Nr,np,fPF);

  doob1 = GetJoint("MiddleTip");  assert(doob1);
  sph.Define(doob1->m_pos,rad);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("MiddleTip",height_thresh,np,fPF);
  np += AddJointCyl(doob1,rad1,slabres,Nr,np,fPF);
  sph.Define(doob1->m_joint[0].m_pos,rad);
  doob1->m_joint[0].AddSphere(sph);
  np += sph.AddSurfacePoints("",height_thresh,np,fPF);

  doob1 = GetJoint("Index");  assert(doob1);
  index = doob1;
  sph.Define(doob1->m_pos,prad);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("Index",height_thresh,np,fPF);
  np += AddJointCyl(doob1,rad1,slabres,Nr,np,fPF);


  v = (Vector(-1.0,0.0,0.0) / index->m_R);
  h = sqrt(v / v);
  v += index->m_pos;
  cyl.Define(index->m_pos,v,prad,h);
  doob1->AddCylinder(cyl);
  n = h / slabres;
  np += cyl.AddSurfacePoints("",Nr,n,np,fPF);


  doob1 = GetJoint("IndexMid");  assert(doob1);
  sph.Define(doob1->m_pos,rad1);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("IndexMid",height_thresh,np,fPF);
  np += AddJointCyl(doob1,rad1,slabres,Nr,np,fPF);

  doob1 = GetJoint("IndexTip");  assert(doob1);
  sph.Define(doob1->m_pos,rad);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("IndexTip",height_thresh,np,fPF);
  np += AddJointCyl(doob1,rad1,slabres,Nr,np,fPF);
  sph.Define(doob1->m_joint[0].m_pos,rad);
  doob1->m_joint[0].AddSphere(sph);
  np += sph.AddSurfacePoints("",height_thresh,np,fPF);


  doob1 = GetJoint("PalmThumb");  assert(doob1);
  sph.Define(doob1->m_pos,rad2);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("PalmThumb",height_thresh,np,fPF);
  np += AddJointCyl(doob1,prad,slabres,Nr,np,fPF);
  palmthumb = doob1;

  
  doob1 = GetJoint("Thumb");  assert(doob1);
  thumb = doob1;
  sph.Define(doob1->m_pos,prad);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("Thumb",height_thresh,np,fPF);
  np += AddJointCyl(doob1,rad1,slabres,Nr,np,fPF);


  // Define points for thumb base flesh
  doob1 = GetJoint("ThumbMid");  assert(doob1);
  thumbmid = doob1;
  Vector handmid = thumbmid->m_pos + (Vector(-1.3,-1.0,0.6) / thumb->m_R);

  v = thumb->m_pos - handmid;
  h = sqrt(v / v);
  cyl.Define(thumb->m_pos,handmid,prad,h);
  thumb->AddCylinder(cyl);
  n = h / slabres;
  np += cyl.AddSurfacePoints("",Nr,n,np,fPF);

  v = thumbmid->m_pos - handmid;
  h = sqrt(v / v);
  cyl.Define(thumbmid->m_pos,handmid,prad,h);
  thumb->AddCylinder(cyl);
  n = h / slabres;
  np += cyl.AddSurfacePoints("",Nr,n,np,fPF);
  sph.Define(handmid,rad);
  thumb->AddSphere(sph);
  np += sph.AddSurfacePoints("",height_thresh,np,fPF);





  doob1 = GetJoint("ThumbMid");  assert(doob1);
  sph.Define(doob1->m_pos,rad1);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("ThumbMid",height_thresh,np,fPF);
  np += AddJointCyl(doob1,rad1,slabres,Nr,np,fPF);


  doob1 = GetJoint("ThumbTip");  assert(doob1);
  sph.Define(doob1->m_pos,rad);
  doob1->AddSphere(sph);
  np += sph.AddSurfacePoints("ThumbTip",height_thresh,np,fPF);
  np += AddJointCyl(doob1,rad1,slabres,Nr,np,fPF);
  sph.Define(doob1->m_joint[0].m_pos,rad);
  doob1->m_joint[0].AddSphere(sph);
  np += sph.AddSurfacePoints("",height_thresh,np,fPF);

  fclose(fPF);
  fPF = fopen(name,"r");
  ReadSurfDef(fPF);
  fclose(fPF);
  SetState(0);    
  m_bvh.AttachObjs(&m_pobj, &m_pcur);

  return np;
}

int Hand::DoesTriangleIntersectHand(Vector &v1, Vector &v2, Vector &v3)
{
  Triangle tri;
  tri.Define(v1,v2,v3);

  if(m_bvh.m_root.DoesTriangleIntersect(tri))
    return 1;
  else
    return 0;
}
int Hand::ExposedVertex(int vertex)
{
  if(m_exposedvert[vertex] == -1)
    {
      Vector v = m_pcur.m_vert[vertex];
      m_exposedvert[vertex] = m_bvh.m_root.IsPointContained(v);
    }
  return m_exposedvert[vertex];
}
int Hand::ExposedFace(int facecent)
{
  if(m_exposedface[facecent] == -1)
    {
      Vector v;
      m_pcur.m_face[facecent].Center(m_pcur.m_vert,v);
      m_exposedface[facecent] = !m_bvh.m_root.IsPointContained(v);
      m_facecenter[facecent] = v;
      if(m_exposedface[facecent])
  {
    if(WRITE_EXPOSED_POV)
      {
        fprintf(PV_OUT_FILE," sphere { < %f , %f , %f> ,  EXP_RAD()  EXP_MAT() }\n",v[0],v[1],v[2]);
      }
    m_selected1[facecent] = SelectedFace(v,1);
    if(m_selected1[facecent] && WRITE_SELECT1_POV)
      {
        fprintf(PV_OUT_FILE," sphere { < %f , %f , %f> ,  SEL_RAD()  SEL_MAT1() }\n",v[0],v[1],v[2]);
        m_nsel1++;
      }
    m_selected2[facecent] = SelectedFace(v,2);
    if(m_selected2[facecent] && WRITE_SELECT2_POV)
      {
        fprintf(PV_OUT_FILE," sphere { < %f , %f , %f> ,  SEL_RAD()  SEL_MAT2() }\n",v[0],v[1],v[2]);
        m_nsel2++;
      }
    m_selected3[facecent] = SelectedFace(v,3);
    if(m_selected3[facecent] && WRITE_SELECT3_POV)
      {
        fprintf(PV_OUT_FILE," sphere { < %f , %f , %f> ,  SEL_RAD()  SEL_MAT3() }\n",v[0],v[1],v[2]);
        m_nsel3++;
      }
  }
      else
  {
    m_selected1[facecent] = m_selected2[facecent] = m_selected3[facecent] = 0;
  }
   
    }
  return m_exposedface[facecent];
}


int Hand::CurlFinger(char *name,int state,double deg)
{
  BVHJoint *doob1,*doob2,*doob3;
  doob1 = m_bvh.GetJoint(name); assert(doob1);
  doob2 = &doob1->m_joint[0];
  doob3 = &doob2->m_joint[0];
  
  int chan;
  chan = doob1->m_rot_channels[ 0 ]; //xrot 
  m_bvh.m_state[state][chan] += deg;
  chan = doob2->m_rot_channels[ 0 ]; //xrot 
  m_bvh.m_state[state][chan] += deg;
  chan = doob3->m_rot_channels[ 0 ]; //xrot 
  m_bvh.m_state[state][chan] += deg;
  return 1;
}

int Hand::UniformGrasp(int state,double curl_factor)
{
  CurlFinger("Pinky",state,curl_factor);
  CurlFinger("Ring",state,curl_factor);
  CurlFinger("Middle",state,curl_factor);
  CurlFinger("Index",state,curl_factor);
  CurlFinger("Thumb",state,curl_factor);
  SetState(state);
  return 1;
}
int Hand::SetState(int state)
{
  m_bvh.SetState(state);
  m_pobj = m_pcur;
  m_model_id = state;
  ////////// clear vert flags
  if(m_exposedvert == NULL)
    {
      m_exposedvert = new int[m_pcur.m_nverts+1];
      assert(m_exposedvert != NULL);
    }
  for(int i = 0;i<=m_pcur.m_nverts;i++)
    m_exposedvert[i] = -1;

  ////////// clear face flags
  if(m_exposedface == NULL)
    {
      m_exposedface = new int[m_pcur.m_nfaces+1];
      assert(m_exposedface != NULL);
    }
  for(int i = 0;i<=m_pcur.m_nfaces;i++)
    m_exposedface[i] = -1;


  if(m_selected1 == NULL)
    {
      m_selected1 = new int[m_pcur.m_nfaces+1];
      assert(m_selected1 != NULL);
    }
  for(int i = 0;i<=m_pcur.m_nfaces;i++)
    m_selected1[i] = -1;

  if(m_selected2 == NULL)
    {
      m_selected2 = new int[m_pcur.m_nfaces+1];
      assert(m_selected2 != NULL);
    }
  for(int i = 0;i<=m_pcur.m_nfaces;i++)
    m_selected2[i] = -1;
  if(m_selected3 == NULL)
    {
      m_selected3 = new int[m_pcur.m_nfaces+1];
      assert(m_selected3 != NULL);
    }
  for(int i = 0;i<=m_pcur.m_nfaces;i++)
    m_selected3[i] = -1;


  ////////// allocate space for face centers
  if(m_facecenter == NULL)
    {
      m_facecenter = new Vector[m_pcur.m_nfaces+1];
      assert(m_facecenter != NULL);
    }

  return 1; 
}
int Hand::SelectedFace(Vector &p, int selectnum)
{

  Sphere * s;
  switch(selectnum)
    {
    case 1:      s = m_selection1; break;
    case 2:      s = m_selection2; break;
    case 3:      s = m_selection3; break;
    default: assert(0);
    }
  while(s != NULL)
    {
      Vector v = s->m_pos - p;
      if(v/v < (s->m_rad * s->m_rad))
  return 1;
      s = s->m_next;
    }
  return 0;
}

template < class T >
int CopyMap(T *src, int N, T ** dest)
{
  T *nmap;
  if(*dest != NULL)
    delete [] (*dest);
  nmap = new T[N+1];
  assert (nmap != NULL);
  for(int i=0;i<=N;i++)
    nmap[i] = src[i];
  *dest = nmap;
  return 1;
}
Hand & Hand::operator=(Hand &hnd)
{
  m_model_id = hnd.m_model_id;
  m_bvh = hnd.m_bvh;
  m_pobj = hnd.m_pobj;
  m_pcur = hnd.m_pcur;
  m_bvh.AttachObjs(&m_pobj,&m_pcur);

  int N = hnd.m_pcur.m_nfaces;

  CopyMap <int> (hnd.m_exposedface, N, &m_exposedface);
  CopyMap <int> (hnd.m_selected1, N, &m_selected1);
  CopyMap <int> (hnd.m_selected2, N, &m_selected2);
  CopyMap <int> (hnd.m_selected3, N, &m_selected3);
  CopyMap <int> (hnd.m_exposedvert, N, &m_exposedvert);
  CopyMap <Sphere> (hnd.m_selection1, N, &m_selection1);
  CopyMap <Sphere> (hnd.m_selection2, N, &m_selection2);
  CopyMap <Sphere> (hnd.m_selection3, N, &m_selection3);
  CopyMap <Vector> (hnd.m_facecenter, N, &m_facecenter);
  m_nsel1 = hnd.m_nsel1;
  m_nsel2 = hnd.m_nsel2;
  m_nsel3 = hnd.m_nsel3;
  return *this;
}

int Hand::Move(Matrix &R, Vector &T)
{
  Matrix tR = !R;

  m_bvh.Move(R,T);
  m_pcur.Move(R,T);
  m_pobj.Move(R,T);
  if(m_selection1 != NULL)
    {
      Sphere *s = m_selection1;
      while(s != NULL)
  {
    s->m_pos = R / s->m_pos  + T;
    s = s->m_next;
  }
    }
  if(m_selection2 != NULL)
    {
      Sphere *s = m_selection2;
      while(s != NULL)
  {
    s->m_pos = R / s->m_pos  + T;
    s = s->m_next;
  }
    }
  if(m_selection3 != NULL)
    {
      Sphere *s = m_selection3;
      while(s != NULL)
  {
    s->m_pos = R / s->m_pos  + T;
    s = s->m_next;
  }
    }
  if(m_facecenter != NULL)
    {
      for(int i =0; i<=m_pcur.m_nfaces;i++)
    m_facecenter[i] = R / m_facecenter[i]  + T;
    }
  return 1;
}

int Hand::WritePovray(FILE fPF,int query = 0)
{
  return 1;
}

int Hand::DoHandsIntersect(Hand *hnd)
{
  return m_bvh.m_root.JointIntersection(&hnd->m_bvh.m_root);
}
#endif
