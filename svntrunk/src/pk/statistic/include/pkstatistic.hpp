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
 /***************************************************************************
 * Project:         pK
 *
 * Module:          pkstatistic.hpp
 *
 * Purpose:         gather and report statistics
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         081105 TJCW Created
 *
 ***************************************************************************/
#ifndef __PKSTATISTIC_HPP__
#define __PKSTATISTIC_HPP__

#ifndef PKFXLOG_STATISTIC
#define PKFXLOG_STATISTIC ( 0 )
#endif
#ifndef PKFXLOG_STATISTIC_DETAIL
#define PKFXLOG_STATISTIC_DETAIL ( 0 )
#endif

    // Statistic observation (usually for load balancing)
    class Statistic
       {
       	  typedef unsigned int Stat ;
       public:
         enum {
             // kSequence = 1024 ,
                kSequence = 1,
         	kHistogramBuckets = 16
         } ; 
         Stat mValues[kSequence] ;
         unsigned int mCurrent ;
         unsigned int mHistogram[kHistogramBuckets] ;
         Statistic()
         {
         	mCurrent = 0 ; 
         }
         Stat First(void) const { return mValues[0] ; } ;
         Stat Max(void) const
         {
         	Stat Candidate = mValues[0] ; 
         	for ( unsigned int a=1;a < mCurrent;a+=1)
         	{
         		if ( mValues[a] > Candidate ) Candidate = mValues[a] ;
         	}
         	return Candidate ;
         }
         Stat Min(void) const
         {
         	Stat Candidate = mValues[0] ; 
         	for ( unsigned int a=1;a < mCurrent;a+=1)
         	{
         		if ( mValues[a] < Candidate ) Candidate = mValues[a] ;
         	}
         	return Candidate ;
         }
         double Sum(void) const
         {
         	double Running = 0.0 ; 
         	for ( unsigned int a=0;a<mCurrent;a+=1)
         	{
         		Running += mValues[a] ; 
         	}
         	return Running ;
         }
         double SumSquare(void) const
         {
         	double Running = 0.0 ;
         	for ( unsigned int a=0;a<mCurrent;a+=1)
         	{
         		double v = mValues[a] ; 
         		Running += v*v ;
         	}
         	return Running ;
         }
         void Histogram(Stat Lo, Stat Hi)
         {
         	for ( unsigned int a=0;a<kHistogramBuckets;a+=1) 
         	{
         		mHistogram[a] = 0 ; 
         	}
         	double BucketReciprocal = ((double)kHistogramBuckets)/(Hi-Lo) ;
         	for ( unsigned int b=0;b<mCurrent;b+=1)
         	{
         		int Bucket = (mValues[b]-Lo)*BucketReciprocal ;
         		if ( Bucket >= 0 && Bucket < kHistogramBuckets )
         		{
         			mHistogram[Bucket] += 1 ;
         		}
         	}
         }
         
         void Report(void) 
         {
         	Stat aMin = Min() ; 
         	Stat aMax = Max() ; 
         	double aSum = Sum() ; 
         	double aSumSquare = SumSquare() ; 
         	double aMean = aSum/mCurrent ;
         	double aVariance = aSumSquare/mCurrent - aMean*aMean ;
         	Histogram(aMin,aMax+1) ; 
         	BegLogLine( PKFXLOG_STATISTIC ) 
         	  << " Statistic Count=" << mCurrent
         	  << " First=" << First() 
         	  << " Min=" << aMin
         	  << " Max=" << aMax
         	  << " Mean=" << aMean 
         	  << " Variance=" << aVariance
         	  << EndLogLine ;
         	for ( int a=0;a<kHistogramBuckets;a+=1)
         	{
         		BegLogLine(PKFXLOG_STATISTIC)
         		  << "Bucket=" << a
         		  << " Count=" << mHistogram[a] 
         		  << EndLogLine ;
         	}
         	
         	
         } 
         
         void Observe(Stat Value)
         {
         	BegLogLine(PKFXLOG_STATISTIC_DETAIL)
         	  << "Statistic::Observe mCurrent=" << mCurrent
         	  << " Value=" << Value
         	  << EndLogLine ;
         	  
         	if ( mCurrent < kSequence )
         	{
         		mValues[mCurrent] = Value ;
         		mCurrent += 1 ;
         		if ( mCurrent == kSequence ) Report() ; 
         	} ;
         }
         
       } ;

#endif 
