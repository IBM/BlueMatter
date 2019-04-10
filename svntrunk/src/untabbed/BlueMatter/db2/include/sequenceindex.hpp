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
 // ***********************************************************************
// File: sequenceindex.hpp
// Author: RSG
// Date: May 5, 2002
// Namespace: db2
// Class: SequenceIndex
// Description: Class encapsulating data and methods associated with an
//              index defined on a sequence. It is assumed that the
//              keys for which the index is being defined are
//              integer-valued and run sequentially upward from zero.
//
//              There are additional requirements on the function
//              object TComp comp that is used for sorting.  This function
//              object must also support:
//              a method: int TComp::key(int) that returns
//              the integer key value used for sorting.
// ***********************************************************************

#ifndef INCLUDE_DB2_SEQUENCEINDEX_HPP
#define INCLUDE_DB2_SEQUENCEINDEX_HPP

#include <BlueMatter/db2/compareinteger.hpp>

#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>
#include <iomanip>


#include <assert.h>
#include <cstdlib>

namespace db2
{
  template <class TSeq, class TComp>
  class SequenceIndex
  {
  private:
    const TSeq& d_seq;
    TComp& d_comp;
    std::vector<int> d_sorted;
    std::vector<std::vector<int>::const_iterator> d_index;
    SequenceIndex(const TSeq&, TComp&);
    SequenceIndex(const SequenceIndex&);
    SequenceIndex& operator=(const SequenceIndex&);
  public:
    static SequenceIndex<TSeq, TComp>* create(const TSeq&, TComp&);
    std::vector<int>::const_iterator begin(int) const; // returns iterator
    // to beginning of the list offsets into the the original sequence
    // with the key value specified as an argument
    std::vector<int>::const_iterator end(int) const; // returns iterator
    // to end of the list offsets into the the original sequence
    // with the key value specified as an argument
    std::vector<int>::const_iterator end() const;  // returns d_sorted.end()
    static int main(int, char**); // test driver
  };

  template<class TSeq, class TComp>
  SequenceIndex<TSeq, TComp>::SequenceIndex(const TSeq& seq, TComp& comp)
    : d_seq(seq), d_comp(comp)
  {}

  template<class TSeq, class TComp>
  SequenceIndex<TSeq, TComp>::SequenceIndex(const SequenceIndex& proto)
    : d_seq(proto.d_seq), d_comp(proto.d_comp)
  {}

  template<class TSeq, class TComp>
  SequenceIndex<TSeq, TComp>& SequenceIndex<TSeq, TComp>::operator=(const SequenceIndex&)
  {
    return(*this);
  }

  template<class TSeq, class TComp>
  SequenceIndex<TSeq, TComp>* SequenceIndex<TSeq, TComp>::create(const TSeq& seq, TComp& comp)
  {
    SequenceIndex<TSeq, TComp>* index =
      new SequenceIndex<TSeq, TComp>(seq, comp);
    assert(index != NULL);

    index->d_sorted.resize(index->d_seq.size());
    int idx = 0;
    for (std::vector<int>::iterator iter = index->d_sorted.begin();
   iter != index->d_sorted.end();
   ++iter)
      {
  *iter = idx;
  ++idx;
      }

    std::sort(index->d_sorted.begin(), index->d_sorted.end(), index->d_comp);

    index->d_index.resize((index->d_comp.key(index->d_sorted.front()) >
       index->d_comp.key(index->d_sorted.back()) ?
       index->d_comp.key(index->d_sorted.front()) :
       index->d_comp.key(index->d_sorted.back()))+1,
        index->d_sorted.end());

    int currentKey = -1;
    for (std::vector<int>::const_iterator iter = index->d_sorted.begin();
   iter != index->d_sorted.end();
   ++iter)
      {
  if (index->d_comp.key(*iter) != currentKey)
    {
      for (int i = currentKey+1; i < index->d_comp.key(*iter); ++i)
        {
    std::cerr << i << " " << index->d_comp.key(*iter)
        << " " << *iter << std::endl;
    index->d_index[i] = iter;
        }
      index->d_index[index->d_comp.key(*iter)] = iter;
      currentKey = index->d_comp.key(*iter);
    }
      }
    return(index);
  }

  template<class TSeq, class TComp>
  std::vector<int>::const_iterator SequenceIndex<TSeq, TComp>::begin(int key) const
  {
    if ((key < 0) || (key >= d_index.size()))
      {
  return(d_sorted.end());
      }
    return(d_index[key]);
  }

  template<class TSeq, class TComp>
  std::vector<int>::const_iterator SequenceIndex<TSeq, TComp>::end(int key) const
  {
    if ((key < 0) || (key+1 >= d_index.size()))
      {
  return(d_sorted.end());
      }
    return(d_index[key+1]);
  }


  template<class TSeq, class TComp>
  std::vector<int>::const_iterator SequenceIndex<TSeq, TComp>::end() const
  {
    return(d_sorted.end());
  }

  template<class TSeq, class TComp>
  int SequenceIndex<TSeq, TComp>::main(int argc, char** argv)
  {
    if (argc < 3)
      {
  std::cerr << argv[0] << " SequenceLength DistinctValues" << std::endl;
  return(-1);
      }
    int seqLen = std::atoi(argv[1]);
    int nVal = std::atoi(argv[2]);

    std::vector<int> seq;
    seq.reserve(seqLen);
    for (int i = 0; i < seqLen; ++i)
      {
  seq.push_back(std::rand() % nVal);
      }

    CompareInteger comp(seq);
    SequenceIndex<std::vector<int>, CompareInteger>* arrayIndex =
      SequenceIndex<std::vector<int>, CompareInteger>::create(seq, comp);
    assert(arrayIndex != NULL);
    int offset = 0;
    for (std::vector<int>::const_iterator iter = seq.begin();
   iter != seq.end();
   ++iter)
      {
  std::cout << std::setw(5) << offset << " "
      << std::setw(5) << *iter << " "
      << std::setw(5) << arrayIndex->d_sorted[offset] << " "
      << std::setw(5) << seq[arrayIndex->d_sorted[offset]] << " "
      << std::setw(5)
      << arrayIndex->d_comp.key(arrayIndex->d_sorted[offset])
      << " ";
  std::cout << std::endl;
  std::cout.flush();
  ++offset;
      }
    for (int i = 0; i < arrayIndex->d_index.size(); ++i)
      for (std::vector<int>::const_iterator iter = arrayIndex->begin(i);
     iter != arrayIndex->end(i);
     ++iter)
  {
    std::cout << std::setw(5) << i << " "
        << std::setw(5) << *iter << std::endl;
  }
    return(0);
  }

}
#endif
