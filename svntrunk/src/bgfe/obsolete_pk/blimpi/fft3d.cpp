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
 // A prototype pass at an active packet based 3D FFT.  Aug 23 2002.  BGF
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "BladeMpi.hpp"

#include "fxlogger.hpp"

#include "complex.hpp"
#include <math.h>
//#include "fft128.hpp"
#include "fftw.h"

// This is included to create regression FFT data
//#include "t3da.hpp"


template < int iVLen, typename tElementType >
class FFT1D
  {
public:
  typedef tElementType ElementType;

  static
  void
  Op( tElementType aV[ iVLen ] )
    {
    }
  };


fftw_plan fftw_1d_plan;


static fftwnd_plan forward_3d_Plan;
static fftwnd_plan inverse_3d_Plan;

template<int tPX, int tPY, int tPZ,  // Processor Dim
int tGX, int tGY, int tGZ,  // mesh dim
class InnerOp,
class FFT1D >
class FFT
  {
public:
  // Make the parameters visable outside the class
  enum { PX = tPX, PY = tPY, PZ = tPZ, GX = tGX, GY = tGY, GZ = tGZ };

  typedef complex Value;

  struct FFT_Hdr
    {
    // could just gen the address on th sender side, but for now send the indexs
    FFT   *This;
    int    Bar;
    int    Pencil;
    int    PencilIndex;
    };

  // Fiture out how many values fit into a single packet
  enum { GridValuesPerPacket =
    ( ( sizeof( _BGL_TorusPktPayload ) - sizeof( FFT_Hdr ) ) / sizeof( Value ) ) };

  typedef Value  FFT_Payload  [ GridValuesPerPacket ];

  struct ActorPacket
    {
    FFT_Hdr  mHdr;
    FFT_Payload mData;
    };

  // Definitions
  // Pencil - a 1x1xN vector in the mesh
  // Bar - a 1xM group of pencils where M is the number of contingious pencils in a particular plain

  typedef Value XPencil[ GX ];
  typedef Value YPencil[ GY ];
  typedef Value ZPencil[ GZ ];

  // Figure out max mesh values pre processor in each dimension
  // Max is because GX may not be an even factor of PX
  enum { XGridPointsPerProc = GX / PX };
  enum { YGridPointsPerProc = GY / PY };
  enum { ZGridPointsPerProc = GZ / PZ };


#define mMIN(expa,expb) ( ((expa) < (expb)) ? (expa) : (expb) )

  // Since we chose round numbers, the packets in each dim will have the same number of values
  // but they may not be full.
  // Each FFT is formed by parts coming from 1 node in each orthogonal row in a plain.
  // NOTE: When there are enough nodes - this number will go to 0, but should be forced to 1 as the FFTs will be done
  // somewhere.
  enum { ZGridPointsPerPacket   = mMIN( (GZ / PZ ), GridValuesPerPacket ) };
  enum { YGridPointsPerPacket   = mMIN( (GY / PY ), GridValuesPerPacket ) };
  enum { XGridPointsPerPacket   = mMIN( (GX / PX ), GridValuesPerPacket ) };
  enum { ZHGridPointsPerPacket  = mMIN( (GZ / PZ ), GridValuesPerPacket ) };

////  enum { ZGridPointsPerPacket  = ( GZ /  PZ     < GridValuesPerPacket ) ?  PZ     : GridValuesPerPacket };
////  enum { YGridPointsPerPacket  = ( GY / (PX*PZ) < GridValuesPerPacket ) ? (PX*PZ) : GridValuesPerPacket };
////  enum { XGridPointsPerPacket  = ( GX / (PZ*PY) < GridValuesPerPacket ) ? (PZ*PY) : GridValuesPerPacket };
////  enum { ZHGridPointsPerPacket = ( GZ / (PY*PX) < GridValuesPerPacket ) ? (PY*PX) : GridValuesPerPacket };

  // Figure out how many packets will be recieved per pencil.
  // NOTE, AGAIN THIS MUST NOT ROUND
  enum { ZPacketsPerPencil  = GZ / ZGridPointsPerPacket  };
  enum { YPacketsPerPencil  = GY / YGridPointsPerPacket  };
  enum { XPacketsPerPencil  = GX / XGridPointsPerPacket  };
  enum { ZHPacketsPerPencil = (GZ/PZ) / ZHGridPointsPerPacket };

  enum { ZintoYPencilsPerBar = YGridPointsPerProc };
  enum { YintoXPencilsPerBar = XGridPointsPerProc };
  enum { XintoZPencilsPerBar = ZGridPointsPerProc };


/////  // Figure out how many packets will be recieved per pencil.
/////  // NOTE: THIS WILL BE A PROBLEM IF THERE IS ANY REMAINDER TO THE DIVISION - AND WE DON'T CHECK.
/////  enum { ZPacketsPerPencilPerProc = ZPacketsPerPencil / PZ };
/////  enum { YPacketsPerPencilPerProc = YPacketsPerPencil / PY };
/////  enum { XPacketsPerPencilPerProc = XPacketsPerPencil / PX };

  // This typedef defines a local processors share of the total grid.
  typedef Value ProcGridSection[ XGridPointsPerProc ][ YGridPointsPerProc ][ ZGridPointsPerProc ];

  // Keep a pointer to the callers data area so we can put results back in place.
  ProcGridSection *mPMS;

  // Figure out how many bars are shared by a row of processors
  enum { ZBarsSharedPerProc = XGridPointsPerProc };
  enum { YBarsSharedPerProc = ZGridPointsPerProc };
  enum { XBarsSharedPerProc = YGridPointsPerProc };

  // Figure out how many bars each processor in a row is responsible for.
  enum { ZBarsOwnedPerProc = ZBarsSharedPerProc / PZ };
  enum { YBarsOwnedPerProc = YBarsSharedPerProc / PY };
  enum { XBarsOwnedPerProc = XBarsSharedPerProc / PX };


  // Allocate the local memory for bars owned
  // This is the memory footprint that allows all sent messages to
  // deposit data on arival.
  ZPencil ZBarSet[ ZBarsOwnedPerProc ][ ZintoYPencilsPerBar ];
  YPencil YBarSet[ YBarsOwnedPerProc ][ YintoXPencilsPerBar ];
  XPencil XBarSet[ XBarsOwnedPerProc ][ XintoZPencilsPerBar ];

  // An int counter for each pencil to figure when it's done.
  int ZCompletePacketsInPencilCount[ ZBarsOwnedPerProc ][ ZintoYPencilsPerBar ];
  int YCompletePacketsInPencilCount[ YBarsOwnedPerProc ][ YintoXPencilsPerBar ];
  int XCompletePacketsInPencilCount[ XBarsOwnedPerProc ][ XintoZPencilsPerBar ];

  int ZHCompletePacketsInPencilCount[ ZBarsOwnedPerProc ][ZintoYPencilsPerBar ];

  // An int counter for each bar set - when complete, packets can be sent into next dim
  int  ZCompletePencilsInBarCount[ ZBarsOwnedPerProc ];
  int  YCompletePencilsInBarCount[ YBarsOwnedPerProc ];
  int  XCompletePencilsInBarCount[ XBarsOwnedPerProc ];

  int  ZHCompletePencilsInBarCount[ ZBarsSharedPerProc ];

#if 0
  // Using these seems to indicate an extra bounce of memory.
  // Allocate the headers for all the communication
  _BGL_TorusPktHdr ZintoYPacket[ ZBarsSharedPerProc ][ ZintoYPencilsPerBar ][ ZPacketsPerPencilPerProc ];
  _BGL_TorusPktHdr YPacket[ YBarsSharedPerProc ][ YintoXPencilsPerBar ][ YPacketsPerPencilPerProc ];
  _BGL_TorusPktHdr XPacket[ XBarsSharedPerProc ][ XintoZPencilsPerBar ][ XPacketsPerPencilPerProc ];
#endif


  // Local processor coordinates
  int mXPCoord, mYPCoord, mZPCoord;

  void Check() { }

  static int
  ZPacketActorFx( void *pkt )
    {
    ActorPacket *ap = (ActorPacket *) pkt;

    FFT *This              = ap->mHdr.This;
    int AbsBar             = ap->mHdr.Bar;
    int MyBarIndex         = AbsBar;
    int Pencil             = ap->mHdr.Pencil;
    int BaseOffsetInPencil = ap->mHdr.PencilIndex;

    int x = 0, y = 0, z = 0;
    BGLPartitionGetCoords( &x, &y, &z );

    for( int v = 0; v < ZGridPointsPerPacket; v++ )
      {
      // Look out for partial last packet.
      if( BaseOffsetInPencil + v == GZ )
        break;
      if( This->ZBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ].re != 0 )
        {
        BegLogLine(1) << " Fatal Error in Z on [" <<x<<","<<y<<","<<z<<"] "
             <<  "Bar " << MyBarIndex << " Pencil " << Pencil << " BaseIndex " <<  BaseOffsetInPencil << " v " << v
             << " Value " << This->ZBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ].re << EndLogLine;
        }
      assert( This->ZBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ].re == 0 );
      This->ZBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ] = ap->mData[ v ];
      }

    This->ZCompletePacketsInPencilCount[ MyBarIndex ][ Pencil ]++;

    assert(    This->ZCompletePacketsInPencilCount[ MyBarIndex ][ Pencil ] >= 0
            && This->ZCompletePacketsInPencilCount[ MyBarIndex ][ Pencil ] <=  ZPacketsPerPencil );

    if( This->ZCompletePacketsInPencilCount[ MyBarIndex ][ Pencil ] == ZPacketsPerPencil )
      {
      // Make counter ready for next phase.
      This->ZCompletePacketsInPencilCount[ MyBarIndex ][ Pencil ] = 0;

      // Do the 1D FFT
      fftw_complex input[ GZ ];
      for(int i = 0; i < GZ ; i++ )
        {
        input[i].re = This->ZBarSet[ MyBarIndex ][ Pencil ][ i ].re;
        input[i].im = This->ZBarSet[ MyBarIndex ][ Pencil ][ i ].im;
        }

      fftw_one(  fftw_1d_plan,
                 input ,
                 (fftw_complex *) This->ZBarSet[ MyBarIndex ][ Pencil ]   );

      // When done, mark count ... could put down in outbound packets as well.
      This->ZCompletePencilsInBarCount[ MyBarIndex ] ++ ;

      BegLogLine(1)
        << "ZActor : Pencil Complete "
        << " AbsBar "
        << ap->mHdr.Bar
        << " MyBarIndex "
        << MyBarIndex
        << " Count "
        << This->ZCompletePencilsInBarCount[ MyBarIndex ]
        << EndLogLine;

      // Increment completed pencils in bar count.
      // Since this bar is done, packets could be sent.
      // BUT, can't send them from an active message context without risking deadlock.
      // SO, should post this state somewhere.
      }

    BegLogLine(1) << " Done Actor in Z collect on [" <<x<<","<<y<<","<<z<<"] "
    <<  "Bar " << MyBarIndex << " Pencil " << Pencil << " PencilOffset " <<  BaseOffsetInPencil
    << " PacketsInPencilCount "
    <<  This->ZCompletePacketsInPencilCount[ MyBarIndex ][ Pencil ]
    << " PencilsInBarCompleteCount "
    << This->ZCompletePencilsInBarCount[ ap->mHdr.Bar ]
    << EndLogLine;

    return(0);
    }


  static int
  YPacketActorFx( void *pkt )
    {
    ActorPacket *ap = (ActorPacket *) pkt;

    FFT *This              = ap->mHdr.This;
    int AbsBar             = ap->mHdr.Bar;
    int MyBarIndex         = AbsBar;
    int Pencil             = ap->mHdr.Pencil;
    int BaseOffsetInPencil = ap->mHdr.PencilIndex;

    for( int v = 0; v < YGridPointsPerPacket; v++ )
      {
      // Look out for partial last packet.
      if( BaseOffsetInPencil + v == GY )
        break;
      if( This->YBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ].re != 0 )
      BegLogLine(1) << " Fatal Error in Y " <<  "Bar " << MyBarIndex << " Pencil " << Pencil << " BaseIndex " <<  BaseOffsetInPencil << " v " << v
           << " Value " << This->YBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ].re << EndLogLine;
      assert( This->YBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ].re == 0 );
      This->YBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ] = ap->mData[ v ];
      }

    This->YCompletePacketsInPencilCount[ MyBarIndex ][ Pencil ]++;

    if( This->YCompletePacketsInPencilCount[ MyBarIndex ][ Pencil ] == YPacketsPerPencil )
      {
      // Do the 1D FFT
      fftw_complex input[ GY ];
      for(int i = 0; i < GY ; i++ )
        {
        input[i].re = This->YBarSet[ MyBarIndex ][ Pencil ][ i ].re;
        input[i].im = This->YBarSet[ MyBarIndex ][ Pencil ][ i ].im;
        }

      fftw_one(  fftw_1d_plan,
                 input ,
                (fftw_complex *) This->YBarSet[ MyBarIndex ][ Pencil ]   );

      // When done, mark count ... could put down in outbound packets as well.
      This->YCompletePencilsInBarCount[ MyBarIndex ] ++ ;

      BegLogLine(1)
        << "YActor : Pencil Complete "
        << " AbsBar "
        << ap->mHdr.Bar
        << " MyBarIndex "
        << MyBarIndex
        << " Count "
        << This->YCompletePencilsInBarCount[ MyBarIndex ]
        << EndLogLine;

      // Increment completed pencils in bar count.
      // Since this bar is done, packets could be sent.
      // BUT, can't send them from an active message context without risking deadlock.
      // SO, should post this state somewhere.
      }

    return(0);
    }


  static int
  XPacketActorFx( void *pkt )
    {
    ActorPacket *ap = (ActorPacket *) pkt;

    FFT *This              = ap->mHdr.This;
    int AbsBar             = ap->mHdr.Bar;
    int MyBarIndex         = AbsBar;
    int Pencil             = ap->mHdr.Pencil;
    int BaseOffsetInPencil = ap->mHdr.PencilIndex;

    for( int v = 0; v < XGridPointsPerPacket; v++ )
      {
      // Look out for partial last packet.
      if( BaseOffsetInPencil + v == GX )
        break;
      if( This->XBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ].re != 0 )
      BegLogLine(1) << " Fatal Error in X " <<  "Bar " << MyBarIndex << " Pencil " << Pencil << " BaseIndex " <<  BaseOffsetInPencil << " v " << v
           << " Value " << This->XBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ].re << EndLogLine;
      assert( This->XBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ].re == 0 );
      This->XBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ] = ap->mData[ v ];
      }

    This->XCompletePacketsInPencilCount[ MyBarIndex ][ Pencil ]++;

    if( This->XCompletePacketsInPencilCount[ MyBarIndex ][ Pencil ] == XPacketsPerPencil )
      {
      // Do the 1D FFT
      fftw_complex input[ GX ];
      for(int i = 0; i < GX ; i++ )
        {
        input[i].re = This->XBarSet[ MyBarIndex ][ Pencil ][ i ].re;
        input[i].im = This->XBarSet[ MyBarIndex ][ Pencil ][ i ].im;
        }

      fftw_one(  fftw_1d_plan,
                 input ,
                 (fftw_complex *) This->XBarSet[ MyBarIndex ][ Pencil ]   );

      // When done, mark count ... could put down in outbound packets as well.
      This->XCompletePencilsInBarCount[ ap->mHdr.Bar ] ++ ;

      BegLogLine(1)
        << "XActor : Pencil Complete "
        << " AbsBar "
        << ap->mHdr.Bar
        << " MyBarIndex "
        << MyBarIndex
        << " Count "
        << This->XCompletePencilsInBarCount[ MyBarIndex ]
        << EndLogLine;

      // Increment completed pencils in bar count.
      // Since this bar is done, packets could be sent.
      // BUT, can't send them from an active message context without risking deadlock.
      // SO, should post this state somewhere.
      }

    return(0);
    }

  static int
  ZHomePacketActorFx( void *pkt )
    {
    ActorPacket *ap = (ActorPacket *) pkt;

    FFT *This              = ap->mHdr.This;
    int AbsBar             = ap->mHdr.Bar;
    int MyBarIndex         = AbsBar;
    int Pencil             = ap->mHdr.Pencil;
    int BaseOffsetInPencil = ap->mHdr.PencilIndex;

    BegLogLine(1)
      << "XHActor : Got Packet "
      << " AbsBar "
      << ap->mHdr.Bar
      << " Pencil "
      << Pencil
      << " PenciIndex "
      << BaseOffsetInPencil
      << EndLogLine;

    for( int v = 0; v < ZHGridPointsPerPacket; v++ )
      {
      // Look out for partial last packet.
      if( BaseOffsetInPencil + v == GZ )
        break;

      //This->ZBarSet[ MyBarIndex ][ Pencil ][ BaseOffsetInPencil + v ] = ap->mData[ v ];

      (*(This->mPMS))[ MyBarIndex ][ Pencil ][ v ] = ap->mData[ v ];

      }

    This->ZHCompletePacketsInPencilCount[ MyBarIndex ][ Pencil ]++;

    if( This->ZHCompletePacketsInPencilCount[ MyBarIndex ][ Pencil ] == ZHPacketsPerPencil )
      {
      // When done, mark count ... could put down in outbound packets as well.
      This->ZHCompletePencilsInBarCount[ MyBarIndex ] ++ ;

      BegLogLine(1)
        << "XHActor : Pencil Complete "
        << " AbsBar "
        << ap->mHdr.Bar
        << " MyBarIndex "
        << MyBarIndex
        << " Count "
        << This->ZHCompletePencilsInBarCount[ MyBarIndex ]
        << EndLogLine;

      }

    return(0);
    }
  int Configured;

  void
  Configure( int aXPCoord, int aYPCoord, int aZPCoord )
    {
///    assert( Configured == 0 );
    mXPCoord = aXPCoord;
    mYPCoord = aYPCoord;
    mZPCoord = aZPCoord;

    // Need to do a plan for each dim.
    fftw_1d_plan = fftw_create_plan( GX , FFTW_FORWARD, FFTW_MEASURE );

    Configured = 1;
MPI_Barrier( MPI_COMM_WORLD );         //Wait for all processes to get here.
    };

  void
  DoFFT( ProcGridSection &aPMS )
    {
    mPMS = &aPMS;

    BegLogLine(1) << "*** Collect on Z axis ***" << EndLogLine;

    ActorPacket zap;

    // Probably best to start in Z dim since that will be contigious in C++ data representation
    // For now, just suffer the striding.

    for(int xm = 0; xm < XGridPointsPerProc; xm++ )
      {
      for(int ym = 0; ym < YGridPointsPerProc; ym++ )
        {
        int LoadCount = 0;
        int PencilElementIndex = mZPCoord * ZGridPointsPerProc;

        for(int zm = 0; zm < ZGridPointsPerProc; zm++ )
          {
          zap.mData[ LoadCount ] = aPMS[ xm ][ ym ][ zm ];
          LoadCount++;

          BegLogLine(1)
            << "DoFFT "
            << "LoadCount " << LoadCount
            << " PencilElementIndex " << PencilElementIndex
            << EndLogLine;

          if( LoadCount == ZGridPointsPerPacket ) // should not need this:   || (zm + 1 ==  ZGridPointsPerProc) )
            {
            int XPTarget            = mXPCoord;
            int YPTarget            = mYPCoord;
            int ZPTarget            = xm / ZBarsOwnedPerProc; // Distribute shared bars over procs in row.

            int ZBarIndex           = xm % ZBarsOwnedPerProc;
            int ZPencilIndex        = ym;
            int ZPencilElementIndex = PencilElementIndex ;

            BegLogLine(1)
              << "DoFFT "
              << "H->Z Send To: "
              << "[" << XPTarget << "," << YPTarget << "," << ZPTarget << "]"
              << " AbsBar "             << ZBarIndex
              << " PencilIndex "        << ZPencilIndex
              << " PencilElementIndex " << ZPencilElementIndex
              << EndLogLine;

            zap.mHdr.This                = this;
            zap.mHdr.Bar                 = ZBarIndex;
            zap.mHdr.Pencil              = ZPencilIndex;
            zap.mHdr.PencilIndex         = ZPencilElementIndex;

            BGLTorusSimpleActivePacketSend( ZPacketActorFx,
                                            XPTarget,
                                            YPTarget,
                                            ZPTarget,
                                            sizeof( zap ),
                                            (void *) &zap );

            PencilElementIndex += LoadCount;
            LoadCount = 0;
            }
          }
        }
      }

    // Spin-wait for complete (including fft'd) Z vectors
    BegLogLine(1) << " Done with Z send, START BARRIER " << EndLogLine;

// This explicit barrier would not be used - the wait on data would be used below.
MPI_Barrier( MPI_COMM_WORLD );         //Wait for all processes to get here.

    BegLogLine(1) << " Done with Z send, FINISH BARRIER " << EndLogLine;

    for( int bar = 0; bar < ZBarsOwnedPerProc ; bar++ )
      {
      ActorPacket yap;
      // Wait for the bar to comlete
      // If bars complete in order, we might have some chance to catch this high
      // in the memory hierarchy
      int ZBarMon = -1;
      while( ZCompletePencilsInBarCount[ bar ] != ZintoYPencilsPerBar )
        {
        if( ZBarMon != ZCompletePencilsInBarCount[ bar ] )
          {
          BegLogLine(1)
            << "DoFFT "
            << "Waiting on Local Bar "
            << bar
            << " Current Count "
            <<  ZCompletePencilsInBarCount[ bar ]
            << " expecting "
            << ZintoYPencilsPerBar
            << EndLogLine;
          ZBarMon = ZCompletePencilsInBarCount[ bar ];
          }
        sleep(1);
///        sched_yield();
        }
      // Make counter ready for next phase
      ZCompletePencilsInBarCount[ bar ] = 0;

      BegLogLine(1)
        << "DoFFT "
        << "Finished Wait on Local Bar "
        << bar
        << EndLogLine;

      for(int zm = 0; zm < GZ; zm++ )   // Actually indexes the Y pencil parts in the Z bar
        {

        int LoadCount = 0;
        int PencilElementIndex = mYPCoord * YGridPointsPerProc;

        for(int ym = 0; ym < YGridPointsPerProc; ym++ )
          {
          //NOTE: middle index varying fastest as we stide to accum packets
          yap.mData[ LoadCount ] = ZBarSet[ bar ][ ym ][ zm ];
          LoadCount++;

          BegLogLine(1) << "DoFFT Z->Y Load: LoadCount " << LoadCount << " bar " << bar << " ym " << ym << " zm " << zm << EndLogLine;

          if( LoadCount == YGridPointsPerPacket ) // should not need this:   || (zm + 1 ==  YGridPointsPerProc) )
            {
            int XPTarget            = mXPCoord;
            int YPTarget            = (zm % ZGridPointsPerProc) / YBarsOwnedPerProc ; // (Abs Y Bar Index) / No Procs in Y
            int ZPTarget            = zm / ZGridPointsPerProc ;        // Put back z proc

            int YBarIndex           = zm % YBarsOwnedPerProc ;
            int YPencilIndex        = bar + mZPCoord * YBarsOwnedPerProc ;
            int YPencilElementIndex = PencilElementIndex;

            BegLogLine(1)
              << "DoFFT "
              << "Z->Y Send To: "
              << "[" << XPTarget << "," << YPTarget << "," << ZPTarget << "]"
              << " AbsBar "             << YBarIndex
              << " PencilIndex "        << YPencilIndex
              << " PencilElementIndex " << YPencilElementIndex
              << EndLogLine;

            yap.mHdr.This                = this;
            yap.mHdr.Bar                 = YBarIndex;
            yap.mHdr.Pencil              = YPencilIndex;
            yap.mHdr.PencilIndex         = YPencilElementIndex;

            BGLTorusSimpleActivePacketSend( YPacketActorFx,
                                            XPTarget,
                                            YPTarget,
                                            ZPTarget,
                                            sizeof( yap ),
                                            (void *) &yap );


            PencilElementIndex += LoadCount;
            LoadCount = 0;
            }
          }
        }
      }


    // Spin-wait for complete (including fft'd) Z vectors
    BegLogLine(1) << " Done with END of Y send barrier on [" <<
              mXPCoord << "," << mYPCoord << "," << mZPCoord << "]" << EndLogLine;
MPI_Barrier( MPI_COMM_WORLD );         //Wait for all processes to get here.


    for( int bar = 0; bar < YBarsOwnedPerProc ; bar++ )
      {
      ActorPacket xap;
      // Wait for the bar to comlete
      // If bars complete in order, we might have some chance to catch this high
      // in the memory hierarchy
      int YBarMon = -1;
      while( YCompletePencilsInBarCount[ bar ] != YintoXPencilsPerBar )
        {
        if( YBarMon != YCompletePencilsInBarCount[ bar ] )
          {
              printf("Waiting on [%d,%d,%d] to complete YCompBenInBarC[ %d ] = %d / %d \n",
              mXPCoord, mYPCoord, mZPCoord,
              bar, YCompletePencilsInBarCount[ bar ], YintoXPencilsPerBar );
          YBarMon = YCompletePencilsInBarCount[ bar ];
          }

        fflush( stdout );
        sleep(1);
///        sched_yield();
        }

      for(int ym = 0; ym < GY; ym++ )
        {

        int LoadCount = 0;
        int PencilElementIndex = mXPCoord * XGridPointsPerProc;

        for(int xm = 0; xm < XGridPointsPerProc; xm++ )
          {
          //NOTE: middle index varying fastest as we stide to accum packets
          xap.mData[ LoadCount ] = YBarSet[ bar ][ xm ][ ym ];
          LoadCount++;

          BegLogLine(1) << "DoFFT Y->X Load: LoadCount " << LoadCount << " bar " << bar << " xm " << xm << " ym " << ym << EndLogLine;

          if( LoadCount == XGridPointsPerPacket ) // should not need this:   || (zm + 1 ==  XGridPointsPerProc) )
            {
            int XPTarget         = (ym % YGridPointsPerProc) / XBarsOwnedPerProc ;
            int YPTarget         = ym / YGridPointsPerProc ;
            int ZPTarget         = mZPCoord;

            int XBarIndex           = ym % XBarsOwnedPerProc ;
            int XPencilIndex        = bar + mYPCoord * ZBarsOwnedPerProc ;
            int XPencilElementIndex = PencilElementIndex;

            BegLogLine(1)
              << "DoFFT "
              << "Y->X Send To: "
              << "[" << XPTarget << "," << YPTarget << "," << ZPTarget << "]"
              << " AbsBar "             << XBarIndex
              << " PencilIndex "        << XPencilIndex
              << " PencilElementIndex " << XPencilElementIndex
              << EndLogLine;

            assert( XBarIndex   <  GY );
            assert( XPencilIndex <  YintoXPencilsPerBar );
            assert( XPencilElementIndex <  GX );

            xap.mHdr.This                = this;
            xap.mHdr.Bar                 = XBarIndex;
            xap.mHdr.Pencil              = XPencilIndex;
            xap.mHdr.PencilIndex         = XPencilElementIndex;

            //XPacketActorFx( (void *) &xap );
            BGLTorusSimpleActivePacketSend( XPacketActorFx,
                                            XPTarget,
                                            YPTarget,
                                            ZPTarget,
                                            sizeof( xap ),
                                            (void *) &xap );


            PencilElementIndex += LoadCount;
            LoadCount = 0;
            }
          }
        }
      }

    // Spin-wait for complete (including fft'd) Z vectors
    BegLogLine(1) << " Done with END of X send barrier on [" <<
              mXPCoord << "," << mYPCoord << "," << mZPCoord << "]" << EndLogLine;

#if 1
MPI_Barrier( MPI_COMM_WORLD );         //Wait for all processes to get here.

    // Wait for X bars to complete, load 'em up and send 'em home.
    for( int bar = 0; bar < XBarsOwnedPerProc ; bar++ )
      {
      ActorPacket zhap;

      // Wait for the bar to comlete
      // If bars complete in order, we might have some chance to catch this high
      // in the memory hierarchy
      int XBarMon = -1;
      while( XCompletePencilsInBarCount[ bar ] != XintoZPencilsPerBar )
        {
        if( XBarMon != XCompletePencilsInBarCount[ bar ] )
          {
              printf("Waiting on [%d,%d,%d] to complete XCompBenInBarC[ %d ] = %d / %d \n",
              mXPCoord, mYPCoord, mZPCoord,
              bar, XCompletePencilsInBarCount[ bar ], XintoZPencilsPerBar );
          XBarMon = XCompletePencilsInBarCount[ bar ];
          }
        sleep(1);
///        sched_yield();
        }
      for(int xm = 0; xm < GX; xm++ )
        {

        int LoadCount = 0;
        int PencilElementIndex = mZPCoord * ZGridPointsPerProc;

        for(int zm = 0; zm <  XintoZPencilsPerBar; zm++ )
          {
          //NOTE: middle index varying fastest as we stide to accum packets
          zhap.mData[ LoadCount ] = XBarSet[ bar ][ zm ][ xm ];
          LoadCount++;

          if( LoadCount == ZHGridPointsPerPacket ) // should not need this:   || (zm + 1 ==  ZGridPointsPerProc) )
            {
            int XPTarget         = xm / ZGridPointsPerProc ;
            int YPTarget         = mYPCoord;
            int ZPTarget         = mZPCoord;

            int ZBarIndex           = xm % ZBarsSharedPerProc ; // Note: Uses SHARED number since we send into orig data structure
            int ZPencilIndex        = bar + mXPCoord * YBarsOwnedPerProc ;
            int ZPencilElementIndex = 0; /// PencilElementIndex;

            BegLogLine(1)
              << "DoFFT "
              << "X->Home Send To: "
              << "[" << XPTarget << "," << YPTarget << "," << ZPTarget << "]"
              << " AbsBar "             << ZBarIndex
              << " PencilIndex "        << ZPencilIndex
              << " PencilElementIndex " << ZPencilElementIndex
              << EndLogLine;

            zhap.mHdr.This                = this;
            zhap.mHdr.Bar                 = ZBarIndex;
            zhap.mHdr.Pencil              = ZPencilIndex;
            zhap.mHdr.PencilIndex         = ZPencilElementIndex;

            BGLTorusSimpleActivePacketSend( ZHomePacketActorFx,
                                            XPTarget,
                                            YPTarget,
                                            ZPTarget,
                                            sizeof( zhap ),
                                            (void *) &zhap );


            PencilElementIndex += LoadCount;
            LoadCount = 0;
            }
          }
        }
      }


MPI_Barrier( MPI_COMM_WORLD );         //Wait for all processes to get here.

    BegLogLine(1)
      << "DoFFT "
      << "X->Home Done Send : Start Barrier "
      << EndLogLine;

    for( int bar = 0; bar < ZBarsSharedPerProc ; bar++ )
      {
      // Wait for the bar to comlete
      // If bars complete in order, we might have some chance to catch this high
      // in the memory hierarchy
      int ZHBarMon = -1;
      while( ZHCompletePencilsInBarCount[ bar ] != ZintoYPencilsPerBar )
        {
        if( ZHBarMon != ZHCompletePencilsInBarCount[ bar ] )
          {
              printf("Waiting on [%d,%d,%d] to complete ZHCompBenInBarC[ %d ] = %d / %d \n",
              mXPCoord, mYPCoord, mZPCoord,
              bar, ZHCompletePencilsInBarCount[ bar ], XintoZPencilsPerBar );
          ZHBarMon = ZHCompletePencilsInBarCount[ bar ];
          }
        sleep(1);
///        sched_yield();
        }
      // Make counter ready for next phase
      ZHCompletePencilsInBarCount[ bar ] = 0;
      }

#endif

MPI_Barrier( MPI_COMM_WORLD );         //Wait for all processes to get here.
    printf("All Z Home bars completed.\n");
    fflush( stdout );

MPI_Barrier( MPI_COMM_WORLD );         //Wait for all processes to get here.


    return;
    }

  };

