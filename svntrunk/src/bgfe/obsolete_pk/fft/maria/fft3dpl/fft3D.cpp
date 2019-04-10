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
 ///#include <iostream>
using namespace std;
#include "fft3D.hpp"
////#include <mpi.h>
#include <assert.h>
#include "fftw.h"
#include "math.h"
#include <stdlib.h>
#include <unistd.h>

#ifndef PKFXLOG_DEBUG
#define PKFXLOG_DEBUG 0
#endif

template class FFT3D<FFTOneD>;

//typedef void* Bit32;
void
BGLTorusSimpleActivePacketSend( _BGL_TorusPktHdr &pktHdr, Bit32 ActorFx, 
                                int x, int y, int z, int len, Bit32 data )
  {
  // NEED TO SHORT CUT LOCAL SENDS
  _BGL_TorusPkt    Pkt;
  ///  int chunks = 7;
  Pkt.hdr = pktHdr;
  char *str = (char *) Pkt.payload;

//   BGLTorusMakeHdr( & Pkt.hdr,               // Filled in on return
//                      x,y,z,                 // destination coordinates
//                      0,          //???      // destination Fifo Group
//                      ActorFx,       // Active Packet Function matching signature above
//                      0,                     // primary argument to actor
//                      0,                     // secondary 10bit argument to actor
//                      _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

  memcpy( Pkt.payload, (const void*) data, len );
  int chunk_count = (len +  _BGL_TORUS_SIZEOF_CHUNK - 1) /  _BGL_TORUS_SIZEOF_CHUNK;

  int rc = 0;

  // local actors to occur on caller's stack
  int myPx, myPy, myPz;
  BGLPartitionGetCoords( &myPx, &myPy, &myPz );

  if( ( x == myPx ) && ( y == myPy ) && ( z == myPz ) )
    {
    ((BGLTorusBufferedActivePacketHandler)(ActorFx))( & Pkt.payload, Pkt.hdr.swh1.arg, Pkt.hdr.swh0.extra );
    }
  else
    {
    BGLTorusSendPacketUnaligned( & Pkt.hdr ,      // Previously created header to use
                                   Pkt.payload,     // Source address of data (16byte aligned)
                                   len ); // Payload bytes = ((chunks * 32) - 16)
    }
  }

#if 0
void
BGLTorusSimpleActivePacketSendTemp( void *ActorFx, int x, int y, int z, int len, void *data )
  {
  // NEED TO SHORT CUT LOCAL SENDS
  _BGL_TorusPkt    Pkt;
    ///  int chunks = 7;
  /// Pkt.hdr = pktHdr;
  //  char *str = (char *) Pkt.payload;

   BGLTorusMakeHdr( & Pkt.hdr,               // Filled in on return
                      x,y,z,                 // destination coordinates
                      0,          //???      // destination Fifo Group
                      ActorFx,       // Active Packet Function matching signature above
                      0,                     // primary argument to actor
                      0,                     // secondary 10bit argument to actor
                      _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)

  memcpy( Pkt.payload, data, len );
  int chunk_count = (len +  _BGL_TORUS_SIZEOF_CHUNK - 1) /  _BGL_TORUS_SIZEOF_CHUNK;

  int rc = 0;

  // local actors to occur on caller's stack
  int myPx, myPy, myPz;
  BGLPartitionGetCoords(&myPx, &myPy, &myPz);

  if( ( x == myPx ) && ( y == myPy ) && ( z == myPz ) )
    //  if( (x == BS.mMyX) && (y == BS.mMyY) && (z == BS.mMyZ) )
    {
    ((BGLTorusBufferedActivePacketHandler)(ActorFx))( & Pkt.payload, Pkt.hdr.swh1.arg, Pkt.hdr.swh0.extra );
    }
  else
    {
    BGLTorusSendPacketUnaligned( & Pkt.hdr ,      // Previously created header to use
                                   Pkt.payload,     // Source address of data (16byte aligned)
                                   len ); // Payload bytes = ((chunks * 32) - 16)
    }
  }
#endif

#include <sys/atomic_op.h>

void
ImportFence()
  {
  int a = 0 ;
  (void) _check_lock(&a, 0, 0) ;
  }

void
ExportFence()
   {
   int a;
  _clear_lock(&a,0) ;
  }

