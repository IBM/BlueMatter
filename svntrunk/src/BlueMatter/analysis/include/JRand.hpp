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
 #ifndef INCL_JRAND_HPP
#define INCL_JRAND_HPP

#include <math.h>
#include <vector>

using namespace std ;

class jrand
{
public:
    vector<int> m_list;
    int m_max;
    int m_listsize;
    double m_INVFACT;

    jrand(int max=RAND_MAX, int listsize=5001)
    {
	m_max = max;
	m_listsize = listsize;
	m_INVFACT = 1.0/(RAND_MAX+1.0);
	for (int i=0; i<listsize; i++)
	    m_list.push_back(int(rand()*m_INVFACT*max));
    }

    // this returns a number from 0 to m_max-1;
    int get() {
	int n=int(rand()*m_INVFACT*m_listsize);
	if (n<0 || n >= m_listsize)
	    cerr << "n too big " << n << endl;
	int r=int(rand()*m_INVFACT*m_max);
	if (r<0 || r >= m_max)
	    cerr << "r too big " << r << endl;
	int x = m_list[n];
	if (x<0 || x >= m_max)
	    cerr << "x too big " << x << endl;
	m_list[n] = r;
	return x;
    }

    float fget() {
	int n=int(rand()*m_INVFACT*m_listsize);
	if (n<0 || n >= m_listsize)
	    cerr << "n too big " << n << endl;
	int r=int(rand()*m_INVFACT*m_max);
	if (r<0 || r >= m_max)
	    cerr << "r too big " << r << endl;
	int x = m_list[n];
	if (x<0 || x >= m_max)
	    cerr << "x too big " << x << endl;
	m_list[n] = r;
	return x/(m_max+1.0);
    }

};

#endif
