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
// File: regressionrecord.cpp
// Author: RSG
// Date: September 28
// Description: Classes encapsulating data and operations associated
//              with a single regression record 
// **********************************************************************************



// ************************************************************************************
// Sample log line with force (and energy) regression data for a
// single interaction (double precision numbers are output as hex
// preceeded by :XD:).  Following >UDFREG, the first number is the
// simtick and the second number is the udf code
//
// 2.070085527    62 0 >UDFREG      0   34  S0   325  S1   326 :XD:4045FAA5D7172C15  F0 [ :XD:BFF16354E664D774 :XD:40090F1AE53E4930 :XD:3FD6BF74F56D6E18 ] F1 [ :XD:3FF16354E664D774 :XD:C0090F1AE53E4930 :XD:BFD6BF74F56D6E18 ]<    /include/BlueMatter/LocalTupleListDriver.hpp 941
// ************************************************************************************

#include <BlueMatter/regressionutils.hpp>

#include <strstream>
#include <string>
#include <cstring>
#include <cmath>
#include <iterator>

namespace trc
{

  union LL2Double
  {
    long long d_llval;
    double d_dval;
  };

  const std::string hexTag(":XD:");

  RegressionRecordInputIterator RegressionRecordInputIterator::s_begin;

  const RegressionRecordInputIterator& RegressionRecordInputIterator::begin(std::istream& is)
  {
    s_begin.d_is_p = &is;
    s_begin.d_recordCounter = 0;
    s_begin.advance();
    return(s_begin);
  }

  void RegressionRecordInputIterator::advance()
  {
    while(std::getline(*d_is_p, d_logLine))
      {
  ++d_recordCounter;
  d_cursor = 0;
  // search for UDFREG payload start
  if ((d_cursor = d_logLine.find(">UDFREG")) != std::string::npos)
    {
#if defined(DEBUG_ON)
      std::cerr << LOCATION << " : Found regression record at line " << d_recordCounter
          << " d_cursor = " << d_cursor << std::endl;
#endif
      return; // we found a regression record
    }
      }

    if (!d_is_p->eof())
      {
  std::cerr << "RegressionRecordInputIterator::advance():  Invalid stream reference while not at EOF!" << std::endl;
      }
    d_recordCounter = -1;
    return;
  }