template<class FFT_T> 
FFT3D<FFT_T> :: FFT3D( unsigned int aLocalN_x,
                       unsigned int aLocalN_y,
                       unsigned int aLocalN_z,
                       unsigned int aP_x,
                       unsigned int aP_y,
                       unsigned int aP_z,
                       int aFFTDir ):  // TODO: make this general
  mLocalN_x(aLocalN_x), mLocalN_y(aLocalN_y), mLocalN_z(aLocalN_z),
  mP_x(aP_x), mP_y(aP_y), mP_z(aP_z), mFFTDirection(aFFTDir) 
{

  //  mComm = new COMM_T( mP_x, mP_y, mP_z, MPI_COMM_WORLD );  
  
  mBarSize_x = mLocalN_y / mP_x; // size of bars in the y-dim
  mBarSize_y = mLocalN_z / mP_y; // size of bars in the z-dim
  mBarSize_z = mLocalN_x / mP_z; // size of bars in the x-dim

//   BegLogLine( PKFXLOG_DEBUG ) 
//       <<" Size of barZ: " <<  mBarSize_x
//       <<" Size of barY: " <<  mBarSize_y
//       <<" Size of barX: " <<  mBarSize_z
//       << EndLogLine;

  mCompletePacket_x = new int[ mBarSize_x * mLocalN_z ];
  mCompletePacket_y = new int[ mBarSize_y * mLocalN_x ];
  mCompletePacket_z = new int[ mBarSize_z * mLocalN_y ];

  mCompletePacket_b = (int **) new int[ mLocalN_x ];
  for(int i=0; i < mLocalN_x; i++)
    mCompletePacket_b[ i ] = new int[ mLocalN_z ];
  
  for(int i=0; i < mLocalN_x; i++)
    for(int k=0; k < mLocalN_z; k++)
      {
        mCompletePacket_b[ i ][ k ] = 0;
      }
  
  for(int i=0; i < mBarSize_x * mLocalN_z; i++) 
    mCompletePacket_x[ i ] = 0;
  
  for(int i=0; i < mBarSize_y * mLocalN_x; i++)
    mCompletePacket_y[ i ] = 0;

  for(int i=0; i < mBarSize_z * mLocalN_y; i++)
    mCompletePacket_z[ i ] = 0;
  
  mCompleteFFTsInBarCount_x = 0;
  mCompleteFFTsInBarCount_y = 0;
  mCompleteFFTsInBarCount_z = 0;
  
  mCompleteFFTsInBarCount_b = new int[ mLocalN_x ];

  for(int i=0; i < mLocalN_x; i++)
    mCompleteFFTsInBarCount_b[ i ] = 0;
  
  mPointsPerPacket_x = min( mBarSize_z, ELEMENTS_PER_PACKET );
  mPointsPerPacket_y = min( mBarSize_x, ELEMENTS_PER_PACKET );
  mPointsPerPacket_z = min( mLocalN_z, ELEMENTS_PER_PACKET );
  mPointsPerPacket_b = min( mLocalN_y, ELEMENTS_PER_PACKET );
  
  mPacketsPerFFT_x = (int) ( mP_x * ceil( (double)mLocalN_x / (double)mPointsPerPacket_x ));
  mPacketsPerFFT_y = (int) ( mP_y * ceil( (double)mLocalN_y / (double)mPointsPerPacket_y ));
  mPacketsPerFFT_z = (int) ( mP_z * ceil( (double)mLocalN_z / (double)mPointsPerPacket_z ));
  mPacketsPerFFT_b = (int) (        ceil( (double)mLocalN_y / (double)mPointsPerPacket_b ));
 
  mFFT_x = new FFT_T( mLocalN_x * mP_x, mFFTDirection );  // 1D fft in the x-dim
  mFFT_y = new FFT_T( mLocalN_y * mP_y, mFFTDirection );  // 1D fft in the y-dim
  mFFT_z = new FFT_T( mLocalN_z * mP_z, mFFTDirection );  // 1D fft in the z-dim
  
  int maxBarSz   = max( max( mBarSize_x, mBarSize_y ), mBarSize_z);
  int maxLocalN  = max( max( mLocalN_x, mLocalN_y ), mLocalN_z );
  int maxGlobalN = max( max( mLocalN_x * mP_x, mLocalN_y * mP_y ), mLocalN_z * mP_z);
  int maxBarNum  = max( max( mP_x, mP_y ), mP_z);
  
  if( maxBarSz == 0 || maxLocalN == 0 || maxGlobalN == 0 || maxBarNum == 0 )
    {
      maxBarSz += 1;
      printf( " Error in the allocation\n") ;
      printf( " maxBarSz = %d\n", maxBarSz );
      printf( " barSzX = %d\n", mBarSize_x );     
      printf( " barSzY = %d\n", mBarSize_y );    
      printf( " barSzZ = %d\n", mBarSize_z );
      printf( " maxLocalN = %d\n", maxLocalN );
      printf( " maxGlobalN = %d\n", maxGlobalN );
      printf( " maxBarNum  = %d\n", maxBarNum );
  }

  mGlobalIn_x  = (complex**) new complex[ mBarSize_x * mLocalN_z ];
  mGlobalIn_z  = (complex**) new complex[ mBarSize_z * mLocalN_y ];

  mGlobalOut_x = (complex**) new complex[ mBarSize_x * mLocalN_z ];
  mGlobalOut_z = (complex**) new complex[ mBarSize_z * mLocalN_y ];

  mGlobalIn_y  = (complex**) new complex[ mBarSize_y * mLocalN_x ];
  // mGlobalIn_b  = (complex**) new complex[ mBarSize_y * mLocalN_x ];

  mGlobalOut_y = (complex**) new complex[ mBarSize_y * mLocalN_x ];
  // mGlobalOut_b = (complex**) new complex[ mBarSize_y * mLocalN_x ];


  for(int i=0; i < mLocalN_z * mBarSize_x; i++) 
    {
      mGlobalIn_x[ i ]  = (complex*) new complex[ mLocalN_x * mP_x ];
      mGlobalOut_x[ i ] = (complex*) new complex[ mLocalN_x * mP_x ];
    }

  for(int i=0; i < mLocalN_y * mBarSize_z; i++) 
    {
      mGlobalIn_z[ i ]  = (complex*) new complex[ mLocalN_z * mP_z ];
      mGlobalOut_z[ i ] = (complex*) new complex[ mLocalN_z * mP_z ];
    }

   for(int i=0; i < mLocalN_x * mBarSize_y; i++) 
     {
       mGlobalIn_y[i]  = (complex*) new complex[ mLocalN_y * mP_y ];
       mGlobalOut_y[i] = (complex*) new complex[ mLocalN_y * mP_y ];
       // mGlobalIn_b[i]  = (complex*) new complex[ mLocalN_y * mP_y ];
       // mGlobalOut_b[i] = (complex*) new complex[ mLocalN_y * mP_y ];   
     }

  // mLocalIn  = (complex***) new complex[ mLocalN_x ];
  mLocalOut = (complex***) new complex[ mLocalN_x ];

  for(int i=0; i<mLocalN_x; i++) 
    {
      // mLocalIn[i]  = (complex**) new complex[ mLocalN_y ];
      mLocalOut[ i ] = (complex**) new complex[ mLocalN_y ];

      for(int j=0; j<mLocalN_y; j++) 
        {
          // mLocalIn[i][j]  = (complex*) new complex[ mLocalN_z ];
          mLocalOut[ i ][ j ] = (complex*) new complex[ mLocalN_z ];
        }
    }
}

