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
 
#ifndef _INCLUDED_HANDHYPO__
#define _INCLUDED_HANDHYPO__

#include "HandFeatureExtractor.hpp"


#define TRAN_BITS 7
#define TRAN_MIN -31.75
#define TRAN_MAX 32.25
#define TRAN_BIN_WIDTH (TRAN_MAX - TRAN_MIN)/(1 << TRAN_BITS)

//  # define ROT1_BITS 5
#define ROT1_BITS 5
#define ROT1_MIN -1.0
#define ROT1_MAX 1.0
#define ROT1_BIN_WIDTH (ROT1_MAX - ROT1_MIN)/(1 << ROT1_BITS)


int compareHandFeatures (const void * a, const void * b)
{
  HandFeature * aH = (HandFeature *) a;
  HandFeature * bH = (HandFeature *) b;
  if(aH->key > bH->key )
    return 1;
  else if (aH->key < bH->key )
    return -1;
  else return 0;
}



class Hand_Transformation_Light;

class TransformationClass
{
public:
  int m_rot,m_tran,m_modelId;
  int score;
  TransformationClass *next;
  Hand_Transformation_Light *hypo;

  TransformationClass(){m_rot = m_tran = m_modelId = 0;score = 0;next = NULL; hypo = NULL;}
  TransformationClass(TransformationClass & t)
  { *this = t;}
  int operator==(TransformationClass &t)
  {
    return( (m_rot == t.m_rot) && (m_tran == t.m_tran) && (m_modelId == t.m_modelId));
  }
  TransformationClass & operator=(TransformationClass &t)
  {
    m_rot = t.m_rot;
    m_tran = t.m_tran;
    m_modelId = t.m_modelId;
    score = 0;next = NULL; hypo = NULL;
    return *this;
  }

  void Write(FILE *fPF)
  {
    fprintf(fPF," %x %x %d %d \n", m_rot, m_tran, m_modelId, score); 
  }
  int Read(FILE *fPF)
  {
    return (4 == fscanf(fPF," %x %x %d %d \n", &m_rot, &m_tran, &m_modelId, &score));
  }

#define TRANSFORMATION_HASH_BITS 12
  int Hash()
  { 
    int i = m_tran % (1 << TRANSFORMATION_HASH_BITS);
    int j = m_rot % (1 << TRANSFORMATION_HASH_BITS);
    j ^= i;
    return j;
  }
};

class Hand_Transformation;

class Hand_Transformation_Light
{
public:
  int m_qf,m_mf;
  Hand_Transformation_Light *next;
  Hand_Transformation_Light(){m_qf = m_mf = 0; next = NULL;}
};

class Hand_Transformation
{

public:

private:
  // data
 
  double d_x,d_y,d_z,d_x0,d_y0,d_z0,d_t,d_p,d_o; 
  
public:
  int d_modelId,score;
  int d_qfn,d_mfn; // feature numbers for tracking
  TransformationClass trans;
  Hand_Transformation *next;

    // methods
  public:
  Hand_Transformation()
  {
    score = 0;
    d_x = d_y = d_z = d_x0 = d_y0 = d_z0 = d_t = d_p = d_o = 0;   
    d_modelId =  d_qfn = d_mfn = 0;
    next = NULL;
  }
  Hand_Transformation( Hand_Transformation&t){*this = t;}
  ~Hand_Transformation(){}
   Hand_Transformation& operator=(Hand_Transformation& t)
  {
    d_x = t.d_x; d_y = t.d_y; d_z = t.d_z; 
    d_x0 = t.d_x0; d_y0 = t.d_y0; d_z0 = t.d_z0; 
    d_t = t.d_t; d_p = t.d_p; d_o = t.d_o;
    d_modelId = t.d_modelId;
    d_qfn = t.d_qfn;
    d_mfn = t.d_mfn;
    trans = t.trans;
    score = t.score;
    next = NULL;
    return *this;
  }

