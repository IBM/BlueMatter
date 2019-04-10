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
 //  
//  IBM T.J.W  R.C.
//  Date : 24/01/2003
//  Name : Maria Eleftheriou
//  Last Modified on: 09/24/03 by Maria Eleftheriou
//  fftmain.hpp
//  tests the 3D fft using MPI and fftw 


#if defined(USE_PK_ACTORS)
#include <BonB/BGL_PartitionSPI.h>
#include <rts.h>
#include <Pk/API.hpp>
#include <Pk/Compatibility.hpp>
#include <stdio.h>
#include <BonB/BGLTorusAppSupport.h>
#include <BonB/BGLTorusAppSupport.c>

#else
#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#endif
#include <complex.hpp>
//#include <assert.h>
#include <fft3D.hpp>
//#include <stdlib.h>
//#include <string.h>
////#include <iostream.h>
#include <math.h>
// #include <fft3d/timer_MPI.hpp>

#define TYPE float 

#define FFT3D_STUCT_ALIGNMENT __attribute__ (( aligned( (0x01 << 6) ) ))
#define TOLERANCE  (1./1000000000.)
#define N_SIZE 128
#define MESS_SIZE 8
#define ITERATIONS 10

#define REGRESS_REVERSE

#ifndef PKFXLOG_FFTMAIN
#define PKFXLOG_FFTMAIN (0)
#endif

#ifndef PKFXLOG_FFTINTER
#define PKFXLOG_FFTINTER (0)
#endif

#ifndef PKFXLOG_FFTITERATION
#define PKFXLOG_FFTITERATION (0) 
#endif

#ifndef PKFXLOG_FFTBENCHMARK
#define PKFXLOG_FFTBENCHMARK (0) 
#endif

struct FFT_PLAN
{
  enum
  {
    P_X = MESS_SIZE,
    P_Y = MESS_SIZE,
    P_Z = MESS_SIZE 
  };
  enum
  {
    GLOBAL_SIZE_X = N_SIZE,
    GLOBAL_SIZE_Y = N_SIZE,
    GLOBAL_SIZE_Z = N_SIZE
  };
};

// CHECK needs to be removed
#define FORWARD -1
#define REVERSE 1

typedef  BGL3DFFT< FFT_PLAN, FORWARD, TYPE > FFT_FORWARD FFT3D_STUCT_ALIGNMENT;
typedef  BGL3DFFT< FFT_PLAN, REVERSE, TYPE > FFT_REVERSE FFT3D_STUCT_ALIGNMENT;

static FFT_FORWARD fftForward;
static FFT_REVERSE fftReverse;