//#define DEBUG_Z 1
template<class FFT_T>
int  FFT3D<FFT_T>::PacketActorFx_Z(void* pkt) 
{
  ActorPacket *ap = (ActorPacket*) pkt;

  FFT3D<FFT_T> *This = (*ap).mHdr.This;

  int a = (*ap).mHdr.aIndex;
  int b = (*ap).mHdr.bIndex;
  ////int myPx, myPy, myPz;
  int globalNz = (*This).mLocalN_z * (*This).mP_z;

//   BegLogLine(0)
//     << "PacketActorFx_Z "
//     << "Before the Increment the packet-counter[ "<< a <<"]=" 
//     //    << (*This).mCompletePacket_z[ a ]
//     << (*This).mCompletePacket_z[ a ]
//     << EndLogLine;
 
//   ////  BGLPartitionGetCoords(&myPx, &myPy, &myPz);
 
//   BegLogLine(0)
//     << "PacketActorFx_Z "
//     << "After the Increment the packet-counter[ " << a << "]=" 
//     << (*This).mCompletePacket_z[ a ]
//         << EndLogLine;
  
  int zPointsPerPacket = (*This).mPointsPerPacket_z;
    if( (globalNz-b) % (*This).mLocalN_z != 0 
     && (globalNz-b) < (*This).mPointsPerPacket_z)
    {
      zPointsPerPacket = (globalNz-b) % (*This).mPointsPerPacket_z;
    }
  
//   BegLogLine( 0 )
//     << "PacketActorFx_Z:: zPointsPerPacket  "
//     << zPointsPerPacket 
//     << EndLogLine;

  for(int k=0; k < zPointsPerPacket; k++) 
    {
      if( (b+k) >= globalNz ) 
        {
            printf("====Error inside PacketActorFx_Z %d %d\n", b+k, globalNz);
            //            exit( -1 );
            return -1;
        }
      
      (*This).mGlobalIn_z[ a ][ b+k ].re = (*ap).mData[ k ].re;
      (*This).mGlobalIn_z[ a ][ b+k ].im = (*ap).mData[ k ].im;
      
//       BegLogLine(0)
//           << "ZPacket::"
//           << "Recv a packet( "
//           <<  (*This).mMyP_z << " ab " << a
//           <<","<< b+k <<") =  "
//           << (*This).mGlobalIn_z[ a ][ b+k ].re 
//           << ",  "
//           << (*This).mGlobalIn_z[ a ][ b+k ].im 
//           << EndLogLine;
    }
  
  ImportFence();
  (*This).mCompletePacket_z[ a ] ++;
  ExportFence();

  ///  ImportFence();
  
  int cmpl = (*This).mCompletePacket_z[ a ]; 

  if( cmpl == (*This).mPacketsPerFFT_z )
    {
//       BegLogLine( 0 )
// 	<< " PacketActorFx_Z "
// 	<< " Got " << (*This).mPacketsPerFFT_z << " of " << cmpl
// 	<< " DOING FFT "
// 	<< EndLogLine;

//       for(int b=0; b < zPointsPerPacket; b++)
// 	{
// 	  BegLogLine( 0 )
// 	    << "ZPacket::"
// 	    << "BeforeFFT "
// 	    <<  (*This).mMyP_z << " ab "<< a
// 	    <<"," << b << "  "
// 	    <<  (*This).mGlobalIn_z[ a ][ b ].re
// 	    << EndLogLine;
// 	}
      
      ((*This).mFFT_z)->fftInternal( (*This).mGlobalIn_z[ a ],
                                     (*This).mGlobalOut_z[ a ] );
      
//       for(int b=0; b < zPointsPerPacket; b++)
// 	{
// 	  BegLogLine( 0 )
// 	    << "ZPacket::"
// 	    << "AfterFFT "
// 	    <<  (*This).mMyP_z << " ab " << a
// 	    << "," << b << "  " << "("
// 	    << (*This).mGlobalIn_z[ a ][ b ].re << "," 
// 	    << (*This).mGlobalIn_z[ a ][ b ].im << "," 
// 	    << EndLogLine;
// 	}

      (*This).mCompleteFFTsInBarCount_z += 1;
	
//       BegLogLine( 0 )
// 	<< "PacketActorFx_Z "
// 	<< " Done incremented mCompleteFFTsInBarCount_z to "
// 	<< (*This).mCompleteFFTsInBarCount_z
// 	<< EndLogLine;
    }
  return 0;
}


template<class FFT_T>
void FFT3D<FFT_T>::GatherZ(complex *** in, int dim) 
{
//   int myPx, myPy, myPz;
//   BGLPartitionGetCoords( &myPx, &myPy, &myPz);

  ActorPacket zap;
  
  int count = 0;
  int headerCount = 0;
  for(int i=0; i < mLocalN_x; i++) 
    {
      int trgPz = i / mBarSize_z;
        
      for(int j=0; j < mLocalN_y; j++) 
        {
          int loadCountZ = 0;
          int zPointsPerPacket = mPointsPerPacket_z;
                
          for(int k =0; k < mLocalN_z; k++) 
            {
              zap.mData[ loadCountZ ].re = in[ i ][ j ][ k ].re;
              zap.mData[ loadCountZ ].im = in[ i ][ j ][ k ].im;
              loadCountZ++;
              
              if( ( k == mLocalN_z-1 ) && ( loadCountZ < mPointsPerPacket_z ) )
                {
                  // Points in the last packet
                  zPointsPerPacket = mLocalN_z % mPointsPerPacket_z;
                }
              
              if( loadCountZ == zPointsPerPacket ) 
                {
//                   zap.mHdr.This = this;
//                   zap.mHdr.aIndex = ( i % mBarSize_z ) * mLocalN_y + j;
//                   zap.mHdr.bIndex = k+1 - loadCountZ + myPz * mLocalN_z;
                  
//                   for(int kk=0; kk < zPointsPerPacket; kk++)
//                     BegLogLine(0)
//                         << "GatherZ::"
//                         << "Sending a packet to proc "
//                         <<  trgPz  << " ab " << (i % mBarSize_z) * mLocalN_y + j
//                         << "," << k+1 - loadCountZ + myPz * mLocalN_z <<" data( "
//                         << zap.mData[ kk ].re << "," 
//                         << zap.mData[ kk ].im << ")"
//                         << EndLogLine;
                  zap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mFFT_Hdr;

                  BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mBGLTorus_Hdr,
                                                  (Bit32) PacketActorFx_Z,
                                                  mMyP_x,
                                                  mMyP_y,
                                                  trgPz,
                                                  sizeof( zap ),
                                                  (Bit32) &zap);
                  headerCount++;
                  loadCountZ = 0;
                }
            }
        }
    }
}

