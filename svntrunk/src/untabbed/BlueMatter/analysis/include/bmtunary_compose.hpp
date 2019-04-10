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
// File: bmtunary_compose.hpp
// Author: RSG
// Date: October 6, 2002
// Namespace: bmt
// Class: unary_compose
// Description: class allowing composition of unary function objects.
//              It attempts to follow the description from Generic
//              Programming and the STL by Matthew Austern
// ********************************************************************

#ifndef INCLUDE_BMT_BMTUNARY_COMPOSE_HPP
#define INCLUDE_BMT_BMTUNARY_COMPOSE_HPP

#include <functional>

namespace bmt
{
  template <class TFunction2, class TFunction1>
  class unary_compose : public
  std::unary_function<typename TFunction2::result_type,
          typename TFunction1::argument_type>
  {
  private:
    TFunction1& d_f1;
    TFunction2& d_f2;
  public:
    inline unary_compose(TFunction2& f2, TFunction1& f1)
      : d_f1(f1), d_f2(f2)
    {}
    inline typename TFunction2::result_type operator()(typename TFunction1::argument_type x)
    {
      return d_f2(d_f1(x));
    }
    inline TFunction1& first()
    {
      return d_f1;
    }
    inline const TFunction1& first() const
    {
      return d_f1;
    }
    inline TFunction2& second()
    {
      return d_f2;
    }
    inline const TFunction2& second() const
    {
      return d_f2;
    }
  };
}
#endif
