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
 #ifndef SWAP_HPP
#define SWAP_HPP

// AK : added include
#include <BlueMatter/XYZ.hpp> // required for XYZ

class SwapIO
{
private:
    
    static inline void Swap(char *c, const int j, const int k) {
	char t = c[j];
	c[j] = c[k];
	c[k] = t;
    }
    
    static inline void Swap_2(char *c) {
	Swap(c, 0, 1);
    }
    
    static inline void Swap_4(char *c) {
	Swap(c, 0, 3);
	Swap(c, 1, 2);
    }
    
    static inline void Swap_8(char *c) {
	Swap(c, 0, 7);
	Swap(c, 1, 6);
	Swap(c, 2, 5);
	Swap(c, 3, 4);
    }
    
    static inline void Swap(float *f) {
	Swap_4((char *)f);
    }
    
    static inline void Swap(double *d) {
	Swap_8((char *)d);
    }
    
    static inline void Swap(int *i) {
	Swap_4((char *)i);
    }
    
    static inline void Swap(unsigned short *i) {
	Swap_2((char *)i);
    }

    static inline void Swap(short *i) {
	Swap_2((char *)i);
    }
    
    static inline void Swap(XYZ *v) {
	Swap(&v->mX);
	Swap(&v->mY);
	Swap(&v->mZ);
    }

    static inline void Swap(fXYZ *v) {
	Swap(&v->mX);
	Swap(&v->mY);
	Swap(&v->mZ);
    }

    static inline void Swap(sXYZ *v) {
	Swap(&v->mX);
	Swap(&v->mY);
	Swap(&v->mZ);
    }

    static inline void Swap(usXYZ *v) {
	Swap(&v->mX);
	Swap(&v->mY);
	Swap(&v->mZ);
    }


public:    
    
    template <class T> static inline int Read(int f, T *pt) {
	int NumToRead = sizeof(T);
	if (NumToRead != _read(f, pt, NumToRead))
	    return 1;
#ifdef SWAP_IN
	Swap(pt);
#endif
	return 0;
    }
    
    template <class T> static inline int Read(FILE *f, T *pt) {
	int NumToRead = sizeof(T);
	if (NumToRead != fread(pt, 1, NumToRead, f))
	    return 1;
#ifdef SWAP_IN
	Swap(pt);
#endif
	return 0;
    }
    
    template <class T> static inline int Write(int f, T *pt) {
	T p = *pt;
#ifdef SWAP_OUT
	Swap(&p);
#endif
	int NumToWrite = sizeof(T);
	if (NumToWrite != _write(f, p, NumToWrite))
	    return 1;
	return 0;
    }
    
    template <class T> static inline int Write(FILE *f, T *pt) {
	T p = *pt;
#ifdef SWAP_OUT
	Swap(&p);
#endif
	int NumToWrite = sizeof(T);
	if (NumToWrite != fwrite(&p, 1, NumToWrite, f))
	    return 1;
	return 0;
    }

    // static inline int Read(int f, float *pt) { return Read(f, pt); }

};

#endif
