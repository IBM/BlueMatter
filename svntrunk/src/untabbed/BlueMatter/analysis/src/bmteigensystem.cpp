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
 // ********************************************************************
// File: bmteigensystem.cpp
// Author: RSG
// Date: October 12, 2002
// Namespace: bmt
// Class: BmtEigensystem
// Description: Wrapper function for ESSL (or other library)
//              Eigensystem routines
// Template parameter: TEngine
// 
// ********************************************************************

#include <BlueMatter/bmteigensystem.hpp>
#include <cstdlib>
#include <essl.h>

namespace bmt
{
  template <>
  int 
  eigensystem<LowerPackedSymmetricMatrix>(const
            Matrix<LowerPackedSymmetricMatrix>& m,
            std::vector<double>&
            eigenvalues,
            Matrix<ColumnMajorMatrix>&
            eigenvectors)
  {
    // create working matrix for essl routine
    double* a = new double[(m.rows()*(m.rows()+1))/2];
    if (a == NULL)
      {
  return(-1);
      }

    double* a_p = a;
    for (Matrix<LowerPackedSymmetricMatrix>::const_iterator iter =
     m.begin();
   iter != m.end();
   ++iter)
      {
  *a_p = *iter;
  ++a_p;
      }

    // check size of vector for eigenvalue return
    if (eigenvalues.size() < m.rows())
      {
  return(-2);
      }

    // check size of matrix for eigenvector return
    if ((eigenvectors.rows() != m.rows()) || (eigenvectors.cols() !=
                m.cols()))
      {
  return(-3);
      }

    // Doing something a little cheesy here, assuming that vector's
    // memory is stored as single contiguous block

    double* w = &(*eigenvalues.begin()); // return area for
    // eigenvalues
    
    double* z = &(*eigenvectors.begin()); // return area for
    // eigenvectors

    int ldz = m.rows();
    int n = m.rows();
    
    dspev(1, a, w, z, ldz, n, NULL, 0);
    
    delete [] a;
    return(0);
  }

  int 
  eigensystem(const Matrix<LowerPackedSymmetricMatrix>& m,
        std::vector<double>& eigenvalues,
        Matrix<ColumnMajorMatrix>& eigenvectors,
        int count)
  {
    // Are we computing count largest or -count smallest eigenvalues?
    int iopt = count < 0 ? 1 : 11;
    count = count < 0 ? -count : count;

    // create working matrix for essl routine
    double* a = new double[(m.rows()*(m.rows()+1))/2];
    if (a == NULL)
      {
  return(-1);
      }

    double* a_p = a;
    for (Matrix<LowerPackedSymmetricMatrix>::const_iterator iter =
     m.begin();
   iter != m.end();
   ++iter)
      {
  *a_p = *iter;
  ++a_p;
      }

    // check size of vector for eigenvalue return
    if (eigenvalues.size() < m.rows())
      {
  return(-2);
      }

    // check size of matrix for eigenvector return
    if ((eigenvectors.rows() != m.rows()) || (eigenvectors.cols() !=
                m.cols()))
      {
  return(-3);
      }

    // Doing something a little cheesy here, assuming that vector's
    // memory is stored as single contiguous block

    double* w = &(*eigenvalues.begin()); // return area for
    // eigenvalues
    
    double* z = &(*eigenvectors.begin()); // return area for
    // eigenvectors

    int ldz = m.rows();
    int n = m.rows();

    dspsv(iopt, a, w, z, ldz, n, count, NULL, 0);

    delete [] a;
    return(0);
  }
}
