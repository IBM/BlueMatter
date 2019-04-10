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
 #ifndef _INCLUDED_BVH__
#define _INCLUDED_BVH__


#include <FFUtils.hpp>
#include <PoserObj.hpp>
#include <CSGModel.hpp>

#define MAX_JOINTS 6
class BVHJoint;


#define SPH_SOFT_FRAC 0.79
#define CYL_SOFT_FRAC 0.79


class BVHJoint
{
public:
  int m_njoints;
  int m_level;
  int m_nchannels;
  int m_rmap[3];
  int m_rot_channels[3];
  char m_name[128];
  BVHJoint *m_joint;
  Vector m_offset;
  Vector m_pos;
  Vector m_O;
  Matrix m_R; // matrix for (v / m_R) + m_O where v is internal 
  PoserObject *m_ref;
  PoserObject *m_cur;
  Group * m_grp;
  Sphere *m_spheres;
  Cylinder *m_cylinders;

  int Move(Matrix &R, Vector &T)
  {
    m_offset = R / m_offset;
    m_pos = (R / m_pos) + T;
    m_O = (R / m_O) + T;
    m_R = (!R) / m_R;

    Sphere *ss = m_spheres;
    while(ss != NULL)
      {      
  ss->m_pos = (R / ss->m_pos) + T;
  ss = ss->m_next;
      }
    Cylinder *cyl = m_cylinders;
    while(cyl != NULL)
      {      
  cyl->m_line.m_pos = (R / cyl->m_line.m_pos) + T;
  cyl->m_line.m_axis = R / cyl->m_line.m_axis;
  cyl = cyl->m_next;
      }

    // Do not transform poser objects... handled elsewhere by owner
    for(int i = 0 ; i < m_njoints; i++)
      {
  m_joint[i].Move(R,T);
      }
    return 1;
  }

  int CountExcludedObjects(int *nspheres, int *ncyls)
  {
    int ns = *nspheres;
    int nc = *ncyls;
    Sphere *sph = m_spheres;
    while (sph != NULL)
      {
  ns++;
  sph = sph->m_next;
      }    
    Cylinder *cyl = m_cylinders;
    while (cyl != NULL)
      {
  nc++;
  cyl = cyl->m_next;
      }    
    for(int i=0;i<m_njoints;i++)
      {
  m_joint[i].CountExcludedObjects(&ns,&nc);
      }
    *nspheres = ns;
    *ncyls = nc;
    return 1;
  }
  int ResetExcludedSphereArray(Sphere * spheremap, int sphindex, int nmax)
  {
    int n = sphindex;
    Sphere *sph = m_spheres;
    while (sph != NULL)
      {
  assert(n < nmax);
  spheremap[n] = *sph;
  n++;	
  sph = sph->m_next;
      }    
    for(int i=0;i<m_njoints;i++)
      {
  m_joint[i].ResetExcludedSphereArray(spheremap,n,nmax);
      }

    return n;
  }
  int ResetExcludedCylinderArray(Cylinder * cylmap,int cylindex, int nmax)
  {
    int n = cylindex;
    Cylinder *cyl = m_cylinders;
    while (cyl != NULL)
      {
  assert(n < nmax);
  cylmap[n] = *cyl;
  n++;
  cyl = cyl->m_next;
      }    
    for(int i=0;i<m_njoints;i++)
      {
  m_joint[i].ResetExcludedCylinderArray(cylmap,n,nmax);
      }
    return n;
  }


  BVHJoint()
  {
    m_name[0] = '\0';
    m_njoints = m_level = 0;
    m_joint = NULL;
    m_nchannels = 0;
    m_rot_channels[0] = m_rot_channels[1] = m_rot_channels[2] = 0;
    m_rmap[0] = m_rmap[1] = m_rmap[2]=0;
    m_ref = m_cur = NULL;
    m_grp = NULL;
    m_spheres = NULL;
    m_cylinders = NULL;
    m_R[0][0] = m_R[1][1] = m_R[2][2] = 1.0;

  }
  ~BVHJoint()
  {
    delete [] m_joint;
    Sphere *s = m_spheres;
    while(s != NULL)
      {
  Sphere *ss = s->m_next;
  delete s;
  s = ss;
      }
    Cylinder *c = m_cylinders;
    while(c != NULL)
      {
  Cylinder *cc = c->m_next;
  delete c;
  c = cc;
      }
  }
  /////////////////////////////////////////////////////////////////////////////////////////
  int DoesJointIntersectSphere(Sphere *s);
  int DoesJointIntersectCylinder(Cylinder *cyl);
  int JointIntersection(BVHJoint *doob1);