template<class FFT_T>
int  FFT3D<FFT_T>::PacketActorFx_X(void* pkt) 
{
  ActorPacket *ap = (ActorPacket*) pkt;
  FFT3D< FFT_T > *This = (*ap).mHdr.This;
  int a = (*ap).mHdr.aIndex;
  int b = (*ap).mHdr.bIndex;

//   int myPx, myPy, myPz;
//   BGLPartitionGetCoords( &myPx, &myPy, &myPz);

//   BegLogLine(0)
//     << "PacketActorFx_X "
//     << "Before the Increment the packet-counterX[ "<< a <<"]=" 
//     <<(*This).mCompletePacket_x[ a ]
//     << EndLogLine;

  int globalNx = (*This).mLocalN_x * (*This).mP_x;
  int xPointsPerPacket = (*This).mPointsPerPacket_x;

  if((globalNx-b) % (*This).mLocalN_x !=0 
     && (globalNx-b) < (*This).mPointsPerPacket_x)
    {
      xPointsPerPacket = (globalNx-b) % (*This).mPointsPerPacket_x;
    }
  
  for(int i=0; i < xPointsPerPacket; i++) 
    {
       if( (b+i) == globalNx )
 	{
 	  printf( "XPacket::wrong index %d , %d\n" , b+i, (*This).mLocalN_x);
          return -1;
 	}
            
       (*This).mGlobalIn_x[ a ][ b+i ] = (*ap).mData[ i ];
       //       BegLogLine(0) << "mGlobalIn_x[" << a <<" ]["<< b+i << "] ="
       // 		    << (*This).mGlobalIn_x[a][b+i].re <<" , "
       // 		    << (*This).mGlobalIn_x[a][b+i].im <<" )  "
       // 		    << "  xCompletePacket[a]" 
       // 		    << ((*This).mCompletePacket_x[a])<<"  "
       // 		    << (*This).mPacketsPerFFT_x 
       // 		    << EndLogLine(1);
    }
  
  //  ImportFence();
  ImportFence();
  (*This).mCompletePacket_x[ a ] ++;// (*This).mPointsPerPacket_x;
  ExportFence();

  int cmpl = ((*This).mCompletePacket_x[ a ]);
  
  if( cmpl == (*This).mPacketsPerFFT_x )
    {
      ((*This).mFFT_x)->fftInternal( (*This).mGlobalIn_x[ a ], 
                                     (*This).mGlobalOut_x[ a ] );

     (*This).mCompleteFFTsInBarCount_x++;
//      BegLogLine(0) << "After the Increment the fft-counterX = "
//                    << (*This).mCompleteFFTsInBarCount_x
//                    << EndLogLine;
    }
  
  return 0;
}

template<class FFT_T>
void FFT3D<FFT_T>::GatherX(complex ** in, int dim) 
{
//   int myPx, myPy, myPz;
//   BGLPartitionGetCoords( &myPx, &myPy, &myPz);
  ActorPacket xap;

  int loadCountX;
  //  int count=0;
  int headerCount=0;
 
  for(int trgPz=0; trgPz < mP_z; trgPz++) 
    {
      for(int trgPx=0; trgPx < mP_x; trgPx++) 
	{
	  for(int ax=0; ax < mLocalN_z * mBarSize_x; ax++) 
	    {
	      loadCountX=0;
	      for(int bx = mMyP_z * mBarSize_z + mMyP_x * mLocalN_x; 
		      bx < ( mMyP_z+1 ) * mBarSize_z + mMyP_x * mLocalN_x; 
                      bx++) 
                  {
                    int az = ((bx%mLocalN_x)%mBarSize_z)*mLocalN_y+ax/mLocalN_z + trgPx*mBarSize_x;
                    int bz = ax%mLocalN_z + mLocalN_z*trgPz;

//                     BegLogLine(0) << "ax = "<< ax << " bx = "<< bx << EndLogLine;
//                     BegLogLine(0) << "az = "<< az << " bz = "<< bz << EndLogLine;

//                     BegLogLine(0) << "_GatherX::mGlobalOut_ztoX[" << az <<"][" 
//                                   << bz << "]" 
//                                   << mGlobalOut_z[ az ][ bz ].re <<","
//                                   << mGlobalOut_z[ az ][ bz ].im
//                                   << EndLogLine;
                    
                    xap.mData[ loadCountX ].re = mGlobalOut_z[ az ][ bz ].re;
                    xap.mData[ loadCountX ].im = mGlobalOut_z[ az ][ bz ].im;
                    loadCountX++;

                    int xPointsPerPacket =  mPointsPerPacket_x;
                    
		if( ( bx == (mMyP_z + 1) * mBarSize_z + mMyP_x * mLocalN_x-1 ) 
		   && (loadCountX<mPointsPerPacket_x))
		  {
		    xPointsPerPacket = 
		      ( (mMyP_z+1) * mBarSize_z + mMyP_x * mLocalN_x ) % mPointsPerPacket_x;
		  }

		if( loadCountX == xPointsPerPacket ) 
		  {
// 		    xap.mHdr.This = this;
// 		    xap.mHdr.aIndex = ax;
// 		    xap.mHdr.bIndex = bx - loadCountX + 1;
// 		    xap.mHdr.cIndex = 0;

                    xap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mFFT_Hdr;
                    
                    BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mBGLTorus_Hdr,
                                                    (Bit32) PacketActorFx_X,
                                                    trgPx,
                                                    mMyP_y,
                                                    trgPz,
                                                    sizeof( xap ),
                                                    (Bit32) &xap );
		    loadCountX = 0;
                    //                    count++;
                    headerCount++;
		  }
	      }
	    }
	}
    }
}

template<class FFT_T>
int  FFT3D<FFT_T>::PacketActorFx_Y(void* pkt) 
{
  ActorPacket *ap = (ActorPacket*) pkt;
  FFT3D<FFT_T> *This = (*ap).mHdr.This;
  int a = (*ap).mHdr.aIndex;
  int b = (*ap).mHdr.bIndex;

//   int myPx, myPy, myPz;
//   BGLPartitionGetCoords(&myPx, &myPy, &myPz);

  for(int i=0;  i<(*This).mPointsPerPacket_y; i++) 
    {
      if( b + i == (*This).mLocalN_y * (*This).mP_y)  
          break;

      (*This).mGlobalIn_y[ a ][ b+i ] = (*ap).mData[ i ];
    }


  ImportFence();
  (*This).mCompletePacket_y[a] ++; // (*This).mPointsPerPacket_x;
  ExportFence();

  int cmpl = ((*This).mCompletePacket_y[a]);

  //  ImportFence();
  if(cmpl == (*This).mPacketsPerFFT_y )
    {
      ((*This).mFFT_y)->fftInternal((*This).mGlobalIn_y[ a ],
                                    (*This).mGlobalOut_y[ a ]);
      (*This).mCompleteFFTsInBarCount_y++;        
    }

  return 0;
}

