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
 // This is file provides the abstarct class for the communication.

#ifndef __COMM3DLAYER_ABC_H_
#define __COMM3DLAYER_ABC_H_

template <class T>
class Comm3DLayerABC
{
public:
  virtual ~Comm3DLayerABC(){}
  virtual inline void allToAllv(void* sendBuf,int* sendCount, int* sendDisp,  
       void* recvBuf,int* recvCount, int* recvDisp, int dir, int justforspi)=0;
  
  virtual inline void getCartCoords(int* myPx, int* myPy, int* myPz) const=0;
  virtual inline void getCartRank(int* rank) const = 0;
  virtual inline void getCartRank( int* coords, int* rank) const=0;

  virtual inline void getSubCommSize(int subCommDir, int *size) const=0;
  virtual inline void getSubCommRank(int subCommDir, int* coords, int *rank) const =0;
  virtual inline void getSubCommRank(int subCommDir, int *rank) const=0;
  virtual inline void globalBarrier() const=0;
};
  
#endif


  
