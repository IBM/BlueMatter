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
// File: regressionutils.hpp
// Author: RSG
// Date: September 28
// Description: Classes encapsulating data and operations associated
//              with regression records
// **********************************************************************************
#ifndef INCLUDE_TRC_REGRESSIONUTILS_HPP
#define INCLUDE_TRC_REGRESSIONUTILS_HPP

#include <BlueMatter/tolerance.hpp>
#include <BlueMatter/diffscalar.hpp>

#include <utility>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <stdint.h>
#include <cmath>

#if defined(__FUNCTION__)
#define LOCATION trc::Location( __FILE__ ,  __LINE__ , __FUNCTION__)
#else
#define LOCATION trc::Location( __FILE__ ,  __LINE__ )
#endif

namespace trc
{
  enum {MAX_SITES = 5};

  class Location
  {
  private:
    std::string d_file;
    long int d_line;
#if defined(__FUNCTION__)
    std::string d_function;
#endif
  public:
#if defined(__FUNCTION__)
    inline Location(const char* file,
		    const long int line,
		    const char* function) :
      d_file(file), d_line(line), d_function(function)
    {}
#else
    inline Location(const char* file,
		    const long int line) :
      d_file(file), d_line(line)
    {}
#endif
    inline const std::string& file() const
    {
      return(d_file);
    }
    inline const long int line() const
    {
      return(d_line);
    }
#if defined(__FUNCTION__)
    inline const std::string& function() const
    {
      return(d_function);
    }
#endif
  };

  std::ostream& operator<<(std::ostream& os, const trc::Location&);
  // adopt typedefs from fp:
  typedef fp::t_float t_float;
  typedef fp::t_fixed t_fixed;

  struct UdfKey
  {
    int d_udfCode;
    int d_ts;
    // methods
    UdfKey(int ts = 0, int udfCode = 0)
      : d_ts(ts), d_udfCode(udfCode)
    {}

    UdfKey(const UdfKey& proto)
      : d_ts(proto.d_ts),
	d_udfCode(proto.d_udfCode)
    {}

    UdfKey& operator=(const UdfKey& other)
    {
      d_ts = other.d_ts;
      d_udfCode = other.d_udfCode;
      return(*this);
    }

    bool operator<(const UdfKey& other) const
    {
      if (d_ts != other.d_ts)
	{
	  return(d_ts < other.d_ts);
	}
      return(d_udfCode < other.d_udfCode);
    }

    bool operator!=(const UdfKey& other) const
    {
      return((d_udfCode != other.d_udfCode) || (d_ts != other.d_ts));
    }
  };

  struct DiffDataRecord
  {
    std::vector<t_float> d_energyDelta;
    std::vector<t_float> d_forceDeltaX;
    std::vector<t_float> d_forceDeltaY;
    std::vector<t_float> d_forceDeltaZ;
    Tolerance* d_tolerance_p;
    // methods
    DiffDataRecord();
    inline bool forceInTolerance() const
    {
      return(d_tolerance_p->inTolerance(d_forceDeltaX.back()) &&
	     d_tolerance_p->inTolerance(d_forceDeltaY.back()) &&
	     d_tolerance_p->inTolerance(d_forceDeltaZ.back()));
    }

    inline bool energyInTolerance() const
    {
      return(d_tolerance_p->inTolerance(d_energyDelta.back()));
    }
  };

  class DiffData : public std::map<UdfKey, DiffDataRecord>
  {
  public:
    std::ostream* d_os_p;
  };

  struct ThreeVector
  {
    t_float d_x[3];
    ThreeVector& operator=(const ThreeVector& other)
    {
      d_x[0] = other.d_x[0];
      d_x[1] = other.d_x[1];
      d_x[2] = other.d_x[2];
      return(*this);
    }
  };

  typedef std::pair<t_fixed, ThreeVector> SiteForcePair;

  template <class T, int NELTS>
  class ArrayAssignment
  {
  public:
    static void assign(T* dest, const T* src)
    {
      *dest = *src;
      ArrayAssignment<T, NELTS-1>::assign(dest+1, src+1);
    }
  };

  template <class T>
  class ArrayAssignment<T,1>
  {
  public:
    static void assign(T* dest, const T* src)
    {
      *dest = *src;
    }
  };

  class CompareSiteForcePair
  {
  public:
    bool operator()(const SiteForcePair& p1, const SiteForcePair& p2) const
    {
      return(p1.first < p2.first);
    }
  };

