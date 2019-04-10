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
 /***************************************************************************
 * Project:         BlueMatter
 *
 * Module:          P3ME
 *
 * Purpose:         P3ME method for periodic Coulomb Interactions
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010915 RZ (JCP 115, 2348-2358,2001)
 *
 ***************************************************************************/

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "P3MEimpl.hpp"
#include "P3MEmultiDimArray.hpp"


int p3m_const_pressure_set(p3m_t *that)
{
  assert (that!=NULL);
  that->isFlexibleBox=1; /* box size changes in simulation */
  return 0;
}

int p3m_prepare(p3m_t *     that,
		double      boxx,
		double      boxy,
		double      boxz,
                double      alpha
		)
{
  /* gridSpacingX is used as an initial guess for each dimension */
  that->meshPtsX = p3m_choose_mesh_points(boxx, that->gridSpacingX);
  that->meshPtsY = p3m_choose_mesh_points(boxy, that->gridSpacingX);
  that->meshPtsZ = p3m_choose_mesh_points(boxz, that->gridSpacingX);

  fprintf(stdout, "P3M: using {%d, %d, %d} mesh partition\n",
	  that->meshPtsX, that->meshPtsY, that->meshPtsZ);
  
  that->gridSpacingX = boxx/that->meshPtsX;
  that->gridSpacingY = boxy/that->meshPtsY;
  that->gridSpacingZ = boxz/that->meshPtsZ;

  that->grdSpX_1 = 1.0/that->gridSpacingX;
  that->grdSpY_1 = 1.0/that->gridSpacingY;
  that->grdSpZ_1 = 1.0/that->gridSpacingZ;
  
  fprintf(stdout, "P3M: using grid spacing {%10.5f, %10.5f, %10.5f}\n",
	  that->gridSpacingX, that->gridSpacingY, that->gridSpacingZ);
  
  that->alpha = alpha;

  /* by default the center of the mesh is taken to be zero */
  that->meshCenter.x = that->meshCenter.y = that->meshCenter.z = 0.0;  
		
  that->cache.numChg = 0;
  that->cache.asgnwgt = 0;
  
  p3m_allocate(that);

  p3m_green_function(that);

  p3m_create_FFT_plan(that);

  p3m_zero_mesh_charge(that);
  
  that->isPrepared = 1;

  return 0;
}

p3m_t *p3m_create(double    grdSp,  /* grid spacing */
		  diffop_t  diffOp, /* differentiator */
		  assign_t  assgn   /* order of assignment scheme(0,1,2) */
                  )
{
  p3m_t *that = (p3m_t *)safe_malloc(sizeof(p3m_t));

  that->gridSpacingX = grdSp;
  that->diffOp = diffOp;
  that->assgn = assgn;
  that->isPrepared = 0;
  return that; 
}  

/*
 * Since FFTW is especially fast when the number of points can be
 * written as $2^a 3^b 5^c 7^d 11^e 13^f$, we try to use such numbers
 * for partitioning the mesh. we use the smallest number $2^a 3^b 5^c
 * 7^d$ that's larger than $[boxsize/grdSp]$ as the number of
 * meshpoints. 
 */
int p3m_choose_mesh_points(double    boxsize,
			   double    grdSp)
{
  double min = ceil(boxsize/grdSp);
  int a, b, c, d;
  int amin, bmin, cmin, dmin;
  int meshPts;
  double lnmin, rema, remb, remc;
  double excess, minexcess;
#define LOG2 0.693147181
#define LOG3 1.098612289
#define LOG5 1.609437912
#define LOG7 1.945910149
  static const double ln2=LOG2, ln3=LOG3, ln5=LOG5, ln7=LOG7;
  static const double ln2_1=1.0/LOG2, ln3_1=1.0/LOG3,
    ln5_1=1.0/LOG5, ln7_1=1.0/LOG7;

  lnmin = log(min);

  amin = (int)floor(lnmin*ln2_1) + 1;
  bmin = cmin = dmin = 0;
  minexcess = amin*ln2 - lnmin;
  
  for (a=0; a<=(int)floor(lnmin*ln2_1)+1; a++) {
    rema = lnmin - a*ln2;
    for (b=0; b<=(int)floor(rema*ln3_1)+1; b++) {
      remb = rema - b*ln3;
      for (c=0; c<=(int)floor(remb*ln5_1)+1; c++) {
	remc = remb - c*ln5;
	d = (int)floor(remc*ln7_1) + 1;
	excess = d*ln7 - remc;
	if (excess<minexcess) {
	  amin = a;
	  bmin = b;
	  cmin = c;
	  dmin = d;
	  minexcess = excess;
	}
      }
    }
  }
  meshPts = 1;
  for (a=0; a<amin; a++)
    meshPts *= 2;
  for (b=0; b<bmin; b++) 
    meshPts *= 3;
  for (c=0; c<cmin; c++)
    meshPts *= 5;
  for (d=0; d<dmin; d++)
    meshPts *= 7;
  return meshPts;
}

