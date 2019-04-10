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
 #ifndef TOPOLOGY_HPP
#define TOPOLOGY_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/LSQ.hpp>
// #include <BlueMatter/db2/molecularsystem.hpp>
// #include <db2/stmthandle.hpp>
// #include <BlueMatter/db2/bondlist.hpp>
// #include <BlueMatter/db2/dbname.hpp>

#include <BlueMatter/mrc.hpp>
#include <BlueMatter/Imaging.hpp>


using namespace std ;

/* 
 * Masses, Charges, and even AtomicNumbers are essentially the same class --
 * should probably make a single class then subclass them
 */
template <class T>
class tAtomValues
{
/*
private:

*/
public:
    T *m_Values;
    int m_NValues;
    
    tAtomValues(char *fname)
    {
    ifstream inp(fname);
    if (!inp.good()) {
	    cerr << "Error opening " << fname << endl;
	    exit(-1);
	}
	T m;
	vector<T> mv;
    while (!inp.eof()) {
        inp >> m;
        mv.push_back(m);
    }
    inp.close();
	m_NValues = mv.size();
	m_Values = new T[m_NValues];
	for (int i=0; i<m_NValues; i++)
        {
	    m_Values[i] = mv[i];
        }
	mv.clear();
    }


    T GetValue(const int index)
    {
	assert( (index >= 0) && (index < m_NValues) );
	return m_Values[index];
    }

    T *GetValues()
    {
	return m_Values;
    }
};

typedef tAtomValues<int>    iAtomValues;
typedef tAtomValues<float>  fAtomValues;
typedef tAtomValues<double>  AtomValues;


// This is a run of molecules of the same size, like 3*1 or 1660*3

// The class MoleculeRun has been moved into mrc.hpp inorder to
// suit scope requirements with the other includes
/*
class MoleculeRun
{
public:
    int m_Count;  // number of molecules
    int m_Size;   // atoms per molecule

    MoleculeRun() {
        m_Count = 0;
        m_Size = 0;
    }

    MoleculeRun(int a, int b) {
	m_Count = a;
	m_Size = b;
    }
};
*/


// fragment is start, atomcount, center
template <class T>
class tFragment
{
public:
    int m_Start;
    int m_Count;
    tXYZ<T> m_Center;
    tXYZ<T> m_CenterOfMass;
    T m_Mass;
    T m_Charge;
    tXYZ<T> *m_Positions;
    tXYZ<T> m_PrincipalAxes[3];
    T *m_Masses;
    T *m_Charges;
    int *m_AtomicNumbers;
    tXYZ<T> m_StartingPosition;
    tXYZ<T> m_StartingTranslationVector;
    tXYZ<T> m_StartingTranslatedPosition;
    tXYZ<T> m_PreviousPosition;
    tXYZ<T> m_LocalPosition;
    bool m_LocalPositionSet;
    bool m_PreviousPositionSet;
    char m_Name[128];

    tFragment(int start, int count, tXYZ<T> *p=NULL) : m_Start(start), m_Count(count), m_Positions(p),
	m_LocalPositionSet(false), m_PreviousPositionSet(false)
	{
	    strcpy(m_Name, "");
	    // start += count;
	    // cerr << "frag1 emptyname " << start << " " << count << " " << endl;
	}

    tFragment(char *name, int start, int count) : m_Start(start), m_Count(count)
	{
	    // cerr << "frag2 " << name << " " << start << " " << count << endl;
	    // start += count;
	    strcpy(m_Name, name);
	    m_Positions = NULL;
	}

    void AssignPositions(tXYZ<T> *p)
    {
	m_Positions = p;
    }

    tFragment(const tFragment &f)
    {
	m_Start = f.m_Start;
	m_Count = f.m_Count;
	m_Center = f.m_Center;
	m_CenterOfMass = f.m_CenterOfMass;
	m_Mass = f.m_Mass;
	m_Charge = f.m_Charge;
	m_Positions = f.m_Positions;
	for (int i=0; i<3; i++)
	    m_PrincipalAxes[i] = f.m_PrincipalAxes[i];
	m_Masses = f.m_Masses;
	m_Charges = f.m_Charges;
	m_AtomicNumbers = f.m_AtomicNumbers;
	m_StartingPosition = f.m_StartingPosition;
	m_StartingTranslationVector = f.m_StartingTranslationVector;
	m_StartingTranslatedPosition = f.m_StartingTranslatedPosition;
	m_PreviousPosition = f.m_PreviousPosition;
	m_LocalPosition = f.m_LocalPosition;
	m_LocalPositionSet = f.m_LocalPositionSet;
	strncpy(m_Name, f.m_Name, 128);
    }

    tFragment &operator=(const tFragment &f)
    {
	m_Start = f.m_Start;
	m_Count = f.m_Count;
	m_Center = f.m_Center;
	m_CenterOfMass = f.m_CenterOfMass;
	m_Mass = f.m_Mass;
	m_Charge = f.m_Charge;
	m_Positions = f.m_Positions;
	for (int i=0; i<3; i++)
	    m_PrincipalAxes[i] = f.m_PrincipalAxes[i];
	m_Masses = f.m_Masses;
	m_Charges = f.m_Charges;
	m_AtomicNumbers = f.m_AtomicNumbers;
	m_StartingPosition = f.m_StartingPosition;
	m_StartingTranslationVector = f.m_StartingTranslationVector;
	m_StartingTranslatedPosition = f.m_StartingTranslatedPosition;
	m_PreviousPosition = f.m_PreviousPosition;
	m_LocalPosition = f.m_LocalPosition;
	m_LocalPositionSet = f.m_LocalPositionSet;
	strncpy(m_Name, f.m_Name, 128);
	return *this;
    }