static int compare (complexTemplate<TYPE>* in, complexTemplate<TYPE>* rev, 
       int localNx, int localNy, int localNz, 
       double fftSz)
{
  for(int i=0; i<localNx; i++)
    for(int j=0; j<localNy; j++) 
      for(int k=0; k<localNz; k++) 
  {
    int myIndex  = i*localNy*localNz+j*localNz+k;
    if ( fabs(in[myIndex].re-rev[myIndex].re*fftSz) > TOLERANCE
      || fabs(in[myIndex].im-rev[myIndex].im*fftSz) > TOLERANCE
       ) {
          
      BegLogLine(1) 
        << "[" << i
        << "," << j
        << "," << k
        << "] in=(" << in[myIndex].re
        << "," << in[myIndex].im
        << ") rev*fftSz=(" << rev[myIndex].re*fftSz
        << "," << rev[myIndex].im*fftSz
        << ")" 
        << EndLogLine ;
        return 1;
         }
//	  if(fabs(in[i][j][k].re-out[i][j][k].re)>TOLERANCE)
//	    { 
//	  BegLogLine(1) 
//	    << "[" << i
//	    << "," << j
//	    << "," << k
//	    << "] in=(" << in[i][j][k].re
//	    << "," << in[i][j][k].im
//	    << ") out=(" << out[i][j][k].re
//	    << "," << out[i][j][k].im
//	    << ")" 
//	    << EndLogLine ;
////	      cout << "in ["<<i<<"]["<< j<<"]["<<k<<"]="<< in[i][j][k]<<endl;
////	      cout << "out["<<i<<"]["<< j<<"]["<<k<<"]="<< out[i][j][k]<<endl;
//	      return 1;
//	    }
//	  if(fabs(in[i][j][k].im-out[i][j][k].im)>TOLERANCE) 
//	    {
//	  BegLogLine(1) 
//	    << "[" << i
//	    << "," << j
//	    << "," << k
//	    << "] in=(" << in[i][j][k].re
//	    << "," << in[i][j][k].im
//	    << ") out=(" << out[i][j][k].re
//	    << "," << out[i][j][k].im
//	    << ")" 
//	    << EndLogLine ;
////	      cout << "in ["<<i<<"]["<< j<<"]["<<k<<"]="<< in[i][j][k]<<endl;
////	      cout << "out["<<i<<"]["<< j<<"]["<<k<<"]="<< out[i][j][k]<<endl;
//	      return 1;
//	    }
  }
  return 0;
}
// get processor mesh and fft size
static void init( int argc, char*argv[],  int& globalNx, int& globalNy, int& globalNz,
    int& pX, int& pY, int& pZ,
    int& subPx, int& subPy, int& subPz, 
    int& iterations ,
    int& markrank ,
    int& markindex )
{
  BegLogLine(PKFXLOG_FFTMAIN)
    << "init argc=" << argc
    << EndLogLine ;
  for(int i=0; i<argc; i++)
    {
      BegLogLine(PKFXLOG_FFTMAIN)
        << "i=" << i
        << " argv[i]=" << argv[i]
        << EndLogLine ;
      if (!strcmp(argv[i],"-procmesh"))
    {
      pX = atoi(argv[++i]);
      pY = atoi(argv[++i]);
      pZ = atoi(argv[++i]);
    }
  
      if (!strcmp(argv[i],"-subprocmesh"))
    {
      subPx = atoi(argv[++i]);
      subPy = atoi(argv[++i]);
      subPz = atoi(argv[++i]);
    }
    
      if (!strcmp(argv[i],"-fftsize")) 
    {
    
      globalNx = atoi(argv[++i]);
      globalNy = atoi(argv[++i]);
      globalNz = atoi(argv[++i]);
    }
    
      if (!strcmp(argv[i],"-iterations")) 
    {
      iterations = atoi(argv[++i]) ;
    }
        
      if (!strcmp(argv[i],"-mark")) 
    {
      markrank = atoi(argv[++i]) ;
      markindex = atoi(argv[++i]) ;
    }
        
    }
}

#if !defined(USE_PK_ACTORS)
extern "C" {
void *
PkMain(int argc, char** argv, char** envp) ;
} ;
#endif

int PkStartType = PkStartAllNodesCore0 ;
#ifdef PKTRACE
int PkTracing = 1 ;
#else
int PkTracing = 0 ;
#endif

