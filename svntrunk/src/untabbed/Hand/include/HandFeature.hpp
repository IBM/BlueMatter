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
 
#ifndef _INCLUDED_HANDFEATURE_
#define _INCLUDED_HANDFEATURE_
#include <HandModel.hpp>



// Each unit length in its own bin
#define MAX_SIDE 18.5
#define MIN_SIDE 2.5
#define SIDE_BITS 4
#define SIDE_BIN_WIDTH (MAX_SIDE - MIN_SIDE)/(1 << SIDE_BITS)

#define MAX_ANG1 125.0
#define MIN_ANG1 25.0
#define ANG1_BITS 2
#define ANG1_BIN_WIDTH (MAX_ANG1 - MIN_ANG1)/(1 << ANG1_BITS)

#define MAX_ANG2 180.0
#define MIN_ANG2 0.0
#define ANG2_BITS 2
#define ANG2_BIN_WIDTH (MAX_ANG2 - MIN_ANG2)/(1 << ANG2_BITS)

#define MAX_ANG3 1.0
#define MIN_ANG3 0.0
#define ANG3_BITS 2
#define ANG3_BIN_WIDTH (MAX_ANG3 - MIN_ANG3)/(1 << ANG3_BITS)

#define SINGULAR 1e-6



/////////////////////////////////////////////////////////////////////

class HandFeature
{
public:

  int m_feature_id;
  int m_model_id;
  // Feature Def:
  //    Take triplet of segments and permute surface points at 90 degees
  //  
  double m_s1,m_s2,m_s3;
  double m_a1,m_a2,m_a3;
  double m_t1,m_t2,m_t3;
  double m_p1,m_p2,m_p3;
  Vector m_xvec;  // lab X
  Vector m_yvec;  // lab Y
  Vector m_zvec;  // lab Z
  Matrix m_R;
  double d_t,d_p,d_o; // eulars of internal to lab
  double d_x,d_y,d_z; // v1
  double d_x0,d_y0,d_z0;  // center of seg1
  Vector m_origin;  // center of seg1
  Vector m_v1,m_v2,m_v3;
  Vector m_n1,m_n2,m_n3;
  int m_seg1,m_seg2,m_seg3;
  unsigned int key;
  HandFeature * next;



  HandFeature(){Init();}
  void Init();
  int Read(FILE *fPF);
  int Write(FILE *fPF);
  void WriteSingleLine(FILE *fPF, int header);
  int WritePovray(FILE *fPF);
  void Origin(Vector &org){org = m_origin;}
  void SetOrigin(Vector &org){ m_origin = org;  }
  void SetTupleIds(int s1,int s2, int s3){ m_seg1 = s1; m_seg2 = s2; m_seg3 = s3;}
  void InvertNormals()  {    m_n1 *= -1.0;    m_n2 *= -1.0;    m_n3 *= -1.0;  }

