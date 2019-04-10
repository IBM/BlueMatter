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
 // *********************************************************************
// Routine to compute high powers of matrices using L'Ecuyer's code
// *********************************************************************

#include <cstdlib>
#include <iostream>
#include <iomanip>

namespace rng
{
  const double m1   =       4294967087.0;
  const double m2   =       4294944443.0;
  const double norm =       1.0 / ( m1 + 1.0 );
  const double a12  =       1403580.0;
  const double a13n =       810728.0;
  const double a21  =       527612.0;
  const double a23n =       1370589.0;
  const double two17 =      131072.0;
  const double two53 =      9007199254740992.0;
  const double fact =       5.9604644775390625e-8;     /* 1 / 2^24  */

// The following are the transition matrices of the two MRG components
// (in matrix form), raised to the powers -1, 1, 2^76, and 2^127, resp.

  const double InvA1[3][3] =
    {          // Inverse of A1p0
      { 184888585.0,   0.0,  1945170933.0 },
      {         1.0,   0.0,           0.0 },
      {         0.0,   1.0,           0.0 }
    };

  const double InvA2[3][3] =
    {          // Inverse of A2p0
      {      0.0,  360363334.0,  4225571728.0 },
      {      1.0,          0.0,           0.0 },
      {      0.0,          1.0,           0.0 }
    };

  const double A1p0[3][3] =
    {
      {
        0.0,        1.0,       0.0
      },
      {       0.0,        0.0,       1.0 },
      { -810728.0,  1403580.0,       0.0 }
    };

  const double A2p0[3][3] =
    {
      {
        0.0,        1.0,       0.0
      },
      {        0.0,        0.0,       1.0 },
      { -1370589.0,        0.0,  527612.0 }
    };

  const double A1p76[3][3] =
    {
      {
        82758667.0, 1871391091.0, 4127413238.0
      },
      {    3672831523.0,   69195019.0, 1871391091.0 },
      {    3672091415.0, 3528743235.0,   69195019.0 }
    };

  const double A2p76[3][3] =
    {
      {
        1511326704.0, 3759209742.0, 1610795712.0
      },
      {    4292754251.0, 1511326704.0, 3889917532.0 },
      {    3859662829.0, 4292754251.0, 3708466080.0 }
    };

  const double A1p127[3][3] =
    {
      {
        2427906178.0, 3580155704.0,  949770784.0
      },
      {     226153695.0, 1230515664.0, 3580155704.0 },
      {    1988835001.0,  986791581.0, 1230515664.0 }
    };

  const double A2p127[3][3] =
    {
      {
        1464411153.0,  277697599.0, 1610723613.0
      },
      {      32183930.0, 1464411153.0, 1022607788.0 },
      {    2824425944.0,   32183930.0, 2093834863.0 }
    };



