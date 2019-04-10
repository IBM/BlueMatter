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
 // **********************************************************************
// File: testmatrix.cpp
// Author: RSG
// Date: October 10, 2002
// Description: Simple test for bmt::Matrix and related types
// **********************************************************************

#include <BlueMatter/bmtrowmajormatrix.hpp>
#include <BlueMatter/bmtcolumnmajormatrix.hpp>
#include <BlueMatter/bmtsimpleiterator.hpp>
#include <BlueMatter/bmtlowerpackedsymmetricmatrix.hpp>
#include <BlueMatter/bmteigensystem.hpp>

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <algorithm>

int main(int argc, char** argv)
{
  int rows = 10;
  int cols = 10;
  int count = 2;
  switch(argc)
    {
    case 4:
      count = std::atoi(argv[3]);
    case 3:
      rows = std::atoi(argv[1]);
      cols = std::atoi(argv[2]);
      break;
    default:
      std::cerr << argv[0] << " nRows nColumns <count=2>" << std::endl;
      return(1);
    }
  bmt::RowMajorMatrix rowMajorMatrix(rows, cols);
  bmt::SimpleIterator iter;
  std::cout << "RowMajorMatrix Test:" << std::endl;

  std::for_each(rowMajorMatrix.begin(), rowMajorMatrix.end(), iter);

  for (int i = 0; i < rows; ++i)
    {
      for (int j = 0; j < cols; ++j)
	{
	  std::cout << std::setw(12) << std::setprecision(2) << rowMajorMatrix(i,j);
	}
      std::cout << std::endl;
    }

  bmt::ColumnMajorMatrix colMajorMatrix(rows, cols);
  bmt::SimpleIterator iter2;
  std::cout << "RowMajorMatrix Test:" << std::endl;

  std::for_each(colMajorMatrix.begin(), colMajorMatrix.end(), iter2);

  for (int i = 0; i < rows; ++i)
    {
      for (int j = 0; j < cols; ++j)
	{
	  std::cout << std::setw(12) << std::setprecision(2) << colMajorMatrix(i,j);
	}
      std::cout << std::endl;
    }

  bmt::LowerPackedSymmetricMatrix lowerPackedSymmetricMatrix(rows);
  bmt::SimpleIterator iter3;
  std::cout << "LowerPackedSymmetricMatrix Test:" << std::endl;

  std::for_each(lowerPackedSymmetricMatrix.begin(),
		lowerPackedSymmetricMatrix.end(),
		iter3);

  for (int i = 0; i < rows; ++i)
    {
      for (int j = 0; j < cols; ++j)
	{
	  std::cout << std::setw(12) << std::setprecision(2) << lowerPackedSymmetricMatrix(i,j);
	}
      std::cout << std::endl;
    }

  std::vector<double> eigenvalues(rows);
  bmt::ColumnMajorMatrix eigenvectors(rows, rows);

  int rc = bmt::eigensystem(lowerPackedSymmetricMatrix,
			    eigenvalues,
			    eigenvectors);

  std::cout << "Eigenvalues:" << std::endl;
  for (std::vector<double>::const_iterator iter = eigenvalues.begin();
       iter != eigenvalues.end();
       ++iter)
    {
      std::cout << std::setw(8) << std:: setprecision(2) << *iter << " ";
    }
  std::cout << "\nEigenvectors:" << std::endl;

  for (int i = 0; i < rows; ++i)
    {
      for (int j = 0; j < cols; ++j)
	{
	  std::cout << std::setw(12) << std::setprecision(2) << eigenvectors(i,j);
	}
      std::cout << std::endl;
    }

  rc = bmt::eigensystem(lowerPackedSymmetricMatrix,
			eigenvalues,
			eigenvectors,
			count);
  count = count < 0 ? -count : count;
  std::cout << "Extreme Eigenvalues:" << std::endl;
  for (int i = 0;
       i < count;
       ++i)
    {
      std::cout << std::setw(8) << std:: setprecision(2) << eigenvalues[i] << " ";
    }
  std::cout << "\nEigenvectors:" << std::endl;

  for (int i = 0; i < rows; ++i)
    {
      for (int j = 0; j < count; ++j)
	{
	  std::cout << std::setw(12) << std::setprecision(2) << eigenvectors(i,j);
	}
      std::cout << std::endl;
    }
  
}