  void AddSphere(Sphere &S)
  {
    Sphere * s = new Sphere;
    *s = S;
    s->m_rad *= SPH_SOFT_FRAC;
    s->m_next = m_spheres;
    m_spheres = s;
  }
  void AddCylinder(Cylinder &C)
  {
    Cylinder *c = new Cylinder;
    *c = C;
    c->m_rad *= CYL_SOFT_FRAC;
    c->m_next = m_cylinders;
    m_cylinders = c;
  }
  void SetName(char *name)
  {
    strcpy(m_name,name);
  }
  int operator==(char *name)
  {
    return !strcmp(m_name,name);
  }

  BVHJoint &operator=(BVHJoint &bvh)
  {
    if (m_joint == NULL)
      {
  AllocJoints(MAX_JOINTS);
      }
    m_njoints = bvh.m_njoints;
    m_level = bvh.m_level;
    m_nchannels = bvh.m_nchannels;
    for(int i=0;i<3;i++)
      {
  m_rmap[i] = bvh.m_rmap[i]; 
  m_rot_channels[i] = m_rot_channels[i];
      }
    strcpy(m_name,bvh.m_name);
    m_offset = bvh.m_offset;
    m_pos = bvh.m_pos;
    m_O = bvh.m_O;
    m_R = bvh.m_R; 
    // poser objects are not owned, must be reattached
    //  PoserObject *m_ref;m_ref;
    //  PoserObject *m_cur;m_cur;
    // Group is handled by AttachObjs   Group * m_grp;
    while(m_spheres != NULL)
      {      
  Sphere *s = m_spheres->m_next;
  delete m_spheres;
  m_spheres = s;
      }
    while(m_cylinders != NULL)
      {      
  Cylinder *s = m_cylinders->m_next;
  delete m_cylinders;
  m_cylinders = s;
      }
    Sphere *ss = bvh.m_spheres;
    while(ss != NULL)
      {      
  Sphere *s = new Sphere();
  *s = *ss;
  s->m_next = m_spheres;
  m_spheres = s;
  ss = ss->m_next;
      }
    Cylinder *cyl = bvh.m_cylinders;
    while(cyl != NULL)
      {      
  Cylinder *s = new Cylinder();
  *s = *cyl;
  s->m_next = m_cylinders;
  m_cylinders = s;
  cyl = cyl->m_next;
      }
    for(int i = 0;i< bvh.m_njoints; i++)
      {
  m_joint[i] = bvh.m_joint[i];
      }
    return *this;
  }

  void GetMatrix(Matrix &R){R = m_R;}
  void GetOrigin(Vector &D){D = m_O;}
  void GetJointPos(Vector &D){D = m_pos;}

  BVHJoint *GetJoint(char *name)
  {
    if(*this == name)
      return this;
    for(int i=0;i < m_njoints;i++)
      {
  BVHJoint *doob = m_joint[i].GetJoint(name);
  if(doob != NULL)
    return doob;
      }
    return NULL;
  }
  int IsPointContained(Vector &p)
  {

    if(m_cylinders != NULL)
      {
  Sphere s;
  s.Define(p,0.0);
  Cylinder *c = m_cylinders;
  while(c != NULL)
    {
      if(c->DoesCapsuleIntersectSphere(s))
        return 1;
      c = c->m_next;
    }
      }
    for(int i=0;i < m_njoints;i++)
      {
  if(m_joint[i].IsPointContained(p))
    return 1;
      }
    if(m_spheres != NULL)
      {
  Sphere *s = m_spheres;
  while(s != NULL)
    {
      Vector v = p - s->m_pos;
      double d = v / v;
      if(d < s->m_rad * s->m_rad)
        return 1;
      s = s->m_next;
    }
      }
    return 0;
  }
  int DoesTriangleIntersect(Triangle &tri)
  {
    if(m_cylinders != NULL)
      {
  Cylinder *c = m_cylinders;
  while(c != NULL)
    {
      if(c->DoesCapsuleIntersectTriangle(tri))
        return 1;
      c = c->m_next;
    }
      }
    for(int i=0;i < m_njoints;i++)
      {
  if(m_joint[i].DoesTriangleIntersect(tri))
    return 1;
      }
    if(m_spheres != NULL)
      {
  Sphere *s = m_spheres;
  while(s != NULL)
    {
      if(s->DoesTriangleIntersect(tri))
        return 1;
      s = s->m_next;
    }
      }
    return 0;
  }

