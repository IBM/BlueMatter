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
#include <rfftw.h>
#include <assert.h>
#include "P3MEimpl.hpp"

void p3m_mesh_field(p3m_t   *that)
{
  rfftwnd_plan *chgFFT = &(that->fftPlan.chgFFT),
    *fldFFT = &(that->fftPlan.fldFFT);
  double ***qm = that->chgGrid;
  double ***gft = that->greenFunct;
  complex_t ***pfldft = /* in-place Fourier transform of potential field */
    (complex_t ***)that->potFld; 
  double ***potFld = that->potFld;
  cartVec_t ***efld = that->efld;
  cmplxCart_t ***efldft = (cmplxCart_t ***)efld;
  
  int n1, n2, n3;
  cartVec_t kvec;
  int diffOrd, p, grdleft, grdright;
  int mptsX=that->meshPtsX, mptsY=that->meshPtsY, mptsZ=that->meshPtsZ,
    halfMptsZ = mptsZ/2+1;
  double grdSpX = that->gridSpacingX,
    grdSpY = that->gridSpacingY,
    grdSpZ = that->gridSpacingZ;
  double halfGrdSpX_1 = 0.5*that->grdSpX_1, halfGrdSpY_1 = 0.5*that->grdSpY_1,
    halfGrdSpZ_1 = 0.5*that->grdSpZ_1, p_1;
  cartVec_t kNyquist;
  cartVec_t kunit;
  double fdc;   /* finite difference coefficient */

  kNyquist.x = 2*M_PI*that->grdSpX_1;
  kNyquist.y = 2*M_PI*that->grdSpY_1;
  kNyquist.z = 2*M_PI*that->grdSpZ_1;
  
  rfftwnd_one_real_to_complex((*chgFFT), qm[0][0], pfldft[0][0]);
  
  /* \FFT(G(x) \convolving \rho_{M}(x)) = \FFT(G)*\FFT(\rho) */
  for (n1=0; n1<mptsX; n1++)
    for (n2=0; n2<mptsY; n2++)
      for (n3=0; n3<halfMptsZ; n3++) {

	pfldft[n1][n2][n3].re *= gft[n1][n2][n3];
	pfldft[n1][n2][n3].im *= gft[n1][n2][n3];

	/* the charge on mesh is no longer needed and we reset it to
           zero and get ready for the next computation. here
           qm[n1][n2][halfMptsZ-1] (and qm[n1][n2][halfMptsZ-2] in the
           case of even mptsZ) are zeroed twice, a little redundancy. */
	qm[n1][n2][n3] = 0.0;   
	qm[n1][n2][mptsZ-n3-1] = 0.0;
      }

  /* set the remaining qm to zero */
  qm[mptsX-1][mptsY-1][mptsZ-1] = 0.0;
  
  if (that->diffOp==ANALYTICAL) {
    kunit.x = kNyquist.x/mptsX;
    kunit.y = kNyquist.y/mptsY;
    kunit.z = kNyquist.z/mptsZ;
    
    /* inverse transform $i k \phi(k)$ to get the electric field */
    for (n1=0; n1<mptsX; n1++) {
      kvec.x = ((n1>(mptsX/2))?(n1-mptsX):n1)*kunit.x;

      for (n2=0; n2<mptsY; n2++) {
	kvec.y = ((n2>(mptsY/2))?(n2-mptsY):n2)*kunit.y;

	for (n3=0; n3<mptsZ/2+1; n3++) {
	  kvec.z = n3*kunit.z;

	  /* the real part of $ -i \vec{k} \phi(k) $ */
	  efldft[n1][n2][n3].x.re = pfldft[n1][n2][n3].im*kvec.x;
	  efldft[n1][n2][n3].y.re = pfldft[n1][n2][n3].im*kvec.y;
	  efldft[n1][n2][n3].z.re = pfldft[n1][n2][n3].im*kvec.z;
	  /******* real part south *******/

	  /* the imaginary part of $ -i \vec{k} \phi(k) $ */
	  efldft[n1][n2][n3].x.im = -pfldft[n1][n2][n3].re*kvec.x;
	  efldft[n1][n2][n3].y.im = -pfldft[n1][n2][n3].re*kvec.y;
	  efldft[n1][n2][n3].z.im = -pfldft[n1][n2][n3].re*kvec.z;
	  /******* imaginary part south ******/

	}
      }
    }
    rfftwnd_complex_to_real((*fldFFT),
			    P3M_SPACE_DIM, /* 3 transforms at once */
			    (complex_t *)efldft[0][0],
			    P3M_SPACE_DIM, /* the distance between
                                              elements in each array
                                              to be transformed */
			    1,  /* the distance between arrays */
			        /* below ignored in in-place transform */
			    NULL, 0, 0); 
  }
  
  /* inverse Fourier transform to potential field */
  rfftwnd_one_complex_to_real((*fldFFT), pfldft[0][0], NULL);

  /* normalize the FFT */
  /*  for (n1=0; n1<mptsX; n1++)
      for (n2=0; n2<mptsY; n2++)
      for (n3=0; n3<mptsZ; n3++) 
      potFld[n1][n2][n3] *= scale;
  */
  switch (that->diffOp) {
  case FINITE_2PT:
  case FINITE_4PT:
  case FINITE_6PT:
  case FINITE_8PT:
  case FINITE_10PT:
  case FINITE_12PT:
    /* use finite difference to find the electric field
     *    E_p = -(\phi_{p+1} - \phi_{p-1})/H
     * finite difference method usually has larger error, not recommented/
    
    /* use the hack that the integer value of FINITE_{2m}P is m-1 */
    diffOrd = (int)(that->diffOp)+1;
    
    for (n1=0; n1<mptsX; n1++)
      for (n2=0; n2<mptsY; n2++)
	for (n3=0; n3<mptsZ; n3++) {
	  efld[n1][n2][n3].x = efld[n1][n2][n3].y = efld[n1][n2][n3].z = 0.0;
	  for (p=1; p<=diffOrd; p++) {
	    fdc = p3mFiniteDiffCoeff[diffOrd-1][p-1];
	    p_1 = 1.0/p;
	    
	    grdleft = n1 - p;
	    if (grdleft<0) grdleft += mptsX;
	    grdright = n1 + p;
	    if (grdright>=mptsX) grdright -= mptsX;
	    efld[n1][n2][n3].x += fdc*
	      (potFld[grdleft][n2][n3] - potFld[grdright][n2][n3])*
	      halfGrdSpX_1*p_1;

	    grdleft = n2 - p;
	    if (grdleft<0) grdleft += mptsY;
	    grdright = n2 + p;
	    if (grdright>=mptsY) grdright -= mptsY;
	    efld[n1][n2][n3].y += fdc*
	      (potFld[n1][grdleft][n3] - potFld[n1][grdright][n3])*
	      halfGrdSpY_1*p_1;

	    grdleft = n3 - p;
	    if (grdleft<0) grdleft += mptsZ;
	    grdright = n3 + p;
	    if (grdright>=mptsZ) grdright -= mptsZ;
	    efld[n1][n2][n3].z += fdc*
	      (potFld[n1][n2][grdleft] - potFld[n1][n2][grdright])*
	      halfGrdSpZ_1*p_1;
	  }
	}
    break;
  }
}

