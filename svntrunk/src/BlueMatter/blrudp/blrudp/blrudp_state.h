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
 #ifndef _BLRUDP_STATE_H_
#define BLRUDP_STATE_H_

#include "blrudp_st.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////
// Forward declarations for state handling functions.
// See Blrudp_state.c file for details.
///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
const char *BlrudpStateString(int nState);
//
// Map a context state to a context state string.
//
// inputs:
//    nState -- context state to map.
// outputs:
//    returns -- pointer to context state string.
//
///////////////////////////////////////////////////////////////////

void BlrudpStFirstSentXmit(BLRUDP_ACTIVE_CONTEXT *pContext);

void BlrudpStFirstSentRecv(BLRUDP_ACTIVE_CONTEXT *pContext,
                           BLRUDP_IO_PKT *pPkt);
void BlrudpStFirstSentWTimer(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStFirstSentCTimer(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStFirstSentCTimeout(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStFirstSentRTimer(BLRUDP_ACTIVE_CONTEXT *pContext);

void BlrudpStFirstRecvXmit(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStFirstRecvRecv(BLRUDP_ACTIVE_CONTEXT *pContext,
                           BLRUDP_IO_PKT *pPkt);
void BlrudpStFirstRecvWTimer(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStFirstRecvCTimer(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStFirstRecvCTimeout(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStFirstRecvRTimer(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStEstabRecv(BLRUDP_ACTIVE_CONTEXT *pContext,
                           BLRUDP_IO_PKT *pPkt);

void BlrudpStEstabXmit(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStEstabWTimer(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStEstabCTimer(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStEstabCTimeout(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStEstabRTimer(BLRUDP_ACTIVE_CONTEXT *pContext);


void BlrudpStClosingXmit(BLRUDP_ACTIVE_CONTEXT *pContext);

void BlrudpStClosingRecv(BLRUDP_ACTIVE_CONTEXT *pContext,
                           BLRUDP_IO_PKT *pPkt);
void BlrudpStClosingWTimer(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStClosingCTimer(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStClosingCTimeout(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStClosingRTimer(BLRUDP_ACTIVE_CONTEXT *pContext);

void BlrudpStZombieXmit(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStZombieRecv(BLRUDP_ACTIVE_CONTEXT *pContext,
                           BLRUDP_IO_PKT *pPkt);
void BlrudpStZombieWTimer(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStZombieCTimer(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStZombieCTimeout(BLRUDP_ACTIVE_CONTEXT *pContext);
void BlrudpStZombieRTimer(BLRUDP_ACTIVE_CONTEXT *pContext);

#ifdef __cplusplus
};
#endif


#endif