    void Translate(const tXYZ<T> &v)
    {
        tXYZ<T> *p = &m_Positions[m_Start];
        for (int i = 0; i < m_Count; i++)
        {
            //*p = *p + v;
            p[i] = p[i] + v;
        }
    }

    void SetStartingPosition()
    {
	m_StartingPosition = m_CenterOfMass;
    }

    void SetPreviousPosition()
    {
	m_PreviousPosition = m_CenterOfMass;
	m_PreviousPositionSet = true;
    }

    void ImageToNearPrevious(tXYZ<T> &box)
    {
	tXYZ<T> nearest = tImaging<T>::NearestImagePosition(m_PreviousPosition, m_CenterOfMass, box);
	m_CenterOfMass = nearest;
    }

    T MinDistance( const tXYZ<T> &point, const tXYZ<T> &box)
    // Compute the minimum atom-wise distance between point and the fragment
    {
	tXYZ<T> *v = &m_Positions[m_Start];
	T min = 1.0e40;
	for (int i = 0; i < m_Count; i++,v++)
	{
	    tXYZ<T> sep = tImaging<T>::NearestImageDisplacement(*v, point, box);
	    T d = sep.Length(); 
	    if (d < min)
            {
		min = d;
            }
        }
    return min;
    }

    tXYZ<T> GetLocalMotion(tXYZ<T> &p)
    {
	if (!m_LocalPositionSet) {
	    m_LocalPosition = p;
	    m_LocalPositionSet = true;
	}
	return p - m_LocalPosition;
    }

    // need to find raw com
    // then image that into box
    void SetStartingPositionAndTranslation(tXYZ<T> &box)
    {
	SetStartingPosition();
	tXYZ<T> v;
	tImaging<T>::TranslationVector(m_StartingPosition, box, v);
	m_StartingTranslationVector = v;
	m_StartingTranslatedPosition = m_StartingPosition - v;
    }

    // map into central cell by center of mass
    void MapIntoBox(const tXYZ<T> &box)
    {
	tXYZ<T> origin = tXYZ<T>::ZERO_VALUE();
        tXYZ<T> *p = &m_Positions[m_Start];
        FindCenterOfMass();
        tXYZ<T> vec=tImaging<T>::NearestImageDisplacement(origin, 
                                                          m_CenterOfMass, 
                                                          box);
        tXYZ<T> disp = m_CenterOfMass - vec;
        //cout << "COM: " << m_CenterOfMass << "  Disp: " << disp << endl;
        m_CenterOfMass = m_CenterOfMass - disp;
        m_Center = m_Center - disp;
        for (int i=0; i < m_Count; i++)
        {
            //cout <<    "before: " << i << "  " << p[i] << endl;
            p[i] = p[i] - disp;
            //cout <<    "after:  " << i << "  " << p[i] << endl;
        }
    }

    void MapIntoBoxByAtom(const tXYZ<T> &box)
    {
        tXYZ<T> origin = tXYZ<T>::ZERO_VALUE();
        tXYZ<T> *p = &m_Positions[m_Start];
        for (int i=0; i < m_Count; i++)
        {
            p[i] = tImaging<T>::NearestImageDisplacement(origin,p[i],box);
        }
    }

    void AssignAtomicNumbers(iAtomValues &an)
    {
	m_AtomicNumbers = an.GetValues();
    }

    void AssignMasses(tAtomValues<T> &m)
    {
	m_Masses = m.GetValues();
	m_Mass = 0;
	for (int i=m_Start; i < m_Start+m_Count; i++)
	    m_Mass += m_Masses[i];
    }

    void AssignCharges(tAtomValues<T> &c)
    {
	m_Charges = c.GetValues();
	m_Charge = 0;
	for (int i=m_Start; i < m_Start+m_Count; i++)
	    m_Charge += m_Charges[i];
    }

    tXYZ<T> FindGeometricCenter(tXYZ<T> *p)
    {
//	tXYZ<T> sum;
	tXYZ<T> sum = tXYZ<T>::ZERO_VALUE();
	tXYZ<T> *v = &p[m_Start];
	for (int i=0; i<m_Count; i++, v++)
	    sum += *v;
	m_Center = sum/m_Count;
	return m_Center;
    }

    tXYZ<T> &FindGeometricCenter()
    {
	tXYZ<T> sum = tXYZ<T>::ZERO_VALUE();
	tXYZ<T> *v = &m_Positions[m_Start];
	for (int i=0; i<m_Count; i++, v++)
	    sum += *v;
	m_Center = sum/m_Count;
	return m_Center;
    }

    tXYZ<T> &FindCenterOfMass()
    {
	tXYZ<T> sum = tXYZ<T>::ZERO_VALUE();
	tXYZ<T> *v = &m_Positions[m_Start];
	T *m = &m_Masses[m_Start];
	for (int i=0; i<m_Count; i++, v++, m++)
	    sum += (*v)*(*m);
	m_CenterOfMass = sum/m_Mass;
	m_Center = m_CenterOfMass;
	return m_CenterOfMass;
    }

    // AK: Changed the floats to doubles in function arguments

    // This should be generalized to a AssignPropertyDensity, with a 
    // weight vector being passed -- then you could do charge density,
    // electron density, mass density, number density 
    void AssignElectronDensity(double *h, const int numbins, 
                               const double hist_min, const double hist_max, 
                               const double bin_width)
    {
	/*
	 Assign the electron density for the fragment into a histogram.
	 Imaging is done on a per-atom basis, with the assumption that the 
	 fragment as a whole is already correctly imaged and the box is 
	 centered on the origin.
	 */
	double range = hist_max - hist_min;
	for (int i = 0; i<m_Count; i++)
	{
	    T z = m_Positions[m_Start + i][2];
	    if (z < hist_min)
	    {
		z+=range;
	    }
	    else if (z > hist_max)
	    {
		z-=range;
	    }
	    double electrons = -m_Charges[m_Start + i] + 
				(double)m_AtomicNumbers[m_Start + i];
        //cout << i << "\t" << -m_Charges[m_Start + i]
        //          << "\t" << m_AtomicNumbers[m_Start + i]
        //          << "\t" << electrons
        //          << endl;
	    int bin = (int) ((z-hist_min) / bin_width);
	    assert( (bin >=0) && (bin < numbins));
	    h[bin] += electrons; 
	}
    }

