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
 #if !defined(INCLUDE_MYLIB_HPP)
#define INCLUDE_MYLIB_HPP

// #include <math.h>
#include <pk/pkmath.h>

// Select the implementation for error function and derivative
// Options are 'Abramowitz and Stegun' (traditional for MD)
//             'Chebyshev' (practically double precision, vectorisable, but a lot of arithmetic)
//             'Spline' (meeting MD accuracy spec, vectorisable, much less arithmetic)
// Select 'Abramowitz and Stegun' if not overridden
#if !defined(CHEBYSHEV_ERFCC) && !defined(SPLINE_ERFCC) && !defined(ABRAMOWITZ_ERFCC)
#define SPLINE_ERFCC 1
#endif
// 'fsel' is a built-in instruction on PPCGR and above, sometimes we want to force its use
#if ( defined(ARCH_HAS_FSEL) && defined( PK_XLC ) )
#include <builtins.h>
#define fsel(a, x, y) __fsel((a),(x),(y))
#else
#define fsel(a, x, y) ( (a) >= 0.0 ? (x) : (y) )
#endif


#if defined(MSD_COMPILE_DATA_ONLY)
// Put in 'compile-time' versions of sin and cos
inline double compile_time_cos(double X)
{
  double X2 = X*X ;
  double t33 = (1.0       )/(33.0*34.0) ;
  double t31 = (1.0-X2*t33)/(31.0*32.0) ;
  double t29 = (1.0-X2*t31)/(29.0*30.0) ;
  double t27 = (1.0-X2*t29)/(27.0*28.0) ;
  double t25 = (1.0-X2*t27)/(25.0*26.0) ;
  double t23 = (1.0-X2*t25)/(23.0*24.0) ;
  double t21 = (1.0-X2*t23)/(21.0*22.0) ;
  double t19 = (1.0-X2*t21)/(19.0*20.0) ;
  double t17 = (1.0-X2*t19)/(17.0*18.0) ;
  double t15 = (1.0-X2*t17)/(15.0*16.0) ;
  double t13 = (1.0-X2*t15)/(13.0*14.0) ;
  double t11 = (1.0-X2*t13)/(11.0*12.0) ;
  double t09 = (1.0-X2*t11)/( 9.0*10.0) ;
  double t07 = (1.0-X2*t09)/( 7.0* 8.0) ;
  double t05 = (1.0-X2*t07)/( 5.0* 6.0) ;
  double t03 = (1.0-X2*t05)/( 3.0* 4.0) ;
  double t01 = (1.0-X2*t03)/( 1.0* 2.0) ;
  return 1.0-X2*t01 ;
}

inline double compile_time_sin(double X)
{
  double X2 = X*X ;
  double t30 = (1.0       )/(30.0*31.0) ;
  double t28 = (1.0-X2*t30)/(28.0*29.0) ;
  double t26 = (1.0-X2*t28)/(26.0*27.0) ;
  double t24 = (1.0-X2*t26)/(24.0*25.0) ;
  double t22 = (1.0-X2*t24)/(22.0*23.0) ;
  double t20 = (1.0-X2*t22)/(20.0*21.0) ;
  double t18 = (1.0-X2*t20)/(18.0*19.0) ;
  double t16 = (1.0-X2*t18)/(16.0*17.0) ;
  double t14 = (1.0-X2*t16)/(14.0*15.0) ;
  double t12 = (1.0-X2*t14)/(12.0*13.0) ;
  double t10 = (1.0-X2*t12)/(10.0*11.0) ;
  double t08 = (1.0-X2*t10)/( 8.0* 9.0) ;
  double t06 = (1.0-X2*t08)/( 6.0* 7.0) ;
  double t04 = (1.0-X2*t06)/( 4.0* 5.0) ;
  double t02 = (1.0-X2*t04)/( 2.0* 3.0) ;
  return X*(1.0-X2*t02) ;
  
}

// #undef cos
// #undef sin
// 
// #define cos compile_time_cos
// #define sin compile_time_sin

#endif

#define MSDAngleMacro(Original,Degrees,Radians) (Radians)
#define MSDCosMacro(Originial,Degrees,Radians,Cosine) (Cosine)
#define MSDSinMacro(Original,Degrees,Radians,Sine) (Sine)

#if ( defined(PERFORMANCE_ERFC_VIA_ESTIMATE) && defined( PK_XLC ) )
#include <builtins.h>
static inline double better_reciprocal(double a, double x0)
{
   double f0 = a*x0 - 1.0 ;
   double x1 = x0 - x0 * f0 ;
   return x1 ;
}

static inline double recip(double a)
{
   double x0 = __fres(a) ; // take it as read that a is in single-precision range
   double x1 = better_reciprocal(a,x0) ;
   double x2 = better_reciprocal(a,x1) ;
   double x3 = better_reciprocal(a,x2) ;
   return x3 ;
}
#else
static inline double recip(double X)
{
   return 1.0 / X ;
}
#endif

//const  double PI     = M_PI;       // why should we bother????? 3.1415926535;
//const  double sqrtPI = 1.7724539;  // sqrt (PI)
//const  double E      = 2.718281828;
static const  double EPS    = 1.0e-15;

inline double sqr(double x) { return x*x; }
inline double cube(double x) { return x*x*x;}
// inline double min(double x, double y) { return x>=y?y:x; }
#undef min
#undef max
inline double min(double x, double y) { return fsel(x-y,y,x) ; }
inline double min(double x, int y) { return x>=y?y:x; }
inline double min(int x, double y) { return x>=y?y:x; }
inline int    min(int x, int y) { return x>=y?y:x; }
// inline double max(double x, double y) { return x>=y?x:y; }
inline double max(double x, double y) { return fsel(x-y,x,y) ;  }
inline double max(double x, int y) { return x>=y?x:y; }
inline double max(int x, double y) { return x>=y?x:y; }
inline int    max(int x, int y) { return x>=y?x:y; }
inline int    sgn(double x) { return x>=0.0?1:-1; }
inline int    is_almost_zero(double epsilon) { return fabs(epsilon) < 1.0e-15; }

template<class T>
class Pair
{
  T firstElement, secondElement;

public:

  Pair() {}

  Pair(T f, T s)
    : firstElement(f), secondElement(s) {}

  T first() const
    { return firstElement; }

  T second() const
    { return secondElement; }

  ~Pair() {}

};

struct Dimension
{
  double length, width, height;

  Dimension(double size = 0)
    : length(size), width(size), height(size) {}

  Dimension(double l, double w, double h)
    : length(l), width(w), height(h) {}

};


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

/*
 * 'SwitchFunction' is the function as follows ...
 *    x <= MinRadius                   ---> +1
 *    MinRadius < x < MaxRadius        ---> smooth variation from +1 to 0
 *    x >= MaxRadius                   ---> 0
 *    x = MinRadius                    ---> first and second derivatives = 0
 *    x = MaxRadius                    ---> first and second derivatives = 0
 *    MinRadius < x < MaxRadius        ---> lowest-order polynomial satisfying the above criteria
 *  The function and its derivative are returned
 *
 *  (This ends up being a 5th order polynomial in the interval)
 */
class SwitchFunctionRadii
{
 public:

//   double MinRadius ;
//   double MaxRadius ;
   double MinRadiusSquared ;
   double MaxRadiusSquared ;
   double Scale ;
   double MidPointScaled ;

   double DerivativeK1 ;
   double DerivativeK2 ;

   // aMinRadius needs to be < aMaxRadius, but this is not checked
   void SetRadii(double aMinRadius, double aMaxRadius)
   {
      if ( aMinRadius > 1e99 ) aMaxRadius = 2.0*aMinRadius ; // For Frank , no switching if lower bound is large enough
//      MinRadius = aMinRadius ;
//      MaxRadius = aMaxRadius ;
      MinRadiusSquared = aMinRadius*aMinRadius ;
      MaxRadiusSquared = aMaxRadius*aMaxRadius ;
      const double MidPoint = 0.5 * ( aMinRadius + aMaxRadius ) ;
      const double HalfRange = 0.5 * ( aMaxRadius - aMinRadius ) ;
      Scale = 1.0 / HalfRange ;
      MidPointScaled = MidPoint*Scale ;
      DerivativeK1 = Scale * ( -15.0 / 16.0 ) ;
      DerivativeK2 = Scale * (  30.0 / 16.0 ) ;
   }
#if 0
// The existence of constructors makes the compiler put things like RunTimeGlobals into initialised data. Not do it.
   // Give it a constructor which sets the radii
   SwitchFunctionRadii(double aMinRadius, double aMaxRadius)
   {
      SetRadii(aMinRadius,aMaxRadius) ;
   }
   // And a default constructor which will give a zero value for the function everywhere
   SwitchFunctionRadii()
   {
      SetRadii(-2.0,-1.0) ;
   }
#endif

   // Accessors for pairwise-distance-based optimisations to look at things
   double GetMinRadiusSquared(void) const
   {
      return MinRadiusSquared ;
   }

   double GetMaxRadiusSquared(void) const
   {
      return MaxRadiusSquared ;
   }

} ;
class SwitchFunction
{
   double Scale ;
   double MidPointScaled ;

   double DerivativeK1 ;
   double DerivativeK2 ;

   double RangeScale(double x) const { return x*Scale - MidPointScaled ; } ;

public:
  SwitchFunction(){}

  inline
  void
  Set( const SwitchFunctionRadii& sfr )
    {
    Scale          = sfr.Scale;
    MidPointScaled = sfr.MidPointScaled;
    DerivativeK1   = sfr.DerivativeK1;
    DerivativeK2   = sfr.DerivativeK2;
    }
//    SwitchFunction(const SwitchFunctionRadii& sfr):
//        Scale(sfr.Scale)
//      , MidPointScaled(sfr.MidPointScaled)
//      , DerivativeK1(sfr.DerivativeK1)
//      , DerivativeK2(sfr.DerivativeK2)
//    { }