  class RegressionRecordBase
  {
  private:
    int d_nSites;
    t_float d_energy;
    UdfKey d_key;
  protected:
    RegressionRecordBase(const int nSites, const int ts = 0, const int udfCode = 0, const double energy = 0)
      : d_nSites(nSites), d_key(ts, udfCode), d_energy(energy)
    {}
  public:
    RegressionRecordBase(const RegressionRecordBase& proto)
      : d_nSites(proto.d_nSites),
	d_key(proto.d_key),
	d_energy(proto.d_energy)
    {}
    inline int nSites() const;
    inline int getTs() const;
    inline void setTs(const int);
    inline t_float& energy();
    inline const t_float& energy() const;
    inline void setUdfCode(const int);
    inline int udfCode() const;
    inline const UdfKey& key() const;
    static const RegressionRecordBase& read(std::istream&);
    inline bool operator<(const RegressionRecordBase&) const;
    inline bool operator!=(const RegressionRecordBase&) const;
    inline RegressionRecordBase& operator=(const RegressionRecordBase&);
    std::ostream& write(std::ostream&) const;
  };

  RegressionRecordBase& RegressionRecordBase::operator=(const RegressionRecordBase& other)
  {
    if (d_nSites != other.d_nSites)
      {
	std::cerr << "RegressionRecordBase::operator=(): mismatched number of sites, this->nSites() = "
		  << nSites() << ", other.nSites() = " << other.nSites() << std::endl;
	return(*this);
      }
    d_key = other.d_key;
    d_energy = other.d_energy;
    return(*this);
  }

  template <int NSITES>
  class RegressionRecord : public RegressionRecordBase
  {
    friend class RegressionRecordBase;
  private:
    SiteForcePair d_sfPair[NSITES];
    // methods
    std::istream& readRest(std::istream&);
  public:
    enum {N_SITES = NSITES};
    typedef std::vector<trc::RegressionRecord<NSITES> > vector;

    RegressionRecord() : RegressionRecordBase(NSITES)
    {}
    RegressionRecord(const RegressionRecord<NSITES>& proto)
      : RegressionRecordBase(static_cast<const RegressionRecordBase&>(proto))
    {
      ArrayAssignment<SiteForcePair, NSITES>::assign(&d_sfPair[0], &proto.d_sfPair[0]);
    }
    // time-step, udfCode, energy, site-force pair array
    void init(const int, const int, const t_float&, const SiteForcePair*);
    inline SiteForcePair* sfPair();
    inline const SiteForcePair* sfPair() const;
    inline bool operator<(const RegressionRecord<NSITES>&) const;
    inline RegressionRecord<NSITES>& operator=(const RegressionRecord<NSITES>&);
    inline RegressionRecord<NSITES>& operator=(const RegressionRecordBase&);
    std::ostream& writeRest(std::ostream&) const;
    std::ostream& output(std::ostream&) const;
    static inline bool diff(const RegressionRecord<NSITES>&, 
			    const RegressionRecord<NSITES>&,
			    DiffData&);
  };

  std::ostream& operator<<(std::ostream& os, const trc::RegressionRecordBase&);
  
  std::ostream& operator<<(std::ostream& os, const trc::ThreeVector&);
  
  std::ostream& operator<<(std::ostream& os, const trc::RegressionRecord<1>&);
  std::ostream& operator<<(std::ostream& os, const trc::RegressionRecord<2>&);
  std::ostream& operator<<(std::ostream& os, const trc::RegressionRecord<3>&);
  std::ostream& operator<<(std::ostream& os, const trc::RegressionRecord<4>&);
  std::ostream& operator<<(std::ostream& os, const trc::RegressionRecord<5>&);

  template <typename TRec, int NELT = TRec::N_SITES>
  class CompareRegressionRecordKey
  {
    public:
    static bool equals(const TRec& r1,
		       const TRec& r2)
    {
      return((r1.sfPair()[NELT-1].first == r2.sfPair()[NELT-1].first)
	     && CompareRegressionRecordKey<TRec, NELT-1>::equals(r1, r2));
    }
  };

  template <typename TRec>
  class CompareRegressionRecordKey<TRec,1>
  {
  public:
    static bool equals(const TRec& r1,
		       const TRec& r2)
    {
      return((r1.sfPair()[0].first == r2.sfPair()[0].first) && (r1.udfCode() == r2.udfCode()) && (r1.getTs() == r2.getTs()));
    }
  };

  template <typename TRecVec>
  void diffRecordVector(const TRecVec& qVec, const TRecVec& rVec, DiffData& d);

  template <typename TRec>
  class CompareRecords
  {
  public:
    inline bool operator()(const RegressionRecord<TRec::N_SITES>& r1, const RegressionRecord<TRec::N_SITES>& r2)
    {
      if (r1.getTs() != r2.getTs())
	{
	  return(r1.getTs() < r2.getTs());
	}
      if (r1.udfCode() != r2.udfCode())
	{
	  return(r1.udfCode() < r2.udfCode());
	}
      
      for (int i = 0; i < TRec::N_SITES; ++i)
	{
	  if (r1.sfPair()[i].first != r2.sfPair()[i].first)
	    {
	      return(r1.sfPair()[i].first < r2.sfPair()[i].first);
	    }
	}
      return(false);
    }
  };

