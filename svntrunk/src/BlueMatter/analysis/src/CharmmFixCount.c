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
 #include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int f;
	int NFrames;
	int NOrig;

	if (argc < 3) {
		printf("CharmmFixCount fname.dcd count\nChanges count in Charmm dcd trajectory file\n");
		exit(0);
	}

	f = open(argv[1], O_RDWR);
	if (f<0) {
		printf("Error, can't open %s\n", argv[1]);
		exit(1);
	}
	lseek(f, 8, SEEK_SET);
	read(f, &NOrig, sizeof(NOrig));
	lseek(f, 8, SEEK_SET);
	NFrames = atoi(argv[2]);
	if (!NFrames) {
		printf("Frame number converted to 0.  No changes made.\n");
		goto error;
	}

	if (sizeof(NFrames) != write(f, &NFrames, sizeof(NFrames))) {
		printf("Error writing number to file.\n");
		goto error;
	}
	printf("Converted frame count from %d to %d\n", NOrig, NFrames);
error:
	close(f);
	exit(0);
}

