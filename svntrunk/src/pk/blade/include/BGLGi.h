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
 
/*********************************************************************/
/*                        I.B.M. CONFIDENTIAL                        */
/*      (c) COPYRIGHT IBM CORP. 2001, 2003 ALL RIGHTS RESERVED.      */
/*                                                                   */
/*                LICENSED MATERIALS-PROPERTY OF IBM.                */
/*                                                                   */
/*********************************************************************/


/**********************************************************
 *
 * Filename:    BGLGi.h
 *
 * Originator:  Jose R. Brunheroto
 *
 * Purpose: Implement Global Interrupt devices interface.
 *
 ***********************************************************/

#ifndef _BGLGI_H
#define _BGLGI_H


#include "bglmemmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
 *     Global Interrupt Low-Level Interface                                  *
 *---------------------------------------------------------------------------*/

/* Channel specifiers.  Use these to construct channel vectors to specify
 * multiple channels in one operation.
 */
#define BGLGI_CHANNELS 4
#define BGLGI_CHx(n) (1<<(3-(n)))	/* use n=0..3 only! */

/* Base addresses that should work on CNK and the microloader
 * On Linux use these with ioremap().  Use BGL_MEM_GI_SYS_RANGE for the size.
 */
#define BGLGI_USR_BASE            ((void *)BGL_MEM_GI_USR_BASE)
#define BGLGI_SYS_BASE            ((void *)BGL_MEM_GI_SYS_BASE)

/* Sticky ARM type */
#define BGLGI_ARM_TYPE_OR	0
#define BGLGI_ARM_TYPE_AND	1

/* Information returned by status reads.  Note that userEnables returns the
 * uptree state in diagnostic read mode.
 */
typedef struct {
    unsigned mode:4;          /* Mode 0-3 (1=sticky, 0=direct) */
    unsigned stickyInt:4;     /* Sticky mode interrupt 0-3 (1=active) */
    unsigned stickyActive:4;  /* Sticky mode in progress 0-3 (1=true) */
    unsigned stickyType:4;    /* Sticky mode type 0-3 (0=OR, 1=AND) */
    unsigned readMode:1;      /* Read mode (0=normal, 1=diagnostic) */
    unsigned p0addrErr:1;     /* Processor 0 address error (1=true) */
    unsigned p1addrErr:1;     /* Processor 1 address error (1=true) */
    unsigned parityErr:1;     /* Parity error (1=true) */
    unsigned giRecvState:4;   /* Receive state 0-3 ("downtree" state) */
    unsigned userEnables:4;   /* User-level enables 0-3 (1=enabled). */
                              /* Actual "uptree" state when readMode=1. */
    unsigned giSendState:4;   /* Send state 0-3 ("uptree" state) */
} BGLGI_Status;

#define _BGL_GI_PRIV_CHANNEL_START   (2)    // Reserve channels 2-3 for the OS



/*---------------------------------------------------------------------------*
 * Low-level Functions
 *
 * These take a device base address.
 * The base addr is BGLGI_SYS_BASE, BGLGI_USR_BASE, or some remapped address
 * as required in Linux.
 *---------------------------------------------------------------------------*/

/** \brief Set state of a global interrupt channel
 *  Note that this also unsets "sticky" mode for the channel.
 *
 * \param base		Base addr of hardware (e.g. BGLGI_USR_BASE)
 * \param channel	Channel to use (e.g. 0..3 -- NOT BGLGI_CHx!)
 * \param state		State to set (0 or 1)
 */
static inline void BGLGI_SetState(void *base, int channel, int state)
{
    *((volatile unsigned int *)((char *)base + (channel << 4))) = state;
}

/** \brief Get status of global interrupts
 *
 * \param base	Base addr of hardware (e.g. BGLGI_USR_BASE)
 */
static inline BGLGI_Status BGLGI_GetStatus(void *base)
{
#if 1
    /* This obfuscation is for C++.  Sigh. */
    unsigned int status = *((volatile unsigned int *)base);
    return *((BGLGI_Status *)&status);
#else
    return *((volatile BGLGI_Status *)base);
#endif
}

/** \brief Return true (non-zero) if channel's receive state is raised
 *
 * \param status	Status captured from BGLGiStatus()
 * \param channel	Channel to test (e.g. 0..3 -- NOT BGLGI_CHx!)
 */
static inline int BGLGI_IsRaised(BGLGI_Status status, int channel)
{
    return (status.giRecvState & BGLGI_CHx(channel)) != 0;
}

/** \brief Set sticky arm type for a given channel.
 *
 * \param base		Base addr of hardware (e.g. BGLGI_USR_BASE)
 * \param channel	Channel to use (e.g. 0..3 -- NOT BGLGI_CHx!)
 * \param armtype	Type to set (0=OR, 1=AND)
 */