  void Write(FILE *fPF)
  {
    fprintf(fPF,"%d %d %d %d %f %f %f  %f %f %f  %f %f %f  ",score,d_qfn, d_mfn, d_modelId, d_t, d_p, d_o, d_x, d_y, d_z, d_x0, d_y0, d_z0);
    trans.Write(fPF);
  }
  int Read(FILE *fPF)
  {
    if( 13 != fscanf(fPF,"%d %d %d %d %lf %lf %lf  %lf %lf %lf  %lf %lf %lf ",&score , &d_qfn, &d_mfn, &d_modelId, &d_t, &d_p, &d_o, &d_x, &d_y, &d_z, &d_x0, &d_y0, &d_z0))
      return 0;
    return trans.Read(fPF);
  }
   int operator==( Hand_Transformation&t)
  {
    return (trans == t.trans);
  }
  int getTranslation(){ return trans.m_tran; }
  int getRotation() { return trans.m_rot; }
   int getTranslation(Vector & v){v = Vector(d_x,d_y,d_z); return 1;}
  int getDestination(Vector & v){v = Vector(d_x0,d_y0,d_z0); return 1;}
  int getRotation(Matrix & m){genMatrix(d_t,d_p,d_o,m); return 1;}
  

   double getTheta(){return d_t;}
   double getPhi(){return d_p;}
   double getOmega(){return d_o;}
   double getX(){return d_x;}
   double getY(){return d_y;}
   double getZ(){return d_z;}
   double getX0(){return d_x0;}
   double getY0(){return d_y0;}
   double getZ0(){return d_z0;}
   void setTheta(double t){d_t = t;}
   void setPhi(double p){d_p = p;}
   void setOmega(double o){d_o = o;}
   void setX(double x){d_x = x;}
   void setY(double y){d_y = y;}
   void setZ(double z){d_z = z;}
   void setX0(double x){d_x0 = x;}
   void setY0(double y){d_y0 = y;}
   void setZ0(double z){d_z0 = z;}