  template <typename TRecVec>
  std::pair<typename TRecVec::const_iterator, typename TRecVec::const_iterator> findRecWithKey(const TRecVec& vec, typename TRecVec::const_reference qRec)
  {
    std::pair<typename TRecVec::const_iterator, typename TRecVec::const_iterator> ret;
    if (std::binary_search(vec.begin(), vec.end(), qRec, CompareRecords<TRecVec::value_type>()))
      {
	std::cerr << "findRecWithKey: At least one record is present" << std::endl;
      }
    else
      {
	std::cerr << "findRecWithKey: No record present" << std::endl;
	ret.first = vec.end();
	ret.second = vec.end();
	return(ret);
      }
    ret.first = std::lower_bound(vec.begin(), vec.end(), qRec, CompareRecords<TRecVec::value_type>());
    ret.second = std::upper_bound(vec.begin(), vec.end(), qRec, CompareRecords<TRecVec::value_type>());
    //    ret.first = ((ret.first == vec.end()) || (*ret.first < qRec)) ? vec.end() : ret.first;
    return(ret);
  }

  template <int NSITES>
  RegressionRecord<NSITES>& RegressionRecord<NSITES>::operator=(const RegressionRecord<NSITES>& other)
  {
    this->RegressionRecordBase::operator=(static_cast<const RegressionRecordBase&>(other));
    ArrayAssignment<SiteForcePair,NSITES>::assign(&d_sfPair[0], &other.d_sfPair[0]);
    return(*this);
  }

  template <int NSITES>
  RegressionRecord<NSITES>& RegressionRecord<NSITES>::operator=(const RegressionRecordBase& other)
  {
    this->RegressionRecordBase::operator=(other);
    return(*this);
  }

  template <int NSITES>
  std::ostream& RegressionRecord<NSITES>::output(std::ostream& os) const
  {
    os << std::setw(6) << this->getTs() << " ";
    os << std::setw(6) << this->udfCode() << " ";
    for (int i = 0; i < NSITES; ++i)
    {
      os << std::setw(6) << this->sfPair()[i].first << " \t";
    }
    os << std::scientific << std::setprecision(17)
       << std::setw(25) << this->energy() << " \t";

    for (int i = 0; i < NSITES; ++i)
    {
      os << std::setw(25) << this->sfPair()[i].second << " \t";
    }
    return(os);
  }


  class RegressionRecordInputIterator : public std::iterator<std::input_iterator_tag, RegressionRecordBase>
  {
  private:
    enum {MAX_SITES = 5};
    RegressionRecord<1> d_oneSiteRec;
    RegressionRecord<2> d_twoSiteRec;
    RegressionRecord<3> d_threeSiteRec;
    RegressionRecord<4> d_fourSiteRec;
    RegressionRecord<5> d_fiveSiteRec;
    std::istream* d_is_p;
    int d_recordCounter;
    std::string d_logLine;
    std::string::size_type d_cursor;

    RegressionRecordInputIterator* d_tmpIter_p; // used for postfix operator++

    static RegressionRecordInputIterator s_begin;
    RegressionRecordInputIterator* d_end_p;
    // methods
    void advance();
    const RegressionRecordBase* parseRegressionRecord();
    RegressionRecordInputIterator(std::istream* is_p = NULL, const int recordCounter = 0) :
      d_is_p(is_p),
      d_recordCounter(recordCounter),
      d_end_p(NULL),
      d_tmpIter_p(NULL),
      d_cursor(0)
    {}
  public:
    RegressionRecordInputIterator(const RegressionRecordInputIterator& proto) :
      d_is_p(proto.d_is_p),
      d_recordCounter(proto.d_recordCounter),
      d_logLine(proto.d_logLine),
      d_cursor(proto.d_cursor),
      d_end_p(NULL),
      d_tmpIter_p(NULL)
    {}

    // the only way to initialize a RegressionRecordInputIterator is to
    // call the copy constructor (or assignment operator) with an
    // argument consisting of the static member begin(std::istream&)
    static const RegressionRecordInputIterator& begin(std::istream&);

    inline const RegressionRecordInputIterator& end();

    inline bool operator==(const RegressionRecordInputIterator&) const;

    inline bool operator!=(const RegressionRecordInputIterator&) const;

    inline RegressionRecordInputIterator& operator++(); // prefix

    inline RegressionRecordInputIterator& operator++(int); // postfix

    inline const RegressionRecordBase& operator*();

