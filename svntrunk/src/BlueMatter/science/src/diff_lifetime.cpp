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
#include <BlueMatter/BMT.hpp>
#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/TableFile.hpp>
#include <BlueMatter/HBond.hpp>

using namespace std ;

int main(int argc, char **argv)
{
    const int nperleaf=36;
    const int nlip = 2*nperleaf;
    double timestep = 0.005;
    // x y z * 72, lower leaf first
    // thus tf will have x0, x1, x2, ... x71, y0, y1... z71
    TableFile tf("/fs/lipid5/sope_diff_xyz.dat", nlip);
    int ncols = tf.m_Columns.size();
    assert(ncols == nlip*3);
    int nrows = tf.m_NRows;

    double *l2d, *u2d, *l3d, *u3d;
    double *l2dmean, *u2dmean, *l3dmean, *u3dmean;
    int NDPtsl2d=0, NDPtsu2d=0, NDPtsl3d=0, NDPtsu3d=0;
    l2dmean = new double[nrows];
    u2dmean = new double[nrows];
    l3dmean = new double[nrows];
    u3dmean = new double[nrows];

    for (int i=0; i<nrows; i++)
	l2dmean[i] = u2dmean[i] = l3dmean[i] = u3dmean[i] = 0;

    for (int i=0; i<nperleaf; i++) {
	tf.m_Columns[i].diffusion2D(l2d, NDPtsl2d, tf.m_Columns[i+nlip]);
	tf.m_Columns[i+nperleaf].diffusion2D(u2d, NDPtsu2d, tf.m_Columns[i+nperleaf+nlip]);
	tf.m_Columns[i].diffusion3D(l3d, NDPtsl3d, tf.m_Columns[i+nlip], tf.m_Columns[i+2*nlip]);
	tf.m_Columns[i+nperleaf].diffusion3D(u3d, NDPtsu3d, tf.m_Columns[i+nperleaf+nlip], tf.m_Columns[i+nperleaf+2*nlip]);
	for (int j=0; j<NDPtsl2d; j++) {
	    l2dmean[j] += l2d[j]/nperleaf/4.0;
	    u2dmean[j] += u2d[j]/nperleaf/4.0;
	    l3dmean[j] += l3d[j]/nperleaf/6.0;
	    u3dmean[j] += u3d[j]/nperleaf/6.0;
	}
    }
    
    for (int i=0; i<NDPtsl2d; i++) {
	double t = i*timestep;
	cout << t << " " << l2dmean[i] << " " << u2dmean[i] << " " << (l2dmean[i]+u2dmean[i])/2 << " "
	     << l3dmean[i] << " " << u3dmean[i] << " " << (l3dmean[i]+u3dmean[i])/2 << endl;
    }


    return 0;
}
