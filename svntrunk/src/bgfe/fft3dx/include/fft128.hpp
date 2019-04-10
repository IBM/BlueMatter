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
 #ifndef INCLUDE_FFT128_HPP
#define INCLUDE_FFT128_HPP

#include <builtins.h>
#include <math.h>
//#include <BlueMatter/complex.hpp>
#include <MPI/complex.hpp>

#define COSHALFANGLE(cosx) (sqrt((1.0+cosx)*0.5))
#define SINHALFANGLE(sinx,coshx) (sinx/(2.0*coshx))

#define PREFETCH_TYPE
// #define PREFETCH_TYPE (const volatile void *)

class fft128_s {
   public:

   enum {
      k_l2points = 7 ,
      k_points = 1 << k_l2points
      } ;

   // Data which are transformed in place
//   fft3d_complex m_table[k_points] ;
   fft3d_complex *m_table ;
   fft3d_complex *m_table_prefetch ;

  // fft128_s(fft3d_complex* a_table) { m_table = a_table ; m_table_prefetch = a_table ; } ;
   fft128_s(fft3d_complex* a_table) { m_table = (fft3d_complex *) a_table ; m_table_prefetch = (fft3d_complex *) a_table ; } ;
   fft128_s(fft3d_complex* a_table, fft3d_complex* a_table_prefetch) { m_table = a_table ; m_table_prefetch = a_table_prefetch ; } ;

   // 'internal' functions

