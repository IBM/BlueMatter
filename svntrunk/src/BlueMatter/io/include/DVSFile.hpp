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
 #ifndef _DVSFILES_HPP_
#define _DVSFILES_HPP_

// This file contains routines to read a DVS file.
// A DVS file is a file that contains the complete
// Dynamic VARIABLE SET for the timestep requested.
// The information in the file is in the format
// described for the ExternalDatagram interface.

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/BoundingBox.hpp>

#ifndef PKFXLOG_DVSFILE
#define PKFXLOG_DVSFILE ( 0 )
#endif

#ifndef PKFXLOG_ROTATE_SIMULATION
#define PKFXLOG_ROTATE_SIMULATION (0)
#endif

#ifndef PKFXLOG_THERMOSTAT
#define PKFXLOG_THERMOSTAT (0)
#endif

extern char dvsFilePtr[];
extern int dvsFileSize;

static
void
RotateSimulation(int aNumberOfSites)
{
  iXYZ m;
  Platform::Topology::GetDimensions(&m.mX, &m.mY, &m.mZ);

  // Make absolutely sure the boxes have positive dimension
  RTG.mBirthBoundingBoxDimensionVector.mX = fabs(RTG.mBirthBoundingBoxDimensionVector.mX);
  RTG.mBirthBoundingBoxDimensionVector.mY = fabs(RTG.mBirthBoundingBoxDimensionVector.mY);
  RTG.mBirthBoundingBoxDimensionVector.mZ = fabs(RTG.mBirthBoundingBoxDimensionVector.mZ);

  RTG.mStartingBoundingBoxDimensionVector.mX = fabs(RTG.mStartingBoundingBoxDimensionVector.mX);
  RTG.mStartingBoundingBoxDimensionVector.mY = fabs(RTG.mStartingBoundingBoxDimensionVector.mY);
  RTG.mStartingBoundingBoxDimensionVector.mZ = fabs(RTG.mStartingBoundingBoxDimensionVector.mZ);

  RTG.mBoxIndexOrder = RTG.mBirthBoundingBoxDimensionVector.MatchOrder(m);

  BegLogLine(PKFXLOG_ROTATE_SIMULATION)
    << "Machine " << m.mX
    << " " << m.mY
    << " " << m.mZ
    << EndLogLine ;
  
  BegLogLine(PKFXLOG_ROTATE_SIMULATION)
    << "Box " << RTG.mBirthBoundingBoxDimensionVector.mX
    << " " << RTG.mBirthBoundingBoxDimensionVector.mY
    << " " << RTG.mBirthBoundingBoxDimensionVector.mZ
    << EndLogLine ;
  
  BegLogLine(PKFXLOG_ROTATE_SIMULATION)
    << "BoxOrder " << RTG.mBoxIndexOrder.mX
    << " " << RTG.mBoxIndexOrder.mY
    << " " << RTG.mBoxIndexOrder.mZ
    << EndLogLine ;
  
  for (int i=0; i<aNumberOfSites; i++)
  {
    StartingPositions[i].ReOrder(RTG.mBoxIndexOrder);
    StartingVelocities[i].ReOrder(RTG.mBoxIndexOrder);
  }

  RTG.mBirthBoundingBoxDimensionVector.ReOrderAbs(RTG.mBoxIndexOrder);
  RTG.mStartingBoundingBoxDimensionVector.ReOrderAbs(RTG.mBoxIndexOrder);
}

