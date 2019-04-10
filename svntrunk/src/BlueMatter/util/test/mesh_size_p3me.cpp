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
 
#include <iostream.h>
#include <strstream.h>
#include <math.h>

int opt_num_pts( double boxsize, double grdSp )
{
    double min = ceil(boxsize/grdSp);
    int a, b, c, d;
    int amin, bmin, cmin, dmin;
    int meshPts;
    double lnmin, rema, remb, remc;
    double excess, minexcess;
    static const double ln2 = log(2.), ln3 = log(3.), ln5 = log(5.), ln7 = log(7.);
    static const double ln2_1 = 1.0/ln2, ln3_1 = 1.0/ln3,
        ln5_1 = 1.0/ln5, ln7_1 = 1.0/ln7;

    lnmin = log(min);

    amin = (int)floor(lnmin*ln2_1) + 1;
    bmin = cmin = dmin = 0;
    minexcess = amin*ln2 - lnmin;

    for( a = 0; a <= (int)floor(lnmin*ln2_1) + 1; a++ )
    {
        rema = lnmin - a*ln2;
        for( b = 0; b<=(int)floor(rema*ln3_1)+1; b++ )
        {
            remb = rema - b*ln3;
            for( c = 0; c <= (int)floor(remb*ln5_1)+1; c++ )
            {
                remc = remb - c*ln5;
                d = (int)floor(remc*ln7_1) + 1;
                excess = d*ln7 - remc;
                if (excess<minexcess) {
                    amin = a;
                    bmin = b;
                    cmin = c;
                    dmin = d;
                    minexcess = excess;
                }
            }
        }
    }

    meshPts = 1;
    for( a = 0; a < amin; a++ )
        meshPts *= 2;
    for( b = 0; b < bmin; b++ )
        meshPts *= 3;
    for( c = 0; c < cmin; c++ )
        meshPts *= 5;
    for( d = 0; d < dmin; d++ )
        meshPts *= 7;

    return meshPts;
}


int main( int argc, char *argv[] )
{
    if( argc != 3 )
    {
        cerr << "arguments:  BoxSize InitGridSpacing" << endl;
        exit(1);
    }

    double BoxSize;
    istrstream iStrBoxSize( argv[1] );
    iStrBoxSize >> BoxSize;

    double InitGridSpacing;
    istrstream iStrInitGridSpaing( argv[2] );
    iStrInitGridSpaing >> InitGridSpacing;

    int NumMeshPoints;
    NumMeshPoints = opt_num_pts( BoxSize, InitGridSpacing );
    cout << NumMeshPoints << endl;
    return 0;
}
