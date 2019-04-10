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
 #ifndef __LOCAL_FRAGMENT_DIRECT_DRIVER_HPP__
#define __LOCAL_FRAGMENT_DIRECT_DRIVER_HPP__

#ifndef PKFXLOG_LOCALFRAGMENTDIRECTDRIVER
#define PKFXLOG_LOCALFRAGMENTDIRECTDRIVER ( 0 )
#endif

class LDFDD_NULL_UDF
    : public UDF_Base
  {
  public:
  template<class MDVM>
  static
  void
  Execute( MDVM mdvm )
    {
    }
  };

template< class UDF0, class UDF1=LDFDD_NULL_UDF, class UDF2=LDFDD_NULL_UDF >
class LocalFragmentDirectDriver
  {
  public:
    void
    Init()
      {          
      }
  
    template<class DynVarIF>
    static
    void
    Execute(DynVarIF &mDynVarIF, int aSimTick, int aRespaLevel )
    {
    static TraceClient LFTDStart;
    static TraceClient LFTDFinis;
      
    // TimeValue StartTime = Platform::Clock::GetTimeValue();
    EmitTimeStamp(aSimTick, Tag::LocalFragmentDirectDriver_Begin, 1 );
      
    LFTDStart.HitOE( PKTRACE_LOCAL_FRAGMENT_DIRECT_DRIVER,
                     UDF0::UdfName,
                     Platform::Topology::GetAddressSpaceId(),
                     LFTDStart );
      
      
    BegLogLine( PKFXLOG_LOCALFRAGMENTDIRECTDRIVER )
      << "LocalFragmentDirectDriver "
      << " BEG "
      << " UDF0: "
      << UDF0::UdfName
      << " UDF1: "
      << UDF1::UdfName
      << " UDF2: "
      << UDF2::UdfName
      << EndLogLine;

    DirectMDVM< DynVarIF > mdvm( mDynVarIF );

    mdvm.Setup( aSimTick, aRespaLevel );

    if( UDF0::CalculatesEnergy )
      {
      mdvm.ZeroEnergy( 0 );        
      }

    if( UDF1::CalculatesEnergy )
      {
      mdvm.ZeroEnergy( 1 );
      }

    if( UDF2::CalculatesEnergy )
      {
      mdvm.ZeroEnergy( 2 );
      }

    int NumberOfFragments = mdvm.GetNumberOfFragments();

    for( int i = 0; i < NumberOfFragments; i++ )
      {         
      #if ( defined( PK_PHASE1_5 ) )
      if( !mDynVarIF.IsFragmentLocal( i ) )
        continue;
      #endif

      mdvm.SetFragmentId( i );
      mdvm.SetNonsharedSelect( 0 );
      UDF0::Execute( mdvm );
      mdvm.SetNonsharedSelect( 1 );
      UDF1::Execute( mdvm );
      mdvm.SetNonsharedSelect( 2 );
      UDF2::Execute( mdvm );
      }

    int CurrentTS = aSimTick /  MSD::SimTicksPerTimeStep;
    if( UDF0::ReportsEnergy && (aSimTick % MSD::SimTicksPerTimeStep == 0) )
      {
      if( CurrentTS % RTG.mEmitEnergyTimeStepModulo == 0 )
        EmitEnergy( CurrentTS, UDF0::Code, mdvm.GetEnergy( 0 ), 0 );
      }

    if( UDF1::ReportsEnergy && (aSimTick % MSD::SimTicksPerTimeStep == 0) )
      {
      if( CurrentTS % RTG.mEmitEnergyTimeStepModulo == 0 )
        EmitEnergy( CurrentTS, UDF1::Code, mdvm.GetEnergy( 1 ), 0 );
      }

    if( UDF2::ReportsEnergy && (aSimTick % MSD::SimTicksPerTimeStep == 0) )
      {
      if( CurrentTS % RTG.mEmitEnergyTimeStepModulo == 0 )
        EmitEnergy( CurrentTS, UDF2::Code, mdvm.GetEnergy( 2 ), 0 );
      }

    BegLogLine(PKFXLOG_LOCALFRAGMENTDIRECTDRIVER)
      << "LocalFragmentDirectDriver "
      << " END "
      << " UDF0: "
      << UDF0::UdfName
      << " UDF1: "
      << UDF1::UdfName
      << " UDF2: "
      << UDF2::UdfName
      << EndLogLine;

    EmitTimeStamp(aSimTick, Tag::LocalFragmentDirectDriver_End, 1 );
    LFTDFinis.HitOE( PKTRACE_LOCAL_FRAGMENT_DIRECT_DRIVER,
                     UDF0::UdfName,
                     Platform::Topology::GetAddressSpaceId(),
                     LFTDFinis );
    }
};
#endif
