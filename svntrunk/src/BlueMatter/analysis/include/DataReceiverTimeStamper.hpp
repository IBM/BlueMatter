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
 #ifndef DATARECEIVERTIMESTAMPER_HPP
#define DATARECEIVERTIMESTAMPER_HPP

#include <iostream>
#include <iomanip>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/Tag.hpp>

#include <algorithm>
#include <vector>

class TimeValue 
{
public:
  unsigned long mSeconds;
  unsigned long mNanoseconds;

    static inline TimeValue MinValue() 
    {
        TimeValue rc;
        rc.mSeconds = 0;
        rc.mNanoseconds = 0;
        return ( rc );
    }
          
    static inline TimeValue MaxValue() 
    {
        TimeValue rc;
        rc.mSeconds = 0x7fffffff;
        rc.mNanoseconds = 0x7fffffff;
        return ( rc );
    }

    inline TimeValue operator+=( const TimeValue& aOther )
    {
        int nanoSum = mNanoseconds + aOther.mNanoseconds;
        
        int difference = nanoSum - 1000000000;
        
        // If carry over occured adjust for it
        if ( difference < 0 )
            {
                mSeconds += aOther.mSeconds;
                mNanoseconds = nanoSum;                
            }
        else
        {
            mSeconds += (aOther.mSeconds + 1);
            mNanoseconds = difference;
        }
        
        return (*this);
    }
    
    inline TimeValue operator-( const TimeValue& aOther ) const
    {
        TimeValue rc;
        rc.mSeconds = mSeconds - aOther.mSeconds;
        rc.mNanoseconds = mNanoseconds - aOther.mNanoseconds;
        
        // If carry over occured adjust for it
        if( rc.mNanoseconds < 0 )
            {
                rc.mSeconds--;
                rc.mNanoseconds += 1000000000;
            }                
        
        return ( rc );
    }
    
    inline
        int operator<( const TimeValue& aOther )
    {
        if(mSeconds < aOther.mSeconds)
            return 1;
        else if(mSeconds ==  aOther.mSeconds)
            if(mNanoseconds < aOther.mNanoseconds)
                return 1;
            else 
                return 0;
        else
            return 0;
    }

    inline
        int operator<=( const TimeValue& aOther )
    {
        if(mSeconds < aOther.mSeconds)
          return 1;
        else if (mSeconds == aOther.mSeconds)
            if(mNanoseconds <=  aOther.mNanoseconds)
                return 1;
            else
                return 0;
        else 
            return 0;
    }

    inline
        int operator>=( const TimeValue& aOther )
    {
        if(mSeconds > aOther.mSeconds)
            return 1;
        else if (mSeconds == aOther.mSeconds)
            if(mNanoseconds >=  aOther.mNanoseconds)
                return 1;
            else
                return 0;
        else 
            return 0;
    }

    inline
        int operator>( const TimeValue& aOther )
    {
        if(mSeconds > aOther.mSeconds)
            return 1;
        else if(mSeconds ==  aOther.mSeconds)
            if(mNanoseconds > aOther.mNanoseconds)
                return 1;
            else 
                return 0;
        else
            return 0;
    }

  void Zero() 
  {
    mSeconds = 0;
    mNanoseconds = 0;
  }

  static double getDouble( TimeValue aTime ) 
    {
      char timeString[512];
      double result;
      //      sprintf(timeString, "%d.%09d", aTime.mSeconds, aTime.mNanoseconds);
      //sscanf(timeString, "%lf", &result);
      double rc = aTime.mNanoseconds * 0.000000001 + aTime.mSeconds;
      return(rc);
      
        //      return result;
    }    
};

struct RunInfoPerTimeStep 
{
  double timeInfo[ Tag::TAG_COUNT ];  
  int numberOfValues[ Tag::TAG_COUNT ];
};

#define MAX_TIMESTEP 100

static RunInfoPerTimeStep dataCollectionArray[ MAX_TIMESTEP ];

static RunInfoPerTimeStep sumX;
static RunInfoPerTimeStep sumXsqr;
static RunInfoPerTimeStep minX;
static RunInfoPerTimeStep maxX;

class DataReceiverTimeStamper : public DataReceiver
{

  double    mTotalTime;
  int       mTotalTimeSteps;
  int       mTotalPrintOutTimeSteps[ Tag::TAG_COUNT ];
  int       mFirstTimeStep;

