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
 #ifndef INCLUDE_FFT128a_HPP
#define INCLUDE_FFT128a_HPP

#include <math.h>
#include <MPI/complex.hpp>

extern "builtin" void __compute_parallel(double, double, ...) ;

class fft128_recurrence_tables
{
   public:
   fft3d_complex forward_table_128[64] ;
   fft3d_complex inverse_table_128[64] ;
   fft3d_complex forward_table_64[32] ;
   fft3d_complex inverse_table_64[32] ;
   fft3d_complex forward_table_32[16] ;
   fft3d_complex inverse_table_32[16] ;
   fft3d_complex forward_table_16[8] ;
   fft3d_complex inverse_table_16[8] ;
   fft3d_complex forward_table_8[4] ;
   fft3d_complex inverse_table_8[4] ;
   fft3d_complex forward_table_4[2] ;
   fft3d_complex inverse_table_4[2] ;
   fft3d_complex forward_table_2[1] ;
   fft3d_complex inverse_table_2[1] ;

   fft128_recurrence_tables(void) ;
}
__attribute__((aligned(32)))
;

class fft128 {
   public:

   enum {
      k_l2points = 7 ,
      k_points = 1 << k_l2points
      } ;
  
        // The transform is run with a local table which is aligned for Double Hummer

    fftcomplexpair m_table[k_points] ;

    // This matches a PPC hardware instruction, for the case where 'mask' is a contiguous set of bits
    template <unsigned int shift, unsigned int mask> unsigned int rlwimi(unsigned int source, unsigned int insertion)
    {
       return ((insertion << shift) & mask ) | (source & ~mask) ;
    } ;
    template <unsigned int shift, unsigned int mask> unsigned int rrwimi(unsigned int source, unsigned int insertion)
    {
       return ((insertion >> shift) & mask ) | (source & ~mask) ;
    } ;

    unsigned int bitreverse7_5(unsigned int n)
    {
       unsigned int n0 = (n >> 6) ;
       unsigned int n1 = rrwimi< 4,0x02>(n0,n) ;
       unsigned int n2 = rrwimi< 2,0x04>(n1,n) ;
       unsigned int n3 = rlwimi< 0,0x08>(n2,n) ;
       unsigned int n4 = rlwimi< 2,0x10>(n3,n) ;
       return n4 ;
    }

    void copyin_pair( unsigned int tgtx, unsigned int srcx, const fft3d_complex * src_a, const fft3d_complex * src_b
    ) {
      double        re_a = src_a[srcx].re ;
      double        re_b = src_b[srcx].re ;
      double        im_a = src_a[srcx].im ;
      double        im_b = src_b[srcx].im ;
      m_table[tgtx].re_a = re_a ;
      m_table[tgtx].re_b = re_b ;
      m_table[tgtx].im_a = im_a ;
      m_table[tgtx].im_b = im_b ;
    }

    void copyin(const fft3d_complex * source_a, const fft3d_complex * source_b)
    {
       for (unsigned int q=0; q<128; q+=4)
       {
          unsigned int brq = bitreverse7_5(q) ;
          copyin_pair(brq+0x00,q+0,source_a,source_b) ;
          copyin_pair(brq+0x40,q+1,source_a,source_b) ;
          copyin_pair(brq+0x20,q+2,source_a,source_b) ;
          copyin_pair(brq+0x60,q+3,source_a,source_b) ;
       } /* endfor */
    }


   void copyout(fft3d_complex * target_a, fft3d_complex * target_b)
   {
          for (int x=0;x<k_points;x+=2)
          {
                fftcomplexpair mt_0 = m_table[x]  ;
                fftcomplexpair mt_1 = m_table[x+1] ;
                target_a[x].re = mt_0.re_a     ;
                target_b[x].re = mt_0.re_b     ;
                target_a[x].im = mt_0.im_a     ;
                target_b[x].im = mt_0.im_b     ;
                target_a[x+1].re = mt_1.re_a     ;
                target_b[x+1].re = mt_1.re_b     ;
                target_a[x+1].im = mt_1.im_a     ;
                target_b[x+1].im = mt_1.im_b     ;
          }
   }



