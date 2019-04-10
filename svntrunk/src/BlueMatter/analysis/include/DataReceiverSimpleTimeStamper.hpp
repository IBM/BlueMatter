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
 #ifndef DATARECEIVERSIMPLETIMESTAMPER_HPP
#define DATARECEIVERSIMPLETIMESTAMPER_HPP


#include <iostream>
#include <iomanip>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/Tag.hpp>

class TimeHisto
{
public:
    double m_MinTime;
    double m_MaxTime;
    enum {MAXBIN=4000000};
    enum {MAXLEGITSTEP=3600};  // max possible time step is 3600 sec
    // if MAXBIN is 2 and maxtime is 2, have -0.5-0.5   0.5-1.5  1.5-2.5
    double m_MaxBinTime;
    double m_BinStep;
    double m_TimeOfMin;
    double m_TimeOfMax;
    int m_StepOfMin;
    int m_StepOfMax;
    double m_Sum;
    double m_SumSQ;
    int m_NCounts;
    char m_Name[128];
    double m_RawValue;
    double m_PrevDT;
    bool m_Primed;
    double m_FirstRawTime;

    int m_Counts[MAXBIN+1];

    void clear() {
	m_MinTime = 1.0E99;
	m_MaxTime = -m_MinTime;
	m_TimeOfMin = 0;
	m_TimeOfMax = 0;
	m_StepOfMin = 0;
	m_StepOfMax = 0;
	m_NCounts = 0;
	m_Sum = 0;
	m_SumSQ = 0;
	m_PrevDT = 0;

	for (int i=0; i<=MAXBIN; i++)
	    m_Counts[i] = 0;
    }

    TimeHisto(char *name, float maxtime=1000.0) {
	clear();
	strcpy(m_Name, name);
	m_RawValue = 0;
	m_Primed = false;

	m_MaxBinTime = maxtime;
	m_BinStep = m_MaxBinTime/MAXBIN;
	// cerr << "TimeHisto maxtime, binstep " << m_MaxBinTime << " " << m_BinStep << endl;
	for (int i=0; i<=MAXBIN; i++)
	    m_Counts[i] = 0;
    }

    void addTime(double realtime, int step) {
	if (!m_Primed) {
	    m_RawValue = realtime;
	    m_Primed = true;
	    m_FirstRawTime = realtime;
	    // cout << "Priming " << realtime << endl;
	    return;
	}
	
	double t = realtime-m_RawValue;
	// cout << "New T " << t << endl;
	if (t < 0 || t > MAXLEGITSTEP) {
	    m_RawValue = realtime;
	    return;
	}
	int nbin = t/m_BinStep+0.5;
	if (nbin > MAXBIN)
	    nbin = MAXBIN;
	if (nbin < 0)
	    nbin = 0;
	m_Counts[nbin]++;
	m_NCounts++;
	m_Sum += t;
	m_SumSQ += t*t;

	if (m_MinTime > t) {
	    m_MinTime = t;
	    m_TimeOfMin = realtime;
	    m_StepOfMin = step;
	}
	if (m_MaxTime < t) {
	    m_MaxTime = t;
	    m_TimeOfMax = realtime-m_FirstRawTime;
	    m_StepOfMax = step;
	    // cout << "Set MaxtTime, tom, first raw " << m_MaxTime << " " << m_TimeOfMax << " " << m_FirstRawTime << endl;
	}
	m_RawValue = realtime;
	m_PrevDT = t;
    }

    // just return time of accum
    // this updates the state of the accumulator: p and sum
    float getAccum(double frac, int &p, int &sum) {
	// add until too much
	// then respond with p-1
	// p always points to current not yet added bin count
	// sum is always sum of bins up to current
	while (p < MAXBIN && float(sum)/m_NCounts <= frac) {
	    sum += m_Counts[p];
	    p++;
	}
	return (p-1)*m_BinStep;
    }

    void dumpaccum(double frac, int &p, int &sum) {
	cout << getAccum(frac, p, sum) << " ";
    }

    // step range, time range
    // name mean sd min 2% 10% 20% ... 90% 98% max stepofmax timeofmax
    void dump() {
	if (m_NCounts < 1)
	    return;
	double mean = m_Sum/m_NCounts;
	double sd = sqrt(m_SumSQ/m_NCounts-mean*mean);
	cout << m_Name << " " << mean << " " << sd << " " << m_MinTime << " ";

	int ptr = 0;
	int countsum = 0;

	dumpaccum(0.005, ptr, countsum);
	dumpaccum(0.01, ptr, countsum);
	dumpaccum(0.5, ptr, countsum);
	dumpaccum(0.99, ptr, countsum);
	dumpaccum(0.995, ptr, countsum);

	cout << m_MaxTime << " " << m_StepOfMax << " " << m_TimeOfMax << endl;
    }

