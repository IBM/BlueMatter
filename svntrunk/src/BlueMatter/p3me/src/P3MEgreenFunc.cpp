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
 * History:         011002 RZ (JCP 115, 2348-2358,2001)
 *
 ***************************************************************************/

#include <pk/pkmath.h>
#include "P3MEimpl.hpp"

void p3m_green_function(p3m_t    *that)
{
  int mptsX=that->meshPtsX, mptsY=that->meshPtsY, mptsZ=that->meshPtsZ;
  double grdSpX=that->gridSpacingX, grdSpX_1 = 1.0/grdSpX,
    grdSpY=that->gridSpacingY, grdSpY_1=1.0/grdSpY,
    grdSpZ=that->gridSpacingZ, grdSpZ_1=1.0/grdSpZ;
  cartVec_t kunit;
  double invhcube =
    1.0/(grdSpX*grdSpY*grdSpZ), /* inverse mesh cell volume */   
    scale = 1.0/(mptsX*mptsY*mptsZ);
  int n1, n2, n3;
  int m1, m2, m3;
  int diffOrd, p;
  double fdc;     /* finite difference coefficient */
  double kxHx, kyHy, kzHz; /* k_i H_i */
  double sinK1, sinK2, sinK3, /* \sin(k_i H_i/2) */
    sinProd,      /* \PI_{i}\sin(k_i H/2) */
    kmProd,       /* \PI_{i}(k_i H/2+m_i \pi) */
    uProd,        /* (\PI_{i}\sin(k_i H/2)) /
		     (\PI_{i}(k_i H/2+m_i \pi))^{(p+1)} */
    kmSqr,        /* |k_m|^2 */
    tmpExp,       /* \exp(-|k_m|^2/(4\alpha^2)) */
    cmnFacUR,     /* \exp(-k_m^2/4/\alpha^2)/k_m^2/
		     (\PI_{i}(k_i H/2+m_i \pi))^{(p+1)} */
    RdotD,        /* \tilde{R}^{\ast}(k_m) \cdot \tilde{D}(k_m) */        
    tmpSum;       /* \sum_{m}\tilde{R}^{\ast}(k_m)\cdot\tilde{D}(k_m)
		     \tilde{U}(k_m)^2 */
  cartVec_t kvec, /* {2\pi/H n1/N1, 2\pi/H n2/N2, 2\pi/H n3/N3} */
    kmVec,        /* k_i H_i/2 + m k_{gi} */
    dft,          /* the Fourier transform of the differentiator */
    UmultR,       /* U(k_m) \tilde{R}^{\ast}(k_m) */
    tmpVecSum;    /* \sum_{m}\tilde{R}^{\ast} \tilde{U}(k_m)^2 */
  double cosHk1, cosHk2, cosHk3, cosProd;
  double sumUsqr, sumU2k2, dftSqr;
  double numerator, denominator;
  double kmProdCume, sinProdCume, kmSqrCume;
  double spfunc_p_cosHk1, spfunc_p_cosHk2, spfunc_p_cosHk3;
  
  kunit.x = 2*M_PI/mptsX;
  kunit.y = 2*M_PI/mptsY;
  kunit.z = 2*M_PI/mptsZ;
  
  for (n1=0; n1<mptsX; n1++) {
    kxHx = n1*kunit.x;
    sinK1 = sin(0.5*kxHx);
    cosHk1 = cos(kxHx);
    
    for (n2=0; n2<mptsY; n2++) {
      kyHy = n2*kunit.y;
      sinK2 = sin(0.5*kyHy);
      cosHk2 = cos(kyHy);

      for (n3=0; n3<mptsZ/2+1; n3++) {

	/* k={0,0,0} leads to singularity, which corresponds to
           different choice of boundary conditions. here we omit that
           term and assume the corresponding conducting boundary
           surface */
	if (n1==0 && n2==0 && n3==0) {
	  that->greenFunct[n1][n2][n3] = 0.0;
	  continue;
	}
	
	/* if use finite difference operator, when 2*n_i % M_i == 0,
             \FFT{D}(k) = {0,0,0}, the optimum \FFT{G} is
             undefined. We arbitrarily make it zero */
	///////////*	switch (that->diffOp) {
	case FINITE_2PT:
	case FINITE_4PT:
	case FINITE_6PT:
	case FINITE_8PT:
	case FINITE_10PT:
	case FINITE_12PT:
	  if ((2*n1==mptsX || n1==0) && (2*n2==mptsY || n2==0) &&
	      (2*n3==mptsZ || n3==0)) {
	    that->greenFunct[n1][n2][n3] = 0.0;
	    continue;
	  }
	  break;
	}
	////////////////*/
	
	kzHz = n3*kunit.z;
	sinK3 = sin(0.5*kzHz);
	cosHk3 = cos(kzHz);
	/*
	 * compute the numerator of G(k):
	 * $\sum_{m}
	 * [\tilde{R}^{\ast}(k_m)\cdot\tilde{D}(k_m) \tilde{U}(k_m)^2]$
	 * sum over m={m1, m2, m3} till it converges
	 *

	/* in case of finite difference operator, compute the
           components of its Fourier transform */
	switch (that->diffOp) {
	case FINITE_2PT:
	case FINITE_4PT:
	case FINITE_6PT:
	case FINITE_8PT:
	case FINITE_10PT:
	case FINITE_12PT:
	  
	  dft.x = dft.y = dft.z = 0.0;
	  /* use the hack that the integer value of FINITE_{2m}P is m-1 */
	  diffOrd = (int)(that->diffOp)+1;
	  for (p=1; p<=diffOrd; p++) {
	    fdc = p3mFiniteDiffCoeff[diffOrd-1][p-1];
	    dft.x += (fdc*sin(p*kxHx)/(p*grdSpX));
	    dft.y += (fdc*sin(p*kyHy)/(p*grdSpY));
	    dft.z += (fdc*sin(p*kzHz)/(p*grdSpZ));
	  }
	  dftSqr = sqr(dft.x) + sqr(dft.y) + sqr(dft.z);
	  tmpVecSum.x = tmpVecSum.y = tmpVecSum.z = 0;
	  break;
	case GRADIENT:
	case ANALYTICAL:
	  tmpSum = 0;
	}

	for (m1=-MVEC_MAX; m1<=MVEC_MAX; m1++) {
	  kmVec.x = 0.5*kxHx + m1*M_PI;
	  
	  for (m2=-MVEC_MAX; m2<=MVEC_MAX; m2++) {
	    kmVec.y = 0.5*kyHy + m2*M_PI;
	    kmSqrCume = sqr(kmVec.x)/sqr(grdSpX) + sqr(kmVec.y)/sqr(grdSpY);
	    
	    kmProdCume = 1.0;
	    sinProdCume = 1.0;
	    if (n1!=0 || m1!=0) {
	      kmProdCume = kmVec.x;
	      sinProdCume = sinK1;
	    }
	    if (n2!=0 || m2!=0) {
	      kmProdCume *= kmVec.y;
	      sinProdCume *= sinK2;
	    }
	    
	    for (m3=-MVEC_MAX; m3<=MVEC_MAX; m3++) {
	      kmVec.z = 0.5*kzHz + m3*M_PI; 
	      kmProd = kmProdCume;
	      sinProd = sinProdCume;
	      if (n3!=0 || m3!=0) {
		kmProd *= kmVec.z;
		sinProd *= sinK3;
	      }
	      uProd = sinProd/kmProd;
	      switch (that->assgn) {
	      case NGP: 
		uProd = sqr(uProd);
		break;
	      case CIC: 
		uProd = fourth(uProd);
		break;
	      case TSC: 
		uProd = sixth(uProd);
		break;
	      case A4P:
		uProd = sqr(fourth(uProd));
		break;
	      }
	      
	      /* compute $\tilde{R}^{\ast}(k_m) \cdot \tilde{D}(k_m)$ */
	      kmSqr = kmSqrCume + sqr(kmVec.z)/sqr(grdSpZ);
	      tmpExp = exp(-kmSqr/sqr(that->alpha));
	      
	      switch (that->diffOp) {
	      case GRADIENT:
	      case ANALYTICAL:
		tmpSum += tmpExp*uProd;
		break;
	      case FINITE_2PT:
	      case FINITE_4PT:
	      case FINITE_6PT:
	      case FINITE_8PT:
	      case FINITE_10PT:
	      case FINITE_12PT:
		cmnFacUR = 0.5*tmpExp/kmSqr*uProd;
		UmultR.x = cmnFacUR*kmVec.x*grdSpX_1;
		UmultR.y = cmnFacUR*kmVec.y*grdSpY_1;
		UmultR.z = cmnFacUR*kmVec.z*grdSpZ_1;
		tmpVecSum.x += UmultR.x;
		tmpVecSum.y += UmultR.y;
		tmpVecSum.z += UmultR.z;
		break;
	      }
	    }
	  }
	}
      
	switch (that->diffOp) {
	case GRADIENT:
	case ANALYTICAL:
	  numerator = 4.0*M_PI*tmpSum;
	  break;
	case FINITE_2PT:
	case FINITE_4PT:
	case FINITE_6PT:
	case FINITE_8PT:
	case FINITE_10PT:
	case FINITE_12PT:
	  numerator = 4.0*M_PI*
	    (tmpVecSum.x*dft.x + tmpVecSum.y*dft.y + tmpVecSum.z*dft.z);
	  break;
	}

	/*
	 * compute the denominator of G(k) analytically:
	 * $ (\sum_{m} \tilde{U}(k_m)^2 |\tilde{D}(k_m)|^2) \cdot
	 * \sum_{m} \tilde{U}(k_m)^2 $
	 *
	 */

	/**
	 Here we give the result for the infinite sum
	   $\sum_{n=-\infty}^{\infty} \frac{1}{(y+n \pi)^{2p}} =
	  \csc(y)^{2p} F_{p}(\cos(2y))$, where $F_{p}(\cos(2y))$ is a
	  $p-1$th order polynomial of $\cos(2y)$. we denote $x =
	  \cos(2y)$ and $SPFUNC_P(x) = F_{p}(\cos(2y))$ in the
	  following code.
	  \[
	    \sum_{n=-\infty}^{\infty} \frac{1}{(y + n \pi)^2} = \csc(y)^2
	  \]
	 
	  \[
	    \sum_{n=-\infty}^{\infty} \frac{1}{(y + n \pi)^4} = \csc(y)^4
	       \cdot (2/3 + 1/3 \cos(2y))
	  \]
	 
	  \[
	    \sum_{n=-\infty}^{\infty} \frac{1}{(y + n \pi)^6} = \csc(y)^6
	       \cdot (8/15 + 13/30 \cos(2y) + 1/30 \cos(2y)^2)
	  \]

	  \[
	    \sum_{n=-\infty}^{\infty} \frac{1}{(y + n \pi)^8} = \csc(y)^8
	      \cdot (136/315 + 33/70 \cos(2y) + 2/21 \cos(2y)^2 +
	                1/630 \cos(2y)^3)
	  \]
	**/    
	  
#define SPFUNC_1(x) 1
#define SPFUNC_2(x) (2.0/3.0 + 1.0/3.0*(x))
#define SPFUNC_3(x) (8.0/15.0 + 13.0/30.0*(x) + 1.0/30.0*sqr((x)))
#define SPFUNC_4(x) \
(136.0/315.0 + 33.0/70.0*(x) + 2.0/21.0*sqr((x)) + 1.0/630.0*cube((x)))
	    
	
	/** the Fourier transform of the assignment function is
	    \[
	      \tilde{U}^2(k_m) =
	      \frac{\sin^{2p}(k_i H_i/2)}{(k_i H_i/2 + m_i \pi)^{2p}}
	    \]
	    Summing over $m_i = -\infty, \infty$ gives
	    \[
	      \sum_{m} \tilde{U}^2(k_m) = F_{p}(\cos(k_i H_i))
	    \]
	    $F_{p}(x) is the $p-1$th order polynomial defined above
	**/
	
	switch (that->assgn) {
	case NGP:
	  sumUsqr = 1.0;
	  break;
	  
	case CIC:

	  spfunc_p_cosHk1 = SPFUNC_2(cosHk1);
	  spfunc_p_cosHk2 = SPFUNC_2(cosHk2);
	  spfunc_p_cosHk3 = SPFUNC_2(cosHk3);
	  sumUsqr = spfunc_p_cosHk1*spfunc_p_cosHk2*spfunc_p_cosHk3;

	  /*  sumUsqr = cube(1.0/3.0)*(2+cosHk1)*(2+cosHk2)*(2+cosHk3); */
	  
#if 0 /* that seems to be wrong */
#define CIC_SUMUSQR_COEFF1 (2.0/3.0)
#define CIC_SUMUSQR_COEFF2 (1.0/3.0)
	  
	  sumUsqr = (CIC_SUMUSQR_COEFF1 + CIC_SUMUSQR_COEFF2*cosHk1)*
	    (CIC_SUMUSQR_COEFF1 + CIC_SUMUSQR_COEFF2*cosHk2)*
	    (CIC_SUMUSQR_COEFF1 + CIC_SUMUSQR_COEFF2*cosHk3);
#undef CIC_SUMUSQR_COEFF1
#undef CIC_SUMUSQR_COEFF2
#endif
	  
	  break;
	  
	case TSC: 
	  spfunc_p_cosHk1 = SPFUNC_3(cosHk1);
	  spfunc_p_cosHk2 = SPFUNC_3(cosHk2);
	  spfunc_p_cosHk3 = SPFUNC_3(cosHk3);
  	  sumUsqr = spfunc_p_cosHk1*spfunc_p_cosHk2*spfunc_p_cosHk3; 
	  /*
  	  sumUsqr = cube(1.0/30.0)* 
  	    (16+13*cosHk1+sqr(cosHk1))* 
  	    (16+13*cosHk2+sqr(cosHk2))* 
  	    (16+13*cosHk3+sqr(cosHk3));
	  */
#if 0
	  
#define TSC_SUMUSQR_COEFF1 (8.0/15.0)
#define TSC_SUMUSQR_COEFF2 (13.0/30.0)
#define TSC_SUMUSQR_COEFF3 (1.0/30.0)

	  sumUsqr = (TSC_SUMUSQR_COEFF1 +
		     TSC_SUMUSQR_COEFF2*cosHk1 +
		     TSC_SUMUSQR_COEFF3*sqr(cosHk1)) *
	    (TSC_SUMUSQR_COEFF1 +
	     TSC_SUMUSQR_COEFF2*cosHk2 +
	     TSC_SUMUSQR_COEFF3*sqr(cosHk2)) *
	    (TSC_SUMUSQR_COEFF1 +
	     TSC_SUMUSQR_COEFF2*cosHk3 +
	     TSC_SUMUSQR_COEFF3*sqr(cosHk3));

#undef TSC_SUMUSQR_COEFF1
#undef TSC_SUMUSQR_COEFF2
#undef TSC_SUMUSQR_COEFF3

#endif
	  
	  break;

	case A4P:

	  spfunc_p_cosHk1 = SPFUNC_4(cosHk1);
	  spfunc_p_cosHk2 = SPFUNC_4(cosHk2);
	  spfunc_p_cosHk3 = SPFUNC_4(cosHk3);
	  sumUsqr = spfunc_p_cosHk1*spfunc_p_cosHk2*spfunc_p_cosHk3;
	  
/*  	  sumUsqr = */
/*  	    (136.0/315.0 + 33.0/70.0*CosHk1 + 2.0/21.0*sqr(CosHk1) + */
/*  	     1.0/630.0*cube(cosHk1)) * */
/*  	    (136.0/315.0 + 33.0/70.0*CosHk2 + 2.0/21.0*sqr(CosHk2) + */
/*  	     1.0/630.0*cube(cosHk2)) * */
/*  	    (136.0/315.0 + 33.0/70.0*CosHk3 + 2.0/21.0*sqr(CosHk3) + */
/*  	     1.0/630.0*cube(cosHk3)); */
	  
#define A4P_SUMUSQR_COEFF1 272.0/630.0
#define A4P_SUMUSQR_COEFF2 297.0/630.0
#define A4P_SUMUSQR_COEFF3 60.0/630.0
#define A4P_SUMUSQR_COEFF4 1.0/630.0

	  sumUsqr = (A4P_SUMUSQR_COEFF1 + A4P_SUMUSQR_COEFF2*cosHk1 +
		     A4P_SUMUSQR_COEFF3*sqr(cosHk1) +
		     A4P_SUMUSQR_COEFF4*cube(cosHk1)) *
	    (A4P_SUMUSQR_COEFF1 + A4P_SUMUSQR_COEFF2*cosHk2 +
	     A4P_SUMUSQR_COEFF3*sqr(cosHk2) +
	     A4P_SUMUSQR_COEFF4*cube(cosHk2)) *
	    (A4P_SUMUSQR_COEFF1 + A4P_SUMUSQR_COEFF2*cosHk3 +
	     A4P_SUMUSQR_COEFF3*sqr(cosHk3) +
	     A4P_SUMUSQR_COEFF4*cube(cosHk3));

#undef A4P_SUMUSQR_COEFF1
#undef A4P_SUMUSQR_COEFF2
#undef A4P_SUMUSQR_COEFF3
#undef A4P_SUMUSQR_COEFF4
	  
	  break;
	  
	}
	switch (that->diffOp) {
	case ANALYTICAL:
	case GRADIENT:

	  /** we need to compute
	      \[
	      \sum_{m} \tilde{U}^{2}(k_m) |\tilde{D}(k_m)|^2 =
	        \prod_{i=1}^{3} \sin^{2p}(k_i H_i/2) * (
	        \sum_{m}\frac{(k_1+m_1 k_{g1})^2}{(k_1 H_1/2 + m_1 \pi)^{2p}}
		        \frac{1}{(k_2 H_2/2 + m_2 \pi)^{2p}}
		        \frac{1}{(k_3 H_3/2 + m_3 \pi)^{2p}} + ......)
		= frac{2}{H_1^{2}} (1-\cos(k_1 H_1))
		  F_{p-1}(\cos(k_1 H_1))
		  F_{p}(\cos(k_2 H_2)) F_{p}(\cos(k_3 H_3)) + ......
	      \]
	      $F_{p}(x)$ is the $p-1$th order polynomial defined as above.
	  **/
	  
	  switch (that->assgn) {
	  case NGP:
	    /* the aliasing sum diverges. we cannot use these
               differentiators when using a one-point assignment
               scheme */
	    die ("cannot use ANALYTICAL or GRADIENT differentiator\
 with NGP assignment");
	    break;
	    
	  case CIC:

	    sumU2k2 =
	      2.0/sqr(grdSpX)*
	      (1-cosHk1)*SPFUNC_1(cosHk1)*spfunc_p_cosHk2*spfunc_p_cosHk3 +
	      2.0/sqr(grdSpY)*
	      (1-cosHk2)*SPFUNC_1(cosHk2)*spfunc_p_cosHk1*spfunc_p_cosHk3 +
	      2.0/sqr(grdSpZ)*
	      (1-cosHk3)*SPFUNC_1(cosHk3)*spfunc_p_cosHk1*spfunc_p_cosHk2;
/*  	    sumU2k2 = 2.0/9.0* */
/*  	      (1/sqr(grdSpX)*(1-cosHk1)*(2+cosHk2)*(2+cosHk3) + */
/*  	       1/sqr(grdSpY)*(1-cosHk2)*(2+cosHk1)*(2+cosHk3) + */
/*  	       1/sqr(grdSpZ)*(1-cosHk3)*(2+cosHk1)*(2+cosHk2)); */
#if 0
	    
#define CIC_SUMUSQRKSQR_COEFF1 2.0/3.0
#define CIC_SUMUSQRKSQR_COEFF2 -1.0/6.0
#define CIC_SUMUSQRKSQR_COEFF3 -1.0/6.0

	    sumU2k2 = 4.0*grdSpSqr_1*
	      (CIC_SUMUSQRKSQR_COEFF1 +
	       CIC_SUMUSQRKSQR_COEFF2*(cosHk1*cosHk2+cosHk2*cosHk3+
				       cosHk3*cosHk1) +
	       CIC_SUMUSQRKSQR_COEFF3*(cosHk1*cosHk2*cosHk3));

#undef CIC_SUMUSQRKSQR_COEFF1
#undef CIC_SUMUSQRKSQR_COEFF2
#undef CIC_SUMUSQRKSQR_COEFF3

#endif
	    
	    break;
	    
	  case TSC:
	    sumU2k2 =
	      2.0/sqr(grdSpX)*
	      (1-cosHk1)*SPFUNC_2(cosHk1)*spfunc_p_cosHk2*spfunc_p_cosHk3 +
	      2.0/sqr(grdSpY)*
	      (1-cosHk2)*SPFUNC_2(cosHk2)*spfunc_p_cosHk3*spfunc_p_cosHk1 +
	      2.0/sqr(grdSpZ)*
	      (1-cosHk3)*SPFUNC_2(cosHk3)*spfunc_p_cosHk2*spfunc_p_cosHk1;
	    /*	    
  	    sumU2k2 = 1.0/1350.0* 
  	      (1/sqr(grdSpX)*(2-cosHk1-sqr(cosHk1))* 
  	       (16+13*cosHk2+sqr(cosHk2))*(16+13*cosHk3+sqr(cosHk3)) + 
  	       1/sqr(grdSpY)*(2-cosHk2-sqr(cosHk2))* 
  	       (16+13*cosHk1+sqr(cosHk1))*(16+13*cosHk3+sqr(cosHk3)) + 
  	       1/sqr(grdSpZ)*(2-cosHk3-sqr(cosHk3))* 
  	       (16+13*cosHk1+sqr(cosHk1))*(16+13*cosHk2+sqr(cosHk2))); 
	    */
#if 0 /* that is the old code */
	    
#define TSC_SUMUSQRKSQR_COEFF0 21600.0 
#define TSC_SUMUSQRKSQR_COEFF1 (24576.0/TSC_SUMUSQRKSQR_COEFF0) 
#define TSC_SUMUSQRKSQR_COEFF2 (9216.0/TSC_SUMUSQRKSQR_COEFF0) 
#define TSC_SUMUSQRKSQR_COEFF3 (-3072.0/TSC_SUMUSQRKSQR_COEFF0) 
#define TSC_SUMUSQRKSQR_COEFF4 (-1248.0/TSC_SUMUSQRKSQR_COEFF0) 
#define TSC_SUMUSQRKSQR_COEFF5 (-3168.0/TSC_SUMUSQRKSQR_COEFF0) 
#define TSC_SUMUSQRKSQR_COEFF6 (-480.0/TSC_SUMUSQRKSQR_COEFF0) 
#define TSC_SUMUSQRKSQR_COEFF7 (-8112.0/TSC_SUMUSQRKSQR_COEFF0) 
#define TSC_SUMUSQRKSQR_COEFF8 (-3120.0/TSC_SUMUSQRKSQR_COEFF0) 
#define TSC_SUMUSQRKSQR_COEFF9 (-432.0/TSC_SUMUSQRKSQR_COEFF0) 
#define TSC_SUMUSQRKSQR_COEFF10 (-48.0/TSC_SUMUSQRKSQR_COEFF0) 
	       
	    cosProd = cosHk1*cosHk2*cosHk3; 
	    sumU2k2 = grdSpSqr_1 * 
	      (TSC_SUMUSQRKSQR_COEFF1 + 
	       TSC_SUMUSQRKSQR_COEFF2*(cosHk1 + cosHk2 + cosHk3) + 
	       TSC_SUMUSQRKSQR_COEFF3*(sqr(cosHk1)+sqr(cosHk2)+sqr(cosHk3)) + 
	       TSC_SUMUSQRKSQR_COEFF4*(cosHk1*cosHk2 + cosHk2*cosHk3 + 
				       cosHk1*cosHk3) + 
	       TSC_SUMUSQRKSQR_COEFF5*(sqr(cosHk1)*cosHk2+sqr(cosHk2)*cosHk1 + 
				       sqr(cosHk2)*cosHk3+sqr(cosHk3)*cosHk2 + 
				       sqr(cosHk3)*cosHk1+sqr(cosHk1)*cosHk3)+ 
	       TSC_SUMUSQRKSQR_COEFF6*(sqr(cosHk1*cosHk2)+sqr(cosHk2*cosHk3)+ 
				       sqr(cosHk3*cosHk1)) + 
	       TSC_SUMUSQRKSQR_COEFF7*cosProd + 
	       TSC_SUMUSQRKSQR_COEFF8*cosProd*(cosHk1 + cosHk2 + cosHk3) + 
	       TSC_SUMUSQRKSQR_COEFF9*cosProd*(cosHk1*cosHk2 + cosHk2*cosHk3 + 
					       cosHk3*cosHk1) + 
	       TSC_SUMUSQRKSQR_COEFF10*sqr(cosProd)); 
#undef TSC_SUMUSQRKSQR_COEFF0 
#undef TSC_SUMUSQRKSQR_COEFF1 
#undef TSC_SUMUSQRKSQR_COEFF2 
#undef TSC_SUMUSQRKSQR_COEFF3 
#undef TSC_SUMUSQRKSQR_COEFF4 
#undef TSC_SUMUSQRKSQR_COEFF5 
#undef TSC_SUMUSQRKSQR_COEFF6 
#undef TSC_SUMUSQRKSQR_COEFF7 
#undef TSC_SUMUSQRKSQR_COEFF8 
#undef TSC_SUMUSQRKSQR_COEFF9 
#undef TSC_SUMUSQRKSQR_COEFF10 

#endif /* end of old code */
	    
	    break;
	    
	  case A4P:
	    
	    sumU2k2 =
	      2.0/sqr(grdSpX)*
	      (1-cosHk1)*SPFUNC_3(cosHk1)*spfunc_p_cosHk2*spfunc_p_cosHk3 +
	      2.0/sqr(grdSpY)*
	      (1-cosHk2)*SPFUNC_3(cosHk2)*spfunc_p_cosHk3*spfunc_p_cosHk1 +
	      2.0/sqr(grdSpZ)*
	      (1-cosHk3)*SPFUNC_3(cosHk3)*spfunc_p_cosHk1*spfunc_p_cosHk2;
	      
	    /*  	    sumU2k2 = 1.0/15.0* */
	    /*  	      (1/sqr(grdSpX)* */
	    /*  	       (2-cosHk1+sqr(cosHk1))*(1-cosHk1)* */
	    /*  	       (136.0/315.0 + 33.0/70.0*cosHk2 +  */
#if 0
	    
#define A4P_SUMUSQRKSQR_COEFF1 73984.0/496125.0
#define A4P_SUMUSQRKSQR_COEFF2 49232.0/496125.0
#define A4P_SUMUSQRKSQR_COEFF3 -1088.0/70875.0
#define A4P_SUMUSQRKSQR_COEFF4 143.0/3675.0
#define A4P_SUMUSQRKSQR_COEFF5 -272.0/99225.0
#define A4P_SUMUSQRKSQR_COEFF6 -5092.0/165375.0
#define A4P_SUMUSQRKSQR_COEFF7 -3267.0/98000.0
#define A4P_SUMUSQRKSQR_COEFF8 -1601.0/496125.0
#define A4P_SUMUSQRKSQR_COEFF9 -464.0/33075.0
#define A4P_SUMUSQRKSQR_COEFF10 -1199.0/24500.0
#define A4P_SUMUSQRKSQR_COEFF11 -388.0/496125.0
#define A4P_SUMUSQRKSQR_COEFF12 -1111.0/294000.0
#define A4P_SUMUSQRKSQR_COEFF13 -29.0/1575.0
#define A4P_SUMUSQRKSQR_COEFF14 -11.0/496125.0
#define A4P_SUMUSQRKSQR_COEFF15 -599.0/661500.0
#define A4P_SUMUSQRKSQR_COEFF16 -4.0/735.0
#define A4P_SUMUSQRKSQR_COEFF17 -1.0/4725.0
#define A4P_SUMUSQRKSQR_COEFF18 -199.0/7938000.0
#define A4P_SUMUSQRKSQR_COEFF19 -11.0/1984500.0
#define A4P_SUMUSQRKSQR_COEFF20 -1.0/7938000.0
	    
	    sumU2k2 = 4.0*grdSpSqr_1*
	      (A4P_SUMUSQRKSQR_COEFF1 +
	       A4P_SUMUSQRKSQR_COEFF2*(cosHk1 + cosHk2 + cosHk3) +
	       A4P_SUMUSQRKSQR_COEFF3*(sqr(cosHk1)+sqr(cosHk2)+sqr(cosHk3)) +
	       A4P_SUMUSQRKSQR_COEFF4*(cosHk1*cosHk2+cosHk2*cosHk3+cosHk3*cosHk1) +
	       A4P_SUMUSQRKSQR_COEFF5*(cube(cosHk1)+cube(cosHk2)+cube(cosHk3)) +
	       A4P_SUMUSQRKSQR_COEFF6*(sqr(cosHk1)*cosHk2+sqr(cosHk2)*cosHk1
				       +sqr(cosHk2)*cosHk3+sqr(cosHk3)*cosHk2
				       +sqr(cosHk3)*cosHk1+sqr(cosHk1)*cosHk3)+
	       A4P_SUMUSQRKSQR_COEFF7*cosHk1*cosHk2*cosHk3 +
	       A4P_SUMUSQRKSQR_COEFF8*(cube(cosHk1)*cosHk2+cube(cosHk2)*cosHk1
				       +cube(cosHk2)*cosHk3
				       +cube(cosHk3)*cosHk2
				       +cube(cosHk3)*cosHk1
				       +cube(cosHk1)*cosHk3) +
	       A4P_SUMUSQRKSQR_COEFF9*(sqr(cosHk1*cosHk2)+sqr(cosHk2*cosHk3)+
				       sqr(cosHk3*cosHk1)) +
	       A4P_SUMUSQRKSQR_COEFF10*(sqr(cosHk1)*cosHk2*cosHk3+
					cosHk1*sqr(cosHk2)*cosHk3+
					cosHk1*cosHk2*sqr(cosHk3)) +
	       A4P_SUMUSQRKSQR_COEFF11*(cube(cosHk1)*sqr(cosHk2) +
					cube(cosHk2)*sqr(cosHk1) +
					cube(cosHk2)*sqr(cosHk3) +
					cube(cosHk3)*sqr(cosHk2) +
					cube(cosHk3)*sqr(cosHk1) +
					cube(cosHk1)*sqr(cosHk3)) +
	       A4P_SUMUSQRKSQR_COEFF12*(cube(cosHk1)*cosHk2*cosHk3 +
					cosHk1*cube(cosHk2)*cosHk3 +
					cosHk1*cosHk2*cube(cosHk3)) +
	       A4P_SUMUSQRKSQR_COEFF13*(sqr(cosHk1)*sqr(cosHk2)*cosHk3 +
					sqr(cosHk1)*cosHk2*sqr(cosHk3) +
					cosHk1*sqr(cosHk2)*sqr(cosHk3)) +
	       A4P_SUMUSQRKSQR_COEFF14*(cube(cosHk1*cosHk2) +
					cube(cosHk2*cosHk3) +
					cube(cosHk3*cosHk1)) +
	       A4P_SUMUSQRKSQR_COEFF15*(cube(cosHk1)*sqr(cosHk2)*cosHk3 +
					sqr(cosHk1)*cube(cosHk2)*cosHk3 +
					cube(cosHk1)*cosHk2*sqr(cosHk3) +
					sqr(cosHk1)*cosHk2*cube(cosHk3) +
					cosHk1*cube(cosHk2)*sqr(cosHk3) +
					cosHk1*sqr(cosHk2)*cube(cosHk3)) +
	       A4P_SUMUSQRKSQR_COEFF16*sqr(cosHk1*cosHk2*cosHk3) +
	       A4P_SUMUSQRKSQR_COEFF17*sqr(cosHk1*cosHk2*cosHk3)*(cosHk1+
								  cosHk2+
								  cosHk3) +
	       A4P_SUMUSQRKSQR_COEFF18*cosHk1*cosHk2*cosHk3*
	       (sqr(cosHk1*cosHk2)+sqr(cosHk2*cosHk3)+sqr(cosHk3*cosHk1)) +
	       A4P_SUMUSQRKSQR_COEFF19*sqr(cosHk1*cosHk2*cosHk3)*
	       (cosHk1*cosHk2 + cosHk2*cosHk3 + cosHk3*cosHk1) +
	       A4P_SUMUSQRKSQR_COEFF20*cube(cosHk1*cosHk2*cosHk3));

#undef A4P_SUMUSQRKSQR_COEFF1
#undef A4P_SUMUSQRKSQR_COEFF2
#undef A4P_SUMUSQRKSQR_COEFF3
#undef A4P_SUMUSQRKSQR_COEFF4
#undef A4P_SUMUSQRKSQR_COEFF5
#undef A4P_SUMUSQRKSQR_COEFF6
#undef A4P_SUMUSQRKSQR_COEFF7
#undef A4P_SUMUSQRKSQR_COEFF8
#undef A4P_SUMUSQRKSQR_COEFF9
#undef A4P_SUMUSQRKSQR_COEFF10
#undef A4P_SUMUSQRKSQR_COEFF11
#undef A4P_SUMUSQRKSQR_COEFF12
#undef A4P_SUMUSQRKSQR_COEFF13
#undef A4P_SUMUSQRKSQR_COEFF14
#undef A4P_SUMUSQRKSQR_COEFF15
#undef A4P_SUMUSQRKSQR_COEFF16
#undef A4P_SUMUSQRKSQR_COEFF17
#undef A4P_SUMUSQRKSQR_COEFF18
#undef A4P_SUMUSQRKSQR_COEFF19
#undef A4P_SUMUSQRKSQR_COEFF20

#endif
	    
	    break;
	       
	  }
	  denominator = sumU2k2*sumUsqr;
	  break;
	case FINITE_2PT:
	case FINITE_4PT:
	case FINITE_6PT:
	case FINITE_8PT:
	case FINITE_10PT:
	case FINITE_12PT:
	  denominator = dftSqr*sqr(sumUsqr);
	}


	/* We didn't divide by the volume of the mesh cell when
           we compute the charge density on the mesh grids, which
           otherwise could be costly since charge assignment is done
           every time step. we instead compensate for the missing
           factor by scaling the green function here, which is only
           computed once. we also need to divide the FFT of inverse
           FFT by the product of the dimensions
           (scale=1.0/(mptsX*mptsY*mptsZ) to get the original
           function. we do it here once and for all. */
	that->greenFunct[n1][n2][n3] = (numerator/denominator)*invhcube*scale;
      }
    }
  }

}	  



