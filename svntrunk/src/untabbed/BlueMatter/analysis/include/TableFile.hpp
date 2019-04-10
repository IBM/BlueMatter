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
 // TableFile.h: interface for the TableFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TABLEFILE_H__56CFFC1C_4F8B_4BDE_A12C_A5A544CFB9FB__INCLUDED_)
#define AFX_TABLEFILE_H__56CFFC1C_4F8B_4BDE_A12C_A5A544CFB9FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <BlueMatter/Column.hpp>

using namespace std;


class TableFile  
{
public:
    TableFile();
    TableFile(char *fname);
    TableFile(char *fname, int interleave);
    // TableFile(char *fname, char *s, ...);
    virtual ~TableFile();

    vector<Column> m_Columns;
    vector<double> m_Mean;
    vector<double> m_SD;
    // map<char*, Column> m_NameMap;
    char m_FileName[1024];
    FILE *m_File;
    enum {BUFFSIZE=8192};
    char m_Buffer[BUFFSIZE];
    int m_NColumns;
    int m_NRows;

    inline Column& operator[] (const int index)
    {
  return m_Columns[index];
    }

    inline Column& operator[] (const char *s)
    {
  for(vector<Column>::iterator i = m_Columns.begin(); i != m_Columns.end(); i++) {
      if (!strcmp(s, i->m_Name))
    return *i;
  }
  cerr << "Error - column " << s << " not found." << endl;
  exit(-1);

  // keep compiler quiet
  Column *cdummy = new Column();
  return *cdummy;
    }
};

#endif // !defined(AFX_TABLEFILE_H__56CFFC1C_4F8B_4BDE_A12C_A5A544CFB9FB__INCLUDED_)
