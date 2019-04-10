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
 // **********************************************************************
//
// File: autocorrelation.cpp
// Author: RSG
// Date: January 24, 2008
// Class: AutoCorrelation
// Description: Computes autocorrelation of a time series expressed as
//              an array of doubles by brute force lag product
//              computation distributed over p processors.
//              The lag products will only be computed up to some
//              maximum lag specified on the command line.  The mean
//              of the input data will be subtracted prior to
//              computation of the autocorrelation.
//
// **********************************************************************

#ifndef PKFXLOG_DEBUG
#define PKFXLOG_DEBUG ( 0 )
#endif

#include <BlueMatter/autocorrelation.hpp>
#include <BlueMatter/barrier.hpp>
#include <pk/fxlogger.hpp>

#include <pthread.h>
#include <iostream>
#include <cstdlib>
//#include <sys/systemcfg.h>
#define DCACHE_LINE_SIZE 128

static void* lagThread( void* ); // will be passed pointer to
                                 // AutoCorrelationData struct

static AutoCorrelation::TDouble 
computeSumAtLag( const long,                                            // lag
                 std::vector<AutoCorrelation::TDouble>::const_iterator, // begin input data
                 std::vector<AutoCorrelation::TDouble>::const_iterator, // end input data
                 const AutoCorrelation::TDouble,                        // mean
                 const AutoCorrelation::TDouble );                      // inverse of variance

struct AutoCorrelationData
{
  pthread_mutex_t d_meanMutex;
  AutoCorrelation::TDouble d_meanSum;
  long d_meanCount;
  pthread_mutex_t d_varianceMutex;
  AutoCorrelation::TDouble d_varianceSum;
  long d_varianceCount;
  Barrier d_meanBarrier;
  Barrier d_varianceBarrier;
  Barrier d_correlationBarrier;
  const long d_maxLag;
  const int d_chunkSize; // want it in units of sizeof(TDouble) now
  const int d_threadCount;
  std::vector<AutoCorrelation::TDouble>::const_iterator d_first; // begin input data
  std::vector<AutoCorrelation::TDouble>::const_iterator d_last;  // end input data
  AutoCorrelation::TDouble* d_cxx;
  // methods
  AutoCorrelationData(const long,                      // maxLag (output)
                      const int,                       // chunk size
                      const int,                       // thread count
                      std::vector<AutoCorrelation::TDouble>::const_iterator, // begin input data
                      std::vector<AutoCorrelation::TDouble>::const_iterator, // begin input data
                      AutoCorrelation::TDouble*);       // output array
  ~AutoCorrelationData();
};

struct AutoCorrelationThreadData
{
  AutoCorrelationData* d_data;
  int d_threadNo;
};

