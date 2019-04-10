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
// File: bmtread.cpp
// Author: RSG
// Date: September 24, 2002
// Class: BmtRead
// Description: Class encapsulating the reading of a bmt format file
//              one frame at a time.
// ********************************************************************

#include <BlueMatter/bmtread.hpp>

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <cstring>

#define BEGINTRAJ "TRAJECTORYFORMAT"

namespace bmt
{
  BmtRead::BmtRead(const char* fName, bool verbose) : d_verbose(verbose), 
    d_end(*this), d_working(*this)
  {
    d_is.open(fName);
    if (!d_is)
      {
  std::cerr << "Unable to open file " << fName << " for input" << std::endl;
  exit(1);
      }
    assert(d_is);
  }

  BmtReadConstIterator& BmtRead::begin()
  {
    // rewind to the beginning
    d_is.seekg(0);
    d_working.d_frameIndex=-1;
    char buff[BUFFSIZE];
    d_is.get(buff, BUFFSIZE);
    assert(d_is);
    if (d_is.gcount() < BUFFSIZE-1)
      {
  d_is.ignore();
      }
    assert(!std::strncmp(buff, "BMT", 3));
    
    d_is.get(buff, BUFFSIZE);
    assert(d_is);
    if (d_is.gcount() < BUFFSIZE-1)
      {
  d_is.ignore();
      }
    
    char PDBFileName[256];
    int  NProteinAtoms = 0;
    int  NWaters = 0;
    int  NoWaters = 0;
    while (std::strncmp(buff, BEGINTRAJ, strlen(BEGINTRAJ)))
      {
  if (d_verbose)
    {
      std::cerr << buff << std::endl;
    }
  if      (1 == sscanf(buff, "STRUCTURE %s\n", PDBFileName))
    ;
  else if (1 == sscanf(buff, "NUMNONWATERATOMS %d\n", &NProteinAtoms))
    ;
  else if (1 == sscanf(buff, "NUMWATERS %d\n", &NWaters))
    ;
  else if (1 == sscanf(buff, "NOWATERS %d\n", &NoWaters))
    ;
  
  d_is.get(buff, BUFFSIZE);
  assert(d_is);
  if (d_is.gcount() < BUFFSIZE-1)
    {
      d_is.ignore();
    }
      }
    
    if (NoWaters)
      NWaters = 0;
    
    d_atomCount = NProteinAtoms + 3 * NWaters;
    d_frame1.resize(d_atomCount);
    d_frame2.resize(d_atomCount);
    d_currentFrame_p = &d_frame1;
    d_oldFrame_p = &d_frame2;
    if (d_verbose)
      {
  std::cerr << "atom count = " << d_atomCount << std::endl;
      }

    d_working.d_frameIndex = nextFrame();

    return(d_working);
  }

  const BmtReadConstIterator& BmtRead::end() const
  {
    return(d_end);
  }

  int BmtRead::nextFrame()
  {
    if (!d_is || d_is.eof())
      {
  d_working.d_frameIndex = -1;
  return(d_working.d_frameIndex);
      }
    std::vector<Position>* newFrame_p = d_oldFrame_p;
    d_oldFrame_p = d_currentFrame_p;
    d_currentFrame_p = newFrame_p;
    
    float x0, y0, z0, s;
    
    d_is.read(reinterpret_cast<char*>(&x0), sizeof(float));
    d_is.read(reinterpret_cast<char*>(&y0), sizeof(float));
    d_is.read(reinterpret_cast<char*>(&z0), sizeof(float));
    d_is.read(reinterpret_cast<char*>(&s),  sizeof(float));
    if (!d_is || d_is.eof())
      {
  d_working.d_frameIndex = -1;
  return(d_working.d_frameIndex);
      }
    if (d_verbose)
      {
  std::cerr << "x0: " << x0;
  std::cerr << "  y0: " << y0;
  std::cerr << "  z0: " << z0;
  std::cerr << "  s: " << s << std::endl;
      }
    
    for (std::vector<Position>::iterator iter = d_currentFrame_p->begin();
   iter != d_currentFrame_p->end();
   ++iter)
      {
  unsigned short sx, sy, sz;
  d_is.read(reinterpret_cast<char*>(&sx), sizeof(short));
  d_is.read(reinterpret_cast<char*>(&sy), sizeof(short));
  d_is.read(reinterpret_cast<char*>(&sz), sizeof(short));
  
  (*iter).d_x = x0 + sx * s;
  (*iter).d_y = y0 + sy * s;
  (*iter).d_z = z0 + sz * s;
  if (!d_is || d_is.eof())
    {
      std::cerr << "Reached the end of the file" << std::endl;
      d_working.d_frameIndex = -1;
      return(d_working.d_frameIndex);
    }
  if (d_verbose)
    {
      std::cerr << "sx: " << sx;
      std::cerr << "  sy: " << sy;
      std::cerr << "  sz: " << sz << std::endl;;
      
    }
      }
    ++d_working.d_frameIndex;
    if (d_verbose)
      {
  std::cerr << "Completed frame[" << d_working.d_frameIndex << "]" <<
    std::endl;
      }
    return(d_working.d_frameIndex);
  }

  BmtReadConstIterator::BmtReadConstIterator(BmtRead& reader) :
    d_reader_p(&reader), d_frameIndex(-1)
  {}

  BmtReadConstIterator::BmtReadConstIterator(const BmtReadConstIterator& proto)
    : d_reader_p(proto.d_reader_p), d_frameIndex(proto.d_frameIndex)
  {}

  BmtReadConstIterator& BmtReadConstIterator::operator=(const
              BmtReadConstIterator& other)
  {
    d_reader_p = other.d_reader_p;
    d_frameIndex = other.d_frameIndex;
    return(*this);
  }

  const std::vector<Position>&
  BmtReadConstIterator::operator*() const
  {
    if (d_frameIndex < 0)
      {
  return(*(reinterpret_cast<std::iterator_traits<BmtReadConstIterator>::pointer>(NULL)));
      }
    return(*d_reader_p->d_currentFrame_p);
  }

  BmtReadConstIterator& BmtReadConstIterator::operator++() // prefix
  {
    d_frameIndex = d_reader_p->nextFrame();
    return(*this);
  }

  BmtReadConstIteratorOld BmtReadConstIterator::operator++(int) // postfix
  {
    if (d_frameIndex < 0)
      {
  return(BmtReadConstIteratorOld(*reinterpret_cast<BmtRead*>(NULL)));
      }
    operator++();
    return(BmtReadConstIteratorOld(*d_reader_p));
  }

  BmtReadConstIteratorOld::BmtReadConstIteratorOld(BmtRead& reader)
    : d_reader_p(&reader)
  {}

  std::iterator_traits<BmtReadConstIterator>::value_type
  BmtReadConstIteratorOld::operator*()
  {
    return(*d_reader_p->d_oldFrame_p);
  }
}