   // 2-element bit reverse, should be enough to keep pipelines full
   void br2(
     int s0, int t0,
     int s1, int t1
           ) {
//      const double scale = 1.0/sqrt(128.0) ;
      fft3d_complex a0 = m_table[s0] ;
      fft3d_complex b0 = m_table[t0] ;
      fft3d_complex a1 = m_table[s1] ;
      fft3d_complex b1 = m_table[t1] ;
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
//      const double scale = 1.0/sqrt(128.0) ;
      fft3d_complex a0 = m_table[s0] ;
      fft3d_complex b0 = m_table[t0] ;
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
//      const double scale = 1.0/sqrt(128.0) ;
      fft3d_complex a0 = m_table[s0] ;
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
   void bitreverse(fft3d_complex* first_touch, fft3d_complex* second_touch) {
      __prefetch_by_load( PREFETCH_TYPE first_touch ) ;
     // The palindromic indices need rescaling only
     bsc( 0x00 ) ;
     bsc( 0x08 ) ;
     bsc( 0x14 ) ;
     bsc( 0x1c ) ;
     bsc( 0x22 ) ;
     bsc( 0x2a ) ;
     bsc( 0x36 ) ;
     bsc( 0x3e ) ;

     bsc( 0x41 ) ;
     bsc( 0x49 ) ;
     bsc( 0x55 ) ;
     bsc( 0x5d ) ;
     bsc( 0x63 ) ;
     bsc( 0x6b ) ;
     bsc( 0x77 ) ;
     bsc( 0x7f ) ;

     // All the others need rescaling and swapping
     br1( 0x01,0x40 ) ;
     br1( 0x02,0x20 ) ;
     br1( 0x03,0x60 ) ;
     br1( 0x04,0x10 ) ;
     br1( 0x05,0x50 ) ;
     br1( 0x06,0x30 ) ;
     br1( 0x07,0x70 ) ;
     br1( 0x09,0x48 ) ;

     br1( 0x0a,0x28 ) ;
     br1( 0x0b,0x68 ) ;
     br1( 0x0c,0x18 ) ;
     br1( 0x0d,0x58 ) ;
     br1( 0x0e,0x38 ) ;
     br1( 0x0f,0x78 ) ;
     br1( 0x11,0x44 ) ;
     br1( 0x12,0x24 ) ;

     br1( 0x13,0x64 ) ;
     br1( 0x15,0x54 ) ;
     br1( 0x16,0x34 ) ;
     br1( 0x17,0x74 ) ;
     br1( 0x19,0x4c ) ;
     br1( 0x1a,0x2c ) ;
     br1( 0x1b,0x6c ) ;
     br1( 0x1d,0x5c ) ;
     br1( 0x1e,0x3c ) ;

     __prefetch_by_load( PREFETCH_TYPE second_touch ) ;

     br1( 0x1f,0x7c ) ;
     br1( 0x21,0x42 ) ;
     br1( 0x23,0x62 ) ;
     br1( 0x25,0x52 ) ;
     br1( 0x26,0x32 ) ;
     br1( 0x27,0x72 ) ;
     br1( 0x29,0x4a ) ;
     br1( 0x2b,0x6a ) ;

     br1( 0x2d,0x5a ) ;
     br1( 0x2e,0x3a ) ;
     br1( 0x2f,0x7a ) ;
     br1( 0x31,0x46 ) ;
     br1( 0x33,0x66 ) ;
     br1( 0x35,0x56 ) ;
     br1( 0x37,0x76 ) ;
     br1( 0x39,0x4e ) ;

     br1( 0x3b,0x6e ) ;
     br1( 0x3d,0x5e ) ;
     br1( 0x3f,0x7e ) ;
     br1( 0x43,0x61 ) ;
     br1( 0x45,0x51 ) ;
     br1( 0x47,0x71 ) ;
     br1( 0x4b,0x69 ) ;
     br1( 0x4d,0x59 ) ;

     br1( 0x4f,0x79 ) ;
     br1( 0x53,0x65 ) ;
     br1( 0x57,0x75 ) ;
     br1( 0x5b,0x6d ) ;
     br1( 0x5f,0x7d ) ;
     br1( 0x67,0x73 ) ;
     br1( 0x6f,0x7b ) ;

   } ;

   // Kernel of the FFT
   template <int mmax> void Danielson_Lanczos(int i, double wr, double wi)
   {
      int j=i+(mmax/2) ;

      fft3d_complex dj  = m_table[j] ;
      fft3d_complex di  = m_table[i] ;

      double tre = wr*dj.re - wi*dj.im ;
      double tim = wi*dj.re + wr*dj.im ;

      fft3d_complex djj ;
      fft3d_complex dii ;
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

      for (int x=0;x < 128/mmax; x+=1)
      {
         Danielson_Lanczos<mmax>(m+mmax*x,wr,wi) ;
      } /* endfor */
   } ;

   // Outer loop
   template <int mmax, int isign> void loop1(double sintheta, double sinhalftheta, fft3d_complex* first_touch, fft3d_complex* second_touch)
   {
      __prefetch_by_load( PREFETCH_TYPE first_touch ) ;
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

      __prefetch_by_load( PREFETCH_TYPE second_touch ) ;

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

       const double c2p_256 =  COSHALFANGLE(        c2p_128) ;
       const double s2p_256 =  SINHALFANGLE(s2p_128,c2p_256) ;

       // Here are the strided passes which make up the complete FFT
       loop1  <2,isign>(  s2p_2,  s2p_4, (m_table_prefetch+ 1*8) ,(m_table_prefetch+ 9*8)) ;
       loop1  <4,isign>(  s2p_4,  s2p_8, (m_table_prefetch+ 2*8) ,(m_table_prefetch+10*8)) ;
       loop1  <8,isign>(  s2p_8, s2p_16, (m_table_prefetch+ 3*8) ,(m_table_prefetch+11*8)) ;
       loop1 <16,isign>( s2p_16, s2p_32, (m_table_prefetch+ 4*8) ,(m_table_prefetch+12*8)) ;
       loop1 <32,isign>( s2p_32, s2p_64, (m_table_prefetch+ 5*8) ,(m_table_prefetch+13*8)) ;
       loop1 <64,isign>( s2p_64,s2p_128, (m_table_prefetch+ 6*8) ,(m_table_prefetch+14*8)) ;
       loop1<128,isign>(s2p_128,s2p_256, (m_table_prefetch+ 7*8) ,(m_table_prefetch+15*8)) ;
   } ;

   // Interface. Neither scales, so that after 'forward' and 'inverse' you need
   // to divide by the number of points (128) if you want to retrieve the data you
   // started with. This matches FFTW.

   // Note .. these are set up in the 'FFTW' convention, with '-1' exponent
   // for forward, and '1' exponent for inverse. 'Numerical Recipes' has them
   // the other way round. Look carefully at the definitions of FFT in 12.0.1
   // in NR, and FFTW_FORWARD/FFTW_BACKWARD for FFTW.

   void inline_forward(void) {
//   prefetch() ;
     bitreverse((m_table_prefetch+ 0*8),(m_table_prefetch+ 8*8)) ;
     DLSection<-1>() ;
   } ;
   void inline_inverse(void) {
//   prefetch() ;
     bitreverse((m_table_prefetch+ 0*8),(m_table_prefetch+ 8*8)) ;
     DLSection<1>() ;
   }
   ;

   void forward(void) ;
   void inverse(void) ;
   } __attribute__((aligned(32)));


   // Put this in if you want callable (not inline) versions of FFT
#if 0
void fft128_s::forward(void)
{
   inline_forward() ;
}

void fft128_s::inverse(void)
{
   inline_inverse() ;
}
#endif

#endif
