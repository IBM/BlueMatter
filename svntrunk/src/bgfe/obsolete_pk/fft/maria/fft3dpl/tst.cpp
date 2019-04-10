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
 #include <stdio.h>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include "BladeMpi.hpp"
#include "complex.hpp"
#include <math.h>

#include "fxlogger.hpp"

#define nx  4
#define ny  4
#define nz  4
#define Px  1
#define Py  1
#define Pz  1


class FFT
{
public:

  int CountP;

  struct FFT_Hdr
  {
    FFT   *This;
    int    iIndex;
    int    jIndex;
    int    kIndex;
  };

  typedef complex  FFT_Payload  [ 2];
  struct ActorPacket
  {
    FFT_Hdr  mHdr;
    FFT_Payload mData;
  };

  void DoFFT() 
    {
      int x, y, z;
      BGLPartitionGetCoords(&x, &y, &z);
      CountP=0;
      gather();
      //    if(CountP == 1)
      //  printf("success ..[%d]\n" , CountP);
      BegLogLine(1)
        << "DoFFT "
        << "Start waiting for Count P "
        << EndLogLine;

      while( CountP != 1 ) 
        {
          sleep(1);
          //   BegLogLine(1)
          //         << "DoFFT "
          //         << "waiting for CountP "
          //         << CountP
          //         << EndLogLine;
          //printf("Waiting on [%d][%d][%d] \n", x, y, z);
        }
      
      //  BegLogLine(1)
      //       << "DoFFT "
      //       << "Done waiting for CountP "
      //       << CountP
      //       << EndLogLine;
    }
  
  static int actorX(void* pkt)
    {
      int x, y, z;
      printf(" Inside the actor ... \n");
      ActorPacket *ap = (ActorPacket *) pkt;
      FFT *This = ap->mHdr.This;
      int i = ap->mHdr.iIndex;
      int j = ap->mHdr.jIndex;
      int k = ap->mHdr.kIndex;
      
      BGLPartitionGetCoords(&x, &y, &z);
      printf("recv node [%d][%d][%d]  from node[%d][%d][%d] \n", x,y,z, i,j,k);
      
      This->CountP++;
      
      //   BegLogLine(1)
      //       << "actorX() "
      //       << " recv node "
      //       << x << "." << y << "." << z
      //       << " from "
      //       << i << "." << j << "." << k
      //       << " This " << (void *) This
      //       << " CountP "
      //       << (*This).CountP
      //       << EndLogLine;
      //printf(" actor [%d]\n" , (*This).CountP);
      return 0;
    }
  
  int gather()
    {
      ActorPacket ap ;
      int x, y, z;
      BGLPartitionGetCoords(&x, &y, &z);
      ap.mHdr.This = this;
      ap.mHdr.iIndex=x;
      ap.mHdr.jIndex=y;
      ap.mHdr.kIndex=z;
      ap.mData[0] =0;
      
      int trgPx = 0;
      int trgPy = 0;
      int trgPz;

      if(z==0)
        trgPz = 1;//(x+1)%Pz;
      else
        trgPz = 0;
  
      printf("From node [%d][%d][%d] to  node[%d][%d][%d] \n", x,y,z,
             trgPx, trgPy, trgPz);
      //    BegLogLine(1)
      //       << "actorX() "
      //       << " recv node "
      //       << x << "." << y << "." << z
      //       << " from "
      //       << trgPx << "." << trgPy << "." << trgPz
      //       << " This " << (void *) this
      //       << " CountP "
      //       << CountP
      //       << EndLogLine;
      
      BGLTorusSimpleActivePacketSend(actorX, trgPx,trgPy,trgPz,
                                     sizeof( ap ),
                                     (void *) &ap );
      
      //   BegLogLine(1) << "gather CountP = " << CountP << EndLogLine;
      return(0);
    }
  

};

FFT fft;
void  Print_FFT_Info(){}

main(int argc, char** argv, char** argp) 
{
  MPI_Init( &argc, &argv );  /* initialize MPI environment */
  int MpiRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &MpiRank );
  
  //AttachDebugger( argv[0] );
  
  BladeInit( &argc, &argv );
  
  // BegLogLine(1)
  //  << "main() running"
  //  << EndLogLine;
  
  fft.DoFFT();
  
  // printf(" done with the FFT");
  //  if( MpiRank == 0 )
  //     Print_FFT_Info();
  
  //BegLogLine(1)
  //  << "Main() "
  //  << " About to MPI_Finalize() "
  //  << EndLogLine;
  
  MPI_Finalize();
}