void p3m_shift_mesh(p3m_t       *that,
		    cartVec_t   newCm)
{
  that->meshCenter = newCm;
}

void p3m_allocate(p3m_t    *that)
{
  /* the last dimension of the output of real-to-complex Fourier
     transform is slightly larger than half of the input dimension */
  NEW_3D_ARRAY(double, that->meshPtsX, that->meshPtsY, that->meshPtsZ/2+1,
	       that->greenFunct);

  NEW_3D_ARRAY(double, that->meshPtsX, that->meshPtsY, that->meshPtsZ,
	       that->chgGrid);

  NEW_3D_ARRAY(double, that->meshPtsX, that->meshPtsY,
	       2*(that->meshPtsZ/2 + 1), /*necessary for in-place transform*/
	       that->potFld);

  switch (that->diffOp) {
  /* for these differentiators we need to store the electric field on
     the mesh */
  case FINITE_2PT:
  case FINITE_4PT:
  case FINITE_6PT:
  case FINITE_8PT:
  case FINITE_10PT:
  case FINITE_12PT:
  case ANALYTICAL:
    NEW_3D_ARRAY(cartVec_t, that->meshPtsX, that->meshPtsY,
		 2*(that->meshPtsZ/2 + 1), /* for in-place transform */
		 that->efld);
    break;
  }
}

void p3m_create_FFT_plan(p3m_t   *that)
{
  int mptsX=that->meshPtsX, mptsY=that->meshPtsY, mptsZ=that->meshPtsZ;
  that->fftPlan.chgFFT = rfftw3d_create_plan
    (mptsX, mptsY, mptsZ, FFTW_REAL_TO_COMPLEX, FFTW_MEASURE);
  that->fftPlan.fldFFT = rfftw3d_create_plan
    (mptsX, mptsY, mptsZ, FFTW_COMPLEX_TO_REAL, FFTW_MEASURE|FFTW_IN_PLACE);
}
					     
void p3m_zero_mesh_charge(p3m_t     *that)
{
  int n1, n2, n3;
  for (n1=0; n1<that->meshPtsX; n1++)
    for (n2=0; n2<that->meshPtsY; n2++)
      for (n3=0; n3<that->meshPtsZ; n3++)
	that->chgGrid[n1][n2][n3] = 0;
}

void p3m_destroy(p3m_t     *that)
{
  destroy_3d_array((void ***)that->greenFunct);
  destroy_3d_array((void ***)that->chgGrid);
  destroy_3d_array((void ***)that->potFld);
  switch (that->diffOp) {
  case FINITE_2PT:
  case FINITE_4PT:
  case FINITE_6PT:
  case FINITE_8PT:
  case FINITE_10PT:
  case FINITE_12PT:
  case ANALYTICAL:
    destroy_3d_array((void ***)that->efld);
  }
  p3m_destroy_FFT_plan(that);
  p3m_free_cache(that);
  free(that);
}

void p3m_destroy_FFT_plan(p3m_t    *that)
{
  rfftwnd_destroy_plan(that->fftPlan.chgFFT);
  rfftwnd_destroy_plan(that->fftPlan.fldFFT);
}

void p3m_resize_cache(p3m_t       *that,
		    int         newNumChg)
{
  asgnw_t *oldwgt;
  int nq, oldNum;
  p3mCache_t *cache = &(that->cache);
  
  cache->dirty = 0; /* use cache */

  /* already has the storage of the right size, nothing to be done */
  if (cache->numChg==newNumChg)
    return;
  
  /* clean up the old storage */
  oldwgt = cache->asgnwgt;
  oldNum = cache->numChg;
  for (nq=0; nq<oldNum; nq++) {
    free(oldwgt[nq].wgtx); oldwgt[nq].wgtx = 0;
    free(oldwgt[nq].wgty); oldwgt[nq].wgty = 0;
    free(oldwgt[nq].wgtz); oldwgt[nq].wgtz = 0;
  }
  free(oldwgt);
  
  cache->asgnwgt = (asgnw_t *)safe_malloc(newNumChg*sizeof(asgnw_t));
  
  for (nq=0; nq<newNumChg; nq++) {
    cache->asgnwgt[nq].wgtx =
      (double *)safe_malloc((int)(that->assgn)*sizeof(double));
    cache->asgnwgt[nq].wgty =
      (double *)safe_malloc((int)(that->assgn)*sizeof(double));
    cache->asgnwgt[nq].wgtz =
      (double *)safe_malloc((int)(that->assgn)*sizeof(double));
  }
  cache->numChg = newNumChg;
}

