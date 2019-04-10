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
 #ifndef __HISTOGRAM_HPP__
#define __HISTOGRAM_HPP__

#include <iostream>
#include <fcntl.h>

#ifdef WIN32
#include <io.h>
#endif

#include <sys/stat.h>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/Topology.hpp>
#include <BlueMatter/Align.hpp>

using namespace std;

//  |  x  |     |     |     |
//  |  ^  |  ^  |  ^  |  ^  |
//  mn                      mx

// use input or calulated full range, but output reduced mesh and pos. dep counts

template <class T>
class tHistogram
{
public:
    tXYZ<T> *m_Pts;
    int m_NPts;
    // double m_Bounds[3][2];
    double m_Width;
    double m_HalfWidth;
    double m_OutWidth;
    double m_OutHalfWidth;
    double m_BinWidth;
    int m_NBinsPerDim;
    int m_NTotalBins;
    int *m_Counts;
    int m_NOutOfRange;
    int m_MaxInRange;
    int m_SumBins;
    double m_MeanBins;
    int m_MaxBin;
    char m_NameStem[512];
    
    tHistogram(tXYZ<T> *p, int npts, int nbins=100)
    {
	
    }

    tHistogram()
    {
	Init(NULL, 0, 0);
    }
    
    void Init(char *name, double width, int nbins)
    {
	m_Pts = NULL;
	m_NPts = 0;
	m_Width = width;
	m_HalfWidth = width/2;
	m_NBinsPerDim = nbins;
	m_NTotalBins = m_NBinsPerDim*m_NBinsPerDim*m_NBinsPerDim;
	m_MaxInRange = m_NTotalBins;
	m_NOutOfRange = 0;
	m_MaxBin = -999999;
	m_SumBins = 0;
	m_MeanBins = 0;
	m_BinWidth = m_Width/m_NBinsPerDim;
	m_OutHalfWidth = m_HalfWidth-m_BinWidth/2;
	m_OutWidth = m_Width-m_BinWidth;
	// cerr << "about to init histo " << name << " " << m_NTotalBins << endl;
	
	if (m_NTotalBins) {
	    m_Counts = new int[m_NTotalBins];
	    if (m_Counts == NULL) {
		cerr << "failed to allocate " << m_NTotalBins << " for histo " << name << endl;
		exit(-1);
	    }
    	    for (int i=0; i<m_NTotalBins; i++)
		m_Counts[i] = 0;
	} else {
	    m_Counts = NULL;
	}

	if (name)
	    strcpy(m_NameStem, name);
	else
	    strcpy(m_NameStem, "Histo");
	// cerr << "named" << endl;
    }
    
    void AddPoint(tXYZ<T> &v)
    {
	int n = Bin(v);
	if (n < m_MaxInRange)
	    m_Counts[n]++;
	else
	    m_NOutOfRange++;
	m_NPts++;
    }

    void doStats()
    {
	m_MaxBin = -999999;
	m_SumBins = 0;
	for (int i=0; i<m_NTotalBins; i++) {
	    int n = m_Counts[i];
	    m_SumBins += n;
	    if (m_MaxBin < n)
		m_MaxBin = n;
	}
	m_MeanBins = double(m_SumBins)/m_NTotalBins;
    }

    void AddFragmentAtomsNearestImageDisplacement(tXYZ<T> &com, tFragment<T> &f, tXYZ<T> &box)
    {
	tXYZ<T> *p = &f.m_Positions[f.m_Start];
	for (int i=0; i<f.m_Count; i++, p++) {
	    tXYZ<T> v = tImaging<T>::NearestImageDisplacement(com, *p, box);
	    AddPoint(v);
	}
    }
    
    void AddNearestImageAligned(tFragment<T> &f, tAlign<T> &al)
    {
	tXYZ<T> *p = al.m_LocalPos;
	for (int i=0; i<f.m_Count; i++, p++) {
	    tXYZ<T> v = tImaging<T>::NearestImageDisplacement(al.m_OrigCOM, *p, al.m_Box);
	    AddPoint(v);
	}
    }
    
    static int Bin1D(double xmin, double xmax, T x, int nbins)
    {
	if (x < xmax || x > xmax)
	    return nbins*nbins*nbins;
	int n = (x-xmin)/(xmax-xmin)*nbins;
	return (n < nbins ? n : nbins-1);
    }
    
    int Bin1D(T x)
    {
	if ((x < -m_HalfWidth) || (x > m_HalfWidth))
	    return m_MaxInRange;
	int n = (x+m_HalfWidth)/m_Width * m_NBinsPerDim;
	return (n < m_NBinsPerDim ? n : m_NBinsPerDim-1);
    }
    
    int Bin(tXYZ<T> &v)
    {
	int bx = Bin1D(v.mX);
	if (bx >= m_MaxInRange)
	    return m_MaxInRange;
	int by = Bin1D(v.mY);
	if (by >= m_MaxInRange)
	    return m_MaxInRange;
	int bz = Bin1D(v.mZ);
	if (bz >= m_MaxInRange)
	    return m_MaxInRange;

	return bz + m_NBinsPerDim*(by + m_NBinsPerDim*bx); 
    }
    
