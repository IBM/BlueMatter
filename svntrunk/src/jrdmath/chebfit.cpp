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

using namespace std ;
#include <math.h>
#include <iostream>
#include <iomanip>
// Express function-to-fit in a standard form
// 'fsel' is a built-in instruction on PPCGR and above, sometimes we want to force its use
#if defined(ARCH_HAS_FSEL)
#include <builtins.h>
#define fsel(a, x, y) __fsel((a),(x),(y))
#else
#define fsel(a, x, y) ( (a) >= 0.0 ? (x) : (y) )
#endif
class f_fit
{
   public:

   long double func(long double x) {
      long double result = (1.0/sqrtl(2.0*M_PI)) * expl(-0.5*x*x) ; // Evaluate the analytic function
      return result ;
      } ;

} ;

// Fit a Chebyshev polynomial to it
class chebfit: public f_fit
{
   public :
   long double m_bma, m_bpa ;
   void set_bounds(long double a, long double b)
   {
      m_bma = 0.5*(b-a) ;
      m_bpa = 0.5*(b+a) ;
   }
   long double func_sub(long double y)
   {
     return func(y*m_bma+m_bpa) ;
   }
   enum {
      k_points = 128
      } ;
   long double f[k_points] ;
   long double c[k_points] ;

   void evaluate_n(void)
   {
      for (int k=0; k<k_points; k+=1)
      {
         long double y=cosl(M_PI*(k+(long double)0.5)/k_points) ;
         f[k] = func_sub(y) ;
      } /* endfor */
   }

   void evaluate_c(void)
   {
      for (int j=0; j<k_points; j+=1)
      {
         long double sum=0.0 ;

         for (int k=0; k < k_points; k+=1)
         {
            sum += f[k]*cosl(M_PI*j*(k+(long double)0.5)/k_points) ;
         } /* endfor */
         c[j] = (2.0/k_points)*sum ;
      } /* endfor */
   }

   ostream& show_c(ostream& os)
   {
      for (int k=0; k<k_points; k+=1)
      {
         os << '\n' << setw(4) << k
            << ' '  << setprecision(30) << c[k]
            ;
      } /* endfor */
      return os ;
   }
} ;


/*
 * Storage mapping of an IEEE double-precision number, for access to parts of it as integers or bits
 * This is big-endian specific, for little-endian you have to swap m_hi and m_lo, then test it !
 * The intended use of this is in calculating exp(x)
 */
class DoubleMap
{
  public:
  class UIntPair
  {
     public:
    unsigned int m_hi ;
    unsigned int m_lo ;
  } ;
   union {
      double m_d ;
      UIntPair m_u ;
      } m_value ;
  DoubleMap(void) { } ;
  DoubleMap(double X) { m_value.m_d = X ; } ;
  DoubleMap(
    unsigned int Xsign ,   // 0 for positive, 1 for negative
    unsigned int Xexponent ,
    unsigned int Xsignificand_hi ,  // The 0x00100000 bit had better be set, to get the right answer
    unsigned int Xsignificand_lo
    ) {
       m_value.m_u.m_hi = ( ( Xsign << 31 )                & 0x80000000 )
                        | ( ( (Xexponent + 1023 )  << 20 ) & 0x7ff00000 )
                        | ( Xsignificand_hi                & 0x000fffff ) ;
       m_value.m_u.m_lo = Xsignificand_lo ;
   } ;
  double GetValue(void) const { return m_value.m_d ; } ;

  void SetValue(double X) { m_value.m_d = X ; } ;
  void SetValue(
    unsigned int Xsign ,   // 0 for positive, 1 for negative
    unsigned int Xexponent ,
    unsigned int Xsignificand_hi ,  // The 0x00100000 bit had better be set, to get the right answer
    unsigned int Xsignificand_lo
    ) {
       m_value.m_u.m_hi = ( ( Xsign << 31 )                & 0x80000000 )
                        | ( ( (Xexponent + 1023 )  << 20 ) & 0x7ff00000 )
                        | ( Xsignificand_hi                & 0x000fffff ) ;
       m_value.m_u.m_lo = Xsignificand_lo ;
   } ;

  unsigned int HiWord(void) const { return m_value.m_u.m_hi ; } ;
  unsigned int LoWord(void) const { return m_value.m_u.m_lo ; } ;

