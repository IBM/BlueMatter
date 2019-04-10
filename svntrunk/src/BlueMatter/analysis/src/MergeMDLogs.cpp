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
 // ************************************************************************
// File: MergeMDLogs.cpp
// Author: RSG
// Date: January 21, 2008
// Description: Utility to take a list of mdlog files (the output of
//              MDLogger) and concatenate them together in time-step
//              order while eliminating all duplicate time-steps
//
// Command line arguments: mdlog1 <mdlog2> <mdlog3> ..... <mdlogN>
// ************************************************************************

#include <BlueMatter/MDLogUtils.hpp>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <queue>
#include <string>
#include <iterator>

struct MDLogSegment
{
  std::ifstream d_is;
  std::istream_iterator<mdlu::Row> d_iter;
  mdlu::Row d_header;
  mdlu::Row d_first; // first non-header row in stream
  // methods
  inline MDLogSegment( const char* );
  inline bool operator<( const MDLogSegment& ) const;
  inline bool operator>( const MDLogSegment& ) const;
  inline bool operator==( const MDLogSegment& ) const;
};

class CompareMDLogSegmentPtr
{
public:
  inline bool operator()( const MDLogSegment* , const MDLogSegment* ) const;
};

int main( int argc, char** argv )
{
  std::priority_queue<MDLogSegment*, std::vector<MDLogSegment*>, CompareMDLogSegmentPtr> pq;
  int fileCount = argc - 1;
  std::vector<MDLogSegment*> seg_p( fileCount );

  for ( int i = 1; i < argc; ++i )
    {
      seg_p[i] = new MDLogSegment( argv[i] );
      if ( seg_p[i] == NULL )
        {
          std::cerr << "Unable to allocate storage for seg_p[" << i << "]"
                    << std::endl;
          std::exit( -1 );
        }
      pq.push( seg_p[i] );
      if ( i == 1 )
        {
          std::cout << seg_p[i]->d_header << std::endl;
        }
    }

  CompareMDLogSegmentPtr cmp;

  long old_ts = -1;
  while ( !pq.empty() )
    {
      MDLogSegment* cur = pq.top();
      std::cerr << " with header: "
                << pq.top()->d_header << " and first data line: "
                << pq.top()->d_first << std::endl;
      pq.pop();
      std::cerr << "Last line from old file was ts = " 
                << old_ts << std::endl;
      std::cerr << "Running through new file starting at ts = "
                << cur->d_iter->ts() << std::endl;

      while ( ( cur->d_iter != std::istream_iterator<mdlu::Row>() ) &&
              ( (!pq.empty() && cmp( pq.top(), cur )  ) || 
              ( pq.empty() ) ) ) 
        {
          if ( cur->d_iter->isHeader() )
            {
              std::cerr << "Unexpected header line: "
                        << *cur->d_iter  << std::endl;
              ++cur->d_iter;
              continue;
            }
          std::cout << *( cur->d_iter ) << std::endl;
          old_ts = cur->d_iter->ts();
          ++cur->d_iter;
        }

      if ( cur->d_iter == std::istream_iterator<mdlu::Row>() && !pq.empty() )
        {
          std::cerr << "Switch to next stream at ts > " << old_ts;
        }
      else
        {
          if ( !pq.empty() )
            {
              std::cerr << "Switch to next stream at ts = " 
                        << cur->d_iter->ts();
            }
        }
    }
  return( 0 );
}


bool CompareMDLogSegmentPtr::operator()( const MDLogSegment* m1_p,
                                         const MDLogSegment* m2_p ) const
{
  return( m1_p->d_iter->ts() > m2_p->d_iter->ts() );
}

MDLogSegment::MDLogSegment( const char* fileName ) : d_is( fileName ), d_iter( d_is )
{
  if ( !d_is.good() )
    {
      std::cerr << "Unable to open file " << fileName
                << " for input" << std::endl;
      std::cerr << *d_iter << std::endl;
      std::exit( -1 );
    }

  while( ( d_iter->isHeader() ) && 
         ( d_iter != std::istream_iterator<mdlu::Row>() ) )
    {
      d_header = *d_iter;
      std::cerr << "HEADER: " << d_header << std::endl;
      ++d_iter;
    }
  if ( d_iter == std::istream_iterator<mdlu::Row>() )
    {
      std::cerr << "No data rows in " << fileName << std::endl;
      std::exit( -1 );
    }
  d_first = *d_iter;
}

bool MDLogSegment::operator<( const MDLogSegment& other ) const
{
  return( d_first < other.d_first );
}

bool MDLogSegment::operator>( const MDLogSegment& other ) const
{
  return( d_first > other.d_first );
}

bool MDLogSegment::operator==( const MDLogSegment& other ) const
{
  return( d_first == other.d_first );
}