   // Kernel of the FFT
   template <int mmax> void Danielson_Lanczos(int i, double wr, double wi)
   {
      int j=i+(mmax/2) ;

      fftcomplexpair dj  = m_table[j] ;
      fftcomplexpair di  = m_table[i] ;

      double tre_a = wr*dj.re_a - wi*dj.im_a ;
      double tre_b = wr*dj.re_b - wi*dj.im_b ;
      double tim_a = wi*dj.re_a + wr*dj.im_a ;
      double tim_b = wi*dj.re_b + wr*dj.im_b ;

      fftcomplexpair djj ;
      fftcomplexpair dii ;
      djj.re_a = di.re_a - tre_a ;
      djj.re_b = di.re_b - tre_b ;
      djj.im_a = di.im_a - tim_a ;
      djj.im_b = di.im_b - tim_b ;

      dii.re_a = di.re_a + tre_a ;
      dii.re_b = di.re_b + tre_b ;
      dii.im_a = di.im_a + tim_a ;
      dii.im_b = di.im_b + tim_b ;

      m_table[j] = djj ;
      m_table[i] = dii ;

   } ;

   // Inner loop
   template <int mmax> void loop2(int m,double wr, double wi)
   {

      if (128 == mmax )
      {
        // Not really a loop at all
        Danielson_Lanczos<mmax>(m,wr,wi) ;
      }
      else if ( 64 == mmax )
      {
         // Fully hand-unroll the loop
         Danielson_Lanczos<mmax>(m,wr,wi) ;
         Danielson_Lanczos<mmax>(m+mmax,wr,wi) ;
      }
      else
      {
        // Hand-unroll x2
        for (int x=0;x < 128/mmax; x+=2)
        {
           Danielson_Lanczos<mmax>(m+mmax*x,wr,wi) ;
           Danielson_Lanczos<mmax>(m+mmax*(x+1),wr,wi) ;
        } /* endfor */
      } /* endif */
   } ;

   // Outer loop
   template <int mmax, int isign> void loop1(double sintheta, double sinhalftheta, const fft3d_complex * recurrence_table)
   {
      double wr=1.0 ;
      double wi=0.0 ;
      if (2 == mmax)
      {
        loop2<mmax>(0, wr, wi) ;
      }
      else
      {
        double wpr = -2.0 * sinhalftheta * sinhalftheta ;
        double wpi = isign * sintheta ;
        int fullloop = mmax/2 ;

        for (int m=0; m<fullloop; m+=1)
        {
//        loop2<mmax>(m, wr, wi) ;
          loop2<mmax>(m, recurrence_table[m].re, recurrence_table[m].im) ;
//        double wrp = wr*wpr - wi*wpi + wr ;
//        double wip = wi*wpr + wr*wpi + wi ;
//        wr = wrp ;
//        wi = wip ;
        } /* endfor */
      } /* endif */

   } ;

   template <int mmax, int isign> static void init_recurrence(double sintheta, double sinhalftheta, fft3d_complex * recurrence_table)
   {
      {
        double wr=1.0 ;
        double wi=0.0 ;
        double wpr = -2.0 * sinhalftheta * sinhalftheta ;
        double wpi = isign * sintheta ;
        int fullloop = mmax/2 ;

        for (int m=0; m<fullloop; m+=1)
        {
          recurrence_table[m].re = wr ;
          double wrp = wr*wpr - wi*wpi + wr ;
          double wip = wi*wpr + wr*wpi + wi ;
          wr = wrp ;
          wi = wip ;
        } /* endfor */
      }
      {
        double wr=1.0 ;
        double wi=0.0 ;
        double wpr = -2.0 * sinhalftheta * sinhalftheta ;
        double wpi = isign * sintheta ;
        int fullloop = mmax/2 ;

        for (int m=0; m<fullloop; m+=1)
        {
          recurrence_table[m].im = wi ;
          double wrp = wr*wpr - wi*wpi + wr ;
          double wip = wi*wpr + wr*wpi + wi ;
          wr = wrp ;
          wi = wip ;
        } /* endfor */
      }
   }

   static fft128_recurrence_tables fft128_recurrence_table ;

   static void init_all_recurrence(void)
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

       init_recurrence  <2,-1>(  s2p_2,  s2p_4, fft128_recurrence_table.forward_table_2) ;
       init_recurrence  <4,-1>(  s2p_4,  s2p_8, fft128_recurrence_table.forward_table_4) ;
       init_recurrence  <8,-1>(  s2p_8, s2p_16, fft128_recurrence_table.forward_table_8) ;
       init_recurrence <16,-1>( s2p_16, s2p_32, fft128_recurrence_table.forward_table_16) ;
       init_recurrence <32,-1>( s2p_32, s2p_64, fft128_recurrence_table.forward_table_32) ;
       init_recurrence <64,-1>( s2p_64,s2p_128, fft128_recurrence_table.forward_table_64) ;
       init_recurrence<128,-1>(s2p_128,s2p_256, fft128_recurrence_table.forward_table_128) ;

