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
 * Module:          pktrace.cpp
 *
 * Purpose:
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         090497 BGF Created.
 *                  070503 ARAYSHU added function.  
 *
 ***************************************************************************/

#include <pk/pktrace.hpp>

//#ifdef PK_BGL 
AtomicNative<unsigned short> GlobalTraceIdCounter[2];
Platform::Mutex::MutexWord* PKTRACE_Mutex;
Platform::Mutex::MutexWord* PKTRACE_TimeHit_Mutex;
Platform::Mutex::MutexWord* PKTRACE_Flush_Mutex;

unsigned long long* pkTraceServer::mTimeStampsBuffer[ 2 ]; // [ TIME_STAMP_BUFFER_SIZE ];
TimeStampName* pkTraceServer::mTimeStampNames[2];
AtomicNative<unsigned long> pkTraceServer::mHits[2];
AtomicNative<unsigned long> pkTraceServer::mNameCount[2];
//#endif
