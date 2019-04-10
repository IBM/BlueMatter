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
// File: bmtread.hpp
// Author: RSG
// Date: September 24, 2002
// Namespace: bmt
// Class: BmtRead
// Description: Class encapsulating the reading of a bmt format file
//              one frame at a time.  This is an instance of an input
//              iterator in the STL sense.
// ********************************************************************

#ifndef INCLUDE_BMT_BMTREAD_HPP
#define INCLUDE_BMT_BMTREAD_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>

#include <BlueMatter/bmtposition.hpp>

namespace bmt
{
  class BmtRead;

  class BmtReadConstIteratorOld
  {
    friend class BmtReadConstIterator;
  private:
    BmtRead* d_reader_p;
    // methods
    BmtReadConstIteratorOld(BmtRead&);
  public:
    std::vector<Position> operator*();
  };

  class BmtReadConstIterator :
    public std::iterator<std::input_iterator_tag,
			 std::vector<Position>,
                         std::ptrdiff_t,
			 const std::vector<Position>*,
			 const std::vector<Position>&>
  {
    friend class BmtRead;
  private:
    BmtRead* d_reader_p;
    int d_frameIndex;
    // methods
    BmtReadConstIterator(BmtRead&);
  public:
    BmtReadConstIterator(const BmtReadConstIterator&);
    BmtReadConstIterator& operator=(const BmtReadConstIterator&);
    inline bool operator==(const BmtReadConstIterator&) const;
    inline bool operator!=(const BmtReadConstIterator&) const;
    const std::vector<Position>& operator*()
      const;
    BmtReadConstIterator& operator++(); // prefix
    BmtReadConstIteratorOld operator++(int); // postfix
  };

  class BmtRead
  {
    friend class BmtReadConstIterator;
    friend class BmtReadConstIteratorOld;
  private:
    enum {BUFFSIZE=2048};
    // data
    std::ifstream d_is;
    bool d_verbose;
    int d_atomCount;
    std::vector<Position> d_frame1;
    std::vector<Position> d_frame2;
    std::vector<Position>* d_currentFrame_p;
    std::vector<Position>* d_oldFrame_p;
    BmtReadConstIterator d_end;
    BmtReadConstIterator d_working;
    // methods
    int nextFrame();
  public:
    typedef BmtReadConstIterator const_iterator;
    BmtRead(const char*, bool=false);
    BmtReadConstIterator& begin();
    const BmtReadConstIterator& end() const;
  };


  bool BmtReadConstIterator::operator==(const BmtReadConstIterator& other) const
  {
    return((d_reader_p == other.d_reader_p) && (d_frameIndex == other.d_frameIndex));
  }

  bool BmtReadConstIterator::operator!=(const BmtReadConstIterator& other) const
  {
    return((d_frameIndex != other.d_frameIndex) || (d_reader_p != other.d_reader_p));
  }
}

#endif
