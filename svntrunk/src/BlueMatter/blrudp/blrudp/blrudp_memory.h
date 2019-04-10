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
 #ifndef _BLRUDP_MEMORY_H_
#define _BLRUDP_MEMORY_H_

////////////////////////////////////////////////////////////////////////////
// Bluelight Reliable UDP memory functions.
//
// This set of macros maps the memory allocation routines used
// by the reliable UDP module to functions sutiable for use
// with both the kernel module, linux and AIX.  for
//
// For non-kernel modules these macros meerely map to malloc and free.
//
// for kernel level macros, they map to something else.
//
// 
//

#ifdef __cplusplus
extern "C" {
#endif


#include <malloc.h>

//
// for some stupid reason AIX redefines these to be xmalloc as a result of other
// includes that we have done here...
//
#if HAVE_STDLIB_H
	#include <stdlib.h>
#endif


/////////////////////////////////////////////////////////////////////////
// void *BlrAllocMem(size_t nMemSize)
//
// Perform a normal memory allocation.
//
// If you called this routine you MUST call BlrFree when done with it.
// 
//
// inputs:
//    nMemSize -- size of the memory in bytes to allocate.
// outputs:
//    returns -- pointer to memory allocated, or NULL if not allocated
//
#define BlrAllocMem(nMemSize) \
        malloc((nMemSize))
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// BlrAllocAlignedMem(size_t nMemSize)
//
// Perform a normal memory allocation alligned to cache boundiries.
//
// If you called this routine you MUST call BlrFreeAlignedMem when done with it.
// 
//
// inputs:
//    nMemSize -- size of the memory in bytes to allocate.
// outputs:
//    returns -- pointer to memory allocated, or NULL if not allocated
//
#define BlrAllocAlignedMem(nMemSize) \
        malloc((nMemSize))
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// BlrFreeMem(void *pMem);
//
// Free memory allocated by BlrAllocMem...
//
// inputs:
//    pMem -- pointer to memory to free.
// outputs:
//    none.
#define BlrFreeMem(pMem) \
        free((pMem))
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// BlrFreeAlignedMem(void *pMem);
//
// Free memory allocated by BlrAllocMem...
//
// inputs:
//    pMem -- pointer to memory to free.
// outputs:
//    none.
#define BlrFreeAlignedMem(pMem) \
        free((pMem))
////////////////////////////////////////////////////////////




#ifdef __cplusplus
};
#endif

#endif


