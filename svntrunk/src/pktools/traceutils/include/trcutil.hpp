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
 // *************************************************************************
// File: trcutil.hpp
// Author: RSG
// Date: February 26, 2005
// Description: Utilities useful for various things
// Namespace: trc
// *************************************************************************
#ifndef INCLUDE_TRCUTIL_HPP
#define INCLUDE_TRCUTIL_HPP

#include <pktools/trcmetadata.hpp>

#include <iterator>
#include <string>
#include <fstream>
#include <set>
#include <vector>
#include <iostream>

namespace trc
{
  class TracePointCollection;
  class TracePointCollectionIterator;

  template <class TIra> // random access iterator
  class vectorWrapper
  {
  private:
    TIra d_begin;
    TIra d_end;
  public:
    inline TIra begin() const
    {
      return(d_begin);
    }

    inline TIra end() const
    {
      return(d_end);
    }

    inline int size() const
    {
      return(d_end - d_begin);
    }
  };

  // takes begin, end for array of starting trace point timestamps
  // and array of finishing trace point timestamps and returns
  // vector of intervals defined by the difference between the
  // corresponding starting and ending tracepoints.
  template <class Tii, class Tv> // input iterator, vector
  int get_intervals(Tii s1, Tii s2, Tii f1, Tii f2, Tv& interval)
  {
    interval.clear();

    Tii iFinish = f1;
    for (Tii iStart = s1; iStart != s2; ++iStart)
      {
	interval.push_back(*iFinish - *iStart);
	++iFinish;
      }
    return(interval.size());
  }

  template <class Tii, class Tv> // input iterator, vector
  int get_intervals(Tii s1, Tii s2, Tv& interval)
  {
    interval.clear();
    for (Tii iFinish = s1 + 1; iFinish != s2; ++iFinish)
      {
	Tii iStart = iFinish - 1;
	interval.push_back(*iFinish - *iStart);
      }
    return(interval.size());
  }

// tracepoints cut by bracketKey are assumed to come before all
// other trace points in the current time step.
  template <class Tra, class Tv> // random access iterator, vector
  int get_grouped_intervals(Tra s1, Tra s2, // start time stamps
			    Tra f1, Tra f2, // finish time stamps
			    Tra b1, Tra b2, // bracket time stamps
			    Tv& interval) // interval vector
  {
    interval.clear();

    for (Tra iBracket = b1; iBracket != b2; ++iBracket)
      {
	Tv::value_type accumulatedTime = 0;
	for (Tra iStart = std::lower_bound(s1, s2, *iBracket);
           ((iBracket + 1) != b2) ?
	       (*iStart < *(iBracket+1)) && (iStart != s2) 
	       : iStart != s2;
           ++iStart)
	  {
	    Tra iFinish = f1 + (iStart - f1);
	    accumulatedTime += (*iFinish - *iStart);
	  }
	interval.push_back(accumulatedTime);
      }
    return(interval.size());
  }



  template< class ThingType >
  int
  Hex2Thing( const char *source, ThingType & aThing )
  {
    enum{ llcnt = ( ( sizeof(ThingType) + sizeof(long long) - 1 ) / sizeof( long long ) )  };

    union
    {
      unsigned long long mLLThing[ llcnt ];
      ThingType mThing;
    } TheThing;

    int                  llcounter = 0;
    int                  llworkcount = 0;
    unsigned long long   llwork = 0 ;

    const char               * ch = source;
    unsigned             v;


    for( int cnt = 0; cnt < 2 * sizeof( ThingType ) ; cnt ++ )
      {
        switch( *ch )
          {
          case '0' :  v = 0x00; break;
          case '1' :  v = 0x01; break;
          case '2' :  v = 0x02; break;
          case '3' :  v = 0x03; break;
          case '4' :  v = 0x04; break;
          case '5' :  v = 0x05; break;
          case '6' :  v = 0x06; break;
          case '7' :  v = 0x07; break;
          case '8' :  v = 0x08; break;
          case '9' :  v = 0x09; break;
          case 'A' :  v = 0x0A; break;
          case 'a' :  v = 0x0a; break;
          case 'B' :  v = 0x0B; break;
          case 'b' :  v = 0x0b; break;
          case 'C' :  v = 0x0C; break;
          case 'c' :  v = 0x0c; break;
          case 'D' :  v = 0x0D; break;
          case 'd' :  v = 0x0d; break;
          case 'E' :  v = 0x0E; break;
          case 'e' :  v = 0x0e; break;
          case 'F' :  v = 0x0F; break;
          case 'f' :  v = 0x0f; break;
          default  :
            return( -1 );
          };

        llwork      = ( llwork << 4 ) + v;
        llworkcount ++;
        
        if( llworkcount == 2 * sizeof( long long) )
          {
            TheThing.mLLThing[ llcounter ] = llwork;
            llcounter ++ ;
            llwork      = 0;
            llworkcount = 0;
          }

        ch++;
     
      }

    while( llworkcount < 2 * sizeof( long long ) )
      {
        llwork = llwork << 4;
        llworkcount++;
      }

    TheThing.mLLThing[ llcounter ] = llwork;

    memcpy( &aThing, &(TheThing.mThing), sizeof( ThingType ) );


    return( 0 );

  }
  
  
  struct TraceMapElt
  {
    // key components
    int d_traceId;
    int d_node;
    // data
    int d_traceType; // start=0 or finish=1 for now
    int d_globalId; // remapping of d_label, d_traceType onto ints good
                    // for all nodes
  };

