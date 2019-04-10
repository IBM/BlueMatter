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
 #include <cstdio>
#include <cstdlib>
using namespace std ;
#include <BlueMatter/ExternalDatagram.hpp>

main(int argc, char **argv)
{
  FILE *dvsin, *dvsout;
  int h,t,s,i;
  //  double xyz[3],vxyz[3];
  XYZ xyz,vxyz;
  char buff[128];

  /* check for command line arg */
  if (argc != 3) {
    printf("xtest <filename1> <filename2> \n");
    exit(1);
  }

  /* open file for input */
  if ((dvsin = fopen(argv[1], "r")) == NULL) {
    printf("cannot open file \n");
    exit(1);
  }

  /* open file for output */
  if ((dvsout = fopen(argv[2], "w")) == NULL) {
    printf("cannot open file \n");
    exit(1);
  }

  while (!feof(dvsin)) {
    if (fread(&h, 4, 1, dvsin)) {
      fread(&t, 4, 1, dvsin);
      fread(&s, 4, 1, dvsin);
      fread(&xyz, 8, 3, dvsin);
      fread(&vxyz, 8, 3, dvsin);
      //printf("%d %d %d %f %f %f %f %f %f \n",h,t,s,xyz[0],xyz[1],xyz[2],vxyz[0],vxyz[1],vxyz[2]);
      ED_FillPacket_DynamicVariablesSite(buff,0,0,s,xyz,vxyz);
      fwrite(buff,ED_DynamicVariablesSite::PackedSize,1,dvsout);
    }
  }

  fclose(dvsin);
  fclose(dvsout);
}


