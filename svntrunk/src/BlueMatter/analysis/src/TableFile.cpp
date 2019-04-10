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
 #include <iostream>
#include <BlueMatter/TableFile.hpp>
#include <string.h>
TableFile::TableFile()
{
}

TableFile::TableFile(char *fname)
{
    const char SEPCHARS[]=" ,\t\n";
    m_File = fopen(fname, "r");
    if (!m_File) {
	cerr << "Error opening " << fname << endl;
	exit(-1);
    }
    fgets(m_Buffer, BUFFSIZE, m_File);
    char *s = strtok(m_Buffer, SEPCHARS);
    if (!s) {
	cerr << "Error finding first line of text in " << fname << endl;
	exit(-1);
    }

    m_NColumns = 0;
    m_NRows = 0;
    char c = s[0];
    // if first token looks like a number, make them all numbers and give columns simple names
    if (isdigit(c) || c=='-' || c == '+') {
	m_NRows = 1;
	char buff[256];
	while (s) {
	    sprintf(buff, "Column%d", m_NColumns);
	    m_Columns.push_back(Column(buff));
	    m_Columns[m_NColumns].addValue(atof(s));
	    // m_Columns[m_NColumns].m_Data.push_back(v);
	    s = strtok(NULL, SEPCHARS);
	    m_NColumns++;
	}
    } else {
	while (s) {
	    m_Columns.push_back(Column(s));
	    s = strtok(NULL, SEPCHARS);
	    m_NColumns++;
	}
    }
    while (fgets(m_Buffer, BUFFSIZE, m_File)) {
	s = strtok(m_Buffer, SEPCHARS);
	if (!s || !(*s))
	    break;
        m_NRows++;
	int col = 0;
	while (s) {
	    m_Columns[col++].addValue(atof(s));
	    s = strtok(NULL, SEPCHARS);
	}
    }
    fclose(m_File);
    for (vector<Column>::iterator C=m_Columns.begin(); C!= m_Columns.end(); C++)
	C->doStats();
}

TableFile::TableFile(char *fname, int interleave)
{
    const char SEPCHARS[]=" ,\t\n";
    m_File = fopen(fname, "r");
    if (!m_File) {
	cerr << "Error opening " << fname << endl;
	exit(-1);
    }
    fgets(m_Buffer, BUFFSIZE, m_File);

    // STRTOK PRIME
    // char *s = strtok(m_Buffer, SEPCHARS);
    char c = m_Buffer[0];
    char *s;

    if (!c) {
	cerr << "Error finding first line of text in " << fname << endl;
	exit(-1);
    }

    m_NColumns = 0;
    m_NRows = 0;
    char buff[256];

    // if first token looks like a number, make them all numbers and give columns simple names
    if (isdigit(c) || c=='-' || c == '+') {
	m_NRows = 1;
	// STRTOK PRIME
	for (int i=0; i<interleave; i++) {
            int nrawcol = 0;
    	    s = strtok(m_Buffer, SEPCHARS);
	    // loop over tokens on each line, creating nrawcol columns and inserting first values
	    // for xyz, they would go in as 0_0 0_1 0_2 1_0 1_1 1_2 ... interleave_2
	    // one row will be added to each of these new columns
	    // technically the number of cols on each row can be different
	    while (s) {
		sprintf(buff, "%d_%d", i, nrawcol);
		m_Columns.push_back(Column(buff));
		m_Columns[m_NColumns].addValue(atof(s));
		m_NColumns++;
    		s = strtok(NULL, SEPCHARS);
		nrawcol++;
	    }
	    fgets(m_Buffer, BUFFSIZE, m_File);
	}
    // otherwise just read in the names and make interleave*n columns - no data loaded
    } else {
	vector<char *> namelist;
	// STRTOK PRIME
	s = strtok(m_Buffer, SEPCHARS);
	while (s) {
	    char *tmp = new char[256];
	    strcpy(tmp, s);
	    namelist.push_back(tmp);
    	    s = strtok(NULL, SEPCHARS);
	}
	for (int i=0; i<interleave; i++) {
	    for (int j=0; j<namelist.size(); j++) {
		sprintf(buff, "%d_%s", i, namelist[j]);
		m_Columns.push_back(Column(buff));
		m_NColumns++;
	    }
	}
        fgets(m_Buffer, BUFFSIZE, m_File);
    }

    int line=0;
    int ncol=0;
    while (strlen(m_Buffer)) {
	// STRTOK PRIME
	s = strtok(m_Buffer, SEPCHARS);
	if (!s || !(*s))
	    break;
	while (s) {
	    m_Columns[ncol].addValue(atof(s));
	    s = strtok(NULL, SEPCHARS);
	    ncol++;
	}
	line++;
	if (line%interleave == 0)
	    ncol = 0;
        if (!fgets(m_Buffer, BUFFSIZE, m_File))
	    break;
    }

    m_NRows = m_Columns[0].m_NPts;
    fclose(m_File);
    for (vector<Column>::iterator C=m_Columns.begin(); C!= m_Columns.end(); C++)
	C->doStats();
}


TableFile::~TableFile()
{
}
