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
 // *************************************************************
// File: barrier.cpp
// Author: RSG
// Date: January 28, 2008
// Class Barrier
// Description: Encapsulates data and operations associated with a
//              barrier operation implemented using pthreads condition
//              variables for a known number of threads. The
//              assumption (for now) is that initialization is handled
//              by the initial thread.
// *************************************************************

#include <BlueMatter/barrier.hpp>

//#include <sys/atomic_op.h>

Barrier::Barrier( int nthreads ) : d_counter( nthreads )
{
  pthread_cond_init( &d_condition, NULL );
  pthread_mutex_init( &d_mutex, NULL );
}

Barrier::~Barrier()
{
  pthread_cond_destroy( &d_condition );
  pthread_mutex_destroy( &d_mutex );
}

void Barrier::operator()()
{
  pthread_mutex_lock( &d_mutex );
//  int remaining = fetch_and_add( &d_counter, -1 );
  int remaining= __atomic_add_fetch(&d_counter, -1, __ATOMIC_RELAXED) ;
  if ( remaining <= 1 ) // everyone has now entered the barrier
    {
      pthread_cond_broadcast( &d_condition ); // wake everyone
      pthread_mutex_unlock( &d_mutex );
      return;
    }
  pthread_cond_wait( &d_condition,  &d_mutex );
  pthread_mutex_unlock( &d_mutex );
}
