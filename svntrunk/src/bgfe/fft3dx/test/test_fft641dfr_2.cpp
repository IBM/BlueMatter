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
 // #include <pk/platform.hpp>
// #include <pk/fxlogger.hpp>

// #include <fft3d/fftcomplex.hpp>

#include <fft3d/fft64a.hpp>

//#include <fft3d/fft641dfr.hpp>

inline double myfabs(double x)
{
  return (x<0.0) ? (-x) : x ;
}
extern "C" {
  int _CP_main(void)  ;
  int _IOP_main(void) ;
  void s0printf(const char *, ...) ;
  unsigned long long GetTimeBase(void) ;
} ;

//#include <stdio.h>
//#define s0printf printf

void test_fft641dfr_2(void)
{
   fft3d_complex block_a[64] ;
   fft3d_complex block_b[64]  ;
   fft3d_complex block_c[64]  ;
   fft3d_complex block_d[64]  ;
   for (int a=0; a<64; a+=1)
   {
      block_a[a].re = 0.0 ;
      block_a[a].im = 0.0 ;
      block_b[a].re = 0.0 ;
      block_b[a].im = 1.0 ;
      block_c[a].re = -999.0 ;
      block_c[a].im = -999.0 ;
      block_d[a].re = -999.0 ;
      block_d[a].im = -999.0 ;
   } /* endfor */

   fft3d_complex in_org[ 64 ];

   in_org[0].re=0.000000; in_org[0].im=0.000000;
   in_org[1].re=64.000000; in_org[1].im=0.000000;
   in_org[2].re=128.000000; in_org[2].im=0.000000;
   in_org[3].re=192.000000; in_org[3].im=0.000000;
   in_org[4].re=256.000000; in_org[4].im=0.000000;
   in_org[5].re=320.000000; in_org[5].im=0.000000;
   in_org[6].re=384.000000; in_org[6].im=0.000000;
   in_org[7].re=448.000000; in_org[7].im=0.000000;
   in_org[8].re=512.000000; in_org[8].im=0.000000;
   in_org[9].re=576.000000; in_org[9].im=0.000000;
   in_org[10].re=640.000000; in_org[10].im=0.000000;
   in_org[11].re=704.000000; in_org[11].im=0.000000;
   in_org[12].re=768.000000; in_org[12].im=0.000000;
   in_org[13].re=832.000000; in_org[13].im=0.000000;
   in_org[14].re=896.000000; in_org[14].im=0.000000;
   in_org[15].re=960.000000; in_org[15].im=0.000000;
   in_org[16].re=1024.000000; in_org[16].im=0.000000;
   in_org[17].re=1088.000000; in_org[17].im=0.000000;
   in_org[18].re=1152.000000; in_org[18].im=0.000000;
   in_org[19].re=1216.000000; in_org[19].im=0.000000;
   in_org[20].re=1280.000000; in_org[20].im=0.000000;
   in_org[21].re=1344.000000; in_org[21].im=0.000000;
   in_org[22].re=1408.000000; in_org[22].im=0.000000;
   in_org[23].re=1472.000000; in_org[23].im=0.000000;
   in_org[24].re=1536.000000; in_org[24].im=0.000000;
   in_org[25].re=1600.000000; in_org[25].im=0.000000;
   in_org[26].re=1664.000000; in_org[26].im=0.000000;
   in_org[27].re=1728.000000; in_org[27].im=0.000000;
   in_org[28].re=1792.000000; in_org[28].im=0.000000;
   in_org[29].re=1856.000000; in_org[29].im=0.000000;
   in_org[30].re=1920.000000; in_org[30].im=0.000000;
   in_org[31].re=1984.000000; in_org[31].im=0.000000;
   in_org[32].re=2048.000000; in_org[32].im=0.000000;
   in_org[33].re=2112.000000; in_org[33].im=0.000000;
   in_org[34].re=2176.000000; in_org[34].im=0.000000;
   in_org[35].re=2240.000000; in_org[35].im=0.000000;
   in_org[36].re=2304.000000; in_org[36].im=0.000000;
   in_org[37].re=2368.000000; in_org[37].im=0.000000;
   in_org[38].re=2432.000000; in_org[38].im=0.000000;
   in_org[39].re=2496.000000; in_org[39].im=0.000000;
   in_org[40].re=2560.000000; in_org[40].im=0.000000;
   in_org[41].re=2624.000000; in_org[41].im=0.000000;
   in_org[42].re=2688.000000; in_org[42].im=0.000000;
   in_org[43].re=2752.000000; in_org[43].im=0.000000;
   in_org[44].re=2816.000000; in_org[44].im=0.000000;
   in_org[45].re=2880.000000; in_org[45].im=0.000000;
   in_org[46].re=2944.000000; in_org[46].im=0.000000;
   in_org[47].re=3008.000000; in_org[47].im=0.000000;
   in_org[48].re=3072.000000; in_org[48].im=0.000000;
   in_org[49].re=3136.000000; in_org[49].im=0.000000;
   in_org[50].re=3200.000000; in_org[50].im=0.000000;
   in_org[51].re=3264.000000; in_org[51].im=0.000000;
   in_org[52].re=3328.000000; in_org[52].im=0.000000;
   in_org[53].re=3392.000000; in_org[53].im=0.000000;
   in_org[54].re=3456.000000; in_org[54].im=0.000000;
   in_org[55].re=3520.000000; in_org[55].im=0.000000;
   in_org[56].re=3584.000000; in_org[56].im=0.000000;
   in_org[57].re=3648.000000; in_org[57].im=0.000000;
   in_org[58].re=3712.000000; in_org[58].im=0.000000;
   in_org[59].re=3776.000000; in_org[59].im=0.000000;
   in_org[60].re=3840.000000; in_org[60].im=0.000000;
   in_org[61].re=3904.000000; in_org[61].im=0.000000;
   in_org[62].re=3968.000000; in_org[62].im=0.000000;
   in_org[63].re=4032.000000; in_org[63].im=0.000000;


   fft3d_complex out_org[ 64 ];

  out_org[0].re=0.000000; out_org[0].im=0.000000;
  out_org[1].re=96.000000; out_org[1].im=0.000000;
  out_org[2].re=192.000000; out_org[2].im=0.000000;
  out_org[3].re=288.000000; out_org[3].im=0.000000;
  out_org[4].re=384.000000; out_org[4].im=0.000000;
  out_org[5].re=480.000000; out_org[5].im=0.000000;
  out_org[6].re=576.000000; out_org[6].im=0.000000;
  out_org[7].re=672.000000; out_org[7].im=0.000000;
  out_org[8].re=768.000000; out_org[8].im=0.000000;
  out_org[9].re=864.000000; out_org[9].im=0.000000;
  out_org[10].re=960.000000; out_org[10].im=0.000000;
  out_org[11].re=1056.000000; out_org[11].im=0.000000;
  out_org[12].re=1152.000000; out_org[12].im=0.000000;
  out_org[13].re=1248.000000; out_org[13].im=0.000000;
  out_org[14].re=1344.000000; out_org[14].im=0.000000;
  out_org[15].re=1440.000000; out_org[15].im=0.000000;
  out_org[16].re=1536.000000; out_org[16].im=0.000000;
  out_org[17].re=1632.000000; out_org[17].im=0.000000;
  out_org[18].re=1728.000000; out_org[18].im=0.000000;
  out_org[19].re=1824.000000; out_org[19].im=0.000000;
  out_org[20].re=1920.000000; out_org[20].im=0.000000;
  out_org[21].re=2016.000000; out_org[21].im=0.000000;
  out_org[22].re=2112.000000; out_org[22].im=0.000000;
  out_org[23].re=2208.000000; out_org[23].im=0.000000;
  out_org[24].re=2304.000000; out_org[24].im=0.000000;
  out_org[25].re=2400.000000; out_org[25].im=0.000000;
  out_org[26].re=2496.000000; out_org[26].im=0.000000;
  out_org[27].re=2592.000000; out_org[27].im=0.000000;
  out_org[28].re=2688.000000; out_org[28].im=0.000000;
  out_org[29].re=2784.000000; out_org[29].im=0.000000;
  out_org[30].re=2880.000000; out_org[30].im=0.000000;
  out_org[31].re=2976.000000; out_org[31].im=0.000000;
  out_org[32].re=3072.000000; out_org[32].im=0.000000;
  out_org[33].re=3168.000000; out_org[33].im=0.000000;
  out_org[34].re=3264.000000; out_org[34].im=0.000000;
  out_org[35].re=3360.000000; out_org[35].im=0.000000;
  out_org[36].re=3456.000000; out_org[36].im=0.000000;
  out_org[37].re=3552.000000; out_org[37].im=0.000000;
  out_org[38].re=3648.000000; out_org[38].im=0.000000;
  out_org[39].re=3744.000000; out_org[39].im=0.000000;
  out_org[40].re=3840.000000; out_org[40].im=0.000000;
  out_org[41].re=3936.000000; out_org[41].im=0.000000;
  out_org[42].re=4032.000000; out_org[42].im=0.000000;
  out_org[43].re=4128.000000; out_org[43].im=0.000000;
  out_org[44].re=4224.000000; out_org[44].im=0.000000;
  out_org[45].re=4320.000000; out_org[45].im=0.000000;
  out_org[46].re=4416.000000; out_org[46].im=0.000000;
  out_org[47].re=4512.000000; out_org[47].im=0.000000;
  out_org[48].re=4608.000000; out_org[48].im=0.000000;
  out_org[49].re=4704.000000; out_org[49].im=0.000000;
  out_org[50].re=4800.000000; out_org[50].im=0.000000;
  out_org[51].re=4896.000000; out_org[51].im=0.000000;
  out_org[52].re=4992.000000; out_org[52].im=0.000000;
  out_org[53].re=5088.000000; out_org[53].im=0.000000;
  out_org[54].re=5184.000000; out_org[54].im=0.000000;
  out_org[55].re=5280.000000; out_org[55].im=0.000000;
  out_org[56].re=5376.000000; out_org[56].im=0.000000;
  out_org[57].re=5472.000000; out_org[57].im=0.000000;
  out_org[58].re=5568.000000; out_org[58].im=0.000000;
  out_org[59].re=5664.000000; out_org[59].im=0.000000;
  out_org[60].re=5760.000000; out_org[60].im=0.000000;
  out_org[61].re=5856.000000; out_org[61].im=0.000000;
  out_org[62].re=5952.000000; out_org[62].im=0.000000;
  out_org[63].re=6048.000000; out_org[63].im=0.000000;



  // First to touch caches
  fft64_forward(block_c, block_d, in_org, out_org ) ;
  // Next for timing
  int StartTime = GetTimeBase() ;
  fft64_forward(block_c, block_d, in_org, out_org ) ;
  int EndTime = GetTimeBase() ;

  int Elapsed = EndTime-StartTime ;

  s0printf("Elapsed time for 2x 64-point FFTs = %d pclocks\n",Elapsed) ;

  fft3d_complex in[ 64 ];

  in[0].re=129024.000000; in[0].im=0.000000;
  in[1].re=-2048.000000; in[1].im=41687.997696;
  in[2].re=-2048.000000; in[2].im=20793.692954;
  in[3].re=-2048.000000; in[3].im=13806.494526;
  in[4].re=-2048.000000; in[4].im=10295.991280;
  in[5].re=-2048.000000; in[5].im=8176.074309;
  in[6].re=-2048.000000; in[6].im=6751.351212;
  in[7].re=-2048.000000; in[7].im=5723.776558;
  in[8].re=-2048.000000; in[8].im=4944.309376;
  in[9].re=-2048.000000; in[9].im=4330.132188;
  in[10].re=-2048.000000; in[10].im=3831.538507;
  in[11].re=-2048.000000; in[11].im=3416.881573;
  in[12].re=-2048.000000; in[12].im=3065.048602;
  in[13].re=-2048.000000; in[13].im=2761.408335;
  in[14].re=-2048.000000; in[14].im=2495.495220;
  in[15].re=-2048.000000; in[15].im=2259.619790;
  in[16].re=-2048.000000; in[16].im=2048.000000;
  in[17].re=-2048.000000; in[17].im=1856.199002;
  in[18].re=-2048.000000; in[18].im=1680.750164;
  in[19].re=-2048.000000; in[19].im=1518.900319;
  in[20].re=-2048.000000; in[20].im=1368.429850;
  in[21].re=-2048.000000; in[21].im=1227.523960;
  in[22].re=-2048.000000; in[22].im=1094.678806;
  in[23].re=-2048.000000; in[23].im=968.631861;
  in[24].re=-2048.000000; in[24].im=848.309376;
  in[25].re=-2048.000000; in[25].im=732.786117;
  in[26].re=-2048.000000; in[26].im=621.254008;
  in[27].re=-2048.000000; in[27].im=512.997294;
  in[28].re=-2048.000000; in[28].im=407.372528;
  in[29].re=-2048.000000; in[29].im=303.792102;
  in[30].re=-2048.000000; in[30].im=201.710394;
  in[31].re=-2048.000000; in[31].im=100.611788;
  in[32].re=-2048.000000; in[32].im=-0.000000;
  in[33].re=-2048.000000; in[33].im=-100.611788;
  in[34].re=-2048.000000; in[34].im=-201.710394;
  in[35].re=-2048.000000; in[35].im=-303.792102;
  in[36].re=-2048.000000; in[36].im=-407.372528;
  in[37].re=-2048.000000; in[37].im=-512.997294;
  in[38].re=-2048.000000; in[38].im=-621.254008;
  in[39].re=-2048.000000; in[39].im=-732.786117;
  in[40].re=-2048.000000; in[40].im=-848.309376;
  in[41].re=-2048.000000; in[41].im=-968.631861;
  in[42].re=-2048.000000; in[42].im=-1094.678806;
  in[43].re=-2048.000000; in[43].im=-1227.523960;
  in[44].re=-2048.000000; in[44].im=-1368.429850;
  in[45].re=-2048.000000; in[45].im=-1518.900319;
  in[46].re=-2048.000000; in[46].im=-1680.750164;
  in[47].re=-2048.000000; in[47].im=-1856.199002;
  in[48].re=-2048.000000; in[48].im=-2048.000000;
  in[49].re=-2048.000000; in[49].im=-2259.619790;
  in[50].re=-2048.000000; in[50].im=-2495.495220;
  in[51].re=-2048.000000; in[51].im=-2761.408335;
  in[52].re=-2048.000000; in[52].im=-3065.048602;
  in[53].re=-2048.000000; in[53].im=-3416.881573;
  in[54].re=-2048.000000; in[54].im=-3831.538507;
  in[55].re=-2048.000000; in[55].im=-4330.132188;
  in[56].re=-2048.000000; in[56].im=-4944.309376;
  in[57].re=-2048.000000; in[57].im=-5723.776558;
  in[58].re=-2048.000000; in[58].im=-6751.351212;
  in[59].re=-2048.000000; in[59].im=-8176.074309;
  in[60].re=-2048.000000; in[60].im=-10295.991280;
  in[61].re=-2048.000000; in[61].im=-13806.494526;
  in[62].re=-2048.000000; in[62].im=-20793.692954;
  in[63].re=-2048.000000; in[63].im=-41687.997696;


//    block_a[1].re = 1.0 ;
  //    fft64_forward(block_c, block_d, block_a, block_b) ;

  double MaxDiffRe = -1.0;
  double MaxDiffIm = -1.0;

  int LocRe=-1 ;
  int LocIm=-1 ;

  for (int b=0; b<64; b+=1)
    {

      double diffRe = myfabs( block_c[b].re - in[b].re );
      double diffIm = myfabs( block_c[b].im - in[b].im );

      if( diffRe > MaxDiffRe )
        {
        MaxDiffRe = diffRe;
        LocRe = b;
        }

      if( diffIm > MaxDiffIm )
        {
        MaxDiffIm = diffIm;
        LocIm = b ;
        }


        s0printf("FFT_1D block_c(in)[%d] = (%d/1000,%d/1000) (%d/1000,%d/1000)\n",
                b,(int)(block_c[b].re*1000.0), (int) (block_c[b].im*1000.0)
                 ,(int)(in[b].re*1000.0), (int) (in[b].im*1000.0)) ;
    } /* endfor */

  s0printf("FFT_1D max diff=(%d/1000,%d/1000) at %d,%d\n",
        (int) (MaxDiffRe*1000.0), (int) (MaxDiffIm*1000.0), LocRe,LocIm) ;

  for (int b=0; b<64; b+=1)
    {

        s0printf("FFT_1D block_d(out)[%d] = (%d/1000,%d/1000)\n",
                b,(int)(block_d[b].re*1000.0), (int) (block_d[b].im*1000.0)) ;
    }

}

int _CP_main(void)
//int
//PkMain( int argc, char ** argv, char ** envp )
//int main(void)
{
   test_fft641dfr_2() ;
   return 0 ;
}

int _IOP_main(void)
{
   return 0 ;
}