   void Evaluate(const double xu, double& Function, double& Derivative) const
   {

     // x0 is the input value, rescaled to (-1,1) at the cutoff radii
     const double x0=RangeScale(xu) ;
     // x1 is +1 or -1, according as whether the input value was above or below half way
     const double x1 = fsel(x0,1.0,-1.0) ;
     // x is the rescaled input value, but limited to (-1,1) if outside the cutoff radii
     const double x = fsel(x0*x0-1.0, x1, x0) ;
     // Now we can work out the smooth polynomial. With this rescaling, the function is even
     // and the derivative is odd.
     const double xx=x*x ;
     const double fs0=(-3.0/16.0)*xx + (10.0/16.0) ;
     const double fs1=fs0*xx + (-15.0/16) ;
     const double fs2=fs1*x + (8.0/16.0) ;

     //const double ds0=(-15.0*Scale()/16.0)*xx + (30.0*Scale()/16.0) ;
     const double ds0=DerivativeK1*xx + DerivativeK2 ;
     const double ds1=ds0*xx + DerivativeK1 ;

     Function = fs2 ;
     Derivative = ds1 ;
   }

   template <int FullyOn> void Evaluate(const double xu, double& Function, double& Derivative) const
   {

     if (FullyOn)
     {
        Function = 1.0 ;
        Derivative = 0.0 ;
     }
     else
     {
        // x0 is the input value, rescaled to (-1,1) at the cutoff radii
        const double x0=RangeScale(xu) ;
        // x1 is +1 or -1, according as whether the input value was above or below half way
        const double x1 = fsel(x0,1.0,-1.0) ;
        // x is the rescaled input value, but limited to (-1,1) if outside the cutoff radii
        const double x = fsel(x0*x0-1.0, x1, x0) ;
        // Now we can work out the smooth polynomial. With this rescaling, the function is even
        // and the derivative is odd.
        const double xx=x*x ;
        const double fs0=(-3.0/16.0)*xx + (10.0/16.0) ;
        const double fs1=fs0*xx + (-15.0/16) ;
        const double fs2=fs1*x + (8.0/16.0) ;

        //const double ds0=(-15.0*Scale()/16.0)*xx + (30.0*Scale()/16.0) ;
        const double ds0=DerivativeK1*xx + DerivativeK2 ;
        const double ds1=ds0*xx + DerivativeK1 ;

        Function = fs2 ;
        Derivative = ds1 ;
     } /* endif */
   }

} ;

#if defined(CHEBYSHEV_ERFCC)

class ChebyshevPairEvaluator
{
  public:
  enum {
     k_Terms = 16
     } ;

        class DoublePair
        {
                public:
                double pa ;
                double pb ;
        } ;

        class DoublePairArray
        {
                public:
                DoublePair c[k_Terms] ;
        } ;


  static void RawEvaluatePair(double& f, double& df, double x, const DoublePairArray& cp)
  {
     double dppa = 0.0 ;
     double dpa = 0.0 ;
     double dppb = 0.0 ;
     double dpb = 0.0 ;
     for (int j=0; j<k_Terms-1; j+=1)
     {
        double da=(2.0*x)*dpa - dppa + cp.c[j].pa ;
        double db=(2.0*x)*dpb - dppb + cp.c[j].pb ;
        dppa = dpa ;
        dppb = dpb ;
        dpa = da ;
        dpb = db ;
     } /* endfor */
     // Term 0 is a special case; POWER 'multiply-add' makes this same efficiency as rewriting the table
     double resulta = x*dpa - dppa + 0.5*(cp.c[k_Terms-1]).pa ;
     double resultb = x*dpb - dppb + 0.5*(cp.c[k_Terms-1]).pb ;
     f = resulta ;
     df = resultb ;
  }


} ;

class ErfEvaluator: public ChebyshevPairEvaluator
{
   public:
   enum {
      k_Slices = 8 ,
      } ;

        class CTable {
                public:
                DoublePairArray SliceTable[k_Slices] ;
        } ;

        static const CTable ChebyshevTable ;

        static double dk1 ; // The compiler does not know this is constant, so should not 'optimise' away the rounding below
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

        static void Evaluate(const double x, double& f, double& df)
        {

           double xam = fabs(x) - 0.5 ;

           double xi = NearestInteger(xam) ;
           double xf = xam-xi ;                 // -0.5 < x < 0.5
           int ix = (int) xi ;
           int ixmask = ix & 7 ;
           double r0 ;
           double dr0 ;
           RawEvaluatePair(r0, dr0, 2.0*xf, ChebyshevTable.SliceTable[ixmask]) ;
           double  r1 = fsel(xi-7.5,-1.0,  r0) ;
           double dr1 = fsel(xi-7.5, 0.0, dr0) ;
           double   m = fsel(x,1.0, -1.0) ;
           double  r2 = 1.0+m*r1 ;

           f = r2    ;
           df = dr1 ;

        }


} ;