  int       mDelayNsteps;
  int       mDumpSummaryOnly;

public:
    DataReceiverTimeStamper( int delayNsteps = TIMESTEPS_TO_TUNE, int dumpSummaryOnly = 0) {
        mDelayNsteps = delayNsteps;
        mDumpSummaryOnly = dumpSummaryOnly;
    }

  virtual void init() 
  {
    mTotalTime=0.0;
    mTotalTimeSteps = 0;
    mFirstTimeStep = -1;
    
    for(int i=0; i<MAX_TIMESTEP; i++) 
      {
        for( int j=0; j<Tag::TAG_COUNT; j++ ) {
          dataCollectionArray[ i ].timeInfo[j]=0.0;
          dataCollectionArray[ i ].numberOfValues[ j ] = 0;
        }
      }

    for( int j=0; j<Tag::TAG_COUNT; j++ ) 
      {
        mTotalPrintOutTimeSteps[ j ] = 0;

        sumX.timeInfo[j]=0.0;
        sumX.numberOfValues[j] = 0;
  
        sumXsqr.timeInfo[j]=0.0;
        sumXsqr.numberOfValues[j] = 0;

        minX.timeInfo[j]=99999999.0;
        minX.numberOfValues[j] = 0;

        maxX.timeInfo[j]=0.0;
        maxX.numberOfValues[j] = 0;
      }
  }
  
  virtual void controlTimeStamp(ED_ControlTimeStamp &ts)
  {    
    TimeValue t;
    t.mSeconds = ts.mSeconds;
    t.mNanoseconds = ts.mNanoSeconds;    

    double timeValue = TimeValue::getDouble( t );

    int timeStep = ts.mFullOuterTimeStep;

    printf("GotPacket:: ts = %d, tag = %d, tv = %f\n", timeStep, ts.mTag1, timeValue );

    if( mFirstTimeStep == -1 )
        mFirstTimeStep = timeStep;    

    if( ts.mTag1 == Tag::TimeStep )
      mTotalTime += timeValue;


    if( (timeStep - mFirstTimeStep) >= mDelayNsteps ) 
      {
        // cout << "mDelayNsteps: " << mDelayNsteps << endl;

        if( ts.mTag1 >= Tag::TAG_COUNT ) 
          {
            cerr << "Tag: " << ts.mTag1 << " not recognized." << endl;
          }
                
        sumX.timeInfo[ ts.mTag1 ] += timeValue;
        sumX.numberOfValues[ ts.mTag1 ]++;
        sumXsqr.timeInfo[ ts.mTag1 ] += (timeValue * timeValue);  
        sumXsqr.numberOfValues[ ts.mTag1 ]++;
        
        if( timeValue <  minX.timeInfo[ ts.mTag1 ] )
          {
            minX.timeInfo[ ts.mTag1 ] = timeValue;
          }
        
        if( timeValue >  maxX.timeInfo[ ts.mTag1 ] )
          {
            maxX.timeInfo[ ts.mTag1 ] = timeValue;
          }
        
        if( timeStep < (MAX_TIMESTEP+mFirstTimeStep) )
          {        
            int timeStepIndex = (timeStep - mFirstTimeStep) - (mDelayNsteps);
            
            if( timeStepIndex < 0 ) 
              {
                cerr << "ERROR:: timeStepIndex < 0 " << endl;
                exit(1);
              }
              
            // cout << "timeStepIndex: " << timeStepIndex << endl;
  
            dataCollectionArray[ timeStepIndex ].timeInfo[ ts.mTag1 ] += timeValue;
            
            if(dataCollectionArray[ timeStepIndex ].numberOfValues[ ts.mTag1 ] == 0) 
              {
                dataCollectionArray[ timeStepIndex ].numberOfValues[ ts.mTag1 ] = 1;
                mTotalPrintOutTimeSteps[ ts.mTag1 ]++;
              }
          }   
      }
  }  
    
  void addTimeValue(TimeValue &aTs, TimeValue &aResult)
  {
    unsigned long Seconds = aTs.mSeconds;
    unsigned long Nanoseconds = aTs.mNanoseconds;
   
    unsigned long nanoSum = aResult.mNanoseconds + Nanoseconds;
    
    if( nanoSum >= 1000000000 ) {
      aResult.mSeconds += ( Seconds + 1 );
      aResult.mNanoseconds += (Nanoseconds - 1000000000);
    }
    else {
      aResult.mSeconds += Seconds;
      aResult.mNanoseconds += Nanoseconds;
    }
  }