#define MODULO_MESH(x, mpts) {           \
   if ((x)<0) (x)+=(mpts);               \
   else                                  \
     if ((x)>=(mpts)) (x)-=(mpts);         \
}

void p3m_gradient_field(p3m_t        *that,
			int          numFldPts,
			cartVec_t    *fldPos,
			double       *potFld,
			cartVec_t    *eFld)
{
  int mptsX=that->meshPtsX, mptsY=that->meshPtsY, mptsZ=that->meshPtsZ;
  double grdSpX = that->gridSpacingX, grdSpX_1 = that->grdSpX_1,
    grdSpY = that->gridSpacingY, grdSpY_1 = that->grdSpY_1,
    grdSpZ = that->gridSpacingZ, grdSpZ_1 = that->grdSpZ_1;
  cartVec_t cm = that->meshCenter,
    mExt,  /* the spatial extent of the mesh */
    llc;   /* the coordinates of the first grid point, the lower left corner */
  double ***mshFld = that->potFld;
  cartVec_t ***mshE = that->efld;
  int refgd1, refgd2, refgd3;
  int p1, p2, p3, grdi1, grdi2, grdi3, grdix, grdiy, grdiz;
  int pleft, pright;
  cartVec_t rpos;   /* relative dimensionless position: (r - r0)/H */
  cartVec_t fldPt;
  cartVec_t rd;
  double shift;
  double wx[P3M_MAXASSIGN], wy[P3M_MAXASSIGN], wz[P3M_MAXASSIGN], wgt,
    wcume, wcumexy, wcumeyz, wcumezx;
  double dwx[P3M_MAXASSIGN], dwy[P3M_MAXASSIGN], dwz[P3M_MAXASSIGN];
  double *wgtx, *wgty, *wgtz, wxtmp, wytmp, wztmp;
  p3mCache_t cache = that->cache;
  
  int xn;  

  mExt.x = mptsX*grdSpX;    
  mExt.y = mptsY*grdSpY;
  mExt.z = mptsZ*grdSpZ;
  
  llc.x = cm.x - 0.5*mExt.x + 0.5*grdSpX;
  llc.y = cm.y - 0.5*mExt.y + 0.5*grdSpY;
  llc.z = cm.z - 0.5*mExt.z + 0.5*grdSpZ;

  /* in case of odd-order assignment, find the nearest grid point to
     the charge; otherwise find the left boundary of the mesh cell
     that contains the charge */
  if ((int)(that->assgn) % 2) {
    pright = ((int)(that->assgn))/2;
    pleft = - pright;
    shift = 0.5;
  }
  else {
    pright = ((int)(that->assgn))/2;
    pleft = -(pright-1);
    shift = 0;
  }

  assert(!cache.dirty);

  /* p3m_cache_dump(that); */
  
  /* use the cached charge assignment weights to interpolate the field */
  if (!(cache.dirty)) {
    for (xn=0; xn<cache.numChg; xn++) {
      potFld[xn] = 0.0;
      eFld[xn].x = eFld[xn].y = eFld[xn].z = 0.0;
      wgtx = cache.asgnwgt[xn].wgtx;
      wgty = cache.asgnwgt[xn].wgty;
      wgtz = cache.asgnwgt[xn].wgtz;

      rpos.x = (fldPos[xn].x - llc.x)*grdSpX_1;
      rpos.y = (fldPos[xn].y - llc.y)*grdSpY_1;
      rpos.z = (fldPos[xn].z - llc.z)*grdSpZ_1;
      refgd1 = (int)(floor(rpos.x+shift));
      refgd2 = (int)(floor(rpos.y+shift));
      refgd3 = (int)(floor(rpos.z+shift));
     
      rd.x = rpos.x - refgd1 + (shift-0.5);
      rd.y = rpos.y - refgd2 + (shift-0.5);
      rd.z = rpos.z - refgd3 + (shift-0.5);

      refgd1 %= mptsX;
      refgd2 %= mptsY;
      refgd3 %= mptsZ;
      if (refgd1<0) refgd1+=mptsX;
      if (refgd2<0) refgd2+=mptsY;
      if (refgd3<0) refgd3+=mptsZ;
    
      p3m_assign_weight_derivative(&rd, that, dwx, dwy, dwz);


      for (p1=pleft; p1<=pright; p1++) {
	grdi1 = refgd1 + p1;
	MODULO_MESH(grdi1, mptsX);

	/* these are used to compute the gradient of the potential,
           they are divided by the grid spacing so to keep as much
           calculation in the outermost loop */
	wxtmp = wgtx[p1-pleft];

	for (p2=pleft; p2<=pright; p2++) {
	  grdi2 = refgd2 + p2;
	  MODULO_MESH(grdi2, mptsY);

	  wcume = wgtx[p1-pleft]*wgty[p2-pleft];

	  wytmp = wgty[p2-pleft];
	  
	  wcumexy = wxtmp*wgty[p2-pleft];
	  
	  for (p3=pleft; p3<=pright; p3++) {
	    grdi3 = refgd3 + p3;
	    MODULO_MESH(grdi3, mptsZ);

	    wztmp = wgtz[p3-pleft];
	    
	    wgt = wcume*wgtz[p3-pleft];

	    wcumeyz = wytmp*wgtz[p3-pleft];
	    wcumezx = wgtz[p3-pleft]*wxtmp;

	    potFld[xn] += mshFld[grdi1][grdi2][grdi3]*wgt;
	    eFld[xn].x += mshFld[grdi1][grdi2][grdi3]*wcumeyz*dwx[p1-pleft];
	    eFld[xn].y += mshFld[grdi1][grdi2][grdi3]*wcumezx*dwy[p2-pleft];
	    eFld[xn].z += mshFld[grdi1][grdi2][grdi3]*wcumexy*dwz[p3-pleft];
	  }
	}
      }
    }
  }
}

