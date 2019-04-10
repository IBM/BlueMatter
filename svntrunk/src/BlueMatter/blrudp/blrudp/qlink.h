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
 #ifndef _QLINK_H_
#define _QLINK_H_

///////////////////////////////////////////////////////////////////////
// QLink -- //
//
// This file contains inline functions which implement linked list
// functionality.
//


// QLink usage notes:
// - These are doubly-linked circular queues.
// - Elements must be DeQ'd before EnQ'ing elsewhere.
// - Next/Prev are Null only when pkt is on no queue.
// - If the QLink is a queue head, it is empty when
//     Next/Prev both point to itself.
// - We use the address of the Qlink and the address
//     of the element interchangeably. So any queueable
//     structure must have this struct at offset zero.
// - EnQ's add to tail (prev), DeQ's remove from head (next).
// - If we wanted to randomize the ordering, we could
//     just hop a prime number of queue elements before
//     insertion.
//
// ** There is an assumption in the network interface that
// **  the same pkt (head) will be retried until complete,
// **  so only EnQ to Network Q's tail!
//

#include "dbgPrint.h"

#ifdef __cplusplus
extern "C" {
#endif

//#include <assert.h>
#include <stddef.h>
//
#if HAVE_STDINT_H
    #include <stdint.h>
#elif USE_GDB_STDINT_H
    #include "gdb_stdint.h"
#endif


#ifndef INLINE
#if defined(_MSC_VER)
    #define INLINE static
#else
    #if __GNUC__ >= 2
		#define INLINE static inline
    #else
		#define INLINE 
    #endif
#endif
#endif

typedef struct T_QLink
{
    struct T_QLink *next;
    struct T_QLink *prev;
} _QLink;


//
// The MS compiler needs the prototype declarations..
//
#if defined(_MSC_VER)
	void QInit( _QLink *q );
	int QisEmpty( _QLink *q );
	int QisFull( _QLink *q );
	void EnQ( _QLink *i, _QLink *q );
	void DeQ( _QLink *i );
    _QLink *QEnd(_QLink *q);
    _QLink *QBegin(_QLink *q);
#endif

INLINE void QInit( _QLink *q )
  {
  q->next = q;
  q->prev = q;
  }

INLINE int QisEmpty( _QLink *q )
  {
  return( q == q->next );
  }

INLINE int QisFull( _QLink *q ) // really just not empty
  {
  return( q != q->next );
  }

INLINE void EnQ( _QLink *i, _QLink *q )
  {
  ASSERT( (i->next == NULL) && (i->prev == NULL) );

  i->next       = q;
  i->prev       = q->prev;
  q->prev->next = i;
  q->prev       = i;
  }

INLINE void DeQ( _QLink *i )
  {
  // interesting: if we are dequeuing the last item on
  //   the list, i->prev->next loops back to old twice.
  i->prev->next = i->next;
  i->next->prev = i->prev;
  i->next       = NULL;
  i->prev       = NULL;
  }

//
// return the end pointer of the queue.  If the ->next pointer points
// to this then it is the last element of the queue...
// QEnd(..)->prev == the last element in the queue...
//
INLINE _QLink *QEnd(_QLink *q)
  {
  return(q);
  }

//
// Return the pointer to the first element of the queue.
// if the queue is empty this will be equal to QEnd...
//
INLINE _QLink *QBegin(_QLink *q)
  {
  return(q->next);
  }



#ifdef __cplusplus
};
#endif


#endif