       init_recurrence  <2,+1>(  s2p_2,  s2p_4, fft128_recurrence_table.inverse_table_2) ;
       init_recurrence  <4,+1>(  s2p_4,  s2p_8, fft128_recurrence_table.inverse_table_4) ;
       init_recurrence  <8,+1>(  s2p_8, s2p_16, fft128_recurrence_table.inverse_table_8) ;
       init_recurrence <16,+1>( s2p_16, s2p_32, fft128_recurrence_table.inverse_table_16) ;
       init_recurrence <32,+1>( s2p_32, s2p_64, fft128_recurrence_table.inverse_table_32) ;
       init_recurrence <64,+1>( s2p_64,s2p_128, fft128_recurrence_table.inverse_table_64) ;
       init_recurrence<128,+1>(s2p_128,s2p_256, fft128_recurrence_table.inverse_table_128) ;

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
       if (-1 == isign)
       {
         loop1  <2,isign>(  s2p_2,  s2p_4, fft128_recurrence_table.forward_table_2) ;
         loop1  <4,isign>(  s2p_4,  s2p_8, fft128_recurrence_table.forward_table_4) ;
         loop1  <8,isign>(  s2p_8, s2p_16, fft128_recurrence_table.forward_table_8) ;
         loop1 <16,isign>( s2p_16, s2p_32, fft128_recurrence_table.forward_table_16) ;
         loop1 <32,isign>( s2p_32, s2p_64, fft128_recurrence_table.forward_table_32) ;
         loop1 <64,isign>( s2p_64,s2p_128, fft128_recurrence_table.forward_table_64) ;
         loop1<128,isign>(s2p_128,s2p_256, fft128_recurrence_table.forward_table_128) ;
       }
       else
       {
         loop1  <2,isign>(  s2p_2,  s2p_4, fft128_recurrence_table.inverse_table_2) ;
         loop1  <4,isign>(  s2p_4,  s2p_8, fft128_recurrence_table.inverse_table_4) ;
         loop1  <8,isign>(  s2p_8, s2p_16, fft128_recurrence_table.inverse_table_8) ;
         loop1 <16,isign>( s2p_16, s2p_32, fft128_recurrence_table.inverse_table_16) ;
         loop1 <32,isign>( s2p_32, s2p_64, fft128_recurrence_table.inverse_table_32) ;
         loop1 <64,isign>( s2p_64,s2p_128, fft128_recurrence_table.inverse_table_64) ;
         loop1<128,isign>(s2p_128,s2p_256, fft128_recurrence_table.inverse_table_128) ;
       } /* endif */
   } ;

   // Interface. Neither scales, so that after 'forward' and 'inverse' you need
   // to divide by the number of points (128) if you want to retrieve the data you
   // started with. This matches FFTW.

   // Note .. these are set up in the 'FFTW' convention, with '-1' exponent
   // for forward, and '1' exponent for inverse. 'Numerical Recipes' has them
   // the other way round. Look carefully at the definitions of FFT in 12.0.1
   // in NR, and FFTW_FORWARD/FFTW_BACKWARD for FFTW.

   void inline_forward(void) {
     DLSection<-1>() ;
   } ;
   void inline_inverse(void) {
     DLSection<1>() ;
   }
   ;

  } __attribute__((aligned(32)));

fft128_recurrence_tables fft128::fft128_recurrence_table ;

void fft128_forward(fft3d_complex * target_a, fft3d_complex * target_b ,const fft3d_complex * source_a, const fft3d_complex * source_b)
{
        fft128 fft ;
        fft.copyin(source_a, source_b) ;
        fft.inline_forward() ;
        fft.copyout(target_a, target_b) ;
 }

void fft128_inverse(fft3d_complex * target_a, fft3d_complex * target_b ,const fft3d_complex * source_a, const fft3d_complex * source_b)
{
        fft128 fft ;
        fft.copyin(source_a, source_b) ;
        fft.inline_inverse() ;
        fft.copyout(target_a, target_b) ;
}

fft128_recurrence_tables::fft128_recurrence_tables(void)
{
  fft128::init_all_recurrence() ;
}

#endif