#if defined(USE_PK_ACTORS)
int
#else
void *
#endif
PkMain(int argc, char** argv, char** envp)
{
#pragma execution_frequency(very_low)
//  complex ***in, ***tmp, ***out;
//  complex ***localOut;

//  int pX = FFT_PLAN::P_X ;
//  int pY = FFT_PLAN::P_Y ;
//  int pZ = FFT_PLAN::P_Z ;
#if defined(USE_PK_ACTORS)
  int pX ;
  int pY ;
  int pZ ;
  if ( Platform::Thread::GetId() != 0 ) 
  {
    for(;;) ; // hang here if on the IOP
  }
  Platform::Topology::GetDimensions(&pX, &pY, &pZ) ;
#else
  int pX = Platform::Topology::mDimX ;
  int pY = Platform::Topology::mDimY ;
  int pZ = Platform::Topology::mDimZ ;
#endif  
  int globalNx = FFT_PLAN::GLOBAL_SIZE_X ;
  int globalNy = FFT_PLAN::GLOBAL_SIZE_Y ;
  int globalNz = FFT_PLAN::GLOBAL_SIZE_Z ;
  int iterations = ITERATIONS ;
  
  int markrank = 0 ; 
  int markindex = 0 ; 
  
  int subPx = -1 ; 
  int subPy = -1 ;
  int subPz = -1 ; 
  
  init(argc, argv, globalNx,globalNy,globalNz, pX, pY, pZ, subPx, subPy, subPz, iterations, markrank, markindex );	

  if ( -1 == subPx ) subPx = pX ; 
  if ( -1 == subPy ) subPy = pY ; 
  if ( -1 == subPz ) subPz = pZ ; 
  
  int localNx = globalNx / pX ;
  int localNy = globalNy / pY ;
  int localNz = globalNz / pZ ;
  
#if defined(USE_PK_ACTORS)
  int myRank = PkNodeGetId() ;
//  #if defined(PKTRACE)
//  pkTraceServer::Init() ;
//  #endif
#else  
  int myRank = Platform::Topology::GetAddressSpaceId();
#endif  
//  if (myRank == 0) 
//    {
//      printf( " proc mesh GLOBAL_SIZE_X = %d GLOBAL_SIZE_Y = %d GLOBAL_SIZE_Z = %d\n",
//	      FFT_PLAN::GLOBAL_SIZE_X,
//	      FFT_PLAN::GLOBAL_SIZE_Y, 
//	      FFT_PLAN::GLOBAL_SIZE_Z);
//      
//      printf(" 3D FFT of size [%d %d %d] on [%d %d %d] \n",
//	     localNx, localNy, localNz,
//	     FFT_PLAN::P_X, FFT_PLAN::P_Y, FFT_PLAN::P_Z);
//    }
  
  int arraySz= (globalNx*globalNy*globalNz)/(pX*pY*pZ);

  BegLogLine(PKFXLOG_FFTMAIN)
    << "PkMain"
//    << " FFT_PLAN::GLOBAL_SIZE_X=" << FFT_PLAN::GLOBAL_SIZE_X
//    << " FFT_PLAN::GLOBAL_SIZE_Y=" << FFT_PLAN::GLOBAL_SIZE_Y
//    << " FFT_PLAN::GLOBAL_SIZE_Z=" << FFT_PLAN::GLOBAL_SIZE_Z
//    << " FFT_PLAN::P_X=" << FFT_PLAN::P_X
//    << " FFT_PLAN::P_Y=" << FFT_PLAN::P_Y
//    << " FFT_PLAN::P_Z=" << FFT_PLAN::P_Z
    << " globalNx=" << globalNx
    << " globalNy=" << globalNy
    << " globalNz=" << globalNz
    << " pX=" << pX
    << " pY=" << pY
    << " pZ=" << pZ
    << " localNx=" << localNx
    << " localNy=" << localNy
    << " localNz=" << localNz
    << " arraySz=" << arraySz
    << EndLogLine ;

  complexTemplate<TYPE>* in  = new complexTemplate<TYPE>[arraySz];
  complexTemplate<TYPE>* out = new complexTemplate<TYPE>[arraySz];
  complexTemplate<TYPE>* rev = new complexTemplate<TYPE>[arraySz];
  
//#define mainMax(i, j) ( (i>j)?i:j )
//  
//  int maxLocalN = mainMax(mainMax(localNx, localNy), localNz);
//  in  = (complex***) new complex[maxLocalN];
//  tmp = (complex***) new complex[maxLocalN];
//  out = (complex***) new complex[maxLocalN];
//  
//  for(unsigned int i=0; i<localNx; i++) {
//    in[i]  = (complex **) new complex[maxLocalN];
//    tmp[i] = (complex **) new complex[maxLocalN];
//    out[i] = (complex **) new complex[maxLocalN];
//  
//    for(unsigned int j=0; j<localNy; j++) {
//      in[i][j]  = (complex *) new complex[maxLocalN];
//      tmp[i][j] = (complex *) new complex[maxLocalN];
//      out[i][j] = (complex *) new complex[maxLocalN];
//    }
//  }

//  (fftForward).Init(FFT_PLAN::GLOBAL_SIZE_X,FFT_PLAN::GLOBAL_SIZE_Y,FFT_PLAN::GLOBAL_SIZE_Z
//                   ,FFT_PLAN::P_X,FFT_PLAN::P_Y,FFT_PLAN::P_Z);   
                    
  (fftReverse).Init(globalNx,globalNy,globalNz
                   ,pX,pY,pZ
                   ,subPx, subPy, subPz, 1 );   

   (fftForward).Init(globalNx,globalNy,globalNz
                   ,pX,pY,pZ
                   ,subPx, subPy, subPz, 1 );   
                   
#ifndef FFT_FIX_DATA
  srand48(myRank) ; // Arrange that different nodes get different seed data
#endif  
  for(unsigned int i=0; i<localNx; i++) 
    for(unsigned int j=0; j<localNy; j++) 
      for(unsigned int k=0; k<localNz; k++)
  { 
#ifdef FFT_FIX_DATA		
    double rdata = 0.0 ;
    double idata = 0.0 ;
#else
    double rdata = drand48();
    double idata = drand48();	  
#endif	  
    int myIndex  = i*localNy*localNz+j*localNz+k;
    in[myIndex].re = (TYPE) rdata;
    in[myIndex].im = (TYPE) idata;
    
//	  double data = drand48(); 
//	  in[i][j][k]=complex(data,0);
//	  fftForward.PutRealSpaceElement(i, j, k, in[i][j][k].re);
  }
#ifdef FFT_FIX_DATA
    if ( markrank == myRank && markindex >= 0 && markindex < arraySz )
    {
      in[markindex].re = 1.0 ;
    }
#endif	
  
  
//  for(unsigned int i=0; i<localNx; i++) 
//    for(unsigned int j=0; j<localNy; j++) 
//      for(unsigned int k=0; k<localNz; k++)
//	{ 
//	  tmp[i][j][k].re= fftForward.GetRealSpaceElement(i, j, k);
//
//      assert(	  in[i][j][k].re-tmp[i][j][k].re <= TOLERANCE ) ;
////	  if(in[i][j][k].re-tmp[i][j][k].re>TOLERANCE)
////	    {
////	      printf(" Check input data. Exiting...\n");
////	      exit(1);
////	    }
//	}

//  int iter =20;

  BegLogLine(PKFXLOG_FFTMAIN)
    << "Iterations starting"
    << EndLogLine ;

// Synchronise the nodes ...      
  PkCo_Barrier() ;
    
// Do one forward and one reverse, to separate out initialisation effects
      (fftForward).DoFFT(in,out);
    (fftReverse).DoFFT(out, rev);
// Main run
  long long oscillatorAtStart = 
#if defined(USE_PK_ACTORS)
  PkTimeGetRaw()
#else  
  Platform::Clock::Oscillator() 
#endif  
  ;
  {
     BegLogLine(PKFXLOG_FFTINTER)
      << "Starting forwards FFT"
      << EndLogLine ;
      int nextlogiteration = 1 ;
      for(int i=0; i<iterations; i++)
    {
      if ( i >= nextlogiteration )
      {
      BegLogLine(PKFXLOG_FFTITERATION) 
        << "Forward iteration=" << i
        << EndLogLine ;
        nextlogiteration *= 2 ;
      }
      (fftForward).DoFFT(in,out);
    }
     BegLogLine(PKFXLOG_FFTINTER)
      << "Ending forwards FFT"
      << EndLogLine ;
  }
 // if(myRank==0)
//   for(unsigned int i=0; i<localNx; i++) 
//     for(unsigned int j=0; j<localNy; j++) 
//       for(unsigned int k=0; k<localNz; k++)
// 	{ 
//     	  printf(" proc = %d out_forward[%d %d %d] = %f\n", 
// 		 myRank, i, j, k, tmp[i][j][k].re, tmp[i][j][k].im );
// 	}
//  (fftReverse).Init(FFT_PLAN::GLOBAL_SIZE_X,FFT_PLAN::GLOBAL_SIZE_Y,FFT_PLAN::GLOBAL_SIZE_Z
//                   ,FFT_PLAN::P_X,FFT_PLAN::P_Y,FFT_PLAN::P_Z);
//  for( int i=0; i<localNx; i++)
//    for( int j=0; j<localNy; j++)
//      for( int k=0; k<localNz; k++)
//	{
//	  fftReverse.PutRecipSpaceElement(i, j, k,
//			   fftForward.GetRecipSpaceElement( i, j, k ) );
//	}
  
  long long oscillatorAtMid = 
#if defined(USE_PK_ACTORS)
  PkTimeGetRaw()
#else  
  Platform::Clock::Oscillator() 
#endif  
  ;
  {
     BegLogLine(PKFXLOG_FFTINTER)
      << "Starting reverse FFT"
      << EndLogLine ;
      int nextlogiteration = 1 ;
    for(int i=0; i<iterations; i++)
    {
      if ( i >= nextlogiteration )
      {
      BegLogLine(PKFXLOG_FFTITERATION) 
        << "Reverse iteration=" << i
        << EndLogLine ;
        nextlogiteration *= 2 ;
      }
    (fftReverse).DoFFT(out, rev);
    } 
     BegLogLine(PKFXLOG_FFTINTER)
      << "Ending reverse FFT"
      << EndLogLine ;
  }
  long long oscillatorAtEnd = 
#if defined(USE_PK_ACTORS)
  PkTimeGetRaw()
#else  
  Platform::Clock::Oscillator() 
#endif  
  ;
  BegLogLine(PKFXLOG_FFTBENCHMARK)
    << "Benchmark sizeof(TYPE)=" << sizeof(TYPE)
    << " globalNx=" << globalNx
    << " globalNy=" << globalNy
    << " globalNz=" << globalNz
    << " pX=" << pX
    << " pY=" << pY
    << " pZ=" << pZ
    << " iterations=" << iterations
    << " forward_clocks=" << oscillatorAtMid-oscillatorAtStart
    << " reverse_clocks=" << oscillatorAtEnd-oscillatorAtMid
    << " forward_time_per_iter_ns=" << (oscillatorAtMid-oscillatorAtStart)*(1000.0/700.0)/iterations 
    << " reverse_time_per_iter_ns=" << (oscillatorAtEnd-oscillatorAtMid)*(1000.0/700.0)/iterations 
    << EndLogLine ; 
  double fftSz = 1.0/(double)(localNx*pX*localNy*pY*localNz*pZ);

//  for( int i=0; i<localNx; i++)
//    for( int j=0; j<localNy; j++)
//      for( int k=0; k<localNz; k++)
//	{
//      out[i][j][k] = (fftReverse.GetRealSpaceElement(i, j, k ));
//	  out[i][j][k].re = fftSz*out[i][j][k].re;
//	  out[i][j][k].im = fftSz*out[i][j][k].im;
//	  // cout<<"OUT["<<i<<"]["<<j<<"][" <<k<< "] =" << out[i][j][k] <<endl;
//	}

//#define  PRINT_OUTPUT
#ifdef PRINT_OUTPUT
  for(unsigned int i=0; i<localNx; i++) 
    for(unsigned int j=0; j<localNy; j++) 
      for(unsigned int k=0; k<localNz; k++)
  {          
    int myIndex  = i*localNy*localNz+j*localNz+k;
    BegLogLine(1) 
      << "[" << i
      << "," << j
      << "," << k
      << "] in=(" << in[myIndex].re
      << "," << in[myIndex].im
      << ") out=(" << out[myIndex].re
      << "," << out[myIndex].im
      << ") rev=(" << rev[myIndex].re
      << "," << rev[myIndex].im
      << ")" 
      << EndLogLine ;
//	  tmp[i][j][k] =   fftForward.GetRecipSpaceElement( i, j, k );
//	  BegLogLine(1) 
//	    << "[" << i
//	    << "," << j
//	    << "," << k
//	    << "] in=(" << in[i][j][k].re
//	    << "," << in[i][j][k].im
//	    << ") out_forward=(" << tmp[i][j][k].re
//	    << "," << tmp[i][j][k].im
//	    << ") out=(" << out[i][j][k].re
//	    << "," << out[i][j][k].im
//	    << ")" 
//	    << EndLogLine ;
      
//	  printf(" proc = %d out_forward[%d %d %d] = %f\n", 
//		 myRank, i, j, k, tmp[i][j][k].re, tmp[i][j][k].im );
//	  printf(" proc %d out_reverse[%d %d %d] = %f\n", 
//		 myRank, i, j, k, out[i][j][k].re, tmp[i][j][k].im );
    
      }
#endif
#if defined(A2A_WITH_ACTORS)
  #if defined(USE_PK_ACTORS)
    PacketAlltoAllv_PkActors::ReportFifoHistogram() ;
  #else
    PacketAlltoAllv_Actors::ReportFifoHistogram() ;
  #endif
#else
  PacketAlltoAllv::ReportFifoHistogram() ;
#endif  
#ifdef REGRESS_REVERSE  
  if(compare(in, rev, localNx, localNy, localNz, fftSz) == 1) { 
    BegLogLine(1)
      << "FAILED FFT-REVERSE TEST :-( "
      << EndLogLine ;
//   printf( "FAILED FFT-REVERSE TEST :-( \n" ); 
  }
  else {
    BegLogLine(1)
      << "PASSED FFT-REVERSE TEST :-) "
      << EndLogLine ;
//   printf( "PASSED FFT-REVERSE TEST :-) \n" );
  } 
#endif
  BegLogLine(PKFXLOG_FFTMAIN)
    << "PkMain complete"
    << EndLogLine ; 
#if defined(USE_PK_ACTORS)
// 'Barrier' here spins until all nodes have finshed their last FFT; otherwise some nodes
// can exit before emptying their transmit FIFOs
  PkCo_Barrier() ;
  
#if defined(PKTRACE)
  pkTraceServer::FlushBuffer() ;
#endif
  
   exit(0) ; // Bring 'Pk' down explicitly
  return 0 ;
#else  
  return NULL ;
#endif  
}
