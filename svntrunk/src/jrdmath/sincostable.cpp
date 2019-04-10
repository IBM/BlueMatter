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

#include <iostream>
#include <iomanip>
using namespace std ;
#include <math.h>

#define UNINIT_SINCOSTABLE
/*
 * This evaluates 'sin' or 'cos' of an angle, as a single basic block (no branches)
 * so the compiler can schedule the evaluation interleaved with other work.
 *
 * The angle range is split into (-45, 45), (45, 135), (135, 225), (225, 315) degrees
 * and repeating. According as the range and whether we want 'sin' or 'cos', a
 * suitable even function (sin(x)/x or cos(x)) is evaluated as a Chebyshev
 * polynomial. This is then compensated by multiplication by the appropriate one of
 * +1, +x, -1, or -x, giving the required result
 *
 * As convenional, the argument is taken in radians.
 *
 * The tables for the coefficients of the Chebyshev polynomials are set up separately
 *
 * These tables are good for 8 coefficients, but only the first 7 are used to get to
 * double precision
 */

#define A_PI 3.14159265358979323846264338327950288

class TrigConstants
{
   public:
        enum {
                k_Diagnose = 0 ,
                k_ChebSize = 7
        } ;
} ;


static long double SinCosChebTable [2][TrigConstants::k_ChebSize+1] ;

double dk1 = 1.0 ;

class Trig: public TrigConstants
{
        public:
        /*
         * ChebyshevEvaluate is based on 'CSEVL' from the freely-redistributable SLATEC library http://www.netlib.org/slatec
         * Further references are in 'Numerical Recipes' http://www.nr.com/ and 'gsl_cheb_eval' from the
         * GNU Scientific Library at http://www.gnu.org/software/gsl/
         * The compiler unrolls the loop fully, for reasonable 'm'
         */
        template <int m>
        static inline double ChebyshevEvaluate(
                                 long double c[m]
                                ,double x)
        {
                double d=0.0 ;
                double dd=0.0 ;
                double x2=2.0*x;
                for (int j=m-1;j>=1;j -= 1) {
                        double sv=d;
                        d=x2*d-dd+c[j];
                        dd=sv;
                }
                return x*d-dd+0.5*c[0];
        }
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
        static double Sin(double Angle)
        {
                /* Separate domain into
                 * -0.5 .. 0.5 : -45 degree to +45 degrees
                 * 0.5 .. 1.5 : 45 degrees to 135 degrees
                 * and so on
                 */
                double Quadrant = Angle * (2.0/A_PI) ;
                double NearestInt = NearestInteger(Quadrant) ;
                int iQuadrant = NearestInt;
                double Remainder = Quadrant - NearestInt ;
                int iTable = (iQuadrant & 1) ;
                int iSign = (iQuadrant & 2) >> 1 ;
                double ChebVariable = Remainder * 2.0 ;
                double ptable[4] ;
                double stable[4]  ;

                ptable[0] = ChebVariable ;
                ptable[1] = 1.0 ;
                ptable[2] = -ChebVariable ;
                ptable[3] = -1.0 ;

 				stable[0] = Angle - NearestInt*(A_PI/2.0) ;
                stable[1] = 1.0 ;
 				stable[2] = NearestInt*(A_PI/2.0) - Angle ;
                stable[3] = -1.0 ;

                double f = ChebyshevEvaluate<k_ChebSize>(SinCosChebTable[iTable],2.0*ChebVariable*ChebVariable-1.0);
                double pt = ptable[iQuadrant & 3] ;
                double st = stable[iQuadrant & 3] ;
                double result = f*pt +  st ;

//              if ( k_Diagnose )
//              {
//                      cout << "Trig::Sin(" << Angle << ") "
//                           << "Remainder=" << Remainder
//                       << " iTable=" << iTable
//                       <<     " iSign=" << iSign
//                           << " ChebVariable=" << ChebVariable
//                           << " f=" << f
//                       << " result=" << result
//                       << endl ;
//              }
                return result ;
        }
        static double Cos(double Angle)
        {
                /* Separate domain into
                 * -0.5 .. 0.5 : -45 degree to +45 degrees
                 * 0.5 .. 1.5 : 45 degrees to 135 degrees
                 * and so on
                 */
                double Quadrant = Angle * (2.0/A_PI) ;
                double NearestInt = NearestInteger(Quadrant) ;
                int iQuadrant = NearestInt;
                double Remainder = Quadrant - NearestInt ;
                int iTable = (iQuadrant & 1) ;
                int iSign = (iQuadrant & 2) >> 1 ;
                double ChebVariable = Remainder * 2.0 ;

                double ptable[4] ;
                double stable[4] ;

                ptable[3] = ChebVariable;
                ptable[0] = 1.0 ;
                ptable[1] = -ChebVariable  ;
                ptable[2] = -1.0 ;

  				stable[3] = Angle - NearestInt*(A_PI/2.0) ;
                stable[0] = 1.0 ;
   				stable[1] = NearestInt*(A_PI/2.0) - Angle ;
                stable[2] = -1.0 ;

                double f = ChebyshevEvaluate<k_ChebSize>(SinCosChebTable[1-iTable],2.0*ChebVariable*ChebVariable-1.0);
                double pt = ptable[iQuadrant & 3] ;
                double st = stable[iQuadrant & 3] ;
                double result = f*pt + st ;

//              if ( k_Diagnose )
//              {
//                      cout << "Trig::Cos(" << Angle << ") "
//                           << "Remainder=" << Remainder
//                           << " iTable=" << iTable
//                           << " iSign=" << iSign
//                           << " ChebVariable=" << ChebVariable
//                           << " f=" << f
//                           << " result=" << result
//                           << endl ;
//              }
                return result ;
        }
} ;

