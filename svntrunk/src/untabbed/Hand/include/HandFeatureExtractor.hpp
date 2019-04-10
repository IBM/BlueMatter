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
 
#ifndef _INCLUDED_HANDFEATUREXTRACTOR_
#define _INCLUDED_HANDFEATUREXTRACTOR_

#include <HandFeature.hpp>


////////////////////////////////////////////////////////////////////////////////////////////////////////
class HandFeatureExtractor
{
public:

  Polygon *m_seg1;
  Polygon *m_seg2;
  Polygon *m_seg3;
  int m_c1,m_c2,m_c3;
  HandFeature m_curFeature;
  Hand *m_hand; 

  HandFeatureExtractor()  {    m_seg1 = m_seg2 = m_seg3 = NULL;    m_hand = NULL;  }
  void SetHand(Hand *hnd){ m_hand = hnd;}

  int InitSegmentTuple();
  int NextSegmentTuple();
  int NextExposedPoint(int pnt);
  int NextSelectedPoint(int pnt, int selection);
  int ComputeSegmentTupleFeatures(int *nfeatures, HandFeature  *feats, int max, int query);
};

/////////////////////////////////////////////////////////////////////
class HandFeatureExtractionManager
{
public:

  HandFeatureExtractor *m_hndfeat;
  HandFeature *m_feats;
  int m_nfeats,m_curfeat;
  FILE *m_modelPF,*m_featPF;
  int m_query;
  double m_scale;
  int writedatasingleline;
  FILE *singlelinedataPF;	  

  HandFeatureExtractionManager();
  ~HandFeatureExtractionManager()  {    delete [] m_feats;  }
  void SetOutputFiles(FILE *modPF, FILE *featPF){ m_modelPF = modPF; m_featPF = featPF;}
  int RecordFeature(HandFeature *feat){return feat->Write(m_featPF);}
  int RecordModel(Hand *hnd){hnd->Write(m_modelPF); return 1;}


  ////////////////////
  ////////////////////
  int SetOpContext(HandFeatureExtractor * hndfeat, int query);
  int AddInputItem(Hand *aHand);
  int FinalizeProcessing();
  int GetNextOutputItem(HandFeature **aFeat);
  int ClearOpContext();

  int ExtractHandFeatures(Hand *hnd,int query);
  /////////////////////
  ////////////////////
};
 
/////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


HandFeatureExtractionManager::HandFeatureExtractionManager()
{
  m_hndfeat = NULL;
  m_feats = new HandFeature[MAX_FEATURES_PER_SNAKE];
  assert(m_feats != NULL);
  m_nfeats = 0;
  m_curfeat = 0;
  m_query = 0;
  m_modelPF = NULL;
  m_featPF = NULL;
  writedatasingleline = 0;
  singlelinedataPF = NULL;	  
}



int HandFeatureExtractionManager::SetOpContext(HandFeatureExtractor * hndfeat, int query = 0)
{
  m_query = query;
  m_hndfeat = hndfeat;
  return 1;
}
int HandFeatureExtractionManager::AddInputItem(Hand *aHand)
{
  m_hndfeat->SetHand(aHand);
  m_nfeats = 0;
  return m_hndfeat->InitSegmentTuple();
}
int HandFeatureExtractionManager::FinalizeProcessing()
{
  int n;
  do
    {
      n = 0;
      m_hndfeat->ComputeSegmentTupleFeatures(&n, &m_feats[m_nfeats] , MAX_FEATURES_PER_SNAKE,m_query);
      m_nfeats += n;
      if(m_nfeats >= MAX_FEATURES_PER_SNAKE)
  {
    printf("Number of features %d exceeds max: %d\n",m_nfeats,MAX_FEATURES_PER_SNAKE);
    assert(m_nfeats < MAX_FEATURES_PER_SNAKE);
  }
    }while(m_hndfeat->NextSegmentTuple());

  if(m_nfeats > 0)
    {    if(m_modelPF != NULL)RecordModel(m_hndfeat->m_hand); }
  m_curfeat = 0;
  return 1;
}
int HandFeatureExtractionManager::GetNextOutputItem(HandFeature **aFeat)
{
  if(m_curfeat == m_nfeats)
    return 0;
  *aFeat = &m_feats[m_curfeat];
  m_curfeat++;
  return 1;
}
int HandFeatureExtractionManager::ClearOpContext()
{
  m_hndfeat = NULL;
  m_curfeat = m_nfeats = 0;
  return 1;
}