class Op {};
class OneDimFFT {};

#define MESH 4
#define PROC 2  // cube root - so 8 nodes.
#define XG MESH
#define YG MESH
#define ZG MESH
#define XP PROC
#define YP PROC
#define ZP PROC

typedef FFT<XP,YP,ZP,XG,YG,ZG,Op,FFT1D<MESH,complex> > FFT512;

FFT512 fft;

FFT512::ProcGridSection pms;

complex a1[XG][YG][ZG];
fftw_complex *in = (fftw_complex*)a1;
complex a2[XG][YG][ZG];
fftw_complex *out = (fftw_complex*)a2;

int
FFTDriver()
  {
  int NodesX = XP, NodesY = YP, NodesZ = ZP ;
  BGLPartitionGetDimensions( &NodesX, &NodesY, &NodesZ );

  if(    XP != NodesX
      || YP != NodesY
      || ZP != NodesZ )
    {
    cerr << "Partition does not match compile time defintion." << endl;
    exit(-1);
    }

  if(    XG % XP != 0
      || YG % YP != 0
      || ZG % ZP != 0 )
    {
    cerr << "FFT grid not even multiple of Processor grid." << endl;
    exit(-1);
    }

  cerr << "sizeof( FFT512::ProcGridSection ) " << sizeof( FFT512::ProcGridSection ) << endl;

  // Load up some recognizable coords as values of PMS
  for(int xm = 0; xm < FFT512::XGridPointsPerProc; xm++ )
    {
    for(int ym = 0; ym < FFT512::YGridPointsPerProc; ym++ )
      {
      for(int zm = 0; zm < FFT512::ZGridPointsPerProc; zm++ )
        {
        // pms[ xm ][ ym ][ zm ] = static_cast<complex>(1);
        pms[xm][ym][zm] = complex( xm * 100000 + ym * 1000 + zm ,0);
        }
      }
    }

  BegLogLine(1) << "sizeof( ProcGridSection ) " << sizeof( pms ) << EndLogLine;

  int MyX = 0, MyY = 0, MyZ = 0;
  BGLPartitionGetCoords( &MyX, &MyY, &MyZ );

  printf("\n\n* * * * Node %2d %2d %2d * * * *\n", MyX, MyY, MyZ  );

  fft.Configure( MyX, MyY, MyZ   );

  fft.DoFFT( pms );

  forward_3d_Plan = fftw3d_create_plan( XG,YG,ZG,FFTW_FORWARD,FFTW_MEASURE );

  for(int x=0; x<XG; x++)
  for(int y=0; y<YG; y++)
  for(int z=0; z<ZG; z++)
    a1[x][y][z] = complex( x * 100000 + y * 1000 + z , MyX * 100000 +  MyY * 1000 + MyZ  );

  fftwnd_one(forward_3d_Plan,in,out);

  int BadValueCount = 0;
  for(int x=0; x<XG; x++)
  for(int y=0; y<YG; y++)
  for(int z=0; z<ZG; z++)
    {
    complex fftw_value = a2[x][y][z];
    complex fft3d_value = pms[x][y][z];
    if(   fabs( a2[x][y][z].re - pms[x][y][z].re ) > 0.0000000001
       || fabs( a2[x][y][z].im - pms[x][y][z].im ) > 0.0000000001 )
      BadValueCount++;
    }
  printf("Interative Value 3D FFT ");
  if( BadValueCount )
     printf("Failed\n");
  else
     printf("Succeeded\n");
  fflush( stdout );

MPI_Barrier( MPI_COMM_WORLD );         //Wait for all processes to get here.

  return(0);
  }

