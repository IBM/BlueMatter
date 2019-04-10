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
 #if !defined(_T3DA_HPP_)
#define _T3DA_HPP_

#include <assert.h>
#include <BlueMatter/fft128.hpp>
// 3-d data management structure, with a view to doing 3-d (fast) fourier transforms
// This works for the compiled-in size which must be a power of 2, and cubic.
// We should be able to take 'k_extent_bits' as a template parameter
__align(128) class t3Dc {

public:

// original constants
    enum {
       // The whole cube
       k_extent_bits = 7 ,
       k_subcube_bits = 3
       } ;
// derived constants
    enum {
       k_extent = 1 << k_extent_bits ,
       k_subcube_extent = 1 << k_subcube_bits ,
       k_subcube_count = 1 << ( k_extent_bits - k_subcube_bits )
       } ;
      inline void Init( int sz_x, int sz_y, int sz_z, void *ptr = NULL )
      {
         assert( sz_x == k_extent ) ;
         assert( sz_y == k_extent ) ;
         assert( sz_z == k_extent ) ;
         assert( ptr == NULL ) ;
      } ;

      void Zero(void);

    inline complex& operator()( int x, int y, int z )
    {
        return array[x][y][z] ; // row-major, C style
    }
    ;
//  inline operator complex *() const { return array; }
      t3Dc& operator=( const t3Dc& at3Dc );

    // Rotate the cube about its (0,0,0) --> (1,1,1) axis such that
    // 3 rotations are identity; required for cache-blocking a 3d fft
    void Rotate(void) ;

    __align(128) class subcube {
       public:
       complex subarray[k_subcube_extent][k_subcube_extent][k_subcube_extent] ;
       } ;

    void CopyToSubcube     (int x, int y, int z, subcube& Target) const ;
    void TwistSwitchSubcube(int x, int y, int z, const subcube& Source, subcube& Target) ;
    void TwistFromSubcube  (int x, int y, int z, const subcube& Source)  ;

    complex array[k_extent][k_extent][k_extent];

};

void t3Dc
::Zero(void)
{
   for (int x=0; x<k_extent;x+=1 )
   {
      for (int y=0; y<k_extent;y+=1 )
      {
         for (int z=0; z<k_extent;z+=1 )
         {
            array[x][y][z] = static_cast<complex>(0.0) ;
         } /* endfor */
      } /* endfor */
   } /* endfor */
}

t3Dc&  t3Dc
::operator=( const t3Dc& at3Dc )
{

//   #pragma disjoint(*array,*at3Dc.array)

   for (int x=0; x<k_extent;x+=1 )
   {
      for (int y=0; y<k_extent;y+=1 )
      {
         for (int z=0; z<k_extent;z+=1 )
         {
            array[x][y][z] = at3Dc.array[x][y][z] ;
         } /* endfor */
      } /* endfor */
   } /* endfor */

    return *this;
}


void t3Dc
::Rotate(void)
{
   // Run the rotation in subcubes, each of which fits a cache line
   // subcube subcube_a ;
    // subcube subcube_b ;
   static subcube subcube_a ;
   static subcube subcube_b ;

   for (int x=0; x<k_subcube_count;x+=1 )
   {
      for (int y=x; y<k_subcube_count;y+=1 )
      {
         for (int z=y; z<k_subcube_count;z+=1 )
         {
            if (y > x || z > y)
            {
               CopyToSubcube     (x,y,z,subcube_a) ;
               TwistSwitchSubcube(z,x,y,subcube_a,subcube_b) ;
               TwistSwitchSubcube(y,z,x,subcube_b,subcube_a) ;
               TwistFromSubcube  (x,y,z,subcube_a) ;
            } else {
               CopyToSubcube     (x,y,z,subcube_a) ;
               TwistFromSubcube  (x,y,z,subcube_a) ;
            } /* endif */
         } /* endfor */
      } /* endfor */
   } /* endfor */
}

