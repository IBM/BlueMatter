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

#include <iostream>
#include "fft3D.hpp"
#include <mpi.h>
template class FFT3D<FFTOneD, Comm3DLayer>;

template<class FFT_T, class COMM_T>
FFT3D<FFT_T, COMM_T> :: ~FFT3D()
{
  delete _fftx;
  delete _ffty;
  delete _fftz;

  int maxLocalN  = max(max(_localNx, _localNy), _localNz);
  int maxBarSz   = max(max(_barSzX, _barSzY), _barSzZ);
  int maxBarNum  = max(max(_pX, _pY), _pZ);

  for(int i=0; i<maxLocalN*maxBarSz; i++) {
    delete[] _globalIn[i];
    delete[] _globalOut[i];
  }
  delete  _globalIn;
  delete  _globalOut;
  
  for(int i=0; i<maxBarNum; i++) {
    for(int j=0; j<maxLocalN*maxBarSz; j++) {
      delete[] _localIn[i][j];
      delete[] _localOut[i][j];
    }
    delete[] _localIn[i];
    delete[] _localOut[i];
  }
  delete _localIn;
  delete _localOut;
}

template<class FFT_T, class COMM_T>
FFT3D<FFT_T, COMM_T> :: FFT3D(unsigned int localNx,
			      unsigned int localNy,
			      unsigned int localNz,
			      unsigned int pX,
			      unsigned int pY,
			      unsigned int pZ,
			      int fftDir, 
			      MPI_Comm comm):  // TODO: make this genera
  _localNx(localNx), _localNy(localNy), _localNz(localNz),
  _pX(pX), _pY(pY), _pZ(pZ),_fftDir(fftDir) {
  _comm = new COMM_T(_pX, _pY,_pZ, comm);

  _wX[0] = 1; _wX[1] = 0; _wX[2] = 0;
  _wY[0] = 0; _wY[1] = 1; _wY[2] = 0;
  _wZ[0] = 0; _wZ[1] = 0; _wZ[2] = 1;
  
  _barSzX = _localNy/_pX; // size of bars in the y-dim
  _barSzY = _localNz/_pY; // size of bars in the z-dim
  _barSzZ = _localNx/_pZ; // size of bars in the x-dim

  _fftx = new FFT_T(_localNx*_pX, _fftDir);  // 1D fft in the x-dim
  _ffty = new FFT_T(_localNy*_pY, _fftDir);  // 1D fft in the y-dim
  _fftz = new FFT_T(_localNz*_pZ, _fftDir);  // 1D fft in the z-dim
  
  int maxBarSz   = max(max(_barSzX, _barSzY), _barSzZ);
  int maxLocalN  = max(max(_localNx, _localNy), _localNz);
  int maxGlobalN = max(max(_localNx*_pX, _localNy*_pY), _localNz*_pZ);
  int maxBarNum  = max(max(_pX, _pY), _pZ);

  if(maxBarSz==0 || maxLocalN==0 || maxGlobalN==0 || maxBarNum==0){
    maxBarSz+=1;
    cout << " Error in the allocation  "<<endl; 
    cout << " maxBarSz =    " <<maxBarSz <<endl;
    cout << " barSzX = " << _barSzX     << " barSzY = " 
	 << _barSzY     << "barSzZ = " << _barSzZ <<endl;
    cout << " maxLocalN =   " << maxLocalN <<endl;
    cout << " maxGlobalN =  " << maxGlobalN << endl;
    cout << " maxBarNum  =  " << maxBarNum <<endl; 
  }

 
  _globalIn  = (complex**) new complex[maxLocalN*maxBarSz];
  _globalOut = (complex**) new complex[maxLocalN*maxBarSz];

  for(int i=0; i<maxLocalN*maxBarSz; i++) {
    _globalIn[i]  = (complex*) new complex[maxGlobalN];
    _globalOut[i] = (complex*) new complex[maxGlobalN];
  }

  _localIn  = (complex***) new complex[maxLocalN];
  _localOut = (complex***) new complex[maxLocalN];
   
  for(int i=0; i<maxBarNum; i++) {
    
    _localIn[i]  = (complex**) new complex[maxLocalN];
    _localOut[i] = (complex**) new complex[maxLocalN];
    
    for(int j=0; j<maxLocalN*maxBarSz; j++) {
      _localIn[i][j]  = (complex*) new complex[maxLocalN];
      _localOut[i][j] = (complex*) new complex[maxLocalN];
    }
    
  }
  
}