    inline const RegressionRecordBase* operator->();
  };


  template <typename TRec>
  class HistogramPopulation
  {
  public:
    typedef std::map<UdfKey, int> map;
  private:
    std::map<UdfKey, int> d_histByUdf;
  public:
    inline const std::map<UdfKey, int>& histByUdf() const
    {
      return(d_histByUdf);
    }
    inline void operator()(const TRec& rec)
    {
      std::pair<UdfKey, int> elt(UdfKey(rec.getTs(),rec.udfCode()), 1);
      std::pair<std::map<UdfKey, int>::iterator, bool> ret = d_histByUdf.insert(elt);
      if (!ret.second)
	{
	  ++(ret.first->second);
	}
    }
  };


  struct UdfRec
  {
    int d_ts;
    int d_udfCode;
    int d_siteCount;
    int d_count;
    inline bool operator<(const UdfRec& other) const
    {
      if (d_ts != other.d_ts)
	{
	  return(d_ts < other.d_ts);
	}
      if (d_udfCode != other.d_udfCode)
	{
	  return(d_udfCode < other.d_udfCode);
	}
      return(d_siteCount < other.d_siteCount);
    }
  };

  template <typename TRecVec>
  void recordHistogram(const TRecVec& vec, std::vector<UdfRec>& udfStats)
  {
    trc::HistogramPopulation<typename TRecVec::value_type> hist;
    hist = std::for_each(vec.begin(), vec.end(), hist);
    UdfRec tmp;
    for (trc::HistogramPopulation<typename TRecVec::value_type>::map::const_iterator iter = hist.histByUdf().begin();
	 iter != hist.histByUdf().end();
	 ++iter)
      {
	tmp.d_ts = iter->first.d_ts;
	tmp.d_udfCode = iter->first.d_udfCode;
	tmp.d_count = iter->second;
	tmp.d_siteCount = TRecVec::value_type::N_SITES;
	udfStats.push_back(tmp);
      }
  }


  template <typename TRecVec>
  void diffRecordVector(const TRecVec& qVec, const TRecVec& rVec, DiffData& d)
  {
    // This routine assumes that the vectors are sorted
    // Make sure of that

    TRecVec::const_iterator qIter = qVec.begin();
    TRecVec::const_iterator rIter = rVec.begin();

    while((qIter != qVec.end()) && (rIter != rVec.end()))
      {
	if (!TRecVec::value_type::diff(*qIter, *rIter, d))
	  // we have a mismatch in the interactions (udfCodes and/or
	  // lists of sites don't match)
	  {
	    *d.d_os_p << "Mismatched interaction pair: " 
		      << *qIter << "  " 
		      << *rIter << std::endl;
	    if (*qIter < *rIter)
	      {
		++qIter;
	      }
	    else
	      {
		++rIter;
	      }
	    continue;
	  }
	++qIter;
	++rIter;
      }

    while (qIter != qVec.end())
      {
	*d.d_os_p << "Unmatched query interaction: " << *qIter << std::endl;
	++qIter;
      }

    while (rIter != rVec.end())
      {
	*d.d_os_p << "Unmatched reference interaction: " << *rIter << std::endl;
	++rIter;
      }
  }
  
  class RegressionRecordVectorTuple
  {
  private:
    std::vector<trc::RegressionRecord<1> > d_oneSite;
    std::vector<trc::RegressionRecord<2> > d_twoSite;
    std::vector<trc::RegressionRecord<3> > d_threeSite;
    std::vector<trc::RegressionRecord<4> > d_fourSite;
    std::vector<trc::RegressionRecord<5> > d_fiveSite;

    // methods
    template <typename TArray>
    inline int add(const RegressionRecordBase&, TArray&);
  public:
    RegressionRecordVectorTuple();
    int init(std::istream&);
    int write(std::ostream&) const;

    inline int insert(const RegressionRecordBase&); // returns size of
						    // array if successful

    inline std::vector<trc::RegressionRecord<1> >& oneSite();
    inline const std::vector<trc::RegressionRecord<1> >& oneSite() const;
    inline std::vector<trc::RegressionRecord<2> >& twoSite();
    inline const std::vector<trc::RegressionRecord<2> >& twoSite() const;
    inline std::vector<trc::RegressionRecord<3> >& threeSite();
    inline const std::vector<trc::RegressionRecord<3> >& threeSite() const;
    inline std::vector<trc::RegressionRecord<4> >& fourSite();
    inline const std::vector<trc::RegressionRecord<4> >& fourSite() const;
    inline std::vector<trc::RegressionRecord<5> >& fiveSite();
    inline const std::vector<trc::RegressionRecord<5> >& fiveSite() const;
    static int diff(const RegressionRecordVectorTuple&, const RegressionRecordVectorTuple&, DiffData&);
  };

