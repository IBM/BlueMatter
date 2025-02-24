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
 /*
 * (c) Copyright IBM Corp. 2006. CPL
 */ 
#if !defined(CHEBERF_BUILD_HPP)
#define CHEBERF_BUILD_HPP 1

#include <math.h>
#include <cheberf.hpp>

// We start with the analytic representation of the derivative of the error function

class DerivativeErrorFunction
{
   public:

   static XDOUBLE FunctionToFit(XDOUBLE x) {
      XDOUBLE result = (2.0/sqrtl(M_PI)) * expl(-x*x) ; // Evaluate the analytic function
      return result ;
      } ;

} ;

// Fit a Chebyshev polynomial to the function over a subrange. See for example Numerical Recipes
// This selects N=128 points to evaluate the function; evaluates the function at these points;
// and produces the Chebyshev coefficients
class ChebyshevFit: public DerivativeErrorFunction, public ErfEvaluator
{
   public :
   XDOUBLE m_bma, m_bpa ;
   void SetBounds(XDOUBLE a, XDOUBLE b)
   {
      m_bma = 0.5*(b-a) ;
      m_bpa = 0.5*(b+a) ;
   }
   XDOUBLE func_sub(XDOUBLE y)
   {
     return FunctionToFit(y*m_bma+m_bpa) ;
   }
   enum {
      k_Points = 128
      } ;
   XDOUBLE f[k_Points] ;
   XDOUBLE c[k_Points] ;

   void EvaluateN(void)
   {
      for (int k=0; k<k_Points; k+=1)
      {
         XDOUBLE y=cosl(M_PI*(k+0.5)/k_Points) ;
         f[k] = func_sub(y) ;
      } /* endfor */
   }

   void EvaluateC(void)
   {
      for (int j=0; j<k_Points; j+=1)
      {
         XDOUBLE sum=0.0 ;

         for (int k=0; k < k_Points; k+=1)
         {
            sum += f[k]*cosl(M_PI*j*(k+0.5L)/k_Points) ;
         } /* endfor */
         c[j] = (2.0L/k_Points)*sum ;
      } /* endfor */
   } 




// We look at 8 domains
// 0..1, 1..2, 2..3, ... , 7..8
// and produce 8 Chebyshev polynomials.
// The polynomial for 'erfc' is produced from the polynomial for 'derfc'
// by applying the integration transform (Numerical Recipes)
// Negative numbers are handled by symmetry. Numbers >= 8 are
// special-cased (erfc == 0)
// For a given parameter, we choose the appropriate domain,
// evaluate the Chebyshev polynomial, and fix up for symmetry and range

    CTable mTable ;
//	double DErfcTable[k_Slices][k_Terms] ;
//	double NErfTable[k_Slices][k_Terms] ;

	void Setup(XDOUBLE x_lo, XDOUBLE x_hi, int slice, XDOUBLE& f_at_x_lo, XDOUBLE& f_at_x_hi)
	{
	   SetBounds(x_lo, x_hi) ;
	   EvaluateN() ;
	   EvaluateC() ;
	   mTable.SliceTable[slice].c[k_Terms-1].pb = -c[0] ;
//	   DErfcTable[slice][k_Terms-1] = -afit.c[0] ;
	   mTable.SliceTable[slice].c[k_Terms-1].pa = 0.0 ;
//	   NErfTable[slice][k_Terms-1] = 0.0 ;
	
	   double dq=4.0 ;
	   for (int q=1; q<k_Terms; q+=1)
	   {
		  mTable.SliceTable[slice].c[(k_Terms-1)-q].pb  = -c[q] ;
//	      DErfcTable[slice][(k_Terms-1)-q] = -afit.c[q] ;
	      mTable.SliceTable[slice].c[(k_Terms-1)-q].pa =  -(c[q-1] - c[q+1])/dq  ;
//	      NErfTable[slice][(k_Terms-1)-q] = -(afit.c[q-1] - afit.c[q+1])/dq  ;
	      dq += 4.0 ;
	   } /* endfor */
	
		XDOUBLE dummy1 ;
		XDOUBLE dummy2 ;	
	   RawEvaluatePair(f_at_x_lo,dummy1,-1.0,mTable.SliceTable[slice]) ;
	   RawEvaluatePair(f_at_x_hi,dummy2,1.0,mTable.SliceTable[slice]) ;
	} 
	
