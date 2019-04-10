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
 #ifndef INCLUDE_FFT1DFR_HPP
#define INCLUDE_FFT1DFR_HPP
#include <MPI/fftcomplex.hpp>
void fft128_forward(fftcomplex * target_a, fftcomplex * target_b ,const fftcomplex * source_a, const fftcomplex * source_b) ;
void fft128_inverse(fftcomplex * target_a, fftcomplex * target_b ,const fftcomplex * source_a, const fftcomplex * source_b) ;

void fft128_forward(fftcomplex * target_a, fftcomplex * target_b ,const fftcomplex * source_a, const fftcomplex * source_b)
{
        fft128 fft ;
        fft.copyin(source_a, source_b) ;
        fft.inline_forward() ;
        fft.copyout(target_a, target_b) ;
 }

void fft128_inverse(fftcomplex * target_a, fftcomplex * target_b ,const fftcomplex * source_a, const fftcomplex * source_b)
{
        fft128 fft ;
        fft.copyin(source_a, source_b) ;
        fft.inline_inverse() ;
        fft.copyout(target_a, target_b) ;
 }

#endif