  inline bool diffThreeVector(const ThreeVector& q, const ThreeVector& r, DiffDataRecord& d)
  {
    d.d_forceDeltaX.push_back(fp::diffScalar(q.d_x[0], r.d_x[0]));
    d.d_forceDeltaY.push_back(fp::diffScalar(q.d_x[1], r.d_x[1]));
    d.d_forceDeltaZ.push_back(fp::diffScalar(q.d_x[2], r.d_x[2]));
    return(d.forceInTolerance());
  }

  template <typename TRec, int NELT>
  class DiffRecord
  {
  public:
    static bool diff(const TRec& q, const TRec& r, DiffDataRecord& d)
    {
      return(diffThreeVector(q.sfPair()[NELT-1].second, r.sfPair()[NELT-1].second, d) &&
	     DiffRecord<TRec,NELT-1>::diff(q, r, d));
    }
  };

  template <typename TRec>
  class DiffRecord<TRec,1>
  {
  public:
    static bool diff(const TRec& q, const TRec& r, DiffDataRecord& d)
    {
      bool foo = diffThreeVector(q.sfPair()[0].second, r.sfPair()[0].second, d);
#if defined(DEBUG_ON)
      std::cerr << std::setprecision(17) << q.energy() << " \t" << r.energy() << " \t"
		<< fp::diffScalar(q.energy(),r.energy()) << std::endl;
#endif
      d.d_energyDelta.push_back(fp::diffScalar(q.energy(), r.energy()));
      foo = foo && d.energyInTolerance();
      return(foo);
    }
  };

  template <int NSITES>
  bool RegressionRecord<NSITES>::diff(const RegressionRecord<NSITES>& q, 
				     const RegressionRecord<NSITES>& r,
				     DiffData& d)
  {
    if (!CompareRegressionRecordKey<RegressionRecord<NSITES>, RegressionRecord<NSITES>::N_SITES>::equals(q, r))
      {
	return(false);
      }
    DiffDataRecord& ddr = d[q.key()];
    bool foo = DiffRecord<RegressionRecord<NSITES>, RegressionRecord<NSITES>::N_SITES>::diff(q, r, ddr);
    // Do tolerance check here based on return from DiffRecord<>::diff
    if (!foo) // print out records that contain diffs out of tolerance
      {
	std::cerr << "diff_out_of_tolerance_for_q: " << q << "\n"
		  << "       when_compared_with_r: " << r << std::endl;
	std::cerr << "                      diffs: "
		  << std::setw(6) << q.getTs() << " "
		  << std::setw(6) << q.udfCode() << " ";
	for (int i = 0; i < NSITES; ++i)
	  {
	    std::cerr << std::setw(6) << q.sfPair()[i].first << " \t";
	  }

	std::cerr << std::setprecision(5) << std::setw(25)
		  << ddr.d_energyDelta.back() << " \t";
	for (int i = NSITES; i > 0 ; --i)
	  {
	    std::cerr << std::setw(25) 
		      << ddr.d_forceDeltaX[ddr.d_forceDeltaX.size() - i]
		      << " \t"
		      << std::setw(25) 
		      << ddr.d_forceDeltaY[ddr.d_forceDeltaY.size() - i]
		      << " \t"
		      << std::setw(25) 
		      << ddr.d_forceDeltaZ[ddr.d_forceDeltaZ.size() - i]
		      << " \t";
	  }
	std::cerr << std::endl;
      }
    return(true);
  }

  std::vector<trc::RegressionRecord<1> >& RegressionRecordVectorTuple::oneSite()
  {
    return(d_oneSite);
  }

  const std::vector<trc::RegressionRecord<1> >& RegressionRecordVectorTuple::oneSite() const
  {
    return(d_oneSite);
  }

  std::vector<trc::RegressionRecord<2> >& RegressionRecordVectorTuple::twoSite()
  {
    return(d_twoSite);
  }

  const std::vector<trc::RegressionRecord<2> >& RegressionRecordVectorTuple::twoSite() const
  {
    return(d_twoSite);
  }

  std::vector<trc::RegressionRecord<3> >& RegressionRecordVectorTuple::threeSite()
  {
    return(d_threeSite);
  }

  const std::vector<trc::RegressionRecord<3> >& RegressionRecordVectorTuple::threeSite() const
  {
    return(d_threeSite);
  }

  std::vector<trc::RegressionRecord<4> >& RegressionRecordVectorTuple::fourSite()
  {
    return(d_fourSite);
  }

  const std::vector<trc::RegressionRecord<4> >& RegressionRecordVectorTuple::fourSite() const
  {
    return(d_fourSite);
  }

  std::vector<trc::RegressionRecord<5> >& RegressionRecordVectorTuple::fiveSite()
  {
    return(d_fiveSite);
  }