  void SetVerticiesAndNormals(Vector &v1, Vector &v2, Vector &v3,Vector &n1, Vector &n2, Vector &n3);
  unsigned int ComputeKey();
  int ComputeSideCode(double side);
  int ComputeAngleCode1(double ang);
  int ComputeAngleCode2(double ang);
  int ComputeAngleCode3(double ang);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
void HandFeature::Init()
{
  m_seg1 = m_seg2 = m_seg3 = m_model_id = m_feature_id = 0;
  m_s1 = m_s2 = m_s3 = m_a1 = m_a2 = m_a3 = m_t1 = m_t2 = m_t3 = m_p1 = m_p2 = m_p3 = 0.0;
  d_x = d_y = d_z = d_x0 = d_y0 = d_z0 = d_t = d_p = d_o = 0;
  Vector zero(0.0,0.0,0.0);
  m_xvec = zero;    m_yvec = zero;    m_zvec = zero;    m_origin = zero;
  m_xvec[0] =  m_yvec[1] =  m_zvec[2] = 1.0;

  m_v1 = zero;    m_v2 = zero;    m_v3 = zero;
  key = 0;
  next = NULL;
} 
  
int HandFeature::Read(FILE *fPF)
{
  fscanf(fPF," %x %d %d %d %d %d\n", &key, &m_feature_id,  &m_model_id,  &m_seg1, &m_seg2, &m_seg3);
  fscanf(fPF," %lf %lf %lf   %lf %lf %lf   %lf %lf %lf   %lf %lf %lf\n", 
   &m_s1,&m_s2,&m_s3,&m_a1,&m_a2,&m_a3,&m_t1,&m_t2,&m_t3,&m_p1,&m_p2,&m_p3);
  fscanf(fPF," %lf %lf %lf   %lf %lf %lf   %lf %lf %lf \n",
   &d_t,&d_p,&d_o, &d_x,&d_y,&d_z, &d_x0,&d_y0,&d_z0); 
  fscanf(fPF," %lf %lf %lf   %lf %lf %lf   %lf %lf %lf   %lf %lf %lf\n", 
   &m_xvec[0],&m_xvec[1],&m_xvec[2],&m_yvec[0],&m_yvec[1],&m_yvec[2],&m_zvec[0],&m_zvec[1],&m_zvec[2],&m_origin[0],&m_origin[1],&m_origin[2]);
  fscanf(fPF," %lf %lf %lf   %lf %lf %lf   %lf %lf %lf  \n", 
   &m_v1[0],&m_v1[1],&m_v1[2],&m_v2[0],&m_v2[1],&m_v2[2],&m_v3[0],&m_v3[1],&m_v3[2]);
  fscanf(fPF," %lf %lf %lf   %lf %lf %lf   %lf %lf %lf  \n", 
   &m_n1[0],&m_n1[1],&m_n1[2],&m_n2[0],&m_n2[1],&m_n2[2],&m_n3[0],&m_n3[1],&m_n3[2]);
  next = NULL;
  return 1;
}
int HandFeature::Write(FILE *fPF)
{
  fprintf(fPF," %x %d %d %d %d %d\n", key, m_feature_id, m_model_id,  m_seg1, m_seg2, m_seg3);
  fprintf(fPF," %f %f %f   %f %f %f   %f %f %f   %f %f %f\n", 
    m_s1,m_s2,m_s3,m_a1,m_a2,m_a3,m_t1,m_t2,m_t3,m_p1,m_p2,m_p3);
  fprintf(fPF," %f %f %f   %f %f %f   %f %f %f \n",d_t,d_p,d_o, d_x,d_y,d_z, d_x0,d_y0,d_z0); 
  fprintf(fPF," %f %f %f   %f %f %f   %f %f %f   %f %f %f\n", 
    m_xvec[0],m_xvec[1],m_xvec[2],m_yvec[0],m_yvec[1],m_yvec[2],m_zvec[0],m_zvec[1],m_zvec[2],m_origin[0],m_origin[1],m_origin[2]);
  fprintf(fPF," %f %f %f   %f %f %f   %f %f %f  \n", 
    m_v1[0],m_v1[1],m_v1[2],m_v2[0],m_v2[1],m_v2[2],m_v3[0],m_v3[1],m_v3[2]);
  fprintf(fPF," %f %f %f   %f %f %f   %f %f %f  \n", 
    m_n1[0],m_n1[1],m_n1[2],m_n2[0],m_n2[1],m_n2[2],m_n3[0],m_n3[1],m_n3[2]);

  return 1;
}
void HandFeature::WriteSingleLine(FILE *fPF, int header = 0)
{
  if(header)
    {
      fprintf(fPF,"// key m_feature_id m_model_id  m_seg1 m_seg2 m_seg3 m_s1 m_s2 m_s3 m_a1 m_a2 m_a3 m_t1 m_t2 m_t3 m_p1,m_p2 m_p3 d_t d_p d_o  d_x d_y d_z  d_x0 d_y0 d_z0  m_v1[0] m_v1[1] m_v1[2] m_v2[0] m_v2[1] m_v2[2] m_v3[0] m_v3[1] m_v3[2]   m_n1[0] m_n1[1] m_n1[2] m_n2[0] m_n2[1] m_n2[2] m_n3[0] m_n3[1] m_n3[2]\n");
      return;
    }

  fprintf(fPF," %x %d %d %d %d %d ", key, m_feature_id, m_model_id,  m_seg1, m_seg2, m_seg3);
  fprintf(fPF," %f %f %f   %f %f %f   %f %f %f   %f %f %f  ", 
    m_s1,m_s2,m_s3,m_a1,m_a2,m_a3,m_t1,m_t2,m_t3,m_p1,m_p2,m_p3);
  fprintf(fPF," %f %f %f   %f %f %f   %f %f %f ",d_t,d_p,d_o, d_x,d_y,d_z, d_x0,d_y0,d_z0); 
  fprintf(fPF," %f %f %f   %f %f %f   %f %f %f ", 
    m_v1[0],m_v1[1],m_v1[2],m_v2[0],m_v2[1],m_v2[2],m_v3[0],m_v3[1],m_v3[2]);
  fprintf(fPF," %f %f %f   %f %f %f   %f %f %f \n", 
    m_n1[0],m_n1[1],m_n1[2],m_n2[0],m_n2[1],m_n2[2],m_n3[0],m_n3[1],m_n3[2]);
    
  return ;
}
int HandFeature::WritePovray(FILE *fPF)
{
        
  fprintf( fPF, " cylinder { < %f , %f, %f> , < %f , %f,  %f>,  SIDE_RAD()  MAT_SIDE() }\n",
     m_v1[0],m_v1[1],m_v1[2],m_v2[0],m_v2[1],m_v2[2]); 
  fprintf( fPF, " cylinder { < %f , %f, %f> , < %f , %f,  %f>,  SIDE_RAD()  MAT_SIDE() }\n",
     m_v2[0],m_v2[1],m_v2[2],m_v3[0],m_v3[1],m_v3[2]); 
  fprintf( fPF, " cylinder { < %f , %f, %f> , < %f , %f,  %f>,  SIDE_RAD()  MAT_SIDE() }\n",
     m_v1[0],m_v1[1],m_v1[2],m_v3[0],m_v3[1],m_v3[2]); 
  
  fprintf( fPF, " sphere { < %f , %f, %f> ,  VERT_RAD()  MAT_VERT() }\n",
     m_v1[0],m_v1[1],m_v1[2]);
  fprintf( fPF, " sphere { < %f , %f, %f> ,  VERT_RAD()  MAT_VERT() }\n",
     m_v2[0],m_v2[1],m_v2[2]);
  fprintf( fPF, " sphere { < %f , %f, %f> ,  VERT_RAD()  MAT_VERT() }\n",
     m_v3[0],m_v3[1],m_v3[2]);
  return 1;
}

void HandFeature::SetVerticiesAndNormals(Vector &v1, Vector &v2, Vector &v3,Vector &n1, Vector &n2, Vector &n3)
{
  m_v1 = v1;   m_v2 = v2;  m_v3 = v3;  
  m_n1 = n1;   m_n2 = n2;  m_n3 = n3;
}
unsigned int HandFeature::ComputeKey()
{
  // compute sides
  double d;
  Vector side1,side2,side3;
  side1 = m_v2 - m_v1;  m_s1 = sqrt(side1 / side1);
  side2 = m_v3 - m_v2;  m_s2 = sqrt(side2 / side2);
  side3 = m_v3 - m_v1;  m_s3 = sqrt(side3 / side3);
  
  if(1)
    {
      if(m_s1 < MIN_SIDE)	return 0;
      if(m_s1 > MAX_SIDE)	return 0;
      if(m_s2 < MIN_SIDE)	return 0;
      if(m_s2 > MAX_SIDE)	return 0;
      if(m_s3 < MIN_SIDE)	return 0;
      if(m_s3 > MAX_SIDE)	return 0;
    }

  // compute triangle angles
  side1 /= m_s1; side2 /= m_s2; side3 /= m_s3;
  m_a1 = side1 / (-side2); m_a1 = acos(m_a1) * 180.0 / M_PI;
  m_a2 = side2 / side3; m_a2 = acos(m_a2) * 180.0 / M_PI;
  m_a3 = side1 / side3; m_a3 = acos(m_a3) * 180.0 / M_PI;
  if(1)
    {
      if(m_a1 < MIN_ANG1)	return 0;
      if(m_a1 > MAX_ANG1)	return 0;
      if(m_a2 < MIN_ANG1)	return 0;
      if(m_a2 > MAX_ANG1)	return 0;
      if(m_a3 < MIN_ANG1)	return 0;
      if(m_a3 > MAX_ANG1)	return 0;
    }
  // compute orientation

  m_xvec = side1; m_xvec /= sqrt(m_xvec / m_xvec);
  m_zvec = side1 * side2; d = m_zvec / m_zvec; 
  if (d > SINGULAR)
    m_zvec /= sqrt(d);
  else
    {
      key = 0;
      return 0;
    }
  m_yvec = m_zvec * m_xvec; 
  m_yvec /= sqrt( m_yvec / m_yvec );
    
  // compute surf normal theta and phi
  m_n1 /= sqrt( m_n1 / m_n1);
  m_n2 /= sqrt( m_n2 / m_n2);
  m_n3 /= sqrt( m_n3 / m_n3);
    
  m_t1 = m_n1 / m_zvec; m_t1 = acos(m_t1) * 180.0/M_PI;
  m_t2 = m_n2 / m_zvec; m_t2 = acos(m_t2) * 180.0/M_PI;
  m_t3 = m_n3 / m_zvec; m_t3 = acos(m_t3) * 180.0/M_PI;

  Vector ax1 = 0.5 * (m_v2 + m_v3 ) - m_v1; ax1 /= sqrt(ax1/ax1);
  Vector ax2 = 0.5 * (m_v1 + m_v3 ) - m_v2; ax2 /= sqrt(ax2/ax2);
  Vector ax3 = 0.5 * (m_v1 + m_v2 ) - m_v3; ax3 /= sqrt(ax3/ax3);
  m_p1 = m_n1 / ax1;
  m_p2 = m_n2 / ax2;
  m_p3 = m_n3 / ax3;

  if(1)
    {
      if(m_p1 < MIN_ANG3)	return 0;
      if(m_p2 < MIN_ANG3)	return 0;
      if(m_p3 < MIN_ANG3)	return 0;
    }
    
  Matrix R ( m_xvec[0], m_xvec[1], m_xvec[2], // from lab to internal
       m_yvec[0], m_yvec[1], m_yvec[2], 
       m_zvec[0], m_zvec[1], m_zvec[2]);
  decodeMatrix(R,d_t,d_p,d_o);
  m_R = R;

  Vector cent = (m_v1 + m_v2 + m_v3) / 3.0;
  d_x = cent[0]; d_y = cent[1]; d_z = cent[2];
  d_x0 = m_origin[0]; d_y0 = m_origin[1]; d_z0 = m_origin[2];

    
  // compute key
  unsigned int s;
  key = 0;
  s = ComputeSideCode(m_s1);    
  key |= s; key <<= SIDE_BITS; 

  s = ComputeSideCode(m_s2);    
  key |= s; key <<= SIDE_BITS; 

  s = ComputeSideCode(m_s3);    
  key |= s; key <<= SIDE_BITS; 

//   s = ComputeAngleCode1(m_a1);    
//   key |= s; key <<= ANG1_BITS; 

//   s = ComputeAngleCode1(m_a2);    
//   key |= s; key <<= ANG1_BITS; 

//   s = ComputeAngleCode1(m_a3);    
//   key |= s; key <<= ANG1_BITS; 

  s = ComputeAngleCode2(m_t1);    
  key |= s; key <<= ANG2_BITS; 

  s = ComputeAngleCode2(m_t2);    
  key |= s; key <<= ANG2_BITS; 

  s = ComputeAngleCode2(m_t3);    
  key |= s; key <<= ANG2_BITS; 

  s = ComputeAngleCode3(m_p1);    
  key |= s; key <<= ANG2_BITS; 

  s = ComputeAngleCode3(m_p2);    
  key |= s; key <<= ANG2_BITS; 

  s = ComputeAngleCode3(m_p3);    
  key |= s; key <<= ANG2_BITS; 
  return key;
}

int HandFeature::ComputeSideCode(double side)
{
  double s = side;    
  if(s > MAX_SIDE)s = MAX_SIDE; 
  if(s < MIN_SIDE)s = MIN_SIDE; 
  s -= MIN_SIDE;
  s /= SIDE_BIN_WIDTH;
  return (int)(s + 0.5);
}
int HandFeature::ComputeAngleCode1(double ang)
{
  double s = ang;    
  if(s > MAX_ANG1)s = MAX_ANG1; 
  if(s < MIN_ANG1)s = MIN_ANG1; 
  s -= MIN_ANG1;
  s /= ANG1_BIN_WIDTH;
  return (int)(s + 0.5);
}
int HandFeature::ComputeAngleCode2(double ang)
{
  double s = ang;    
  if(s > MAX_ANG2)s = MAX_ANG2; 
  if(s < MIN_ANG2)s = MIN_ANG2; 
  s -= MIN_ANG2;
  s /= ANG2_BIN_WIDTH;
  return (int)(s + 0.5);
}  

int HandFeature::ComputeAngleCode3(double ang)
{
  double s = ang;    
  if(s > MAX_ANG3)s = MAX_ANG3; 
  if(s < MIN_ANG3)s = MIN_ANG3; 
  s -= MIN_ANG3;
  s /= ANG3_BIN_WIDTH;
  return (int)(s + 0.5);
}  


#endif
