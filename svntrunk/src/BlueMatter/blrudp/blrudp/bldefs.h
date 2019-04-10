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
 
#ifndef _BLDEFS_H_
#define _BLDEFS_H_
////////////////////////////////////////////////////////////////
//
// Blue light common definitions...
//
///////////////////////////////////////////////////////////////////


#if defined(_MSC_VER)
#pragma warning( disable : 4786 )  // "identifier truncated to 255 chars"
#endif

#if !defined(_MSC_VER)
//
// some minor linux/unix differences       
#define stricmp strcasecmp
#endif

#define FAIL goto Fail
#define FAILERR(nErr, nValue) {(nErr) = (nValue); goto Fail;}


#define SIZEOF_ARRAY(n) (sizeof(n)/sizeof(n[0]))

#ifndef TRUE
#define TRUE 1
#define FALSE 0 
#endif


#ifndef min
#define min(a,b) ((int)(a) > (int)(b)? (b):(a))
#endif

#ifndef max
#define max(a,b) ((int)(a) < (int)(b)? (b):(a))
#endif


///////////////////////////////////////////////////////////////////////////////
// some debug test flags to force us to exercise some pathalogical code.
//
#ifdef _DEBUG 

#define TEST_HALF_OPEN 0

#define TEST_DROPPED_PKTS 0
#define TEST_DROPPED_PKTS_INTERVAL 3            // drop every third packet....

#endif



#endif