void p3m_free_cache(p3m_t     *that)
{
  int nq;
  p3mCache_t *cache = &(that->cache);

  if (cache->asgnwgt) {
    for (nq=0; nq<cache->numChg; nq++) { 
      free(cache->asgnwgt[nq].wgtx);
      cache->asgnwgt[nq].wgtx = 0;
      free(cache->asgnwgt[nq].wgty);
      cache->asgnwgt[nq].wgty = 0;
      free(cache->asgnwgt[nq].wgtz);
      cache->asgnwgt[nq].wgtz = 0;
    }
    free(cache->asgnwgt);
  }
  cache->asgnwgt = NULL;
}

void p3m_cache_dump(p3m_t     *that)
{
  int nq, p;
  int asgnOrd = (int)(that->assgn);
  p3mCache_t cache = that->cache;
  printf("P3M: dumping cache ......\n");
  if (cache.dirty) {
    printf("cache dirty!!!\n");
    return;
  }
  else {
    printf("cache clean!!!\n");
    printf("memorized %d charges.\n", cache.numChg);
    for (nq=0; nq<cache.numChg; nq++) {
      printf("charge %d, reference grid: {%d, %d, %d}\n",
	     nq, cache.asgnwgt[nq].refgd1, cache.asgnwgt[nq].refgd2,
	     cache.asgnwgt[nq].refgd3);
      for (p=0; p<asgnOrd; p++) 
	printf("cached weights: wx[%d]=%f, wy[%d]=%f, wz[%d]=%f\n",
	       p, cache.asgnwgt[nq].wgtx[p], p, cache.asgnwgt[nq].wgty[p],
	       p, cache.asgnwgt[nq].wgtz[p]);
      printf("\n");
    }
  }
  fflush(stdout);
}

void p3m_k_space_field(p3m_t      *that,
		       int        numChg,
		       double     *chgMag,
		       cartVec_t  *chgPos,
                       double     boxx,
                       double     boxy,
                       double     boxz,
                       double     alpha,
		       int        fldDesc,
		       int        numFldPts,
		       cartVec_t  *fldPos,
		       double     *potFld,
		       cartVec_t  *eFld)
{
  assert (that!=NULL);

  if (that->isPrepared==0) 
    p3m_prepare(that, boxx, boxy, boxz, alpha);
    
  /* check if we have enough space for cache, if it's used */
  if (fldDesc & P3M_SRCFIELD) 
    p3m_resize_cache(that, numChg);
  else
    that->cache.dirty = 1; /* don't use cache */
  
  p3m_assign_charge(numChg, chgMag, chgPos, that);

  /* p3m_cache_dump(that);  */
  
  p3m_mesh_field(that);

  /* check if we are only computing the field at the source*/
  if (!(fldDesc^P3M_SRCFIELD)) {
    numFldPts = numChg;
    fldPos = chgPos;
  }

/*    p3m_cache_dump(that);  */
  if (that->diffOp!=GRADIENT)
    p3m_interpolate_field(that, numFldPts, fldPos, potFld, eFld);
  else
    p3m_gradient_field(that, numFldPts, fldPos, potFld, eFld);
  
  /* p3m_cache_dump(that);  */
}

diffop_t p3m_differentiator_encode(char  *diffStr)
{
  if (!strcmp(diffStr, "FINITE_2PT"))
    return FINITE_2PT;
  else if (!strcmp(diffStr, "FINITE_4PT"))
    return FINITE_4PT;
  else if (!strcmp(diffStr, "FINITE_6PT"))
    return FINITE_6PT;
  else if (!strcmp(diffStr, "FINITE_8PT"))
    return FINITE_8PT;
  else if (!strcmp(diffStr, "FINITE_10PT"))
    return FINITE_10PT;
  else if (!strcmp(diffStr, "FINITE_12PT"))
    return FINITE_12PT;
  else if (!strcmp(diffStr, "GRADIENT"))
    return GRADIENT;
  else if (!strcmp(diffStr, "ANALYTICAL"))
    return ANALYTICAL;
  else {
    fprintf(stderr, "unknown differentiator: %s\n", diffStr);
    exit(1);
  }
  return GRADIENT; /* never gets here */
}

assign_t p3m_assignment_encode(char  *assgnStr)
{
  if (!strcmp(assgnStr, "NGP")) 
    return NGP;
  else if (!strcmp(assgnStr, "TSC"))
    return TSC;
  else if (!strcmp(assgnStr, "CIC"))
    return CIC;
  else if (!strcmp(assgnStr, "A4P"))
    return A4P;
  else {
    fprintf(stderr, "unknown assignment scheme: %s\n", assgnStr);
    exit(1);
  }
  return TSC; /* never gets here */
}














