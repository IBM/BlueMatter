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
 // *********************************************************************
// File: reduce.hpp
// Author: RSG
// Date: April 25, 2004
// Class: Reduce
// Description: Algorithm to do a reduction (accumulation of
//              N elements at a time in [first, last). Accumulated
//              values are placed into the successive elements starting
//              from the OutputIterator result. 
//              The return value of algorithm is the new "last" value.
// *********************************************************************
#ifndef INCLUDE_REDUCE_HPP
#define INCLUDE_REDUCE_HPP

#include <numeric>

template <class InputIterator, class OutputIterator, class T>
OutputIterator Reduce(InputIterator first,
                      InputIterator last,
                      OutputIterator result,
                      T init,
                      int repeatCount)
{
  InputIterator currentFirst = first;
  InputIterator currentLast = first;
  OutputIterator next = result;
  int k = 0;
  while(currentLast != last)
    {
      while((currentLast != last) && (k < repeatCount))
        {
          ++k;
          ++currentLast;
        }
      *next = std::accumulate(currentFirst, currentLast, init);
      currentFirst = currentLast;
      ++next;
      k = 0;
    }
  return(next);
}

#endif