  virtual void final(int status=1) 
  {

    // We don't count the last time step
    //    mTotalPrintOutTimeSteps[ Tag::TimeStep ]--;

    if( !mDumpSummaryOnly ) {
        cout << "TimeStep(after delay)\t";
        // Output the information per time step
        for( int i=0; i<Tag::TAG_COUNT; i++) 
            {
              if( sumX.numberOfValues[ i ] == 0 )
                continue;

                char nameForTag[64];
                Tag::GetNameForTag( i, nameForTag );
                cout << nameForTag  << "\t";
            }
        cout << endl;
        
        for( int i=0; i<mTotalPrintOutTimeSteps[ Tag::TimeStep ]; i++ ) {
            cout << i << "\t";
            for( int j=0; j<Tag::TAG_COUNT; j++ ) {  
              if( sumX.numberOfValues[j] == 0 )
                continue;
              cout << setw(12) << setprecision(6) << dataCollectionArray[i].timeInfo[j];
            }
            cout << endl;
        }    
    }   

    // Report results with some analysis.
    // Calculate the Total Time
//     for(int i=0; i<mTotalTimeSteps; i++) {
//       mTotalTime += dataCollectionArray[i].timeInfo[ Tag::TimeStep ];
//     }
    
//     char nanoSecStr[16];
//     sprintf(nanoSecStr, "%09d", mTotalTime.mNanoseconds);
           
    cout << "Delay N steps: " << mDelayNsteps << endl;
    cout << "Number of time steps to print out: " << mTotalPrintOutTimeSteps[ Tag::TimeStep ] << endl; 
    cout << "Number of time steps analyzed : " << sumX.numberOfValues[ Tag::TimeStep ] << endl; 
    cout << "Total time of run: " << mTotalTime << endl;
    
    for( int j=0; j<Tag::TAG_COUNT; j++ ) 
      {
        DumpInfoForTag( j );    
      }
  }
  
  void DumpInfoForTag( int tag ) 
    {    
      char tagName[ 32 ];
      Tag::GetNameForTag( tag, tagName );
      
      if( sumX.numberOfValues[ tag ] == 0 )
        return;
      
      double meanTime = sumX.timeInfo[ tag ] / sumX.numberOfValues[ tag ];
      double variance = sumXsqr.timeInfo[ tag ] / sumXsqr.numberOfValues[ tag ] - meanTime*meanTime;
      double std = sqrt(variance);

      cout << "Mean time step (" << tagName << "): " << meanTime << endl;
      cout << "Variance (" << tagName << "): " << variance << endl;      
      cout << "StdDev (" << tagName << "): " << std << endl;      
      cout << "Max (" << tagName << "): " << maxX.timeInfo[ tag ] << endl;
      cout << "Min (" << tagName << "): " << minX.timeInfo[ tag ] << endl;      
    }  

#if 0
  void DumpInfoForTag( int tag ) 
    {
      char tagName[ 32 ];
      Tag::GetNameForTag( tag, tagName );

      double TotalSumAfterRebalancing = 0.0;

      double minTime = 99999999.0;
      double maxTime = 0.0;
      
      
      for( int i = mDelayNsteps+1; i < mTotalTimeSteps; i++ )
      {
        double currentTimeValue = dataCollectionArray[i].timeInfo[ tag ];
        TotalSumAfterRebalancing += currentTimeValue;
        
        double curTime =  currentTimeValue ;
        // Look for Min
        if( curTime < minTime ) {
            minTime = curTime;
        }

        // Look for Max
        if( curTime > maxTime ) {
            maxTime = curTime;
        }
      }
      
      double sumAR = TotalSumAfterRebalancing ;
      int numberOfElements = mTotalTimeSteps - (mDelayNsteps+1);
      double meanTime = ( sumAR/numberOfElements );
      
      cout << "Mean time step (" << tagName << "): " << meanTime << endl;
      
      double sum = 0.0;
      for( int i = mDelayNsteps+1; i < mTotalTimeSteps; i++ )
      {
       double currentTime = dataCollectionArray[ i ].timeInfo[ tag ] ;
       sum += ( (currentTime - meanTime) * (currentTime - meanTime) );                
      }
    
      double variance = (sum/(numberOfElements));
      
      cout << "Variance(" << tagName << "): " << variance << endl;      
      cout << "Max(" << tagName << "): " << maxTime << endl;
      cout << "Min(" << tagName << "): " << minTime << endl;
      cout << endl;
    }
#endif
};

#endif
