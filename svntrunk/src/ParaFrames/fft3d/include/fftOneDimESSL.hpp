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
 /*#####################################################################
#                        I.B.M. CONFIDENTIAL                        #
#      (c) COPYRIGHT IBM CORP. 2001, 2003 ALL RIGHTS RESERVED.      #
#                                                                   #
#                LICENSED MATERIALS-PROPERTY OF IBM.                #
#                                                                   #
#####################################################################
*/
//  
//  IBM T.J.W  R.C.
//  Date : 24/01/2003
//  Name : Maria Eleftheriou
//  Last Modified: 
//  fft_one_dim_ESSL.hpp
//


#ifndef __FFTONED_H_
#define __FFTONED_H_

#include <math.h>
#include <assert.h>
//#include <complexTemplate.hpp>

#include <complex>
#ifdef PK_BGL
#else
#include <fftw.h>
#endif

extern "C" double rtc(void);         // elapsed-time timer from -lxlf90   


extern "C" void scft(int * init,     // initialization flag
                     float * x,     // input array x
                     int * inc1x,    // element stride for x (usually 1)
                     int * inc2x,    // stride between sequences of x 
                     float * y,     // result array y
                     int * inc1y,    // element stride for y (usually 1)
                     int * inc2y,    // stride between sequences of y
                     int * nlen,     // transform length
                     int * nseq,     // number of sequences
                     int * isign,    // direction of the transform
                     float * scale, // scale factor
                     double * aux1,  // work space
                     int * naux1,    // work space dimension
                     double * aux2,  // work space
                     int * naux2);   // work space dimension

extern "C" void dcft(int * init,     // initialization flag
                     double * x,     // input array x
                     int * inc1x,    // element stride for x (usually 1)
                     int * inc2x,    // stride between sequences of x 
                     double * y,     // result array y
                     int * inc1y,    // element stride for y (usually 1)
                     int * inc2y,    // stride between sequences of y
                     int * nlen,     // transform length
                     int * nseq,     // number of sequences
                     int * isign,    // direction of the transform
                     double * scale, // scale factor
                     double * aux1,  // work space
                     int * naux1,    // work space dimension
                     double * aux2,  // work space
                     int * naux2);   // work space dimension


template <class T, class Tcomplex> 
class FFTOneD {
private:
  unsigned int _vectorSz;  // size of the 1D-FFT
  int _fftDir;
  int _init;  // Flag: 1 for initialization, 2 for performing the 1D FFT
  int _incx1; // stride for input arrayl 
  int _incx2; // stride between sequential of 1D FFTs
  int _incy1; // stride for the output (transformed) array
  int _incy2; // stride between sequencies of transformed 1D FFT
  int _naux1; // dimentions of the work array
  int _naux2; // dimentions of the work array
  int _n;     // lenght of the transformed array
  int _m;     // number of FFTs to transform
 
  double* _aux1; // work array
  double* _aux2; // work array
  T _scale;   // 1 for not normalizing the results
  
public:
//  inline FFTOneD(unsigned int vectorSz,  int fftDir, 
//      Tcomplex* in, Tcomplex*out, unsigned int numOfFFTs = 1);
  
  FFTOneD(unsigned int vectorSz,  int fftDir, 
      unsigned int numOfFFTs = 1);
  
  FFTOneDInit(unsigned int vectorSz,  int fftDir, 
      unsigned int numOfFFTs = 1);
  
  ~FFTOneD();
  
  inline void fftInternal(Tcomplex* globalIn, Tcomplex* globalOut);
  
  inline void fftInit(Tcomplex* globalIn, Tcomplex* globalOut);

  inline void printFFT(Tcomplex* *in)
  {
    for(int i=0; i<_vectorSz; i++)
      {
	printf("in[%d] = (%f, %f)  \n", i, in[i].re, in[i].im);
      }
  }
    
};

template <class T, class Tcomplex> 
FFTOneD<T, Tcomplex>::~FFTOneD() 
{
  delete[] _aux1;
  delete[] _aux2;
}

// template <class T, class Tcomplex> 
template<>
void  FFTOneD<double, complex<double> >::fftInternal(complex<double> *in, 
    complex<double> *out) 
{
  int init = 0;
      dcft(&init, (double*)in, &_incx1, &_incx2, (double*)out, &_incy1, &_incy2, &_n, &_m,
           &_fftDir, &_scale,_aux1, &_naux1, _aux2, &_naux2); 
}

// template <class T, class Tcomplex> 
template<>
void  FFTOneD<float, complex<float> >::fftInternal(complex<float> *in, 
    complex<float> *out) 
{
  int init = 0;
  scft(&init, (float*)in, &_incx1, &_incx2, (float*)out, &_incy1, &_incy2, &_n, &_m,
       &_fftDir, &_scale,_aux1, &_naux1, _aux2, &_naux2); 
}

// template <class T, class Tcomplex> 
template<>
void  FFTOneD<double, complex<double> >::fftInit(complex<double> *in, 
    complex<double> *out) 
{
  int init = 1;
      dcft(&init, (double*)in, &_incx1, &_incx2, (double*)out, &_incy1, &_incy2, &_n, &_m,
           &_fftDir, &_scale,_aux1, &_naux1, _aux2, &_naux2); 
}

// template <class T, class Tcomplex> 
template<>
void  FFTOneD<float, complex<float> >::fftInit(complex<float> *in, 
    complex<float> *out) 
{
  int init = 1;
  scft(&init, (float*)in, &_incx1, &_incx2, (float*)out, &_incy1, &_incy2, &_n, &_m,
       &_fftDir, &_scale,_aux1, &_naux1, _aux2, &_naux2); 
}