  Hand_Transformation(HandFeature& model, HandFeature& query)
  {
        Vector center;
  Vector Cent;
  double a,b,c;

        d_qfn = query.m_feature_id;
  d_mfn = model.m_feature_id;
  d_modelId = model.m_model_id;

        model.Origin(Cent);
        Matrix Rfrom;
  a = model.d_t;
  b = model.d_p;
  c = model.d_o;
        genMatrix(a,b,c,Rfrom);

        Matrix Rto;
  a = query.d_t;
  b = query.d_p;
  c = query.d_o;
        genMatrix(a,b,c,Rto);


  // Rq(Pq - Q) = Rm(Pm - M)
  // where: 
  //     Rq is query feature axes in lab frame
  //     Pq is the target  point in lab frame 
  //     Q is the query feature origin in lab frame
  //     Rm is model feature axes in lab frame
  //     Pm is the  model  point in lab frame to be aligned to target
  //     M is the model feature origin in lab frame

        Matrix R;
        R = (!Rto) / Rfrom;

        Vector T;
  a = query.d_x;
  b = query.d_y;
  c = query.d_z;
        Vector Q(a,b,c);
  a = model.d_x;
  b = model.d_y;
  c = model.d_z;
        Vector M(a,b,c);

  //        T =  (!Rto) / (M - Q);  (use when M and Q are internal)
  // M and Q are in lab frame
  T = Q - R / M;

        Vector D,C(Cent[0], Cent[1], Cent[2]);
        D = (R / C ) + T;
  int p=0;
  if(p)
    {
//	    cout << form("Dest to query frame %f %f %f\n",D[0],D[1],D[2]);
      cout << "Dest to query frame " << D[0] << " " << D[1] << " " << D[2] << "\n";

        cout << "Hand_Transformation(): \n";
//        cout << form("\tquery %f %f %f \n\tref = %f %f %f\n",Q[0],Q[1],Q[2],M[0],M[1],M[2]);
        cout << "\tquery "
         << Q[0] 
         << " " << Q[1]
         << " " << Q[2]
         << "\n\tref = " << M[0]
         << " " << M[1]
         << " " << M[2]
         << "\n";
//        cout << form("\ttranslation %f %f %f \n\tdestination = %f %f %f\n",T[0],T[1],T[2],D[0],D[1],D[2]);
        cout << "\ttranslation %f %f %f \n\tdestination = %f %f %f\n"
          << T[0]
          << " " << T[1]
          << " " << T[2]
          << "\n\tdestination = " << D[0]
          << " " << D[1]
          << " " << D[2] 
          << "\n";

    }
  

  d_x = T[0];
  d_y = T[1];
  d_z = T[2];
  d_x0 = D[0];
  d_y0 = D[1];
  d_z0 = D[2];
  
  trans.m_tran = 0;
  int s;
  int tmask = (1 << TRAN_BITS) - 1;
  s = ComputeTranCode(D[0]);
        trans.m_tran |= tmask & s;	trans.m_tran <<= TRAN_BITS;
  s = ComputeTranCode(D[1]);
        trans.m_tran |= tmask & s;	trans.m_tran <<= TRAN_BITS;
  s = ComputeTranCode(D[2]);
        trans.m_tran |= tmask & s;	trans.m_tran <<= TRAN_BITS;

  decodeMatrix(R,T[0],T[1],T[2]);
  d_t = T[0];
  d_p = T[1];
  d_o = T[2];

// 	if(fabs(sin(d_o)) < delta)
// 	  { 
// 	    T = Vector(0.0,0.0,0.0);
// 	  }
// 	else
// 	  {
// 	    T[0] = cos(d_t);
// 	    T[1] = d_p/M_PI;
// 	    T[2] = sin(d_o);
// 	  }

  // Rotation bin stunt:
  //
  //   Take the direction cosines of the axis of rotation => T
  //   scale by sin(1/2 * omega) to scale from [0,1]
  //   ensure consistent bin assignment give 180 about ax is -180 about -ax
  //
  Vector axis;
  axis[0] = cos(d_p) * sin(d_t);
  axis[1] = sin(d_p) * sin(d_t);
  axis[2] = cos(d_t);
  T[0] = R[0][0]*axis[0] + R[0][1]*axis[1] + R[0][2]*axis[2]; 
  T[1] = R[1][0]*axis[0] + R[1][1]*axis[1] + R[1][2]*axis[2]; 
  T[2] = R[2][0]*axis[0] + R[2][1]*axis[1] + R[2][2]*axis[2]; 
  // decode matrix only gives positive rotations about axis. 
  // Therefore, we do not need to worry about degeneracy of +- rotations about +- axis
  // rotations of greater than (180 - half_bin_width) should wrap to an axis with an
  // octant with positive parity

  double delta =  0.5 * ROT1_BIN_WIDTH;
  if(d_o > M_PI - delta)
    {
      double sign1 = (axis[0] > -EPS) ? 1. : -1.;
      double sign2 = (axis[1] > -EPS) ? 1. : -1.;
      double sign3 = (axis[2] > -EPS) ? 1. : -1.;
      T *= sign1 * sign2 * sign3;
    }

  T *=  sin(0.5 * d_o);

  int rmask = (1 << ROT1_BITS)-1;
  trans.m_rot = 0;
  s =  ComputeRotCode(T[0]);	
        trans.m_rot |= rmask & s;	trans.m_rot <<= ROT1_BITS;
  s =  ComputeRotCode(T[1]);	
        trans.m_rot |= rmask & s;	trans.m_rot <<= ROT1_BITS;
  s = ComputeRotCode(T[2]);	
        trans.m_rot |= rmask & s;	trans.m_rot <<= ROT1_BITS;

  trans.m_modelId = d_modelId;
  if(p)
    {
//	    cout << form("rotation %f %f %f\n",T[0],T[1],T[2]);
      cout << "rotation " <<T[0]
           << " " << T[1]
           << " " << T[2]
           << "\n" ;
//	    cout << form("tran num %d rot num %d\n",trans.m_tran,trans.m_rot);
      cout << "tran num " <<trans.m_tran
           << " rot num " <<trans.m_rot
           << "\n";
  
      char buf[100];
      fgets(buf,5,stdin);
    }
  }

