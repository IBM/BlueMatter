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
 #ifndef INCLUDE_FFT64_HPP
#define INCLUDE_FFT64_HPP

#include <builtins.h>
#include <math.h>
#include <BlueMatter/complex.hpp>


#define COSHALFANGLE(cosx) (sqrt((1.0+cosx)*0.5))
#define SINHALFANGLE(sinx,coshx) (sinx/(2.0*coshx))

class fft64 {
   public:

   enum {
      k_l2points = 6 ,
      k_points = 1 << k_l2points
      } ;

   // Data which are transformed in place
   complex *m_table ;

   fft64(complex* a_table) { m_table = a_table ; } ;

   // 'internal' functions

   // 2-element bit reverse, should be enough to keep pipelines full
   void br2(
     int s0, int t0,
     int s1, int t1
           ) {
//      const double scale = 1.0/sqrt(64.0) ;
      complex a0 = m_table[s0] ;
      complex b0 = m_table[t0] ;
      complex a1 = m_table[s1] ;
      complex b1 = m_table[t1] ;
//      a0.re *= scale ;
//      a0.im *= scale ;
//      b0.re *= scale ;
//      b0.im *= scale ;
//      a1.re *= scale ;
//      a1.im *= scale ;
//      b1.re *= scale ;
//      b1.im *= scale ;
      m_table[t0] = a0 ;
      m_table[s0] = b0 ;
      m_table[t1] = a1 ;
      m_table[s1] = b1 ;

   } ;
   // 1-element bit reverse, good compiler keeps the pipelines full
   void br1(
     int s0, int t0
           ) {
//      const double scale = 1.0/sqrt(64.0) ;
      complex a0 = m_table[s0] ;
      complex b0 = m_table[t0] ;
//      a0.re *= scale ;
//      a0.im *= scale ;
//      b0.re *= scale ;
//      b0.im *= scale ;
      m_table[t0] = a0 ;
      m_table[s0] = b0 ;

   } ;
   void bsc(
     int s0
   ) {
//      const double scale = 1.0/sqrt(64.0) ;
      complex a0 = m_table[s0] ;
//      a0.re *= scale ;
//      a0.im *= scale ;
      m_table[s0] = a0 ;
   } ;

// void prefetch(void) const
// {
//    for (int x=0; x<k_points; x+=8)
//    {
//       __prefetch_by_load(m_table+x) ;
//    } /* endfor */
// } ;

   // This rearranges the array, swapping each element with the
   // one at its bit-reversed index. Note that elements with
   // palindromic indices are left untouched, and elements
   // are only swapped with higher-index ones to avoid
   // swapping things back whence they came.
   // This swap enables FFT to work as an iteration rather
   // than as a recursion.
   void bitreverse(void) {
     // The palindromic indices need rescaling only
     bsc( 0x00 ) ;
     bsc( 0x0c ) ;
     bsc( 0x12 ) ;
     bsc( 0x1e ) ;
     bsc( 0x21 ) ;
     bsc( 0x2d ) ;
     bsc( 0x33 ) ;
     bsc( 0x3f ) ;
     // All the others need rescaling and swapping
     br1( 0x01,0x20 ) ;
     br1( 0x02,0x10 ) ;
     br1( 0x03,0x30 ) ;
     br1( 0x04,0x08 ) ;
     br1( 0x05,0x28 ) ;
     br1( 0x06,0x18 ) ;
     br1( 0x07,0x38 ) ;
     br1( 0x09,0x24 ) ;
     br1( 0x0a,0x14 ) ;
     br1( 0x0b,0x34 ) ;
     br1( 0x0d,0x2c ) ;
     br1( 0x0e,0x1c ) ;
     br1( 0x0f,0x3c ) ;
     br1( 0x11,0x22 ) ;
     br1( 0x13,0x32 ) ;
     br1( 0x15,0x2a ) ;
     br1( 0x16,0x1a ) ;
     br1( 0x17,0x3a ) ;
     br1( 0x19,0x26 ) ;
     br1( 0x1b,0x36 ) ;
     br1( 0x1d,0x2e ) ;
     br1( 0x1f,0x3e ) ;
     br1( 0x23,0x31 ) ;
     br1( 0x25,0x29 ) ;
     br1( 0x27,0x39 ) ;
     br1( 0x2b,0x35 ) ;
     br1( 0x2f,0x3d ) ;
     br1( 0x37,0x3b ) ;
   } ;

