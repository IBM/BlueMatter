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
  template <class T> static inline T asin_small(T x)
    {
       const double ap0 = 1.0           , aq0 = 1.0       ;
       const double ap1 = ap0 * 1.0     , aq1 = aq0 * 2.0 ;
       const double ap2 = ap1 * 3.0     , aq2 = aq1 * 4.0 ;
       const double ap3 = ap2 * 5.0     , aq3 = aq2 * 6.0 ;
       const double ap4 = ap3 * 7.0     , aq4 = aq3 * 8.0 ;
       const double ap5 = ap4 * 9.0     , aq5 = aq4 * 10.0 ;
       const double ap6 = ap5 * 11.0    , aq6 = aq5 * 12.0 ;
       const double ap7 = ap6 * 13.0    , aq7 = aq6 * 14.0 ;
       const double ap8 = ap7 * 15.0    , aq8 = aq7 * 16.0 ;
       const double ap9 = ap8 * 17.0    , aq9 = aq8 * 18.0 ;
       const double apa = ap9 * 19.0    , aqa = aq9 * 20.0 ;
       const double apb = apa * 21.0    , aqb = aqa * 22.0 ;
       const double apc = apb * 23.0    , aqc = aqb * 24.0 ;
       const double apd = apc * 25.0    , aqd = aqc * 26.0 ;
       const double ape = apd * 27.0    , aqe = aqd * 28.0 ;
       const double apf = ape * 29.0    , aqf = aqe * 30.0 ;
       const double a14 =  apf / ( aqf * 31.0 ) ;
       const double a13 =  ape / ( aqe * 29.0 ) ;
       const double a12 =  apd / ( aqd * 27.0 ) ;
       const double a11 =  apc / ( aqc * 25.0 ) ;      ;
       const double a10 =  apb / ( aqb * 23.0 ) ;
       const double a9  =  apa / ( aqa * 21.0 ) ;
       const double a8  =  ap9 / ( aq9 * 19.0 ) ;
       const double a7  =  ap8 / ( aq8 * 17.0 ) ;
       const double a6  =  ap7 / ( aq7 * 15.0 ) ;
       const double a5  =  ap6 / ( aq6 * 13.0 ) ;
       const double a4  =  ap5 / ( aq5 * 11.0 ) ;
       const double a3  =  ap4 / ( aq4 * 9.0 ) ;
       const double a2  =  ap3 / ( aq3 * 7.0 ) ;
       const double a1  =  ap2 / ( aq2 * 5.0 ) ;
       const double a0  =  ap1 / ( aq1 * 3.0 ) ;
      double b, s, t1, t0;
      double r, d, h, d1, h1, yy1, g1, d2, h2, gl;
      double e, e1, g2, u, v, yy0;

      s =  a14 + a13 ;

      b = x*x;
      t0 = a14 * b + a13;
      s = b * b;
      t0 = (((((t0*s + a11)*s + a9)*s + a7)*s + a5)*s +a3)*s
        + a1;
      t1 = (((((a12*s + a10)*s + a8)*s + a6)*s + a4)*s + a2);
      return ( x + (x*b)*(a0 + b*(t0 + b*t1)));
    }

    // Given the sin and cos of an angle, return the angle.
    // Returns an angle in (-PI, PI)

    inline static double acossin ( double sinang, double cosang )
    {
      const double piby8 = M_PI / 8.0 ; // 22.5 degrees, in radians;
      const double pi3by8 = M_PI * ( 3.0 / 8.0 ) ; // 3*22.5 degrees, in radians;
      const double pi5by8 = M_PI * ( 5.0 / 8.0 ) ; // 5*22.5 degrees, in radians;
      const double pi7by8 = M_PI * ( 7.0 / 8.0 ) ; // 7*22.5 degrees, in radians;
      const double cospiby4 = sqrt(2.0) * 0.5 ;
      const double cospiby8 = sqrt((1+cospiby4) * 0.5) ;
      const double sinpiby8 = sqrt(1-cospiby8*cospiby8) ;
      double abscos = fabs(cosang) ; // abscos in (0,1)
      double abssin = fabs(sinang) ; // abssin in (0,1)
      double coslarge = abscos - abssin ;
      // Now we have the sin and cos of an angle between 0 and 90 degrees
      double sincand1 = abssin * cospiby8 - abscos * sinpiby8 ; // sin of an angle in (-22.5,+67.5 degrees)
      double coscand2 = abscos * cospiby8 - abssin * sinpiby8 ; // cos of an angle in (+22.5, 112.5 degrees)
                                                                // which is sin of an angle in (+67.5, -22.5 degrees)

      double trigang = fsel( coslarge , sincand1 , coscand2 ); // reduced-range item ready for 'arcsin'
      double ang = asin_small(trigang) ;
      // Now we have an angle which is piecewise-linear related to the wanted one, over the whole circle
      // Compute the multiplier and addend to stitch the angle back together
      // according as which octant we are in; this computation is interleavable
      // since both are branchless
      double km0 = fsel( sinang, 1.0, -1.0 ) ;
      double km1 = fsel( sinang, -1.0, 1.0 ) ;
      double kma = fsel(  coslarge , km0 , km1  ) ;
      double kmb = fsel(  coslarge , km1 , km0 ) ;

      double km  = fsel( cosang , kma , kmb ) ;

      double kaa = fsel( coslarge , piby8, pi3by8 ) ;
      double kab = fsel( coslarge , pi7by8, pi5by8 ) ;
      double ka  = fsel( cosang , kaa , kab ) * km0 ;

      // And stitch the angle back together
      return (ang*km) + ka ;

    }