int HandFeatureExtractionManager::ExtractHandFeatures(Hand *hnd, int query = 0)
{
  HandFeatureExtractor hndfeatX;
  HandFeature *feat;
    
  SetOpContext(&hndfeatX,query);
  //hnd->SetModelId(hnd->ModelId() + 1);
  int Rc = AddInputItem(hnd);
  if(Rc)
    {
      Rc = FinalizeProcessing();      
      while(GetNextOutputItem(&feat))
  {
    RecordFeature(feat);
    if (WRITE_FEATURE_POV)
      {
        feat->WritePovray(PV_OUT_FILE);
      }
    if(writedatasingleline)
      {
        feat->WriteSingleLine(singlelinedataPF,0);	  
      }
    
  }
    }
  ClearOpContext();
  return Rc;
}


int HandFeatureExtractor::NextExposedPoint(int pnt)
{
  Vector p;
  int c = pnt + 1;
  while(!m_hand->ExposedFace(c))
    { 
      c++;
      if(c > m_hand->m_pcur.m_nfaces)
  { return 0;}
    }
  return c;
}

int HandFeatureExtractor::NextSelectedPoint(int pnt, int selection)
{
  Vector *p;
  int c = NextExposedPoint(pnt);
  if (c == 0)
    return 0;
  int * s;
  switch(selection)
    {
    case 1:      s = m_hand->m_selected1; break;
    case 2:      s = m_hand->m_selected2; break;
    case 3:      s = m_hand->m_selected3; break;
    default: assert(0);
    }
  while(s[c] != 1)
    { 
      c = NextExposedPoint(c);
      if(c == 0)
  { return 0;}
    }
  return c;
}

int HandFeatureExtractor::InitSegmentTuple()
{
  m_c1 = NextSelectedPoint(0,1);
  m_c2 = NextSelectedPoint(0,2);
  if(m_c2 == m_c1)
    m_c2 = NextSelectedPoint(m_c2,2);
  if(m_c1 == 0 || m_c2 == 0)
    return 0;
  m_c3 = 0;
  return  NextSegmentTuple();
}
int HandFeatureExtractor::NextSegmentTuple()
{
  double dd,ddd;
  do
    {
      if(m_c1 == 0)
  {
    return 0;
  }
      if(m_c2 == 0)
  {
    m_c1 = NextSelectedPoint(m_c1,1);
    m_c2 = 0;
    do
      {
        m_c2 = NextSelectedPoint(m_c2,2);
        if(m_c1 == m_c2)
    m_c2 = NextSelectedPoint(m_c2,2);
        if(m_c2 == 0)
    break;
        Vector pp = m_hand->m_facecenter[m_c1] - m_hand->m_facecenter[m_c2];	
        dd = pp/pp - MIN_SIDE * MIN_SIDE;
        ddd = pp/pp - MAX_SIDE * MAX_SIDE;
      }while( dd < 0.0 || ddd > 0.0);

    m_c3 = 0;
    if(WRITE_FEATURE_POV)
      fprintf(PV_OUT_FILE,"// Base point %d of %d\n",m_c1,m_hand->m_pcur.m_nfaces);
  }
      m_c3 = NextSelectedPoint(m_c3,3);
      if(m_c3 == m_c2)
  m_c3 = NextSelectedPoint(m_c3,3);
      if(m_c3 == 0)
  {
    do
      {
        m_c2 = NextSelectedPoint(m_c2,2);
        if(m_c1 == m_c2)
    m_c2 = NextSelectedPoint(m_c2,2);
        if(m_c2 == 0)
    break;
        Vector pp = m_hand->m_facecenter[m_c1] - m_hand->m_facecenter[m_c2];	
        dd = pp/pp - MIN_SIDE * MIN_SIDE;
        ddd = pp/pp - MAX_SIDE * MAX_SIDE;
      }while( dd < 0.0 || ddd > 0.0);
  }
    }while((m_c1 == 0) || (m_c2 == 0) || (m_c3 == 0));
  
  m_seg1 = &m_hand->m_pcur.m_face[m_c1];
  m_seg2 = &m_hand->m_pcur.m_face[m_c2];
  m_seg3 = &m_hand->m_pcur.m_face[m_c3];

  return 1;
}