  const RegressionRecordBase* RegressionRecordInputIterator::parseRegressionRecord()
  {
#if defined(DEBUG_ON)
    std::cerr << "Processing record: " << d_recordCounter << std::endl; 
    std::cerr << "On entry to parseRegressionRecord(), d_cursor = " << d_cursor << std::endl;
#endif
    std::string::size_type cursor = d_cursor;
    cursor += std::strlen(">UDFREG");
#if defined(DEBUG_ON)
    std::cerr << LOCATION << " d_logLine: " << d_logLine << std::endl;
    std::cerr << "d_logLine.substr( " << cursor << " ) = " << d_logLine.substr(cursor) << std::endl;
#endif
    std::istrstream logLine(d_logLine.c_str() + cursor, std::strlen(d_logLine.c_str() + cursor));
    int ts;
    int udfCode;
    logLine >> ts >> udfCode;

    SiteForcePair sfPair[MAX_SITES];
    char buff[4];
    int siteCount = 0;
    while(siteCount < MAX_SITES)
      {
  std::string::size_type lcursor = 0;
  std::ostrstream siteStr(buff, 4);
  siteStr << "S" << siteCount << std::ends;
  siteStr.freeze();
  if ((lcursor = d_logLine.find(siteStr.str(), cursor)) == std::string::npos)
    {
      break;
    }
  cursor = lcursor;
  cursor += std::strlen(siteStr.str());
  std::istrstream cLine(d_logLine.c_str() + cursor, std::strlen(d_logLine.c_str() + cursor));
  cLine >> sfPair[siteCount].first;
#if defined(DEBUG_ON)
  std::cerr << LOCATION << ": sfPair[" << siteCount << "].first = " << sfPair[siteCount].first << std::endl;
#endif
  ++siteCount;
      }

    // Now parse out energy and forces corresponding to each site
#if defined(DEBUG_ON)
    std::cerr << LOCATION << ": d_logLine.substr(" << cursor << ") " << d_logLine.substr(cursor) << std::endl;
#endif
    if ((cursor = d_logLine.find(hexTag, cursor)) == std::string::npos)
      {
  std::cerr << "RegressionRecordInputIterator::parseRegressionRecord()" << LOCATION <<  " Unable to find hex record for energy at line "
      << d_recordCounter << " of input stream" << std::endl;
  return(NULL);
      }

    cursor += hexTag.size();
    LL2Double conv;
    conv.d_llval = strtoull(d_logLine.c_str() + cursor, NULL, 16);
#if defined(DEBUG_ON)
    std::cerr << LOCATION << ": conv.d_llval = " << conv.d_llval << " = " 
        << std::setbase(16) <<  conv.d_llval  << std::setbase(10)
        << " , conv.d_dval = " << std::setprecision(17) << conv.d_dval << std::endl;
#endif
    if (isnan(conv.d_dval))
      {
  std::cerr << "NaN on conversion of energy " << conv.d_llval << " = "
      << std::setbase(16) << conv.d_llval << std::setbase(10) << std::endl;
  std::cerr << d_logLine << std::endl;
      }
    t_float energy = conv.d_dval;

    for (int i = 0; i < siteCount; ++i)
      {
  for (int j = 0; j < 3; ++j)
    {
      if ((cursor = d_logLine.find(hexTag, cursor)) == std::string::npos)
        {
    std::cerr << "RegressionRecordInputIterator::parseRegressionRecord()" << LOCATION << " Unable to find force F" << i 
        << " component " << j 
        << " at line " << d_recordCounter << " of input stream" << std::endl;
    return(NULL);
        }
      cursor += hexTag.size();
      conv.d_llval = strtoull(d_logLine.c_str() + cursor, NULL, 16);
      if (isnan(conv.d_dval))
        {
    std::cerr << "NaN on conversion of force[" << j << "] " << conv.d_llval << " = "
        << std::setbase(16) << conv.d_llval << std::setbase(10) << std::endl;
    std::cerr << d_logLine << std::endl;
        }
      sfPair[i].second.d_x[j] = conv.d_dval;
    }
      }

    // Then we need to sort SiteForcePair records by site id to put
    // them in canonical order for comparison

    std::sort(sfPair, sfPair + siteCount, CompareSiteForcePair());

    switch(siteCount)
      {
      default:
  std::cerr << "RegressionRecordInputIterator::parseRegressionRecord()" << LOCATION << " Illegal site count value of " << siteCount 
      << " at line " << d_recordCounter << " of input stream" << std::endl;
  break;
      case 1:
  d_oneSiteRec.init(ts, udfCode, energy, sfPair);
  return(static_cast<RegressionRecordBase*>(&d_oneSiteRec));
      case 2:
  d_twoSiteRec.init(ts, udfCode, energy, sfPair);
  return(static_cast<RegressionRecordBase*>(&d_twoSiteRec));
      case 3:
  d_threeSiteRec.init(ts, udfCode, energy, sfPair);
  return(static_cast<RegressionRecordBase*>(&d_threeSiteRec));
      case 4:
  d_fourSiteRec.init(ts, udfCode, energy, sfPair);
  return(static_cast<RegressionRecordBase*>(&d_fourSiteRec));
      case 5:
  d_fiveSiteRec.init(ts, udfCode, energy, sfPair);
  return(static_cast<RegressionRecordBase*>(&d_fiveSiteRec));
      }
    return(NULL);
  }


  RegressionRecordVectorTuple::RegressionRecordVectorTuple()
  {
  }

