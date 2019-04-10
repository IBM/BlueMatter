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
 // XYZColumn.h: interface for the XYZColumn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XYZCOLUMN_H__58207EC6_0791_42FA_919E_A4C6EB4C03A7__INCLUDED_)
#define AFX_XYZCOLUMN_H__58207EC6_0791_42FA_919E_A4C6EB4C03A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>
#include <cmath>
#include <fcntl.h>
#include <cstdio>
#ifdef WIN32
#include <io.h>
#define SWAP_IN
#define SWAP_OUT
#endif
#include <iostream>
#include <ostream>
#include <fstream>
#include <vector>

#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/Swap.hpp>
#include <BlueMatter/Topology.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/BoundingBox.hpp>
#include <BlueMatter/ExternalDatagram.hpp> // for tSiteData
#include <BlueMatter/Column.hpp>

using namespace std ;


class XYZColumn  
{
public:

    vector<XYZ> m_Data;

    XYZ m_Center;

    double m_Radius;
    
    int getNPts() {
	return m_Data.size();
    }

    void clear() {
	m_Data.clear();
    }

    void add(const XYZ &v) {
	m_Data.push_back(v);
    }


    XYZColumn()
    {
	m_Radius = 0;
	m_Center = XYZ::ZERO_VALUE();
    }

    // for now this does not include periodic imaging
    // but this is ok as long as the input positions don't wrap
    void findBoundingSphere()
    {
	m_Center = XYZ::ZERO_VALUE();
	m_Radius = 0;
	int n = getNPts();
	if (n < 1)
	    return;
	for (int i=0; i<n; i++) {
	    XYZ p = m_Data[i];
	    m_Center += m_Data[i];
	}
	m_Center = m_Center/n;
	for (int i=0; i<n; i++) {
	    double d = m_Data[i].DistanceSquared(m_Center);
	    if (m_Radius < d)
		m_Radius = d;
	}
	m_Radius = sqrt(m_Radius);
    }

    virtual ~XYZColumn() {
	clear();
    }

};

#endif // !defined(AFX_XYZCOLUMN_H__58207EC6_0791_42FA_919E_A4C6EB4C03A7__INCLUDED_)