    void FindPrincipalAxes()
    {
	FindGeometricCenter();
	double e[3];

	// Matrix m, v;
	double **m = new double*[3];
	m[0] = new double[3];
	m[1] = new double[3];
	m[2] = new double[3];

	double **v = new double*[3];
	v[0] = new double[3];
	v[1] = new double[3];
	v[2] = new double[3];

	for (int i=0; i<3; i++) {
	    for (int j=0; j<3; j++) {
		m[i][j] = 0;
		for (int k=m_Start; k<m_Start+m_Count; k++)
		    m[i][j] += (double(m_Positions[k][i])-double(m_Center[i]))*(double(m_Positions[k][j])-double(m_Center[j]));
	    }
	}
	Eigen(m, e, v, 3);
	// the order from Eigen appears have eigenvalues in order of size, but eigenvectors in opposite order
	for (int p=0; p<3; p++) {
	    m_PrincipalAxes[2-p].mX = v[p][0];
	    m_PrincipalAxes[2-p].mY = v[p][1];
	    m_PrincipalAxes[2-p].mZ = v[p][2];
	}
	delete [] m;
	delete [] v;
    }

};

typedef tFragment<float>  fFragment;
typedef tFragment<double>  Fragment;

// Fragment list has a name, but individual frags also have names
// Frags could all be same, or different.
template <class T>
class tFragmentList
{
public:
    vector<tFragment<T> > m_Fragments;
    tXYZ<T> *m_Positions;
    char m_Name[128];
    T m_Mass;
    T m_Charge;
    tXYZ<T> m_CenterOfMass;
    tXYZ<T> m_StartingCenterOfMass;
    tXYZ<T> m_Center;

    tFragmentList(char *name) {
	// cerr << "fraglist1 " << name << endl;
	strcpy(m_Name, name);
    }
    tFragmentList(char *name, int start, int count) {
	// cerr << "fraglist2 " << name << " " << start << " " << count << endl;
	strcpy(m_Name, name);
	tFragment<T> f(name, start, count);
	m_Fragments.push_back(f);
    }

    tFragmentList() {
	strcpy(m_Name, "");
    }

    tFragmentList(const tFragmentList &f)
    {
	m_Fragments = f.m_Fragments;
	m_Positions = f.m_Positions;
	strncpy(m_Name, f.m_Name, 128);
	m_Mass = f.m_Mass;
	m_Charge = f.m_Charge;
	m_CenterOfMass = f.m_CenterOfMass;
	m_StartingCenterOfMass = f.m_StartingCenterOfMass;
    }

    tFragmentList &operator=(const tFragmentList &f)
    {
	m_Fragments = f.m_Fragments;
	m_Positions = f.m_Positions;
	strncpy(m_Name, f.m_Name, 128);
	m_Mass = f.m_Mass;
	m_Charge = f.m_Charge;
	m_CenterOfMass = f.m_CenterOfMass;
	m_StartingCenterOfMass = f.m_StartingCenterOfMass;
	return *this;
    }

    void AddFragment(tFragment<T> &f, int &currentindex) {
	// cerr << "AddFrag " << f.m_Name << " " <<  f.m_Start << " " << f.m_Count << endl;
	f.m_Start = currentindex;
	m_Fragments.push_back(f);
	currentindex += f.m_Count;
	// cerr << m_Name << " frag list now has " << m_Fragments.size() << " fragments" << endl;
    }
    void AddFragment(tFragment<T> &f) {
	m_Fragments.push_back(f);
	// cerr << m_Name << " frag list now has " << m_Fragments.size() << " fragments" << endl;
    }
    void FindGeometricCenters(tXYZ<T> *p) {
	for (typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i.FindGeometricCenter(p);
	}
    }
    void FindGeometricCenters() {
	tXYZ<T> cog = tXYZ<T>::ZERO_VALUE();
	int count = 0;
	for (typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->FindGeometricCenter();
	    count += i->m_Count;
	    cog = cog + double(i->m_Count)*(i->m_Center);
	}
	m_Center = cog/count;
    }

    void Translate(const tXYZ<T> &v)
    {
    //m_CenterOfMass += v;
        for (typename vector<tFragment<T> >::iterator f  = m_Fragments.begin();
                                             f != m_Fragments.end(); f++)
        {
            f->Translate(v);
        }
    }

    void clear() {
	m_Fragments.clear();
	m_Mass = 0;
    }

    int GetFirstFragmentCount() {
	if (m_Fragments.size() == 0)
	    return 0;
	return m_Fragments[0].m_Count;
    }

    int GetTotalAtomCount() {
	int n = 0;
	// cerr << "In GetTotalAtCount, nfrags is " << m_Name << " " << m_Fragments.size() << endl;
	for (typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    // cerr << "Frag count " << n << " " << i->m_Count << endl;
	    n += i->m_Count;
	}
	return n;
    }

    void SetPreviousPositions() {
	for (typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->SetPreviousPosition();
	}
    }

    void ImageToNearPrevious(tXYZ<T> &box) {
	for (typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->ImageToNearPrevious(box);
	}
    }

    // translate will find the total com based on individual com's relative to
    // starting central box location
    void FindCenterOfMasses(bool translate = false) {
	m_Mass = 0;
	m_CenterOfMass = tXYZ<T>::ZERO_VALUE();
	for (typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->FindCenterOfMass();
	    if (translate)
		m_CenterOfMass += i->m_Mass*(i->m_CenterOfMass - i->m_StartingTranslationVector);
	    else
		m_CenterOfMass += i->m_Mass*i->m_CenterOfMass;
	    m_Mass += i->m_Mass;
	}
	m_CenterOfMass = m_CenterOfMass/m_Mass;
    }