   // Kernel of the FFT
   template <int mmax> void Danielson_Lanczos(int i, double wr, double wi)
   {
      int j=i+(mmax/2) ;

      complex dj  = m_table[j] ;
      complex di  = m_table[i] ;

      double tre = wr*dj.re - wi*dj.im ;
      double tim = wi*dj.re + wr*dj.im ;

      complex djj ;
      complex dii ;
      djj.re = di.re - tre ;
      djj.im = di.im - tim ;

      dii.re = di.re + tre ;
      dii.im = di.im + tim ;

      m_table[j] = djj ;
      m_table[i] = dii ;

   } ;

   // Inner loop
   template <int mmax> void loop2(int m,double wr, double wi)
   {

      for (int x=0;x < 64/mmax; x+=1)
      {
         Danielson_Lanczos<mmax>(m+mmax*x,wr,wi) ;
      } /* endfor */
   } ;

   // Outer loop
   template <int mmax, int isign> void loop1(double sintheta, double sinhalftheta)
   {
      double wpr = -2.0 * sinhalftheta * sinhalftheta ;
      double wpi = isign * sintheta ;
      double wr=1.0 ;
      double wi=0.0 ;
      int halfloop = mmax/4 ;
      int fullloop = mmax/2 ;

      for (int m0=0; m0<halfloop; m0+=1)
      {
        loop2<mmax>(m0, wr, wi) ;
        double wrp = wr*wpr - wi*wpi + wr ;
        double wip = wi*wpr + wr*wpi + wi ;
        wr = wrp ;
        wi = wip ;
      } /* endfor */

      for (int m1=halfloop; m1<fullloop; m1+=1)
      {
        loop2<mmax>(m1, wr, wi) ;
        double wrp = wr*wpr - wi*wpi + wr ;
        double wip = wi*wpr + wr*wpi + wi ;
        wr = wrp ;
        wi = wip ;
      } /* endfor */
   } ;

   // The Danielson-Lanczos section
   template <int isign> void DLSection(void)
   {
       const double c2p_1   = 1.0 ;
       const double s2p_1   = 0.0 ;

       const double c2p_2   = -1.0 ;
       const double s2p_2   =  0.0 ;

       const double c2p_4   =  0.0 ;
       const double s2p_4   =  1.0 ;

       // The square roots implied by these calls are done at compile time
       const double c2p_8   =  COSHALFANGLE(      c2p_4) ;
       const double s2p_8   =  SINHALFANGLE(s2p_4,c2p_8) ;

       const double c2p_16  =  COSHALFANGLE(      c2p_8) ;
       const double s2p_16  =  SINHALFANGLE(s2p_8,c2p_16) ;

       const double c2p_32  =  COSHALFANGLE(       c2p_16) ;
       const double s2p_32  =  SINHALFANGLE(s2p_16,c2p_32) ;

       const double c2p_64  =  COSHALFANGLE(       c2p_32) ;
       const double s2p_64  =  SINHALFANGLE(s2p_32,c2p_64) ;

       const double c2p_128 =  COSHALFANGLE(       c2p_64) ;
       const double s2p_128 =  SINHALFANGLE(s2p_64,c2p_128) ;

       // Here are the strided passes which make up the complete FFT
       loop1  <2,isign>(  s2p_2,  s2p_4) ;
       loop1  <4,isign>(  s2p_4,  s2p_8) ;
       loop1  <8,isign>(  s2p_8, s2p_16) ;
       loop1 <16,isign>( s2p_16, s2p_32) ;
       loop1 <32,isign>( s2p_32, s2p_64) ;
       loop1 <64,isign>( s2p_64,s2p_128) ;
   } ;

   // Interface. Neither scales, so that after 'forward' and 'inverse' you need
   // to divide by the number of points (64) if you want to retrieve the data you
   // started with. This matches FFTW.

   // Note .. these are set up in the 'FFTW' convention, with '-1' exponent
   // for forward, and '1' exponent for inverse. 'Numerical Recipes' has them
   // the other way round. Look carefully at the definitions of FFT in 12.0.1
   // in NR, and FFTW_FORWARD/FFTW_BACKWARD for FFTW.

   void inline_forward(void) {
//   prefetch() ;
     bitreverse() ;
     DLSection<-1>() ;
   } ;
   void inline_inverse(void) {
//   prefetch() ;
     bitreverse() ;
     DLSection<1>() ;
   }
   ;

   void forward(void) ;
   void inverse(void) ;
   } ;


   // Put this in if you want callable (not inline) versions of FFT
#if 0
void fft64::forward(void)
{
   inline_forward() ;
}

void fft64::inverse(void)
{
   inline_inverse() ;
}
#endif

#endif