#ifndef MSD_COMPILE_CODE_ONLY
const ErfEvaluator::CTable ErfEvaluator::ChebyshevTable = {
        {
                {

 {  {                  -1.9514453114346613e-14 ,                  -4.3962027206143962e-14 }
,   {                   1.3625665209647924e-13 ,                  -1.1610210709915711e-12 }
,   {                    2.229902591391045e-12 ,                   7.5864104901966937e-12 }
,   {                  -2.1427408195614467e-11 ,                   1.1479391368134277e-10 }
,   {                  -2.0634390485502084e-10 ,                  -1.0209291828992976e-09 }
,   {                   2.7257129691783295e-09 ,                  -8.9643378999395747e-09 }
,   {                   1.4115809274810406e-08 ,                   1.0800758958423388e-07 }
,   {                  -2.7818478883356451e-07 ,                   4.9920479599323502e-07 }
,   {                  -5.2333748523420065e-07 ,                  -8.7939056530898302e-06 }
,   {                    2.211114704099522e-05 ,                  -1.4154244790564384e-05 }
,   {                  -1.8363892921493974e-05 ,                   0.00052187362333079546 }
,   {                   -0.0012919410465849694 ,                  -0.00038143210322044387 }
,   {                     0.004492934887683828 ,                    -0.020149183122028715 }
,   {                     0.049552626796204341 ,                     0.053533786548985489 }
,   {                     -0.42582445804381047 ,                      0.37627183124760605 }
,   {                     -0.93926583578230194 ,                      -1.6497640456262563 }
} }
,{ {  {                  -5.0317730474937121e-16 ,                  -4.2117485562976088e-14 }
,     {                   9.4040069643033129e-14 ,                  -2.7959038508193287e-14 }
,     {                  -5.4676286926192122e-13 ,                   5.2241264144468789e-12 }
,     {                  -7.8746039930688689e-12 ,                  -2.8459628240128098e-11 }
,     {                   1.1428281965722648e-10 ,                  -3.7275686525285883e-10 }
,     {                   7.5247753102189958e-11 ,                    4.999984436677837e-09 }
,     {                  -1.1752407126361825e-08 ,                   2.6371532588347393e-09 }
,     {                   7.3301093509556885e-08 ,                  -4.1808667211234786e-07 }
,     {                   4.6810084713974775e-07 ,                   2.3482721455646551e-06 }
,     {                  -8.8537423943830358e-06 ,                   1.2688737047800589e-05 }
,     {                   3.2638635138919629e-05 ,                  -0.00021014154531962822 }
,     {                   0.00029384471334866456 ,                   0.00066546143982619316 }
,     {                   -0.0041797192715572768 ,                    0.0044913738682590045 }
,     {                     0.023555662412541055 ,                    -0.049491169818861133 }
,     {                    -0.072164111860376176 ,                      0.19293667316858745 }
,     {                      -1.8857045114727804 ,                     -0.33814761726036585 }
} }
,{ {  {                   1.1536030588919383e-15 ,                  -4.5290945318974183e-15 }
,     {                  -2.5831909518634811e-15 ,                   6.9035600532037278e-14 }
,     {                  -1.2219969549456367e-13 ,                  -1.4918778783625236e-13 }
,     {                    1.650088654139529e-12 ,                  -6.2853485651852727e-12 }
,     {                  -4.1430599274545238e-12 ,                   7.9055067610861132e-11 }
,     {                   -1.303276091975957e-10 ,                  -1.8857998537318434e-10 }
,     {                   1.8722149256193656e-09 ,                  -5.1340493002929665e-09 }
,     {                  -9.3478974795511597e-09 ,                    6.721115733692397e-08 }
,     {                  -4.9199521904598378e-08 ,                  -3.0426676864593005e-07 }
,     {                   1.2964140790298608e-06 ,                  -1.3103754559918305e-06 }
,     {                  -1.2343188523307585e-05 ,                   3.0809671128070729e-05 }
,     {                   7.5321057602619065e-05 ,                  -0.00024817414592214355 }
,     {                  -0.00032191106470368769 ,                    0.0012359465927699756 }
,     {                   0.00097018407704014555 ,                   -0.0041111069223663957 }
,     {                   -0.0019935206572262573 ,                    0.0089974192090911398 }
,     {                      -1.9973937586662434 ,                    -0.012085189551271426 }
} }
,{ {  {                   2.8811019587551389e-18 ,                    2.684646603916068e-16 }
,     {                  -6.2333922975783146e-16 ,                   1.4252468434090293e-16 }
,     {                   9.0877705975533128e-15 ,                  -3.4638532206046957e-14 }
,     {                  -6.2053772915893235e-14 ,                   4.7270659575711313e-13 }
,     {                  -9.4369582503557563e-14 ,                  -3.0132196321689225e-12 }
,     {                   8.1568391992615411e-12 ,                  -3.6795550343994194e-12 }
,     {                  -1.1859203571949966e-10 ,                   3.2326034833829272e-10 }
,     {                   1.1302927297982895e-09 ,                  -4.2729928409363866e-09 }
,     {                  -8.1653674229686321e-09 ,                    3.649262770188356e-08 }
,     {                   4.6711146297670056e-08 ,                  -2.3290328068405808e-07 }
,     {                  -2.1434786976693524e-07 ,                    1.157560138845965e-06 }
,     {                   7.8712065435992157e-07 ,                  -4.5198606760227629e-06 }
,     {                  -2.2827106290462468e-06 ,                   1.3751490608604711e-05 }
,     {                    5.108210075437179e-06 ,                  -3.1912388224577725e-05 }
,     {                   -8.532197326791333e-06 ,                   5.4617171212102142e-05 }
,     {                      -1.9999897804462703 ,                  -6.6041177531743063e-05 }
} }
,{ {  {                  -7.4993123127081172e-19 ,                  -9.2154967085056884e-19 }
,     {                   1.6453122028975649e-17 ,                  -4.4526109981603194e-17 }
,     {                  -2.2216971245348065e-16 ,                   9.2045328395178569e-16 }
,     {                   2.3187986651361246e-15 ,                  -1.1597351157562596e-14 }
,     {                  -1.9966897966189304e-14 ,                   1.1222278921048576e-13 }
,     {                   1.4567188498750867e-13 ,                  -8.9014086166989192e-13 }
,     {                  -9.1094290960037775e-13 ,                   5.9390981887108318e-12 }
,     {                   4.9015331385524046e-12 ,                  -3.3684085607283493e-11 }
,     {                  -2.2668130864064367e-11 ,                   1.6278815862238778e-10 }
,     {                   8.9656333841687577e-11 ,                  -6.6839174980108575e-10 }
,     {                  -3.0070122824813192e-10 ,                   2.3145401708228895e-09 }
,     {                   8.4494270776318715e-10 ,                  -6.6824163147637241e-09 }
,     {                  -1.9576794331996479e-09 ,                   1.5833623495033884e-08 }
,     {                   3.6665887957624338e-09 ,                  -3.0174569513159495e-08 }
,     {                  -5.4258802952106441e-09 ,                   4.5166333861133352e-08 }
,     {                      -1.9999999937936801 ,                  -5.1878090694002071e-08 }
} }
,{ {  {                  -7.3128123766095007e-21 ,                   5.2783768964163724e-20 }
,     {                   5.8331678204505884e-20 ,                  -4.4428465693890153e-19 }
,     {                  -4.1665340361792927e-19 ,                   3.3193577484164931e-18 }
,     {                   2.6702440438238306e-18 ,                  -2.2110261645071223e-17 }
,     {                  -1.5352877622533002e-17 ,                   1.3149107185196035e-16 }
,     {                   7.9054987528274155e-17 ,                  -6.9763687703652322e-16 }
,     {                  -3.6338645128878323e-16 ,                   3.2936905729829264e-15 }
,     {                   1.4841921457728194e-15 ,                  -1.3779549123432719e-14 }
,     {                  -5.3535076599248966e-15 ,                   5.0787839237713144e-14 }
,     {                   1.6922815522375102e-14 ,                  -1.6367776360132983e-13 }
,     {                  -4.6440786423613345e-14 ,                   4.5693541177471563e-13 }
,     {                   1.0939887392901224e-13 ,                  -1.0924934920735968e-12 }
,     {                  -2.1831634568207554e-13 ,                   2.2073173946389116e-12 }
,     {                   3.6368947573372969e-13 ,                  -3.7122896402585035e-12 }
,     {                  -4.9822933420386177e-13 ,                   5.1168332005087491e-12 }
,     {                      -1.9999999999994464 ,                  -5.7052069770739506e-12 }
} }
,{ {  {                  -1.1581263794211803e-24 ,                   1.1756024314709261e-23 }
,     {                   6.8651109138591869e-24 ,                  -7.1268610401803094e-23 }
,     {                   -3.738518343124306e-23 ,                   3.9620223549082376e-22 }
,     {                   1.8656852708023271e-22 ,                  -2.0152981488264423e-21 }
,     {                  -8.5056522662053255e-22 ,                   9.3514915353419952e-21 }
,     {                   3.5290018709287991e-21 ,                  -3.9440168120129876e-20 }
,     {                  -1.3264545171699188e-20 ,                   1.5051156637249394e-19 }
,     {                   4.4925669595385181e-20 ,                  -5.1696379430130061e-19 }
,     {                  -1.3624823721052732e-19 ,                   1.5881329934248198e-18 }
,     {                   3.6731865126752203e-19 ,                  -4.3319144361960655e-18 }
,     {                  -8.7299974058134151e-19 ,                   1.0403780623845348e-17 }
,     {                   1.8120620323788916e-18 ,                  -2.1791909247822896e-17 }
,     {                   -3.251392893971571e-18 ,                   3.9396773141907617e-17 }
,     {                   4.9898793744105705e-18 ,                  -6.0808623975481749e-17 }
,     {                  -6.4850528855079053e-18 ,                   7.9315808137192181e-17 }
,     {                      -2.0000000000000009 ,                  -8.6748835517513367e-17 }
} }
,{ {  {                   -1.286993020143186e-29 ,                   1.6244908657170241e-28 }
,     {                   6.2765862086305583e-29 ,                  -8.0275046304822356e-28 }
,     {                   -2.839692427061988e-28 ,                   3.6773373634048155e-27 }
,     {                   1.1882092616181199e-27 ,                  -1.5569151083770561e-26 }
,     {                   -4.582191637967378e-27 ,                   6.0711381921074574e-26 }
,     {                   1.6221432712231815e-26 ,                  -2.1718558315433516e-25 }
,     {                   -5.247933806259193e-26 ,                   7.0956869041034713e-25 }
,     {                   1.5437131529880892e-25 ,                  -2.1064417534076447e-24 }
,     {                  -4.1053336780396758e-25 ,                   5.6494507799722326e-24 }
,     {                    9.807876328051367e-25 ,                  -1.3601376051918737e-23 }
,     {                  -2.0903055414542895e-24 ,                   2.9188353967295514e-23 }
,     {                   3.9443914370892561e-24 ,                  -5.5407486881004531e-23 }
,     {                  -6.5385165651342071e-24 ,                   9.2298616960723605e-23 }
,     {                   9.4486116746898621e-24 ,                  -1.3386968566261501e-22 }
,     {                  -1.1822408135528828e-23 ,                   1.6788751035824249e-22 }
,     {                      -2.0000000000000009 ,                  -1.8115931820473034e-22 }
} }
                }
        }


 ;

double ErfEvaluator::dk1 = 1.0 ;
#endif

#endif

#if defined(SPLINE_ERFCC)

class ErfEvaluator
{
   public:
   class doublepair
   {
           public:
           double ca ;
           double cb ;
   } ;
   class polypairlist
   {
           public:
           enum {
              k_Terms = 4
           } ;
           doublepair dp[k_Terms] ;
   } ;
   enum {
           k_Slices = 64
   } ;
   static const polypairlist p[k_Slices] ;
   static const float WholeNumbers[k_Slices] ;
   static const double IntegrationConstants[k_Slices] ;
   static void Evaluate (double xRaw, double& f, double& df)
   {
      // assert(xRaw >= 0.0) ;                // This version only defined for positive argument
      xRaw = fsel(xRaw-3.999,3.999,xRaw) ; // Pinned if out of range
           double xScale = xRaw * 16.0 ;
           int a = xScale ;
           double xWhole = WholeNumbers[a] ;
           double xFrac = xScale-xWhole ;
           double x = (xFrac*2.0) - 1.0 ;

           double rf =  ((((p[a].dp[0].ca)*x+p[a].dp[1].ca)*x+p[a].dp[2].ca)*x+p[a].dp[3].ca) ;
           double rdf = ((((p[a].dp[0].cb)*x+p[a].dp[1].cb)*x+p[a].dp[2].cb)*x+p[a].dp[3].cb) ;

           f = rf * x + IntegrationConstants[a];
           df = rdf  ;

   }
} ;
#ifndef MSD_COMPILE_CODE_ONLY
const float ErfEvaluator::WholeNumbers[ErfEvaluator::k_Slices] =
{
        0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0
        ,10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0
        ,20.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0
        ,30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0
        ,40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0, 48.0, 49.0
        ,50.0, 51.0, 52.0, 53.0, 54.0, 55.0, 56.0, 57.0, 58.0, 59.0
        ,60.0, 61.0, 62.0, 63.0
} ;