  int ParseOffset(FILE *fPF)
  {
    char bufS[256];
    fgets(bufS,256,fPF);
    while(!feof(fPF))
      {
  char token[32];
  double x,y,z;
  int n = sscanf(bufS,"%s %lf %lf %lf",token,&x,&y,&z);
  if(!strcmp("OFFSET",token))
    { 
      assert(n == 4);
      m_offset = Vector(x,y,z);
      return 1;
    }
  fgets(bufS,256,fPF);
      }
    return 0;
  }
  int ParseChannels(FILE *fPF, int &chan)
  {
    char bufS[256];
    fgets(bufS,256,fPF);
    while(!feof(fPF))
      {
  char token[32];
  int ch;
  char r1[32],r2[32],r3[32];
  int n = sscanf(bufS,"%s %d %s %s %s ",token,&ch,r1,r2,r3);
  if(!strcmp("CHANNELS",token))
    { 
      assert(n == 5);
      m_nchannels = ch;
      if(m_nchannels == 6)
        {
    sscanf(bufS,"%s %d %*s %*s %*s %s %s %s ",token,&ch,r1,r2,r3);
    chan += 3;
        }
      switch(r1[0])
        {
        case 'X': m_rmap[0] = 0; break;
        case 'Y': m_rmap[0] = 1; break;
        case 'Z': m_rmap[0] = 2; break;
        }
      m_rot_channels[ m_rmap[0] ] = chan++;
      switch(r2[0])
        {
        case 'X': m_rmap[1] = 0; break;
        case 'Y': m_rmap[1] = 1; break;
        case 'Z': m_rmap[1] = 2; break;
        }
      m_rot_channels[ m_rmap[1] ] = chan++;
      switch(r3[0])
        {
        case 'X': m_rmap[2] = 0; break;
        case 'Y': m_rmap[2] = 1; break;
        case 'Z': m_rmap[2] = 2; break;
        }
      m_rot_channels[ m_rmap[2] ] = chan++;
      return 1;
    }
  fgets(bufS,256,fPF);
      }
    return 0;
  }
  int  AllocJoints(int n);
  int ParseJoints(FILE *fPF, int &chan,char *pname = NULL)
  {
    if (m_joint == NULL)
      {
  AllocJoints(MAX_JOINTS);
      }
    m_pos = m_O + m_offset;
    char bufS[256],name[128];
    fgets(bufS,256,fPF);
    while(!feof(fPF))
      {
  char token[32];
  int ch;
  int n = sscanf(bufS,"%s %s",token,name);
  if(!strcmp("}",token))
    {
      return 1;
    }
  if(!strcmp("End",token))
    { 
      assert(n == 2);
      char endname[256];
      strcpy(endname,pname);
      strcat(endname,"End");
      m_joint[m_njoints].SetName(endname);
      m_joint[m_njoints].m_level = m_level + 1;
      m_joint[m_njoints].ParseOffset(fPF);
      m_joint[m_njoints].m_O = m_O + m_offset;
      m_joint[m_njoints].m_pos = m_joint[m_njoints].m_O + m_joint[m_njoints].m_offset;
      m_njoints++;
      fgets(bufS,256,fPF);
      assert(m_njoints < MAX_JOINTS);
    }
  if(!strcmp("JOINT",token))
    { 
      assert(n == 2);
      m_joint[m_njoints].SetName(name);
      m_joint[m_njoints].m_O = m_pos;
      m_joint[m_njoints].m_level = m_level + 1;
      m_joint[m_njoints].ParseOffset(fPF);
      m_joint[m_njoints].ParseChannels(fPF,chan);
      m_joint[m_njoints].ParseJoints(fPF,chan,name);
      m_njoints++;
      assert(m_njoints < MAX_JOINTS);
    }
  fgets(bufS,256,fPF);
      }
    return 0;
  }

  void AttachObjs(PoserObject *ref, PoserObject * cur)
  {
    m_ref = ref;
    m_cur = cur;
    m_grp = new Group;
    ref->GetGroup(m_name, *m_grp);
    m_grp->SetOrigin(m_pos);
    for(int i = 0; i < m_njoints; i++)
      {
  m_joint[i].AttachObjs(ref,cur);
      }
  }

