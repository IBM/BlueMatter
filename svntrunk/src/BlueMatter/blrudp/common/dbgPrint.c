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
 #ifdef _DEBUG


#include "hostcfg.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

#include <stdarg.h>
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MTI_SIM
#include <mti.h>
#endif

#include "dbgPrint.h"

////////////////////////////////////////////////////////////////
// This file contains the definition needed to do debug printf
// to the debug output.
//

int DBGPrintf(const char *szFormat, ... ) 
{ 
#ifndef BLHPK
    static char buf[512]; 
    va_list va;

#ifdef MTI_SIM
	printf("%dns: ", mti_Now()/100);
#endif

    va_start(va, szFormat); 

    vsprintf(buf, szFormat, va); 

    va_end(va);

#ifdef WIN32
    OutputDebugString(buf); // go to the debug console
#else
    printf(buf);            // out to standard output
#endif

#endif
	return(0);
} 

#ifdef __cplusplus
};
#endif

#endif