    void dumpArray(int nbins) {
	if (m_NCounts < 1)
	    return;
	int ptr = 0;
	int countsum = 0;

	cout << m_MinTime << " ";
	for (int i=1; i<nbins; i++)
	    dumpaccum(double(i)/nbins, ptr, countsum);
	cout << m_MaxTime << endl;
    }

    // output ts, mean, min, 0, t1, r1, t2, r2, ..., tn, rn, max, 0
    void dumpIntegral(int nbins) {
	if (m_NCounts < 1)
	    return;
	int ptr = 0;
	int countsum = 0;

	float times[1000];
	if (nbins > 1000) {
	    cerr << "ndetailed bins cannot be greater than 1000" << endl;
	    exit(-1);
	}

	cout << m_MinTime << " 0.0 ";
	float t1 = m_MinTime;
	float t2 = 0.0;
	float r;
	int prevsum = 0;
	for (int i=1; i<nbins; i++) {
	    t2 = getAccum(double(i)/nbins, ptr, countsum);
	    if (t2 == t1)
		r = m_Counts[ptr]/m_BinStep/float(m_NCounts);
	    else
		r = (countsum-prevsum)/(t2-t1)/float(m_NCounts);
	    cout << (t1+t2)/2.0 << " " << r << " ";
	    t1 = t2;
	    prevsum = countsum;
	}

	cout << (m_MaxTime+t2)/2 << " ";

	cout << (m_NCounts-prevsum)/(m_MaxTime-t2) << " "; 

	cout << m_MaxTime << " 0.0" << endl;
    }

};

class DataReceiverSimpleTimeStamper : public DataReceiver
{
public:
    int m_NSteps;
    TimeHisto *m_Histo;
    TimeHisto *m_FullHisto;
    int m_ReportInterval;
    enum {SUMMARY=0, FULL=1, DETAILED=2, RAW=3, SIMPLE=4, INTEGRAL=5};
    int m_ReportMode;
    int m_Skip;
    int m_NSkipped;
    int m_NDetailedBins;
    float m_MaxTime;
    int m_TimeTag;


    DataReceiverSimpleTimeStamper()
    {
	m_NSteps = 0;
	// m_Histo = new TimeHisto("", m_MaxTime);
	// m_FullHisto = new TimeHisto("", m_MaxTime);
	m_ReportInterval = 1000;
	m_ReportMode = FULL;
	m_Skip = 0;
	m_TimeTag = 0;
    }

    void setParameters(int reportmode, int reportinterval, int nskip, int ndetailedbins, float maxtime=1000.0, int ttag = 0)
    {
	m_ReportMode = reportmode;
	m_ReportInterval = reportinterval;
	m_Skip = nskip;
	m_NDetailedBins = ndetailedbins;
	m_MaxTime = maxtime;
	m_Histo = new TimeHisto("", m_MaxTime);
	m_FullHisto = new TimeHisto("", m_MaxTime);
	m_TimeTag = ttag;
    }

    virtual void init() 
    {
	cout.precision(6);
	if (m_ReportMode==FULL)
	    cout << "TimeStep  Mean  SD   Min  Pct005 Pct01 Pct50 Pct99 Pct995 Max StepOfMax TimeOfMax" << endl;
    }

    virtual void final(int status = 1)
    {
	if (m_ReportMode != RAW) {
	    cout << m_FullHisto->m_NCounts << " ";
	    m_FullHisto->dump();
	}
    }
  
    virtual void controlTimeStamp(ED_ControlTimeStamp &ts)
    {    
	if (m_ReportMode==RAW) {
	    cout << ts << endl;
	    return;
	}
	if (ts.mTag1 != m_TimeTag)
	    return;
	m_NSteps++;
	if (m_NSteps < m_Skip)
	    return;
	double t = ts.mSeconds + ts.mNanoSeconds/1.0E9;
	m_FullHisto->addTime(t, ts.mFullOuterTimeStep);
	if (m_ReportMode == SIMPLE && m_FullHisto->m_PrevDT > 0) {
	    cout << ts.mFullOuterTimeStep << " " << m_FullHisto->m_PrevDT << endl;
	    return;
	}
	if (m_ReportMode == SUMMARY)
	    return;
	m_Histo->addTime(t, ts.mFullOuterTimeStep);
	if (m_NSteps <= m_Skip)
	    return;
	if (m_NSteps%m_ReportInterval == 0) {
	    cout << ts.mFullOuterTimeStep << " ";
	    if (m_ReportMode == FULL)
		m_Histo->dump();
	    else if (m_ReportMode == DETAILED)
		m_Histo->dumpArray(m_NDetailedBins);
	    else {
		cout << m_Histo->m_Sum/m_Histo->m_NCounts << " ";
		m_Histo->dumpIntegral(m_NDetailedBins);
	    }
	    m_Histo->clear();
	}
    }  
};

#endif
