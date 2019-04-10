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
 // **********************************************************************************
// File: forceregression.cpp
// Author: RSG
// Date: September 28
// Description: Utility to compare the contents of two binary udf
//              regression sets (regression against reference data)
// **********************************************************************************

#include <BlueMatter/tolerance.hpp>
#include <BlueMatter/regressionutils.hpp>
#include <xstream/bz.h>

#include <exception>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>

int browseRegressionData(std::istream&);

int main(int argc, char** argv)
{
  const char* qFileName = NULL;
  const char* rFileName = NULL;
  int nBins = 20; // bin count to use for histogramming
  double printTolerance = -1; // will print pairs of records where
			      // abs(diff) is larger than
			      // printTolerance. Negative numbers
			      // cause nothing to be printed out

  switch(argc)
    {
    default:
      std::cerr << argv[0] << " queryFileName referenceFileName <nBins=20> <printTolerance=-1>" << std::endl;
      std::exit(-1);
    case 5:
      printTolerance = std::atof(argv[4]);
    case 4:
      nBins = std::atoi(argv[3]);
    case 3:
      qFileName = argv[2];
      rFileName = argv[1];
      break;
    }

  trc::Tolerance::setTolerance(printTolerance);

  std::ifstream refInFile(rFileName);
  if (!refInFile)
    {
      std::cerr << argv[0] << " Error opening reference file " << rFileName << " for input" << std::endl;
      std::exit(-1);
    }

  std::ifstream queryInFile(qFileName);
  if (!queryInFile)
    {
      std::cerr << argv[0] << " Error opening query file " << qFileName << " for input" << std::endl;
      std::exit(-1);
    }

  trc::RegressionRecordVectorTuple ref;

  xstream::bz::istreambuf bzRefBuf(refInFile.rdbuf());
  std::istream bzRefInFile(&bzRefBuf);

  ref.init(bzRefInFile);

  trc::RegressionRecordVectorTuple query;

  xstream::bz::istreambuf bzQueryBuf(queryInFile.rdbuf());
  std::istream bzQueryInFile(&bzQueryBuf);

  query.init(bzQueryInFile);

  trc::DiffData diffData;

  diffData.d_os_p = &std::cout;

  int nTerms = trc::RegressionRecordVectorTuple::diff(query, ref, diffData);
  
  int iTerm = 0;
  for (trc::DiffData::iterator iter = diffData.begin();
       iter != diffData.end();
       ++iter)
    {
      std::sort(iter->second.d_energyDelta.begin(), iter->second.d_energyDelta.end());
      std::sort(iter->second.d_forceDeltaX.begin(), iter->second.d_forceDeltaX.end());
      std::sort(iter->second.d_forceDeltaY.begin(), iter->second.d_forceDeltaY.end());
      std::sort(iter->second.d_forceDeltaZ.begin(), iter->second.d_forceDeltaZ.end());

      std::cout << "# min(energyDelta) = " << iter->second.d_energyDelta.front() << std::endl;
      std::cout << "# max(energyDelta) = " << iter->second.d_energyDelta.back() << std::endl;
      std::cout << "# min(forceDeltaX) = " << iter->second.d_forceDeltaX.front() << std::endl;
      std::cout << "# max(forceDeltaX) = " << iter->second.d_forceDeltaX.back() << std::endl;
      std::cout << "# min(forceDeltaY) = " << iter->second.d_forceDeltaY.front() << std::endl;
      std::cout << "# max(forceDeltaY) = " << iter->second.d_forceDeltaY.back() << std::endl;
      std::cout << "# min(forceDeltaZ) = " << iter->second.d_forceDeltaZ.front() << std::endl;
      std::cout << "# max(forceDeltaZ) = " << iter->second.d_forceDeltaZ.back() << std::endl;

      if ((iter->second.d_forceDeltaX.size() != iter->second.d_forceDeltaY.size()) ||
	  (iter->second.d_forceDeltaZ.size() != iter->second.d_forceDeltaX.size()))
	{
	  std::cerr << "Mismatch in forceDelta component sizes" << std::endl;
	}

      std::cout << "#\n# energyDelta.size() = " << iter->second.d_energyDelta.size() << std::endl;
      std::cout << "# forceDeltaX.size() = " << iter->second.d_forceDeltaX.size() << std::endl;
      std::cout << "# forceDeltaY.size() = " << iter->second.d_forceDeltaY.size() << std::endl;
      std::cout << "# forceDeltaZ.size() = " << iter->second.d_forceDeltaZ.size() << std::endl;
      std::cout << "#\n#\n#" << std::endl;


      std::cout << "# Index = " << iTerm << ", Energy Delta Distribution Function for key (ts, udfCode) = "
		<< iter->first << std::endl;
      std::cout << "# column 1: Number of counts less than or equal to value in column 3" << std::endl;
      std::cout << "# column 2: Fraction of counts less than or equal to values in column 3" << std::endl;
      std::cout << "# column 3: Value of energyDelta"
		<< std::endl;
  
      for (int i = 0; i <= nBins; ++i)
	{
	  int offset = (i*iter->second.d_energyDelta.size())/nBins;
	  offset = (offset == iter->second.d_energyDelta.size()) ? offset - 1 : offset;
	  double frac = static_cast<double>(offset)/static_cast<double>(iter->second.d_energyDelta.size());
	  std::cout << std::setw(8) << offset << " \t"
		    << std::setw(20) << frac << " \t"
		    << std::setw(20) << iter->second.d_energyDelta[offset]
		    << std::endl;
	}

      std::cout << "\n\n# Index = " << iTerm + 1 << ", Force Component Delta Distribution Function for key (ts, udfCode) = "
		<< iter->first << std::endl;
      std::cout << "# column 1: Number of counts less than or equal to values in columns 3, 4, and 5" << std::endl;
      std::cout << "# column 2: Fraction of counts less than or equal to values in columns 3, 4, and 5" << std::endl;
      std::cout << "# columns 3, 4, 5: " << std::endl;
      std::cout << "# Value of delta for forceDeltaX, forceDeltaY, and forceDeltaZ respectively" << std::endl;

      for (int i = 0; i <= nBins; ++i)
	{
	  int offset = (i*iter->second.d_forceDeltaX.size())/nBins;
	  offset = (offset == iter->second.d_forceDeltaX.size()) ? offset - 1 : offset;
	  double frac = static_cast<double>(offset)/static_cast<double>(iter->second.d_forceDeltaX.size());
	  std::cout << std::setw(8) << offset << " \t"
		    << std::setw(20) << frac << " \t"
		    << std::setw(20) << iter->second.d_forceDeltaX[offset] << " \t"
		    << std::setw(20) << iter->second.d_forceDeltaY[offset] << " \t"
		    << std::setw(20) << iter->second.d_forceDeltaY[offset] << std::endl;
	}
      iTerm += 2;
      std::cout << "\n\n";
    }

  return(0);
}