const ErfEvaluator::polypairlist ErfEvaluator::p[ErfEvaluator::k_Slices] = {
 { {
  { -1.677043705606067e-08,-2.146615943175766e-06 }
  ,{ 1.143371616236766e-05,0.001097636751587296 }
  ,{ 3.440177110943935e-05,0.002201713351004118 }
  ,{ -0.03522741365975122,  -1.127277237112039 }
} }
,{ {
  { -4.965987042014532e-08,-6.356463413778601e-06 }
  ,{ 1.116730250618423e-05,0.001072061040593686 }
  ,{ 0.0001024021637152795, 0.00655373847777789 }
  ,{ -0.03495327227036313,   -1.11850471265162 }
} }
,{ {
  { -8.063031663779123e-08,-1.032068052963728e-05 }
  ,{ 1.06447944258563e-05,0.001021900264882204 }
  ,{ 0.000168024277188707, 0.01075355374007725 }
  ,{ -0.03441137302506241,  -1.101163936801997 }
} }
,{ {
  { -1.085177767547462e-07,-1.389027542460751e-05 }
  ,{ 9.886272248044382e-06,0.0009490821358122607 }
  ,{ 0.0002297848019711107, 0.01470622732615109 }
  ,{  -0.0336142358925829,  -1.075655548562653 }
} }
,{ {
  { -1.323228464452476e-07,-1.693732434499169e-05 }
  ,{ 8.920499832118013e-06,0.0008563679838833292 }
  ,{ 0.0002863479486297409, 0.01832626871230342 }
  ,{ -0.03258003643662622,  -1.042561165972039 }
} }
,{ {
  { -1.512604951606179e-07,-1.936134338055909e-05 }
  ,{ 7.783401736190406e-06,0.0007472065666742789 }
  ,{ 0.0003365732792269584, 0.02154068987052534 }
  ,{ -0.03133191701999147,  -1.002621344639727 }
} }
,{ {
  { -1.647954146847001e-07,-2.109381307964162e-05 }
  ,{ 6.51619848252759e-06,0.0006255550543226487 }
  ,{ 0.0003795532894002569, 0.02429141052161644 }
  ,{ -0.02989712649661543, -0.9567080478916936 }
} }
,{ {
  { -1.726609582696268e-07,-2.210060265851223e-05 }
  ,{ 5.163323960181864e-06,0.0004956791001774589 }
  ,{ 0.0004146390499898948, 0.02653689919935327 }
  ,{ -0.02830603301975991,  -0.905793056632317 }
} }
,{ {
  { -1.748610042477048e-07,-2.238220854370622e-05 }
  ,{ 3.77025961848157e-06,0.0003619449233742307 }
  ,{ 0.0004414530010017535, 0.02825299206411223 }
  ,{ -0.02659105981260158, -0.8509139140032504 }
} }
,{ {
  { -1.716553961417857e-07,-2.197189070614856e-05 }
  ,{ 2.381420131299626e-06,0.0002286163326047641 }
  ,{ 0.0004598888066024617, 0.02943288362255755 }
  ,{ -0.02478559574453316, -0.7931390638250611 }
} }
,{ {
  { -1.63530815350447e-07,-2.093194436485722e-05 }
  ,{ 1.03821508918815e-06,9.966864856206237e-05 }
  ,{ 0.000470098964779177, 0.03008633374586733 }
  ,{ -0.02292293133148951, -0.7335338026076644 }
} }
,{ {
  { -1.511599296296717e-07,-1.934847099259798e-05 }
  ,{ -2.22607659821702e-07,-2.137033534288339e-05 }
  ,{ 0.0004724715617529516,  0.0302381799521889 }
  ,{ -0.02103526657315815, -0.6731285303410608 }
} }
,{ {
  { -1.353523545145142e-07,-1.732510137785782e-05 }
  ,{ -1.370257092802229e-06,-0.000131544680909014 }
  ,{ 0.0004675981220443889, 0.02992627981084089 }
  ,{ -0.01915283030640111, -0.6128905698048354 }
} }
,{ {
  { -1.170013221461676e-07,-1.497616923470946e-05 }
  ,{ -2.380782501521628e-06,-0.0002285551201460763 }
  ,{ 0.0004562348970672072, 0.02919903341230126 }
  ,{ -0.01730314210665839, -0.5537005474130686 }
} }
,{ {
  { -9.702996492746713e-08,-1.241983551071579e-05 }
  ,{ -3.237526760163412e-06,-0.0003108025689756875 }
  ,{ 0.0004392601411388399, 0.02811264903288575 }
  ,{ -0.01551043792261867, -0.4963340135237975 }
} }
,{ {
  { -7.634081565236948e-08,-9.771624403503293e-06 }
  ,{ -3.931160936497558e-06,-0.0003773914499037656 }
  ,{ 0.0004176299431532064, 0.02672831636180521 }
  ,{ -0.01379527033462789, -0.4414486507080926 }
} }
,{ {
  { -5.577155569193423e-08,-7.138759128567582e-06 }
  ,{ -4.459338467442303e-06,-0.0004280964928744611 }
  ,{ 0.0003923350293816243, 0.02510944188042395 }
  ,{ -0.01217428430645078, -0.3895770978064249 }
} }
,{ {
  { -3.605928292969135e-08,-4.615588215000493e-06 }
  ,{ -4.826028411989542e-06,-0.000463298727550996 }
  ,{ 0.0003643606551067552, 0.02331908192683234 }
  ,{ -0.01066016019099486, -0.3411251261118356 }
} }
,{ {
  { -1.781470707992619e-08,-2.280282506230552e-06 }
  ,{ -5.040602043929432e-06,-0.0004838977962172255 }
  ,{ 0.0003346512962838997, 0.02141768296216958 }
  ,{ -0.009261708063277772, -0.2963746580248887 }
} }
,{ {
  { -1.506798321843929e-09,-1.928701851960229e-07 }
  ,{ -5.116754768286062e-06,-0.0004912084577554619 }
  ,{ 0.0003040813783947048, 0.01946120821726111 }
  ,{ -0.007984091541769101, -0.2554909293366112 }
} }
,{ {
  { 1.254240539445633e-08,1.60542789049041e-06 }
  ,{ -5.071346219164424e-06,-0.0004868492370397846 }
  ,{ 0.0002734327802100555, 0.01749969793344355 }
  ,{ -0.006829155307778654, -0.2185329698489169 }
} }
,{ {
  { 2.415382265476675e-08,3.091689299810144e-06 }
  ,{ -4.923236176470265e-06,-0.0004726306729411454 }
  ,{ 0.0002433793645195896, 0.01557627932925373 }
  ,{ -0.00579582856353525,  -0.185466514033128 }
} }
,{ {
  { 3.327970129438388e-08,4.259801765681137e-06 }
  ,{ -4.692183812243332e-06,-0.0004504496459753599 }
  ,{ 0.0002144783492816111, 0.01372661435402311 }
  ,{ -0.004880576558929733, -0.1561784498857515 }
} }
,{ {
  { 3.998619096881058e-08,5.118232444007755e-06 }
  ,{ -4.397864199620254e-06,-0.0004221949631635444 }
  ,{ 0.0001871679663079738, 0.01197874984371032 }
  ,{ -0.004077873823019379, -0.1304919623366201 }
} }
,{ {
  { 4.443210814183816e-08,5.687309842155285e-06 }
  ,{ -4.059040540924509e-06,-0.0003896678919287529 }
  ,{ 0.0001617705765833013, 0.01035331690133128 }
  ,{ -0.003380675535016474, -0.1081816171205272 }
} }
,{ {
  { 4.684585000529841e-08,5.996268800678196e-06 }
  ,{ -3.692914687568888e-06,-0.0003545198100066132 }
  ,{ 0.0001385002284466404,0.008864014620584983 }
  ,{ -0.002780867190684069,-0.08898775010189021 }
} }
,{ {
  { 4.750223096564192e-08,6.080285563602166e-06 }
  ,{ -3.314663535346774e-06,-0.0003182076993932903 }
  ,{ 0.0001174735554921313,0.007518307551496402 }
  ,{ -0.002269676983939308,-0.07262966348605786 }
} }
,{ {
  { 4.670072464852188e-08, 5.9776927550108e-06 }
  ,{ -2.937155806094876e-06,-0.0002819669573851081 }
  ,{ 9.872290650570907e-05,0.006318266016365381 }
  ,{ -0.001838039771308927,-0.05881727268188565 }
} }
,{ {
  { 4.474623841270917e-08,5.727518516826773e-06 }
  ,{ -2.570833263466638e-06,-0.0002467999932927972 }
  ,{ 8.22106662144394e-05,0.005261482637724121 }
  ,{ -0.00147690580564427,-0.04726098578061663 }
} }
,{ {
  { 4.193316738169349e-08,5.367445424856766e-06 }
  ,{ -2.223732912047361e-06,-0.0002134783595565467 }
  ,{ 6.784384611408351e-05,0.004342006151301345 }
  ,{ -0.001177491364418304,-0.03767972366138573 }
} }
,{ {
  { 3.853310716189559e-08,4.932237716722635e-06 }
  ,{ -1.901622239253096e-06,-0.0001825557349682972 }
  ,{ 5.548818102150092e-05,0.003551243585376059 }
  ,{ -0.0009314717770673435,-0.02980709686615499 }
} }
,{ {
  { 3.478627717320908e-08,4.452643478170763e-06 }
  ,{ -1.608217862833661e-06,-0.0001543889148320314 }
  ,{ 4.498114169517277e-05,0.002878793068491057 }
  ,{ -0.000731120067225914,-0.02339584215122925 }
} }
,{ {
  { 3.089643929408761e-08,3.954744229643214e-06 }
  ,{ -1.345458635910959e-06,-0.000129164029047452 }
  ,{ 3.614345122621364e-05,0.002313180878477673 }
  ,{ -0.0005693964281980402,-0.01822068570233729 }
} }
,{ {
  { 2.702889954915076e-08,3.459699142291297e-06 }
  ,{ -1.113806810496106e-06,-0.0001069254538076262 }
  ,{ 2.878886012546207e-05,0.001842487048029572 }
  ,{ -0.0004399950596155604,-0.01407984190769793 }
} }
,{ {
  { 2.331105572966068e-08,2.983815133396567e-06 }
  ,{ -9.125546956961835e-07,-8.760525078683361e-05 }
  ,{ 2.273208266980214e-05,0.001454853290867337 }
  ,{ -0.0003373555705483165,-0.01079537825754613 }
} }
,{ {
  { 1.983489619315291e-08,2.538866712723572e-06 }
  ,{ -7.40118807871303e-07,-7.105140555564509e-05 }
  ,{ 1.779491911631872e-05,0.001138874823444398 }
  ,{ -0.0002566462908318581,-0.00821268130661946 }
} }
,{ {
  { 1.66608547873407e-08,2.13258941277961e-06 }
  ,{ -5.943082966062757e-07,-5.705359647420247e-05 }
  ,{ 1.381068205132086e-05,0.0008838836512845352 }
  ,{ -0.0001937265370264521,-0.006199249184846466 }
} }
,{ {
  { 1.382247111915601e-08,1.769276303251969e-06 }
  ,{ -4.725590338093643e-07,-4.536566724569897e-05 }
  ,{ 1.062711036208305e-05,0.0006801350631733152 }
  ,{ -0.000145094266684409,-0.004643016533901087 }
} }
,{ {
  { 1.133138043388216e-08,1.450416695536916e-06 }
  ,{ -3.721288720513465e-07,-3.572437171692926e-05 }
  ,{ 8.107993188612562e-06,0.000518911564071204 }
  ,{ -0.0001078247346620797,-0.003450391509186549 }
} }
,{ {
  { 9.182249868241103e-09,1.175327983134861e-06 }
  ,{ -2.902530201337116e-07,-2.786428993283631e-05 }
  ,{ 6.133742295451233e-06,0.0003925595069088789 }
  ,{ -7.950483672849648e-05,-0.002544154775311887 }
} }
,{ {
  { 7.357376121771672e-09,9.41744143586774e-07 }
  ,{ -2.242611569820876e-07,-2.152907107028042e-05 }
  ,{ 4.601149006582652e-06,0.0002944735364212897 }
  ,{ -5.816687120992353e-05,-0.001861339878717553 }
} }
,{ {
  { 5.830754319011006e-09,7.463365528334088e-07 }
  ,{ -1.716598017483407e-07,-1.647934096784071e-05 }
  ,{ 3.422545861207931e-06,0.0002190429351173076 }
  ,{ -4.222453301828047e-05,-0.001351185056584975 }
} }
,{ {
  { 4.571512315677453e-09,5.85153576406714e-07 }
  ,{ -1.301846350963824e-07,-1.249772496925271e-05 }
  ,{ 2.524568002693763e-06,0.0001615723521724008 }
  ,{ -3.041312172389421e-05,-0.0009732198951646148 }
} }
,{ {
  { 3.546674836120153e-09,4.539743790233795e-07 }
  ,{ -9.782802374705724e-08,-9.391490279717495e-06 }
  ,{ 1.846679051040788e-06,0.0001181874592666104 }
  ,{ -2.173522470186589e-05,-0.0006955271904597086 }
} }
,{ {
  { 2.723275825780344e-09,3.48579305699884e-07 }
  ,{ -7.284706052621752e-08,-6.993317810516882e-06 }
  ,{ 1.339594192283002e-06,8.573402830611214e-05 }
  ,{ -1.541254130223323e-05,-0.0004932013216714634 }
} }
,{ {
  { 2.069875334693146e-09,2.649440428407226e-07 }
  ,{ -5.375712506594504e-08,-5.160684006330723e-06 }
  ,{ 9.637020384530377e-07,6.167693046099441e-05 }
  ,{ -1.084404604250008e-05,-0.0003470094733600024 }
} }
,{ {
  { 1.557560545084242e-09,1.99367749770783e-07 }
  ,{ -3.931541933447623e-08,-3.774280256109718e-06 }
  ,{ 6.875582939894272e-07,4.400373081532334e-05 }
  ,{ -7.570340751290444e-06,-0.0002422509040412942 }
} }
,{ {
  { 1.160521497792107e-09,1.485467517173897e-07 }
  ,{ -2.849824560521101e-08,-2.735831578100257e-06 }
  ,{ 4.864995334541183e-07,3.113597014106357e-05 }
  ,{ -5.243804289419642e-06,-0.0001678017372614286 }
} }
,{ {
  { 8.562933419045533e-10,1.096055477637828e-07 }
  ,{ -2.04750605380212e-08,-1.965605811650035e-06 }
  ,{ 3.41404983959747e-07,2.184991897342381e-05 }
  ,{ -3.603997550434695e-06,-0.0001153279216139102 }
} }
,{ {
  { 6.25751255928244e-10,8.009616075881524e-08 }
  ,{ -1.458162415139377e-08,-1.399835918533802e-06 }
  ,{  2.3761818240736e-07,1.520756367407104e-05 }
  ,{ -2.457703348469032e-06,-7.864650715100903e-05 }
} }
,{ {
  { 4.529340758609023e-10,5.79755617101955e-08 }
  ,{ -1.029391197359319e-08,-9.882155494649466e-07 }
  ,{ 1.640284771128031e-07,1.04978225352194e-05 }
  ,{ -1.662958424634951e-06,-5.321466958831844e-05 }
} }
,{ {
  { 3.247602445078765e-10,4.15693112970082e-08 }
  ,{ -7.203899323906403e-09,-6.915743350950147e-07 }
  ,{ 1.123040862084225e-07,7.187461517339037e-06 }
  ,{ -1.116452558660085e-06,-3.572648187712273e-05 }
} }
,{ {
  { 2.30686638875616e-10,2.952788977607884e-08 }
  ,{ -4.997870017311665e-09,-4.797955216619198e-07 }
  ,{ 7.626321981856181e-08,4.880846068387956e-06 }
  ,{ -7.43714246992367e-07,-2.379885590375575e-05 }
} }
,{ {
  { 1.62348343465352e-10,2.078058796356506e-08 }
  ,{ -3.437547837088295e-09,-3.300045923604763e-07 }
  ,{ 5.136699602782352e-08,3.287487745780705e-06 }
  ,{ -4.915626419109025e-07,-1.573000454114888e-05 }
} }
,{ {
  { 1.132062933665611e-10,1.449040555091982e-08 }
  ,{ -2.344090013092694e-09,-2.250326412568986e-07 }
  ,{ 3.43169347778733e-08,2.196283825783891e-06 }
  ,{ -3.223729352241681e-07,-1.031593392717338e-05 }
} }
,{ {
  { 7.822032766619364e-11,1.001220194127279e-08 }
  ,{ -1.5848050842967e-09,-1.521412880924832e-07 }
  ,{ 2.274016527990733e-08,1.455370577914069e-06 }
  ,{ -2.097708834108638e-07,-6.71266826914764e-06 }
} }
,{ {
  { 5.355770518520448e-11,6.855386263706173e-09 }
  ,{ -1.062348163068451e-09,-1.019854236545713e-07 }
  ,{ 1.494668127269781e-08, 9.5658760145266e-07 }
  ,{ -1.354374489379326e-07,-4.333998366013844e-06 }
} }
,{ {
  { 3.634147481032868e-11,4.651708775722071e-09 }
  ,{ -7.060897746027254e-10,-6.778461836186165e-08 }
  ,{ 9.74466484193354e-09,6.236585498837466e-07 }
  ,{ -8.676393515408498e-08,-2.776445924930719e-06 }
} }
,{ {
  { 2.443906800145819e-11,3.128200704186648e-09 }
  ,{ -4.653362120425529e-10,-4.467227635608508e-08 }
  ,{ 6.301801322850928e-09,4.033152846624594e-07 }
  ,{ -5.515014134094942e-08,-1.764804522910381e-06 }
} }
,{ {
  { 1.628883049763073e-11,2.084970303696734e-09 }
  ,{ -3.04087667299206e-10,-2.919241606072378e-08 }
  ,{ 4.042431109848616e-09,2.587155910303114e-07 }
  ,{ -3.478251051578401e-08,-1.113040336505088e-06 }
} }
,{ {
  { 1.076065172092256e-11,1.377363420278088e-09 }
  ,{ -1.970454032946794e-10,-1.891635871628923e-08 }
  ,{ 2.572200970727208e-09,1.646208621265413e-07 }
  ,{ -2.176617521777173e-08,-6.965176069686953e-07 }
} }
,{ {
  { 7.046112666236051e-12,9.019024212782145e-10 }
  ,{ -1.266132861125826e-10,-1.215487546680793e-08 }
  ,{ 1.623512136833541e-09,1.039047767573466e-07 }
  ,{ -1.351481829664788e-08,-4.324741854927323e-07 }
} }
,{ {
  { 4.573415576263572e-12,5.853971937617372e-10 }
  ,{ -8.067651102423245e-11,-7.744945058326315e-09 }
  ,{ 1.016478927962931e-09,6.505465138962761e-08 }
  ,{ -8.326168379349991e-09,-2.664373881391997e-07 }
} }
,{ {
  { 2.942581325108452e-12,3.766504096138819e-10 }
  ,{ -5.097751607967133e-11,-4.893841543648448e-09 }
  ,{ 6.313018521581508e-10,4.040331853812165e-08 }
  ,{ -5.08963981633602e-09,-1.628684741227526e-07 }
} }

} ;


