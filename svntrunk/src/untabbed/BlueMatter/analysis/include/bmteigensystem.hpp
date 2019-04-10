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
// File: bmteigensystem.hpp
// Author: RSG
// Date: October 12, 2002
// Namespace: bmt
// Class: BmtEigensystem
// Description: Wrapper function for ESSL (or other library)
//              Eigensystem routines
// Template parameter: TEngine
// 
// ********************************************************************

#ifndef INCLUDE_BMT_BMTEIGENSYSTEM_HPP
#define INCLUDE_BMT_BMTEIGENSYSTEM_HPP

#include <BlueMatter/bmtmatrix.hpp>
#include <BlueMatter/bmtcolumnmajormatrix.hpp>
#include <BlueMatter/bmtlowerpackedsymmetricmatrix.hpp>

#include <cstdlib>

namespace bmt
{
  template <class TEngine>
  int eigensystem(const Matrix<TEngine>& m,
      std::vector<double>& eigenvalues,
      Matrix<ColumnMajorMatrix>& eigenvectors)
  {
    // matrix must be square
    if (m.rows() != m.cols())
      {
  return(-1);
      }
    return(-2); // no implementation for general matrix
  }

  template <>
  int 
  eigensystem<LowerPackedSymmetricMatrix>(const
            Matrix<LowerPackedSymmetricMatrix>&,
            std::vector<double>&,
            Matrix<ColumnMajorMatrix>&);

  int 
  eigensystem(const Matrix<LowerPackedSymmetricMatrix>&,
        std::vector<double>&,
        Matrix<ColumnMajorMatrix>&,
        int); // integer specifying number of largest
  // eigenvalues (and corresponding eigenvectors) to compute
            
}

#endif
