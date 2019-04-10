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
// File: trc2regression.cpp
// Author: RSG
// Date: October 6, 2006
// Description: Utility to extract loglines with UDFREG data and
//              convert into a binary form for further analysis
//              (regression against reference data)
//              Takes input from stdin and output goes to stdout
// **********************************************************************************

#include <BlueMatter/regressionutils.hpp>
#include <xstream/bz.h>

#include <iostream>
#include <exception>

int extractRegressionData(std::istream& is);


int main(int argc, char** argv)
{
  int nRecs = extractRegressionData(std::cin);
  std::cerr << nRecs << " total records read" << std::endl;
  return(0);
}



int extractRegressionData(std::istream& is)
{
  trc::RegressionRecordVectorTuple qRegData;
  trc::RegressionRecordInputIterator qIter(trc::RegressionRecordInputIterator::begin(is));

  int nRec = 0;
  int nErrors = 0;
  for (; qIter != qIter.end(); ++qIter)
    {
      int rc = qRegData.insert(*qIter);
      if (rc > 0)
	{
	  ++nRec;
	}
      else
	{
	  ++nErrors;
	}
    }

  std::cerr << nRec << " records read successfully and " 
	    << nErrors << " read with errors" << std::endl;

  std::cerr << std::setw(12) << "Site Count" << " "
	    << std::setw(16) << "Record Count" << std::endl;
  std::cerr.flush();
  int siteCount = 1;
  std::cerr << std::setw(12) << siteCount << " " << qRegData.oneSite().size() << std::endl;
  ++siteCount;
  std::cerr << std::setw(12) << siteCount << " " << qRegData.twoSite().size() << std::endl;
  ++siteCount;
  std::cerr << std::setw(12) << siteCount << " " << qRegData.threeSite().size() << std::endl;
  ++siteCount;
  std::cerr << std::setw(12) << siteCount << " " << qRegData.fourSite().size() << std::endl;
  ++siteCount;
  std::cerr << std::setw(12) << siteCount << " " << qRegData.fiveSite().size() << std::endl;
  ++siteCount;
  std::cerr.flush();

  std::sort(qRegData.oneSite().begin(),   qRegData.oneSite().end());
  std::sort(qRegData.twoSite().begin(),   qRegData.twoSite().end());
  std::sort(qRegData.threeSite().begin(), qRegData.threeSite().end());
  std::sort(qRegData.fourSite().begin(),  qRegData.fourSite().end());
  std::sort(qRegData.fiveSite().begin(),  qRegData.fiveSite().end());

  try
    {
      xstream::bz::ostreambuf bzOutBuf(std::cout.rdbuf());
      std::ostream bzOut(&bzOutBuf);

      bzOut.exceptions(std::ios::badbit);

      qRegData.write(bzOut);
      bzOut.flush();
    }
  catch(std::exception& e)
    {
      std::cerr << "Error: " << e.what() << std::endl;
    }

  return(nRec);
}