  struct GlobalMapElt
  {
    // key components
    std::string d_label;
    int d_traceType;
    //data
    int d_globalId;
    std::string d_desc;
  };


  struct CompareElt
  {
    inline bool operator()(const TraceMapElt& e1, const TraceMapElt& e2) const;
  };

  struct CompareGlobalElt
  {
    inline bool operator()(const GlobalMapElt& e1, const GlobalMapElt& e2) const;
  };

  bool CompareElt::operator()(const TraceMapElt& e1, const TraceMapElt& e2) const
  {
    if (e1.d_traceId != e2.d_traceId)
      {
        return (e1.d_traceId < e2.d_traceId);
      }
    return (e1.d_node < e2.d_node);
  }

  bool CompareGlobalElt::operator()(const GlobalMapElt& e1, const GlobalMapElt& e2) const
  {
    if (e1.d_label != e2.d_label)
      {
        return(e1.d_label < e2.d_label);
      }
    return(e1.d_traceType < e2.d_traceType);
  }

  // *********************TYPEDEFS******************************
  typedef std::set<GlobalMapElt, CompareGlobalElt> GlobalIdMap;

  class TracePoint
  {
    friend class TracePointCollectionIterator;
  public:
    typedef long long pclock_type;
  private:
    std::string d_traceId;
    int d_traceType;
    int d_node;
    int d_globalId;
    pclock_type d_clock;
  public:
    inline TracePoint& operator=(const TracePoint&);
    inline const std::string& traceId() const;
    inline int traceType() const;
    inline int node() const;
    inline int globalId() const;
    inline const pclock_type& clock() const;
  };
  

  class TracePointCollectionIterator : 
    public std::iterator<std::input_iterator_tag, TracePoint>
  {
    friend class TracePointCollection;
  public:
    // typedefs
    typedef std::set<TraceMapElt, CompareElt> TraceMap;

    // enums
    enum {EMPTY_BUFFER = -1, GOT_DATA = 0, GOT_UNKNOWN = 1};

  private:
    // enums
    enum {MAX_LINE = 16*1024};
    enum {END_LINE_NO = -1, ERROR_ON_READ = -2, ERROR_UNPACKING = -3};

    // constant strings

    // NOTE: code casts this thing to a long long to check for log
    // line matches - must be 8 chars
    static char FXLOG_TAG_STR[10];

    static char FXLOG_DATA_STR[6];
    static char FXLOG_ID_STR[4];

    
    // member data
    TraceMap d_traceMap;

    GlobalIdMap d_globalIdMap;
    std::vector<GlobalMapElt> d_byGlobalId;
    
    unsigned long d_runMHz;

    std::ifstream* d_is_p;
    std::ostream* d_os_p;
    std::string d_logLine;
    int d_lineNo;
    int d_inputLines;
    long long d_byteCount;
    long long d_fileSize;
    unsigned long long d_earliestTraceEvent;
    TracePoint d_current;
    int d_cursor;
    int d_node;
    int d_maxTraceId;
    int d_traceOffset;
    bool d_renumberTraceId;
    TraceMetaData* d_metaData_p;
#if defined(RSG_DEBUG)
    std::ofstream* d_trace;
    std::ofstream* d_nontrace;
#endif

    // methods
    void init( int,        // clock frequency in Mhz
               int = -1,   // trace offset to use
               std::ostream& = std::cout ); // renumbered trace output
    int advance();
    int unpack();
    inline void setModeRenumber( const int,       // first trace id to use
                                 std::ostream& ); // output stream
  public:
    TracePointCollectionIterator();
    TracePointCollectionIterator&
    operator=(const TracePointCollectionIterator&);
    inline bool operator==(const TracePointCollectionIterator&) const;
    inline bool operator!=(const TracePointCollectionIterator&) const;
    inline const TracePoint& operator*() const;
    inline const TracePoint* operator->() const;
    inline TracePointCollectionIterator& operator++();
    inline const GlobalIdMap& globalIdMap() const;
    inline const std::vector<GlobalMapElt>& byGlobalId() const;
    inline const TraceMap& traceMap() const;
    int maxTraceId() const; // returns maximum trace id encountered
    inline bool renumberTraceId() const;
    inline int traceOffset() const;
  };