template<class DynVarMgr>
void
ReadDVSFile( int          aTimeStepNo,
             int          aNumberOfSites,
             char        *aFileName,
             const BoundingBox &aCheckBoundingBox,
             DynVarMgr    &aDynVarMgr )
  {
  BegLogLine( PKFXLOG_DVSFILE  )
    << "ReadInitialConditions() "
    << " Expecting SiteCount "
    << aNumberOfSites
    << "Input File : "
    << aFileName
    << EndLogLine;

  char * StartingPosVelCheckList; // = new char[ aNumberOfSites ];
  pkNew( &StartingPosVelCheckList, aNumberOfSites, __FILE__, __LINE__ );  

  for(int i = 0; i < aNumberOfSites; i++ )
    StartingPosVelCheckList[ i ] = 0;

  BoundingBox aBoundingBox;

#if !defined(DVS_BUILT_IN)
  int Fd = -1;
  Fd = open( aFileName, O_RDONLY );

  if ( Fd < 0 )
    {
    PLATFORM_ABORT("DVS File not opened successfully.");
    }
#else
  char* dvsFile = (char * ) dvsFilePtr; 
#endif


  int SelectTS = -99; // NOTE: magic -99 is a flag used below.
  if( aTimeStepNo != -1 )
    SelectTS = aTimeStepNo;

  int ScanedTS = -1;
  int FirstInTS = 1;

  char Record[ 1024 ];
  ED_Header Hdr;
  int BytesRead = 0;

  int isBirthBoxPresentInDvs = 0;
  int isRanNumGenStatePresent = 0;
  while( 1 )
    {

#if defined(DVS_BUILT_IN)
    if( dvsFileSize == BytesRead )
      break;
    
    Packet::load( &dvsFile, Record, Hdr, &BytesRead );
#else
    if ( Packet::load( Fd, Record, Hdr) != 0 )
        break;
#endif

    // Assert that the record is of right class.
    switch( Hdr.mClass )
      {
      case ED_CLASS::UDF:
        {
        switch( Hdr.mType )
          {
          case ED_UDF_TYPES::d1 :
            {
            ED_UDFd1 udf;

            char * RecordPtr = &Record[0];
            ED_OpenPacket_UDFd1( RecordPtr, udf );

            switch( udf.mCode )
              {
              case UDF_Binding::ResampleEnergyLoss_Code:
                {
                 BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_UDFd1 packet to reset GlobalResampleEnergyLoss from "
                      << RTG.mGlobalResampleEnergyLoss
                      << " to "
                      << udf.mValue
                      << EndLogLine;

                //  Need to divide by number of nodes since each node will be outputting its piece
                RTG.mGlobalResampleEnergyLoss = udf.mValue/Platform::Topology::GetAddressSpaceCount();
                break;
                }
              default:
                {
                BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile Control packet contains ED_UDF_TYPES::d1 with udf code " <<  udf.mCode << " unrecognized and ignored "<< EndLogLine;
                break;
                }
              }
            break;
            }
            default:
              {
              BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile Control packet contains TYPE " <<  Hdr.mType << " unrecognized and ignored "<< EndLogLine;
              break;
              }
          }
        break;
        }
      case ED_CLASS::Control :
        {
        switch( Hdr.mType )
          {
          case ED_CONTROL_TYPES::SyncId :
            {
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile Control packet contains SyncId " << EndLogLine;
            ED_ControlSyncId dvs;
            char * RecordPtr = &Record[0];
            ED_OpenPacket_ControlSyncId( RecordPtr, dvs );
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_ControlSyncId packet to reset StartingTimeStep from "
                 << RTG.mStartingTimeStep
                 << " to "
                 << dvs.mFullOuterTimeStep
                 << EndLogLine;
            RTG.mStartingTimeStep = dvs.mFullOuterTimeStep;
            break;
            }
          case ED_CONTROL_TYPES::Pressure :
            {
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile Control packet contains Pressure " << EndLogLine;
            ED_ControlPressure dvs;
            char * RecordPtr = &Record[0];
            ED_OpenPacket_ControlPressure( RecordPtr, dvs );

            // Get out the VolumeVelocity
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_ControlPressure packet to set MSD::PressureControlPistonInitialVelocity from "
                 << RTG.mPressureControlPistonInitialVelocity
                 << " to "
                 << dvs.mVolumeVelocity.mX
                 << EndLogLine;

            #ifdef MSD_RTP_PressureControlPistonInitialVelocity_NONCONST
              RTG.mPressureControlPistonInitialVelocity = dvs.mVolumeVelocity.mX;
            #else
              PLATFORM_ABORT("DVS RTP Packet ED_ControlPressure.mVolumeVelocity implies assignment of const RTP value.");
            #endif

            break;
            }
          default :
            {
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile Control packet contains TYPE " <<  Hdr.mType << " unrecognized and ignored "<< EndLogLine;
            break;
            } // End of case default :
          }
        break;
        }
      case ED_CLASS::Information:
        {
        switch( Hdr.mType )
          {
          case ED_INFORMATION_TYPES::RTP :
            {
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile Got ED_INFORMATION_TYPES::RTP packet."<< EndLogLine;
            ED_InformationRTP dvs;
            char * RecordPtr = &Record[0];
            ED_OpenPacket_InformationRTP( RecordPtr, dvs );

           // Get out the Bounding Box
            #ifdef MSD_RTP_BoundingBoxDef_NONCONST

              if(dvs.mDynamicBoundingBoxMax == XYZ::ZERO_VALUE() && dvs.mDynamicBoundingBoxMin == XYZ::ZERO_VALUE())
                {
                if( RTG.mStartingBoundingBoxDimensionVector == XYZ::ZERO_VALUE() )
                  PLATFORM_ABORT("DVS ERROR:: BoundingBox specified in both ctp and dvs are both invalid.");
                }
              else
                {

                BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_InformationRTP packet to assign RTG.mStartingBoundingBoxDimensionVector from "
                     << RTG.mStartingBoundingBoxDimensionVector
                     << " to "
                     << dvs.mDynamicBoundingBoxMin
                     << " and "
                     << dvs.mDynamicBoundingBoxMax
                     << EndLogLine;

                BoundingBox tempBox;
                tempBox.mMinBox = dvs.mDynamicBoundingBoxMin;
                tempBox.mMaxBox = dvs.mDynamicBoundingBoxMax;
                RTG.mStartingBoundingBoxDimensionVector = tempBox.GetDimensionVector();
                }
            #else
              PLATFORM_ABORT("DVS RTP Packet ED_Information.RTP.mDynamicBoundingBox implies assignment of const RTP value.");
            #endif

              // Get out VelocityResampleTargetTemperature
            if( dvs.mVelocityResampleTargetTemperature != 0.0 )
              {
              BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_InformationRTP.mVelocityResampleTargetTemperature packet to assign "
                   << "RTG.mVelocityResampleTargetTemperature from "
                   << RTG.mVelocityResampleTargetTemperature
                   << " to "
                   << dvs.mVelocityResampleTargetTemperature
                   << EndLogLine;

              #ifdef MSD_RTP_VelocityResampleTargetTemperature_NONCONST
                RTG.mVelocityResampleTargetTemperature = dvs.mVelocityResampleTargetTemperature;
              #else
                PLATFORM_ABORT("DVS RTP Packet ED_InformationRTP.mVelocityResampleTargetTemperature implies assignment of const RTP value.");
              #endif
              }

            //  Get out SnapshotPeriodInOTS
            if( dvs.mSnapshotPeriodInOTS != 0 )
              {
              BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_InformationRTP.mSnapshotPeriodInOTS packet to assign "
                   << "RTG.mSnapshotPeriodInOTS from "
                   << RTG.mSnapshotPeriodInOTS
                   << " to "
                   << dvs.mSnapshotPeriodInOTS
                   << EndLogLine;

              #ifdef MSD_RTP_SnapshotPeriodInOTS_NONCONST
                RTG.mSnapshotPeriodInOTS = dvs.mSnapshotPeriodInOTS;
              #else
                PLATFORM_ABORT("DVS RTP Packet ED_InformationRTP.mSnapshotPeriodInOTS implies assignment of const RTP value.");
              #endif
              }

#ifndef DEBUG_POINTENERGY
            if( dvs.mNumberOfOuterTimeSteps != 0 )
              {
              BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_InformationRTP.mNumberOfOuterTimeSteps packet to assign "
                   << "RTG.mNumberOfOuterTimeSteps from "
                   << RTG.mNumberOfOuterTimeSteps
                   << " to "
                   << dvs.mNumberOfOuterTimeSteps
                   << EndLogLine;

              #ifdef MSD_RTP_NumberOfOuterTimeSteps_NONCONST
                RTG.mNumberOfOuterTimeSteps = dvs.mNumberOfOuterTimeSteps;
              #else
                PLATFORM_ABORT("DVS RTP Packet ED_InformationRTP.mNumberOfOuterTimeSteps implies assignment of const RTP value.");
              #endif
              }
#else
            RTG.mNumberOfOuterTimeSteps=0;
#endif
            //  Get out PressureControlTarget
            if( dvs.mPressureControlTarget != 0.0 )
              {
              BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_InformationRTP.mPressureControlTarget packet to assign "
                   << "RTG.mPressureControlTarget from "
                   << RTG.mPressureControlTarget
                   << " to "
                   << dvs.mPressureControlTarget
                   << EndLogLine;

              #ifdef MSD_RTP_PressureControlTarget_NONCONST
              RTG.mPressureControlTarget = dvs.mPressureControlTarget;
              #else
                PLATFORM_ABORT("DVS RTP Packet ED_InformationRTP.mPressureControlTarget implies assignment of const RTP value.");
              #endif
              }

            //  Get out VelocityResamplePeriodInOTS
            if( dvs.mVelocityResamplePeriodInOTS != 0 )
              {
              BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_InformationRTP.mVelocityResamplePeriodInOTS packet to assign "
                   << "RTG.mVelocityResamplePeriodInOTS from "
                   << RTG.mVelocityResamplePeriodInOTS
                   << " to "
                   << dvs.mVelocityResamplePeriodInOTS
                   << EndLogLine;

              #ifdef MSD_RTP_VelocityResamplePeriodInOTS_NONCONST
                RTG.mVelocityResamplePeriodInOTS = dvs.mVelocityResamplePeriodInOTS;
              #else
                PLATFORM_ABORT("DVS RTP Packet ED_InformationRTP.mVelocityResamplePeriodInOTS implies assignment of const RTP value.");
              #endif
              }

            break;
            } // End of case ED_INFORMATION_TYPES::RTP
          default :
            {
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile Control packet contains TYPE " <<  Hdr.mType << " unrecognized and ignored "<< EndLogLine;
            break;
            } // End of case default :
          }
        break;
        }
      case ED_CLASS::RTPValues :
        {
        switch( Hdr.mType )
          {
          case ED_RTPVALUES_TYPES::VelocityResampleTargetTemperature :
            {
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile Got ED_RTPVALUES_TYPES::VelocityResampleTargetTemperature packet."<< EndLogLine;
            ED_RTPValuesVelocityResampleTargetTemperature dvs;
            char * RecordPtr = &Record[0];
            ED_OpenPacket_RTPValuesVelocityResampleTargetTemperature( RecordPtr, dvs );
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_RTPValuesVelocityResampleTargetTemperature packet to assign RTG.mVelocityResampleTargetTemperature from "
                 << RTG.mVelocityResampleTargetTemperature
                 << " to "
                 << dvs.mValue
                 << EndLogLine;
            #ifdef MSD_RTP_VelocityResampleTargetTemperature_NONCONST
              RTG.mVelocityResampleTargetTemperature = dvs.mValue;
            #else
              PLATFORM_ABORT("DVS RTP Packet ED_RTPValuesVelocityResampleTargetTemperature implies assignment of const RTP value.");
            #endif
            break;
            } // End of case ED_RTPVALUES_TYPES::VelocityResampleTargetTemperature
          case ED_RTPVALUES_TYPES::VelocityResampleRandomSeed :
            {
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile Got ED_RTPVALUES_TYPES::VelocityResampleRandomSeed packet."<< EndLogLine;
            ED_RTPValuesVelocityResampleRandomSeed dvs;
            char * RecordPtr = &Record[0];
            ED_OpenPacket_RTPValuesVelocityResampleRandomSeed( RecordPtr, dvs );
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_RTPValuesVelocityResampleRandomSeed packet to assign RTG.mVelocityResampleTargetTemperature from "
                 << RTG.mVelocityResampleRandomSeed
                 << " to "
                 << dvs.mValue
                 << EndLogLine;
            #ifdef MSD_RTP_VelocityResampleRandomSeed_NONCONST
              RTG.mVelocityResampleRandomSeed = dvs.mValue;
            #else
              PLATFORM_ABORT("DVS RTP Packet ED_RTPValuesVelocityResampleRandomSeed implies assignment of const RTP value.");
            #endif
            break;
            } // End of case ED_RTPVALUES_TYPES::VelocityResampleRandomSeed
          case ED_RTPVALUES_TYPES::NumberOfOuterTimeSteps :
            {
	    } // End of case  ED_RTPVALUES_TYPES::NumberOfOuterTimeSteps
          case ED_RTPVALUES_TYPES::SnapshotPeriodInOTS :
            {
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile Got  ED_RTPVALUES_TYPES::SnapshotPeriodInOTS packet."<< EndLogLine;
            ED_RTPValuesSnapshotPeriodInOTS dvs;
            char * RecordPtr = &Record[0];
            ED_OpenPacket_RTPValuesSnapshotPeriodInOTS( RecordPtr, dvs );
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_RTPValuesSnapshotPeriodInOTS packet to assign RTG.mSnapshotPeriodInOTS from "
                 << RTG.mSnapshotPeriodInOTS
                 << " to "
                 << dvs.mValue
                 << EndLogLine;
            #ifdef MSD_RTP_SnapshotPeriodInOTS_NONCONST
              RTG.mSnapshotPeriodInOTS = dvs.mValue;
            #else
              PLATFORM_ABORT("DVS RTP Packet ED_RTPValuesSnapshotPeriodInOTS implies assignment of const RTP value.");
            #endif
            break;
            } // End of case ED_RTPVALUES_TYPES::SnapshotPeriodInOTS
          case ED_RTPVALUES_TYPES::EmitEnergyTimeStepModulo :
            {
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile Got  ED_RTPVALUES_TYPES::EmitEnergyTimeStepModulo packet."<< EndLogLine;
            ED_RTPValuesEmitEnergyTimeStepModulo dvs;
            char * RecordPtr = &Record[0];
            ED_OpenPacket_RTPValuesEmitEnergyTimeStepModulo( RecordPtr, dvs );
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile using ED_RTPValuesEmitEnergyTimeStepModulo packet to assign RTG.mEmitEnergyTimeStepModulo from "
                 << RTG.mEmitEnergyTimeStepModulo
                 << " to "
                 << dvs.mValue
                 << EndLogLine;
            #ifdef MSD_RTP_EmitEnergyTimeStepModulo_NONCONST
              RTG.mEmitEnergyTimeStepModulo = dvs.mValue;
            #else
              PLATFORM_ABORT("DVS RTP Packet ED_RTPValuesEmitEnergyTimeStepModulo implies assignment of const RTP value.");
            #endif
            break;
            } // End of  case ED_RTPVALUES_TYPES::EmitEnergyTimeStepModulo
          default :
            {
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile RTPValues packet contains TYPE " <<  Hdr.mType << " unrecognized and ignored "<< EndLogLine;
            break;
            } // End of case default :
          } // End of switch( Hdr.mType )
        break;
        } // End of case ED_CLASS::RTPValues :
      case ED_CLASS::DynamicVariables :
        {
        switch( Hdr.mType )
          {
          case ED_DYNAMICVARIABLES_TYPES::RandomState :
            {                
            ED_DynamicVariablesRandomState dvs;
            char * RecordPtr = &Record[0];
            ED_OpenPacket_DynamicVariablesRandomState( RecordPtr, dvs);    

            NormalDeviate::state tempState;

            if( dvs.mSeed != 0 )                
                RTG.mVelocityResampleRandomSeed = dvs.mSeed;

            if( dvs.mX0_N0 == 0 && 
                dvs.mX0_N1 == 0 &&
                dvs.mX0_N2 == 0 &&
                dvs.mX1_N0 == 0 &&
                dvs.mX1_N1 == 0 &&
                dvs.mX1_N2 == 0 &&
                dvs.mDummy4 == 0 &&
                dvs.mDummy5 == 0 &&
                dvs.mDummy6 == 0 &&
                dvs.mDummy7 == 0 &&
                dvs.mDummy8 == 0 &&
                dvs.mDummy9 == 0 &&
                dvs.mDummy10 == 0 &&
                dvs.mDummy11 == 0 &&
                dvs.mDummy12 == 0 &&
                dvs.mDummy13 == 0 &&
                dvs.mDummy14 == 0 &&
                dvs.mDummy15 == 0 &&
                dvs.mDummy16 == 0 &&
                dvs.mDummy17 == 0 &&
                dvs.mDummy18 == 0 &&
                dvs.mDummy19 == 0 &&
                dvs.mDummy20 == 0 &&
                dvs.mDummy21 == 0 &&
                dvs.mDummy22 == 0 &&
                dvs.mDummy23 == 0 &&
                dvs.mDummy24 == 0 &&
                dvs.mDummy25 == 0 &&
                dvs.mDummy26 == 0 &&
                dvs.mDummy27 == 0 &&
                dvs.mDummy28 == 0 &&
                dvs.mDummy29 == 0 &&
                dvs.mDummy30 == 0 &&
                dvs.mDummy31 == 0 )
                
                {
                    isRanNumGenStatePresent = 0;
                }
            else
                {
                    tempState.mHaveDeviate  = 
                      dvs.mHaveDeviate == 0 ? false : true;;
                    tempState.mNormalDeviate  = dvs.mStoredNormalDeviate;
                    tempState.mRngState.mX0_N0 = dvs.mX0_N0;
                    tempState.mRngState.mX0_N1 = dvs.mX0_N1;
                    tempState.mRngState.mX0_N2 = dvs.mX0_N2;
                    tempState.mRngState.mX1_N0 = dvs.mX1_N0;
                    tempState.mRngState.mX1_N1 = dvs.mX1_N1;
                    tempState.mRngState.mX1_N2 = dvs.mX1_N2;
                    
                    NormalDeviateIF.initialize( tempState );
            
                    isRanNumGenStatePresent = 1;
                }
            break;
            }              
          case ED_DYNAMICVARIABLES_TYPES::BoundingBox :
            {
              ED_DynamicVariablesBoundingBox dvs;
              char * RecordPtr = &Record[0];
              ED_OpenPacket_DynamicVariablesBoundingBox( RecordPtr, dvs );
              
              BegLogLine( PKFXLOG_DVSFILE ) 
                  << "DVSFile using ED_DynamicVariablesBoundingBox packet to assign RTG.mBirthBoundingBoxDimensionVector from "
                  << RTG.mBirthBoundingBoxDimensionVector
                  << " to "
                  << dvs.mSize
                  << EndLogLine;

              RTG.mBirthBoundingBoxDimensionVector = dvs.mSize;
              isBirthBoxPresentInDvs=1;

              break;
            }               
          case ED_DYNAMICVARIABLES_TYPES::TwoChainFragmentThermostat :
            {
#if (MSDDEF_NOSE_HOOVER)
                ED_DynamicVariablesTwoChainFragmentThermostat dvs;
                char *ptr = &Record[0];
                int fragmentNumber;
                int numSitesPerChain;
                int voxId;

                double p0[ NUMBER_OF_SITES_PER_THERMOSTAT ];
                double v0[ NUMBER_OF_SITES_PER_THERMOSTAT ];
                double p1[ NUMBER_OF_SITES_PER_THERMOSTAT ];
                double v1[ NUMBER_OF_SITES_PER_THERMOSTAT ];

                // NUMBER_OF_SITES_PER_THERMOSTAT must be passed in, since it is needed to know how many
                // array elements to get.  There is an assert in the call that confirms
                // the number of sites per chain read from the packet matches the expected value
                ED_Open_DynamicVariablesTwoChainFragmentThermostat(ptr, fragmentNumber, voxId, NUMBER_OF_SITES_PER_THERMOSTAT, p0, v0, p1, v1 );

                assert( fragmentNumber >=0  && fragmentNumber < NUMBER_OF_FRAGMENTS );

                memcpy( aDynVarMgr.mLocalFragmentTable[ fragmentNumber ].mThermostatPosition[ 0 ],
                        p0,
                        sizeof(double) * NUMBER_OF_SITES_PER_THERMOSTAT);

                memcpy( aDynVarMgr.mLocalFragmentTable[ fragmentNumber ].mThermostatVelocity[ 0 ],
                        v0,
                        sizeof(double) * NUMBER_OF_SITES_PER_THERMOSTAT);

                memcpy( aDynVarMgr.mLocalFragmentTable[ fragmentNumber ].mThermostatPosition[ 1 ],
                        p1,
                        sizeof(double) * NUMBER_OF_SITES_PER_THERMOSTAT);

                memcpy( aDynVarMgr.mLocalFragmentTable[ fragmentNumber ].mThermostatVelocity[ 1 ],
                        v1,
                        sizeof(double) * NUMBER_OF_SITES_PER_THERMOSTAT);

                NoseHooverRestart = 1;

                if( PKFXLOG_THERMOSTAT )
                  {	
                     for(int i=0; i<NUMBER_OF_SITES_PER_THERMOSTAT; i++)
                     {
                    	 BegLogLine(PKFXLOG_THERMOSTAT)
                    	   << " aDynVarMgr.mLocalFragmentTable[" << fragmentNumber
                    	   << "].ThermostatVelocity[ 1 ][" << i 
                    	   << "]=" 
                    	   << aDynVarMgr.mLocalFragmentTable[ fragmentNumber ].mThermostatVelocity[ 1 ][i]
                    	   << EndLogLine ;
                     }
                  }
#endif
                break;
            }
          case ED_DYNAMICVARIABLES_TYPES::OneChainThermostat :
            {
#if ( MSDDEF_DoPressureControl && MSDDEF_NOSE_HOOVER )
                ED_DynamicVariablesOneChainThermostat dvs;
                char *ptr = &Record[0];
                int numSitesPerChain;
                int voxId;

                double p0[ NUMBER_OF_SITES_PER_THERMOSTAT ];
                double v0[ NUMBER_OF_SITES_PER_THERMOSTAT ];

                ED_Open_DynamicVariablesOneChainThermostat(ptr, voxId, NUMBER_OF_SITES_PER_THERMOSTAT, p0, v0 );

                memcpy( aDynVarMgr.mPistonThermostatPosition,
                        p0,
                        sizeof(double)*NUMBER_OF_SITES_PER_THERMOSTAT);

                memcpy( aDynVarMgr.mPistonThermostatVelocity,
                        v0,
                        sizeof(double)*NUMBER_OF_SITES_PER_THERMOSTAT);
                for(int i=0; i<NUMBER_OF_SITES_PER_THERMOSTAT; i++)
                {
               	 BegLogLine(PKFXLOG_THERMOSTAT)
               	   << " aDynVarMgr.mPistonThermostatVelocity[" << i
               	   << "].ThermostatVelocity[ 1 ]=" 
               	   << aDynVarMgr.mPistonThermostatVelocity[ i ]
               	   << EndLogLine ;
                }
                NoseHooverRestart = 1;
#endif
            break;
            }
          case ED_DYNAMICVARIABLES_TYPES::Site :
            {
            ED_DynamicVariablesSite dvs;
            char *ptr = &Record[0];
            ED_OpenPacket_DynamicVariablesSite(ptr, dvs.mVoxelId, dvs.mSiteId, dvs.mPosition, dvs.mVelocity);

            BegLogLine( PKFXLOG_DVSFILE )
              << "Got SiteDynamicVariables "
              << dvs.mSiteId
              << " Pos: "<< dvs.mPosition
              << " Vel: "<< dvs.mVelocity
              << EndLogLine;

            // if non selected by args, take the first time step found
            if( SelectTS == -99 )
              {
              SelectTS = 0;
              }

            int internalSiteId = MSD_IF::GetInternalSiteIdForExternalSiteId( dvs.mSiteId );


            // check bounds and then stuff dynamic vars into starting pos/vel.
            if( internalSiteId < 0 || internalSiteId >= aNumberOfSites )
              {
              BegLogLine( PKFXLOG_DVSFILE )
               << "DVS Start contains PosVel packet for site that doesn't exixt.  Packet >"
               << dvs.mSiteId
               << " Pos: "<< dvs.mPosition
               << " Vel: "<< dvs.mVelocity
               << "<"
               << EndLogLine;
              PLATFORM_ABORT("DVS Start contains PosVel packet for site that doesn't exixt.");
              }

            if( StartingPosVelCheckList[ internalSiteId ] )
              {
              if( StartingPositions [ internalSiteId ] != dvs.mPosition
                  ||
                  StartingVelocities [ internalSiteId ] != dvs.mVelocity )
                {
                PLATFORM_ABORT("DVS Start contained duplicate Pos/Vel packet with inconsistant values.");
                }
              else
                {
                BegLogLine( PKFXLOG_DVSFILE )
                  << "WARNING: DVS Start got identical duplicate Pos/Vel packet for site. Packet is>"
                  << dvs.mSiteId
                  << " Pos: "<< dvs.mPosition
                  << " Vel: "<< dvs.mVelocity
                  << "<"
                  << EndLogLine;
                }
              }
            else
              {
                 BegLogLine( PKFXLOG_DVSFILE )
                 << "DVS internalSiteId = " << internalSiteId
                 << " mPosition = ("
		 << dvs.mPosition
                 << ") mVelocity = ("
		 << dvs.mVelocity
                 << ")"
                 << EndLogLine ;

              StartingPositions [ internalSiteId ] = dvs.mPosition;
              StartingVelocities [ internalSiteId ] = dvs.mVelocity;
              StartingPosVelCheckList[ internalSiteId ] = 1;
              }

            // Keep track of the true bounding box to report into log.
            if( FirstInTS )
              {
              FirstInTS = 0;
              aBoundingBox.Init( dvs.mPosition );
              }
            else
              {
              aBoundingBox.Update( dvs.mPosition );
              }

            break;
            } // End case: pos/vel record for selected ts.
          default:
            {
            BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile contains packet TYPE " <<  Hdr.mType << " unrecognized and ignored "<< EndLogLine;
            }
          } // End of switch ( Hdr.mType )
          break;
        } // End of case:  ED_CLASS::DynamicVariables
      default:
        {
        BegLogLine( PKFXLOG_DVSFILE ) << "DVSFile contains packet CLASS " <<  Hdr.mClass << " unrecognized and ignored "<< EndLogLine;
        }
      } // End of switch ( Hdr.mClass )
    } // End of while scan.


  int TotalMissedSites = 0;
  for(int i = 0; i < aNumberOfSites; i++ )
    {
    if( StartingPosVelCheckList[ i ] != 1 )
      {
      BegLogLine( PKFXLOG_DVSFILE )
        << "DVS Start missed packet for site "
        << i
        << EndLogLine;
      TotalMissedSites++;
     }
    }
  if( TotalMissedSites != 0 )
    {
    BegLogLine( PKFXLOG_DVSFILE )
      << "DVS Start missed a total of "
      << TotalMissedSites
      << " Pos Vel site packets."
      << EndLogLine;
    PLATFORM_ABORT("DVS file didn't contain correct site count.");
    }

  // 

  if( !isBirthBoxPresentInDvs )
    RTG.mBirthBoundingBoxDimensionVector = RTG.mStartingBoundingBoxDimensionVector;

  if( !isRanNumGenStatePresent )
   {
     NormalDeviateIF.initializeFromSeed( *reinterpret_cast<unsigned*>( &RTG.mVelocityResampleRandomSeed ) );
   }
            
  RotateSimulation(aNumberOfSites);

  BegLogLine( PKFXLOG_DVSFILE )
    << "Done ReadInitialConditions() "
    << " Got SiteCount "
    << aNumberOfSites
    << " True Bounding Box is Min: "
    << aBoundingBox.mMinBox
    << " Max: "
    << aBoundingBox.mMaxBox
    << EndLogLine;

  return;
  }
#endif