void t3Dc
::CopyToSubcube(int x,int y, int z, subcube& Target) const
{
// #pragma disjoint(*Target.array,*array)
   int cx = x * k_subcube_extent ;
   int cy = y * k_subcube_extent ;
   int cz = z * k_subcube_extent ;
   for (int x0=0; x0 < k_subcube_extent; x0 += 1)
   {
      for (int y0=0; y0 < k_subcube_extent; y0 += 1)
      {
         complex q0 = array[cx+x0][cy+y0][cz+0] ;
         complex q2 = array[cx+x0][cy+y0][cz+2] ;
         complex q4 = array[cx+x0][cy+y0][cz+4] ;
         complex q6 = array[cx+x0][cy+y0][cz+6] ;
         complex q1 = array[cx+x0][cy+y0][cz+1] ;
         complex q3 = array[cx+x0][cy+y0][cz+3] ;
         complex q5 = array[cx+x0][cy+y0][cz+5] ;
         complex q7 = array[cx+x0][cy+y0][cz+7] ;


         Target.subarray[x0][y0][0] = q0 ;
         Target.subarray[x0][y0][1] = q1 ;
         Target.subarray[x0][y0][2] = q2 ;
         Target.subarray[x0][y0][3] = q3 ;
         Target.subarray[x0][y0][4] = q4 ;
         Target.subarray[x0][y0][5] = q5 ;
         Target.subarray[x0][y0][6] = q6 ;
         Target.subarray[x0][y0][7] = q7 ;

//       for (int z0=0; z0 < k_subcube_extent; z0 += 1)
//       {
//          Target.subarray[x0][y0][z0] = array[cx+x0][cy+y0][cz+z0] ;
//       } /* endfor */
      } /* endfor */
   } /* endfor */
}

void t3Dc
::TwistSwitchSubcube(int x,int y, int z, const subcube& Source, subcube& Target)
{
//#pragma disjoint(*Target.array,*array)
//#pragma disjoint(*Source.array,*array)
//#pragma disjoint(*Source.array,*Target.array)
   int cx = x * k_subcube_extent ;
   int cy = y * k_subcube_extent ;
   int cz = z * k_subcube_extent ;
   for (int x0=0; x0 < k_subcube_extent; x0 += 1)
   {
      for (int y0=0; y0 < k_subcube_extent; y0 += 1)
      {
         if (y0 < k_subcube_extent-1)
         {
           __prefetch_by_stream(1, array[cx+x0][(cy+1)+y0] ) ;
         }
         else
         {
            if (x0 < k_subcube_extent-1)
            {
               __prefetch_by_stream(1, array[(cx+1)+x0][0] ) ;
            } /* endif */
         } /* endif */
         complex q0 = array[cx+x0][cy+y0][cz+0] ;
         complex q2 = array[cx+x0][cy+y0][cz+2] ;
         complex q4 = array[cx+x0][cy+y0][cz+4] ;
         complex q6 = array[cx+x0][cy+y0][cz+6] ;
         complex q1 = array[cx+x0][cy+y0][cz+1] ;
         complex q3 = array[cx+x0][cy+y0][cz+3] ;
         complex q5 = array[cx+x0][cy+y0][cz+5] ;
         complex q7 = array[cx+x0][cy+y0][cz+7] ;

         complex s0 =  Source.subarray[0][x0][y0] ;
         complex s1 =  Source.subarray[1][x0][y0] ;
         array[cx+x0][cy+y0][cz+0] = s0 ;
         complex s2 =  Source.subarray[2][x0][y0] ;
         array[cx+x0][cy+y0][cz+1] = s1 ;
         complex s3 =  Source.subarray[3][x0][y0] ;
         array[cx+x0][cy+y0][cz+2] = s2 ;
         complex s4 =  Source.subarray[4][x0][y0] ;
         array[cx+x0][cy+y0][cz+3] = s3 ;
         complex s5 =  Source.subarray[5][x0][y0] ;
         array[cx+x0][cy+y0][cz+4] = s4 ;
         complex s6 =  Source.subarray[6][x0][y0] ;
         array[cx+x0][cy+y0][cz+5] = s5 ;
         complex s7 =  Source.subarray[7][x0][y0] ;
         array[cx+x0][cy+y0][cz+6] = s6 ;
         array[cx+x0][cy+y0][cz+7] = s7 ;

         Target.subarray[x0][y0][0] = q0 ;
         Target.subarray[x0][y0][1] = q1 ;
         Target.subarray[x0][y0][2] = q2 ;
         Target.subarray[x0][y0][3] = q3 ;
         Target.subarray[x0][y0][4] = q4 ;
         Target.subarray[x0][y0][5] = q5 ;
         Target.subarray[x0][y0][6] = q6 ;
         Target.subarray[x0][y0][7] = q7 ;


//         for (int z0=0; z0 < k_subcube_extent; 70 += 1)
//         {
//            Target.subarray[x0][y0][z0] = array[cx+x0][cy+y0][cz+z0] ;
//            array[cx+x0][cy+y0][cz+z0] = Source.subarray[z0][x0][y0] ;
//         } /* endfor */
      } /* endfor */
   } /* endfor */
}

