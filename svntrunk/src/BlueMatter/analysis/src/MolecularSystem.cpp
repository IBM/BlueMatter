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
 #include <BlueMatter/Topology.hpp>

#ifndef WIN32
#include <BlueMatter/db2/bondlist.hpp>
#include <db2/dbname.hpp>
#include <db2/stmthandle.hpp>
#endif

#include <BlueMatter/MolecularSystem.hpp>

MolecularSystemProperties::MolecularSystemProperties()
{
	m_NAtoms = 0;
	m_NWaters = 0;
	m_NNonWaterAtoms = 0;
	m_PDBFileName= "";
	m_Mass = NULL;
	m_SystemID = 0;
	strcpy(m_CommonName, "");
}


void MolecularSystemProperties::getMoleculeStringAndList(char *buff, vector<MoleculeRun> &list, int *hits, int nSites)
{
	char s[MAXCONNECTIVITYSTRING];
	char t[MAXCONNECTIVITYSTRING];
	s[0] = '\0';
	vector<int> mols;
	vector<int> molsize;
	vector<int> molcount;
	int prevmol = -1;
	int run = 0;
	for (int i=0; i < nSites; i++) {
	    if (hits[i] != prevmol) {
		if (prevmol != -1)
		    mols.push_back(run);
		run = 0;
		prevmol = hits[i];
	    }
	    run++;
	}
	if (run > 0)
	    mols.push_back(run);
	int firsttime = 1;
	int count;
	for (vector<int>::iterator i = mols.begin(); i != mols.end(); i++) {
	    if (firsttime) {
		firsttime = 0;
		prevmol = *i;
		count = 1;
	    } else {
		if (*i == prevmol) {
		    count++;
		} else {
		    molsize.push_back(prevmol);
		    molcount.push_back(count);
                    // Check to make sure we don't overflow s
                    sprintf(t, "%d*%d", count, prevmol);
                    int current_length = strlen(s);
                    int new_length = strlen(s) + strlen(t);
                    if (new_length >= MAXCONNECTIVITYSTRING) {
                        cerr << "Connectivity string too long, "
                             << "must increase MAXCONNECTIVITYSTRING"
                             << endl;
                        exit(-1);
                    }
		    if (strlen(s))
			sprintf(t, "%s+%d*%d", s, count, prevmol);
		    else
			sprintf(t, "%d*%d", count, prevmol);
		    strcpy(s, t);
		    count = 1;
		    prevmol = *i;
		}
	    }
	}
        int current_length = strlen(s);
        int new_length = strlen(s) + strlen(t);
        if (new_length >= MAXCONNECTIVITYSTRING) {
            cerr << "Connectivity string too long, "
                 << "must increase MAXCONNECTIVITYSTRING"
                 << endl;
            exit(-1);
        }
	if (strlen(s))
	    sprintf(t, "%s+%d*%d", s, count, prevmol);
	else
	    sprintf(t, "%d*%d", count, prevmol);
	strcpy(buff, t);
	molsize.push_back(prevmol);
	molcount.push_back(count);
	for (int i=0; i<molsize.size(); i++) {
	    MoleculeRun mr(molcount[i], molsize[i]);
	    list.push_back(mr);
	}
}

inline void MolecularSystemProperties::adjVisit(int *hits, const vector<vector<int> > &bl, int site, int id)
{
	if (hits[site] == -1) {
	    hits[site] = id;
	    for (vector<int>::const_iterator iter = bl[site].begin(); iter != bl[site].end(); iter++) {
		adjVisit(hits, bl, *iter, id);
	    }
	}
}

int MolecularSystemProperties::getSystemInfo(int sourceid, DatabaseProperties *db)
{
#ifdef WIN32
	return 1;
#else
	char buff[512];
	char localdbname[512];
	if (db)
	    strcpy(localdbname, db->getName());
	else
	    strcpy(localdbname, "mdsetup");
	if (!m_SystemID && !sourceid) {
	    cerr << "Cannot get system info without knowing SystemID or SourceID" << endl;
	    return 0;
	}
	db2::DBName::instance(localdbname);
	db2::ConnHandle& conn = db2::DBName::instance()->connection();


	loadSystemID(conn,sourceid);

        //locale loc(locale(), new mynumpunct) ;
        //std::cout.imbue(loc);

	db2::BondList bList;
	int siteId = 0;

	bList.init(m_SystemID);
	int nSites = bList.asAdjList().size();
	int *hits = new int[nSites];
	for (int i=0; i<nSites; i++)
	    hits[i] = -1;
	int mol = 0;
	for (int i=0; i<nSites; i++) {
	    if (hits[i] == -1) {
		adjVisit(hits, bList.asAdjList(), i, mol);
		mol++;
	    }
	}
        char buff_conn[MAXCONNECTIVITYSTRING];
	getMoleculeStringAndList(buff_conn, m_MoleculeRun, hits, nSites);
	m_ConnectivityString.setConnectivityString(buff_conn); // update

	loadMasses(conn,nSites);

	delete [] hits;
#endif // WIN32
	return 1;
}

void MolecularSystemProperties::loadMasses(db2::ConnHandle &conn,int nSites) {
	m_Mass = new double[nSites];

	char buff[512];
	sprintf(buff, "select atomic_mass from mdsystem.site where sys_id=%d order by site_id", m_SystemID);
	db2::StmtHandle stmt(conn);
	stmt.prepare(buff);
	SQLDOUBLE mass;
	stmt.bindCol(1, mass);
	try {
		stmt.execute();
        } catch (...) {
        	cout << "caught exception before stmt execute: " << buff << endl;
		cout.flush();
        }
	int i = 0;
	while (stmt.fetch()) {
	    m_Mass[i++] = mass;
	}
	stmt.close();
}

void MolecularSystemProperties::loadSystemID(db2::ConnHandle &conn, int sourceid) {
	if (m_SystemID) return;

	char buff[512];
	sprintf(buff, "select sys_id from experiment.system_source where source_id=%d", sourceid);

	db2::StmtHandle stmt(conn);
	SQLINTEGER id;
        try {
		stmt.prepare(buff);
	    	stmt.bindCol(1, id);
        } catch (...) {
            	cout << "caught exception before stmt execute " << endl;
	    	cout.flush();
        }
        try {
	    stmt.execute();
        } catch (...) {
            cout << "caught exception in stmt execute " << endl;
	    cout.flush();
        }
	stmt.fetch();

	m_SystemID = id;
	stmt.close();

}

