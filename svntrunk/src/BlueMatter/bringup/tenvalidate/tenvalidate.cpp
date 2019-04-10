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
 * Function and performance validation for Double Hummer 10-way unroll
 */
#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <float.h>
#include <math.h>

#include <BlueMatter/fpexploit.hpp>

#if defined(ARCH_HAS_FSEL)
#include <builtins.h>
#define fsel(a, x, y) __fsel((a),(x),(y))
#else
#define fsel(a, x, y) ( (a) >= 0.0 ? (x) : (y) )
#endif

#include <BlueMatter/fpatrig.hpp>

#if defined(PK_BGL)
#define AlignForBGL __align(16)
#else
#define AlignForBGL
#endif

#if defined(PK_BGL)
#define printf s0printf
#endif
/*
 * 10-way unroll evaluation fragments
 */
void tenroot(double* f, const double* x)
{
   QuadWordAligned(f) ;
   QuadWordAligned(x) ;
   double x0 = x[0] ;
   double x1 = x[1] ;
   double x2 = x[2] ;
   double x3 = x[3] ;
   double x4 = x[4] ;
   double x5 = x[5] ;
   double x6 = x[6] ;
   double x7 = x[7] ;
   double x8 = x[8] ;
   double x9 = x[9] ;
   double r0 = 1.0/sqrt(x0) ;
   double r1 = 1.0/sqrt(x1) ;
   double r2 = 1.0/sqrt(x2) ;
   double r3 = 1.0/sqrt(x3) ;
   double r4 = 1.0/sqrt(x4) ;
   double r5 = 1.0/sqrt(x5) ;
   double r6 = 1.0/sqrt(x6) ;
   double r7 = 1.0/sqrt(x7) ;
   double r8 = 1.0/sqrt(x8) ;
   double r9 = 1.0/sqrt(x9) ;
   f[0] = r0 ;
   f[1] = r1 ;
   f[2] = r2 ;
   f[3] = r3 ;
   f[4] = r4 ;
   f[5] = r5 ;
   f[6] = r6 ;
   f[7] = r7 ;
   f[8] = r8 ;
   f[9] = r9 ;
}

void tenrecip(double* f, const double* x)
{
   QuadWordAligned(f) ;
   QuadWordAligned(x) ;
   double x0 = x[0] ;
   double x1 = x[1] ;
   double x2 = x[2] ;
   double x3 = x[3] ;
   double x4 = x[4] ;
   double x5 = x[5] ;
   double x6 = x[6] ;
   double x7 = x[7] ;
   double x8 = x[8] ;
   double x9 = x[9] ;
   double r0 = 1.0/x0 ;
   double r1 = 1.0/x1 ;
   double r2 = 1.0/x2 ;
   double r3 = 1.0/x3 ;
   double r4 = 1.0/x4 ;
   double r5 = 1.0/x5 ;
   double r6 = 1.0/x6 ;
   double r7 = 1.0/x7 ;
   double r8 = 1.0/x8 ;
   double r9 = 1.0/x9 ;
   f[0] = r0 ;
   f[1] = r1 ;
   f[2] = r2 ;
   f[3] = r3 ;
   f[4] = r4 ;
   f[5] = r5 ;
   f[6] = r6 ;
   f[7] = r7 ;
   f[8] = r8 ;
   f[9] = r9 ;
}

void tensqrt(double* f, const double* x)
{
   QuadWordAligned(f) ;
   QuadWordAligned(x) ;
   double x0 = x[0] ;
   double x1 = x[1] ;
   double x2 = x[2] ;
   double x3 = x[3] ;
   double x4 = x[4] ;
   double x5 = x[5] ;
   double x6 = x[6] ;
   double x7 = x[7] ;
   double x8 = x[8] ;
   double x9 = x[9] ;
   double r0 = sqrt(x0) ;
   double r1 = sqrt(x1) ;
   double r2 = sqrt(x2) ;
   double r3 = sqrt(x3) ;
   double r4 = sqrt(x4) ;
   double r5 = sqrt(x5) ;
   double r6 = sqrt(x6) ;
   double r7 = sqrt(x7) ;
   double r8 = sqrt(x8) ;
   double r9 = sqrt(x9) ;
   f[0] = r0 ;
   f[1] = r1 ;
   f[2] = r2 ;
   f[3] = r3 ;
   f[4] = r4 ;
   f[5] = r5 ;
   f[6] = r6 ;
   f[7] = r7 ;
   f[8] = r8 ;
   f[9] = r9 ;
}

