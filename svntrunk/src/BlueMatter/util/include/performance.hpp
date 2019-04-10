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
 
THIS FILE IS RETIRED - DO NOT INCLUDE - USE FXLOG FOR THIS FUNCTION

#if 0

#ifndef __PERFORMANCE_HPP__
#define __PERFORMANCE_HPP__

#include <sys/time.h>

/////#define DO_PERFORMANCE

#ifdef DO_PERFORMANCE

#define ReadTime(b, t) if (b) { read_real_time(&t, TIMEBASE_SZ); }
#define PrintTime(b, s, f, m) if(b)  { printTime(s, f, m); }
#define PrintForPerformance(b, m, i) if (b) { cerr << m << i << endl; }

#else

#define ReadTime(b, t)
#define PrintTime(b, s, f, m)
#define PrintForPerformance(b, m, i)

#endif



/*************************************************************************
* Parameters:  start and finishing times before adjustment, message
* describing what the times mean.
**************************************************************************/
void
printTime(timebasestruct_t& start, timebasestruct_t& finish, char* message)
{

  int secs, n_secs;

  time_base_to_time(&start, TIMEBASE_SZ);
  time_base_to_time(&finish, TIMEBASE_SZ);

  /* subtract the starting time from the ending time */
   secs = finish.tb_high - start.tb_high;
   n_secs = finish.tb_low - start.tb_low;

   /*
    * If there was a carry from low-order to high-order during
   * the measurement, we may have to undo it.
   */
   if (n_secs < 0)  {
     secs--;
     n_secs += 1000000000;
   }

   cerr << message << ":: " << secs << " secs, " << n_secs << " nanosecs" << endl;
}

#endif
#endif