int HandFeatureExtractor::ComputeSegmentTupleFeatures(int *nfeatures, HandFeature  *feats, int max, int query = 0)
{
  // pass query = 1 if normals are to be inverted

  if (*nfeatures >= max )
    return 0;

    
//   m_seg1->Center(m_hand->m_pcur.m_vert,v1);
//   m_seg2->Center(m_hand->m_pcur.m_vert,v2);
//   m_seg3->Center(m_hand->m_pcur.m_vert,v3);
  
  int valid,nfeats = 0;
  Vector *v1,*v2,*v3;
  v1 = &m_hand->m_facecenter[m_c1];
  v2 = &m_hand->m_facecenter[m_c2];
  v3 = &m_hand->m_facecenter[m_c3];

  if(query)
    valid = 1;
  else
    valid = !m_hand->DoesTriangleIntersectHand(*v1,*v2,*v3);
    
    
  if(valid)
    {
      m_curFeature.Init();
      m_curFeature.m_model_id = m_hand->m_model_id;
      double d;
      Vector n1;
      Vector n2;
      Vector n3;
      m_seg1->PolygonNorm(m_hand->m_pcur.m_vert,n1);
      m_seg2->PolygonNorm(m_hand->m_pcur.m_vert,n2);
      m_seg3->PolygonNorm(m_hand->m_pcur.m_vert,n3);
      Vector org1 = (*v1 + *v2 + *v3) / 3.0;
      m_curFeature.SetOrigin(org1);			
  
      m_curFeature.SetTupleIds(m_c1,m_c2,m_c3);
      m_curFeature.SetVerticiesAndNormals(*v1,*v2,*v3,n1,n2,n3);
      if(query)
  m_curFeature.InvertNormals();
      if(m_curFeature.ComputeKey())
  {
    m_curFeature.m_feature_id++;
    feats[nfeats] = m_curFeature;
    nfeats++; assert(nfeats <= max);
  }
  
      int do_perm = 0;
      if(query && do_perm)  // Do additional 5 permutations on triangle
  {
    // Perm 1 3 2
    m_curFeature.SetTupleIds(m_c1,m_c3,m_c2);
    m_curFeature.SetVerticiesAndNormals(*v1,*v3,*v2,n1,n3,n2);
    m_curFeature.InvertNormals();
    if(m_curFeature.ComputeKey())
      {m_curFeature.m_feature_id++;feats[nfeats] = m_curFeature;nfeats++; assert(nfeats<=max);}
      
    // Perm 2 1 3
    m_curFeature.SetTupleIds(m_c2,m_c1,m_c3);
    m_curFeature.SetVerticiesAndNormals(*v2,*v1,*v3,n2,n1,n3);
    m_curFeature.InvertNormals();
    if(m_curFeature.ComputeKey())
      {m_curFeature.m_feature_id++;feats[nfeats] = m_curFeature;nfeats++;assert(nfeats<=max);}
      
    // Perm 2 3 1
    m_curFeature.SetTupleIds(m_c2,m_c3,m_c1);
    m_curFeature.SetVerticiesAndNormals(*v2,*v3,*v1,n2,n3,n1);
    m_curFeature.InvertNormals();
    if(m_curFeature.ComputeKey())
      {m_curFeature.m_feature_id++;feats[nfeats] = m_curFeature;nfeats++;assert(nfeats<=max);}
      
    // Perm 3 1 2
    m_curFeature.SetTupleIds(m_c3,m_c1,m_c2);
    m_curFeature.SetVerticiesAndNormals(*v3,*v1,*v2,n3,n1,n2);
    m_curFeature.InvertNormals();
    if(m_curFeature.ComputeKey())
      {m_curFeature.m_feature_id++;feats[nfeats] = m_curFeature;nfeats++;assert(nfeats<=max);}
      
    // Perm 3 2 1
    m_curFeature.SetTupleIds(m_c3,m_c2,m_c1);
    m_curFeature.SetVerticiesAndNormals(*v3,*v2,*v1,n3,n2,n1);
    m_curFeature.InvertNormals();
    if(m_curFeature.ComputeKey())
      {m_curFeature.m_feature_id++;feats[nfeats] = m_curFeature;nfeats++;assert(nfeats<=max);}
      
  }
    }
  else
    {
      //printf("Rejected feature\n");
    }
  *nfeatures = nfeats;
  return nfeats;
}
  

#endif