  const std::vector<trc::RegressionRecord<5> >& RegressionRecordVectorTuple::fiveSite() const
  {
    return(d_fiveSite);
  }


  const RegressionRecordInputIterator& RegressionRecordInputIterator::end()
  {
    if (d_end_p == NULL)
      {
	d_end_p = new RegressionRecordInputIterator;
	if (d_end_p == NULL)
	  {
	    std::cerr << "RegressionRecordInputIterator::end() Error allocating RegressionRecordInputIterator" << std::endl;
	    std::exit(-1);
	  }
	d_end_p->d_is_p = d_is_p;
	d_end_p->d_recordCounter = -1;
      }
    return(*d_end_p);
  }

  bool RegressionRecordInputIterator::operator==(const RegressionRecordInputIterator& other) const
  {
    return((d_is_p == other.d_is_p) && (d_recordCounter == other.d_recordCounter));
  }
  
  bool RegressionRecordInputIterator::operator!=(const RegressionRecordInputIterator& other) const
  {
    return((d_recordCounter != other.d_recordCounter) || (d_is_p != other.d_is_p));
  }
  
  RegressionRecordInputIterator& RegressionRecordInputIterator::operator++() // prefix
  {
    advance();
    return(*this);
  }

  RegressionRecordInputIterator& RegressionRecordInputIterator::operator++(int) // postfix
  {
    if (d_tmpIter_p == NULL)
      {
	d_tmpIter_p = new RegressionRecordInputIterator;
	if (d_tmpIter_p == NULL)
	  {
	    std::cerr << "RegressionRecordInputIterator::operator++(int): Unable to allocate RegressionRecordInputIterator for temporary" << std::endl;
	    std::exit(-1);
	  }
      }
    d_tmpIter_p = this;
    advance();
    return(*d_tmpIter_p);
  }

  const RegressionRecordBase& RegressionRecordInputIterator::operator*()
  {
    return(*parseRegressionRecord());
  }

  const RegressionRecordBase* RegressionRecordInputIterator::operator->()
  {
    return(parseRegressionRecord());
  }

  template <typename TWriteable>
  class WriteOne
  {
  private:
    std::ostream& d_os;
  public:
    WriteOne(std::ostream& os = std::cout) : d_os(os)
    {}

    void operator()(const TWriteable& t)
    {
      t.writeRest(d_os);
    }
  };

  template <typename TArray>
  int RegressionRecordVectorTuple::add(const RegressionRecordBase& rec, TArray& nSite)
  {
    RegressionRecord<TArray::value_type::N_SITES> tmp = reinterpret_cast<const RegressionRecord<TArray::value_type::N_SITES>& >(rec);
#if defined DEBUG_ON
    std::cerr << LOCATION << " : rec<" << TArray::value_type::N_SITES
	      << "> = " << tmp << std::endl;
#endif
	  nSite.push_back(tmp);
	  return(nSite.size());
  }

  int RegressionRecordVectorTuple::insert(const RegressionRecordBase& rec)
  {
#if defined(DEBUG_ON)
    std::cerr << LOCATION << " : nSites = " << rec.nSites() << std::endl;
#endif
    switch(rec.nSites())
      {
      default:
	return(-1);
      case 1:
	{
	  return(add(rec, d_oneSite));
	}
      case 2:
	{
	  return(add(rec, d_twoSite));
	}
      case 3:
	{
	  return(add(rec, d_threeSite));
	}
      case 4:
	{
	  return(add(rec, d_fourSite));
	}
      case 5:
	{
	  return(add(rec, d_fiveSite));
	}
      } // end of switch(rec.nSites())
    return(-2); // should never get here
  }

  int RegressionRecordBase::nSites() const
  {
    return(d_nSites);
  }

  int RegressionRecordBase::getTs() const
  {
    return(d_key.d_ts);
  }

  void RegressionRecordBase::setTs(const int ts)
  {
    d_key.d_ts = ts;
  }

  template <int NSITES>
  void RegressionRecord<NSITES>::init(const int ts, const int udfCode, const t_float& aEnergy, const SiteForcePair* sfPair)
  {
    this->setUdfCode(udfCode);
    this->setTs(ts);
    this->energy() = aEnergy;
    for (int i = 0; i < N_SITES; ++i)
      {
	this->d_sfPair[i] = sfPair[i];
      } 
  }

  bool RegressionRecordBase::operator<(const RegressionRecordBase& other) const
  {
    return (d_key < other.d_key);
  }

  bool RegressionRecordBase::operator!=(const RegressionRecordBase& other) const
  {
    return((d_key != other.d_key) || (d_nSites != other.d_nSites));
  }

