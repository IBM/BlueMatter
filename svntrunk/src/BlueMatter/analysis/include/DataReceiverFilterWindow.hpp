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
 #ifndef DATARECEIVERFILTERWINDOW_HPP
#define DATARECEIVERFILTERWINDOW_HPP

#include <BlueMatter/DataReceiver.hpp>
// #include <BlueMatter/PacketStructures.hpp>
#include <BlueMatter/ExternalDatagram.hpp>

class DataReceiverFilterWindow : public DataReceiver
{
    typedef struct
    {
        double intraE;
        double intraE2;
        double interE;
        double interE2;
        double kineticE;
        double kineticE2;
        double totalE;
        double totalE2;
        double temp;
        double temp2;
    } tEnergySummary;
    
private:
    int mWindowWidth;
    tEnergySummary mesWindow;
    tEnergySummary mesTotal;
    unsigned mPrev;
    unsigned mCurrent;
    int mCount;
    int mTotalCount;
    int mStarted;
    
public:
    
    void zeroEnergies(tEnergySummary *es)
    {
        es->intraE     = 0;
        es->intraE2    = 0;
        es->interE     = 0;
        es->interE2    = 0;
        es->kineticE   = 0;
        es->kineticE2  = 0;
        es->totalE     = 0;
        es->totalE2    = 0;
        es->temp       = 0;
        es->temp2      = 0;
    }
    
    void addEnergies(tEnergySummary *dest, tEnergyInfo *src)
    {
        double d;
        
        d = src->mEnergySums.mIntraE;
        dest->intraE += d;
        dest->intraE2 += d*d;
        
        d = src->mEnergySums.mInterE;
        dest->interE += d;
        dest->interE2 += d*d;
        
        d = src->mEnergySums.mTotalE;
        dest->totalE += d;
        dest->totalE2 += d*d;
        
        d = src->mEnergySums.mTemp;
        dest->temp += d;
        dest->temp2 += d*d;
        
        d = src->mEnergySumAccumulator.mKineticEnergy;
        dest->kineticE += d;
        dest->kineticE2 += d*d;
        
    }
    
    void restartWindow()
    {
        mCount = 0;
        mPrev = mCurrent;
        zeroEnergies(&mesWindow);
    }
    
    DataReceiverFilterWindow(int w, int DelayStats=0)
    {
        setWindowWidth(w);
        mPrev = 0;
        mCurrent = 0;
        mCount = 0;
        mTotalCount = 0;
        mStarted = 0;
        zeroEnergies(&mesTotal);
        restartWindow();
    }
    
    void setWindowWidth(int w)
    {
        mWindowWidth = w;
    }
    
    virtual void init()
    {
        char buff[512];
        cout << "# WindowSize = " << mWindowWidth << endl;
        sprintf(buff, "%7s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s", "# Step ", "Avg(intraE)", "RMSD(intraE)", "Avg(interE)", "RMSD(interE)",
            "Avg(kinE)", "RMSD(kinE)", "Avg(totalE)", "RMSD(totalE)", "Avg(Temp)", "RMSD(temp)");
        cout << buff << endl;
    }
    
#define PRINTSTEP(a) invStep*es->a, sqrt(invStep*(es->a##2 - es->a * es->a * invStep))
    
    void printEnergies(tEnergySummary *es, int k)
    {
        char buff[512];
        double invStep = 1.0/k;
        sprintf(buff, "%7d %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f",
            mCurrent,
            PRINTSTEP(intraE),
            PRINTSTEP(interE),
            PRINTSTEP(kineticE),
            PRINTSTEP(totalE),
            PRINTSTEP(temp) );
        cout << buff << endl;
    }
    
    // New energy
    virtual void energy(Frame *f)
    {
        mCurrent = f->mEnergyInfo.mEnergySumAccumulator.mFullTimeStep;
        if (!mStarted) {
            mPrev = mCurrent - 1;
            mStarted = 1;
        }
        addEnergies(&mesTotal, &f->mEnergyInfo);
        addEnergies(&mesWindow, &f->mEnergyInfo);
        mCount++;
        mTotalCount++;
        
        if (mCurrent - mPrev >= mWindowWidth) {
            printEnergies(&mesWindow, mCount);
            restartWindow();
        }
    }
    
    virtual void final()
    {
        cout << endl;
        cout << endl;
        cout << "# Final:" << endl;
        printEnergies(&mesTotal, mTotalCount);
    }
};

#endif