  int SetState(double **m_state, int t, int chan, Matrix &PR, Vector &PD)
  {


    Matrix Zr,Xr,Yr;
    Matrix R,LR;
    double dx,dy,dz;

    if(m_nchannels == 6)
      {
  dx = m_state[t][chan];   chan++;
  dy = m_state[t][chan];   chan++;
  dz = m_state[t][chan];   chan++;
  PD += Vector(dx,dy,dz);
      }

    if(m_nchannels > 0)
      {
  switch(m_rmap[0])
    {
    case 0:
      dx = m_state[t][chan];   chan++;
      dx *= M_PI/180.0;   
      genMatrix(M_PI/2.0,0.0,dx,Xr);  
      LR = Xr;
      break;
    case 1:
      dy = m_state[t][chan];   chan++;
      dy *= M_PI/180.0;   
      genMatrix(M_PI/2.0,M_PI/2.0,dy,Yr);
      LR = Yr;
      break;
    case 2:
      dz = m_state[t][chan];   chan++;
      dz *= M_PI/180.0;   
      genMatrix(0.,0.,dz,Zr);  
      LR = Zr;
      break;
    default:
      assert(0);
    }
  switch(m_rmap[1])
    {
    case 0:
      dx = m_state[t][chan];   chan++;
      dx *= M_PI/180.0;   
      genMatrix(M_PI/2.0,0.0,dx,Xr);  
      LR = Xr / LR;
      break;
    case 1:
      dy = m_state[t][chan];   chan++;
      dy *= M_PI/180.0;   
      genMatrix(M_PI/2.0,M_PI/2.0,dy,Yr);
      LR = Yr / LR;
      break;
    case 2:
      dz = m_state[t][chan];   chan++;
      dz *= M_PI/180.0;   
      genMatrix(0.,0.,dz,Zr);  
      LR = Zr / LR;
      break;
    default:
      assert(0);
    }
  switch(m_rmap[2])
    {
    case 0:
      dx = m_state[t][chan];   chan++;
      dx *= M_PI/180.0;   
      genMatrix(M_PI/2.0,0.0,dx,Xr);  
      LR =  Xr / LR;
      break;
    case 1:
      dy = m_state[t][chan];   chan++;
      dy *= M_PI/180.0;   
      genMatrix(M_PI/2.0,M_PI/2.0,dy,Yr);
      LR = Yr / LR;
      break;
    case 2:
      dz = m_state[t][chan];   chan++;
      dz *= M_PI/180.0;   
      genMatrix(0.,0.,dz,Zr);  
      LR = Zr / LR;
      break;
    default:
      assert(0);
    }
  R = LR / PR;
      }
    else
      R = PR;

    Vector npos = (m_offset / PR) + PD; 

    // Move Group Verticies
    if(m_grp!=NULL)
      m_grp->Move(m_ref,m_R,m_pos,R,npos,m_cur);

    // Move Spheres
    if(m_spheres != NULL)
      {
  Matrix Rc;
  Rc = (!m_R) / R;
  Sphere * s;
  s = m_spheres;
  while(s != NULL)
    {
      s->m_pos = ( (s->m_pos - m_pos) / Rc) + npos;
      s = s->m_next;
    }
      }
    // Move Cylinders
    if(m_cylinders != NULL)
      {
  Matrix Rc;
  Rc = (!m_R) / R;
  Cylinder * c;
  c = m_cylinders;
  while(c != NULL)
    {
      c->m_line.m_pos = ( (c->m_line.m_pos - m_pos) / Rc ) + npos;
      c->m_line.m_axis = c->m_line.m_axis / Rc ;
      c = c->m_next;
    }
      }
    m_R = R;
    m_pos = npos;
    for(int i = 0;i<m_njoints;i++)
      {
  chan = m_joint[i].SetState(m_state, t, chan, R, m_pos);
      }
    return chan;
  }
  void WritePovray(FILE *fPF)
  {
    for(int i = 0;i< m_njoints;i++)
      {
  fprintf( fPF, " cylinder { < %f , %f, %f> ,  < %f , %f, %f> ,  CYL_RAD()  MAT() }\n", 
     m_pos[0],m_pos[1],m_pos[2],m_joint[i].m_pos[0],
     m_joint[i].m_pos[1],m_joint[i].m_pos[2]);
  m_joint[i].WritePovray(fPF);
      }
    return;
  }
  
};
int BVHJoint::AllocJoints(int n)
{
  m_joint = new BVHJoint[n];
  assert (m_joint != NULL);
  return (m_joint != NULL);
}

