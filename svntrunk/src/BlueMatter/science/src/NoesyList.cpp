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
 // NoesyList.cpp: implementation of the NoesyList class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <BlueMatter/NoesyList.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// This only loads the group info - it knows nothing about the corresponding fragment multiples
NoesyList::NoesyList(char *fname)
{
    strcpy(m_FileName, fname);
    FILE *f = fopen(fname, "r");
    char buff[1024];
    *buff = '\0';
    fgets(buff, 1024, f);
    m_NGroups = 0;
    m_NTotalIds = 0;

    int nsum = 0;
    while(strlen(buff)>0) {
	VINT *v = new VINT;
	char *t = strtok(buff, " \t\n\f\r");
	while(t) {
	    int n = atoi(t)-1;    // HERE CONVERT TO 0-BASED!!
	    v->push_back(n);
	    m_IdsAsList.push_back(n);
	    t = strtok(NULL, " \t\n\f\r");
	}
	m_Ids.push_back(*v);
	m_NIds.push_back(v->size());
	m_StartingIndex.push_back(nsum);
	nsum += v->size();
	if (!fgets(buff, 1024, f))
	    break;
    }
    fclose(f);
    m_NGroups = m_Ids.size();
    m_NTotalIds = m_IdsAsList.size();
}

NoesyList::~NoesyList()
{
}
