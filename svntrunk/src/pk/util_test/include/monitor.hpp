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
 #ifndef __MONITOR_HPP__
#define __MONITOR_HPP__

#include <math.h>
#include <stdio.h>
//#include <pk/pthread.hpp>
#include <pk/platform.hpp>

class Monitor // : public PhysicalThreadContext
  {
  public:
    int       mStartedFlag;
    unsigned *mCounterPtr;
    int       mIterationCount;
    int       mIgnoreIterationCount;
    int       mMicroSecondsPerIteration;
    int       mStabalizationPercent;
    unsigned *mFinishedFlag;
    unsigned *mResults;



    Monitor( unsigned *aCounterPtr )
      {
      mCounterPtr               = aCounterPtr;
      mStartedFlag              = 0;
      mIterationCount           = 15;
      mIgnoreIterationCount     = 0;
      mMicroSecondsPerIteration = 1000000;
      mStabalizationPercent     = 50;
      mResults                  = NULL;
      mFinishedFlag             = NULL;


      }

    void Start()
      {
      Platform::Thread::Create( StaticPhysicalThreadMain, (void*)this );
      }

    void
    SetIterationCount( int aIterationCount )
      {
      assert( mStartedFlag == 0 );  // Not allowed to mod if monitor is running.
      mIterationCount = aIterationCount;
      }

    void
    SetMicroSecondsPerIteration( int aMicroSecondsPerIteration )
      {
      assert( mStartedFlag == 0 );  // Not allowed to mod if monitor is running.
      mMicroSecondsPerIteration = aMicroSecondsPerIteration;
      }

    void
    SetIgnoreIterations( int aIgnoreIterationCount )
      {
      assert( mStartedFlag == 0 );  // Not allowed to mod if monitor is running.
      mIgnoreIterationCount = aIgnoreIterationCount;
      }

    void
    SetStabalizeWithinPercent( int aStabalizePercent )
      {
      assert( mStartedFlag == 0 );  // Not allowed to mod if monitor is running.
      mStabalizationPercent = aStabalizePercent;
      }


    // Cheesy little wrapper calls Java like main method from static thread function.
    static
    void *
    StaticPhysicalThreadMain( void *arg )
      {
      return( ((Monitor *)arg)->PhysicalThreadMain( NULL ) );
      }


    void
    SetFinishedFlagPtr( unsigned *aFinishedFlag )
      {
      mFinishedFlag = aFinishedFlag;
      }

    void *
    PhysicalThreadMain( void * arg )
      {
      mStartedFlag = 1;
      double TotalEvents = 0;

      mResults = new unsigned[mIterationCount];
      assert( mResults != NULL );

      unsigned CounterLast = *mCounterPtr;

      for ( int iter = (0 - mIgnoreIterationCount) ; iter < mIterationCount ; iter++ )
        {
        usleep( mMicroSecondsPerIteration ) ;
        unsigned CounterNow = *mCounterPtr;

        if( iter >= 0 )
          {

          double Events = mResults[iter] = (CounterNow - CounterLast) ;

          double permsg  = mMicroSecondsPerIteration / Events ;

          printf("Events %14f, Events/Second %14f, uSecs/Event %14f\n",
                  Events, Events * 1000000.0/mMicroSecondsPerIteration, permsg );

          TotalEvents += Events;
          double DevFromAve = fabs((TotalEvents / iter) - Events );
          if( (Events / DevFromAve / 100 ) > mStabalizationPercent  )
            {
            TotalEvents = 0;
            iter = 0;
            }
          }

        CounterLast = CounterNow;
        }
      if( mFinishedFlag != NULL )
        *mFinishedFlag = 1;
      return( NULL );
      }

  };

#endif
