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
// File: bmtcovariance.cpp
// Author: RSG
// Date: October 13, 2002
// Namespace: bmt
// Class: Covariance
// Description: Class encapsulating the computation of the covariance matrix
//              from a correlation matrix (like bmt::Correlation)
// ********************************************************************

#include <BlueMatter/bmtcovariance.hpp>
#include <cstdlib>
#include <cmath>
#include <iostream>

namespace bmt
{
  Covariance::Covariance() : d_matrix_p(NULL)
  {}

  Covariance::~Covariance()
  {
    delete d_matrix_p;
  }

  const LowerPackedSymmetricMatrix& Covariance::operator()(const LowerPackedSymmetricMatrix& corr)
  {
    if (d_matrix_p != NULL)
      {
	delete d_matrix_p;
	d_matrix_p = NULL;
      }
    d_matrix_p = new LowerPackedSymmetricMatrix(corr.rows());
    if (d_matrix_p == NULL)
      {
	std::cerr << "Covariance: error allocating memory for covariance matrix" << std::endl;
	exit(-1);
      }
    LowerPackedSymmetricMatrix& cov = *d_matrix_p;
    for (int i = 0; i < corr.rows(); ++i)
      {
	cov(i,i) = 1;
	for (int j = 0; j < i; ++j)
	  {
	    cov(i,j) = corr(i,j)/std::sqrt(corr(i,i)*corr(j,j));
	  }
      }
    return cov;
  }
}
