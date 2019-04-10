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
 //*****************************************************************
// Utility program to compute high powers of matrices in modulus arithmetic
//*****************************************************************
#include <iostream>
#include <iomanip>
#include <cstdlib>


const long long A0[3][3] =
  {
    {
      0,        1,       0
    },
    {
      0,        0,       1
    },
    {
      -810728,  1403580,       0
    }
  };


const long long A1[3][3] =
  {
    {
      0,           1,       0
    },
    {
      0,           0,       1
    },
    {
      -1370589,    0,       527612
    }
  };

const long long m0 = 4294967087;
const long long m1 = 4294944443;

void powLog2(const long long Arg[3][3], const int n, const unsigned long long m);

int main(int argc, char** argv)
{
  int n = 76;
  if (argc > 1)
    {
      n = std::atoi(argv[1]);
    }

  powLog2(A0, n, m0);
  std::cout << std::endl;
  powLog2(A1, n, m1);
  std::cout << std::endl;
  return(0);
}

void powLog2(const long long Arg[3][3], const int n, const unsigned long long m)
{
  long long tmp;
  unsigned long long A[3][3];
  unsigned long long B[3][3];
  unsigned long long C[3][3];
  for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
        {
          tmp = Arg[i][j];
          while (tmp < 0)
            {
              tmp += m; // ensure that tmp >= 0;
            }
          A[i][j] = tmp;
          B[i][j] = A[i][j];
          C[i][j] = A[i][j];
        }
    }
  for (int r = 0; r < n; ++r)
    {
      for (int i = 0; i < 3; ++i)
        {
          for (int j = 0; j < 3; ++j)
            {
              C[i][j] = 
                (A[i][0]*B[0][j] % m);
#if defined(RSG_VERBOSE)
              std::cerr << "power::powLog2: r = " << r 
                        << "  A[" << i << "][0]*B[0]["
                        << j << "] % " << std::setw(12) << m << " = "
                        << std::setw(12) << A[i][0] << " * " 
                        << std::setw(12) << B[0][j] << " = "
                        << std::setw(12) << C[i][j]
                        << "  Accum = " << std::setw(12)  << C[i][j]
                        << std::endl;
#endif
              long long cTmp = (A[i][1]*B[1][j] % m);
#if defined(RSG_VERBOSE)
              std::cerr << "power::powLog2: r = " << r 
                        << "  A[" << i << "][1]*B[1]["
                        << j << "] % " << std::setw(12) << m << " = " 
                        << std::setw(12) << A[i][1] << " * " 
                        << std::setw(12) << B[1][j] << " = "
                        << std::setw(12) << cTmp;
#endif
              C[i][j] += cTmp;
#if defined(RSG_VERBOSE)
              std::cerr << "  Accum = " << std::setw(12)
                        << cTmp << std::endl;
#endif
              cTmp = (A[i][2]*B[2][j] % m);
#if defined(RSG_VERBOSE)
              std::cerr << "power::powLog2: r = " << r 
                        << "  A[" << i << "][2]*B[2]["
                        << j << "] % " << std::setw(12) << m << " = " 
                        << std::setw(12) << A[i][2] << " * " 
                        << std::setw(12) << B[2][j] << " = "
                        << std::setw(12) << cTmp;
#endif
              C[i][j] += cTmp;
#if defined(RSG_VERBOSE)
              std::cerr << "  Accum = " << std::setw(12)
                        << C[i][j] << std::endl;
#endif
              C[i][j] = C[i][j] % m;
#if defined(RSG_VERBOSE)
              std::cerr << "r = " << r 
                        << "  C[" << i << "][" << j << "] % "
                        << std::setw(12) << m 
                        << " = " << std::setw(12)
                        << C[i][j] << std::endl;
#endif              
              if (C[i][j] < 0)
                {
                  std::cerr << "ERROR: C[i][j] = " << C[i][j] << " < 0" << std::endl;
                  C[i][j] += m;
                }
            }
        }
      for (int i = 0; i < 3; ++i)
        {
          for (int j = 0; j < 3; ++j)
            {
              A[i][j] = C[i][j];
              B[i][j] = C[i][j];
            }
        }
#if defined(RSG_VERBOSE)
      std::cerr << "Power = " << r << std::endl;
      for (int i = 0; i < 3; ++i)
        {
          for (int j = 0; j < 3; ++j)
            {
              std::cerr << std::setw(20) << C[i][j] << " ";
            }
          std::cerr << std::endl;
        }
#endif
    } // end loop over r

  for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
        {
          std::cout << std::setw(20) << C[i][j] << " ";
        }
      std::cout << std::endl;
    }
  

#if 0
  long long even[3][3];
  long long odd[3][3];
  for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
        {
          even[i][j]=A0[i][j];
        }
    }

  bool isEven = true;
  for (int r = 0; r < n; ++r)
    {
      if (isEven)
        {
          for (int i = 0; i < 3; ++i)
            {
              for (int j = 0; j < 3; ++j)
                {
                  odd[i][j] = 0;
                  for (int k = 0; k < 3; ++k)
                    {
                      odd[i][j] = (odd[i][j] + even[i][k]*even[k][j]);
                    }
                  odd[i][j] = odd[i][j] % m;
                  while(odd[i][j] < 0)
                    {
                      odd[i][j] = odd[i][j] + m;
                    }
                }
            }
          isEven = false;
        }
      else // r is odd
        {
          for (int i = 0; i < 3; ++i)
            {
              for (int j = 0; j < 3; ++j)
                {
                  even[i][j] = 0;
                  for (int k = 0; k < 3; ++k)
                    {
                      even[i][j] = (even[i][j] + odd[i][k]*odd[k][j]);
                    }
                  even[i][j] = even[i][j] % m;
                  while(even[i][j] < 0)
                    {
                      even[i][j] = even[i][j] + m;
                    }
                }
            }
          isEven = true;
        }
    }
  for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
        {
          std::cout << (isEven ? even[i][j] : odd[i][j]) << "  ";
        }
      std::cout << std::endl;
    }
#endif
}
