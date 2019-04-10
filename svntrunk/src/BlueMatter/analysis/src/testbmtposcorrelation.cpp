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
// File: testbmtposcorrelation.hpp
// Author: RSG
// Date: October 6, 2002
// Description: computes correlation matrix for actual site positions from
//              bmt file data for essential dynamics analysis
// ********************************************************************

#include <BlueMatter/bmtread.hpp>
#include <BlueMatter/bmtpos2float.hpp>
#include <BlueMatter/bmtcorrelation.hpp>
#include <BlueMatter/bmtcovariance.hpp>
#include <BlueMatter/bmtunary_compose.hpp>
#include <BlueMatter/bmtcolumnmajormatrix.hpp>
#include <BlueMatter/bmtrowmajormatrix.hpp>
#include <BlueMatter/bmtlowerpackedsymmetricmatrix.hpp>
#include <BlueMatter/bmteigensystem.hpp>

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <algorithm>
#include <vector>

int main(int argc, char** argv)
{
  const char* bmtFile = NULL;
  bool verbose = false;
  std::vector<int> site;
  switch(argc)
    {
    default:
      bmtFile = argv[1];
      for (int i = 2; i < argc; ++i)
	{
	  std::cerr << "argv[" << i << "] = " << argv[i] << std::endl;
	  site.push_back(std::atoi(argv[i]));
	}
      break;
    case 1:
    case 0:
      std::cerr << argv[0] << " bmtFile site_id1 site_id2 ... site_idN" << std::endl;
      return(1);
    }
  for (std::vector<int>::const_iterator iter = site.begin();
       iter != site.end();
       ++iter)
    {
      std::cerr << *iter << std::endl;
    }
  bmt::BmtRead bmtReader(bmtFile, verbose);
  bmt::Pos2Float pos2float(site);
  bmt::Correlation corr;

  std::cerr << "About to call unary_compose" << std::endl;
  bmt::unary_compose<bmt::Correlation, bmt::Pos2Float> combo(corr, pos2float);
  std::cerr << "About to call for_each" << std::endl;
  bmt::BmtRead::const_iterator start = bmtReader.begin();
  bmt::BmtRead::const_iterator finish = bmtReader.end();
  std::for_each(start, finish, combo);
  std::cerr << "About to call finalize()" << std::endl;
  combo.second().finalize();
  std::cout << "computed correlation matrix from " << combo.second().count()
	    << " instances" << std::endl;
  std::cout << combo.second() << std::endl;

  // compute largest eigenvalues and eigenvectors for correlation matrix
  bmt::ColumnMajorMatrix eigenvector(combo.second().size(),
				     combo.second().size());
  std::vector<double> eigenvalue(combo.second().size());
  int eCount = 10;
  int rc = bmt::eigensystem(combo.second()(),
			    eigenvalue,
			    eigenvector,
			    eCount);
  // coordinate array for correlation matrix eigenvectors

  for (int k = 0; k < eCount; ++k)
    {
      std::cout << "eigenvalue[" << k << "] = " << eigenvalue[k] <<
	std::endl;
      std::cout << "eigenvector[" << k << "]:" << std::endl;
      for (int i = 0; i < site.size(); ++i)
	{
	  int ix=3*i;
	  std::cout << "[" << i << "] " << eigenvector(ix, k) << " "
		    << eigenvector(ix+1, k) << " "
		    << eigenvector(ix+2, k) << std::endl;
	}
      std::cout << std::endl;
    }

  return(0);
}