template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::gatherZ(complex *** in, int*w, int dim) {
  int barSz = w[0]*_barSzX+ w[1]*_barSzY + w[2]*_barSzZ; 
  int myPx, myPy, myPz;

  (*_comm).getMyPx(&myPx);
  (*_comm).getMyPy(&myPy);
  (*_comm).getMyPz(&myPz);

  int myP   = w[0]*myPx + w[1]*myPy + w[2]*myPz;
  int nBars = w[0]*_pX  + w[1]*_pY  + w[2]*_pZ;
 
  for(int i=0; i<_localNx; i++) 
    for(int j=0; j<_localNy; j++)
      for(int k =0; k<_localNz; k++){
        
	int targetProc = i /_barSzZ;

	if(myPz != targetProc) { // send data along the z dim
	  (*_comm).send(myPx, myPy, targetProc,
			&in[i][j][k], 2, dim);
	}
	else {  // copy to the global Array
	  int a = (i%_barSzZ)*_localNy+j;
	  int b = k + myPz*_localNz;
	  _globalIn[a][b].re = in[i][j][k].re;
	  _globalIn[a][b].im = in[i][j][k].im;
	  
	  for(int p=0; p<_pZ; p++) {
	    if(p==myPz) continue;
	    
	    int ap = (i%_barSzZ)*_localNy+j;
	    int bp = k + p*_localNz;
	    
	    (*_comm).recv(myPx, myPy, p, &_globalIn[ap][bp],2, dim); 
	  } 
	}
      }
}


template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::gatherX(complex ** globalOut, int*w, int dim) {
  int myPx, myPy, myPz;
  (*_comm).getMyPx(&myPx);
  (*_comm).getMyPy(&myPy);
  (*_comm).getMyPz(&myPz);

  for(int a=0; a< (_localNx/_pZ)*_localNy; a++) 
    for(int b=0; b<_localNz*_pZ; b++){
      int ap = ((a%_localNy)%_barSzX)*_localNz + (b%_localNz);
      int bp = (a/_localNy) + myPz*_barSzZ + myPx*_localNx;
      
      int trgPz = b /_localNz;
      int trgPx = (a%_localNy)/_barSzX;
      
       if(trgPx==myPx&&trgPz==myPz) {
	 _globalIn[ap][bp].re = _globalOut[a][b].re;
	 _globalIn[ap][bp].im = _globalOut[a][b].im;
		 
	 for(int pz=0; pz<_pZ; pz++){
	   for(int px=0; px<_pX; px++) {
	     ap = ((a%_localNy)%_barSzX)*_localNz + (b%_localNz);
	     bp = (a/_localNy) + pz*_barSzZ + px*_localNx;

	     if(px==myPx && pz==myPz) continue;
	     (*_comm).recv(px, myPy, pz, &_globalIn[ap][bp], 2, dim);
	     
	   }
	 }
       } 
       else if(trgPx!=myPx || trgPz!=myPz){
	(*_comm).send(trgPx, myPy, trgPz, &_globalOut[a][b], 2, dim);
      }
      
    }
}


template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::gatherY(complex ** globalOut, int*w, int dim) {
  int myPx, myPy, myPz;
  (*_comm).getMyPx(&myPx);
  (*_comm).getMyPy(&myPy);
  (*_comm).getMyPz(&myPz);

  for(int a=0; a< (_localNy/_pX)*_localNz; a++) 
    for(int b=0; b<_localNx*_pX; b++){
      int ap = ((a%_localNz)%_barSzY)*_localNx
	+ (b%_localNx);
      int bp = (a/_localNz) +myPx*_barSzX + myPy*_localNy;
      
      int trgPx = b /_localNx;
      int trgPy = (a%_localNz)/_barSzY;
      
      if(trgPx==myPx&&trgPy==myPy) {
	_globalIn[ap][bp].re = _globalOut[a][b].re;
	 _globalIn[ap][bp].im = _globalOut[a][b].im;
	 for(int px=0; px<_pX; px++){
	   for(int py=0; py<_pY; py++) {
	     ap = ((a%_localNz)%_barSzY)*_localNx
	       + (b%_localNx);
	     bp = (a/_localNz) +px*_barSzX + py*_localNy;
	     if(px==myPx && py==myPy) continue;
	     (*_comm).recv(px, py, myPz, &_globalIn[ap][bp], 2,dim);
	   }
	 }
      } 
      else if( trgPx!=myPx||trgPy!=myPy){
	(*_comm).send(trgPx, trgPy, myPz, &_globalOut[a][b], 2, dim);
      }
    }
  
  //(*_comm).MPIBarrier(dim); 
}


