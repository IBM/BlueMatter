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
 /* $Id: random.cpp 9996 2006-09-21 16:34:39Z tjcw $ */

THIS FILE SHOULD NOT BE COMPILED

#include <pk/platform.hpp>
#include <BlueMatter/math.hpp>


#include <math.h>

#ifndef NULL
#define NULL (void * 0)
#endif

/* ----------------------- for ran2() ----------------------------- */
#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

double
ran2(long *idum)
/* ****************************************************************
 * returns a uniform random deviate between 0.0 and 1.0 (exclusive
 * of the endpoint values).
 * **************************************************************** */
  {
  int j;
  long k;
  static long idum2=123456789;
  static long iy=0;
  static long iv[NTAB];
  double temp;

  if (*idum <= 0) {
          if (-(*idum) < 1) *idum=1;
          else *idum = -(*idum);
          idum2=(*idum);
          for (j=NTAB+7;j>=0;j--) {
                  k=(*idum)/IQ1;
                  *idum=IA1*(*idum-k*IQ1)-k*IR1;
                  if (*idum < 0) *idum += IM1;
                  if (j < NTAB) iv[j] = *idum;
          }
          iy=iv[0];
  }
  k=(*idum)/IQ1;
  *idum=IA1*(*idum-k*IQ1)-k*IR1;
  if (*idum < 0) *idum += IM1;
  k=idum2/IQ2;
  idum2=IA2*(idum2-k*IQ2)-k*IR2;
  if (idum2 < 0) idum2 += IM2;
  j=iy/NDIV;
  iy=iv[j]-idum2;
  iv[j] = *idum;
  if (iy < 1) iy += IMM1;
  if ((temp=AM*iy) > RNMX) return RNMX;
  else return temp;
  }

double
ran(void)
  {
  static int once = 0;
  static long idum;
  if (!once)
    {
    Platform::Clock::TimeValue cv = Platform::Clock::GetTimeValue();
#if 0
    struct timeval tv;
    gettimeofday(&tv, NULL);
    idum = -tv.tv_sec;
#endif
    idum = cv.Seconds;
    once = 1;
    }
  return ran2(&idum);
  }

double
gasdev(long *idum)
  {
  double ran2(long *idum);
  static int iset=0;
  static double gset;
  double fac,rsq,v1,v2;

  if( iset == 0 )
    {
    do
      {
      v1=2.0*ran2(idum)-1.0;
      v2=2.0*ran2(idum)-1.0;
      rsq=v1*v1+v2*v2;
      } while (rsq >= 1.0 || rsq == 0.0);
    fac=sqrt(-2.0*Math::hlog(rsq)/rsq);
    gset=v1*fac;
    iset=1;
    return v2*fac;
    }
  else
    {
    iset=0;
    return gset;
    }
  }

double
normal(void)
  {
  static int once = 0;
  static long idum;
  if (!once)
    {
    Platform::Clock::TimeValue cv = Platform::Clock::GetTimeValue();
#if 0
    struct timeval tv;
    gettimeofday(&tv, NULL);
    idum = -tv.tv_sec;
#endif
    idum = cv.Seconds;
    once = 1;
    }
  return gasdev(&idum);
  }







