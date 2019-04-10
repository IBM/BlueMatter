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

static const double SinCosChebTable [2][TrigConstants::k_ChebSize+1] = {
{
  -0.0789004058803453350315181615833139406341584620305671897936151525
 ,-0.0391445675270819570174285389007398988869386583741731420316625604
 ,+0.0003045094206789444055815695590884452454366695761311146250488983
 ,-0.0000011235749767964159582142036241454977987906529477530843654213
 ,+0.0000000024140399724137496071057892543727134506131049276895890590
 ,-0.0000000000033916367050375354740011828074494768576077765801943509
 ,+0.0000000000000033580876185142034466964844195879864668971641022093
 ,-0.0000000000000000024689833209931834105086652046149581936814955209
} , {
  -0.2967361725903839745991879698781526954590354296709458772475200482
 ,-0.1464366443908368633207963601399962102709746936143883813474659270
 ,+0.0019214493118146467969071454374523876476540840033801859035372729
 ,-0.0000099649684898293000686691061850349099578334955892601473193098
 ,+0.0000000275765956071873951864383928564160302546849325446961470666
 ,-0.0000000000473994980816484403744256516400929203293229935948172038
 ,+0.0000000000000554954854148518274108762542315929735058906972782761
 ,-0.0000000000000000470970490651755595726933928753403549933019035260
}
}
;


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
                                 const double c[m]
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

               return result ;
        }
} ;