  int ComputeTranCode(double x)
  {
    double s = x;    
    if(s > TRAN_MAX)s = TRAN_MAX; 
    if(s < TRAN_MIN)s = TRAN_MIN; 
    s -= TRAN_MIN;
    s /= TRAN_BIN_WIDTH;
    int ival = s + 0.5;
    return ival;
  }
  int ComputeRotCode(double x)
  {
    double s = x;    
    if(s > ROT1_MAX)s = ROT1_MAX; 
    if(s < ROT1_MIN)s = ROT1_MIN; 
    s -= ROT1_MIN;
    s /= ROT1_BIN_WIDTH;
    int ival = s + 0.5;
    return ival;
  }

};


class HashKeyBag 
{
  TransformationClass* bins[1 << TRANSFORMATION_HASH_BITS];
public:
  int count;
  int topscore;
  int min_score;
  TransformationClass *top;
  HandFeature *m_feat;
  HandFeature *m_qfeat;

#define HIST_BINS 100
  int m_hist[HIST_BINS];

  HashKeyBag()
  {
    m_feat = m_qfeat = NULL;
    count = topscore = 0;
    top = NULL;
    int ln = 1<<TRANSFORMATION_HASH_BITS;
    for(int i = 0; i < ln ; i++)
      {
  bins[i] = NULL;
      }
  }
  int ConfigHandTransformation(Hand_Transformation_Light &hndL, Hand_Transformation *hnd)
  {    
    Hand_Transformation shyp(m_feat[ hndL.m_mf ],m_qfeat[ hndL.m_qf ]);
    *hnd = shyp;
    return 1;
  }

  TransformationClass * Bin(int ind)
  {
    assert( ind >= 0 ); 
    assert( ind < (1<<TRANSFORMATION_HASH_BITS));
    return bins[ind];
  }
//   int AddHandHypo(Hand_Transformation * ahyp)
//   {
//     Hand_Transformation *hyp = new Hand_Transformation(*ahyp);
//     assert(hyp != NULL);
//     TransformationClass *t = GetTransClass(ahyp->trans);
//     if(t != NULL)
//       {
// 	hyp->next = t->hypo;
// 	t->hypo = hyp;
// 	t->score++;
//       }
//     else
//       {
// 	t = new TransformationClass;
// 	assert(t != NULL);
// 	*t = ahyp->trans;
// 	t->next = bins[t->Hash()];
// 	bins[t->Hash()] = t;
// 	t->hypo = hyp;
// 	t->score=1;
//       }
//     count++;
//     if(t->score > topscore)
//       {
// 	topscore = t->score;
// 	top = t;
//       }
//     return count;
//   }
  int AddHandHypo(TransformationClass &trans,Hand_Transformation_Light &ahypL)
  {
    Hand_Transformation_Light *hyp = new Hand_Transformation_Light();
    *hyp = ahypL;
    assert(hyp != NULL);
    TransformationClass *t = GetTransClass(trans);
    if(t != NULL)
      {
  hyp->next = t->hypo;
  t->hypo = hyp;
  m_hist[t->score]--;
  t->score++;
  m_hist[t->score]++;
  
      }
    else
      {
  t = new TransformationClass;
  assert(t != NULL);
  *t = trans;
  t->next = bins[t->Hash()];
  bins[t->Hash()] = t;
  t->hypo = hyp;
  t->score=1;
  m_hist[t->score]++;
      }
    count++;
    if(t->score > topscore)
      {
  topscore = t->score;
  top = t;
      }
    return count;
  }
  TransformationClass * GetTransClass(TransformationClass &cls)
  {
    TransformationClass *t = bins[cls.Hash()];
    if(t == NULL)
      return NULL;
    else
      {
  while(t != NULL)
    {
      if (*t == cls)
    return t;
      else
        t = t->next;
    }
      }
    return NULL;
  }

