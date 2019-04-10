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
 #ifndef _PERIODICIMAGE_HPP
#define _PERIODICIMAGE_HPP

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/RunTimeGlobals.hpp>

// Need 'builtins' for fmadd/fmsub to hold off the optimiser

#if defined(PK_XLC)
#include <builtins.h>
#endif

#if !defined(PERFORMANCE_PERIODIC_IMAGE) || ! (defined(PK_AIX) || defined(PK_BGL))
static inline double NearestImageInFullyPeriodicLine(const double a, const double b, const double c, const double invL)
{
  double  d = b-a ;
  double ad = fabs(d) ;
  // calculate rounded number of box widths away
  int     n = ad * invL + 0.5 ;
  double result = (d >= 0.0) ? (b-n*c) : (b+n*c) ;
  return result ;
}
#else
extern double dk1 ;
#if !defined(LEAVE_SYMS_EXTERN)
double dk1 = 1.0 ; // The compiler does not know this is constant, so should not 'optimise' away the rounding below
#endif
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

// note ... 'FracScale' returns unsigned
static inline unsigned int FracScale(double n, double rd)
{
   double t = n*rd ;
   double ti = NearestInteger(t) ;
   double tr = t-ti ;                  // tr should be in (-0.5, 0.5)
   const double two10 = 1024.0 ;
   const double two32 = two10 * two10 * two10 * 4.0 ;
   double tri = tr*two32 ;
   int itri=tri ;
   unsigned int utri = itri ;
   BegLogLine(0)
     << "FracScale n=" << n
     << " rd=" << rd
     << " t=" << t
     << " ti=" << ti
     << " tri=" << tri
     << " itri=" << itri
     << " utri=" << utri
     << EndLogLine ;
   return utri ;             // should be a 32-bit integer representing the fractional position
}

// note ... 'FracScale' returns unsigned
static inline unsigned int FracScale_logged(double n, double rd)
{
   double t = n*rd ;
   double ti = NearestInteger(t) ;
   double tr = t-ti ;                  // tr should be in (-0.5, 0.5)
   const double two10 = 1024.0 ;
   const double two32 = two10 * two10 * two10 * 4.0 ;
   double tri = tr*two32 ;
   int itri=tri ;
   unsigned int utri = itri ;
   BegLogLine(0)
     << "FracScale n=" << n
     << " rd=" << rd
     << " t=" << t
     << " ti=" << ti
     << " tri=" << tri
     << " itri=" << itri
     << " utri=" << utri
     << EndLogLine ;
   return utri ;             // should be a 32-bit integer representing the fractional position
}

static inline double NearestImageInFullyPeriodicLine(
  const double a
  , const double b
  , const double Period
  , const double ReciprocalPeriod
  )
{
   const double d = b-a ; // 'Regular' distance between them, if small enough the result will be 'b'
   const double d_unit = d * ReciprocalPeriod ; // express with respect to unit periodicity,
                                                // for -0.5 < d_unit < 0.5 result will be 'b'
   const double d_unit_rounded = NearestInteger( d_unit ) ;
   const double result = b - d_unit_rounded * Period ;

   BegLogLine( 0 )
       << " NearestImageInFullyPeriodicLine:: "
       << " a=" << a
       << " b=" << b
       << " d=" << d
       << " d_unit=" << d_unit
       << " d_unit_rounded=" << d_unit_rounded
       << " Period=" << Period
       << " ReciprocalPeriod=" << ReciprocalPeriod
       << " result=" << result
       << EndLogLine;

   return result ;
}

static inline double NearestImageInFullyPeriodicLineMultiplier(
  const double a
  , const double b
  , const double Period
  , const double ReciprocalPeriod
  )
{
   const double d = b-a ; // 'Regular' distance between them, if small enough the result will be 'b'
   const double d_unit = d * ReciprocalPeriod ; // express with respect to unit periodicity,
                                                // for -0.5 < d_unit < 0.5 result will be 'b'
   const double d_unit_rounded = NearestInteger( d_unit ) ;

   return d_unit_rounded;
}


static inline double NearestDistanceInFullyPeriodicLine(
  const double a
  , const double b
  , const double Period
  , const double ReciprocalPeriod
  )
{
      const double d = b-a ; // 'Regular' distance between them, if small enough the result will be 'b'
   const double d_unit = d * ReciprocalPeriod ; // express with respect to unit periodicity,
                                                // for -0.5 < d_unit < 0.5 result will be 'b'
   const double d_unit_rounded = NearestInteger(d_unit) ;
   const double result = d - d_unit_rounded * Period ;
   return result ;
}

#endif

static inline double NearestVectorInFullyPeriodicLine(
  const double a
  , const double b
  , const double Period
  , const double ReciprocalPeriod
  )
{
   return a-NearestImageInFullyPeriodicLine(a,b,Period,ReciprocalPeriod) ;
}

