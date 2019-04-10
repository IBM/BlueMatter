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
 // NoesyList.h: interface for the NoesyList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOESYLIST_H__FD0A8E90_AF3D_4D7E_801B_40D4314FACD3__INCLUDED_)
#define AFX_NOESYLIST_H__FD0A8E90_AF3D_4D7E_801B_40D4314FACD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

using namespace std;

typedef vector<int> VINT;

class NoesyList  
{
public:
    char m_FileName[512];
    vector<VINT> m_Ids; // group of list.  each element is the list of ids for that group
    vector<int> m_NIds; // list of number of elements in each group
    vector<int> m_IdsAsList;  // single list of all ids in all groups, in order
    vector<int> m_StartingIndex;  // starting index of the group
    vector<int> m_NIdsAllFragments; // includes fragment count
    vector<int> m_StartingIndexAllFragments;  // includes fragment multiplicity
    int m_NGroups;  // number of groups of id's
    int m_NTotalIds; // total number of id's in all groups
    int m_NTotalIdsAllFragments;  // total number of id's in all groups and fragments

    NoesyList(char *fname);
    virtual ~NoesyList();

};

#endif // !defined(AFX_NOESYLIST_H__FD0A8E90_AF3D_4D7E_801B_40D4314FACD3__INCLUDED_)