void tendivide(double* f, const double* y, const double* x)
{
   QuadWordAligned(f) ;
   QuadWordAligned(y) ;
   QuadWordAligned(x) ;
   double x0 = x[0] ;
   double x1 = x[1] ;
   double x2 = x[2] ;
   double x3 = x[3] ;
   double x4 = x[4] ;
   double x5 = x[5] ;
   double x6 = x[6] ;
   double x7 = x[7] ;
   double x8 = x[8] ;
   double x9 = x[9] ;
   double y0 = y[0] ;
   double y1 = y[1] ;
   double y2 = y[2] ;
   double y3 = y[3] ;
   double y4 = y[4] ;
   double y5 = y[5] ;
   double y6 = y[6] ;
   double y7 = y[7] ;
   double y8 = y[8] ;
   double y9 = y[9] ;
   double r0 = y0/x0 ;
   double r1 = y1/x1 ;
   double r2 = y2/x2 ;
   double r3 = y3/x3 ;
   double r4 = y4/x4 ;
   double r5 = y5/x5 ;
   double r6 = y6/x6 ;
   double r7 = y7/x7 ;
   double r8 = y8/x8 ;
   double r9 = y9/x9 ;
   f[0] = r0 ;
   f[1] = r1 ;
   f[2] = r2 ;
   f[3] = r3 ;
   f[4] = r4 ;
   f[5] = r5 ;
   f[6] = r6 ;
   f[7] = r7 ;
   f[8] = r8 ;
   f[9] = r9 ;
}

void tenatrig(double* f, const double* s, const double* c)
{
   QuadWordAligned(f) ;
   QuadWordAligned(c) ;
   QuadWordAligned(s) ;
   double s0 = s[0] ;
   double s1 = s[1] ;
   double s2 = s[2] ;
   double s3 = s[3] ;
   double s4 = s[4] ;
   double s5 = s[5] ;
   double s6 = s[6] ;
   double s7 = s[7] ;
   double s8 = s[8] ;
   double s9 = s[9] ;
   double c0 = c[0] ;
   double c1 = c[1] ;
   double c2 = c[2] ;
   double c3 = c[3] ;
   double c4 = c[4] ;
   double c5 = c[5] ;
   double c6 = c[6] ;
   double c7 = c[7] ;
   double c8 = c[8] ;
   double c9 = c[9] ;
   double f0 = atrig(s0,c0) ;
   double f1 = atrig(s1,c1) ;
   double f2 = atrig(s2,c2) ;
   double f3 = atrig(s3,c3) ;
   double f4 = atrig(s4,c4) ;
   double f5 = atrig(s5,c5) ;
   double f6 = atrig(s6,c6) ;
   double f7 = atrig(s7,c7) ;
   double f8 = atrig(s8,c8) ;
   double f9 = atrig(s9,c9) ;
   f[0] = f0 ;
   f[1] = f1 ;
   f[2] = f2 ;
   f[3] = f3 ;
   f[4] = f4 ;
   f[5] = f5 ;
   f[6] = f6 ;
   f[7] = f7 ;
   f[8] = f8 ;
   f[9] = f9 ;
}

//void pairatrig(double* f, const double* s, const double* c)
//{
//   QuadWordAligned(f) ;
//   QuadWordAligned(c) ;
//   QuadWordAligned(s) ;
//   double s0 = s[0] ;
//   double s1 = s[1] ;
//   double c0 = c[0] ;
//   double c1 = c[1] ;
//   double f0 = atrig(s0,c0) ;
//   double f1 = atrig(s1,c1) ;
//   f[0] = f0 ;
//   f[1] = f1 ;
//}