int AttachDebugger( char * );


void Print_FFT_Info()
  {
  BegLogLine(1) << "GX " << FFT512::GX << " GY " << FFT512::GY << " GZ " << FFT512::GZ << EndLogLine;
  BegLogLine(1) << "PX " << FFT512::PX << " PY " << FFT512::PY << " PZ " << FFT512::PZ << EndLogLine;

  BegLogLine(1) << "Number of mesh values total " <<                          FFT512::GX * FFT512::GY * FFT512::GZ << EndLogLine;
  BegLogLine(1) << "Memory of mesh values total " <<  sizeof(FFT512::Value) * FFT512::GX * FFT512::GY * FFT512::GZ << EndLogLine;

  BegLogLine(1) << "XGridPointsPerProc " << FFT512::XGridPointsPerProc << EndLogLine;
  BegLogLine(1) << "YGridPointsPerProc " << FFT512::YGridPointsPerProc << EndLogLine;
  BegLogLine(1) << "ZGridPointsPerProc " << FFT512::ZGridPointsPerProc << EndLogLine;

  BegLogLine(1) << "ZintoYPencilsPerBar " << FFT512::ZintoYPencilsPerBar << EndLogLine;
  BegLogLine(1) << "YintoXPencilsPerBar " << FFT512::YintoXPencilsPerBar << EndLogLine;
  BegLogLine(1) << "XintoZPencilsPerBar " << FFT512::XintoZPencilsPerBar << EndLogLine;

  BegLogLine(1) << "ZPacketsPerPencil " << FFT512::ZPacketsPerPencil << EndLogLine;
  BegLogLine(1) << "YPacketsPerPencil " << FFT512::YPacketsPerPencil << EndLogLine;
  BegLogLine(1) << "XPacketsPerPencil " << FFT512::XPacketsPerPencil << EndLogLine;
  BegLogLine(1) << "ZHPacketsPerPencil " << FFT512::ZHPacketsPerPencil << EndLogLine;

/////  BegLogLine(1) << "ZPacketsPerPencilPerProc " << FFT512::ZPacketsPerPencilPerProc  << EndLogLine;
/////  BegLogLine(1) << "YPacketsPerPencilPerProc " << FFT512::YPacketsPerPencilPerProc  << EndLogLine;
/////  BegLogLine(1) << "XPacketsPerPencilPerProc " << FFT512::XPacketsPerPencilPerProc  << EndLogLine;

  BegLogLine(1) << "Grid Values Per Packet " << FFT512::GridValuesPerPacket << EndLogLine;

  BegLogLine(1) << "ZGridPointsPerPacket " << FFT512::ZGridPointsPerPacket << EndLogLine;
  BegLogLine(1) << "YGridPointsPerPacket " << FFT512::YGridPointsPerPacket << EndLogLine;
  BegLogLine(1) << "XGridPointsPerPacket " << FFT512::XGridPointsPerPacket << EndLogLine;
  BegLogLine(1) << "ZHGridPointsPerPacket " << FFT512::ZHGridPointsPerPacket << EndLogLine;

  BegLogLine(1) << "Number of mesh values per node "
       <<                         FFT512::XGridPointsPerProc * FFT512::YGridPointsPerProc * FFT512::ZGridPointsPerProc << EndLogLine;
  BegLogLine(1) << "Memory of mesh values per node (bytes) "
       << sizeof(FFT512::Value) * FFT512::XGridPointsPerProc * FFT512::YGridPointsPerProc * FFT512::ZGridPointsPerProc << EndLogLine;

  BegLogLine(1) << "ZBarsSharedPerProc " << FFT512::ZBarsSharedPerProc << EndLogLine;
  BegLogLine(1) << "YBarsSharedPerProc " << FFT512::YBarsSharedPerProc << EndLogLine;
  BegLogLine(1) << "XBarsSharedPerProc " << FFT512::XBarsSharedPerProc << EndLogLine;

  BegLogLine(1) << "ZBarsOwnedPerProc " << FFT512::ZBarsOwnedPerProc << EndLogLine;
  BegLogLine(1) << "YBarsOwnedPerProc " << FFT512::YBarsOwnedPerProc << EndLogLine;
  BegLogLine(1) << "XBarsOwnedPerProc " << FFT512::XBarsOwnedPerProc << EndLogLine;

  BegLogLine(1) << "sizeof( ZBarSet ) (bytes) " << sizeof( FFT512::ZBarSet ) << EndLogLine;
  BegLogLine(1) << "sizeof( YBarSet ) (bytes) " << sizeof( FFT512::YBarSet ) << EndLogLine;
  BegLogLine(1) << "sizeof( XBarSet ) (bytes) " << sizeof( FFT512::XBarSet ) << EndLogLine;

  }

main(int argc, char** argv, char** argp)
  {
  MPI_Init( &argc, &argv );  /* initialize MPI environment */
  AttachDebugger( argv[0] );

  BladeInit( &argc, &argv );

  int MpiRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &MpiRank );

  BegLogLine(1)
    << "Main(): "
    << "Rank "
    << MpiRank
    << EndLogLine;

  if( MpiRank == 0 )
    Print_FFT_Info();

  FFTDriver();

  return(0);
  }