    tXYZ<T> FindAggregateCenterOfMass() {
	T mass = 0;
	tXYZ<T> com = tXYZ<T>::ZERO_VALUE();
	for (typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    com += i->m_Mass * i->m_CenterOfMass;
	    mass += i->m_Mass;
	}
	com = com/mass;
	return com;
    }

    T MinDistance(const tXYZ<T> &point, const tXYZ<T> &box)
    {
	T min = 1.0e40;
	for (typename vector<tFragment<T> >::iterator i  = m_Fragments.begin();
                                         i != m_Fragments.end(); i++)
        {
	    T d = i->MinDistance(point, box);
	    if (d < min)
            {
		min = d;
            }
        }
    return min;
    }

    void MapIntoBox(const tXYZ<T> &box)
    {
    for (typename vector<tFragment<T> >::iterator f = m_Fragments.begin();
                                         f != m_Fragments.end(); f++)
        {
	    f->MapIntoBox(box);
        }
    }

    void MapIntoBoxByAtom(const tXYZ<T> &box)
    {
    for (typename vector<tFragment<T> >::iterator f = m_Fragments.begin();
                                         f != m_Fragments.end(); f++)
        {
	    f->MapIntoBoxByAtom(box);
        }
    }

    // find center of mass and determine translation vector for each fragment.
    // final com is based on all frags in central box
    void FindStartingCenterOfMasses(tXYZ<T> &box) {
	m_CenterOfMass = tXYZ<T>::ZERO_VALUE();
	m_Mass = 0;
	for (typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->FindCenterOfMass();  // raw com
	    i->SetStartingPositionAndTranslation(box);
	    m_CenterOfMass += i->m_Mass*(i->m_CenterOfMass - i->m_StartingTranslationVector);
	    m_Mass += i->m_Mass;
	    // cout << "findscom " << m_CenterOfMass << endl;
	}
	// cout << "mmass is " << m_Mass << endl;
	m_CenterOfMass = m_CenterOfMass/m_Mass;
	m_StartingCenterOfMass = m_CenterOfMass;
    }
    void FindPrincipalAxes() {
	for(typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->FindPrincipalAxes();
	}
    }
    void AssignPositions(tXYZ<T> *p) {
        m_Positions = p;
	for(typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->AssignPositions(p);
	}
    }
    void SetStartingPositions() {
	for(typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->SetStartingPosition();
	}
    }
    void SetStartingPositionsAndTranslation(tXYZ<T> &box) {
	for(typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->SetStartingPositionAndTranslation(box);
	}
    }
    void AssignMasses(tAtomValues<T> &m) {
	m_Mass = 0;
	for(typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->AssignMasses(m);
	    m_Mass += i->m_Mass;
	}
    }
    void AssignCharges(tAtomValues<T> &c) {
	m_Charge = 0;
	for(typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->AssignCharges(c);
	    m_Charge += i->m_Charge;
	}
    }
    void AssignAtomicNumbers(iAtomValues &an) {
	for(typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->AssignAtomicNumbers(an);
	}
    }

    void SubtractCenterOfMassFromStart() {
	for(typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->m_StartingPosition -= m_CenterOfMass;
	}
    }

    void SubtractFromStart(tXYZ<T> &d) {
	for(typename vector<tFragment<T> >::iterator i=m_Fragments.begin(); i != m_Fragments.end(); i++) {
	    i->m_StartingPosition -= d;
	}
    }

    void SplitUpperLower(tFragmentList<T> &lower, tFragmentList<T> &upper)
    {
	float zsum=0;
	for (int i=0; i<m_Fragments.size(); i++)
	    zsum += m_Fragments[i].m_CenterOfMass.mZ;
	zsum /= m_Fragments.size();
	for (int i=0; i<m_Fragments.size(); i++) {
	    if (m_Fragments[i].m_CenterOfMass.mZ < zsum)
		lower.m_Fragments.push_back(m_Fragments[i]);
	    else
		upper.m_Fragments.push_back(m_Fragments[i]);
	}
    }

    void Print()
    {
        cout << m_Name << endl;
        for (int j=0; j<m_Fragments.size(); j++) {
            // ADD ONE SO WORLD SEES PDB 1-BASED ATOMS
            cout << m_Fragments[j].m_Name << " " << m_Fragments[j].m_Start+1 << " " << m_Fragments[j].m_Count << " " << (void *)m_Fragments[j].m_Positions << endl;
        }
        cout << endl;
    }

};

typedef tFragmentList<float> fFragmentList;
typedef tFragmentList<double> FragmentList;


#if 0
STEA 54
PCGL 34
DHA  54
REPEAT 72
CHOL 74
REPEAT 24
OHH 3
REPEAT 1087
// C13C18 CHOL 25 1

FRAGMENT is start, count
SUBFRAGMENT is FRAGMENT, start, count
SUBFRAGMENTLIST is FRAGMENT, a0, a1, a2...
allow exclusion, union, intersection
FRAGMENTBOND is FRAGMENT, a, b
BOND is a, b

#endif


template <class T>
class tFragmentListTable
{
public:
    vector<tFragmentList<T> > m_FragmentLists;
    tXYZ<T> *m_Positions;
    T m_Mass;
    tXYZ<T> m_CenterOfMass;

    tFragmentListTable(char *fname)
    {
	Load(fname);
    }

    tFragmentListTable()
    {
    }