static inline double NearestImageInPeriodicLine(const double a, const double b, const double c)
{
  double d=b-a ;
  double ad = fabs(d) ;
  double bound = ad+ad-c ;
  double rx = ( d >= 0.0 ) ? (b-c) : (b+c) ;
  double result = ( bound >= 0.0 ) ? rx : b ;
  return result ;
}

inline
void
NearestImageInPeriodicVolume(const XYZ &PositionA, const XYZ &PositionB, XYZ &Nearest)
  {
  double mX = NearestImageInFullyPeriodicLine(PositionA.mX, PositionB.mX, DynVarMgrIF.mDynamicBoxDimensionVector.mX, DynVarMgrIF.mDynamicBoxInverseDimensionVector.mX ) ;
  double mY = NearestImageInFullyPeriodicLine(PositionA.mY, PositionB.mY, DynVarMgrIF.mDynamicBoxDimensionVector.mY, DynVarMgrIF.mDynamicBoxInverseDimensionVector.mY ) ;
  double mZ = NearestImageInFullyPeriodicLine(PositionA.mZ, PositionB.mZ, DynVarMgrIF.mDynamicBoxDimensionVector.mZ, DynVarMgrIF.mDynamicBoxInverseDimensionVector.mZ ) ;

  Nearest.mX = mX ;
  Nearest.mY = mY ;
  Nearest.mZ = mZ ;
  }

inline
void
NearestImageInPeriodicVolumeMultiplier(const XYZ &PositionA, const XYZ &PositionB, XYZ &Nearest)
  {
  double mX = NearestImageInFullyPeriodicLineMultiplier(PositionA.mX, PositionB.mX, DynVarMgrIF.mDynamicBoxDimensionVector.mX, DynVarMgrIF.mDynamicBoxInverseDimensionVector.mX ) ;
  double mY = NearestImageInFullyPeriodicLineMultiplier(PositionA.mY, PositionB.mY, DynVarMgrIF.mDynamicBoxDimensionVector.mY, DynVarMgrIF.mDynamicBoxInverseDimensionVector.mY ) ;
  double mZ = NearestImageInFullyPeriodicLineMultiplier(PositionA.mZ, PositionB.mZ, DynVarMgrIF.mDynamicBoxDimensionVector.mZ, DynVarMgrIF.mDynamicBoxInverseDimensionVector.mZ ) ;

  Nearest.mX = mX ;
  Nearest.mY = mY ;
  Nearest.mZ = mZ ;
  }

inline
void
NearestVectorInPeriodicVolume(const XYZ &PositionA, const XYZ &PositionB, XYZ &Nearest)
  {
  double mX = NearestVectorInFullyPeriodicLine(PositionA.mX, PositionB.mX, DynVarMgrIF.mDynamicBoxDimensionVector.mX, DynVarMgrIF.mDynamicBoxInverseDimensionVector.mX ) ;
  double mY = NearestVectorInFullyPeriodicLine(PositionA.mY, PositionB.mY, DynVarMgrIF.mDynamicBoxDimensionVector.mY, DynVarMgrIF.mDynamicBoxInverseDimensionVector.mY ) ;
  double mZ = NearestVectorInFullyPeriodicLine(PositionA.mZ, PositionB.mZ, DynVarMgrIF.mDynamicBoxDimensionVector.mZ, DynVarMgrIF.mDynamicBoxInverseDimensionVector.mZ ) ;

  Nearest.mX = mX ;
  Nearest.mY = mY ;
  Nearest.mZ = mZ ;
  }

inline
double
NearestSquareDistanceInPeriodicVolume(const XYZ &PositionA, const XYZ &PositionB)
  {
  	double pX = NearestImageInFullyPeriodicLine(PositionA.mX, PositionB.mX,  DynVarMgrIF.mDynamicBoxDimensionVector.mX, DynVarMgrIF.mDynamicBoxInverseDimensionVector.mX ) ;
  	double pY = NearestImageInFullyPeriodicLine(PositionA.mY, PositionB.mY,  DynVarMgrIF.mDynamicBoxDimensionVector.mY, DynVarMgrIF.mDynamicBoxInverseDimensionVector.mY ) ;
  	double pZ = NearestImageInFullyPeriodicLine(PositionA.mZ, PositionB.mZ,  DynVarMgrIF.mDynamicBoxDimensionVector.mZ, DynVarMgrIF.mDynamicBoxInverseDimensionVector.mZ ) ;
  	double vX = pX-PositionA.mX ;
  	double vY = pY-PositionA.mY ;
  	double vZ = pZ-PositionA.mZ ;
  	return vX*vX + vY*vY + vZ*vZ ;
  }
#endif