void* lagThread( void* inputData_p )
{
  AutoCorrelationThreadData* threadData =
    *reinterpret_cast<AutoCorrelationThreadData**>( inputData_p );
  const int threadNo = threadData->d_threadNo;
  AutoCorrelationData* aData = threadData->d_data;
  const int threadCount = aData->d_threadCount;
  std::vector<AutoCorrelation::TDouble>::const_iterator x = aData->d_first; // begin input data

  const int chunkSize = aData->d_chunkSize;
  long size = aData->d_last - aData->d_first;
  int maxChunk = size/chunkSize;
  maxChunk = size % aData->d_chunkSize == 0 ? maxChunk : maxChunk + 1;

  BegLogLine( PKFXLOG_DEBUG )
    << "chunkSize:" << chunkSize << " "
    << "size:" << size << " "
    << "maxChunk:" << maxChunk << " "
    << EndLogLine;

  // first pass computes the mean
  AutoCorrelation::TDouble sum = 0;
  int count = 0;
  {
    int i = 0;
    for ( i = threadNo; i < maxChunk - 1; i += threadCount )
      {
        const int base = i*chunkSize;
        for ( int j = base; j < base + chunkSize; ++j )
          {
            sum += x[j];
            ++count;
          }
      }

    const int base = i*chunkSize;
    for ( int j = base;
          ( j < base + chunkSize ) && ( j < size );
          ++j )
      {
        sum += x[j];
        ++count;
      }
  }

  BegLogLine ( PKFXLOG_DEBUG )
    << "Contribution from Thread " << threadNo << " to meanSum: "
    << sum << " with meanCount: " << count
    << EndLogLine;

  // Accumulate into shared variables
  pthread_mutex_lock( &aData->d_meanMutex );
  aData->d_meanSum += sum;
  aData->d_meanCount += count;
  pthread_mutex_unlock( &aData->d_meanMutex );

  aData->d_meanBarrier(); // wait for everyone to finish this step

  if ( aData->d_meanCount != size )
    {
      BegLogLine( PKFXLOG_DEBUG )
        << "Thread Number: " << threadNo << " found meanCount = "
        << aData->d_meanCount << " != " << size
        << EndLogLine;
      std::exit( -1 );
    }
  const AutoCorrelation::TDouble mean = 
    aData->d_meanSum/static_cast<AutoCorrelation::TDouble>( aData->d_meanCount );

  // Second pass computes variance
  sum = 0;
  count = 0;
  {
    int i = 0;
    for ( i = threadNo; i < maxChunk - 1; i += threadCount )
      {
        const int base = i*chunkSize;
        for ( int j = base; j < base + chunkSize; ++j )
          {
            AutoCorrelation::TDouble dev = x[j] - mean;
            sum += dev*dev;
            ++count;
          }
      }

    const int base = i*chunkSize;
    for ( int j = base;
          ( j < base + chunkSize ) && ( j < size );
          ++j )
      {
        AutoCorrelation::TDouble dev = x[j] - mean;
        sum += dev*dev;
        ++count;
      }
  }

  BegLogLine ( PKFXLOG_DEBUG )
    << "Contribution from Thread " << threadNo << " to varianceSum: "
    << sum << " with varianceCount: " << count
    << EndLogLine;

  // Accumulate into shared variables
  pthread_mutex_lock( &aData->d_varianceMutex );
  aData->d_varianceSum += sum;
  aData->d_varianceCount += count;
  pthread_mutex_unlock( &aData->d_varianceMutex );

  aData->d_varianceBarrier(); // wait for everyone to finish this step

  if ( aData->d_varianceCount != size )
    {
      BegLogLine( PKFXLOG_DEBUG )
        << "Thread Number: " << threadNo << " found varianceCount = "
        << aData->d_varianceCount << " != " << size
        << EndLogLine;
      std::exit( -1 );
    }
  const AutoCorrelation::TDouble variance = 
    aData->d_varianceSum/static_cast<AutoCorrelation::TDouble>( aData->d_varianceCount );

  const AutoCorrelation::TDouble invVar = 1.0/variance;

  // Third pass is the autocorrelation calculation
  // We divide up the output array amongst the threads

  maxChunk = aData->d_maxLag/chunkSize;
  maxChunk = 
    aData->d_maxLag % aData->d_chunkSize == 0 ? maxChunk : maxChunk + 1;

  sum = 0;
  count = 0;
  {
    int i = 0;
    for ( i = threadNo; i < maxChunk - 1; i += threadCount )
      {
        const int base = i*chunkSize;
        for ( int j = base; j < base + chunkSize; ++j )
          {
            aData->d_cxx[j] = 
              computeSumAtLag( j, aData->d_first, aData->d_last, mean, invVar);
            ++count;
          }
      }

    const int base = i*threadCount;
    for ( int j = base;
          ( j < base + chunkSize ) && ( j < aData->d_maxLag );
          ++j )
      {
        aData->d_cxx[j] = 
          computeSumAtLag( j, aData->d_first, aData->d_last, mean, invVar);
        ++count;
      }
  }
  aData->d_correlationBarrier();
  pthread_exit( NULL );
  return( NULL );
}

template <typename T>
class pad
{
private:
  static long pad_size;
public:
  static const long size();
  static T* cache_line( T* );
};


AutoCorrelation::AutoCorrelation() : d_cxx( NULL ),
                                     d_cxxUnalignedBase_p( NULL ),
                                     d_window( 0 ),
                                     d_chunkSize( DCACHE_LINE_SIZE )
{}

AutoCorrelation::~AutoCorrelation()
{
  if ( d_cxxUnalignedBase_p != NULL )
    {
      delete [] d_cxxUnalignedBase_p;
      d_cxxUnalignedBase_p = NULL;
      d_cxx = NULL;
    }
}


void AutoCorrelation::init( const long window ) // window width (maximum lag)
{
  if ( window < 1 )
    {
      BegLogLine( PKFXLOG_DEBUG )
        << "AutoCorrelation::init() : window must be positive"
        << EndLogLine;
      std::exit( -1 );
    }
 
  if ( d_chunkSize % sizeof( TDouble ) != 0 )
    {
      BegLogLine( PKFXLOG_DEBUG )
        << "AutoCorrelation::init(): sizeof( TDouble ) = "
        << sizeof( TDouble ) << " and chunk size = "
        << d_chunkSize << " are incommensurate"
        << EndLogLine;
      std::exit( -1 );
    }

 d_window = window;
  // pad allocation to enable cache line alignment
  d_cxxUnalignedBase_p = new TDouble[ d_window + pad<TDouble>::size() ];
  if ( d_cxxUnalignedBase_p == NULL )
    {
      BegLogLine( PKFXLOG_DEBUG )
        << "AutoCorrelation::init() : Unable to allocate memory for output array"
        << EndLogLine;
      std::exit( -1 );
    }
  // compute new base for array with cache line alignment
  d_cxx = pad<TDouble>::cache_line( d_cxxUnalignedBase_p );
}