#include <BlueMatter/fpformat.hpp>

/*
 * 'Normal' cases for square root and reciprocal square root
 */
static double AlignForBGL rootsources[] = {
   1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0
} ;

/*
 * 'Special' cases for square root and reciprocal square root, plus a few 'normal' ones
 */
static double AlignForBGL rootsources2[] = {
   -2.0, -1.0, 0.0, 1.0, 2.0, -2.0, -1.0, 0.0, 1.0, 2.0
   } ;

static int AlignForBGL rootsources3[] = {
   0x00000000, 0x00000001,
   0x00000000, 0x00000010,
   0x00000000, 0x00000100,
   0x00000000, 0x00010000,
   0x00000000, 0x01000000,
   0x00000000, 0x10000000,
   0x00000010, 0x00000000,
   0x00001000, 0x00000000,
   0x00010000, 0x00000000,
   0x00100000, 0x00000000,
   } ;
/*
 * 'Normal' cases for divisions and reciprocals
 */
static double AlignForBGL nsources[] = {
   1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0
   } ;

static double AlignForBGL dsources[] = {
   10.0, 9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0
   } ;

/*
 * 'Special' cases for divisions and reciprocals
 */
static int AlignForBGL nsources2[] = {
   0x3ff00000, 0x00000000,
   0x3ff00000, 0x00000000,
   0x001A63D6, 0x5ED4C7AC,
   0x001A63D6, 0x5ED4C7AC,
   0x3ffA63D6, 0x5ED4C7AC,
   0x3ffA63D6, 0x5ED4C7AC,
   0x00201672, 0x00202CE4,
   0x00201672, 0x00202CE4,
   0x3ff01672, 0x00202CE4,
   0x3ff01672, 0x00202CE4
   } ;

static int AlignForBGL dsources2[] = {
   0x00000000, 0x00000000,
   0x00000000, 0x00000000,
   0x3CF9429B, 0x20328536,
   0x3CF9429B, 0x20328536,
   0x3CF9429B, 0x20328536,
   0x3CF9429B, 0x20328536,
   0x03F95B67, 0xED32B6CF,
   0x03F95B67, 0xED32B6CF,
   0x03F95B67, 0xED32B6CF,
   0x03F95B67, 0xED32B6CF
   } ;

/*
 * More cases for divisions and reciprocals, which should be normal but test
 * things special to Double Hummer.
 */
static double AlignForBGL nsources3[] = {
   1.0e-200,
   1.0e+200,
   1.0-DBL_EPSILON,
   1.0+DBL_EPSILON,
   1.0-FLT_EPSILON,
   1.0+FLT_EPSILON,
   1.0-2.0*FLT_EPSILON,
   1.0+2.0*FLT_EPSILON,
   0.0,
   1.0
   } ;

static double AlignForBGL dsources3[] = {
   1.0e-200,
   1.0e+200,
   1.0-DBL_EPSILON,
   1.0+DBL_EPSILON,
   1.0-FLT_EPSILON,
   1.0+FLT_EPSILON,
   1.0-2.0*FLT_EPSILON,
   1.0+2.0*FLT_EPSILON,
   0.0,
   0.0
   } ;

// sqrt(3), originally from 'bc' and sqrt(30000000000000000000...)
#define M_SQRT3 1.732050807568877293527446341505872366942805253810
/*
 * Sines and cosines of angles, to check if the angle is recovered correctly
 */
static double AlignForBGL sines[] = {
   1.0
   , 0.0
   , -1.0
   , 0.0
   ,                           // 0,90,180,270 degrees
     M_SQRT1_2
   , M_SQRT1_2
   , -M_SQRT1_2
   , -M_SQRT1_2
      // 45,135,225,315 degrees
   , 0.5                                            // 30 degrees
   , 0.5*M_SQRT3                                   // 60 degrees
   } ;