template< class FFT_T >
void FFT3D< FFT_T >::GatherY(complex ** in, int dim) 
{
//   int myPx, myPy, myPz;
//   BGLPartitionGetCoords(&myPx, &myPy, &myPz);

  ActorPacket yap;
  int loadCountY;
  int headerCount=0;
  
  for(int trgPx=0; trgPx<mP_x; trgPx++) 
    {
      for(int trgPy=0; trgPy<mP_y; trgPy++) 
        {
          for(int ay=0; ay < mLocalN_x * mBarSize_y; ay++) 
            {
              loadCountY = 0;
              for(int by = mMyP_x * mBarSize_x  + mMyP_y * mLocalN_y;
                      by < (mMyP_x+1) * mBarSize_x + mMyP_y * mLocalN_y; 
                      by ++ ) 
                {
                  int ax = ( ( by % mLocalN_y ) % mBarSize_x ) * mLocalN_z +
                      ay / mLocalN_x + trgPy * mBarSize_y; 

                  int bx = ay % mLocalN_x + mLocalN_x * trgPx;

                  yap.mData[ loadCountY ].re = mGlobalOut_x[ ax ][ bx ].re;
                  yap.mData[ loadCountY ].im = mGlobalOut_x[ ax ][ bx ].im;
                  loadCountY++;

//                   BegLogLine( 0 ) << "ax = "<< ax << " bx = "<< bx << EndLogLine;
//                   BegLogLine( 0 ) << "ay = "<< ay << " bz = "<< by << EndLogLine;
//                   BegLogLine( 0 ) << "_GatherY::mGlobalOut_xtoY[" << ax <<"][" 
// 			<< bx<< "]" 
// 			<< mGlobalOut_x[ ax ][ bx ].re <<","
// 			<< mGlobalOut_x[ ax ][ bx ].im
// 			<< EndLogLine;
                  
//                   BegLogLine(0) << "GatherY::mPointsPerPacket_y "
//                                 << mPointsPerPacket_y << " , "
//                                 << loadCountY << "  , " 
//                                 << "barSzY " << mBarSize_y
//                                 << EndLogLine;

                  if( loadCountY == mPointsPerPacket_y ) 
                    {
//                       yap.mHdr.This = this;
//                       yap.mHdr.aIndex = ay;
//                       yap.mHdr.bIndex = by - loadCountY+1;

//                       for(int kk=0; kk < mPointsPerPacket_y; kk++)
//                           BegLogLine( 0 )
//                               << "GatherY::"
//                               << "Sending a packet to procXY  " << trgPx <<","
//                               <<  trgPy  << " ab "<< ay << ","<< by-loadCountY+1
//                               << yap.mData[ kk ]
//                               << EndLogLine;
                      
                      yap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mFFT_Hdr;
                      
                      BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mBGLTorus_Hdr,
                                                      (Bit32) PacketActorFx_Y,
                                                      trgPx,
                                                      trgPy,
                                                      mMyP_z,
                                                      sizeof( yap ),
                                                      (Bit32) &yap );
                      loadCountY = 0;
                      headerCount++;
                    }
                }
            }
        }
    }
}


template<class FFT_T>
int  FFT3D<FFT_T>::PacketActorFx_B(void* pkt) 
{
  ActorPacket *ap = (ActorPacket*) pkt;
  FFT3D<FFT_T> *This = (*ap).mHdr.This;
  int i = (*ap).mHdr.aIndex;
  int j = (*ap).mHdr.bIndex;
  int k = (*ap).mHdr.cIndex;
 
//   int myPx, myPy, myPz;
//   BGLPartitionGetCoords( &myPx, &myPy, &myPz);
  
  for(int jp=0;  jp < (*This).mPointsPerPacket_b; jp++) 
    {
      (*This).mLocalOut[i][j+jp][k] = (*ap).mData[jp];
      
//       BegLogLine(0) 
//           << "_OutB[" << i <<" ]["
//           << j +jp    << "][" << k << "] ="
//           << (*This).mLocalOut[i][j+jp][k].re <<" , "
//           << (*This).mLocalOut[i][j+jp][k].im <<" )"
//           << EndLogLine;
    }
  
//   BegLogLine(0)
//       << "bCompletePacket[" << i <<"][" << k << " ]="
//       << (*This).mCompletePacket_b[ i ][ k ] << "    "
//       << (*This).mPacketsPerFFT_b 
//       << EndLogLine;
  
  //ImportFence();
  ImportFence();
  ( (*This).mCompletePacket_b[ i ][ k ] )++;// (*This).mPointsPerPacket_x;
  ExportFence();

  int cmpl = ((*This).mCompletePacket_b[ i ][ k ]);
  
  if( cmpl ==(*This).mPacketsPerFFT_b )
    { 
      (*This).mCompleteFFTsInBarCount_b[ i ]++;

//       BegLogLine(0) 
//           << "bCompleteFFTs[" << i << "]="
//           << (*This).mCompleteFFTsInBarCount_b[ i ]
//           << EndLogLine;

  //     // for(int i=0; i<(*This).mLocalN_x; i++)
// // 	{
// 	  for(int k=0; k<(*This).mLocalN_z; k++){ 	 
// 	      (*This).mCompletePacket_b[i][k]=0;
// 	  }
// // 	}

    }
  return 0;
}

template<class FFT_T>
void FFT3D<FFT_T>::GatherB(complex ***out, int dim) 
{
//   int myPx, myPy, myPz;
//   BGLPartitionGetCoords(&myPx, &myPy, &myPz);
  ActorPacket bap;
  int headerCount=0;
  
  for(int a=0; a < ( mLocalN_z / mP_y ) * mLocalN_x; a++)
    {
      int loadCount = 0;
      int i = a % mLocalN_x;
      int k = a / mLocalN_x + mMyP_y * mBarSize_y;

      for(int b=0; b < mLocalN_y * mP_y; b++) 
	{
	  int j = b % mLocalN_y;
	  int trgPy = b / mLocalN_y;

	  bap.mData[ loadCount ].re = mGlobalOut_y[ a ][ b ].re;
	  bap.mData[ loadCount ].im = mGlobalOut_y[ a ][ b ].im;
	  loadCount++;

	  if( loadCount == mPointsPerPacket_b )
	    {
	      bap.mHdr.This = this;
	      bap.mHdr.aIndex = a % mLocalN_x;
	      bap.mHdr.bIndex = b % mLocalN_y - loadCount+1;
	      bap.mHdr.cIndex = a / mLocalN_x + mMyP_y * mBarSize_y;
	     
// 	      BegLogLine(0) 
//                   << "GatherB::mPointsPerPacket_b "
//                   << mPointsPerPacket_b << " , "
//                   << loadCount << "  , " 
//                   << "localNy" <<mLocalN_y
//                   << EndLogLine;
              
              bap.mHdr = mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mFFT_Hdr;
              
              BGLTorusSimpleActivePacketSend( mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mBGLTorus_Hdr,
                                              (Bit32) PacketActorFx_B,
                                              mMyP_x,
                                              trgPy,
                                              mMyP_z,
                                              sizeof( bap ),
                                              (Bit32) &bap );
	      loadCount = 0;
              headerCount++;
	    }
	}
    }
}