template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::gather(complex *** out, int*w, int dim) {
 int myPx, myPy, myPz;
  (*_comm).getMyPx(&myPx);
  (*_comm).getMyPy(&myPy);
  (*_comm).getMyPz(&myPz);

  for(int a=0; a<(_localNz/_pY)*_localNx; a++){
    for(int b=0; b<_localNy*_pY; b++) {
      int i = a%_localNx;
      int j = b%_localNy;
      int k = a/_localNx + myPy * _barSzY;
      
      int trgPy = b/_localNy;
      if(trgPy == myPy) {
	
	out[i][j][k].re = _globalOut[a][b].re;
	out[i][j][k].im = _globalOut[a][b].im;
	for(int p =0; p<_pY; p++){
	  if(p==trgPy) continue;
	  int k = a/_localNx + p* _barSzY;
	  (*_comm).recv(myPx, p, myPz, &out[i][j][k], 2, dim);
	}
      }
      else {
	(*_comm).send(myPx, trgPy,myPz, &_globalOut[a][b], 2, dim);
      }
   }
  }

}

template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::_printComplexArrayR(complex **array, 
					      int nrow, int ncol) {
  int myPx, myPy, myPz;
  (*_comm).getMyPx(&myPx);
  (*_comm).getMyPy(&myPy);
  (*_comm).getMyPz(&myPz);

  for(int i=0; i<nrow; i++) {
    for(int j=0; j<ncol; j++){
      cout << "globalOut [ "<< i << "," << j <<"][" <<
	myPx <<"," << myPy << ","<< myPz<<"] = "<< array[i][j] <<endl;
    }
  }
  cout << endl <<endl;
}

template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::_printComplexArrayI(complex **array, 
					       int nrow, int ncol) {
  int myPx, myPy, myPz;
  (*_comm).getMyPx(&myPx);
  (*_comm).getMyPy(&myPy);
  (*_comm).getMyPz(&myPz);

  for(int i=0; i<nrow; i++) {
    for(int j=0; j<ncol; j++){
      cout << "globalIn [ "<< i << "," << j <<"][" <<
	myPx <<"," << myPy << ","<< myPz<<"] = "<< array[i][j] <<endl;
    }
    cout<<endl<<endl;
  }
}


template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::fft(complex*** in, complex*** out)  {
  int myPx, myPy, myPz;
  (*_comm).getMyPx(&myPx);
  (*_comm).getMyPy(&myPy);
  (*_comm).getMyPz(&myPz);
  
  // gather all the data on the z-nodes  
  gatherZ(in,_wZ, 0);  
  (*_comm).MPIBarrier(MPI_COMM_WORLD);
  
  for(int n=0; n<(_localNx/_pZ)*_localNy; n++){
    (*_fftz).fftInternal(_globalIn[n], _globalOut[n]); 
  }
  
   gatherX(_globalOut, _wX, 1);
   (*_comm).MPIBarrier(MPI_COMM_WORLD);
  
   for(int n=0; n< (_localNy/_pX)*_localNz; n++){
     (*_fftx).fftInternal(_globalIn[n], _globalOut[n]);
   }
   
   (*_comm).MPIBarrier(1);
   gatherY(_globalIn, _wZ, 2);
   (*_comm).MPIBarrier(2);
   
   for(int n=0; n<(_localNz/_pY)*_localNx; n++){
     (*_ffty).fftInternal(_globalIn[n], _globalOut[n]);
   }
   (*_comm).MPIBarrier(2);
   
   gather(out, _wX, 3);
   //  _printComplexArray(_globalOut, (_localNy/_pX)*_localNz, _localNx);
   (*_comm).MPIBarrier(MPI_COMM_WORLD); 
}



template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::getLocalNx(int &localNx) {
  localNx = _localNx;
}

template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::getLocalNy(int &localNy) {
  localNy = _localNy;
}

template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::getLocalNz(int &localNz) {
  localNz = _localNz;
}

template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::getPx(int &px) {
  px = _pX;
}

template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::getPy(int &py) {
  py = _pY;
}

template<class FFT_T, class COMM_T>
void FFT3D<FFT_T, COMM_T>::getPz(int &pz) {
  pz = _pZ;
}