const double  ErfEvaluator::IntegrationConstants[ErfEvaluator::k_Slices] = {
  0.9647496350557387
 ,0.8945235826411205
 ,0.8251151582995999
 ,0.7570483478113061
 ,0.6908163429125882
 ,0.6268708869645903
 ,0.5656131765729484
 ,0.5073866186839697
 ,0.452471647684221
 ,0.4010827047956272
 ,0.3533673790720676
 ,0.3094076118069897
 ,0.2692227796948113
 ,0.2327744011161023
 ,0.1999721575621347
 ,0.1706808901260279
 ,0.1447282193300569
 ,0.1219124441277339
 ,0.1020104003572524
 ,0.0847849970053737
 ,0.06999219660381942
 ,0.05738725995438308
 ,0.04673013130128872
 ,0.03778989454781171
 ,0.03034828122884282
 ,0.02420225448230857
 ,0.01916572874603583
 ,0.01507051162193889
 ,0.01176657225069364
 ,0.00912175014762694
 ,0.007021020716142793
 ,0.005365429817903765
 ,0.004070801226287904
 ,0.003066308931668393
 ,0.002292992435297803
 ,0.001702278540127081
 ,0.001254558696270697
 ,0.0009178574353023258
 ,0.0006666153543141322
 ,0.000480599801055098
 ,0.0003439479971021772
 ,0.0002443408016824203
 ,0.0001723005396039184
 ,0.0001206030943084474
 ,8.379256147784313e-05
 ,5.778591550183871e-05
 ,3.955511222290099e-05
 ,2.687460931683381e-05
 ,1.81232329484865e-05
 ,1.213049270853172e-05
 ,8.058717921777377e-06
 ,5.313661691920574e-06
 ,3.477428056922396e-06
 ,2.258680312250875e-06
 ,1.456062300565673e-06
 ,9.316013422969179e-07
 ,5.915640033304468e-07
 ,3.728134139618971e-07
 ,2.33182677406109e-07
 ,1.447481221205399e-07
 ,8.91740616342884e-08
 ,5.452181280340989e-08
 ,3.308304223623579e-08
 ,1.99223879235018e-08
} ;

