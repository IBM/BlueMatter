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
#include <iostream.h>
#include <pk/platform.hpp>

#define PACKET_SIZE 240

template < int iVLen, typename tElementType >
class FFT1D
  {
public:
  static
  void
  Op( tElementType aV[ iVLen ] )
    {
    int t;
    printf(" FFTing ");
    for(int i = 0; i < iVLen; i++ )
      {
      printf( " %6d->", t = aV[ i ] );
      aV[ i ] += aV[ (i + 1) % iVLen ] * 2;
      printf( "%6d ", t = aV[ i ] );
      }
    printf("\n");
    }
  };

template<int tPX, int tPY, int tPZ,  // Processor Dim
int tMX, int tMY, int tMZ,  // mesh dim
class Op,
class FFT1D >
class FFT
  {
public:
  // Make the parameters visable outside the class
  enum { PX = tPX, PY = tPY, PZ = tPZ, MX = tMX, MY = tMY, MZ = tMZ };

  typedef double Value;

  // Definitions
  // Rod - a 1x1xN vector in the mesh
  // Bar - a 1xM group of rods where M is the number of contingious rods in a particular plain on a node.

  typedef Value XRod[ MX ];
  typedef Value YRod[ MY ];
  typedef Value ZRod[ MZ ];

  // Figure out max mesh values pre processor in each dimension
  enum { XMaxMeshValuesPerProc = ( MX + ( PX - 1 ) )  / PX };
  enum { YMaxMeshValuesPerProc = ( MY + ( PY - 1 ) )  / PY };
  enum { ZMaxMeshValuesPerProc = ( MZ + ( PZ - 1 ) )  / PZ };

  typedef Value ProcMeshSection[ ZMaxMeshValuesPerProc ][ YMaxMeshValuesPerProc ][ ZMaxMeshValuesPerProc ];

  enum { ZYRodsPerBar = YMaxMeshValuesPerProc };
  enum { YXRodsPerBar = XMaxMeshValuesPerProc };

  // Figure out the number of bars
  enum { ZBarsSharedPerProc = XMaxMeshValuesPerProc };
  enum { YBarsSharedPerProc = ZMaxMeshValuesPerProc };
  enum { XBarsSharedPerProc = YMaxMeshValuesPerProc };

  enum { ZBarsOwnedPerProc = ZBarsSharedPerProc / PZ };
  enum { YBarsOwnedPerProc = YBarsSharedPerProc / PY };
  enum { XBarsOwnedPerProc = XBarsSharedPerProc / PX };

  enum { XRodsSharedPerProc = YMaxMeshValuesPerProc * ZMaxMeshValuesPerProc };
  enum { YRodsSharedPerProc = XMaxMeshValuesPerProc * ZMaxMeshValuesPerProc };
  enum { ZRodsSharedPerProc = XMaxMeshValuesPerProc * YMaxMeshValuesPerProc };

  enum { XRodsOwnedPerProc = XRodsSharedPerProc / PX };
  enum { YRodsOwnedPerProc = YRodsSharedPerProc / PY };
  enum { ZRodsOwnedPerProc = ZRodsSharedPerProc / PZ };

  // Allocate the local memory for bars owned
  XRod ZBarSet[ ZBarsOwnedPerProc ][ ZMaxMeshValuesPerProc ];
  XRod YBarSet[ YBarsOwnedPerProc ][ YMaxMeshValuesPerProc ];
  YRod XBarSet[ XBarsOwnedPerProc ][ XMaxMeshValuesPerProc ];

  int  ZBarSetCompleteCount[ ZBarsOwnedPerProc ];
  int  YBarSetCompleteCount[ YBarsOwnedPerProc ];
  int  XBarSetCompleteCount[ XBarsOwnedPerProc ];

  // Local processor coordinates
  int mXPCoord, mYPCoord, mZPCoord;

  void Check() { }

  struct ActorPacket
    {
    // could just gen the address on th sender side, but for now send the indexs
    FFT   *This;
    int    Bar;
    int    Rod;
    int    RodIndex;
    Value mData[ ZMaxMeshValuesPerProc ];
    };

  static int
  ZPacketActorFx( void *pkt )
    {
    ActorPacket *ap = (ActorPacket *) pkt;
    for( int v = 0; v < ZMaxMeshValuesPerProc; v++ )
      {
      assert( ap->This->ZBarSet[ ap->Bar ][ ap->Rod ][ ap->RodIndex + v ] == 0 );
      ap->This->ZBarSet[ ap->Bar ][ ap->Rod ][ ap->RodIndex + v ] = ap->mData[ v ];
      }
    ap->This->ZBarSetCompleteCount[ ap->Bar ]++;
    return(0);
    }

  static int
  YPacketActorFx( void *pkt )
    {
    ActorPacket *ap = (ActorPacket *) pkt;
    for( int v = 0; v < ZMaxMeshValuesPerProc; v++ )
      {
      assert( ap->This->YBarSet[ ap->Bar ][ ap->Rod ][ ap->RodIndex + v ] == 0 );
      ap->This->YBarSet[ ap->Bar ][ ap->Rod ][ ap->RodIndex + v ] = ap->mData[ v ];
      }
    ap->This->YBarSetCompleteCount[ ap->Bar ]++;
    return(0);
    }

  static int
  XPacketActorFx( void *pkt )
    {
    ActorPacket *ap = (ActorPacket *) pkt;
    for( int v = 0; v < ZMaxMeshValuesPerProc; v++ )
      {
      assert( ap->This->XBarSet[ ap->Bar ][ ap->Rod ][ ap->RodIndex + v ] == 0 );
      ap->This->XBarSet[ ap->Bar ][ ap->Rod ][ ap->RodIndex + v ] = ap->mData[ v ];
      }
    ap->This->XBarSetCompleteCount[ ap->Bar ]++;
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
    Configured = 1;
    };

  void
  DoFFT( ProcMeshSection &aPMS )
    {
////    assert( Configured != 0 );

    printf("*** Collect on Z axis ***\n");

    ActorPacket zap;
    zap.This = this;


    // Probably best to start in Z dim since that will be contigious in C++ data representation
    // For now, just suffer the striding.
    fflush(stdout);
    for(int xm = 0; xm < XMaxMeshValuesPerProc; xm++ )
      {
      for(int ym = 0; ym < YMaxMeshValuesPerProc; ym++ )
        {
        for(int zm = 0; zm < ZMaxMeshValuesPerProc; zm++ )
          {
          zap.mData[ zm ] = aPMS[ xm ][ ym ][ zm ];
          int i;
          printf("ZBar Pkt %8d ", i = aPMS[ xm ][ ym ][ zm ] );
          }
        int XPTarget         = mXPCoord;
        int YPTarget         = mYPCoord;
        int ZPTarget         = xm % PZ;
        int ZBarIndex        = xm % ZBarsOwnedPerProc;
        int ZRodIndex        = ym;
        int ZRodElementIndex = mZPCoord * ZMaxMeshValuesPerProc;
        printf("Trigger to node %2d %2d %2d ZBar %2d ZRod %2d Element %2d\n",
          XPTarget, YPTarget, ZPTarget, ZBarIndex, ZRodIndex, ZRodElementIndex );
        zap.This     = this;
        zap.Bar      = ZBarIndex;
        zap.Rod      = ZRodIndex;
        zap.RodIndex = ZRodElementIndex;
        ZPacketActorFx( (void *) &zap );
        }
      }
    fflush(stdout);

    // Spin-wait for complete Z vectors

    // 1 D FFTs on Z vectors

    for( int bar = 0; bar < ZBarsOwnedPerProc ; bar++ )
      {
      for( int rod = 0; rod < ZYRodsPerBar; rod++ )
        {
        FFT1D::Op( ZBarSet[ bar ][ rod ] );
        }
      }

    // Redistribute int YZ plain

    printf("*** redistribute from Z to Y ***\n");

    ActorPacket yzap;

    for( int bar = 0; bar < ZBarsOwnedPerProc ; bar++ )
      {
      for( int mz = 0; mz < MZ; mz++ )
        {
        for( int rod = 0; rod < ZYRodsPerBar; rod++ )
          {
          int i = yzap.mData[ rod ] = ZBarSet[ bar ][ rod ][ mz ];
          printf("YBar Pkt %8d  ", i  );

          }
        int XPTarget         = mXPCoord;
        int YPTarget         = mz % PY;
        int ZPTarget         = mz / ZMaxMeshValuesPerProc;
        int YBarIndex        = mz % YBarsOwnedPerProc ;
        int YRodIndex        = bar + mZPCoord * YBarsOwnedPerProc ;
        int YRodElementIndex = mYPCoord * YMaxMeshValuesPerProc;
        printf("Trigger to node %2d %2d %2d ZBar %2d ZRod %2d Element %2d\n",
          XPTarget, YPTarget, ZPTarget, YBarIndex, YRodIndex, YRodElementIndex );
        yzap.This     = this;
        yzap.Bar      = YBarIndex;
        yzap.Rod      = YRodIndex;
        yzap.RodIndex = YRodElementIndex;
        YPacketActorFx( (void *) &yzap );
        }
      }

    // Spin-wait for complete Y vectors

    // 1 D FFTs on Y vectors

    for( int bar = 0; bar < ZBarsOwnedPerProc ; bar++ )
      {
      for( int rod = 0; rod < ZYRodsPerBar; rod++ )
        {
        FFT1D::Op( YBarSet[ bar ][ rod ] );
        }
      }

    // Redsitrubute for X

    printf("*** redistribute from Y to X ***\n");

    ActorPacket xyap;

    for( int bar = 0; bar < YBarsOwnedPerProc ; bar++ )
      {
      for( int my = 0; my < MY; my++ )
        {
        for( int rod = 0; rod < YXRodsPerBar; rod++ )
          {
          int i = xyap.mData[ rod ] = YBarSet[ bar ][ rod ][ my ];
          printf("XBar Pkt %8d  ", i  );

          }
        int XPTarget         = my % PX;
        int YPTarget         = my / YMaxMeshValuesPerProc;
        int ZPTarget         = mZPCoord;
        int XBarIndex        = my % YBarsOwnedPerProc ;
        int XRodIndex        = bar + mYPCoord * XBarsOwnedPerProc ;
        int XRodElementIndex = mXPCoord * XMaxMeshValuesPerProc;
        printf("Trigger to node %2d %2d %2d ZBar %2d ZRod %2d Element %2d\n",
          XPTarget, YPTarget, ZPTarget, XBarIndex, XRodIndex, XRodElementIndex );
        xyap.This     = this;
        xyap.Bar      = XBarIndex;
        xyap.Rod      = XRodIndex;
        xyap.RodIndex = XRodElementIndex;
        XPacketActorFx( (void *) &xyap );
        }
      }


    // Spin-wait for complete X vectors

    // 1 D FFTs on X vectors

    for( int bar = 0; bar < ZBarsOwnedPerProc ; bar++ )
      {
      for( int rod = 0; rod < ZYRodsPerBar; rod++ )
        {
        FFT1D::Op( XBarSet[ bar ][ rod ] );
        }
      }

    // Op()

    }

  };

