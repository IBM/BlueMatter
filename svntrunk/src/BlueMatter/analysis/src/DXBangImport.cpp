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
 #include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#define usleep Sleep
#endif

//
int getFrame(int lastframe)
{
    int waiting = 1;
    int nframe;

    while (waiting) {
	FILE *f = fopen("FrameIndex.bin", "rb");
	while (!f) {
	    usleep(5000);
	    f = fopen("FrameIndex.bin", "rb");
	}
	char c;
	int nread = fread((void *)&c, 1, 1, f);
	while (nread != 1) {
	    usleep(5000);
	    nread = fread((void *)&c, 1, 1, f);
	}
	fclose(f);
	nframe = int(c);
	if (lastframe == nframe) {
	    usleep(5000);
	} else {
	    waiting = 0;
	}
    }
    return nframe;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
	printf("%s lastframe <loop>\n", argv[0]);
	exit(-1);
    }

    int lastframe = atoi(argv[1]);
    int loop = (argc > 2) ? 1 : 0;


    int framesize = 5239*12+256*4+8;
    int frametoread;
    int maxframe = 200;

    if (lastframe == 255) {
	if (loop)
	    frametoread = 0;
	else
	    frametoread = getFrame(-1);
    } else {
	if (loop)
	    frametoread = (lastframe+1+maxframe)%maxframe;  // just get next one
	else
	    frametoread = getFrame(lastframe);	// poll for next different frame
    }


    int startoffset = frametoread*framesize;
    char buff[4096];
    sprintf(buff,
    "object 1 class array type float rank 1 shape 3 items 259 msb ieee data file OutFrames.bin,%d\n"
    "object 2 class array type float rank 1 shape 3 items 4980 msb ieee data file OutFrames.bin,%d\n"
    "object 3 class array type float rank 0 items 256 msb ieee data file OutFrames.bin,%d\n"
    "object 4 class array type int rank 0 items 1 msb ieee data file OutFrames.bin,%d\n"
    "object 5 class array type int rank 0 items 1 msb ieee data file OutFrames.bin,%d\n"
    "object 6 class array type int rank 0 items 1 data follows\n"
    "%d\n"
    "object \"default\" class group\n"
    "member \"protein\" value 1\n"
    "member \"water\" value 2\n"
    "member \"hbe\" value 3\n"
    "member \"timestep\" value 4\n"
    "member \"frame\" value 5\n"
    "member \"index\" value 6\n"
    "end",
    startoffset, startoffset+3108, startoffset+62868, startoffset+63892, startoffset+63896, frametoread);

    printf("%s\n", buff);
    return 0;
}
