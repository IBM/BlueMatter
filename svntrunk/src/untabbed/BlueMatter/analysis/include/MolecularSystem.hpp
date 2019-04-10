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
 #ifndef INCLUDE_MOLECULARSYSTEM_HPP
#define INCLUDE_MOLECULARSYSTEM_HPP

#include <vector>

#include <iostream>
#include <locale>
#include <string>

#include <BlueMatter/Topology.hpp>
#include <BlueMatter/ConnectivityString.hpp>

#ifndef WIN32
#include <BlueMatter/db2/bondlist.hpp>
#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>
#endif

class mynumpunct : public numpunct<char> {
        string do_grouping() const {return "\0"; }
};

class DatabaseProperties
{
private:
    enum {MAXDBASENAME=512};
    char m_Name[MAXDBASENAME];

public:
    void setName(char *name) { strcpy(m_Name, name); }

    DatabaseProperties() { setName("mdsetup"); }

    char *getName() { return m_Name; }
};



// This class captures properties only of the molecular system itself
class MolecularSystemProperties
{
public:
    enum {MAXPDBFILENAME=512};
    int m_NAtoms;
    int m_NWaters;
    int m_NNonWaterAtoms;
    int m_NWaterAtoms;
    string m_PDBFileName;
    // string m_ConnectivityString;
    ConnectivityString m_ConnectivityString;

    // This is initially built based on masses and connectivity
    // Then positions and velocities are assigned during conversion
    FragmentList m_FragmentList;
    double *m_Mass;
    int m_SystemID;
    char m_CommonName[MAXPDBFILENAME];
    vector<MoleculeRun> m_MoleculeRun;

    MolecularSystemProperties();

    void getMoleculeStringAndList(char *buff, vector<MoleculeRun> &list, int *hits, int nSites);

    inline void adjVisit(int *hits, const vector<vector<int> > &bl, int site, int id);

    int getSystemInfo(int sourceid=0, DatabaseProperties *db=NULL);

    void loadMasses(db2::ConnHandle &conn,int nSites);

    void loadSystemID(db2::ConnHandle &conn,int sourceid);

    inline double *getMasses() { 	
  return m_Mass; 
    }
};

#endif
