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
 
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

#define BUFFSIZE 2048
#define MAXATM   1000000
#define PDBPREFIX 30
#define PDBSUFFIX 55
#define PDBWIDTH  80

int ReadLine(FILE *fPF,char *bufS,int &nframe,int &atid, double &x, double &y,double &z)
{
	fgets(bufS,132,fPF);
	if(feof(fPF))
	{
		printf("EOF\n");
		exit(1);
	}
	int nargs = sscanf(bufS,"%d , %d , %lf , %lf , %lf",&nframe,&atid,&x,&y,&z);
	assert (nargs == 5);
	return 1;
}

int
main(int argc, char **argv, char **envp)
{
    // clean this up to avoid fixed size buffers
    char buff[BUFFSIZE];
    char pdbtxt[MAXATM][PDBWIDTH], pdbsuf[MAXATM][PDBWIDTH];


    if (argc != 8 && argc != 11) {
        cout << "csv2pdb csvFileName PDBFileName SeedName start stop skip "
             << endl;
        return 1;
    }

    char *csvname = argv[1];
    char *pdbname = argv[2];
    char *seedname = argv[3];
    int start = atoi(argv[4]);
    int stop = atoi(argv[5]);
    int skip = atoi(argv[6]);

    bool do_subrange = false;
    int first_atom = 0;
    int last_atom = 0;
    
	FILE *fPF = fopen(csvname,"r");
	assert(fPF);
    


    // Read the pdb file
    // Keep only ATOM lines
    FILE *f = fopen(pdbname, "r");
    assert(f);
    fgets(buff, BUFFSIZE, f);
    int npdbatom = 0;
    while (!feof(f) && !ferror(f)) 
        {
        if (!strncmp(buff, "ATOM", 4)) 
            {
            buff[PDBWIDTH-1] = '\0';
            buff[PDBPREFIX] = '\0';
            strcpy(pdbtxt[npdbatom], buff);
            strcpy(pdbsuf[npdbatom], &buff[PDBSUFFIX]);
            npdbatom++;
            }
        fgets(buff, BUFFSIZE, f);
        }
    fclose(f);

    int readerr=0;
    int nframe = -1;
	int cframe;
	int atid = -1;
	double x = 0.0,y = 0.0,z =0.0;

	char bufS[1024];

	ReadLine(fPF,bufS,nframe,atid,x,y,z);
	cframe = nframe;

	while(!feof(fPF))
        {
                char currentpdbname[1024]; 
                sprintf(currentpdbname,"%s.%05d.pdb",seedname,nframe);
                FILE *pdbout = fopen(currentpdbname, "w");
		fprintf(pdbout,"REMARK Frame = ",nframe);

		do
                {
                    fprintf(pdbout, "%s%8.3f%8.3f%8.3f %s", pdbtxt[atid-1],x,y,z ,pdbsuf[atid-1]);
		     ReadLine(fPF,bufS,nframe,atid,x,y,z);
                } while(nframe == cframe);
                fprintf(pdbout,"END\n");
                fclose(pdbout);
	}
}
