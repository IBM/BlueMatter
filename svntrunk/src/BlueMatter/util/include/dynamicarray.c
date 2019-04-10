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
 // **************************************************************
// File:         dynamicarray.c
// Author:       Robert S. Germain
// Date:         March 17, 1997
// Class:        DynamicArray
// Inheritance:  none
// Description:  array that dynamically lengthens itself as necessary
//               
//
// **************************************************************

#include "dynamicarray.hpp"

#include <assert.h>
#include <stdlib.h>

template <class TElement>
DynamicArray<TElement>::DynamicArray(const int size) : d_size(size)
{
  assert(size > 0);
  d_base = new TElement[d_size];
  assert(d_base != NULL);
}

template <class TElement>
DynamicArray<TElement>::DynamicArray(const DynamicArray<TElement>& proto) :
  d_size(proto.d_size)
{
  d_base = new TElement[d_size];
  assert(d_base != NULL);
  for (int i = 0; i < d_size; i++)
    {
      d_base[i] = proto.d_base[i];
    }
}

template <class TElement>
DynamicArray<TElement>& DynamicArray<TElement>::operator=(const
						DynamicArray<TElement>& other)
{
  if (this == &other)
    {
      return *this;
    }

  if (d_size < other.d_size)
    {
      delete [] d_base;
      d_size = other.d_size;
      d_base = new TElement[d_size];
      assert(d_base != NULL);
    }

  for (int i = 0; i < d_size; i++)
    {
      d_base[i] = other.d_base[i];
    }
return *this;
}      

template <class TElement>
DynamicArray<TElement>::~DynamicArray()
{
  delete [] d_base;
}

template <class TElement>
TElement& DynamicArray<TElement>::operator[](const int index)
{
  int newSize;
  TElement* newBase;

  if (index >= d_size)
    {
      // double the current size until it is large enough
      newSize = d_size;
      while (index >= newSize)
	{
	  newSize = newSize*2;
	}
      newBase = new TElement[newSize];
      assert(newBase != NULL);
      for (int i = 0; i < d_size; i++)
	{
	  newBase[i] = d_base[i];
	}
      delete [] d_base;
      d_base = newBase;
      d_size = newSize;
    }
  return d_base[index];
}

template <class TElement>
TElement* DynamicArray<TElement>::base()
{
  return d_base;
}

template <class TElement>
int DynamicArray<TElement>::size() const      
{
  return d_size;
}