  int RegressionRecordVectorTuple::init(std::istream& is)
  {
    int32_t item = 0;
    // First thing read is a 32 bit integer giving sizeof(t_fixed)
    is.read(reinterpret_cast<char*>(&item), sizeof(int32_t));
    if (item != sizeof(t_fixed))
      {
  std::cerr << "RegressionRecordVectorTuple::init() : sizeof(t_fixed) = " << item 
      << " in stream != sizeof(t_fixed) = " << sizeof(t_fixed) << " in program " << std::endl;
  return(-1);
      }
    // Second thing read is a 32 bit integer giving sizeof(t_float)
    is.read(reinterpret_cast<char*>(&item), sizeof(int32_t));
    if (item != sizeof(t_float))
      {
  std::cerr << "RegressionRecordVectorTuple::init() : sizeof(t_float) = " << item 
      << " in stream != sizeof(t_float) = " << sizeof(t_float) << " in program " << std::endl;
  return(-1);
      }
    
    // Next thing read is a 64 bit integer stating what the maximum number of sites/udf might be
    int64_t maxSites = 0;
    is.read(reinterpret_cast<char*>(&maxSites), sizeof(int64_t));
    if (maxSites != trc::MAX_SITES)
      {
  std::cerr << LOCATION << " maxSites from file = " << maxSites
      << " != trc::MAX_SITES = " << trc::MAX_SITES << std::endl;
  std::exit(-1);
      }

    // make sure everything is empty
    d_oneSite.clear();
    d_twoSite.clear();
    d_threeSite.clear();
    d_fourSite.clear();
    d_fiveSite.clear();

    int64_t nRecs = 0;
    int64_t totRecs = 0;
    is.read(reinterpret_cast<char*>(&nRecs), sizeof(int64_t));
    if (nRecs > 0)
      {
  d_oneSite.reserve(nRecs);
      }
    totRecs += nRecs;
    is.read(reinterpret_cast<char*>(&nRecs), sizeof(int64_t));
    if (nRecs > 0)
      {
  d_twoSite.reserve(nRecs);
      }
    totRecs += nRecs;
    is.read(reinterpret_cast<char*>(&nRecs), sizeof(int64_t));
    if (nRecs > 0)
      {
  d_threeSite.reserve(nRecs);
      }
    totRecs += nRecs;
    is.read(reinterpret_cast<char*>(&nRecs), sizeof(int64_t));
    if (nRecs > 0)
      {
  d_fourSite.reserve(nRecs);
      }
    totRecs += nRecs;
    is.read(reinterpret_cast<char*>(&nRecs), sizeof(int64_t));
    if (nRecs > 0)
      {
  d_fiveSite.reserve(nRecs);
      }
    totRecs += nRecs;

    for (int64_t i = 0; i < totRecs; ++i)
      {
  const RegressionRecordBase& rec = RegressionRecordBase::read(is);
  switch(rec.nSites())
    {
    default:
      std::cerr << "RegressionRecordVectorTuple::init() : Unsupported number of site record vectors" << std::endl;
      return(-1);
    case 5:
      d_fiveSite.push_back(reinterpret_cast<const RegressionRecord<5>&>(rec));
      break;
    case 4:
      d_fourSite.push_back(reinterpret_cast<const RegressionRecord<4>&>(rec));
      break;
    case 3:
      d_threeSite.push_back(reinterpret_cast<const RegressionRecord<3>&>(rec));
      break;
    case 2:
      d_twoSite.push_back(reinterpret_cast<const RegressionRecord<2>&>(rec));
      break;
    case 1:
      d_oneSite.push_back(reinterpret_cast<const RegressionRecord<1>&>(rec));
      break;
    }
      }
    // Question:  Should I sort each of the vectors here (they should be sorted already)?
    std::sort(d_oneSite.begin(), d_oneSite.end());
    std::sort(d_twoSite.begin(), d_twoSite.end());
    std::sort(d_threeSite.begin(), d_threeSite.end());
    std::sort(d_fourSite.begin(), d_fourSite.end());
    std::sort(d_fiveSite.begin(), d_fiveSite.end());
    return(nRecs);
  }