	ChebyshevFit(void)
	{
	   XDOUBLE f0l, f0h ;
	   XDOUBLE f1l, f1h ;
	   XDOUBLE f2l, f2h ;
	   XDOUBLE f3l, f3h ;
	   XDOUBLE f4l, f4h ;
	   XDOUBLE f5l, f5h ;
	   XDOUBLE f6l, f6h ;
	   XDOUBLE f7l, f7h ;
	   Setup( 0.0, 1.0, 0, f0l, f0h ) ;
	   Setup( 1.0, 2.0, 1, f1l, f1h ) ;
	   Setup( 2.0, 3.0, 2, f2l, f2h ) ;
	   Setup( 3.0, 4.0, 3, f3l, f3h ) ;
	   Setup( 4.0, 5.0, 4, f4l, f4h ) ;
	   Setup( 5.0, 6.0, 5, f5l, f5h ) ;
	   Setup( 6.0, 7.0, 6, f6l, f6h ) ;
	   Setup( 7.0, 8.0, 7, f7l, f7h ) ;
	
	   // Set up the constants of integration to set up for f(0)=0
	   mTable.SliceTable[0].c[k_Terms-1].pa = 2.0*(   -f0l) ;
	   mTable.SliceTable[1].c[k_Terms-1].pa = 2.0*(f0h-f1l) + mTable.SliceTable[0].c[k_Terms-1].pa ;
	   mTable.SliceTable[2].c[k_Terms-1].pa = 2.0*(f1h-f2l) + mTable.SliceTable[1].c[k_Terms-1].pa ;
	   mTable.SliceTable[3].c[k_Terms-1].pa = 2.0*(f2h-f3l) + mTable.SliceTable[2].c[k_Terms-1].pa ;
	   mTable.SliceTable[4].c[k_Terms-1].pa = 2.0*(f3h-f4l) + mTable.SliceTable[3].c[k_Terms-1].pa ;
	   mTable.SliceTable[5].c[k_Terms-1].pa = 2.0*(f4h-f5l) + mTable.SliceTable[4].c[k_Terms-1].pa ;
	   mTable.SliceTable[6].c[k_Terms-1].pa = 2.0*(f5h-f6l) + mTable.SliceTable[5].c[k_Terms-1].pa ;
	   mTable.SliceTable[7].c[k_Terms-1].pa = 2.0*(f6h-f7l) + mTable.SliceTable[6].c[k_Terms-1].pa ;
//  NErfTable[1][k_Terms-1] = 2.0*(f0h-f1l) + NErfTable[0][k_Terms-1] ;
//  NErfTable[2][k_Terms-1] = 2.0*(f1h-f2l) + NErfTable[1][k_Terms-1] ;
//  NErfTable[3][k_Terms-1] = 2.0*(f2h-f3l) + NErfTable[2][k_Terms-1] ;
//  NErfTable[4][k_Terms-1] = 2.0*(f3h-f4l) + NErfTable[3][k_Terms-1] ;
//  NErfTable[5][k_Terms-1] = 2.0*(f4h-f5l) + NErfTable[4][k_Terms-1] ;
//  NErfTable[6][k_Terms-1] = 2.0*(f5h-f6l) + NErfTable[5][k_Terms-1] ;
//  NErfTable[7][k_Terms-1] = 2.0*(f6h-f7l) + NErfTable[6][k_Terms-1] ;
	
	
	}

} ;

#endif