static double AlignForBGL cosines[] = {
     0.0
   , 1.0
   , 0.0
   , -1.0

   , -M_SQRT1_2
   , M_SQRT1_2
   , M_SQRT1_2
   , -M_SQRT1_2

   , 0.5*M_SQRT3
   , 0.5
   } ;
//static double AlignForBGL sines2[] = {
//   0.0, 1.0,
//   0.0, 1.0,
//   0.0, 1.0,
//   0.0, 1.0,
//   0.0, 1.0,
//   } ;
//static double AlignForBGL cosines2[] = {
//   1.0, 0.0,
//   1.0, 0.0,
//   1.0, 0.0,
//   1.0, 0.0,
//   1.0, 0.0,
//   } ;
int pipedrain(unsigned int x)
{
   int result = 0 ;
   for (unsigned int a=0; a<x; a+=1)
   {
      result += a ;
   } /* endfor */
   return result ;
}

void fpsequence(
     double* recrootresults
   , double* sqrtresults
   , double* recipresults
   , double* divideresults
   , double* angresults
   ) {
  (void) pipedrain(20) ;
  tenroot(recrootresults, rootsources) ;
  (void) pipedrain(20) ;
  tenroot(recrootresults+10, rootsources2) ;
  (void) pipedrain(20) ;
  tenroot(recrootresults+20, (const double *) rootsources3) ;

  (void) pipedrain(20) ;
  tensqrt(sqrtresults, rootsources) ;
  (void) pipedrain(20) ;
  tensqrt(sqrtresults+10, rootsources2) ;
  (void) pipedrain(20) ;
  tensqrt(sqrtresults+20, (const double *)rootsources3) ;

  (void) pipedrain(20) ;
  tendivide(divideresults, nsources,dsources) ;
  (void) pipedrain(20) ;
  tendivide(divideresults+10, (const double *)nsources2,(const double *)dsources2) ;
  (void) pipedrain(20) ;
  tendivide(divideresults+20, nsources3,dsources3) ;

  (void) pipedrain(20) ;
  tenrecip(recipresults, dsources) ;
  (void) pipedrain(20) ;
  tenrecip(recipresults+10, (const double *)dsources2) ;
  (void) pipedrain(20) ;
  tenrecip(recipresults+20, (const double *)dsources3) ;

  (void) pipedrain(20) ;
  tenatrig(angresults, sines, cosines) ;
//  (void) pipedrain(20) ;
//  tenatrig(angresults, sines2, cosines2) ;

  (void) pipedrain(20) ;
      }

void show_results(const char * tag, int count, const double* results)
{
  for (int x=0; x<count; x+=1)
  {
     char bufferz[30] ;
     int i0 = ((int *)(results+x))[0] ;
     int i1 = ((int *)(results+x))[1] ;
     fpformat(bufferz, results[x], 30) ;
     printf("%s[%d] 0x%08x%08x %s\n", tag, x, i0, i1, bufferz) ;
  } /* endfor */
}

int PkMain(int arg, char ** argv, char ** envp)
{
  double recrootresults[30] ;
  double sqrtresults[30] ;
  double divideresults[30] ;
  double recresults[30] ;
//double angresults[20] ;
  double angresults[10] ;

  printf("fp start\n") ;
  fpsequence(recrootresults,sqrtresults,recresults,divideresults,angresults) ; // Warm up the caches
  pipedrain(40) ;
  fpsequence(recrootresults,sqrtresults,recresults,divideresults,angresults) ; // Repeat for cycle counting
  printf("fp end\n") ;
  show_results("sqrt   ",30,sqrtresults) ;
  show_results("recroot",30,recrootresults) ;
//show_results("angle  ",20,angresults) ;
  show_results("angle  ",10,angresults) ;
  show_results("divide ",30,divideresults) ;
  show_results("recip  ",30,recresults) ;
  printf("---\n") ;
  return 0 ;
}
