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
 #define __lfpd __dummy_lfpd
#define __stfpd __dummy_stfpd
#include <builtins.h>
#undef __lfpd
#undef __stfpd
__builtinDecl double _Complex __lfpd (volatile double * addr);
__builtinDecl void __stfpd (volatile double * addr, double _Complex);
void fifoget(volatile double * fifo, double * result)
{
	double _Complex first = __lfpd(fifo) ; 
	double _Complex second = __lfpd(fifo) ; 
	* (double _Complex *) result = first ; 
	* (double _Complex *) (result+2) = second ; 
}

void fifoput(volatile double * fifo, double * source)
{
	double _Complex first = * (double _Complex *) source ; 
	double _Complex second = * (double _Complex *) (source+2) ;
	__stfpd(fifo, first) ; 
	__stfpd(fifo, second) ; 
}