    // This is the total number of atoms stored in a list
    // This may be different from the current pointer into the main if frags aren't added contiguously
    int getTotalAtomCount()
    {
	int count = 0;
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    // cerr << "FragList " << fl->m_Name << " has " << fl->GetTotalAtomCount() << endl;
	    count += fl->GetTotalAtomCount();
	}
	// cerr << "TotAtCount " << count << endl;
	return count;
    }

    void addFragmentList(tFragmentList<T> &fl)
    {
	m_FragmentLists.push_back(fl);
    }

    void deleteFragmentList(char *name)
    {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    if (!strcmp(fl->m_Name, name)) {
		m_FragmentLists.erase(fl);
		return;
	    }
	}
	cerr << "No list named " << name << " found." << endl;
	exit(-1);
    }

    void AssignPositions(tXYZ<T> *p)
    {
        m_Positions = p;
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    fl->AssignPositions(p);
	}
    }

    void Translate(const tXYZ<T> &v)
    {
        //m_CenterOfMass = m_CenterOfMass + v;
        for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
                                                fl != m_FragmentLists.end();
                                                fl++)
        {
            fl->Translate(v);
        }
    }

    void AssignMasses(tAtomValues<T> &m)
    {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    fl->AssignMasses(m);
	}
    }

    void AssignCharges(tAtomValues<T> &c)
    {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    fl->AssignCharges(c);
	}
    }

    void AssignAtomicNumbers(iAtomValues &an)
    {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    fl->AssignAtomicNumbers(an);
	}
    }

    void FindPrincipalAxes()
    {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    fl->FindPrincipalAxes();
	}
    }

    void SubtractCenterOfMassFromStart() {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    fl->SubtractCenterOfMassFromStart();
	}
    }

    void SubtractFromStart(tXYZ<T> &d) {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    fl->SubtractFromStart(d);
	}
    }

    void FindGeometricCenters()
    {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    fl->FindGeometricCenters();
	}
    }

    void SetPreviousPositions()
    {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    fl->SetPreviousPositions();
	}
    }

    void ImageToNearPrevious(tXYZ<T> &box)
    {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    fl->ImageToNearPrevious(box);
	}
    }

    void MapIntoBox(const tXYZ<T> &box)
    {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	                                         fl != m_FragmentLists.end();
	                                         fl++)
        {
        fl->MapIntoBox(box);
        }
    }

    void MapIntoBoxByAtom(const tXYZ<T> &box)
    {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	                                         fl != m_FragmentLists.end();
	                                         fl++)
        {
	    fl->MapIntoBoxByAtom(box);
        }

    }

    void FindCenterOfMasses()
    {
	m_CenterOfMass = XYZ::ZERO_VALUE();
	m_Mass = 0;
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    fl->FindCenterOfMasses();
	    m_Mass += fl->m_Mass;
	    m_CenterOfMass += fl->m_Mass*fl->m_CenterOfMass;
	}
	m_CenterOfMass = m_CenterOfMass/m_Mass;
    }

    tXYZ<T> FindAggregateCenterOfMass()
    {
	tXYZ<T> com = XYZ::ZERO_VALUE();
	T mass = 0;
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    mass += fl->m_Mass;
	    com += fl->m_Mass*fl->FindAggregateCenterOfMass();
	}
	com = com/mass;
	return com;
    }

    // skip over SKIP
    // accomodate empty list
    // recognize repeated names
    void BuildRepeats(tFragmentList<T> &rfl, int &index, int count)
    {
	// cerr << "BuildRepeats " << index << " " << count << endl;
	for (int i=0; i<count; i++) {
	    for(typename vector<tFragment<T> >::iterator f = rfl.m_Fragments.begin(); f != rfl.m_Fragments.end(); f++) {
		if (strcmp(f->m_Name, "SKIP")) {
		    tFragmentList<T> &destfl = GetFragmentListWithCreation(f->m_Name);
		    tFragment<T> frg(f->m_Name, index, f->m_Count);
		    // cerr << "Just made frag " << f->m_Name << " " << index << " " << f->m_Count << endl;
		    destfl.AddFragment(frg, index);
		} else {
		    // cerr << "BuildRepeats skipping a SKIP at index, count " << index << " " << f->m_Count << endl;
		    // cerr << "  about to bump index by count" << endl;
		    index += f->m_Count;
		}
		// index += f->m_Count;
	    }
	}
    }

    void BuildRepeats(int startlist, int &index, int repeatcount, int length)
    {
	// cerr << "in other BuildRepeats that doesn't check SKIP" << endl;
	int nremaining = repeatcount-1;
	for (int i=startlist; i<m_FragmentLists.size(); i++) {
	    tFragmentList<T> &fl = m_FragmentLists[i];
	    int startindex = fl.m_Fragments[0].m_Start + length;
	    int atomcount = fl.m_Fragments[0].m_Count;
	    for (int j=0; j<nremaining; j++) {
		tFragment<T> frg = tFragment<T>(startindex, atomcount);
		// cerr << "BuildRepeats adding " << startindex << " " << atomcount << endl;
		fl.AddFragment(frg, startindex);
		// startindex += length;
	    }
	}
	// index += length*nremaining;
    }

    bool HasFragment(char *name)
    {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    if (!strcmp(fl->m_Name, name)) {
		return true;
	    }
	}
	return false;
    }

    tFragmentList<T> &GetFragmentList(char *name)
    {
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    if (!strcmp(fl->m_Name, name))
		return *fl;
	}
	cerr << "No list named " << name << " found." << endl;
	exit(-1);

	// keep compiler quiet;
	tFragmentList<T> *fdummy = new tFragmentList<T>("foo");
	return *fdummy;
    }

    tFragmentList<T> &GetFragmentListWithCreation(char *name)
    {
	// cerr << "before getfraglistwithcreat" << endl;
	// Print();
	for(typename vector<tFragmentList<T> >::iterator fl = m_FragmentLists.begin();
	     fl != m_FragmentLists.end();
	     fl++)
	{
	    if (!strcmp(fl->m_Name, name)) {
		//if (fl->GetFirstFragmentCount() != count) {
		//    cerr << "Fragment list " << name << " with count " << count << " already has count " << fl->GetFirstFragmentCount() << endl;
		//    exit(-1);
		//}
		// cerr << "FragList " << fl->m_Name << " found" << endl;
		return *fl;
	    }
	}

	// tFragmentList<T> *fnew;
	// fnew = new tFragmentList<T>(name, start, count);
	tFragmentList<T> fnew(name);
	addFragmentList(fnew);
	// cerr << "Created new list " << name << endl;
	// cerr << "after getfraglistwithcreat" << endl;
	// Print();
	return GetFragmentList(name);
    }

    // add things of form
    // 102 TRP
    // 103 TRP
    // 104 TRP
    // 105 GLY
    // 106 GLY
    // 109 GLY
    //
    // and watch for abrupt changes in number or name
    void addFragRun(char *s1, char *s2, FILE *f)
    {
	// cerr << "addFragRun " << s1 << " " << s2 << endl;
	char buff[1024];
	char *rc = buff;
	char prevlab[1024];
	int n;
	int nprev = -999;
	int nstart=-1;
	int count=-1;

	n = atoi(s1);
	prevlab[0] = '\0';
	while (rc) {
	    // cerr << "afr " << n << " " << nprev << " " << s1 << " " << s2 << " " << prevlab << endl;
	    // starting new one and or ending old one
	    if (n != nprev+1 || strcmp(s2, prevlab)) {
		if (strlen(prevlab)) {
		    if (!strcmp(prevlab, "SKIP")) {
			// cerr << "skipping " << n << endl;
			continue;
		    }
		    tFragment<T> frg(prevlab, nstart-1, count); // convert to 0-based
		    tFragmentList<T> &fl = GetFragmentListWithCreation(prevlab);
		    fl.AddFragment(frg);
		    // cerr << "just added frag in fragrun " << prevlab << " " << frg.m_Name << " " << nstart << " " << count << endl;
		}
		nstart = n;
		count = 0;
		strcpy(prevlab, s2);
		// cerr << "nstart count lab prevlab " << nstart << " " << count << " " << s2 << " " << prevlab << endl;
	    }
	    nprev = n;
	    rc = fgets(buff, 1024, f);
	    while (rc && (buff[0] == '#' || buff[0] == '\0' || buff[0] == ' '))
		rc = fgets(buff, 1024, f);
	    if (rc) {
		sscanf(buff, "%d %s", &n, s2);
		count++;
	    }
	    // cerr << "next line " << n << " " << s2 << endl;
	}
	count++;
	// cerr << "checking end of run with count " << count << endl;
	if (count > 0 && strcmp(prevlab, "SKIP")) {
	    int localstart = nstart-1; // convert to 0-based
	    tFragment<T> frg(prevlab, localstart, count);
	    tFragmentList<T> &fl = GetFragmentListWithCreation(prevlab);
	    fl.AddFragment(frg);
	    // cerr << "end of run, added frag " << prevlab << " " << frg.m_Name << " " << nstart << " " << count << endl;
	}
    }

    // Complexity here is due to two ways to add fragments:
    // 1)  Add true fragments with repeat - as contiguous blocks
    // 2)  Add FragmentRuns, which are runs of indices loaded from frag file
    // FragRuns cannot be repeated
    // FragRuns can only be loaded at beginning of file
    // FragRuns are absolute referenced and have no need to know existing fragments
    void Load(char *fname)
    {
	FILE *f = fopen(fname, "r");
	assert(f);
	char buff[1024];
	char s1[1024];
	char s2[1024];
	// a FragRun has a list of individual atoms that may not be contigous
	// once a fragrun has been loaded, normal frags cannot be
	bool hasFragRuns = false;
	int CurrentIndex = 0;

	int RepeatCount;
	int CountOrIndex;
	tFragmentList<T> tempfl("temp");

	// load fragment lists one by one until repeat
	// then repeat all lists since last repeat start
	// then reset repeat start to next list
	// cerr << "in load" << endl;
	int dummyindex = 0;
	while (fgets(buff, 1024, f)) {
	    if (buff[0] == '#' || buff[0] == '\0' || buff[0] == ' ')
		continue;
	    sscanf(buff, "%s %s", s1, s2);
	    // cerr << "load " << s1 << " " << s2 << " " << CurrentIndex << endl;
	    // cerr << "===========================" << endl;
	    // Print();
	    // cerr << "===========================" << endl;
	    if (!strcmp(s1, "REPEAT")) {
		sscanf(s2, "%d", &RepeatCount);
		BuildRepeats(tempfl, CurrentIndex, RepeatCount);
		tempfl.clear();
		continue;
	    } else {
		bool isfrag = (1 == sscanf(s2, "%d", &CountOrIndex));
		if (!isfrag) {
		    hasFragRuns = true;
		    // cerr << "not frag, so calling addFragRun " << s1 << " " << s2 << " " << CountOrIndex << endl;
		    addFragRun(s1, s2, f);
		    // cerr << "returned from addFragRun" << endl;
		} else {
		    if (hasFragRuns) {
			cerr << "Cannot load a normal fragment after a fragment run.  Put all normal fragments at beginning " << s1 << endl;
			exit(-1);
		    }
		    // cerr << "frag, so adding fragment " << s1 << " " << s2 << " " << CountOrIndex << " at current index " << CurrentIndex << endl;
		    tFragment<T> frg(s1, CurrentIndex, CountOrIndex);
		    tempfl.AddFragment(frg, dummyindex);
		}
	    }
	}
	// if didn't end with repeat, need to add final stuff
	// cerr << "adding final" << endl;
	if (!hasFragRuns)
	    BuildRepeats(tempfl, CurrentIndex, 1);
	else
	    for(typename vector<Fragment>::iterator f = tempfl.m_Fragments.begin(); f != tempfl.m_Fragments.end(); f++) {
		if (!strcmp(f->m_Name, "SKIP")) {
		    // cerr << "skipping a SKIP in final" << endl;
		    continue;
		}
		FragmentList &dl = GetFragmentListWithCreation(f->m_Name);
		tFragment<T> fnew(f->m_Name, f->m_Count, f->m_Start);
		dl.AddFragment(fnew, CurrentIndex);
		// cerr << "in final, just added " << f->m_Name << " " << f->m_Start << " " << f->m_Count << endl;
	    }
		
	tempfl.clear();
    }

    // find center of mass of full system of multiple fragment lists
    // split each fragment list into upper and lower set
    // thereby create 2 separate fragment list tables.
    void SplitUpperLower(tFragmentListTable<T> &lower, tFragmentListTable<T> &upper)
    {
	float zsum=0;
	FindCenterOfMasses();

	for (int i=0; i<m_FragmentLists.size(); i++) {
	    fFragmentList *fl = &m_FragmentLists[i];
	    fFragmentList *lowernew = new fFragmentList(fl->m_Name);
	    fFragmentList *uppernew = new fFragmentList(fl->m_Name);
	    for (int j=0; j<fl->m_Fragments.size(); j++) {
		if (fl->m_Fragments[j].m_CenterOfMass.mZ < m_CenterOfMass.mZ)
		    lowernew->m_Fragments.push_back(fl->m_Fragments[j]);
		else
		    uppernew->m_Fragments.push_back(fl->m_Fragments[j]);
	    }
	    lower.addFragmentList(*lowernew);
	    upper.addFragmentList(*uppernew);
	}
    }

    void Print()
    {
	for (int i=0; i<m_FragmentLists.size(); i++) {
	    tFragmentList<T> &fl = m_FragmentLists[i];
	    cout << fl.m_Name << endl;
	    for (int j=0; j<fl.m_Fragments.size(); j++) {
		// ADD ONE SO WORLD SEES PDB 1-BASED ATOMS
		cout << fl.m_Fragments[j].m_Name << " " << fl.m_Fragments[j].m_Start+1 << " " << fl.m_Fragments[j].m_Count << " " << (void *)fl.m_Fragments[j].m_Positions << endl;
	    }
	    cout << endl;
	}
    }
};