static inline void BGLGI_SetARMType(void *base, int channel, int armtype)
{
    *((volatile unsigned int *)((char *)base + ((channel+0xc) << 4))) = armtype;
}

/** \brief Disarm a sticky global interrupt
 *
 * \param base		Base addr of hardware (e.g. BGLGI_USR_BASE)
 * \param channel	Channel to disarm (e.g. 0..3 -- NOT BGLGI_CHx!)
 */
static inline void BGLGI_Disarm(void *base, int channel)
{
    *((volatile unsigned int *)((char *)base + ((channel+8) << 4))) = 0;
}

/** \brief Fire a sticky global interrupt
 *
 * \param base		Base addr of hardware (e.g. BGLGI_USR_BASE)
 * \param channel	Channel to fire (e.g. 0..3 -- NOT BGLGI_CHx!)
 */
static inline void BGLGI_Fire(void *base, int channel)
{
    *((volatile unsigned int *)((char *)base + ((channel+8) << 4))) = 1;
}

/** \brief Return true (non-zero) if channel's sticky state is raised
 *
 * \param status	Status captured from BGLGiStatus()
 * \param channel	Channel to test (e.g. 0..3 -- NOT BGLGI_CHx!)
 */
static inline int BGLGI_IsStickyRaised(BGLGI_Status status, int channel)
{
    return (status.stickyInt & BGLGI_CHx(channel)) != 0;
}


/* This is a callback func that is called repeatedly
 * during the barrier when waiting for a state change.
 * Return non-zero to forcibly break the barrier (which will
 * then return this value).
 */
typedef int (*BGLGI_WaitProc)(void *arg);


/** \brief Barrier using a given channel that calls a waitproc while idle
 *
 * Upon entry the given channel must expected be raised.  If not,
 * the barrier will wait for it to raise.  It is assumed a low channel is
 * from a previous barrier which is returning to idle state.
 *
 * This is the general case barrier.  While the barrier is waiting for a state
 * change in the given global interrupt channel, it repeatedly calls the
 * optional waitproc(waitarg).
 *
 * The waitproc must return an integer value.  A return of zero means the
 * barrier should continue.  A non-zero value will break the barrier and the
 * barrier function itself will return this same non-zero value (application
 * defined).  This is useful for processing timeouts, etc.
 *
 * A typical use with no wait processing would be:
 *	BGLGI_barrier(BGL_MEM_GI_USR_BASE, 0, NULL, NULL);
 *
 * A typical use with a timeout counting iterations would be:
 *	int timeoutproc(void *arg)
 *	{
 *	    int *countp = (int *)arg;
 *	    if (++(*countp) > TIMEOUT)
 *		return 1;
 *	    return 0;
 *	}
 *	int count = 0;
 *	int ret = BGLGI_barrier(BGL_MEM_GI_USR_BASE, 0, timeoutproc, &count);
 *	if (ret == 1)
 *		printf("barrier timeout!\n");
 *
 * \param base		Base addr of hardware (e.g. BGLGI_USR_BASE)
 * \param channel	Channel to use (e.g. 0..3 -- NOT BGLGI_CHx!)
 * \param waitproc	Function to call while waiting (may be NULL)
 * \param waitarg	Argument to pass to waitproc
 *
 * \returns 0 on success, else value of waitproc which breaks the barrier.
 */static inline int BGLGI_barrier(void *base,  int channel,
				BGLGI_WaitProc waitproc, void *waitarg)
{
    BGLGI_Status status;

    /* Wait for idle state. */
    for (;;) {
	status = BGLGI_GetStatus(base);
	if (BGLGI_IsRaised(status, channel))
	    break;
	/* NB: no need for a wait callback here.
	 * This is quick and independent of other nodes entering the barrier.
	 */
    }

    /* Fire global interrupt to show we are entering the barrier. */
    BGLGI_Disarm(base, channel);
    BGLGI_SetARMType(base, channel, BGLGI_ARM_TYPE_AND);
    BGLGI_Fire(base, channel);

    /* wait for interrupt to return */
    for (;;) {
	status = BGLGI_GetStatus(base);
	if (BGLGI_IsStickyRaised(status, channel))
	    break;
	if (waitproc) {
	    /* The waitproc may do something that allows other nodes to enter
	     * the barrier (e.g. MPI advance)
	     */
	    int ret = waitproc(waitarg);
	    if (ret)
		return ret;
	}
    }

    return 0;

}

/* This is provided for code compat with an older implementation. */
int BGLGI_Barrier(void *base,
		  int channelA, int channelB,
		  BGLGI_WaitProc waitproc, void *waitarg);

#ifdef __cplusplus
};
#endif

#endif
