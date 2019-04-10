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
 #include <math.h>
#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <BlueMatter/BMT.hpp>
#include <BlueMatter/Imaging.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/TableFile.hpp>
#include <BlueMatter/HBond.hpp>

using namespace std ;

class Cluster
{
public:
    set<int> m_Members;
    void insert(int k)
    {
	m_Members.insert(k);
    }

    void clear()
    {
	m_Members.clear();
    }
};

class Adjacency
{
public:
    set<int> m_Neighbors;
};

class ClusterList
{
public:
    Cluster *m_Clusters;
    vector<int> m_Don;
    vector<int> m_Acc;
    int *m_ClusterAssignment;
    Adjacency *m_AdjacencyList;
    int m_NItems;
    int m_NClusters;

    ClusterList(int NItems)
    {
	m_ClusterAssignment = new int[NItems];
	m_AdjacencyList = new Adjacency[NItems];
	m_NItems = NItems;
	m_Clusters = new Cluster[NItems];
	m_NClusters = 0;
    }

#if 0
    // find existing cluster with element k in it (at most one)
    // If none, return new cluster
    set<int> &find(int k) {
	return m_Clusters[k];
    }

    // check if k is already in a cluster
    // if so, set current cluster to that one
    // otherwise make new one.
    void startNewCluster(int k)
    {
    }

    // clust is the value to set, k is the
    void addToCluster(int clust, int k)
    {
	if (!clust) {
	    Cluster *c = new Cluster;
	    c->insert(k);
	    m_Clusters.push_back(*c);
	    clust = c;
	} else if (clust->find(k) == clust->end()) {
	    clust->insert(k);
	}
	return;
    }
#endif

    // add pair to pending clusterlist
    void addPair(int j, int k)
    {
	m_Don.push_back(j);
	m_Acc.push_back(k);
    }

    // nstep nclust meansize minsize maxsize nsingles nclust0 meansize0 minsize0 maxsize0 nclust1 meansize1 minsize1 maxsize1
    void dump(float tstep)
    {
	int nclust[3], meansize[3], minsize[3], maxsize[3], nsingles[3];
	int ntot = 0;
	for (int i=0; i<3; i++) {
	    nclust[i] = meansize[i] = maxsize[i] = nsingles[i] = 0;
	    minsize[i] = 100000000;
	}
	for (int i=0; i<m_NClusters; i++) {
	    int csize = m_Clusters[i].m_Members.size();
	    int first = *m_Clusters[i].m_Members.begin();
	    // identify which leaf by first element
	    int leaf = (first < m_NItems/2) ? 0 : 1;
	    meansize[0] += csize;
	    meansize[leaf+1] += csize;
	    nclust[0]++;
	    nclust[leaf+1]++;
	    if (csize==1) {
		nsingles[0]++;
		nsingles[leaf+1]++;
	    }
	    if (csize < minsize[0]) {
		minsize[0] = csize;
	    }
	    if (csize < minsize[leaf+1]) {
		minsize[leaf+1] = csize;
	    }
	    if (csize > maxsize[0]) {
		maxsize[0] = csize;
	    }
	    if (csize > maxsize[leaf+1]) {
		maxsize[leaf+1] = csize;
	    }
	}
	cout << tstep << " ";
	for (int i=0; i<3; i++) {
	    cout << nclust[i] << " " << meansize[i]/float(nclust[i]) << " " << minsize[i] << " " << maxsize[i] << " " << nsingles[i] << " ";
	}
	cout << endl;
#if 0
	cout << nstep << " " << csize << " " << ntot << " ";
	set<int> &s = m_Clusters[i].m_Members;
	for (set<int>::iterator j = s.begin(); j != s.end(); j++) {
	    cout << *j << " ";
	}
	cout << endl;
#endif
    }

    void dumpClusterSizes(float t)
    {
        cout << t << " ";
	for (int i=0; i<m_NItems; i++)
	    cout << m_Clusters[m_ClusterAssignment[i]].m_Members.size() << " ";
	cout << endl;
    }