typedef tFragmentListTable<float> fFragmentListTable;
typedef tFragmentListTable<double> FragmentListTable;

template <class T>
class tBond
{
public:
    int m_A;
    int m_B;
    tXYZ<T> *m_pAPos;
    tXYZ<T> *m_pBPos;
    tXYZ<T> m_ABVec;
    tXYZ<T> m_ABNorm;
    tXYZ<T> m_XLocal;
    tXYZ<T> m_YLocal;
    tXYZ<T> m_ZLocal;
    T m_ABLength;
    T m_ABLengthSquared;

    tBond(int a, int b)
    {
	m_A = a;
	m_B = b;
	m_pAPos = NULL;
	m_pBPos = NULL;
    }
    tBond(int a, int b, tXYZ<T> *p, bool init = true) : m_A(a), m_B(b)
    {
	m_pAPos = &p[m_A];
	m_pBPos = &p[m_B];
	if (init) {
	    Init();
	}
    }
    // simple init
    void Init()
    {
	m_ABVec = *m_pBPos - *m_pAPos;
	m_ABLengthSquared = m_ABVec.LengthSquared();
	m_ABLength = sqrt(m_ABLengthSquared);
	m_ABNorm = m_ABVec/m_ABLength;
    }

    // Build x,y,z directions based on fragment principal axes
    void Init(tFragment<T> &f)
    {
	m_ABVec = *m_pBPos - *m_pAPos;
	m_ABLengthSquared = m_ABVec.LengthSquared();
	m_ABLength = sqrt(m_ABLengthSquared);
	m_ABNorm = m_ABVec/m_ABLength;
	m_ZLocal = f.m_PrincipalAxes[0];
	// if z is upside down, turn it around
	if (m_ZLocal.mZ * m_pAPos->mZ < 0)
	    m_ZLocal = -m_ZLocal;
	m_YLocal = m_ZLocal.crossProduct(m_ABNorm);
	m_YLocal = m_YLocal*m_YLocal.ReciprocalLength();
	m_XLocal = m_YLocal.crossProduct(m_ZLocal);
	m_XLocal = m_XLocal*m_XLocal.ReciprocalLength();
    }    