  //-------------------------------------------------------------------------
  // Return (a*s + c) MOD m; a, s, c and m must be < 2^35
  //
  static double MultModM ( double a, double s, double c, double m )
  {
    double v;
    long a1;

    v = a * s + c;

#if defined(RSG_VERBOSE)
    std::cerr.precision(16);
    std::cerr << "MultModM: "
              << " m = " << std::setw(12) <<  m
              << " a = " << std::setw(12) << a
              << " s = " << std::setw(12) << s
              << " c = " << std::setw(12) << c;
#endif

    if ( v >= two53 || v <= -two53 )
    {
      a1 = static_cast<long> ( a / two17 );
      a -= a1 * two17;
      v  = a1 * s;
      a1 = static_cast<long> ( v / m );
      v -= a1 * m;
      v = v * two17 + a * s + c;
    }

    a1 = static_cast<long> ( v / m );
    /* in case v < 0)*/
#if defined(RSG_VERBOSE)
    std::cerr << " v = " << std::setw(12) 
              << v - a1*m < 0 ? v - (a1*m) + m : v - a1*m;
    std::cerr << std::endl;
#endif
    if ( ( v -= a1 * m ) < 0.0 ) return v += m;
    else return v;
  }


//-------------------------------------------------------------------------
// Compute the vector v = A*s MOD m. Assume that -m < s[i] < m.
// Works also when v = s.
//
  static void MatVecModM ( const double A[3][3], const double s[3], double v[3],
                           double m, int col ) // is the column of the result
  {
    int i;
    double x[3];               // Necessary if v = s

    for ( i = 0; i < 3; ++i )
    {
#if defined(RSG_VERBOSE)
      std::cerr << "MatVecModM::";
#endif
      x[i] = MultModM ( A[i][0], s[0], 0.0, m );
#if defined(RSG_VERBOSE)
      std::cerr << "MatVecModM::";
#endif
      x[i] = MultModM ( A[i][1], s[1], x[i], m );
#if defined(RSG_VERBOSE)
      std::cerr << "MatVecModM::";
#endif
      x[i] = MultModM ( A[i][2], s[2], x[i], m );
#if defined(RSG_VERBOSE)
      std::cerr << "C[" << i << "][" << col << "] = " << x[i] << std::endl;
#endif
    }
    for ( i = 0; i < 3; ++i )
      v[i] = x[i];
  }


//-------------------------------------------------------------------------
// Compute the matrix C = A*B MOD m. Assume that -m < s[i] < m.
// Note: works also if A = C or B = C or A = B = C.
//
  static void MatMatModM ( const double A[3][3], const double B[3][3],
                           double C[3][3], double m )
  {
    int i, j;
    double V[3], W[3][3];

    for ( i = 0; i < 3; ++i )
    {
      for ( j = 0; j < 3; ++j )
        V[j] = B[j][i];
      MatVecModM ( A, V, V, m, i );
      for ( j = 0; j < 3; ++j )
        W[j][i] = V[j];
    }
    for ( i = 0; i < 3; ++i )
      for ( j = 0; j < 3; ++j )
        C[i][j] = W[i][j];
  }


//-------------------------------------------------------------------------
// Compute the matrix B = (A^(2^e) Mod m);  works also if A = B.
//
  static void MatTwoPowModM ( const double A[3][3], double B[3][3], double m, long e )
  {
    int i, j;

    /* initialize: B = A */
    if ( A != B )
    {
      for ( i = 0; i < 3; ++i )
        for ( j = 0; j < 3; ++j )
          B[i][j] = A[i][j];
    }
    /* Compute B = A^(2^e) mod m */
    for ( i = 0; i < e; i++ )
      {
        MatMatModM ( B, B, B, m );
#if defined(RSG_VERBOSE)
        std::cerr << "Power = " << i << std::endl;
        for (int i1 = 0; i1 < 3; ++i1)
          {
            for (int j1 = 0; j1 < 3; ++j1)
              {
                std::cerr << std::setw(12) << B[i1][j1] << " ";
              }
            std::cerr << std::endl;
          }
#endif
      }
  }


//-------------------------------------------------------------------------
// Compute the matrix B = (A^n Mod m);  works even if A = B.
//
  static void MatPowModM ( const double A[3][3], double B[3][3], double m, long n )
  {
    int i, j;
    double W[3][3];

    /* initialize: W = A; B = I */
    for ( i = 0; i < 3; ++i )
      for ( j = 0; j < 3; ++j )
      {
        W[i][j] = A[i][j];
        B[i][j] = 0.0;
      }
    for ( j = 0; j < 3; ++j )
      B[j][j] = 1.0;

    /* Compute B = A^n mod m using the binary decomposition of n */
    while ( n > 0 )
    {
      if ( n % 2 ) MatMatModM ( W, B, B, m );
      MatMatModM ( W, W, W, m );
      n /= 2;
    }
  }

}


int main(int argc, char** argv)
{
  int n = 76;
  if (argc > 1)
    {
      n = std::atoi(argv[1]);
    }

  double B[3][3];

  rng::MatTwoPowModM ( rng::A1p0, B, rng::m1, n );
  for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
        {
          long long tmp = B[i][j];
          std::cout << tmp << "  ";
        }
      std::cout << std::endl;
    }

  std::cout << std::endl;

  rng::MatTwoPowModM ( rng::A2p0, B, rng::m2, n );
  for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
        {
          long long tmp = B[i][j];
          std::cout << tmp << "  ";
        }
      std::cout << std::endl;
    }
  return(0);
}