void t3Dc
::TwistFromSubcube(int x,int y, int z, const subcube& Source)
{
// #pragma disjoint(*Source.array,*array)
   int cx = x * k_subcube_extent ;
   int cy = y * k_subcube_extent ;
   int cz = z * k_subcube_extent ;
   for (int x0=0; x0 < k_subcube_extent; x0 += 1)
   {
      for (int y0=0; y0 < k_subcube_extent; y0 += 1)
      {
         complex s0 =  Source.subarray[0][x0][y0] ;
         complex s1 =  Source.subarray[1][x0][y0] ;
         array[cx+x0][cy+y0][cz+0] = s0 ;
         complex s2 =  Source.subarray[2][x0][y0] ;
         array[cx+x0][cy+y0][cz+1] = s1 ;
         complex s3 =  Source.subarray[3][x0][y0] ;
         array[cx+x0][cy+y0][cz+2] = s2 ;
         complex s4 =  Source.subarray[4][x0][y0] ;
         array[cx+x0][cy+y0][cz+3] = s3 ;
         complex s5 =  Source.subarray[5][x0][y0] ;
         array[cx+x0][cy+y0][cz+4] = s4 ;
         complex s6 =  Source.subarray[6][x0][y0] ;
         array[cx+x0][cy+y0][cz+5] = s5 ;
         complex s7 =  Source.subarray[7][x0][y0] ;
         array[cx+x0][cy+y0][cz+6] = s6 ;
         array[cx+x0][cy+y0][cz+7] = s7 ;

//       for (int z0=0; z0 < k_subcube_extent; z0 += 1)
//       {
//          array[cx+x0][cy+y0][cz+z0] = Source.subarray[z0][x0][y0] ;
//       } /* endfor */

      } /* endfor */
   } /* endfor */
}

class t3Dcfft: public t3Dc
{
   public:
   void forward_1d(void) ;
   void inverse_1d(void) ;
   void forward(void) ;
   void inverse(void) ;
} ;