#endif

#endif

#define Deg2Rad Deg2RadFunction()

class Math
{
public:
const static double PI;
  const static double PI_2;
  const static double PI_4;
  const static double InvPI;
  const static double SqrtPI;
  const static double InvSqrtPI;
  const static double OneThird;

  static double Deg2RadFunction() { return M_PI/180.0 ; } 
//  const static double Deg2Rad;
  const static double Rad2Deg;

  const static double E;
  const static double InvE;

  const static double Log2E;
  const static double Log10E;

  const static double Log2;
  const static double Log10;
  const static double Sqrt2;
  const static double Sqrt2_2;

  /*
   * Constants involved in exp(x)
   */
  const static double Infinity ;
  const static double ExpTable1[16] ;
  const static double ExpTable2[16] ;

  /*
   * Constants involved in log(x)
   */
  const static double hlogComp[16] ;
  const static double hlogTable[16] ;

  /*
   * COMPONENT_NAME: (LIBM) math library
   *
   * FUNCTIONS: asin
   *
   * ORIGINS: 27
   *
   */

  /*
   * NAME: asin
   *
   * FUNCTION: RETURN THE PRINCIPAL VALUE OF THE ARC SINE OF X
   *
   * NOTES:
   *
   * Uses economized power series to evaluate arc-sine for arguments
   * whose magnitude is less than PI/2.
   *
   * RETURNS: arc sine of x in the range [-PI/2,PI/2]
   *
   */

//static const double a14;
//static const double a13;
//static const double a12;
//static const double a11;
//static const double a10;
//static const double a9;
//static const double a8;
//static const double a7;
//static const double a6;
//static const double a5;
//static const double a4;
//static const double a3;
//static const double a2;
//static const double a1;
//static const double a0;
//static const double aa7;
//static const double aa6;
//static const double aa5;
//static const double aa4;
//static const double aa3;
//static const double aa2;
//static const double aa1;
//static const double aa0;

//static const double piby8    ;
//static const double pi3by8   ;
//static const double pi5by8   ;
//static const double pi7by8   ;
//static const double sinpiby8 ;
//static const double cospiby8 ;


  template <class T> static inline T sqr(T t)      { return t*t; }
  template <class T> static inline T cube(T t)     { return t*t*t; }
  template <class T> static inline T min(T a, T b) { return (a < b) ? a : b; }
  template <class T> static inline T max(T a, T b) { return (a > b) ? a : b; }
  template <class T> static inline T sgn(T t)      { return (t > 0) ? 1 : (t < 0) ? -1 : 0; }



  static inline double erfc( double x )
    {
      const double a1 = 0.254829592, a2 = -0.284496736,
        a3 = 1.421413741, a4 = -1.453152027,
        a5 = 1.061405429, p  =  0.3275911;

      double t = 1.0/(1.0+p*x);
      double xsqr = x*x;
      double tp = t*(a1+t*(a2+t*(a3+t*(a4+t*a5))));
      return tp*exp(-xsqr);
    }

  static inline double derfc( double x )
    {
      return -2*InvSqrtPI*exp(-x*x);
    }

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
//       const double Math::aa7 =  0.01154901189590083099584;
//       const double Math::aa6 =  0.01396501522140471126730;
//       const double Math::aa5 =  0.01735275722383019335276;
//       const double Math::aa4 =  0.02237215928757573539490;
//       const double Math::aa3 =  0.03038194444121688698408;
//       const double Math::aa2 =  0.04464285714288482921087;
//       const double Math::aa1 =  0.07499999999999990640378;
//       const double Math::aa0 =  0.1666666666666666667191;
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

    inline static double atrig ( double sinang, double cosang )
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

  /*
   * Branchless exp(x), with a view to vectorising on Double Hummer
   */

  static inline double hexp(double x)
  {

    const double tp10 = 1024.0 ;
    const double tp20 = tp10*tp10 ;
    const double tp40 = tp20*tp20 ;

     // Dividing by ln(2) gives a value such that we can put the integer part into an exponent.
     // Adding (2**44+2**43) aligns and rounds this so that
     // bottom 8 bits can be used for lookup
     // higher bits can be stuffed into exponent
     // truncated bits (recovered by subtraction) can be fed to power series
    const double x1 = x * ( 1.0 / M_LN2 ) + ( tp40 * ( 16.0 + 8.0 ) ) ;
    const DoubleMap m1(x1) ;
    // Figure the appropriate power of 2 from the significand high bits
    const unsigned int sig_lo = m1.SignificandLoBits() ;
    const DoubleMap m2(0,sig_lo >> 8, 0, 0) ;
    // Recover the number that we will have 'exponentiated' by the bit twiddling
    const double xl2 =  x1 - ( tp40 * ( 16.0 + 8.0 ) ) ;
    // Can range-check xl2 to see if sig_lo put a sensible value in m2
    const double xx4 = xl2 * M_LN2 ;

    // Look up the next several bits (4) in a multiplication table
    const unsigned int tabits =(sig_lo >> 4) & 0x0f ;
    const double x31 = ExpTable1[tabits] ;

    // And the next 4 bits in another table
    const unsigned int tabits2=sig_lo & 0x0f ;
    const double x32 = ExpTable2[tabits2] ;
    const double x3 = x31*x32 ;

    // Figure the remaining part of the original number
    const double z = x - xx4 ;
    // z should be between +- (2**-8); feed in to polynomial for exp(z)

    const double f5 =          1.0/(2.0*3.0*4.0*5.0) ;
    const double f4 = z * f5 + 1.0/(2.0*3.0*4.0) ;
    const double f3 = z * f4 + 1.0/(2.0*3.0) ;
    const double f2 = z * f3 + 1.0/2.0 ;
    const double f1 = z * f2 + 1.0 ;
    const double f0 = z * f1 + 1.0 ;

    const double p0 = f0 * x3 ;

    const double x2=m2.GetValue() ;

    const double r0 = p0 * x2 ;

    // Fixup for out-of-range parameter
    const double resultl = fsel(x+709.0, r0, 0.0) ;
    const double resulth = fsel(x-709.0, Infinity, resultl) ;

    return resulth ;
  }

 // Log base e, along similar lines
 // (actually computes log(abs(x)), gives a large negative number for f(0), gives a large positive
 //  number if fed Inf or Nan)
 static inline double hlog(double x)
 {
   const DoubleMap m(x) ;
   int exponent = m.Exponent() ;
   unsigned int sig_hi = m.SignificandHiBits() ;
   DoubleMap m1(0,0,sig_hi, m.SignificandLoBits()) ;
   unsigned int tableIndex = sig_hi >> 16 ;
   double xx=m1.GetValue()*hlogComp[tableIndex] - 1.0 ;

   // There should be scope for shortening this polynomial
   // (1) rescale for 'xx' to be in (-1/32, 1/32) rather than (0,1/16)
   // (2) have a larger table, and take more bits to index it
   // (actually, not sure this is large enough for double precision)
   double p10 =         (-1.0/10.0) ;
   double p9  = p10 * xx + (  1.0 / 9.0 ) ;
   double p8  = p9 * xx + ( -1.0 / 8.0 ) ;
   double p7  = p8 * xx + (  1.0 / 7.0 ) ;
   double p6  = p7 * xx + ( -1.0 / 6.0 ) ;
   double p5  = p6 * xx + (  1.0 / 5.0 ) ;
   double p4  = p5 * xx + ( -1.0 / 4.0 ) ;
   double p3  = p4 * xx + (  1.0 / 3.0 ) ;
   double p2  = p3 * xx + ( -1.0 / 2.0 ) ;
   double p1  = p2 * xx + (  1.0 )  ;
   double p0  = p1 * xx ;
   double result= exponent*M_LN2 + hlogTable[tableIndex] + p0 ;

/*
 * BegLogLine(1)
 *   << "hlog x=" << x
 *   << " exponent=" << exponent
 *   << " tableIndex=" << tableIndex
 *   << " xx=" << xx
 *   << " p0=" << p0
 *   << " result=" << result
 *   << EndLogLine ;
 */
   return result ;
 }

#if defined(CHEBYSHEV_ERFCC) || defined(SPLINE_ERFCC)
  static inline void erfccd(double x, double& fx,double& dfdx)
  {
     ErfEvaluator::Evaluate( x,fx,dfdx) ;
  }
#else
  /*
   * Error function and derivative, from Abramowitz and Stegun
   * Absolute error of erfcc everywhere less than 1.5e-7
   * erfcc has correct asymptotic behaviour as x --> +- infinity
   * derivative is analytic derivative of estimate, i.e. should
   * enable MD to match momentum and energy
   *
   *  f(x) = q(t(x)).exp(-x**2)
   * where t(x) = 1/(1+p*x)
   *       q(t) = polynomial in t
   *          p = constant
   *
   * Substitute a=-x**2
   *            b=exp(a)
   *  f    = q.b
   *
   *  df/dx = (dq/dx)*b + q*(db/dx)
   *  dq/dx = (dq/dt)*(dt/dx) = (dq/dt)*p*(-1)/(1+p*x)**2
   *  db/dx = (db/da)*(da/dx) = exp(a)*(da/dx)
   *  da/dx = -2*x
   *
   */
  static inline void erfccd(double x, double& fx,double& dfdx)
  {
     const double kq1 =  0.254829592 ;
     const double kq2 = -0.284496736 ;
     const double kq3 =  1.421413741 ;
     const double kq4 = -1.453152027 ;
     const double kq5 =  1.061405429 ;
     const double  kp =  0.3275911   ;
     const double z=fabs(x) ;
     const double lz = 1.0+kp*z ;
//     const double t=1.0/lz ;
     const double t=recip(lz) ;

     const double kdq4 = kq5 * 5.0 ;
     const double kdq3 = kq4 * 4.0 ;
     const double kdq2 = kq3 * 3.0 ;
     const double kdq1 = kq2 * 2.0 ;
     const double kdq0 = kq1 * 1.0 ;

     const double q5 =      + kq5 ;
     const double q4 = q5*t + kq4 ;
     const double q3 = q4*t + kq3 ;
     const double q2 = q3*t + kq2 ;
     const double q1 = q2*t + kq1 ;
     const double q  = q1*t       ;

     const double dq4   =         kdq4 ;
     const double dq3   = dq4*t + kdq3 ;
     const double dq2   = dq3*t + kdq2 ;
     const double dq1   = dq2*t + kdq1 ;
     const double dqdt  = dq1*t + kdq0 ;


     const double a=-z*z ;
     const double b=hexp(a) ;
     const double f=q*b ;
     const double dtdz = -kp*t*t ;
     const double dadz = -2.0*z ;
     const double dbdz = dadz*b ;
     const double dqdz = dqdt*dtdz ;
     const double dfdz = dqdz*b + q*dbdz ;

     fx = fsel(x,f,2.0-f) ;
     dfdx = dfdz ;

  }
#endif

