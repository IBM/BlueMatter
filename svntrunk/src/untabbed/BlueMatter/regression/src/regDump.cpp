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
// File: regDump.cpp
// Author: RSG
// Date: October 12
// Description: Utility to dump the contents of a .reg file created by
//              trc2regression. It takes input from stdin and writes
//              to stdout
// **********************************************************************************

#include <BlueMatter/regressionutils.hpp>
#include <xstream/bz.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <limits>
#include <exception>

int main(int argc, char** argv)
{
  int nSites = argc - 3;
  trc::RegressionRecordVectorTuple reg;

  xstream::bz::istreambuf bzInBuf(std::cin.rdbuf());
  std::istream bzIn(&bzInBuf);

  reg.init(bzIn);

  int udfCode = -1;
  int ts = 0;
  switch(argc)
    {
    case 1:
      std::cerr << argv[0] << " <ts udfCode siteID_1 site_ID2 ... siteID_N (where N <= 5)> With no args, entire file is dumped" << std::endl;
      std::cout << reg;
      break;
    case 3: // we are passed only the time-step and udfCode, so we dump all records associated with that udfCode, but do nothing for now
      std::cerr << "Not implemented yet." << std::endl;
      ts = std::atoi(argv[1]);
      udfCode = std::atoi(argv[2]);
      break;
    default:
      {
  trc::SiteForcePair sfPair[5];
  trc::RegressionRecord<1> oneSite;
  trc::RegressionRecord<2> twoSite;
  trc::RegressionRecord<3> threeSite;
  trc::RegressionRecord<4> fourSite;
  trc::RegressionRecord<5> fiveSite;
  
  ts = std::atoi(argv[1]);
  udfCode = std::atoi(argv[2]);
  std::cerr << "Searching for record with " << nSites << " sites, ts = "
      << ts << ", udfCode = " << udfCode << " , and site IDs: ";

  nSites = (nSites <= 5 ? nSites : 5);
  for (int i = 0; i < nSites; ++i)
    {
      sfPair[i].first = std::atoi(argv[i+3]);
      std::cerr << sfPair[i].first << " ";
    }
  std::cerr << std::endl;

  std::sort(sfPair, sfPair + nSites, trc::CompareSiteForcePair());

  switch(nSites)
    {
    default:
      std::cerr << "Number of site must be in the range of 1 to 5" << std::endl;
      break;
    case 1:
      {
        oneSite.init(ts, udfCode, std::numeric_limits<trc::t_float>::min(), sfPair);
        std::pair<trc::RegressionRecord<1>::vector::const_iterator, trc::RegressionRecord<1>::vector::const_iterator> oneSitePair =
    trc::findRecWithKey(reg.oneSite(), oneSite);
        if (oneSitePair.first != reg.oneSite().end())
    {
      for (trc::RegressionRecord<1>::vector::const_iterator iter = oneSitePair.first;
           iter != oneSitePair.second;
           ++iter)
      std::cout << *iter << std::endl;
      return(0);
    }
      }
      break;
    case 2:
      {
        twoSite.init(ts, udfCode, std::numeric_limits<trc::t_float>::min(), sfPair);
        std::pair<trc::RegressionRecord<2>::vector::const_iterator, trc::RegressionRecord<2>::vector::const_iterator> twoSitePair =
    trc::findRecWithKey(reg.twoSite(), twoSite);
        if (twoSitePair.first != reg.twoSite().end())
    {
      for (trc::RegressionRecord<2>::vector::const_iterator iter = twoSitePair.first;
           iter != twoSitePair.second;
           ++iter)
      std::cout << *iter << std::endl;
      return(0);
    }
      }
        break;
    case 3:
      {
        threeSite.init(ts, udfCode, std::numeric_limits<trc::t_float>::min(), sfPair);
        std::pair<trc::RegressionRecord<3>::vector::const_iterator, trc::RegressionRecord<3>::vector::const_iterator> threeSitePair =
    trc::findRecWithKey(reg.threeSite(), threeSite);
        if (threeSitePair.first != reg.threeSite().end())
    {
      for (trc::RegressionRecord<3>::vector::const_iterator iter = threeSitePair.first;
           iter != threeSitePair.second;
           ++iter)
      std::cout << *iter << std::endl;
      return(0);
    }
      }
      break;
    case 4:
      {
        fourSite.init(ts, udfCode, std::numeric_limits<trc::t_float>::min(), sfPair);
        std::pair<trc::RegressionRecord<4>::vector::const_iterator, trc::RegressionRecord<4>::vector::const_iterator> fourSitePair =
    trc::findRecWithKey(reg.fourSite(), fourSite);
        if (fourSitePair.first != reg.fourSite().end())
    {
      for (trc::RegressionRecord<4>::vector::const_iterator iter = fourSitePair.first;
           iter != fourSitePair.second;
           ++iter)
      std::cout << *iter << std::endl;
      return(0);
    }
      }
      break;
    case 5:
      {
        fiveSite.init(ts, udfCode, std::numeric_limits<trc::t_float>::min(), sfPair);
        std::pair<trc::RegressionRecord<5>::vector::const_iterator, trc::RegressionRecord<5>::vector::const_iterator> fiveSitePair =
    trc::findRecWithKey(reg.fiveSite(), fiveSite);
        if (fiveSitePair.first != reg.fiveSite().end())
    {
      for (trc::RegressionRecord<5>::vector::const_iterator iter = fiveSitePair.first;
           iter != fiveSitePair.second;
           ++iter)
      std::cout << *iter << std::endl;
      return(0);
    }
      }
      break;
    }
  std::cerr << "No record found with the " << nSites << " site(s) specified" << std::endl;
      }
      break;
    }
  
  return(0);
}
