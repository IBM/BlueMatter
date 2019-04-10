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
#if !defined(INCLUDE_MYLIB_HPP)
#define INCLUDE_MYLIB_HPP

#include <math.h>

#define XDOUBLE double
static inline XDOUBLE fsel(XDOUBLE a, XDOUBLE b, XDOUBLE c)
{
	return ( a >= 0.0 ) ? b : c ;
} ;


class ChebyshevPairEvaluator
{
  public:
  enum {
     k_Terms = 16
     } ;
     
	class DoublePair
	{
		public:
		XDOUBLE pa ;
		XDOUBLE pb ;
	} ;
	
	class DoublePairArray
	{
		public:
		DoublePair c[k_Terms] ;
	} ;
	

  static void RawEvaluatePair(XDOUBLE& f, XDOUBLE& df, XDOUBLE x, const DoublePairArray& cp)
  {
     XDOUBLE dppa = 0.0 ;
     XDOUBLE dpa = 0.0 ;
     XDOUBLE dppb = 0.0 ; 
     XDOUBLE dpb = 0.0 ; 
     for (int j=0; j<k_Terms-1; j+=1)
     {
        XDOUBLE da=(2.0*x)*dpa - dppa + cp.c[j].pa ;
        XDOUBLE db=(2.0*x)*dpb - dppb + cp.c[j].pb ;
        dppa = dpa ;
        dppb = dpb ;
        dpa = da ;
        dpb = db ;
     } /* endfor */
     // Term 0 is a special case; POWER 'multiply-add' makes this same efficiency as rewriting the table
     XDOUBLE resulta = x*dpa - dppa + 0.5*(cp.c[k_Terms-1]).pa ;
     XDOUBLE resultb = x*dpb - dppb + 0.5*(cp.c[k_Terms-1]).pb ;
     f = resulta ; 
     df = resultb ; 
  }


} ;

class ErfEvaluator: public ChebyshevPairEvaluator
{
   public:
   enum {
      k_Slices = 8 ,
      } ;

	class CTable {
		public: 
		DoublePairArray SliceTable[k_Slices] ;
	} ;
	
	static CTable ChebyshevTable ;
	
	//static double NErfTable[k_Slices][k_Terms] ;
	//static double DErfcTable[k_Slices][k_Terms] ;
	
	static double dk1 ; // The compiler does not know this is constant, so should not 'optimise' away the rounding below
	static inline double NearestInteger(const double x)
	{
	   const double two10 = 1024.0 ;
	   const double two50 = two10 * two10 * two10 * two10 * two10 ;
	   const double two52 = two50 * 4.0 ;
	   const double two51 = two50 * 2.0 ;
	   const double offset = two52 + two51 ;
	
	   // Force add and subtract of appropriate constant to drop lose fractional part
	   // .. hide it from the compiler so the optimiser won't reassociate things ..
	   const double losebits = (dk1*x) + offset ;
	   const double result = (dk1*losebits) - offset ;
	   return result ;
	}
	
	static void Evaluate(const XDOUBLE x, XDOUBLE& f, XDOUBLE& df)
	{
	
	   XDOUBLE xam = fabs(x) - 0.5 ;
	
	   XDOUBLE xi = NearestInteger(xam) ;
	   XDOUBLE xf = xam-xi ;                 // -0.5 < x < 0.5
	   int ix = (int) xi ;
	   int ixmask = ix & 7 ;
	
	//   double * MyNErfTable = NErfTable[ixmask] ;
	//   double * MyDErfcTable = DErfcTable[ixmask] ;
	
		XDOUBLE r0 ;
		XDOUBLE dr0 ;
	   RawEvaluatePair(r0, dr0, 2.0*xf, ChebyshevTable.SliceTable[ixmask]) ; 
	//   double  r0 = RawEvaluate(2.0*xf, MyNErfTable) ;
	//   double dr0 = RawEvaluate(2.0*xf, MyDErfcTable) ;
	   XDOUBLE  r1 = fsel(xi-7.5,-1.0,  r0) ;
	   XDOUBLE dr1 = fsel(xi-7.5, 0.0, dr0) ;
	   XDOUBLE   m = fsel(x,1.0, -1.0) ;
	   XDOUBLE  r2 = 1.0+m*r1 ;
	
	   f = r2    ;
	   df = dr1 ;
	
	}
	

} ;



double ErfEvaluator::dk1 = 1.0 ;


#endif

