void p3m_interpolate_field(p3m_t       *that,
			   int         numFldPts,
			   cartVec_t   *fldPos,
			   double      *potFld,
			   cartVec_t   *eFld)
{
  int mptsX=that->meshPtsX, mptsY=that->meshPtsY, mptsZ=that->meshPtsZ;
  double grdSpX = that->gridSpacingX, grdSpX_1 = that->grdSpX_1,
    grdSpY = that->gridSpacingY, grdSpY_1 = that->grdSpY_1,
    grdSpZ = that->gridSpacingZ, grdSpZ_1 = that->grdSpZ_1;
  cartVec_t cm = that->meshCenter,
    mExt,  /* the spatial extent of the mesh */
    llc;   /* the coordinates of the first grid point, the lower left corner */
  double ***mshFld = that->potFld;
  cartVec_t ***mshE = that->efld;
  int refgd1, refgd2, refgd3;
  int p1, p2, p3, grdi1, grdi2, grdi3;
  int pleft, pright;
  cartVec_t rpos;   /* relative dimensionless position: (r - r0)/H */
  cartVec_t fldPt;
  cartVec_t rd;
  double shift;
  double wx[P3M_MAXASSIGN], wy[P3M_MAXASSIGN], wz[P3M_MAXASSIGN], wgt, wcume;
  double *wgtx, *wgty, *wgtz, wxtmp, wytmp, wztmp;
  p3mCache_t cache = that->cache;
  
  int xn;
  
  mExt.x = mptsX*grdSpX;    
  mExt.y = mptsY*grdSpY;
  mExt.z = mptsZ*grdSpZ;
  
  llc.x = cm.x - 0.5*mExt.x + 0.5*grdSpX;
  llc.y = cm.y - 0.5*mExt.y + 0.5*grdSpY;
  llc.z = cm.z - 0.5*mExt.z + 0.5*grdSpZ;

  /* in case of odd-order assignment, find the nearest grid point to
     the charge; otherwise find the left boundary of the mesh cell
     that contains the charge */
  if ((int)(that->assgn) % 2) {
    pright = ((int)(that->assgn))/2;
    pleft = - pright;
    shift = 0.5;
  }
  else {
    pright = ((int)(that->assgn))/2;
    pleft = -(pright-1);
    shift = 0;
  }

  /* use the cached charge assignment weights to interpolate the field */
  if (!(cache.dirty)) {
    for (xn=0; xn<cache.numChg; xn++) {
      potFld[xn] = 0.0;
      eFld[xn].x = eFld[xn].y = eFld[xn].z = 0.0;
      refgd1 = cache.asgnwgt[xn].refgd1;
      wgtx = cache.asgnwgt[xn].wgtx;
      refgd2 = cache.asgnwgt[xn].refgd2;
      wgty = cache.asgnwgt[xn].wgty;
      refgd3 = cache.asgnwgt[xn].refgd3;
      wgtz = cache.asgnwgt[xn].wgtz;

      
      for (p1=pleft; p1<=pright; p1++) {
	grdi1 = refgd1 + p1;
	MODULO_MESH(grdi1, mptsX);
	wxtmp = wgtx[p1-pleft];
	if (wxtmp==0) continue;
	
	for (p2=pleft; p2<=pright; p2++) {
	  grdi2 = refgd2 + p2;
	  MODULO_MESH(grdi2, mptsY);
	  wytmp = wgty[p2-pleft];
	  if (wytmp==0) continue;
	  wcume = wxtmp*wytmp;
	  
	  for (p3=pleft; p3<=pright; p3++) {
	    grdi3 = refgd3 + p3;
	    MODULO_MESH(grdi3, mptsZ);
	    wztmp = wgtz[p3-pleft];
	    if (wztmp==0) continue;
	    wgt = wcume*wztmp;
	    
	    potFld[xn] += mshFld[grdi1][grdi2][grdi3]*wgt;
	    eFld[xn].x += mshE[grdi1][grdi2][grdi3].x*wgt;
	    eFld[xn].y += mshE[grdi1][grdi2][grdi3].y*wgt;
	    eFld[xn].z += mshE[grdi1][grdi2][grdi3].z*wgt;
	  }
	}
      }
    }
  }

  for (xn=cache.numChg; /* This might be a dangerous hack, change later. If we
			   have used the cache to compute the field at
			   the source points, the rest of the field is
			   to be continued at the index
			   cache.numChg. remember that when field is
			   not computed at the source cache.numChg =
			   0. but someone will screw that up!  */
       xn<numFldPts; xn++) {
    
    potFld[xn]=0.0;
    eFld[xn].x = eFld[xn].y = eFld[xn].z = 0;

    /* of course, the indexing of the other field positions must be
       offset by the number of source points where the field is also
       computed. remember that when no source field is computed,
       cache.numChg=0 */
    fldPt = fldPos[xn-cache.numChg];
    
    rpos.x = (fldPt.x - llc.x)*grdSpX_1;
    rpos.y = (fldPt.y - llc.y)*grdSpY_1;
    rpos.z = (fldPt.z - llc.z)*grdSpZ_1;
    refgd1 = (int)(floor(rpos.x+shift));
    refgd2 = (int)(floor(rpos.y+shift));
    refgd3 = (int)(floor(rpos.z+shift));

    /* if even-order assignment, take the distance to the mid-point of
       the nearest grid, else take the distance to the nearest grid
       point */
    rd.x = rpos.x - refgd1 + (shift-0.5);
    rd.y = rpos.y - refgd2 + (shift-0.5);
    rd.z = rpos.z - refgd3 + (shift-0.5);

    p3m_assignment_weight(&rd, that, wx, wy, wz);
    
    refgd1 %= mptsX;
    refgd2 %= mptsY;
    refgd3 %= mptsZ;
    if (refgd1<0) refgd1+=mptsX;
    if (refgd2<0) refgd2+=mptsY;
    if (refgd3<0) refgd3+=mptsZ;

    for (p1=pleft; p1<=pright; p1++) {
      grdi1 = refgd1 + p1;
      MODULO_MESH(grdi1, mptsX);
      
      for (p2=pleft; p2<=pright+1; p2++) {
	grdi2 = refgd2 + p2;
	MODULO_MESH(grdi2, mptsY);
	wcume = wx[p1-pleft]*wy[p2-pleft];
	  
	for (p3=pleft; p3<=pright+1; p3++) {
	  grdi3 = refgd3 + p3;
	  MODULO_MESH(grdi3, mptsZ);
	  wgt = wcume*wz[p3-pleft];
	  potFld[xn] += mshFld[grdi1][grdi2][grdi3]*wgt;
	  eFld[xn].x += mshE[grdi1][grdi2][grdi3].x*wgt;
	  eFld[xn].y += mshE[grdi1][grdi2][grdi3].y*wgt;
	  eFld[xn].z += mshE[grdi1][grdi2][grdi3].z*wgt;
	}
      }
    }
  }
}

