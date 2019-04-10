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
 #include <math.h>
#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include "BMT.hpp"
#include "Imaging.hpp"
#include <BlueMatter/XYZ.hpp>
#include "JRand.hpp"
#include "/fs/dx_basic/BinaryIO/Histogram.hpp"
#include "/fs/TableFile/Column.h"

using namespace std ;

int main(int argc, char **argv)
{
    fXYZ v[72];
    double d = 1.0;

    float rmax = 1000.0;

    jrand r(rmax);

    for (int i=0; i<72; i++) {
  v[i].mX = (r.get()/rmax-0.5)*100.0;
  v[i].mY = (r.get()/rmax-0.5)*100.0;
  v[i].mZ = (r.get()/rmax-0.5)*100.0;
    }

    float dstep = 1.0/sqrt(2.0);
    for (int j=0; j<4000; j++) {
  for (i=0; i<72; i++) {
      cout << v[i].mX << " " << v[i].mY << " " << v[i].mZ << endl;
      v[i].mX += dstep; // r.get()/rmax-0.5;
      v[i].mY += dstep; // r.get()/rmax-0.5;
      v[i].mZ += dstep; // r.get()/rmax-0.5;
  }
    }
    return 0;
}
