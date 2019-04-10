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
 #ifndef BMM_RDG_EmitWrappers_hpp
#define BMM_RDG_EmitWrappers_hpp

#include <BlueMatter/ExternalDatagram.hpp>

#define EXPECTED_PAYLOAD_SUM ( 0xffffffff >> 1 ) // max unsigned int divided by 2
#define UDF_PAYLOAD          (  EXPECTED_PAYLOAD_SUM / Platform::Topology::GetAddressSpaceCount() )

class EnergyManagerIF_T
  {
  public:

  int     mEnergyCount;
  int     mTotalUDFCount;
  int     mAllocatedEnergyCount;

  int*    mUdfCodesToEmit;
  int*    mTimeStepToEmit;

  double* mEnergyToEmit;
  double* mEnergyToEmitGlobal;            

  long long* mEnergyToEmitLL;
  long long* mEnergyToEmitGlobalLL;

  void Init( int aUdfCount, int aFlushPeriod )
    {
    mTotalUDFCount = aUdfCount;
    
    mAllocatedEnergyCount = aFlushPeriod * mTotalUDFCount;

    mUdfCodesToEmit = (int *) malloc( sizeof( int ) * mAllocatedEnergyCount );
    if( mUdfCodesToEmit == NULL )
      PLATFORM_ABORT( "mUdfCodesToEmit == NULL");

    mTimeStepToEmit = (int *) malloc( sizeof( int ) * mAllocatedEnergyCount );
    if( mTimeStepToEmit == NULL )
      PLATFORM_ABORT( "mTimeStepToEmit == NULL");

    mEnergyToEmit = (double *) malloc( sizeof( double ) * mAllocatedEnergyCount );
    if( mEnergyToEmit == NULL )
      PLATFORM_ABORT( "mEnergyToEmit == NULL");

    mEnergyToEmitGlobal = (double *) malloc( sizeof( double ) * mAllocatedEnergyCount );
    if( mEnergyToEmitGlobal == NULL )
      PLATFORM_ABORT( "mEnergyToEmitGlobal == NULL");

    mEnergyToEmitLL = (long long *) malloc( sizeof( long long ) * mAllocatedEnergyCount );
    if( mEnergyToEmitLL == NULL )
      PLATFORM_ABORT( "mEnergyToEmitLL == NULL");

    mEnergyToEmitGlobalLL = (long long *) malloc( sizeof( long long ) * mAllocatedEnergyCount );
    if( mEnergyToEmitGlobalLL == NULL )
      PLATFORM_ABORT( "mEnergyToEmitGlobalLL == NULL");

    mEnergyCount = 0;
    }
  
  void AddEnergyToEmit( int aTimeStep, int aUdfCode, double aEnergy )
    {
    if( mEnergyCount >= mAllocatedEnergyCount )
      PLATFORM_ABORT( "mEnergyCount >= mAllocatedEnergyCount" );

    mUdfCodesToEmit[ mEnergyCount ] = aUdfCode;
    mEnergyToEmit[ mEnergyCount ] = aEnergy;
    mTimeStepToEmit[ mEnergyCount ] = aTimeStep;

    mEnergyCount++;
    }
      
  void ReduceAndEmitEnergies(int aTimeStep )     
    {
    if( mEnergyCount == 0 )
      return;
    
    double SCALING_FACTOR = 1E12;
    
    for( int i = 0; i < mEnergyCount; i ++ )
      {
      mEnergyToEmitLL[ i ] = (long long) ( mEnergyToEmit[ i ] * SCALING_FACTOR );
      }

    Platform::Collective::LONGLONG_Reduce_Sum( mEnergyToEmitLL, mEnergyToEmitGlobalLL, mEnergyCount, 0 );
    
    for( int i = 0; i < mEnergyCount; i ++ )
      {
      mEnergyToEmitGlobal[ i ] = (double) ( mEnergyToEmitGlobalLL[ i ] / SCALING_FACTOR );
      }

    if( Platform::Topology::GetAddressSpaceId() == 0 )
      {
      for( int i=0; i < mEnergyCount; i++ )
        {
        ED_Emit_UDFd1( mTimeStepToEmit[ i ], mUdfCodesToEmit[ i ], EXPECTED_PAYLOAD_SUM, mEnergyToEmitGlobal[ i ] );
        }
      }
    
    mEnergyCount = 0;
    }
  };

extern EnergyManagerIF_T EnergyManagerIF;
#if !defined(LEAVE_SYMS_EXTERN)
EnergyManagerIF_T EnergyManagerIF;
#endif

#ifdef PK_BLADE_SPI
#include <BlueMatter/BMM_RDG_blade_spi.hpp>
#else
#include <BlueMatter/BMM_RDG_MPI_SendToNodeZero.hpp>
#endif

inline
void
EmitEnergy(unsigned int aTimeStep, unsigned int aUdfCode, double aEnergy, int aUniContext)
  {
  #ifdef RDG_TO_DEVNULL
    return;
  #endif

  #ifndef PK_PARALLEL
  aUniContext = 1;
  #endif

  if( aUniContext )
    ED_Emit_UDFd1( aTimeStep, aUdfCode, EXPECTED_PAYLOAD_SUM, aEnergy );
  else
    {
    // Store away the energy to be vector reduced later.
    EnergyManagerIF.AddEnergyToEmit( aTimeStep, aUdfCode, aEnergy );
    }
  

  return;
  }

/**********************************************************************/
#if defined( NO_TIME_STAMPS )
#define EmitTimeStamp(TimeStep, TypeTag, ToEmit)   {}
#else
#define EmitTimeStamp(TimeStep, TypeTag, ToEmit)           \
 if( ToEmit )                                              \
 if( Platform::Topology::GetAddressSpaceId() == 0 ) \
   {                                                       \
   TimeValue TimeNow = Platform::Clock::GetTimeValue();    \
   ED_Emit_ControlTimeStamp(TimeStep, TypeTag, Platform::Topology::GetAddressSpaceId(), \
                            TimeNow.Seconds, TimeNow.Nanoseconds );                     \
   }
#endif
#endif