  unsigned int SignBit(void) const { return HiWord() & 0x80000000 ; } ;
  unsigned int ExponentBits(void) const { return HiWord() & 0x7ff00000 ; } ;
  unsigned int SignificandHiBits(void) const { return HiWord() & 0x000fffff ; } ;
  unsigned int SignificandLoBits(void) const { return LoWord() ; } ;

  void SetSignificandHiBits(unsigned int new_hi_bits) { m_value.m_u.m_hi = ( m_value.m_u.m_hi & 0xfff00000 )
                                                                            | ( new_hi_bits & 0x000fffff ) ; }
  int Exponent(void) const { return ( ExponentBits() >> 20 ) - 1023 ; } ;
  unsigned int SignificandHi(void) const { return SignificandHiBits() | 0x00100000 ; } ;
  unsigned int SignificandLo(void) const { return SignificandLoBits() ; } ;
  bool IsNegative(void) const { return  0 != SignBit() ; } ;
} ;

class chebapprox
{
   enum {
      k_order = 16 ,
      k_slices = 8
      } ;

   double k_table[k_slices][k_order][2] ;

   double k_slicewidth(void) const { return 1.0 ; } ;

   void evaluate_inline(double x, double& f, double& fi) const
   {
      double xs = x/k_slicewidth() ;
      double axs = fabs(xs) ;
      double axsm = axs - 0.5 ;
    const double tp10 = 1024.0 ;
    const double tp20 = tp10*tp10 ;
    const double tp40 = tp20*tp20 ;

     // Adding (2**44+2**43) aligns and rounds this so that
     // truncated bits (recovered by subtraction) can be fed to power series
    const double x1 = axsm + ( tp40 * ( 16.0 + 8.0 ) ) ;
    const DoubleMap m1(x1) ;  // The lower part of m1 should be an integer in 0, 1, 2, 3, ...
    const double xl2 =  x1 - ( tp40 * ( 16.0 + 8.0 ) ) ;  // xl2 should be in (-0.5, 0.5), the fractional part

    const double xc=2.0*xl2 ; // in (-1,1), ready for chebyshev evaluator
    const double twoxc = 2.0*xc ;

    const unsigned int sig_lo = m1.SignificandLoBits() ;
    const unsigned int slice_index = sig_lo & (k_slices-1) ;

    const double * mytable = &(k_table[slice_index][0][0]) ;
    double dmaf  = mytable[0]  ;
    double dmafi = mytable[1]  ;
    double dmbf  = twoxc*dmaf + mytable[2] ;
    double dmbfi = twoxc*dmaf + mytable[3] ;
    for (unsigned int tx=0; tx<4 ;tx+=1 )
    {
      double dmcf  = twoxc*dmbf  - dmaf  + mytable[3*tx+4] ;
      double dmcfi = twoxc*dmbfi - dmafi + mytable[3*tx+5] ;

             dmaf  = twoxc*dmcf  - dmbf  + mytable[3*tx+6] ;
             dmafi = twoxc*dmcfi - dmbfi + mytable[3*tx+7] ;

             dmbf  = twoxc*dmaf  - dmcf  + mytable[3*tx+8] ;
             dmbfi = twoxc*dmafi - dmcfi + mytable[3*tx+9] ;
    }
    double f0  = xc*dmbf  - dmaf  + mytable[3*4+4] ;
    double f0i = xc*dmbfi - dmafi + mytable[3*4+5] ;

    const double sign_in = fsel(xc,1.0,-1.0) ;
    f = 0.5+sign_in*f0 ;
    fi = f0i ;

   }

   void evaluate(double x, double& f, double& fi) const ;
} ;


void chebapprox::evaluate(double x, double& f, double& fi) const
{
   evaluate_inline(x,f,fi) ;
}

int main(int argc, const char ** argv)
{
   if (argc >= 3)
   {
      char* dummy1 ;
      char* dummy2 ;
      long double a=strtod(argv[1], &dummy1) ;
      long double b=strtod(argv[2], &dummy2) ;
      cout << " Domain " << a << ' ' << b ;
      chebfit cfit ;
      cfit.set_bounds(a,b) ;
      cfit.evaluate_n() ;
      cfit.evaluate_c() ;
      cfit.show_c(cout) ;
      cout << '\n' ;
   }
   else
   {
   } /* endif */
   return 0 ;
}