template<class FFT_T>
void FFT3D<FFT_T>::ActiveMsgBarrier(int &completeFFTsInBarCount,
                                    int barSz) 
{
//   int myPx, myPy, myPz;
  
//   BGLPartitionGetCoords( &myPx, &myPy, &myPz);
  int barMon = -1;
  
//   BegLogLine( 1 ) 
//       << "ActiveMsgBarrier BEGIN, have " << completeFFTsInBarCount
//       << " of " << barSz 
//       << EndLogLine;
  
  while( completeFFTsInBarCount < barSz )
    {
      ImportFence();
      
//       if( barMon != completeFFTsInBarCount )
//         {
//           BegLogLine( 1 ) 
//               <<" ActiveMsgBarrier : waiting, have "
//               << completeFFTsInBarCount
//               << " of "
//               << barSz
//               << EndLogLine;

//           barMon = completeFFTsInBarCount;
//         }

      sleep( 1 );
    }
  
//   BegLogLine( 1 ) 
//     << "ActiveMsgBarrier  Done! " 
//     << completeFFTsInBarCount
//     << " of " << barSz 
//     << EndLogLine;
}

template<class FFT_T> 
void FFT3D<FFT_T>::Init() 
{
  ///  int myPx, myPy, myPz;
  ///  int myRank;  
  ///  BGLPartitionGetCoords( &myPx, &myPy, &myPz);  
  BGLPartitionGetCoords(&mMyP_x, &mMyP_y, &mMyP_z);

  //  if( mMyP_x >= mP_x || mMyP_y >= mP_y || mMyP_z >= mP_z ) 
    {
      printf( "FFT3D::Init:: Mesh size ( %d, %d, %d )\n", mMyP_x, mMyP_y, mMyP_z );
    }

  /*****************************************************************/
  // Z direction pregeneration  
  int PacketsToSendInGatherZ = (int ) ( mLocalN_x * mLocalN_y * ceil( (double)mLocalN_z / (double)mPointsPerPacket_z ) );
  
  mPregeneratedBGLTorusAndFFTHeaders_Z = new BGLTorus_FFT_Headers[ PacketsToSendInGatherZ ];

  int headerCount=0;
  for(int i=0; i < mLocalN_x; i++) 
    {
      int trgPz = i / mBarSize_z;
      
      for(int j=0; j < mLocalN_y; j++) 
        {
          int loadCountZ = 0;
          int zPointsPerPacket = mPointsPerPacket_z;
          
          for(int k =0; k < mLocalN_z; k++) 
            {
              loadCountZ++;
              
              if( ( k == mLocalN_z-1 ) && ( loadCountZ < mPointsPerPacket_z ) )
                {
                  // Points in the last packet
                  zPointsPerPacket = mLocalN_z % mPointsPerPacket_z;
                }              

              if( loadCountZ == zPointsPerPacket ) 
                {
                  //                   zap.mHdr.This = this;
                  //                   zap.mHdr.aIndex = ( i % mBarSize_z ) * mLocalN_y + j;
                  //                   zap.mHdr.bIndex = k+1 - loadCountZ + myPz * mLocalN_z;
                  
                  FFT_Hdr fftHdr;
                  fftHdr.This = this;
                  fftHdr.aIndex = ( i % mBarSize_z ) * mLocalN_y + j;
                  fftHdr.bIndex = k+1 - loadCountZ + mMyP_z * mLocalN_z;;
                  
                  _BGL_TorusPktHdr   BGLTorus_Hdr;
                  BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                   mMyP_x,
                                   mMyP_y,
                                   trgPz,                 // destination coordinates
                                   0,          //???      // destination Fifo Group
                                   (Bit32) PacketActorFx_Z,       // Active Packet Function matching signature above
                                   0,                     // primary argument to actor
                                   0,                     // secondary 10bit argument to actor
                                   _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)              
                 
                  assert( headerCount < PacketsToSendInGatherZ );
                  mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
                  mPregeneratedBGLTorusAndFFTHeaders_Z[ headerCount ].mFFT_Hdr      = fftHdr;
                  headerCount++;
                  loadCountZ=0;
                }
            }
        }
    }

  assert( headerCount == PacketsToSendInGatherZ );    
      
  /*****************************************************************/
  // Pregenerate Y
  headerCount = 0;

  int temp1 = mMyP_y * mLocalN_y;
  int temp2 = mMyP_x * mBarSize_x  + temp1;
  int temp3 = temp2 + mBarSize_x;

  int PacketsToSendInGatherY = (int) ( mP_x * mP_y * ( mLocalN_x * mBarSize_y ) * ceil( (double)mBarSize_x / (double)mPointsPerPacket_y) );
  
  mPregeneratedBGLTorusAndFFTHeaders_Y = new BGLTorus_FFT_Headers[ PacketsToSendInGatherY ];

  
  for(int trgPx=0; trgPx < mP_x; trgPx++) 
    {
      for(int trgPy=0; trgPy < mP_y; trgPy++) 
        {
          for(int ay=0; ay < mLocalN_x * mBarSize_y; ay++) 
            {
              int loadCountY = 0;
              for(int by = temp2;
                      by < temp3; 
                      by ++ ) 
                {
//                   int ax = ( ( by % mLocalN_y ) % mBarSize_x ) * mLocalN_z +
//                       ay / mLocalN_x + trgPy * mBarSize_y; 

//                   int bx = ay % mLocalN_x + mLocalN_x * trgPx;

//                   yap.mData[ loadCountY ].re = mGlobalOut_x[ ax ][ bx ].re;
//                   yap.mData[ loadCountY ].im = mGlobalOut_x[ ax ][ bx ].im;
                  loadCountY++;

                  if( loadCountY == mPointsPerPacket_y ) 
                    {
//                       yap.mHdr.This = this;
//                       yap.mHdr.aIndex = ay;
//                       yap.mHdr.bIndex = by - loadCountY+1;
                      
//                       BGLTorusSimpleActivePacketSendTemp( PacketActorFx_Y,
//                                                           trgPx,
//                                                           trgPy,
//                                                           mMyP_z,
//                                                           sizeof( yap ),
//                                                           (void *) &yap );
                      FFT_Hdr fftHdr;
                      fftHdr.This = this;
                      fftHdr.aIndex = ay;
                      fftHdr.bIndex = by - loadCountY+1;
                      
                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       trgPx,
                                       trgPy,
                                       mMyP_z,                 // destination coordinates
                                       0,          //???      // destination Fifo Group
                                       (Bit32) PacketActorFx_Y,       // Active Packet Function matching signature above
                                       0,                     // primary argument to actor
                                       0,                     // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)              
                      
                      assert( headerCount < PacketsToSendInGatherY );
                      mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
                      mPregeneratedBGLTorusAndFFTHeaders_Y[ headerCount ].mFFT_Hdr      = fftHdr;
                      
                      headerCount++;
                      loadCountY = 0;
                    }
                }
            }
        }
    }
  assert( headerCount == PacketsToSendInGatherY );
  

  /*****************************************************************/
  // Pregenerate X
  headerCount = 0;
  
  temp1 = mMyP_x * mLocalN_x;
  temp2 = mMyP_z * mBarSize_z + temp1;
  temp3 = temp2 + mBarSize_z;

  int PacketsToSendInGatherX = (int) ( mP_z * mP_x * mLocalN_z * mBarSize_x * 
                                       ceil( (double)mBarSize_z / (double)mPointsPerPacket_x) );

  mPregeneratedBGLTorusAndFFTHeaders_X = new BGLTorus_FFT_Headers[ PacketsToSendInGatherX ];
    
  for(int trgPz=0; trgPz < mP_z; trgPz++) 
    {
      for(int trgPx=0; trgPx < mP_x; trgPx++) 
	{
	  for(int ax=0; ax < mLocalN_z * mBarSize_x; ax++) 
	    {
	      int loadCountX=0;
	      for(int bx = temp2;
                      bx < temp3;
                      bx++ ) 
                {
                  loadCountX++;
                  int xPointsPerPacket =  mPointsPerPacket_x;
                  
                  if( ( bx == (mMyP_z + 1) * mBarSize_z + mMyP_x * mLocalN_x-1 ) 
                      && (loadCountX<mPointsPerPacket_x))
                    {
                      xPointsPerPacket = 
                        ( (mMyP_z+1) * mBarSize_z + mMyP_x * mLocalN_x ) % mPointsPerPacket_x;
                    }
                  
                  if( loadCountX == xPointsPerPacket ) 
                    {
//                       xap.mHdr.This = this;
//                       xap.mHdr.aIndex = ax;
//                       xap.mHdr.bIndex = bx - loadCountX + 1;
//                       xap.mHdr.cIndex = 0;
 
//                       BGLTorusSimpleActivePacketSendTemp( PacketActorFx_X,
//                                                           trgPx,
//                                                           mMyP_y,
//                                                           trgPz,
//                                                           sizeof( xap ),
//                                                           (void *) &xap );

                      FFT_Hdr fftHdr;
                      fftHdr.This = this;
                      fftHdr.aIndex = ax;
                      fftHdr.bIndex = bx - loadCountX + 1;
                      fftHdr.cIndex = 0;

                      _BGL_TorusPktHdr   BGLTorus_Hdr;
                      BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                                       trgPx,
                                       mMyP_y,
                                       trgPz,                 // destination coordinates
                                       0,          //???      // destination Fifo Group
                                       (Bit32) PacketActorFx_X,       // Active Packet Function matching signature above
                                       0,                     // primary argument to actor
                                       0,                     // secondary 10bit argument to actor
                                       _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)              
                      
                      assert( headerCount < PacketsToSendInGatherX );
                      mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
                      mPregeneratedBGLTorusAndFFTHeaders_X[ headerCount ].mFFT_Hdr      = fftHdr;
                      headerCount++;
                      loadCountX = 0;
                    }
                }
	    }
	}
    }
  
  assert( headerCount == PacketsToSendInGatherX );

  /*****************************************************************/
  // Pregenerate B
  headerCount=0;
  
  int PacketsToSendInGatherB = (int) (( ( mLocalN_z / mP_y ) * mLocalN_x ) * 
                                      ceil ( (double)(mLocalN_y * mP_y)/(double)(mPointsPerPacket_b) ) );
  
  mPregeneratedBGLTorusAndFFTHeaders_B = new BGLTorus_FFT_Headers[ PacketsToSendInGatherB ];


  for(int a=0; a < ( mLocalN_z / mP_y ) * mLocalN_x; a++)
    {
      int loadCount = 0;
      //      int i = a % mLocalN_x;
      // int k = a / mLocalN_x + mMyP_y * mBarSize_y;

      for(int b=0; b < mLocalN_y * mP_y; b++) 
	{
          //  int j = b % mLocalN_y;
	  int trgPy = b / mLocalN_y;

// 	  bap.mData[ loadCount ].re = mGlobalOut_y[ a ][ b ].re;
// 	  bap.mData[ loadCount ].im = mGlobalOut_y[ a ][ b ].im;
	  loadCount++;

	  if( loadCount == mPointsPerPacket_b )
	    {
// 	      bap.mHdr.This = this;
// 	      bap.mHdr.aIndex = a % mLocalN_x;
// 	      bap.mHdr.bIndex = b % mLocalN_y - loadCount+1;
// 	      bap.mHdr.cIndex = a / mLocalN_x + mMyP_y * mBarSize_y;
	     
// 	      BegLogLine(0) 
//                   << "GatherB::mPointsPerPacket_b "
//                   << mPointsPerPacket_b << " , "
//                   << loadCount << "  , " 
//                   << "localNy" <<mLocalN_y
//                   << EndLogLine;
              
// 	      BGLTorusSimpleActivePacketSend( PacketActorFx_B,
//                                               mMyP_x,
//                                               trgPy,
//                                               mMyP_z,
//                                               sizeof( bap ),
//                                               (void *) &bap );
              
              FFT_Hdr fftHdr;
              fftHdr.This = this;
              fftHdr.aIndex = a % mLocalN_x;
              fftHdr.bIndex = b % mLocalN_y - loadCount + 1;
              fftHdr.cIndex = a / mLocalN_x + mMyP_y * mBarSize_y;
              
              _BGL_TorusPktHdr   BGLTorus_Hdr;
              BGLTorusMakeHdr( & BGLTorus_Hdr,              // Filled in on return
                               mMyP_x,
                               trgPy,
                               mMyP_z,                 // destination coordinates
                               0,          //???      // destination Fifo Group
                               (Bit32) PacketActorFx_B,       // Active Packet Function matching signature above
                               0,                     // primary argument to actor
                               0,                     // secondary 10bit argument to actor
                               _BGL_TORUS_ACTOR_BUFFERED );       // Actor Signature from above (must match Actor)              
              
              assert( headerCount < PacketsToSendInGatherB );
              mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mBGLTorus_Hdr = BGLTorus_Hdr;
              mPregeneratedBGLTorusAndFFTHeaders_B[ headerCount ].mFFT_Hdr      = fftHdr;

              headerCount++;              
	      loadCount = 0;
	    }
	}
    }
  
  assert( headerCount == PacketsToSendInGatherB );
}


