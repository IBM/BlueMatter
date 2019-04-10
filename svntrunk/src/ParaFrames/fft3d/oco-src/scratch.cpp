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
 template <typename T> 
void PrivateFFT<T>::Plan( int fwd_rev )
template <typename T> 
void PrivateFFT<T>::SetSendRecvIndices()
template <typename T> 
void PrivateFFT<T>::SetAllToAllIndexInPlane(int aDir, int srcP, int trgP)
template <typename T> 
void  PrivateFFT<T>::DoLocalMapping(Distribution3DPoint &pointMin,
				    Distribution3DPoint &pointMax, 
				    int x, int y, int z)
template <typename T> 
void PrivateFFT<T>::calcLocalIndexOZ(int dir)

template <typename T> 
void PrivateFFT<T>::calcLocalIndexZY(int dir)
template <typename T> 
void PrivateFFT<T>::calcLocalIndexYX(int dir)

template <typename T> 
void PrivateFFT<T>::AllocateMemory()
template <typename T>   
void PrivateFFT<T>::Finalize()
template <typename T> 
void PrivateFFT<T>::ReorderDoFFTs(int dir,
			    int* srcStride, 
			    int* trgStride,
			    int* localSize,
			    complexTemplate<T>* localDataSrc,
			    complexTemplate<T>* localDataTrg,
			    FFTOneD<T> *fft) 
template <typename T> 
void PrivateFFT<T>::ReorderDoFFTs_R(int dir, int *srcStride, int *trgStride,int *localSize,
			      complexTemplate<T>* localDataSrc, complexTemplate<T>* localDataTrg, FFTOneD<T>* fft)
template <typename T> 
void PrivateFFT<T>::DoFwdFFT(complexTemplate<T>* bglfftIn, complexTemplate<T>* bglfftOut)
template <typename T> 
void PrivateFFT<T>::DoFwdFFT()
template <typename T> 
void PrivateFFT<T>::DoRevFFT(complexTemplate<T>* bglfftIn,complexTemplate<T>*  bglfftOut)
template <typename T> 
void PrivateFFT<T>::DoRevFFT()
