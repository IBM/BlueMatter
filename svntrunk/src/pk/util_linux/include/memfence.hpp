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
 /***************************************************************************
 * Project:         pK
 *
 * Module:          memfence.hpp
 *
 * Purpose:         Memory fencing routines for IBM AIX SMP machines.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         000097 TJCW Created.
 *
 ***************************************************************************/

#ifndef __MEMFENCE_HPP__
#define __MEMFENCE_HPP__

//////
//////#ifndef LINUX
//////#include <sys/atomic_op.h>
//////#endif
//////
//////// ExportFence ensures that all stores have been done (as far as L1 cache) before
//////// letting any more stores start
//////inline void ExportFence(void)
//////  {
//////  int a ;
//////  _clear_lock(&a,0) ;
//////  }
//////
//////// ImportFence ensures that any 'load' instructions before here in program order
//////// complete before any 'load' instructions after here in program order are
//////// started
//////inline void ImportFence(void)
//////  {
//////  int a = 0 ;
//////  (void) _check_lock(&a, 0, 0) ;
//////  }
//////// ExportFence and ImportFence between them can be used to ensure that on a
//////// weakly-ordered-memory SMP, a 'reader' thread can coherently see all the
//////// changes made by a 'writer' thread. Only use them when this coherence is
//////// necessary; the purpose of weakly-ordered-memory is to improve performance
//////// when the coherence is not necessary
//////// If you are running non-threaded, then you could dummy out these functions.
//////// If you are running on a uniprocessor, you could dummy out these functions,
//////// or you could leave them and they will be fairly-efficiently dummied by the
//////// AIX kernel millicode
//////
#endif // nothing below this line