//template <class T, class Tcomplex> 
//void  FFTOneD<T, Tcomplex>::fftInternal(Tcomplex *in, 
//            Tcomplex *out) 
//{
//  int init = 0;
//  if( sizeof(T) == sizeof(float))
//    {
//      scft(&init, (float*)in, &_incx1, &_incx2, (float*)out, &_incy1, &_incy2, &_n, &_m,
//           &_fftDir, &_scale,_aux1, &_naux1, _aux2, &_naux2); 
//    }
//  else
//    {
//      dcft(&init, (double*)in, &_incx1, &_incx2, (double*)out, &_incy1, &_incy2, &_n, &_m,
//           &_fftDir, &_scale,_aux1, &_naux1, _aux2, &_naux2); 
//    }
//
//}

//// template <class T, class Tcomplex> 
//template<>
//void  FFTOneD<double, complex<double> >::FFTOneDInit(unsigned int vectorSz,
//		    int fftDir,
////		    Tcomplex* in,
////		    Tcomplex* out,
//		    unsigned int numOfFFTs)
//{
//  _vectorSz=vectorSz ;
//  _fftDir =fftDir;
//  assert(fftDir!=1||fftDir!=-1);
//  assert(vectorSz>= 0);
//
//  _incx1 = 1;
//  _incx2 = _vectorSz;
//  _incy1 = 1;
//  _incy2 = _vectorSz;
//  _m     = 1;
//  _n     = _vectorSz;
//  
//  _scale = (_fftDir==1)?1:(1./_n);
//  int init=1;
//  
//  _naux1 = ((_vectorSz<=2048)?(2*20000):(2*(20000+1.14*_vectorSz)));
//  _naux2 = _naux1;
//
//  _aux1  = new double[_naux1];
//  _aux2  = new double[_naux2];
//
//  dcft(&init, (double*)NULL, &_incx1, &_incx2, (double*)NULL, &_incy1, &_incy2, &_n, &_m,
//       &_fftDir, &_scale,_aux1, &_naux1, _aux2, &_naux2);   
//
//}
//
//// template <class T, class Tcomplex> 
//template<>
//void  FFTOneD<float, complex<float> >::FFTOneDInit(unsigned int vectorSz,
//                    int fftDir,
//                    unsigned int numOfFFTs)
//{
//  _vectorSz=vectorSz;
//  _fftDir =fftDir;
//  assert(fftDir!=1||fftDir!=-1);
//  assert(vectorSz>= 0);
//
//  _incx1 = 1;
//  _incx2 = _vectorSz;
//  _incy1 = 1;
//  _incy2 = _vectorSz;
//  _m     = 1;
//  _n     = _vectorSz;
//  
//  _scale = (_fftDir==1)?1:(1./_n);
//  int init=1;
//  
//      _naux1 = ((_vectorSz<= 8192)?(2*20000):(2*(20000+1.14*_vectorSz)));
//      _naux2 = _naux1;
//      _aux1  = new double[_naux1];
//      _aux2  = new double[_naux2];
//    
//      scft(&init, (float*)NULL, &_incx1, &_incx2, (float*)NULL, &_incy1, &_incy2, &_n, &_m,
//           &_fftDir, &_scale,_aux1, &_naux1, _aux2, &_naux2);   
//
//}

template <class T, class Tcomplex> 
FFTOneD<T, Tcomplex>::FFTOneD(unsigned int vectorSz,
                    int fftDir,
//                  Tcomplex* in,
//                  Tcomplex* out,
                    unsigned int numOfFFTs):
  _vectorSz(vectorSz)
{
  _fftDir =fftDir;
  assert(fftDir!=1||fftDir!=-1);
  assert(vectorSz>= 0);

  _incx1 = 1;
  _incx2 = _vectorSz;
  _incy1 = 1;
  _incy2 = _vectorSz;
  _m     = 1;
  _n     = _vectorSz;
  
  _scale = (_fftDir==1)?1:(1./_n);
  int init=1;
  
  if( sizeof(T) == sizeof(float))
    {
      _naux1 = ((_vectorSz<= 8192)?(2*20000):(2*(20000+1.14*_vectorSz)));
      _naux2 = _naux1;
      // !!! suspect 'new float' might be sufficient !!!
      _aux1  = new double[_naux1];
      _aux2  = new double[_naux2];
    
//      scft(&init, (T*)in, &_incx1, &_incx2, (T*)out, &_incy1, &_incy2, &_n, &_m,
//           &_fftDir, &_scale,_aux1, &_naux1, _aux2, &_naux2);   
//      scft(&init, (float*)NULL, &_incx1, &_incx2, (float*)NULL, &_incy1, &_incy2, &_n, &_m,
//           &_fftDir, &_scale,_aux1, &_naux1, _aux2, &_naux2);   
    }
  else
    {
      _naux1 = ((_vectorSz<=2048)?(2*20000):(2*(20000+1.14*_vectorSz)));
      _naux2 = _naux1;
    
      _aux1  = new double[_naux1];
      _aux2  = new double[_naux2];
    
//      dcft(&init, (T*)in, &_incx1, &_incx2, (T*)out, &_incy1, &_incy2, &_n, &_m,
//           &_fftDir, &_scale,_aux1, &_naux1, _aux2, &_naux2);   
//      dcft(&init, (double*)NULL, &_incx1, &_incx2, (double*)NULL, &_incy1, &_incy2, &_n, &_m,
//           &_fftDir, &_scale,_aux1, &_naux1, _aux2, &_naux2);   
    }
    fftInit(NULL,NULL) ;
}

#endif