class BVH
{
public:
  BVHJoint m_root;
  double **m_state;
  int m_nchannels;
  int m_nframes;
  double m_frametime;

  BVH()
  {
    m_state = NULL;
    m_nchannels = m_nframes = 0;    
  }

  BVH & operator=(BVH &bvh)
  {
    m_root = bvh.m_root;
    m_nchannels = bvh.m_nchannels;
    AllocStates(bvh.m_nframes);
    for(int i=0;i<bvh.m_nframes;i++)
      {	CopyState(i,bvh.m_state[i]);      }
    m_frametime = bvh.m_frametime;
    return *this;
  }
  int Move(Matrix &R, Vector &T)
  {
    return m_root.Move(R,T);
  }
  int FreeStates()
  {
    if(m_nframes == 0)
      return 0;
    for(int i = 0; i < m_nframes;i++)
      delete [] m_state[i];
    delete [] m_state;
    m_state = NULL;
    m_nframes = 0;
    return 1;
  }
  int AllocStates(int nnew)
  {
    FreeStates();
    
    double **states;
    states = new double*[nnew];
    for(int t = 0; t < nnew; t++)
      {
  states[t] = new double[m_nchannels];
  for(int i=0;i<m_nchannels;i++)
    {
        states[t][i] = 0;
    }
      }
    m_state = states;
    m_nframes = nnew;
    return 1;
  }
  int CopyState(int state,double *states)
  {
    assert(state < m_nframes);
    for(int i=0;i<m_nchannels;i++)
      {
  m_state[state][i] = states[i];
      }
    return 1;
  }
  int AddStates(int nstates)
  {
    
    double **states;
    int nnew = m_nframes + nstates;
    states = new double*[nnew];
    for(int t = 0; t < nnew; t++)
      {
  states[t] = new double[m_nchannels];
  for(int i=0;i<m_nchannels;i++)
    {
      if(t < m_nframes)
        states[t][i] = m_state[t][i];
      else
        states[t][i] = 0;
    }
      }
    for(int t = 0; t < m_nframes; t++)
      {
  delete [] m_state[t];
      }
    delete [] m_state;
    m_nframes = nnew;
    m_state = states;
    return nnew;
  }

  int Read(FILE *fPF)
  {
    char bufS[256];
    fgets(bufS,256,fPF);
    while(!feof(fPF))
      {
  char token[32];
  sscanf(bufS,"%s",token);
  if(!strcmp("HIERARCHY",token))
    {
      ParseHeirarchy(fPF);
      ParseMotion(fPF);
      return 1;
    }
  fgets(bufS,256,fPF);
      }
    return 1;
  }
  int ParseHeirarchy(FILE *fPF)
  {
    char bufS[256];
    fgets(bufS,256,fPF);
    while(!feof(fPF))
      {
  char token[32],name[128];
  sscanf(bufS,"%s %s",token,name);
  if(!strcmp("ROOT",token))
    {
      ParseRoot(name,fPF);
      return 1;
    }
  fgets(bufS,256,fPF);
      }
    return 1;
  }
  int ParseRoot(char *name,FILE *fPF)
  {
    m_nchannels = 0;
    m_root.SetName(name);
    m_root.m_level = 1;
    m_root.ParseOffset(fPF);
    m_root.ParseChannels(fPF,m_nchannels);
    m_root.ParseJoints(fPF,m_nchannels);
    return 1;
  }
  int ParseMotion(FILE *fPF)
  {
    char bufS[256];
    fgets(bufS,256,fPF);
    while(!feof(fPF))
      {
  char token[32],token2[32];
  sscanf(bufS,"%s",token);
  if(!strcmp("MOTION",token))
    {
      fgets(bufS,256,fPF);
      sscanf(bufS,"%s %d",token,&m_nframes);
      int c = strcmp("Frames:",token);
      assert(!c);
      fgets(bufS,256,fPF);
      sscanf(bufS,"%s %s %d",token,token2,&m_frametime);
      m_state = new double*[m_nframes];
      for(int t = 0; t < m_nframes; t++)
        {
    m_state[t] = new double[m_nchannels];
    for(int i=0;i<m_nchannels;i++)
      {
        fscanf(fPF," %lf ",&m_state[t][i]);
      }
        }
    }
  fgets(bufS,256,fPF);
      }
    return 1;
  }
  void WriteState(int state,FILE *fPF)
  {
    fprintf(fPF," %d ",state);
    for(int i=0;i<m_nchannels;i++)
      {
  fprintf(fPF," %f ",m_state[state][i]);
      }
    fprintf(fPF,"\n");
  }
  int ReadState(int *state,FILE *fPF)
  {
    int st;
    int n = fscanf(fPF," %d ",&st);	
    if(n != 1 || feof(fPF))
      return 0;
    *state = st;
#define STATE_PAD_SPACE 100
    if(st >= m_nframes)
      {
  int nnew = st + STATE_PAD_SPACE - m_nframes;
  AddStates(nnew);
      }
    for(int i=0;i<m_nchannels;i++)
      {
  n = fscanf(fPF," %lf ",&m_state[st][i]);	
  if(n != 1 || feof(fPF))
    return 0;
      }
    return 1;
  }
  void ClearMotion()
  {
    if(m_state != NULL)
      {
  for(int t = 0; t < m_nframes; t++)
    {
      delete [] m_state[t];
    }
  delete [] m_state;
  m_state = NULL;
  m_nframes = 0;
      }
  }
  void AttachObjs(PoserObject *ref, PoserObject * cur)
  {
    m_root.AttachObjs(ref,cur);
  }
  int SetState(int t)
  {
    int chan = 0;
    Vector D;
    Matrix R;
    genMatrix(0.,0.,0,R);  

    chan = m_root.SetState(m_state, t, chan, R, D);
    return chan;
  }
  void WritePovray(FILE *fPF)
  {
    fprintf( fPF, " sphere { < %f , %f, %f> ,  POINT_RAD()  MAT() }\n", 
       m_root.m_pos[0],m_root.m_pos[1],m_root.m_pos[2]);
    m_root.WritePovray(fPF);

    return;
  }