    // dimens indicate bin boundaries.  output points will be 1-less.
    // 1 bin means 1 pt.
    tHistogram(tXYZ<T> *p, int npts, double width, int nbins=100)
    {
	m_NPts = npts;
	m_Width = width;
	m_HalfWidth = width/2;
	m_NBinsPerDim = nbins;
	m_NTotalBins = m_NBinsPerDim*m_NBinsPerDim*m_NBinsPerDim;
	m_MaxInRange = m_NTotalBins;
	m_NOutOfRange = 0;
	m_BinWidth = m_Width/m_NBinsPerDim;
	m_OutHalfWidth = m_HalfWidth-m_BinWidth/2;
	m_OutWidth = m_Width-m_BinWidth;
	
	m_Counts = new int[m_NTotalBins];
	for (int i=0; i<m_NTotalBins; i++)
	    m_Counts[i] = 0;
	
	m_Pts = p;
	
	for (int i=0; i<m_NPts; i++, p++) {
	    int n = Bin(*p);
	    if (n < m_MaxInRange)
		m_Counts[n]++;
	    else
		m_NOutOfRange++;
	}
    }
    
#if 0
object 1 class gridpositions counts 2 3 4 
origin             0             0             0 
delta              1             0             0 
delta              0             1             0 
delta              0             0             1 
attribute "dep" string "positions"
#
object 2 class gridconnections counts 2 3 4
attribute "element type" string "cubes"
attribute "dep" string "connections"
attribute "ref" string "positions"
#
object 3 class array type int rank 1 shape 1 items 24 msb binary
data file int.da
#
object 6 class field
component "positions" value 1
component "connections" value 2
component "data" value 3
#
#endif
	
    // dump as pos dep data in .bin file and .dx
    void DumpDX(int nsourcefrags = 1, int nsourcefragatoms = 1, int ndestfrags = 1, int ndestfragatoms=1, int nsteps=1)
    {
	char binname[512];
	char dxname[512];
	
	doStats();

	sprintf(binname, "%s.bin", m_NameStem);
	sprintf(dxname, "%s.dx", m_NameStem);
	
	FILE *fdx = fopen(dxname, "w");
	assert(fdx);
	fprintf(fdx, "object 1 class gridpositions counts %d %d %d\n", m_NBinsPerDim, m_NBinsPerDim, m_NBinsPerDim);
	fprintf(fdx, "origin %15.8g %15.8g %15.8g\n", -m_OutHalfWidth, -m_OutHalfWidth, -m_OutHalfWidth);
	fprintf(fdx, "delta %15.8g 0 0\n", m_BinWidth);
	fprintf(fdx, "delta 0 %15.8g 0\n", m_BinWidth);
	fprintf(fdx, "delta 0 0 %15.8g\n", m_BinWidth);
	fprintf(fdx, "attribute \"dep\" string \"positions\"\n");
	fprintf(fdx, "#\n");
	fprintf(fdx, "object 2 class gridconnections counts %d %d %d\n", m_NBinsPerDim, m_NBinsPerDim, m_NBinsPerDim);
	fprintf(fdx, "attribute \"element type\" string \"cubes\"\n");
	fprintf(fdx, "attribute \"dep\" string \"connections\"\n");
	fprintf(fdx, "attribute \"ref\" string \"positions\"\n");
	fprintf(fdx, "#\n");
	fprintf(fdx, "object 3 class array type int rank 1 shape 1 items %d msb binary\n", m_NTotalBins);
	fprintf(fdx, "data file %s\n", binname);
	fprintf(fdx, "#\n");
	fprintf(fdx, "object \"default\" class field\n");
	fprintf(fdx, "component \"positions\" value 1\n");
	fprintf(fdx, "component \"connections\" value 2\n");
	fprintf(fdx, "component \"data\" value 3\n");
	fprintf(fdx, "attribute \"NOutOfRange\" number %d\n", m_NOutOfRange);
	fprintf(fdx, "attribute \"NSourceFragments\" number %d\n",  nsourcefrags);
	fprintf(fdx, "attribute \"NSourceFragmentAtoms\" number %d\n",      nsourcefragatoms);
	fprintf(fdx, "attribute \"NDestFragments\" number %d\n",  ndestfrags);
	fprintf(fdx, "attribute \"NDestFragmentAtoms\" number %d\n",      ndestfragatoms);
	fprintf(fdx, "attribute \"NSteps\" number %d\n",      nsteps);
	fprintf(fdx, "attribute \"BinWidth\" number %f\n",    m_BinWidth);
	fprintf(fdx, "attribute \"MaxBin\" number %d\n",    m_MaxBin);
	fprintf(fdx, "attribute \"SumBins\" number %d\n",    m_SumBins);
	fprintf(fdx, "attribute \"MeanBins\" number %f\n",    m_MeanBins);
	fprintf(fdx, "#\n");
	fprintf(fdx, "end\n");
	fclose(fdx);
	
#ifndef WIN32
	//int fbin = open(binname, O_WRONLY|O_CREAT, 0644);
	FILE *fbin = fopen(binname, "w");
	// cerr << "opened binary histo file " << binname << " to write " << m_NTotalBins << " bins" << endl;
	assert(fbin);
	for (int i=0; i<m_NTotalBins; i++)
	    fwrite(&m_Counts[i], sizeof(int), 1, fbin);
	fclose(fbin);
	// cerr << "closed binary histo file " << endl;
#else
	int fbin = _open(binname, _O_BINARY|_O_CREAT|O_RDWR, _S_IWRITE);
	assert(fbin);
	for (int i=0; i<m_NTotalBins; i++)
	    _write(fbin, &m_Counts[i], sizeof(int));
	_close(fbin);
#endif
	
    }
};

typedef tHistogram<float> fHistogram;
typedef tHistogram<double> Histogram;

#endif