void  t3Dcfft::forward_1d(void)
{
   // The array is deliberately mis-indexed, to exploit the C/C++ storage layout
   int squarecount = k_extent*k_extent ;
   int quarter_count = squarecount / 4 ;
   for (int q0_index = 0; q0_index < quarter_count ; q0_index += 1)
   {
      int q1_index = q0_index + quarter_count ;
      int q2_index = q0_index + 2*quarter_count ;
      int q3_index = q0_index + 3*quarter_count ;
      int q3_prefetch = ( q0_index < quarter_count-1 ) ? (q3_index+1) : q3_index ;

      fft128 zt0=(array[0][q0_index],
                  array[0][q0_index+1],
                  array[0][q1_index+1],
                  array[0][q2_index+1],
                  array[0][q3_prefetch]) ;
      zt0.forward() ;

      fft128 zt1=(array[0][q1_index],
                  array[0][q1_index+1],
                  array[0][q2_index+1],
                  array[0][q3_prefetch],
                  array[0][q0_index+1]) ;
      zt1.forward() ;

      fft128 zt2=(array[0][q2_index],
                  array[0][q2_index+1],
                  array[0][q3_prefetch],
                  array[0][q0_index+1],
                  array[0][q1_index+1]) ;
      zt2.forward() ;

      fft128 zt3=(array[0][q3_index],
                  array[0][q3_prefetch],
                  array[0][q0_index+1],
                  array[0][q1_index+1],
                  array[0][q2_index+1]) ;

      zt3.forward() ;
   } /* endfor */
// for (int xy=0; xy<(k_extent*k_extent); xy+=1)
// {
//    int prefetched_fft_index = (xy<(k_extent*k_extent-1) ) ? (xy+1) : xy ;
//    fft128 zt(array[0][xy],array[0][prefetched_fft_index]) ;
//    zt.forward() ;
// } /* endfor */
// for (int x=0; x<k_extent; x+=1)
// {
//    for (int y=0; y<k_extent; y+=1)
//    {
//       fft128 zt(array[x][y]) ;
//       zt.forward() ;
//    } /* endfor */
// } /* endfor */
}

void t3Dcfft::inverse_1d(void)
{
   // The array is deliberately mis-indexed, to exploit the C/C++ storage layout
   int squarecount = k_extent*k_extent ;
   int quarter_count = squarecount / 4 ;
   for (int q0_index = 0; q0_index < quarter_count ; q0_index += 1)
   {
      int q1_index = q0_index + quarter_count ;
      int q2_index = q0_index + 2*quarter_count ;
      int q3_index = q0_index + 3*quarter_count ;
      int q3_prefetch = ( q0_index < quarter_count-1 ) ? (q3_index+1) : q3_index ;

      fft128 zt0=(array[0][q0_index],
                  array[0][q0_index+1],
                  array[0][q1_index+1],
                  array[0][q2_index+1],
                  array[0][q3_prefetch]) ;
      zt0.inverse() ;

      fft128 zt1=(array[0][q1_index],
                  array[0][q1_index+1],
                  array[0][q2_index+1],
                  array[0][q3_prefetch],
                  array[0][q0_index+1]) ;
      zt1.inverse() ;

      fft128 zt2=(array[0][q2_index],
                  array[0][q2_index+1],
                  array[0][q3_prefetch],
                  array[0][q0_index+1],
                  array[0][q1_index+1]) ;
      zt2.inverse() ;

      fft128 zt3=(array[0][q3_index],
                  array[0][q3_prefetch],
                  array[0][q0_index+1],
                  array[0][q1_index+1],
                  array[0][q2_index+1]) ;

      zt3.inverse() ;
   } /* endfor */
   // The array is deliverately mis-indexed, to exploit the C/C++ storage layout
// for (int xy=0; xy<(k_extent*k_extent); xy+=1)
// {
//    int prefetched_fft_index = (xy<(k_extent*k_extent-1) ) ? (xy+1) : xy ;
//    fft128 zt(array[0][xy],array[0][prefetched_fft_index]) ;
//    zt.inverse() ;
// } /* endfor */
// for (int x=0; x<k_extent; x+=1)
// {
//    for (int y=0; y<k_extent; y+=1)
//    {
//       fft128 zt(array[x][y]) ;
//       zt.inverse() ;
//    } /* endfor */
// } /* endfor */
}

void  t3Dcfft::forward(void)
{
   forward_1d() ;
   Rotate() ;
   forward_1d() ;
   Rotate() ;
   forward_1d() ;
   Rotate() ;
}

void t3Dcfft::inverse(void)
{
   inverse_1d() ;
   Rotate() ;
   inverse_1d() ;
   Rotate() ;
   inverse_1d() ;
   Rotate() ;
}

#endif