const AutoCorrelation::ReturnData
  AutoCorrelation::compute( std::vector<AutoCorrelation::TDouble>::const_iterator first,
                            std::vector<AutoCorrelation::TDouble>::const_iterator last,
                            int threads )
{
  AutoCorrelationData aData( d_window,
                             d_chunkSize,
                             threads,
                             first,
                             last,
                             d_cxx);
  AutoCorrelationThreadData* aThreadData = 
    new AutoCorrelationThreadData[threads];
  if ( aThreadData == NULL )
    {
      BegLogLine( PKFXLOG_DEBUG )
      << "AutoCorrelation::operator(): Unable to allocate memory for AutoCorrelationThreadData array"
      << EndLogLine;
      std::exit( -1 );
    }
  typedef AutoCorrelationThreadData* AutoCorrelationThreadDataPtr;
  AutoCorrelationThreadDataPtr* aThreadData_p = 
    new AutoCorrelationThreadDataPtr[threads];
  if ( aThreadData_p == NULL )
    {
      BegLogLine( PKFXLOG_DEBUG )
        << "AutoCorrelation::operator(): Unable to allocate memory for AutoCorrelationThreadData* array"
        << EndLogLine;
      std::exit( -1 );
    }
  // set-up thread attributes
  pthread_attr_t* a = new pthread_attr_t[threads];
  if ( a == NULL )
    {
      BegLogLine( PKFXLOG_DEBUG )
        << "AutoCorrelation::operator(): Unable to allocate memory for array of thread attributes"
        << EndLogLine;
      std::exit( -1 );
    }
  pthread_t* thread = new pthread_t[threads];
  if ( thread == NULL )
    {
      BegLogLine( PKFXLOG_DEBUG )
        << "AutoCorrelation::operator(): Unable to allocate memory for array of threads"
        << EndLogLine;
      std::exit( -1 );
    }

  for ( int i = 0; i < threads; ++i )
    {
      pthread_attr_init( a + i );
      pthread_attr_setscope( a + i, PTHREAD_SCOPE_SYSTEM );
      aThreadData[i].d_data = &aData;
      aThreadData[i].d_threadNo = i;
      aThreadData_p[i] = aThreadData + i;
      int rc = pthread_create( thread + i, a + i, lagThread, aThreadData_p + i );
      if ( rc != 0 )
        { 
          BegLogLine( PKFXLOG_DEBUG )
            << "AutoCorrelation::operator(): Error starting thread "
            << i << ". RC = " << rc
            << EndLogLine;
          std::exit(-1);
        }
    }
  // just wait for everyone to finish
  for ( int i = 0; i < threads; ++i )
    {
      pthread_join( thread[i], NULL );
    }

  ReturnData ret;
  ret.d_mean = aData.d_meanSum/static_cast<AutoCorrelation::TDouble>( aData.d_meanCount );
  ret.d_variance = aData.d_varianceSum/static_cast<AutoCorrelation::TDouble>( aData.d_varianceCount );
  ret.d_maxLag = d_window;
  ret.d_autocorrelation = d_cxx;
  // should clean up all the dynamically allocated memory and other resources here
  
  return( ret );
}


template <typename T>
const long pad<T>::size()
{
  return( pad_size );
}

template <typename T>
T* pad<T>::cache_line( T* base )
{
  int offset = ( 128 - ( reinterpret_cast<unsigned long>( base ) % DCACHE_LINE_SIZE ) )/sizeof( T );
  return( base + offset );
}

template <typename T>
long pad<T>::pad_size = DCACHE_LINE_SIZE/sizeof( T );

AutoCorrelationData::AutoCorrelationData(const long maxLag,
                                         const int chunkSize,
                                         const int threadCount,
                                         std::vector<AutoCorrelation::TDouble>::const_iterator first,
                                         std::vector<AutoCorrelation::TDouble>::const_iterator last,
                                         AutoCorrelation::TDouble* cxx)
  : d_maxLag( maxLag ),
    d_first( first ),
    d_last( last ),
    d_chunkSize( chunkSize / sizeof( AutoCorrelation::TDouble ) ),
    d_threadCount( threadCount ),
    d_cxx( cxx ),
    d_meanBarrier( threadCount ),
    d_varianceBarrier( threadCount ),
    d_correlationBarrier( threadCount ),
    d_meanSum( 0 ),
    d_meanCount( 0 ),
    d_varianceSum( 0 ),
    d_varianceCount( 0 )
{
  pthread_mutex_init( &d_meanMutex, NULL );
  pthread_mutex_init( &d_varianceMutex, NULL );
}

AutoCorrelationData::~AutoCorrelationData()
{
  pthread_mutex_destroy( &d_meanMutex );
  pthread_mutex_destroy( &d_varianceMutex );
}


AutoCorrelation::TDouble computeSumAtLag( const long lag,
                                          std::vector<AutoCorrelation::TDouble>::const_iterator first,
                                          std::vector<AutoCorrelation::TDouble>::const_iterator last,
                                          const AutoCorrelation::TDouble mean,
                                          const AutoCorrelation::TDouble invVar)
{
  long size = last - first;
  std::vector<AutoCorrelation::TDouble>::const_iterator x = first;
  AutoCorrelation::TDouble sum = 0;
  for ( long i = 0; i < size - lag; ++i )
    {
      sum += ( x[i] - mean )*( x[i + lag] - mean );
    }
  if ( size - lag > 0 )
    {
      return( sum*invVar/static_cast<AutoCorrelation::TDouble>( size - lag ) );
    }
  return( 0 );
}
