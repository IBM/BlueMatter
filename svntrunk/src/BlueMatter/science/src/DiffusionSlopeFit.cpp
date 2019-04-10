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

using namespace std ;


int main(int argc, char **argv)
{
    if (argc < 2) {
	cerr << argv[0] << " fname.dat width" << endl;
	cerr << "Input: lower_rsq, lower_rsq_sigma, upper_rsq, upper_rsq_sigma, all_rsq, all_rsq_sigma" << endl;
	cerr << "Output: local slope fit to plus/minus 1ns" << endl;
	exit(-1);
    }

    TableFile tf(argv[1]);
    int ncols = tf.m_Columns.size();
    if (ncols != 1+3*2) {
	cerr << "ncols is " << ncols << endl;
	exit(-1);
    }
    int nrows = tf.m_NRows;

    Column &T = tf.m_Columns[0];
    Column L, LS, U, US, A, AS;
    double dt = 3.0;
    if (argc > 2)
	dt = atof(argv[2]);

    T.linearFit(tf.m_Columns[1], tf.m_Columns[2], dt, L, LS);
    T.linearFit(tf.m_Columns[3], tf.m_Columns[4], dt, U, US);
    T.linearFit(tf.m_Columns[5], tf.m_Columns[6], dt, A, AS);

    for (int i=0; i<L.m_NPts; i++) {
	cout << T.m_Data[i] << " "
	     << L.m_Data[i] << " " << LS.m_Data[i] << " "
	     << U.m_Data[i] << " " << US.m_Data[i] << " "
	     << A.m_Data[i] << " " << AS.m_Data[i] << endl;
    }
    return 0;
}