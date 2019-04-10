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
 // Blake Fitch

#ifndef   _COMPLEXMPI_HPP_
#define   _COMPLEXMPI_HPP_
#if 0
#include <iostream.h>
template <typename TYPE>
class complexTemplate
  {
  public:
    TYPE re ;
    TYPE im ;
    complexTemplate()       { }
    complexTemplate(TYPE r, TYPE i = 0.0)       { re=r; im=i; }
    complexTemplate(const complexTemplate<TYPE>& c) { re = c.re;im = c.im; }
    template <typename T>
    friend ostream & operator << (ostream& os, const complexTemplate<T>  & c);
    template <typename T>
  friend  complexTemplate<T> operator*(double d, const complexTemplate<T>& c)
    {
        return complexTemplate<T>( d*c.re, d*c_im );
    }
    template <typename T>
  friend  complexTemplate<T> operator*(const complexTemplate<T>&, double);
  } ;

template <typename T>
inline double real(const complexTemplate<T>& a)
{
  return a.re;
}

template <typename T>
inline double imag(const complexTemplate<T>& a)
{
  return a.im;
}

template <typename TYPE>
ostream& operator << (ostream& os, const complexTemplate<TYPE>  & c)
{
    return os << "("<<c._re<<","<<c._im<<")";
}

//template <typename TYPE>
//inline  complexTemplate<TYPE> operator*(double d, const complexTemplate<TYPE>& c)
//{ 
//  return complexTemplate<TYPE>( d*c._re, d*c._im );
//}


template <typename TYPE>
inline  complexTemplate<TYPE> operator*(const complexTemplate<TYPE>& c, double d)
{
  return d * c;
}
#endif
#endif