#define A_PI_L 3.14159265358979323846264338327950288l
enum {
        k_MaxChebLength = 128
} ;

/*
 * 'chebft' finds the Chebyshev polynomial coefficients for function 'func' on interval (a,b) to order n
 * Suitable implementations are available in 'Numerical Recipes' http://www.nr.com/ or alternatively
 * in 'gsl_cheb_init' in the GNU Scientific Library http://www.gnu.org/software/gsl/
 */
void chebft(long double a
                        , long double b
                        , long double c[]
                        , int n
                        , long double (*func)(long double)) ;
                        




long double sin_function(long double X)
{
        return ( (X == 0.0l ) ? 0.0l : (sinl(X)/X - 1.0l) ) * (A_PI_L*0.25l) ;
}

long double cos_function(long double X)
{
        return cosl(X) - 1.0l ;
}

void InitTrigTable(void)
{
        long double RawChebTable[k_MaxChebLength] ;
        cout << "sin(x)/x table\n" ;
        chebft(-0.25l*A_PI_L,0.25l*A_PI_L,RawChebTable,k_MaxChebLength,sin_function) ;
        for (int ns = 0; ns < Trig::k_ChebSize+1; ns += 1) {
                SinCosChebTable[0][ns] = RawChebTable[2*ns] ;
        }
        cout << "cos(x) table\n" ;
        chebft(-0.25l*A_PI_L,0.25l*A_PI_L,RawChebTable,k_MaxChebLength,cos_function) ;
        for (int nc = 0; nc < Trig::k_ChebSize+1; nc++) {
                SinCosChebTable[1][nc] = RawChebTable[2*nc] ;
        }
}

static void ShowOne(long double v)
{
   if (v < 0.0l)
   {
      v = -v ;
      cout << "-0." ;
   }
   else
   {
      cout << "+0." ;
   } /* endif */
   for (int a=0; a<64; a+=1)
   {
      long double vv = v * 10.0l ;
      int i = vv ;
      cout << i ;
      v = vv - i ;
   } /* endfor */
}


static void ShowOneTable(long double * Table)
{
   cout << "  " ;
   ShowOne(Table[0]) ;
   for (int a=1; a<Trig::k_ChebSize+1; a+=1)
   {
     cout << "\n ," ;
     ShowOne(Table[a]) ;
   } /* endfor */
   cout << "\n" ;
}

static void ShowTrigTable(void)
{
   cout << "{\n" ;
   ShowOneTable(SinCosChebTable[0]) ;
   cout << "} , {\n" ;
   ShowOneTable(SinCosChebTable[1]) ;
   cout << "}\n" ;
}

void TestTrigTable(void)
{
        for (int a = -640; a < 640; a+=40) {
                long double Angle = a*0.01l ;
                long double SinVal = Trig::Sin(Angle) ;
                long double CosVal = Trig::Cos(Angle) ;
                cout << "sin(" << Angle << ")=" << sinl(Angle)
                        << " approx by " << SinVal << " error=" << SinVal-sinl(Angle) << endl ;
                cout << "cos(" << Angle << ")=" << cosl(Angle)
                        << " approx by " << CosVal << " error=" << CosVal-cosl(Angle) << endl ;

        }
}

int main(void)
{
        InitTrigTable() ;
        ShowTrigTable() ;
        TestTrigTable() ;
        return 0 ;
}