  /*
   * Wrapper to match function which just returns 'erfc'
   */
  static inline double erfcc(double x)
  {
     double dummy ;
     double result ;
     erfccd(x,result,dummy) ;
     return result ;
  }



};


#ifndef MSD_COMPILE_CODE_ONLY
const double Math::hlogTable[16] = {
  0.0 // ::log(1.0) ,
  , 0.06062462181643484 // ::log(1.0 + 1.0/16.0),
  , 0.11778303565638346 // ::log(1.0 + 2.0/16.0),
  , 0.17185025692665923 // ::log(1.0 + 3.0/16.0),
  , 0.22314355131420976 // ::log(1.0 + 4.0/16.0),
  , 0.27193371548364176 // ::log(1.0 + 5.0/16.0),
  , 0.31845373111853459 // ::log(1.0 + 6.0/16.0),
  , 0.36290549368936847 // ::log(1.0 + 7.0/16.0),
  , 0.40546510810816438 // ::log(1.0 + 8.0/16.0),
  , 0.44628710262841953 // ::log(1.0 + 9.0/16.0),
  , 0.48550781578170082 // ::log(1.0 + 10.0/16.0),
  , 0.52324814376454787 // ::log(1.0 + 11.0/16.0),
  , 0.55961578793542266 // ::log(1.0 + 12.0/16.0),
  , 0.59470710774669278 // ::log(1.0 + 13.0/16.0),
  , 0.62860865942237409 // ::log(1.0 + 14.0/16.0),
  , 0.66139848224536502 // ::log(1.0 + 15.0/16.0)
} ;
const double Math::hlogComp[16] = {
  1.0 / ( 1.0 + 0.0/16.0 ) ,
  1.0 / ( 1.0 + 1.0/16.0 ) ,
  1.0 / ( 1.0 + 2.0/16.0 ) ,
  1.0 / ( 1.0 + 3.0/16.0 ) ,
  1.0 / ( 1.0 + 4.0/16.0 ) ,
  1.0 / ( 1.0 + 5.0/16.0 ) ,
  1.0 / ( 1.0 + 6.0/16.0 ) ,
  1.0 / ( 1.0 + 7.0/16.0 ) ,
  1.0 / ( 1.0 + 8.0/16.0 ) ,
  1.0 / ( 1.0 + 9.0/16.0 ) ,
  1.0 / ( 1.0 + 10.0/16.0 ) ,
  1.0 / ( 1.0 + 11.0/16.0 ) ,
  1.0 / ( 1.0 + 12.0/16.0 ) ,
  1.0 / ( 1.0 + 13.0/16.0 ) ,
  1.0 / ( 1.0 + 14.0/16.0 ) ,
  1.0 / ( 1.0 + 15.0/16.0 )
} ;
#if defined(PK_BLADE)
// For BlueGene (simulation), we don't have trig libraries, so set up the constants numerically
const double Math::OneThird   = 1.0 / 3.0;
const double Math::PI      = M_PI;
const double Math::PI_2    = M_PI_2;
const double Math::PI_4    = M_PI_4;
const double Math::InvPI   = M_1_PI;
const double Math::SqrtPI  = sqrt(M_PI);
const double Math::InvSqrtPI  = 1.0/sqrt(M_PI);

// const double Math::Deg2Rad = M_PI / 180.0;
const double Math::Rad2Deg = 180.0 / M_PI;

const double Math::E       = M_E;
const double Math::InvE    = 1.0/M_E;
const double Math::Log2E   = M_LOG2E;
const double Math::Log10E  = M_LOG10E;

const double Math::Log2    = M_LN2;
// const double Math::Log10   = log(10.0);
const double Math::Sqrt2   = sqrt(2.0);
const double Math::Sqrt2_2 = sqrt(2.0)/2.0;


#ifdef DBL_MAX
const double Math::Infinity = DBL_MAX * 2.0 ;  // Intended to overflow; will be used as result of hexp(x) for x > 709
#else
#if defined(PK_BGL)
// BG/L compiler doesn't support HUGE_VAL quite the way we want ...
const double Math::Infinity = (1e200*1e200) ; // Intended to overflow
#else
const double Math::Infinity = HUGE_VAL * HUGE_VAL ;
#endif
#endif
const double ExpK1 = M_LN2/16.0 ;
const double ExpK2 = M_LN2/256.0 ;
const double Math::ExpTable1[16] = {
//      ::exp( 0.0*ExpK1) ,       ::exp( 1.0*ExpK1) ,       ::exp( 2.0*ExpK1) ,       ::exp( 3.0*ExpK1) ,
        1.00000000000000000000 ,  1.04427378242741383881 ,  1.09050773266525765605 ,  1.13878863475669164875
//      ::exp( 4.0*ExpK1) ,       ::exp( 5.0*ExpK1) ,       ::exp( 6.0*ExpK1) ,       ::exp( 7.0*ExpK1) ,
     ,  1.18920711500272105982 ,  1.24185781207348403959 ,  1.29683955465100965466 ,  1.35425554693689271456
//      ::exp( 8.0*ExpK1) ,       ::exp( 9.0*ExpK1) ,       ::exp(10.0*ExpK1) ,       ::exp(11.0*ExpK1) ,
     ,  1.41421356237309503240 ,  1.47682614593949929212 ,  1.54221082540794080126 ,  1.61049033194925428250
//      ::exp(12.0*ExpK1) ,       ::exp(13.0*ExpK1) ,       ::exp(14.0*ExpK1) ,       ::exp(15.0*ExpK1) ,
     ,  1.68179283050742905681 ,  1.75625216037329945002 ,  1.83400808640934242627 ,  1.91520656139714725223

   } ;
const double Math::ExpTable2[16] = {
//      ::exp( 0.0*ExpK2) ,       ::exp( 1.0*ExpK2) ,       ::exp( 2.0*ExpK2) ,       ::exp( 3.0*ExpK2) ,
        1.00000000000000000000 ,  1.00271127505020248534 ,  1.00542990111280282117 ,  1.00815589811841751551
//      ::exp( 4.0*ExpK2) ,       ::exp( 5.0*ExpK2) ,       ::exp( 6.0*ExpK2) ,       ::exp( 7.0*ExpK2) ,
     ,  1.01088928605170045965 ,  1.01363008495148943838 ,  1.01637831491095303739 ,  1.01913399607773794904
//      ::exp( 8.0*ExpK2) ,       ::exp( 9.0*ExpK2) ,       ::exp(10.0*ExpK2) ,       ::exp(11.0*ExpK2) ,
     ,  1.02189714865411667749 ,  1.02466779289713564431 ,  1.02744594911876369561 ,  1.03023163768604101185
//      ::exp(12.0*ExpK2) ,       ::exp(13.0*ExpK2) ,       ::exp(14.0*ExpK2) ,       ::exp(15.0*ExpK2) ,
     ,  1.03302487902122842138 ,  1.03582569360195711881 ,  1.03863410196137878930 ,  1.04145012468831613985
   } ;

#else
const double Math::OneThird   = 1.0 / 3.0;
const double Math::PI      = 4*atan(1.0);
const double Math::PI_2    = 2*atan(1.0);
const double Math::PI_4    = atan(1.0);
const double Math::InvPI   = 0.25/atan(1.0);
const double Math::SqrtPI  = 2*sqrt(atan(1.0));
const double Math::InvSqrtPI  = 0.5/sqrt(atan(1.0));

// const double Math::Deg2Rad = 4 * atan(1.0) / 180.0;
const double Math::Rad2Deg = 180.0 / 4 / atan(1.0);

const double Math::E       = exp(1.0);
const double Math::InvE    = 1/exp(1.0);
const double Math::Log2E   = 1/log(2.0);
const double Math::Log10E  = 1/log(10.0);

const double Math::Log2    = log(2.0);
const double Math::Log10   = log(10.0);
const double Math::Sqrt2   = sqrt(2.0);
const double Math::Sqrt2_2 = sqrt(2.0)/2;


#ifdef DBL_MAX
const double Math::Infinity = DBL_MAX * 2.0 ;  // Intended to overflow; will be used as result of hexp(x) for x > 709
#else
const double Math::Infinity = HUGE_VAL * HUGE_VAL ;
#endif
const double ExpK1 = M_LN2/16.0 ;
const double ExpK2 = M_LN2/256.0 ;
const double Math::ExpTable1[16] = {
   ::exp( 0.0*ExpK1) , ::exp( 1.0*ExpK1) , ::exp( 2.0*ExpK1) , ::exp( 3.0*ExpK1) ,
   ::exp( 4.0*ExpK1) , ::exp( 5.0*ExpK1) , ::exp( 6.0*ExpK1) , ::exp( 7.0*ExpK1) ,
   ::exp( 8.0*ExpK1) , ::exp( 9.0*ExpK1) , ::exp(10.0*ExpK1) , ::exp(11.0*ExpK1) ,
   ::exp(12.0*ExpK1) , ::exp(13.0*ExpK1) , ::exp(14.0*ExpK1) , ::exp(15.0*ExpK1) ,

   } ;