class Op {};
class OneDimFFT {};
//typedef FFT<8,8,8,128,128,128,Op,OneDimFFT> FFT512;
//typedef FFT<2,2,2,32,32,32,Op,OneDimFFT> FFT512;
#define MESH 4
#define PROC 1
#define XM MESH
#define YM MESH
#define ZM MESH
#define XP PROC
#define YP PROC
#define ZP PROC
typedef FFT<XP,YP,ZP,XM,YM,ZM,Op,FFT1D<4,double> > FFT512;

FFT512 fft;

PkMain(int,char**,char**)
  {
  cout << "MX " << FFT512::MX << " MY " << FFT512::MY << " MZ " << FFT512::MZ << endl;
  cout << "PX " << FFT512::PX << " PY " << FFT512::PY << " PZ " << FFT512::PZ << endl;

  cout << "Number of mesh values total " <<                          FFT512::MX * FFT512::MY * FFT512::MZ << endl;
  cout << "Memory of mesh values total " <<  sizeof(FFT512::Value) * FFT512::MX * FFT512::MY * FFT512::MZ << endl;

  cout << "XMaxMeshValuesPerProc " << FFT512::XMaxMeshValuesPerProc << endl;
  cout << "YMaxMeshValuesPerProc " << FFT512::YMaxMeshValuesPerProc << endl;
  cout << "ZMaxMeshValuesPerProc " << FFT512::ZMaxMeshValuesPerProc << endl;

  cout << "Mesh Values Per Packet " << PACKET_SIZE / sizeof( FFT512::Value ) << endl;

  cout << "Number of mesh values per node "
       <<                         FFT512::XMaxMeshValuesPerProc * FFT512::YMaxMeshValuesPerProc * FFT512::ZMaxMeshValuesPerProc << endl;
  cout << "Memory of mesh values per node (bytes) "
       << sizeof(FFT512::Value) * FFT512::XMaxMeshValuesPerProc * FFT512::YMaxMeshValuesPerProc * FFT512::ZMaxMeshValuesPerProc << endl;

  cout << "XRodsSharedPerProc " << FFT512::XRodsSharedPerProc << endl;
  cout << "XRodsOwnedPerProc "       << FFT512::XRodsOwnedPerProc       << endl;

  cout << "ZBarsSharedPerProc " << FFT512::ZBarsSharedPerProc << endl;
  cout << "YBarsSharedPerProc " << FFT512::YBarsSharedPerProc << endl;
  cout << "XBarsSharedPerProc " << FFT512::XBarsSharedPerProc << endl;

  cout << "ZBarsOwnedPerProc " << FFT512::ZBarsOwnedPerProc << endl;
  cout << "YBarsOwnedPerProc " << FFT512::YBarsOwnedPerProc << endl;
  cout << "XBarsOwnedPerProc " << FFT512::XBarsOwnedPerProc << endl;

  cout << "sizeof( ZBarSet ) (bytes) " << sizeof( FFT512::ZBarSet ) << endl;
  cout << "sizeof( YBarSet ) (bytes) " << sizeof( FFT512::YBarSet ) << endl;
  cout << "sizeof( XBarSet ) (bytes) " << sizeof( FFT512::XBarSet ) << endl;

  FFT512::ProcMeshSection pms;

  // Load up some recognizable coords as values of PMS
  for(int xm = 0; xm < FFT512::XMaxMeshValuesPerProc; xm++ )
    {
    for(int ym = 0; ym < FFT512::YMaxMeshValuesPerProc; ym++ )
      {
      for(int zm = 0; zm < FFT512::ZMaxMeshValuesPerProc; zm++ )
        {
        pms[ xm ][ ym ][ zm ] = xm * 0x100 + ym * 0x10 + zm ;
        }
      }
    }

  // TEST
  // Whole 3D array
  double TVol[ XM ][ YM ][ ZM ];
  // Initialize
  for(int xm = 0; xm < XM; xm++ )
    {
    for(int ym = 0; ym < YM; ym++ )
      {
      for(int zm = 0; zm < ZM; zm++ )
        {
        TVol[ xm ][ ym ][ zm ] = xm * 0x100 + ym * 0x10 + zm ;
        }
      }
    }
  // Z FFTs
  for(int xm = 0; xm < XM; xm++ )
    {
    for(int ym = 0; ym < YM; ym++ )
      {
      FFT1D<ZM,double>::Op( TVol[ xm ][ ym ] );
      }
    }
  // Y FFTs
  for(int xm = 0; xm < XM; xm++ )
    {
    for(int zm = 0; zm < ZM; zm++ )
      {
      double yv[ YM ];
      for(int ym = 0; ym < YM; ym++ )
        {
        yv[ ym ] = TVol[ xm ][ ym ][ zm ];
        }
      FFT1D<YM,double>::Op( yv );
      for(int ym = 0; ym < YM; ym++ )
        {
        TVol[ xm ][ ym ][ zm ] = yv[ ym ];
        }
      }
    }
  // X FFTs
  for(int ym = 0; ym < ZM; ym++ )
    {
    for(int zm = 0; zm < XM; zm++ )
      {
      double xv[ YM ];
      for(int xm = 0; xm < YM; xm++ )
        {
        xv[ xm ] = TVol[ xm ][ ym ][ zm ];
        }
      FFT1D<XM,double>::Op( xv );
      for(int xm = 0; xm < YM; xm++ )
        {
        TVol[ xm ][ ym ][ zm ] = xv[ xm ];
        }
      }
    }
  // print results
  for(int xm = 0; xm < XM; xm++ )
    {
    for(int ym = 0; ym < YM; ym++ )
      {
      for(int zm = 0; zm < ZM; zm++ )
        {
        printf(" X %3d Y %3d Z %3d  Value %8.0f \n", xm, ym, zm, TVol[ xm ][ ym ][ zm ]);
        }
      }
    }
  fflush(stdout);



  cout << "sizeof( ProcMeshSection ) " << sizeof( pms ) << endl;

  for( int x = 0; x < XP; x++ )
  for( int y = 0; y < YP; y++ )
  for( int z = 0; z < ZP; z++ )
    {
    printf("\n\n* * * * Node %2d %2d %2d * * * *\n", x, y, z );
    fft.Configure( x, y, z );
    fft.DoFFT( pms );
    }

  return(0);
  }