    // Build x,y,z directions based on fragment principal axes, using bond as approx up.
    void Init(tFragment<T> &f, tBond<T> &b)
    {
	m_ABVec = *m_pBPos - *m_pAPos;
	m_ABLengthSquared = m_ABVec.LengthSquared();
	m_ABLength = sqrt(m_ABLengthSquared);
	m_ABNorm = m_ABVec/m_ABLength;
	m_ZLocal = f.m_PrincipalAxes[0];
	// if z is upside down, turn it around
	if (m_ZLocal.DotProduct(b.m_ABVec) < 0)
	    m_ZLocal = -m_ZLocal;
	m_YLocal = m_ZLocal.crossProduct(m_ABNorm);
	m_YLocal = m_YLocal*m_YLocal.ReciprocalLength();
	m_XLocal = m_YLocal.crossProduct(m_ZLocal);
	m_XLocal = m_XLocal*m_XLocal.ReciprocalLength();
    }    
    

    tXYZ<T> FindNearestXYZDelta(tXYZ<T> &v, tXYZ<T> &box)
    {
	tXYZ<T> rel = fImaging::NearestImageDisplacement(*m_pAPos, v, box);
	tXYZ<T> proj;
	proj.mX = rel.DotProduct(m_XLocal);
	proj.mY = rel.DotProduct(m_YLocal);
	proj.mZ = rel.DotProduct(m_ZLocal);
	return proj;
    }
};

