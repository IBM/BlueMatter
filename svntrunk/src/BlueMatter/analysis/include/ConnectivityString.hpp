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
 #ifndef INCLUDE_CONNECTIVITYSTRING_HPP
#define INCLUDE_CONNECTIVITYSTRING_HPP

#include <iostream>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <BlueMatter/Topology.hpp> // for MoleculeRun

#define MAXCONNECTIVITYSTRING 4096

// Sample connectivity string: 1*256+3*1+1660*3

class ConnectivityString {
private: 
	vector<MoleculeRun> m_mr;

protected:
	
	void parseMolEntry(char *s);

	void parseRemaining(char *s);
	
public:
	char m_ConnStr[MAXCONNECTIVITYSTRING];

	ConnectivityString() {
		strcpy(m_ConnStr,"");
		m_mr.clear();
	}

	ConnectivityString(char *s) {
		strcpy(m_ConnStr,s);
		parseConnectivityString();
	}

	inline char *getConnectivityString() {
		return m_ConnStr;
	}

	void setConnectivityString(char *s) {
		strcpy(m_ConnStr,s);
		parseConnectivityString();
	}

	// Sample connectivity string: 1*256+3*1+1660*3
	vector<MoleculeRun> parseConnectivityString() {
		m_mr.clear();
		parseRemaining(m_ConnStr);
		// printMoleculeRun();
		return m_mr;
	}

        inline vector<MoleculeRun> *getMRHandle() {
		return &m_mr;
	}

	int getNumberOfWaters() ;

	 inline int getNumberOfWaterAtoms() {
		return 3*getNumberOfWaters();
	}

	 int getNumberOfMolecules();

	 int getNumberOfAtoms();

	 inline int getNumberOfNonWaters() {
		// printMoleculeRun();
		return (getNumberOfMolecules() - getNumberOfWaters());
	}

	 inline int getNumberOfNonWaterAtoms() {
		return (getNumberOfAtoms() - getNumberOfWaterAtoms());
	}

	char *makeConnectivityString(vector<MoleculeRun> mr) {
		// fill this in
		return NULL;
	}	

	void printMoleculeRun();

	ConnectivityString& operator=(const ConnectivityString& aOther) {
		strcpy(m_ConnStr,aOther.m_ConnStr);	
		parseConnectivityString();
		return *this;
	}

};




ostream& operator<<(ostream& os, const ConnectivityString &cs) {
        os << cs.m_ConnStr;
        return os;
}




#endif