  void Write(FILE *fPF, int min = 0)
  {
    int len = 1<<TRANSFORMATION_HASH_BITS;
    for(int i = 0;i<len;i++)
      {
  TransformationClass *t = bins[i];
  while(t != NULL)
    {
      if(t->score >= min)
        {
    Hand_Transformation_Light *sh = t->hypo;
    while(sh != NULL)
      {
        Hand_Transformation shyp;
        ConfigHandTransformation(*sh,&shyp);
        shyp.score = t->score;
        shyp.Write(fPF);
        sh = sh->next;
      }
        }
      t = t->next;
    }
      }
  }
};

class Model
{
public:
  Hand *mod;
  Model *next;
  Model(){mod = NULL;next = NULL;}
  int Id()
  {
    if(mod != NULL)
      return mod->ModelId();
    else
      return 0;
  }
};


class FeatureHypoManager
{

  HashKeyBag HypoBag;
  FILE *FeatureDB;
  FILE *ModelDB;
public:
  Model *models;
  Hand *hands;
#define MAX_FEATURES 150000
#define MAX_QFEATURES 150000
  HandFeature features[MAX_FEATURES];
  HandFeature query[MAX_QFEATURES];
  int m_nfeats;
  int m_nqfeats;
  int m_cur_bin;
  int m_min_score;
  TransformationClass *cur_transclass;
  Hand_Transformation_Light *cur_hypo;


  FeatureHypoManager() 
  {
    HypoBag.m_feat = features;
    HypoBag.m_qfeat = query;
    m_min_score = 0; m_cur_bin = 0; 
    m_nfeats = m_nqfeats = 0;
    cur_hypo = NULL;
    cur_transclass = NULL; 
    models = NULL;
    //features = NULL; 
    //query = new HandFeature;
    assert(query != NULL);
  }

  Hand_Transformation * CreateHandTransformation(Hand_Transformation_Light &hndL)
  {
    Hand_Transformation shyp(features[ hndL.m_mf ],query[ hndL.m_qf ]);
    return new Hand_Transformation(shyp);
  }
  void InitCursor()
  {
    m_cur_bin = -1; 
    cur_hypo = NULL;
    cur_transclass = NULL; 
  }
  int NextHypo()
  {
    if(cur_hypo == NULL)
      return 0;
    cur_hypo = cur_hypo->next;
    if(cur_hypo != NULL)
      {	return 1;      }
    return 0;
  }
  int NextTransClass()
  {
    if(cur_transclass == NULL)
      return 0;
    cur_transclass = cur_transclass->next;
    if (cur_transclass != NULL)
      {
  cur_hypo = cur_transclass->hypo;
  return 1;
      }
    else
      cur_hypo = NULL;
    return 0;
  }
  int NextBin()
  {
    int len = 1<<TRANSFORMATION_HASH_BITS;
    m_cur_bin++;
    while(m_cur_bin <  len)
      {
  if((cur_transclass = HypoBag.Bin(m_cur_bin)) != NULL)
    {
      cur_hypo = cur_transclass->hypo;
      return 1;
    }
  m_cur_bin++;
      }
    return 0;
  }
  Hand * GetModel(int id)
  {
    Model *modl = models;
    while(modl != NULL)
      {
  if (id == modl->Id())
    {
      return modl->mod;
    }
  modl = modl->next;
      }
    return NULL;
  }