  template <int NSITES>
  bool RegressionRecord<NSITES>::operator<(const RegressionRecord<NSITES>& other) const
  {
    if (RegressionRecordBase::operator!=(static_cast<const RegressionRecordBase&>(other)))
      {
	return(RegressionRecordBase::operator<(static_cast<const RegressionRecordBase&>(other)));
      }
    for (int i = 0; i < NSITES; ++i)
      {
	if (this->d_sfPair[i].first != other.d_sfPair[i].first)
	  {
	    return(this->d_sfPair[i].first < other.d_sfPair[i].first);
	  }
      }
    // may want to add comparison of force terms as well after this
    return(this->energy() < other.energy());
  }

  template <int NSITES>
  SiteForcePair* RegressionRecord<NSITES>::sfPair()
  {
    return(this->d_sfPair);
  }
  
  template <int NSITES>
  const SiteForcePair* RegressionRecord<NSITES>::sfPair() const
  {
    return(this->d_sfPair);
  }
  
  t_float& RegressionRecordBase::energy()
  {
    return(this->d_energy);
  }

  const t_float& RegressionRecordBase::energy() const
  {
    return(this->d_energy);
  }

  void RegressionRecordBase::setUdfCode(const int code)
  {
    this->d_key.d_udfCode = code;
  }
  
  t_fixed RegressionRecordBase::udfCode() const
  {
    return(this->d_key.d_udfCode);
  }

  const trc::UdfKey& RegressionRecordBase::key() const
  {
    return(d_key);
  }

  std::ostream& RegressionRecordBase::write(std::ostream& os) const
  {
    unsigned char ns = static_cast<unsigned char>(this->nSites());
    os.write(reinterpret_cast<const char*>(&ns), sizeof(unsigned char));
    os.write(reinterpret_cast<const char*>(&this->d_key.d_ts), sizeof(t_fixed));
    os.write(reinterpret_cast<const char*>(&this->d_key.d_udfCode), sizeof(t_fixed));
    os.write(reinterpret_cast<const char*>(&this->d_energy), sizeof(t_float));
    return(os);
  }

  template <int NSITES>
  std::ostream& RegressionRecord<NSITES>::writeRest(std::ostream& os) const
  {
    this->RegressionRecordBase::write(os);

    for (int i = 0; i < N_SITES; ++i)
      {
	os.write(reinterpret_cast<const char*>(&(this->d_sfPair[i].first)), sizeof(t_fixed));
	os.write(reinterpret_cast<const char*>(&(this->d_sfPair[i].second.d_x[0])), sizeof(t_float));
	os.write(reinterpret_cast<const char*>(&(this->d_sfPair[i].second.d_x[1])), sizeof(t_float));
	os.write(reinterpret_cast<const char*>(&(this->d_sfPair[i].second.d_x[2])), sizeof(t_float));
      }
    return(os);
  }

  const RegressionRecordBase& RegressionRecordBase::read(std::istream& is)
  {
    static RegressionRecord<1> oneSite;
    static RegressionRecord<2> twoSite;
    static RegressionRecord<3> threeSite;
    static RegressionRecord<4> fourSite;
    static RegressionRecord<5> fiveSite;
    static RegressionRecordBase invalidSite(0);

    unsigned char nSites = 0;
    is.read(reinterpret_cast<char*>(&nSites), sizeof(unsigned char));
    RegressionRecordBase rec(nSites);
    is.read(reinterpret_cast<char*>(&rec.d_key.d_ts), sizeof(t_fixed));
    is.read(reinterpret_cast<char*>(&rec.d_key.d_udfCode), sizeof(t_fixed));
    is.read(reinterpret_cast<char*>(&rec.d_energy), sizeof(t_float));
    if (isnan(rec.energy()))
      {
	std::cerr << "energy is NaN on read from " << rec.nSites() << " record with udfCode = "
		  << rec.udfCode() << std::endl;
      }
#if defined(DEBUG_ON)
    std::cerr << LOCATION << ": rec = " << rec << std::endl;
#endif
    switch(nSites)
      {
      case 1:
	oneSite = rec;
#if defined(DEBUG_ON)
	std::cerr << LOCATION << ": oneSite = " << oneSite << std::endl;
#endif
	oneSite.readRest(is);
#if defined(DEBUG_ON)
	std::cerr << LOCATION << ": oneSite = " << oneSite << std::endl;
#endif
	return(oneSite);
      case 2:
	twoSite = rec;
#if defined(DEBUG_ON)
	std::cerr << LOCATION << ": twoSite = " << twoSite << std::endl;
#endif
	twoSite.readRest(is);
#if defined(DEBUG_ON)
	std::cerr << LOCATION << ": twoSite = " << twoSite << std::endl;
#endif
	return(twoSite);
      case 3:
	threeSite = rec;
#if defined(DEBUG_ON)
	std::cerr << LOCATION << ": threeSite = " << threeSite << std::endl;
#endif
	threeSite.readRest(is);
#if defined(DEBUG_ON)
	std::cerr << LOCATION << ": threeSite = " << threeSite << std::endl;
#endif
	return(threeSite);
      case 4:
	fourSite = rec;
#if defined(DEBUG_ON)
	std::cerr << LOCATION << ": fourSite = " << fourSite << std::endl;
#endif
	fourSite.readRest(is);
#if defined(DEBUG_ON)
	std::cerr << LOCATION << ": fourSite = " << fourSite << std::endl;
#endif
	return(fourSite);
      case 5:
	fiveSite = rec;
#if defined(DEBUG_ON)
	std::cerr << LOCATION << ": fiveSite = " << fiveSite << std::endl;
#endif
	fiveSite.readRest(is);
#if defined(DEBUG_ON)
	std::cerr << LOCATION << ": fiveSite = " << fiveSite << std::endl;
#endif
	return(fiveSite);
      default:
	std::cerr << "Illegal value of nSites = " << nSites << std::endl;
	break;
      }
    return(invalidSite);
  }