typedef tBond<float> fBond;
typedef tBond<double> Bond;

template <class T>
class tBondList
{
public:
    vector<tBond<T> > m_Bonds;
    tBondList(tFragmentList<T> &fl, int a, int b)
    {
	for(typename vector<tFragment<T> >::iterator f = fl.m_Fragments.begin(); f != fl.m_Fragments.end(); f++) {
	    m_Bonds.push_back(tBond<T>(f->m_Start+a, f->m_Start+b));
	}
    }
    tBondList(tFragmentList<T> &fl, tXYZ<T> *p, int a, int b)
    {
	for(typename vector<tFragment<T> >::iterator f = fl.m_Fragments.begin(); f != fl.m_Fragments.end(); f++) {
	    m_Bonds.push_back(tBond<T>(f->m_Start+a, f->m_Start+b, p));
	}
    }
    void Init()
    {
	for(typename vector<tBond<T> >::iterator b = m_Bonds.begin(); b != m_Bonds.end(); b++) {
	    b->Init();
	}
    }
    void Init(tFragmentList<T> &fl, tBondList<T> &bl)
    {
	int i=0;
	assert(m_Bonds.size() == fl.m_Fragments.size());
	for(typename vector<tBond<T> >::iterator b = m_Bonds.begin(); b != m_Bonds.end(); b++, i++) {
	    b->Init(fl.m_Fragments[i], bl.m_Bonds[i]);
	}
    }
    void AssignPositions(tXYZ<T> *p)
    {
	for(typename vector<tBond<T> >::iterator b = m_Bonds.begin(); b != m_Bonds.end(); b++) {
	    b->m_pAPos = &p[b->m_A];
	    b->m_pBPos = &p[b->m_B];
	    b->Init();
	}
    }
};

typedef tBondList<float> fBondList;
typedef tBondList<double> BondList;

class Topology
{
public:
    // convert the hitlist, which maps atom to connected element number, into a run of molecules 1:256, 3:1, 1660:3,
    // and the compact string description:  1*256+3*1+1660*3
    static void getMoleculeStringAndList(char *buff, vector<MoleculeRun> &list, int *hits, int nSites)
    {
	char s[1024];
	char t[1024];
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
	{
	    for(typename vector<int>::iterator i = mols.begin(); i != mols.end(); i++) {
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
	}
	if (strlen(s))
	    sprintf(t, "%s+%d*%d", s, count, prevmol);
	else
	    sprintf(t, "%d*%d", count, prevmol);
	strcpy(buff, t);
	molsize.push_back(prevmol);
	molcount.push_back(count);
	// cerr << list.size() << endl;
	{
	    for (int i=0; i<molsize.size(); i++) {
		MoleculeRun mr(molcount[i], molsize[i]);
		// p[0] = molcount[i];
		// p[1] = molsize[i];
		list.push_back(mr);
	    }
	}
	// cerr << list.size() << endl;
    }

    static inline void adjVisit(int *hits, vector<vector<int> > bl, int site, int id)
    {
	// cerr << "visiting " << site << " " << id << endl;
	if (hits[site] == -1) {
	    // cerr << "setting " << site << endl;
	    hits[site] = id;
	    for(typename vector<int>::const_iterator iter = bl[site].begin(); iter != bl[site].end(); iter++) {
		adjVisit(hits, bl, *iter, id);
	    }
	}
    }


    static void getSystemInfo()
    {
#if 0
#ifndef WIN32
        char buff[512];
        if (!systemid && !rtpknown)
            return;
	if (!sourceid)
	    sourceid = rtp.mSourceID;
        cerr << "source id is " << sourceid << endl;
	cerr << "systemid is " << systemid << endl;
	cerr << "dbname is " << dbasename << endl;
	db2::DBName::instance(dbasename);
        db2::ConnHandle& conn = db2::DBName::instance()->connection();
	if (!systemid) {
	    if (!sourceid)
		return;
	    sprintf(buff, "select sys_id from experiment.system_source where source_id=%d", sourceid);
	    db2::StmtHandle stmt(conn);
	    stmt.prepare(buff);
	    SQLINTEGER id;
	    stmt.bindCol(1, id);
	    stmt.execute();
	    stmt.fetch();
	    systemid = id;
	    cerr << "sysID is " << systemid << endl;
	    stmt.close();
	}

        db2::BondList bList;
        int siteId = 0;
        bList.init(systemid);
	int nSites = bList.asAdjList().size();
	int *hits = new int[nSites];
	cerr << "nSites is " << nSites << endl;
	for (int i=0; i<nSites; i++)
	    hits[i] = -1;
	int mol = 0;
	for (int i=0; i<nSites; i++) {
	    // cerr << "checking " << i << endl;
	    if (hits[i] == -1) {
		// cerr << "visiting " << i << endl;
		adjVisit(hits, bList.asAdjList(), i, mol);
		mol++;
	    }
	}
	cerr << "NFrags: " << mol << endl;
	getMoleculeStringAndList(moleculerunstring, moleculeruns, hits, nSites);
	// cerr << list.size() << endl;
	// for(typename vector<MoleculeRun>::iterator p = list.begin(); p != list.end(); p++)
	//    cerr << p->count << " " << p->size << endl;

	cerr << "Molecule connectivity " << moleculerunstring << endl;

	masses = new double[nSites];
	sprintf(buff, "select atomic_mass from mdsystem.site where sys_id=%d order by site_id", systemid);
	db2::StmtHandle stmt(conn);
	stmt.prepare(buff);
	SQLDOUBLE mass;
	stmt.bindCol(1, mass);
	stmt.execute();
	int i = 0;
	while (stmt.fetch()) {
	    masses[i++] = mass;
	}
	stmt.close();

	delete [] hits;
#endif
#endif
    }

};



#endif
