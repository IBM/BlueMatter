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
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <BlueMatter/BMT.hpp>
#include <BlueMatter/BoundingBox.hpp>
#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/XYZ.hpp>
//#include <fstream>

using namespace std;

#define BUFFSIZE 2048
#define MAXATM   1000000
#define PDBPREFIX 30
#define PDBSUFFIX 55
#define PDBWIDTH  80


int
main(int argc, char **argv, char **envp)
{
    // clean this up to avoid fixed size buffers
    char buff[BUFFSIZE];
    char pdbtxt[MAXATM][PDBWIDTH], pdbsuf[MAXATM][PDBWIDTH];
    vector<XYZ> coors;

#if 0
    vector<MoleculeRun> moleculeruns;
    char moleculerunstring[2048];
#endif

    if (argc != 8 && argc != 11) {
        cout << "bmt2pdb BMTFileName PDBFileName SeedName start stop skip "
             << "onefile <-atomrange begin end>"
             << endl;
        return 1;
    }

    char *bmtname = argv[1];
    char *pdbname = argv[2];
    char *seedname = argv[3];
    int start = atoi(argv[4]);
    int stop = atoi(argv[5]);
    int skip = atoi(argv[6]);
    int onefile = atoi(argv[7]);

    bool do_subrange = false;
    int first_atom = 0;
    int last_atom = 0;
    if ( (argc == 11) && (!strncmp("-atomrange", argv[8], 10)) )
        {
        do_subrange = true;
        // change over from 1-based pdb nomenclature to 0-based c-indexing
        first_atom = atoi(argv[9]) - 1;
        last_atom = atoi(argv[10]) - 1;
        }
    

    
#if 0
    bool do_imaging = false;
    if (argc >=8)
        {
        if (!strncmp("-image",argv[7],6)) 
            {
            do_imaging = true;
            }
        }
#endif
    XYZ *coor;

    BMTReader bmt(bmtname);
    int natom = bmt.getNAtoms();

    // make sure the selected subrange makes sense
    if (!do_subrange)
        {
        first_atom = 0;
        last_atom = natom - 1;
        }
    else
        {
        if (first_atom < 0)
            {
            cerr << "Index of first atom in selection range must be >= 1"
                 << endl;
            exit(-1);
            }
        if (first_atom > last_atom)
            {
            cerr << "first atom in selection range larger than last atom"
                 << endl;
            exit(-1);
            }
        if (last_atom >= natom)
            {
            cerr << "Index of last atom in selection range cannot be greater"
                 << " than the number of atoms"
                 << endl;
            exit(-1);
            }
        }

    if (do_subrange)
        {
        cerr << "Writing a subset of atoms " << first_atom+1 << "-" 
                                             << last_atom+1
                                             << endl;
        }

#if 0
    // TODO -- this is where I was editing -- get Ananth to show me how this 
    // should be done
    // get connectivity information
    ConnectivityString connectivity;
    if (bmt.hasAttrib("CONNECTIVITY", &typ))
        {
        bmt.getAttrib("CONNECTIVITY", &connectivity);
        }

    XYZ box;
    if (do_imaging)
        {
        BoundingBox myInitBox=bmt.getInitialBox();
        box = myInitBox.GetDimensionVector(); 
        }
#endif    

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

    if (natom != npdbatom)
        {
        printf("Num of atoms in PDB (%d) and BMT (%d) doesn't match",
                npdbatom, natom);
        exit(-1);
        }
    
    ofstream pdbout;

    int readerr = bmt.ReadFrame();
    int nframe = -1;
    while (!readerr && (nframe <=stop))
        {
        int ignore_frame = (nframe-start) % skip;
        coor = bmt.getPositions();

        if ( (nframe >= start) && (nframe <= stop) && (!ignore_frame) )
            {
            //ofstream pdbout(currentpdbname);
            if (!onefile)
                {
                char currentpdbname[1024]; 
                sprintf(currentpdbname,"%s.%05d.pdb",seedname,nframe);
                pdbout.open(currentpdbname);
                pdbout << "REMARK Frame = " << nframe << endl;
                }
            else
                {
                cout << "REMARK Frame = " << nframe << endl;
                }

            //for (int i=0; i<natom; i++)
            for (int i=first_atom; i<=last_atom; i++)
                {
                sprintf(buff, "%s%8.3f%8.3f%8.3f %s", pdbtxt[i], 
                        coor[i].mX, coor[i].mY, coor[i].mZ ,pdbsuf[i]);
                if (!onefile)
                    {
                    pdbout << buff; 
                    }
                else
                    {
                    cout << buff;
                    }
                } 
            if (!onefile)
                {
                pdbout << "END" << endl;
                pdbout.close();
                }
            else
                {
                cout << "END" << endl;
                }
            }
        readerr = bmt.ReadFrame();
        nframe++;
        }
}

void mapIntoLine(double x, double &v, double a, double l)
    {
    double dx = x-a;
    int k = fabs(dx/l);
    v = k*l;
    if (dx < 0)
        v = -v - l;
    }
