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
 /*
 * builtin interface to hummer^2 instructions for TOBEY
 */

#define _Complex __complex__
// #include <complex.h>

extern "C" {
/* float, double loads */
double _Complex __lfps (float * addr);
double _Complex __lfxs (float * addr);
double _Complex __lfpd (double * addr);
double _Complex __lfxd (double * addr);

/* float, double stores */
void __stfps (float * addr, double _Complex);
void __stfxs (float * addr, double _Complex);
void __stfpd (double * addr, double _Complex);
void __stfxd (double * addr, double _Complex);
void __stfpiw (int * addr, double _Complex);

/* parallel operations */
double _Complex __fpadd (double _Complex, double _Complex);
double _Complex __fpsub (double _Complex, double _Complex);
double _Complex __fpmul (double _Complex, double _Complex);
double _Complex __fxmul (double _Complex, double _Complex);
double _Complex __fxpmul (double _Complex, double);
double _Complex __fxsmul (double _Complex, double);

/* multiply add */
double _Complex __fpmadd (double _Complex b, double _Complex a, double _Complex c);
double _Complex __fpnmadd (double _Complex b, double _Complex a, double _Complex c);
double _Complex __fpmsub (double _Complex b, double _Complex a, double _Complex c);
double _Complex __fpnmsub (double _Complex b, double _Complex a, double _Complex c);

double _Complex __fxmadd (double _Complex b, double _Complex a, double _Complex c);
double _Complex __fxnmadd (double _Complex b, double _Complex a, double _Complex c);
double _Complex __fxmsub (double _Complex b, double _Complex a, double _Complex c);
double _Complex __fxnmsub (double _Complex b, double _Complex a, double _Complex c);

double _Complex __fxcpmadd (double _Complex b, double _Complex a, double c);
double _Complex __fxcsmadd (double _Complex b, double _Complex a, double c);
double _Complex __fxcpnmadd (double _Complex b, double _Complex a, double c);
double _Complex __fxcsnmadd (double _Complex b, double _Complex a, double c);
double _Complex __fxcpmsub (double _Complex b, double _Complex a, double c);
double _Complex __fxcsmsub (double _Complex b, double _Complex a, double c);
double _Complex __fxcpnmsub (double _Complex b, double _Complex a, double c);
double _Complex __fxcsnmsub (double _Complex b, double _Complex a, double c);

double _Complex __fxcpnpma (double _Complex b, double _Complex a, double c);
double _Complex __fxcsnpma (double _Complex b, double _Complex a, double c);
double _Complex __fxcpnsma (double _Complex b, double _Complex a, double c);
double _Complex __fxcsnsma (double _Complex b, double _Complex a, double c);

double _Complex __fxcxnpma (double _Complex b, double _Complex a, double c);
double _Complex __fxcxnsma (double _Complex b, double _Complex a, double c);
double _Complex __fxcxma (double _Complex b, double _Complex a, double c);
double _Complex __fxcxnms (double _Complex b, double _Complex a, double c);

double _Complex __fpsel (double _Complex a, double _Complex b, double _Complex c);
double _Complex __fpctiw (double _Complex a);
double _Complex __fpctiwz (double _Complex a);
double _Complex __fprsp (double _Complex a);
double _Complex __fpneg (double _Complex a);
double _Complex __fpabs (double _Complex a);
double _Complex __fpnabs (double _Complex a);
double _Complex __fxmr (double _Complex a);

/* estimates */
double _Complex __fpre (double _Complex);
double _Complex __fprsqrte (double _Complex);

/* alignment: like prefetchByStream */
void __alignx (int, const void *);
};