  int SetOpContext(FILE *aFeatureDB,FILE *aModelDB,Hand *hnd,int min_score)
  {
    hands = hnd;
    m_min_score = min_score;
    FeatureDB = aFeatureDB;
    ModelDB = aModelDB;
    while(!feof(FeatureDB))
      {
  HandFeature feat;
  if(feat.Read(FeatureDB))
    {
      features[m_nfeats] = feat;
      m_nfeats++;
      assert(m_nfeats < MAX_FEATURES);
    }
      }
    // Collapse and sort features
    qsort(features,m_nfeats,sizeof(HandFeature),compareHandFeatures);

    //
    //  Assumes that feature database was prepared with same version of bvh file
    //
#define MAX_MODELS 500
    int state = hands->m_bvh.m_nframes;
    hands->m_bvh.AddStates(MAX_MODELS);
    while(!feof(ModelDB))
      {	
  if(hands->m_bvh.ReadState(&state,ModelDB))
    {
      assert(state < MAX_MODELS);
    }
      }

    for(int i=0;i<HIST_BINS;i++)
      {
  HypoBag.m_hist[i]=0;
      }    
    printf("OpContext Set\n");
    return 1;
  }
  int AddInputItem(HandFeature *aQueryFeature)
  {
    query[m_nqfeats] = *aQueryFeature; 
    m_nqfeats++;
    assert(m_nqfeats < MAX_QFEATURES);
    return 1;
  }
  int BagTransforms()
  {
    qsort(query,m_nqfeats,sizeof(HandFeature),compareHandFeatures);

    int nf = 0; 
    int nq = 0;
    while(nf < m_nfeats && nq < m_nqfeats)
      {
  if(features[nf].key == query[nq].key)
    {
      // Add all combinations of equivalent features to bag
      int i,j;
      for(i = nq; i < m_nqfeats; i++)
        {
    if(query[i].key != features[nf].key)
      {break;}
    for( j = nf; j < m_nfeats; j++)
      {
        if(query[i].key == features[j].key)
          {
      Hand_Transformation_Light shypL;
      shypL.m_mf = j;
      shypL.m_qf = i;
      Hand_Transformation shyp(features[j],query[i]);
      HypoBag.AddHandHypo(shyp.trans, shypL);
          }
        else
          break;
      }
        }
      nf = j;
      nq = i;
    }
  else if (features[nf].key < query[nq].key)
    nf++;
  else if (features[nf].key > query[nq].key)
    nq++;
      }
    return 1;
  }

  int FinalizeProcessing()
  {
    BagTransforms();

    //sort hypos on score
    //for now just write out the hypos and report the top;
    InitCursor();

    FILE *out = fopen("RawHypos.dat","w");
    HypoBag.Write(out,m_min_score);
    // just return the top score
    printf("// !!!!!!!!!!! Top Score = %d \n // ", HypoBag.topscore);
    HypoBag.top->Write(stdout);
    int tot = 0;
    for(int i=0;i<HIST_BINS;i++)
      {
  if(HypoBag.m_hist[i] > 0)
    {
      tot += HypoBag.m_hist[i];
      fprintf(stdout,"// score %d count %d cumulant %d\n",i,HypoBag.m_hist[i],tot);
    }
      }
    return 1;
  }
  int GetNextOutputItem(Hand_Transformation *aHyp)
  {
    int flg = 1;
    while(flg)
      {
  //Use if each hypo within the same transfomation is desired:
  //     if(!NextHypo())
    if(!NextTransClass())
      NextBin();
  if(cur_hypo != NULL)
    {
      if(cur_transclass->score >= m_min_score)
        {
    //		cur_hypo->score = cur_transclass->score;
    //		*aHyp = *cur_hypo;    
    HypoBag.ConfigHandTransformation(*cur_hypo,aHyp);
    aHyp->score = cur_transclass->score;
    return 1;
        }
    }
  else flg=0;
      }
    return 0;
  }

  int ClearOpContext(){return 1;}

};
#endif