  int RegressionRecordVectorTuple::write(std::ostream& os) const
  {
    int32_t item = sizeof(t_fixed);
    // First thing written is a 32 bit integer giving sizeof(t_fixed)
    os.write(reinterpret_cast<char*>(&item), sizeof(int32_t));
    // Second thing written is a 32 bit integer giving sizeof(t_float)
    item = sizeof(t_float);
    os.write(reinterpret_cast<char*>(&item), sizeof(int32_t));
    
    // Next thing written is a 64 bit integer stating the maximum number of sites/udf that might be present
    int64_t maxSites = trc::MAX_SITES;
    os.write(reinterpret_cast<char*>(&maxSites), sizeof(int64_t));

    // Next things written are a series of 64 bit integers stating how many regression records are stored with each number of Sites (1...N)
    int64_t nRecs = d_oneSite.size();
    os.write(reinterpret_cast<char*>(&nRecs), sizeof(int64_t));
    nRecs = d_twoSite.size();
    os.write(reinterpret_cast<char*>(&nRecs), sizeof(int64_t));
    nRecs = d_threeSite.size();
    os.write(reinterpret_cast<char*>(&nRecs), sizeof(int64_t));
    nRecs = d_fourSite.size();
    os.write(reinterpret_cast<char*>(&nRecs), sizeof(int64_t));
    nRecs = d_fiveSite.size();
    os.write(reinterpret_cast<char*>(&nRecs), sizeof(int64_t));


    std::for_each(d_fiveSite.begin(), d_fiveSite.end(), WriteOne<RegressionRecord<5> >(os));
    std::for_each(d_fourSite.begin(), d_fourSite.end(), WriteOne<RegressionRecord<4> >(os));
    std::for_each(d_threeSite.begin(), d_threeSite.end(), WriteOne<RegressionRecord<3> >(os));
    std::for_each(d_twoSite.begin(), d_twoSite.end(), WriteOne<RegressionRecord<2> >(os));
    std::for_each(d_oneSite.begin(), d_oneSite.end(), WriteOne<RegressionRecord<1> >(os));
    if (os)
      {
  return(nRecs);
      }
    return(-1);
  }

  int RegressionRecordVectorTuple::diff(const RegressionRecordVectorTuple& q, const RegressionRecordVectorTuple& r, DiffData& d)
  {
    diffRecordVector(q.oneSite(),   r.oneSite(), d);
    diffRecordVector(q.twoSite(),   r.twoSite(), d);
    diffRecordVector(q.threeSite(), r.threeSite(), d);
    diffRecordVector(q.fourSite(),  r.fourSite(), d);
    diffRecordVector(q.fiveSite(),  r.fiveSite(), d);
    return(d.size());
  }

  DiffDataRecord::DiffDataRecord()
  {
    d_tolerance_p = Tolerance::instance();
  }

}

std::ostream& operator<<(std::ostream& os, const trc::RegressionRecordVectorTuple& t)
{
  std::vector<trc::UdfRec> udfStats;

  trc::recordHistogram(t.oneSite(), udfStats);
  trc::recordHistogram(t.twoSite(), udfStats);
  trc::recordHistogram(t.threeSite(), udfStats);
  trc::recordHistogram(t.fourSite(), udfStats);
  trc::recordHistogram(t.fiveSite(), udfStats);
  
  std::sort(udfStats.begin(), udfStats.end());
  // print out statistical information by udf code
  std::cout << std::setw(6) << "ts" << " " 
      << std::setw(6) << "UDF ID" << " "
      << std::setw(6) << "Sites" << " "
      << std::setw(10) << "Count" << std::endl;
  for (std::vector<trc::UdfRec>::const_iterator iter = udfStats.begin();
       iter != udfStats.end();
       ++iter)
    {
      std::cout << std::setw(6) << iter->d_ts << " "
    << std::setw(6) << iter->d_udfCode << " "
    << std::setw(6) << iter->d_siteCount << " "
    << std::setw(10) << iter->d_count << std::endl;
    }
  std::cout << "\n\n";

  std::copy(t.oneSite().begin(), t.oneSite().end(), std::ostream_iterator<trc::RegressionRecord<1> >(os, "\n"));
  std::copy(t.twoSite().begin(), t.twoSite().end(), std::ostream_iterator<trc::RegressionRecord<2> >(os, "\n"));
  std::copy(t.threeSite().begin(), t.threeSite().end(), std::ostream_iterator<trc::RegressionRecord<3> >(os, "\n"));
  std::copy(t.fourSite().begin(), t.fourSite().end(), std::ostream_iterator<trc::RegressionRecord<4> >(os, "\n"));
  std::copy(t.fiveSite().begin(), t.fiveSite().end(), std::ostream_iterator<trc::RegressionRecord<5> >(os, "\n"));
  return(os);
}

