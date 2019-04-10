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
 #ifndef DATARECEIVERWATERDUMPER_HPP
#define DATARECEIVERWATERDUMPER_HPP

#include <fstream>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/ExternalDatagram.hpp>

class dat
{
public:
    double min;
    double max;
    double sum;
    double sumsq;
    int minstep;
    int minwat;
    int maxstep;
    int maxwat;
    int n;
    int step;
    int nwat;

    void init() {
	min = 1.0E10;
	max = -1.0E10;
	sum = 0;
	sumsq = 0;
	n = 0;
    }

    void add(double x, int step, int nwat) {
	sum += x;
	sumsq += x*x;
	if (min > x) {
	    min = x;
	    minstep = step;
	    minwat = nwat;
	}
	if (max < x) {
	    max = x;
	    maxstep = step;
	    maxwat = nwat;
	}
	n++;
    }

    double mean() {
	return sum/n;
    }

    double sigma() {
	return sqrt(sumsq/n-sum*sum/n/n);
    }

    void dump()
    {
	cout << mean() << " " << sigma() << " " << min << " " << minstep << " " << minwat << " " << max << " " << maxstep << " " << maxwat << endl;
    }
};

class DataReceiverWaterDumper : public DataReceiver
{
public:
    int mNNonWaterAtoms;
    int mHOH;  // or OHH;
    dat oh1, oh2, h1h2;

    void setParams(int NNonWaterAtoms, int hoh = 1)
    {
        mNNonWaterAtoms = NNonWaterAtoms;
	mHOH = hoh;
    }

    virtual void init()
    {
	oh1.init();
	oh2.init();
	h1h2.init();
    }

    virtual void final(int status = 1)
    {
	cout << "mean" << " " << "sigma" << " " << "min" << " " << "minstep" << " " << "minwater" << " " << "max" << " " << "maxstep" << " " << "maxwater" << endl;
	oh1.dump();
	oh2.dump();
	h1h2.dump();
    }

    virtual void sites(Frame *f)
        {
            unsigned CurrentStep = f->mOuterTimeStep;

            tSiteData *ps= &f->mSiteData.getArray()[mNNonWaterAtoms];
            int NSites = f->mSiteData.getSize();
            int NWaters = (NSites-mNNonWaterAtoms)/3;

	    XYZ h1, o, h2;

            for (int i=0; i<NWaters; i++) {
		if (mHOH) {
		    h1 = ps->mPosition;
		    ps++;
		    o = ps->mPosition;
		    ps++;
		    h2 = ps->mPosition;
		    ps++;
		} else {
		    o = ps->mPosition;
		    ps++;
		    h1 = ps->mPosition;
		    ps++;
		    h2 = ps->mPosition;
		    ps++;
		}

		oh1.add(h1.Distance(o), CurrentStep, i);
		oh2.add(h2.Distance(o), CurrentStep, i);
		h1h2.add(h1.Distance(h2), CurrentStep, i);

#if 0
                printf("%f %f %f %f\n", h1.mX, h1.mY, h1.mZ, h1.Distance(o));
                printf("%f %f %f %f\n", o.mX, o.mY, o.mZ, o.Distance(h2));
                printf("%f %f %f %f\n", h2.mX, h2.mY, h2.mZ, h2.Distance(h1));
#endif
            }
            // printf("\n");
        }
};

#endif