  int CopyState(int src, int dest)
  {
    assert(src < m_nframes);
    assert(dest < m_nframes);
    for(int i=0;i< m_nchannels;i++)
      {
  m_state[dest][i] = m_state[src][i];
      }
    return 1;
  }

  BVHJoint *GetJoint(char *name)
  {
    return m_root.GetJoint(name);
  }
};


int BVHJoint::DoesJointIntersectSphere(Sphere *s)
  {
    Cylinder *cyl = m_cylinders;
    while(cyl != NULL)
      {      
  if(cyl->DoesCapsuleIntersectSphere(*s))
    return 1;
  cyl = cyl->m_next;
      }
    
    double rs = s->m_rad;
    Sphere *ss = m_spheres;
    while(ss != NULL)
      {    
  if(s->DoSpheresIntersect(*ss))
    return 1;
  ss = ss->m_next;
      }
    // Do not transform poser objects... handled elsewhere by owner
    for(int i = 0 ; i < m_njoints; i++)
      {
  if(m_joint[i].DoesJointIntersectSphere(s))
    return 1;
      }
    return 0;
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  int BVHJoint::DoesJointIntersectCylinder(Cylinder *cyl)
  {
    Cylinder *ccyl = m_cylinders;
    while(ccyl != NULL)
      {      
  if(ccyl->DoesCapsuleIntersectCapsule(*cyl))
    return 1;
  ccyl = ccyl->m_next;
      }
    
    Sphere *ss = m_spheres;
    while(ss != NULL)
      {    
  if(cyl->DoesCapsuleIntersectSphere(*ss))
    return 1;
  ss = ss->m_next;
      }
    // Do not transform poser objects... handled elsewhere by owner
    for(int i = 0 ; i < m_njoints; i++)
      {
  if(m_joint[i].DoesJointIntersectCylinder(cyl))
    return 1;
      }
    return 0;
  }
  /////////////////////////////////////////////////////////////////////////////////////////
  int BVHJoint::JointIntersection(BVHJoint *doob1)
  {
    Cylinder *cyl = m_cylinders;
    while(cyl != NULL)
      {      
  if(doob1->DoesJointIntersectCylinder(cyl))
    return 1;
  cyl = cyl->m_next;
      }
    
    Sphere *ss = m_spheres;
    while(ss != NULL)
      {    
  if(doob1->DoesJointIntersectSphere(ss))
    return 1;
  ss = ss->m_next;
      }
    // Do not transform poser objects... handled elsewhere by owner
    for(int i = 0 ; i < m_njoints; i++)
      {
  if(m_joint[i].JointIntersection(doob1))
    return 1;
      }
    return 0;
  }
  


#endif
