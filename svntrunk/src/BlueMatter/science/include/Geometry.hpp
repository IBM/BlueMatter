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
 #ifndef _GEOMETRY_HPP_
#define _GEOMETRY_HPP_

using namespace std;
#include <math.h>
#include <assert.h>
#include <BlueMatter/XYZ.hpp>

template <typename T, typename type>
void Angle(const T &a, const T &b, const T &c, type &angle)
    {
    T ab = a - b;
    T cb = c - b;
    
    type len = ab.Length()*cb.Length();
    type cosine = ab.DotProduct(cb)/len;
    if (cosine > 1.0) cosine = 1.0;
    if (cosine < -1.0) cosine = -1.0;
    angle= (type)acos(cosine);
    }


template <typename T, typename type>
void Torsion(const T &a, const T &b, const T &c, const T &d, type &angle)
    {
    T ba = b - a;
    T cb = c - b;
    T dc = d - c;
    T abc = ba.crossProduct(cb);
    T bcd = cb.crossProduct(dc);
    type len = abc.Length() * bcd.Length();
    if (len > 0.0)
        {
        type cosine = abc.DotProduct(bcd) / len;
        if (cosine > 1.0) cosine = 1.0;
        if (cosine < -1.0) cosine = -1.0;
        angle = acos(cosine);
        type sign = ba.DotProduct(bcd);
        if (sign < 0.0)
            {
            angle= -angle;
            }
        }
    else
        {
        angle = 0.0;
        }
    }
#endif