const double Math::ExpTable2[16] = {
   ::exp( 0.0*ExpK2) , ::exp( 1.0*ExpK2) , ::exp( 2.0*ExpK2) , ::exp( 3.0*ExpK2) ,
   ::exp( 4.0*ExpK2) , ::exp( 5.0*ExpK2) , ::exp( 6.0*ExpK2) , ::exp( 7.0*ExpK2) ,
   ::exp( 8.0*ExpK2) , ::exp( 9.0*ExpK2) , ::exp(10.0*ExpK2) , ::exp(11.0*ExpK2) ,
   ::exp(12.0*ExpK2) , ::exp(13.0*ExpK2) , ::exp(14.0*ExpK2) , ::exp(15.0*ExpK2) ,
   } ;

#endif

#endif

// SI units or IU (internal units)
// Other things such as Avagadro's number and conversion factors have no units
struct SciConst
{
        const static double kg2amu;
        const static double kcal2joule;
        const static double ECharge_SI;
        const static double Avagadro;
        const static double Planck_SI;
        const static double StdChargeConversion;
        const static double OldChargeConversion;
        const static double AmberChargeConversion;
        const static double ImpactChargeConversion;
        const static double StdTimeConversion;
        const static double OldTimeConversion;
        const static double AmberTimeConversion;
        const static double c_SI;
        const static double KBoltzmann_SI;
        const static double KBoltzmann_IU;
        const static double Atm_IU;
        const static double Atm2Pa;
        const static double SixthRootOfTwo;
};

#ifndef MSD_COMPILE_CODE_ONLY
#define KECharge_SI            1.602176462E-19         // (C) from NIST, 7/01
#define KAvagadro              6.02214199E+23          // (per mole) from NIST, 7/01
#define KPlanck_SI             6.62606876E-34          // (J s) from NIST, 7/01
#define KKBoltzmann_SI         1.3806503E-23           // (J/K) from NIST, 7/01

#define Kc_SI                  299792458               // (m/s) - exact, defines meter (NIST)
#define Kkcal2joule            4184                    // exact - defines kcal
#define KAtm2Pa                101325                  // from nist, 9/01
#define Kkg2amu                (0.001 / KAvagadro)

const double SciConst::ECharge_SI           = 1.602176462E-19;         // (C) from NIST, 7/01
const double SciConst::Avagadro             = 6.02214199E+23;          // (per mole) from NIST, 7/01
const double SciConst::Planck_SI            = 6.62606876E-34;          // (J s) from NIST, 7/01
const double SciConst::KBoltzmann_SI        = 1.3806503E-23;           // (J/K) from NIST, 7/01

const double SciConst::c_SI                 = 299792458;               // (m/s) - exact, defines meter (NIST)
const double SciConst::kcal2joule           = 4184;                    // exact - defines kcal
const double SciConst::Atm2Pa               = 101325;                  // from nist, 9/01
const double SciConst::kg2amu               = 0.001 / SciConst::Avagadro;

//  0.001987207
//  Previous BlueMatter value was 0.00198718
const double SciConst::KBoltzmann_IU        =
    SciConst::KBoltzmann_SI * SciConst::Avagadro / SciConst::kcal2joule;

//  1.458397555E-5 kcal/A^3/mole
const double SciConst::Atm_IU =
    SciConst::Atm2Pa / SciConst::kcal2joule * 1.0E-30 * SciConst::Avagadro;

//  18.222615334920 = sqrt(332.06370964448)
//  For reference, BlueMatter previously used 18.2224^2 = 332.0558618 and Impact uses 18.222617^2 = 332.063762
//#define KStdChargeConversion     (Kc_SI * KECharge_SI * sqrt( 1000 * KAvagadro / Kkcal2joule ))
#define KStdChargeConversion     18.222615334920497787
const double SciConst::StdChargeConversion     =
    SciConst::c_SI * SciConst::ECharge_SI * sqrt( 1000 * SciConst::Avagadro / SciConst::kcal2joule );

const double SciConst::OldChargeConversion = 18.2224;

//   AMBER uses 18.2223 for charge conversions, JWP 2002
const double SciConst::AmberChargeConversion = 18.2223;

const double SciConst::ImpactChargeConversion = sqrt( 332.063762 );

//  0.048888212937617
//  Previous BlueMatter value was 0.0488882
//  const double SciConst::StdTimeConversion       =
//    100 * sqrt( SciConst::kg2amu * SciConst::Avagadro / SciConst::kcal2joule );
const double SciConst::StdTimeConversion =
    sqrt( 10.0 / SciConst::kcal2joule );

const double SciConst::OldTimeConversion = 0.0488882;

//  AMBER uses a conversion of 1/20.455 = 0.0488878;
const double SciConst::AmberTimeConversion = 1.0 / 20.445;

#if defined(PK_BGL)
const double SciConst::SixthRootOfTwo = 1.12246204830937 ;
#else
const double SciConst::SixthRootOfTwo = exp(log(2.0)/6.0);
#endif

#endif

class Heuristics
{
public:
        const static double EpsilonCrossProduct;
        const static double EpsilonVectorLength;
        const static double EpsilonVectorSquareLength;
public:
        static inline int IsCrossProductZero(double x) { return (fabs(x) < EpsilonCrossProduct) ? 1 : 0; }
        static inline int IsVectorLengthZero(double x) { return (x < EpsilonVectorLength) ? 1 : 0; }
        static inline int IsVectorSquareLengthZero(double x) { return (x < EpsilonVectorSquareLength) ? 1 : 0; }
};

#ifndef MSD_COMPILE_CODE_ONLY
const double Heuristics::EpsilonCrossProduct = 1.0E-15;
const double Heuristics::EpsilonVectorLength = 1.0E-10;
const double Heuristics::EpsilonVectorSquareLength = 1.0E-20;
#endif

class Tip3Params
{
public:
  static const double kr;
  static const double r0;
  static const double kth;
  static const double th0;
  static const double sinth0;
  static const double costh0;
};

#ifndef MSD_COMPILE_CODE_ONLY
const double Tip3Params::kr  = 450.0;
const double Tip3Params::r0  =   0.9572;
const double Tip3Params::kth =  55.0;
const double Tip3Params::th0 =   1.824218134;  // 104.52 * Math::Deg2Rad;
#if defined(PK_BGL)
const double Tip3Params::sinth0 = 0.968060182334169 ;
const double Tip3Params::costh0 =-0.250717935894376 ;
#else
const double Tip3Params::sinth0 = sin(th0) ;
const double Tip3Params::costh0 = cos(th0) ;
#endif

#endif

class Comparator
{
public:
  template
  <class Type>
  static
  inline
  int
  Min(Type* array, int length )
    {
      Type minElement = Type::MaxValue();
      int minIndex=-1;

      for( int i=0; i < length; i++ )
        {
        if( array[ i ] <= minElement )
          {
          minElement = array[i];
          minIndex   = i;
          }
        }
      return minIndex;
    }

  template
  <class Type>
  static
  inline
  int
  Max(Type* array, int length)
    {
      Type maxElement= Type::MinValue();
      int maxIndex=-1;

      for( int i=0; i < length; i++ )
        {
          if( array[i] >= maxElement )
            {
              maxElement = array[i];
              maxIndex   = i;
            }
        }
      return maxIndex;
    }

  static
  inline
  int
  Min(int* array, int length )
    {
      unsigned long minElement = 0xffffffff;
      int minIndex = 99999999;

      for( int i=0; i < length; i++ )
        {
        if( array[i] <= minElement )
          {
          minElement = array[i];
          minIndex   = i;
          }
        }
      return minIndex;
    }

};

// This 'cube root' wanted for piston mass in the MSD
#if defined(PK_BLADE)
#undef cbrt
#define cbrt(x) BlueMatterCbrt(x)
static inline double BlueMatterCbrt(double x)
{
  return Math::hexp(Math::hlog(x) / 3.0) ;
}
#endif

class NHC
{

public:

  // TAU   ( derived using a dT)
  // INHC  (ctp)
  // NRESN (ctp)

  enum{ NUMBER_OF_THERMOSTATS = 2 };
  enum{ NYOSH=9 };

  static double wdt[ NYOSH ];
  static double wdti[ NYOSH ];
  static double wdti2[ NYOSH ];
  static double wdti4[ NYOSH ];
  static double wdti8[ NYOSH ];

  static void Init( double aDeltaT )
    {
      wdt[ 0 ] =   0.192;
      wdt[ 1 ] =   0.554910818409783619692725006662999;
      wdt[ 2 ] =   0.124659619941888644216504240951585;
      wdt[ 3 ] =  -0.843182063596933505315033808282941;
      wdt[ 4 ] =   1.0 - 2.0 * ( wdt[ 0 ] + wdt[ 1 ] + wdt[ 2 ] + wdt[ 3 ] );
      wdt[ 5 ] =  -0.843182063596933505315033808282941;
      wdt[ 6 ] =  0.124659619941888644216504240951585;
      wdt[ 7 ] =  0.554910818409783619692725006662999;
      wdt[ 8 ] =  0.192;

      for(int i=0; i< NYOSH; i++)
        {
          double wdtNow = wdt[ i ] * aDeltaT;

          wdti[ i ]  = wdtNow;
          wdti2[ i ] = wdtNow /  2;
          wdti4[ i ] = wdtNow /  4;
          wdti8[ i ] = wdtNow /  8;
        }
    }
};

#ifndef MSD_COMPILE_CODE_ONLY
double NHC::wdt[ NHC::NYOSH ] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
double NHC::wdti[ NHC::NYOSH ] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
double NHC::wdti2[ NHC::NYOSH ] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
double NHC::wdti4[ NHC::NYOSH ] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
double NHC::wdti8[ NHC::NYOSH ] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
#endif

#endif

