    void clear()
    {
	for (int i=0; i<m_NClusters; i++)
	    m_Clusters[i].clear();
	m_NClusters = 0;
	m_Don.clear();
	m_Acc.clear();
	for (int i=0; i<m_NItems; i++) {
	    m_ClusterAssignment[i] = -1;
	    m_AdjacencyList[i].m_Neighbors.clear();
	}
    }


    // clust is the tentative assignment
    // k is the row assignment in the pair list
    // clusterAssignment is indexed by lipid
    void addCluster(int clust, int k)
    {
	set<int> &s = m_Clusters[clust].m_Members;
	if (s.find(k) == s.end())
	    s.insert(k);
    }


    // run through don/acc lists building full adj. table
    void buildAdjacency()
    {
	int npairs = m_Don.size();
	for (int i=0; i<npairs; i++) {
	    int don = m_Don[i];
	    int acc = m_Acc[i];
	    set<int> &dons = m_AdjacencyList[don].m_Neighbors;
	    set<int> &accs = m_AdjacencyList[acc].m_Neighbors;
	    if (dons.find(acc) == dons.end())
		dons.insert(acc);
	    if (accs.find(don) == accs.end())
		accs.insert(don);
	}
    }

    // return 1 if the new cluster id was used
    inline int adjVisit(int site, int id)
    {
	// cerr << "visiting " << site << " " << id << endl;
	if (m_ClusterAssignment[site] == -1) {
	    // cerr << "setting " << site << endl;
	    m_ClusterAssignment[site] = id;
	    set<int> &s = m_AdjacencyList[site].m_Neighbors;
	    for (set<int>::const_iterator iter = s.begin(); iter != s.end(); iter++) {
		adjVisit(*iter, id);
	    }
	    return 1;
	} else {
	    return 0;
	}
    }

    // involves recursive search
    // start with first don/acc and follow, building clusters.
    // 
    void findClusters()
    {
	buildAdjacency();

	int npairs = m_Don.size();
	// int m_NClusters = 0;
	for (int i=0; i<m_NItems; i++) {
	    if (adjVisit(i, m_NClusters))
		m_NClusters++;
	}
	for (int i=0; i<m_NItems; i++)
	    addCluster(m_ClusterAssignment[i], i);
    }
};

int main(int argc, char **argv)
{

    int nlipperleaf = 36;
    int nlips = nlipperleaf*2;
    ClusterList clusters(nlips);

    // step leaf don acc nx ny nz nbonds
    TableFile tf("/fs/lipid7/wetsope_14_bond.txt");

    Column &step = tf.m_Columns[0];
    Column &leaf = tf.m_Columns[1];
    Column &don  = tf.m_Columns[2];
    Column &acc  = tf.m_Columns[3];

    Column csize[72];
    Column time;

    int i = 0;
    int NRows = step.m_Data.size();
    float tstep = step.m_Data[0];
    while (i < NRows) {
	clusters.clear();
	// build set of pairs for this timestep
	while (fabs(step.m_Data[i] - tstep) < 0.0001) {
	    clusters.addPair(don.m_Data[i]+nlipperleaf*leaf.m_Data[i], acc.m_Data[i]+nlipperleaf*leaf.m_Data[i]);
	    i++;
	}
	// run through pairs and assign clusters into Assignment array
	clusters.findClusters();
	// clusters.dump(tstep);
	// clusters.dumpClusterSizes(tstep);
	// set to next timestep

	time.addValue(tstep);
	for (int c=0; c<72; c++) {
	    int s = clusters.m_Clusters[clusters.m_ClusterAssignment[c]].m_Members.size();
	    csize[c].addValue(s);
	}

	tstep = step.m_Data[i];
    }
    double *corr[72];
    int nstep = 1;
    for (int i=0; i<72; i++)
	csize[i].autoCorrelation(corr[i], false, true);

    Column stats;
    for (int i=0; i<time.m_NPts; i+=nstep) {
	cout << time.m_Data[i] << " ";
	stats.flush();

	for (int j=0; j<72; j++) {
	    stats.addValue(corr[j][i]);
	}

	stats.doStats();
	cout << stats.m_Mean << " " << stats.m_Sigma << " ";

	for (int j=0; j<72; j++) {
	    cout << corr[j][i] << " ";
	}

	cout << endl;
    }

    return 0;
}