void p3m_assign_weight_derivative(cartVec_t     *rd,
				  p3m_t         *that,
				  double        *dwx,
				  double        *dwy,
				  double        *dwz)
{
  switch (that->assgn) {
  case NGP:
    /* we cannot use gradient differentiator for that assignment scheme */
    fprintf(stderr, "incompatible assignment scheme and differentiator:\ncannot use GRADIENT differentiator for NGP assignment scheme. to use GRADIENT\ndifferentiator, choose CIC or up assignment scheme; to use NGP assignment\nscheme, choose FINITE_nP differentiator.\n");
    exit(1);
    break;
    
  case CIC:
    dwx[0] = that->grdSpX_1;
    dwy[0] = that->grdSpY_1;
    dwz[0] = that->grdSpZ_1;
    dwx[1] = -that->grdSpX_1;
    dwy[1] = -that->grdSpY_1;
    dwz[1] = -that->grdSpZ_1;
    break;
    
  case TSC:
    dwx[0] = (0.5 - rd->x)*that->grdSpX_1;
    dwy[0] = (0.5 - rd->y)*that->grdSpY_1;
    dwz[0] = (0.5 - rd->z)*that->grdSpZ_1;

    dwx[1] = (2*rd->x)*that->grdSpX_1;
    dwy[1] = (2*rd->y)*that->grdSpY_1;
    dwz[1] = (2*rd->z)*that->grdSpZ_1;

    dwx[2] = -(0.5 + rd->x)*that->grdSpX_1;
    dwy[2] = -(0.5 + rd->y)*that->grdSpY_1;
    dwz[2] = -(0.5 + rd->z)*that->grdSpZ_1;

    break;

  case A4P:
    dwx[0] = (1.0/8.0 - 0.5*rd->x + 0.5*sqr(rd->x))*that->grdSpX_1;
    dwy[0] = (1.0/8.0 - 0.5*rd->y + 0.5*sqr(rd->y))*that->grdSpY_1;
    dwz[0] = (1.0/8.0 - 0.5*rd->z + 0.5*sqr(rd->z))*that->grdSpZ_1;

    dwx[1] = (5.0/8.0 + 0.5*rd->x - 1.5*sqr(rd->x))*that->grdSpX_1;
    dwy[1] = (5.0/8.0 + 0.5*rd->y - 1.5*sqr(rd->y))*that->grdSpY_1;
    dwz[1] = (5.0/8.0 + 0.5*rd->z - 1.5*sqr(rd->z))*that->grdSpZ_1;
    
    dwx[2] = (-5.0/8.0 + 0.5*rd->x + 1.5*sqr(rd->x))*that->grdSpX_1;
    dwy[2] = (-5.0/8.0 + 0.5*rd->y + 1.5*sqr(rd->y))*that->grdSpY_1;
    dwz[2] = (-5.0/8.0 + 0.5*rd->z + 1.5*sqr(rd->z))*that->grdSpZ_1;

    dwx[3] = (-1.0/8.0 - 0.5*rd->x - 0.5*sqr(rd->x))*that->grdSpX_1;
    dwy[3] = (-1.0/8.0 - 0.5*rd->y - 0.5*sqr(rd->y))*that->grdSpY_1;
    dwz[3] = (-1.0/8.0 - 0.5*rd->z - 0.5*sqr(rd->z))*that->grdSpZ_1;

    break;
  }
}






