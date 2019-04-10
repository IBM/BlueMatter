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
// File: bmtlowerpackedsymmetricmatrix.cpp
// Author: RSG
// Date: October 12, 2002
// Namespace: bmt
// Class: LowerPackedSymmetricMatrix
// Description: Engine for a symmetric matrix stored in what the ESSL
//              documentation describes as lower packed storage
//              layout. For ESSL (fortran-based), this means that the
//              lower triangle is packed by columns (equivalent to
//              upper triangle being packed by rows).
// 
// Iterator: TIter will be invoked by allUniqueDo() via an overloaded
//           function operator with arguments: double& elt, int i, int j
// ********************************************************************

#include <BlueMatter/bmtlowerpackedsymmetricmatrix.hpp>

namespace bmt
{
  LowerPackedSymmetricMatrix::LowerPackedSymmetricMatrix(int n) :
    Matrix<LowerPackedSymmetricMatrix>(n, n, (n*(n+1))/2)
  {}
}
