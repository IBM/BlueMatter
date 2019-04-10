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
//  FFT.hpp
//
//  The FFT Class provides an interface for performing a sequencial 3D FFT
//  on distribute data on a 3D-processor mesh. It is templated on the type
//  of 1D FFT to perform and on parallel communication library to use.
//  Parallel Communication libraries available : MPI, BGL_BLADE, BGL_ ...
//

#ifndef __FFT3D_H_
#define __FFT3D_H_

#include <iostream>
#include "fft_one_dim.hpp"
////////////////#include "comm3DLayer.hpp"
#ifndef BUILD_FOR_BLNIE
#include "BladeMpi.hpp"
#include "mpi.h"
#else
#include "blade.h"
#endif

//#include "complex.hpp"
#include "fxlogger.hpp"

template<class FFT_T>
class FFT3D 
{
  enum{ DIM = 3 };
  enum{ PX_MAX = 6, PY_MAX = 6, PZ_MAX = 6 };
    
  public:
    
  // 1D FFTs in each direction
  FFT_T* mFFT_x;
  FFT_T* mFFT_y;
  FFT_T* mFFT_z;
    
  //    COMM_T* mComm;    
  int mFFTDirection;
   
    //  complex*** mLocalIn;  // localIn  [nBars][nFFTs][localSizeOfFFT];
  complex*** mLocalOut; // localOut [nBars][nFFTs][localSizeOfFFT];
    
  complex** mGlobalIn_x;  // globalIn [nFFTs][sizeof(1DFFT)];
  complex** mGlobalOut_x; // globalOut[nFFTs][sizeof(1DFFT)];
    
  complex** mGlobalIn_y;  // globalIn [nFFTs][sizeof(1DFFT)];
  complex** mGlobalOut_y; // globalOut[nFFTs][sizeof(1DFFT)];
    
  complex** mGlobalIn_z;  // globalIn [nFFTs][sizeof(1DFFT)];
  complex** mGlobalOut_z; // globalOut[nFFTs][sizeof(1DFFT)];
    
    //  complex** mGlobalIn_b;  // globalIn [nFFTs][sizeof(1DFFT)];
  complex** mGlobalOut_b; // globalOut[nFFTs][sizeof(1DFFT)];
        
  // Size of the fft mesh on the local node in 3D
  unsigned int mLocalN_x, mLocalN_y, mLocalN_z;
    
  // Topology of the machine in 3D
  unsigned int mP_x, mP_y, mP_z;

  int mMyP_x, mMyP_y, mMyP_z;

  // Size bars in 3D
  unsigned int mBarSize_x, mBarSize_y, mBarSize_z;

  // communication related data struct    
  struct FFT_Hdr 
  {
    FFT3D *This;
    int aIndex;
    int bIndex;
    int cIndex;
  };

  typedef complex Value;

  enum 
    { 
      ELEMENTS_PER_PACKET = ( (sizeof(_BGL_TorusPktPayload ) - sizeof(FFT_Hdr)) / sizeof( Value) ),
    };


  struct BGLTorus_FFT_Headers
  {
    _BGL_TorusPktHdr mBGLTorus_Hdr;    
    FFT_Hdr          mFFT_Hdr;
  };

  struct ActorPacket
  {
    FFT_Hdr mHdr;
    Value   mData[ ELEMENTS_PER_PACKET ];
  };

  BGLTorus_FFT_Headers* mPregeneratedBGLTorusAndFFTHeaders_Z;
  BGLTorus_FFT_Headers* mPregeneratedBGLTorusAndFFTHeaders_Y;
  BGLTorus_FFT_Headers* mPregeneratedBGLTorusAndFFTHeaders_X;
  BGLTorus_FFT_Headers* mPregeneratedBGLTorusAndFFTHeaders_B;

  int mPointsPerPacket_x;
  int mPointsPerPacket_y;
  int mPointsPerPacket_z;
  int mPointsPerPacket_b;

  int*  mCompletePacket_x;
  int*  mCompletePacket_y;
  int*  mCompletePacket_z;
  int** mCompletePacket_b;

//   int _zCompletePencilsInBarCount;
//   int _yCompletePencilsInBarCount;
//   int _xCompletePencilsInBarCount;
//   int _bCompletePencilsInBarCount;

  int  mCompleteFFTsInBarCount_x;
  int  mCompleteFFTsInBarCount_y;
  int  mCompleteFFTsInBarCount_z;
  int* mCompleteFFTsInBarCount_b;

  int mPacketsPerFFT_x;
  int mPacketsPerFFT_y;
  int mPacketsPerFFT_z;
  int mPacketsPerFFT_b;

  FFT3D( unsigned int localNx,
                unsigned int localNy,
                unsigned int localNz,
                unsigned int Px,
                unsigned int Py,
                unsigned int Pz,
                int fftDir );

  ~FFT3D();

  void Init();
  void Execute(complex ***in, complex ***out);

  void GetPx(int &px);
  void GetPy(int &py);
  void GetPz(int &pz);

  void GetLocalNx(int &localNx);
  void GetLocalNy(int &localNy);
  void GetLocalNz(int &localNz);

  static int PacketActorFx_X(void* pkt);
  static int PacketActorFx_Y(void* pkt);
  static int PacketActorFx_Z(void* pkt);
  static int PacketActorFx_B(void* pkt);

  void GatherZ(complex*** in, int dim);
  void GatherX(complex**  in, int dim);
  void GatherY(complex**  in, int dim);
  void GatherB(complex*** out, int dim);
  static void ActiveMsgBarrier(int & completePencilsInBarCount,
                                int   barSize);
  
  // methods for debbugging ...
  void PrintComplexArrayR(complex ** array, int nrow, int ncol);
  void PrintComplexArrayI(complex ** array, int nrow, int ncol);
  int Validate3DFFT(complex ***in, const int globalNx);

#define max(i, j) ( (i>j)?i:j )
#define min(i, j) ( (i<j)?i:j )
};

#endif
