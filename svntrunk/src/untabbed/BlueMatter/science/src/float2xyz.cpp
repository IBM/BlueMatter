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

using namespace std ;

int main(int argc, char **argv)
{
    const int nperleaf=36;
    const int nlip = 2*nperleaf;
    float dummy;
    float v[3];
    

    FILE *f = fopen("dry_f13.bin", "rb");
    fread(&dummy, sizeof(float), 1, f);
    fread(&dummy, sizeof(float), 1, f);
    FILE *fcom = fopen("dry_f13_com.txt", "w");
    FILE *flip = fopen("dry_f13_lip.txt", "w");
    while (!feof(f)) {
  // skip COM's
  fread(v, sizeof(float), 3, f);
  fprintf(fcom, "%f %f %f\n", v[0], v[1], v[2]);
  fread(v, sizeof(float), 3, f);
  fprintf(fcom, "%f %f %f\n", v[0], v[1], v[2]);
  for (int i=0; i<72; i++) {
      fread(v, sizeof(float), 3, f);
      if (i == 0)
        fprintf(flip, "%f %f %f\n", v[0], v[1], v[2]);
  }
    }
    fclose(f);
    fclose(fcom);
    fclose(flip);
}