template<class FFT_T> 
void FFT3D<FFT_T>::Execute(complex*** in, complex*** out) 
{     
  // Start by collecting and doing a 1D FFT in the z-direction
//   for(int a=0; a < mBarSize_z * mLocalN_y; a++)
//     for(int k=0; k < mLocalN_z * mP_z; k++)
//       {
//         BegLogLine( 0 ) 
//           << "fft::mGlobalIn_z[" << a << "][" << k << "]" 
//           << mGlobalIn_z[ a ][ k ].re
//           << " ," << mGlobalIn_z[ a ][ k ].im
//           << EndLogLine;
//       }

  GatherZ(in, 0);
 
  ActiveMsgBarrier( mCompleteFFTsInBarCount_z, mLocalN_y * mBarSize_z );
  printf("Done GatherZ\n");

//   for(int a=0; a < mBarSize_z * mLocalN_y; a++)
//     {
//       for(int k=0; k < mLocalN_z * mP_z; k++)
// 	{  
// 	  BegLogLine(0) << "fft::mGlobalOut_z[" << a << "][" << k <<"]"
//                         << mGlobalOut_z[ a ][ k ].re
// 			<< " ," << mGlobalOut_z[ a ][ k ].im
//                         << EndLogLine;
// 	}
//     }

  BGLPartitionBarrier();
  // MPI_Barrier( MPI_COMM_WORLD );
  GatherX( mGlobalIn_x, 0 );
  
//   for(int a=0; a < mBarSize_x * mLocalN_z; a++)
//     {
//     for(int k=0; k < mLocalN_x * mP_x; k++)
//       {  
//       BegLogLine( 0 ) 
//           << "fft::mGlobalIn_x[" << a <<"]["<< k<<"]"
//           << mGlobalIn_x[a][k].re <<" ," << mGlobalIn_x[a][k].im
//           << EndLogLine;
//       }
//     }
  
//   BegLogLine( 0 ) 
//       << " fft::mCompleteFFTsInBarCount_x = "
//       << mCompleteFFTsInBarCount_x << " "
//       << "mLocalN_z*mBarSize_x = "
//       << mLocalN_z * mBarSize_x
//       << EndLogLine;
   
  ActiveMsgBarrier( mCompleteFFTsInBarCount_x, mLocalN_z * mBarSize_x );

  mCompleteFFTsInBarCount_x = 0;

  BGLPartitionBarrier();
  //  MPI_Barrier( MPI_COMM_WORLD );
  
//   for(int a=0; a<mBarSize_x*mLocalN_z; a++)
//     {
//       for(int k=0; k< mLocalN_x*mP_x; k++) 
// 	{
// 	  BegLogLine(0) 
//               << "fft::mGlobalOut_x[" << a << "][" << k << "]" 
//               << mGlobalOut_x[ a ][ k ].re
//               << " ," << mGlobalOut_x[ a ][ k ].im
//               << EndLogLine;
// 	}
//     }
  
  printf("Done GatherX\n");
  //  MPI_Barrier(MPI_COMM_WORLD);
  BGLPartitionBarrier();

//   for(int a=0; a < mBarSize_y * mLocalN_x; a++)
//     {
//       for(int k=0; k < mLocalN_y * mP_y; k++)
// 	{
// 	  BegLogLine(0)
//               << "fft::mGlobalIn_y[" << a << "]["<< k << "]" 
//               << mGlobalIn_y[ a ][ k ].re
//               << " ," << mGlobalIn_y[ a ][ k ].im
//               << EndLogLine;
// 	}
//     }
  
  GatherY(mGlobalIn_y, 0);
  ActiveMsgBarrier( mCompleteFFTsInBarCount_y, mLocalN_x * mBarSize_y );
  mCompleteFFTsInBarCount_y = 0;
  
//   for(int a=0; a < mBarSize_y * mLocalN_x; a++)
//     for(int k=0; k < mLocalN_y * mP_y; k++) 
//       {
// 	BegLogLine(0)
//             << "fft::mGlobalOut_y[" << a << "]["<< k << "]"
//             << mGlobalOut_y[ a ][ k ].re
//             << " ," << mGlobalOut_y[ a ][ k ].im
//             << EndLogLine;
//       }
 
  printf("Done GatherY\n");
  // MPI_Barrier( MPI_COMM_WORLD );
  BGLPartitionBarrier();
  GatherB( out, 0 );
  
  for(int i=0; i < mLocalN_x; i++)
    {
      ActiveMsgBarrier( mCompleteFFTsInBarCount_b[ i ], mLocalN_z);
//       cout << "done GatherB "<< i+1 << " of " << mLocalN_x 
// 	   << "   " << mCompleteFFTsInBarCount_b[ i ] << endl;

      mCompleteFFTsInBarCount_b[ i ]=0;
    } 
  
  // MPI_Barrier( MPI_COMM_WORLD );
  BGLPartitionBarrier();
  
  for(int i=0; i < mLocalN_x; i++)
    {
      for(int j=0; j < mLocalN_y; j++)
	{
	  for(int k=0; k < mLocalN_z; k++)
	    {
	      out[ i ][ j ][ k ].re = mLocalOut[ i ][ j ][ k ].re;
	      out[ i ][ j ][ k ].im = mLocalOut[ i ][ j ][ k ].im;

// 	      BegLogLine( 0 ) 
//                   << "fft::mLocalOutB[" << i << " ][" << j << "][" << k << "]"
//                   << mLocalOut[ i ][ j ][ k ].re
//                   << " ," << mLocalOut[ i ][ j ][ k ].im
//                   << EndLogLine;

// 	      BegLogLine( 0 )
//                   << "fft::outB[" << i << " ][" << j << "][" << k << "]"
//                   << out[ i ][ j ][ k ].re
//                   << " ," << out[ i ][ j ][ k ].im
//                   << EndLogLine;
	    }
	}
    }
  
  printf("=====  Done FFT ======\n");
  // MPI_Barrier( MPI_COMM_WORLD );   
  BGLPartitionBarrier();
}

template<class FFT_T> 
FFT3D<FFT_T>::~FFT3D()
{
}

template<class FFT_T>
void FFT3D<FFT_T>::GetLocalNx(int &localNx)
{
  localNx = mLocalN_x;
}

template<class FFT_T>
void FFT3D<FFT_T>::GetLocalNy(int &localNy)
{
  localNy = mLocalN_y;
}

template<class FFT_T>
void FFT3D<FFT_T>::GetLocalNz(int &aLocalNz) 
{
  aLocalNz = mLocalN_z;
}

template<class FFT_T>
void FFT3D<FFT_T>::GetPx(int &px)
{
  px = mP_x;
}

template<class FFT_T>
void FFT3D<FFT_T>::GetPy(int &py)
{
  py = mP_y;
}

template<class FFT_T>
void FFT3D<FFT_T>::GetPz(int &pz)
{
  pz = mP_z;
}