  template <int NSITES>
  std::istream& RegressionRecord<NSITES>::readRest(std::istream& is)
  {
    for (int i = 0; i < N_SITES; ++i)
      {
	is.read(reinterpret_cast<char*>(&(this->d_sfPair[i].first)), sizeof(t_fixed));
	is.read(reinterpret_cast<char*>(&(this->d_sfPair[i].second.d_x[0])), sizeof(t_float));
	if (isnan(this->d_sfPair[i].second.d_x[0]))
	  {
	    std::cerr << "force[0] is NaN on read for site " << d_sfPair[i].first
		      << " from " << NSITES << " record with udfCode = "
		      << udfCode() << std::endl;
	  }
	is.read(reinterpret_cast<char*>(&(this->d_sfPair[i].second.d_x[1])), sizeof(t_float));
	if (isnan(this->d_sfPair[i].second.d_x[1]))
	  {
	    std::cerr << "force[1] is NaN on read for site " << this->d_sfPair[i].first
		      << " from " << NSITES << " record with udfCode = "
		      << this->udfCode() << std::endl;
	  }
	is.read(reinterpret_cast<char*>(&(this->d_sfPair[i].second.d_x[2])), sizeof(t_float));
	if (isnan(this->d_sfPair[i].second.d_x[2]))
	  {
	    std::cerr << "force[2] is NaN on read for site " << this->d_sfPair[i].first
		      << " from " << NSITES << " record with udfCode = "
		      << this->udfCode() << std::endl;
	  }
      }
    return(is);
  }

  std::ostream& operator<<(std::ostream& os, const trc::RegressionRecordBase& rec)
  {
    os << std::setw(6) << rec.getTs() << " ";
    os << std::setw(6) << rec.nSites() << " ";
    os << std::setw(6) << rec.udfCode() << " ";
    os << std::scientific << std::setprecision(17)
       << std::setw(25) << rec.energy() << " \t";
    return(os);
  }
  
  std::ostream& operator<<(std::ostream& os, const trc::ThreeVector& v)
  {
    for (int i = 0; i < 3; ++i)
      {
	os << std::setprecision(17) << std::setw(25) << v.d_x[i] << " \t";
      }
    return(os);
  }
  
  std::ostream& operator<<(std::ostream& os, const trc::Location& loc)
  {
    os << "in " << loc.file() << " at line " << loc.line();
#if defined(__FUNCTION__)
    os << " in function " << loc.function();
#endif
    return(os);
  }
  
  std::ostream& operator<<(std::ostream& os, const trc::RegressionRecord<1>& rec)
  {
    return(rec.output(os));
  }
  
  std::ostream& operator<<(std::ostream& os, const trc::RegressionRecord<2>& rec)
  {
    return(rec.output(os));
  }
  
  std::ostream& operator<<(std::ostream& os, const trc::RegressionRecord<3>& rec)
  {
    return(rec.output(os));
  }
  
  std::ostream& operator<<(std::ostream& os, const trc::RegressionRecord<4>& rec)
  {
    return(rec.output(os));
  }
  
  std::ostream& operator<<(std::ostream& os, const trc::RegressionRecord<5>& rec)
  {
    return(rec.output(os));
  }

  std::ostream& operator<<(std::ostream& os, const trc::UdfKey& key)
  {
    os << std::setw(6) << key.d_ts << " "
       << std::setw(6) << key.d_udfCode;
    return(os);
  }

}

std::ostream& operator<<(std::ostream& os, const trc::RegressionRecordVectorTuple&);

#endif