  class TracePointCollection
  {
  private:

    TracePointCollectionIterator d_begin;
    TracePointCollectionIterator d_end;
    std::ifstream d_is;

    int d_runMHz;
  public:
    typedef TracePointCollectionIterator const_iterator;

    TracePointCollection();
    ~TracePointCollection();
    void init( const char*,     // arg: fileName, NULL signifies use std::cin
               int,             //      frequency in MHz
               int = -1,        // trace offset, >= 0 causes renumbering
               std::ostream& = std::cout ); // renumbered output stream

    const_iterator& begin();
    const_iterator& end();
    inline const TraceMetaData& metaData() const;
  };

  TracePoint& TracePoint::operator=(const TracePoint& other)
  {
    if (this != &other)
      {
        d_traceId = other.d_traceId;
        d_traceType = other.d_traceType;
        d_node = other.d_node;
        d_globalId = other.d_globalId;
        d_clock = other.d_clock;
      }
    return(*this);
  }

  const std::string& TracePoint::traceId() const
  {
    return(d_traceId);
  }
  
  int TracePoint::traceType() const
  {
    return(d_traceType);
  }
  
  int TracePoint::node() const
  {
    return(d_node);
  }
  
  const TracePoint::pclock_type& TracePoint::clock() const
  {
    return(d_clock);
  }
  
  int TracePoint::globalId() const
  {
    return(d_globalId);
  }

  bool TracePointCollectionIterator::operator==(const TracePointCollectionIterator& other) const
  {
    return((d_is_p == other.d_is_p) && (d_lineNo == other.d_lineNo));
  }

  bool TracePointCollectionIterator::operator!=(const TracePointCollectionIterator& other) const
  {
    return(!operator==(other));
  }
    
  const TracePoint& TracePointCollectionIterator::operator*() const
  {
    return(d_current);
  }

  const TracePoint* TracePointCollectionIterator::operator->() const
  {
    return(&d_current);
  }

  TracePointCollectionIterator&
  TracePointCollectionIterator::operator++()
  {
#if defined(PRINT_STUFF)
    std::cout << "About to call advance from operator++()" << std::endl;
#endif
    d_lineNo = advance();
#if defined(PRINT_STUFF)
    std::cout << "Returned from advance with return value = " << d_lineNo
              << std::endl;
#endif
    if (d_lineNo < END_LINE_NO)
      {
        std::cerr << "TracePointCollectionIterator error: " << d_lineNo << std::endl;
        d_lineNo = END_LINE_NO;
      }
    return(*this);
  }

  const GlobalIdMap& TracePointCollectionIterator::globalIdMap() const
  {
    return(d_globalIdMap);
  }

  const std::vector<GlobalMapElt>& TracePointCollectionIterator::byGlobalId() const
  {
    return(d_byGlobalId);
  }

  const TracePointCollectionIterator::TraceMap& TracePointCollectionIterator::traceMap() const
  {
    return d_traceMap;
  }

  void TracePointCollectionIterator::setModeRenumber( const int traceOffset,
                                                      std::ostream& os )
  {
    d_traceOffset = traceOffset;
    d_renumberTraceId = true;
    d_os_p = &os;
  }
  bool TracePointCollectionIterator::renumberTraceId() const
  {
    return( d_renumberTraceId );
  }

  int TracePointCollectionIterator::traceOffset() const
  {
    return( d_traceOffset );
  }

  const TraceMetaData& TracePointCollection::metaData() const
  {
    return *d_begin.d_metaData_p;
  }
}
#endif
