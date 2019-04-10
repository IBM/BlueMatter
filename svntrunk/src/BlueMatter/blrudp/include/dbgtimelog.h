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
 #ifndef _DBG_TIMELOG_H_
#define _DBG_TIMELOG_H_
////////////////////////////////////////////////////////////
//
// This file contains a simple debug time log faciliy
// which is available under linux and windows to
// be able to do accurate low-latency point to point 
// timeing measurements.
//
// This specific version of this is rather crude, being
// based on simple array of timer states, but should
// be effective in being able to get detailed timing
// data in an instrumented program.
//
// To use the time log the first call that must be made
// is dbgTimeLogOpen(n), which will open the time log and
// allocate and initialize the memory needed to perform 
// the time logging.  On linux this call will also enable
// access to the intel high performance timers.
//
// When the application closes it should call dbgTimeLogClose()
// to free any resources used by the time log.
//
// During operation any one of 'n' timers can be started and stopped.  
// As each timer is started, a counter is incremented.  When the timer
// is stopped the time delta between the start and the stop is added 
// to the cumulative timer.  This way, at the end of the run, 
// an average time can be computed.
//
// The call dbgTimeLogWrite(xxxx) will write the time log out to a file
// in text for for post mortum analisis....
//
//

#ifdef __cplusplus
extern "C" {
#endif

enum {
    DTLOG_ERR_SUCCESS       =  0,
    DTLOG_ERR_NO_MEMORY     = -1,
    DTLOG_ERR_PARAM_ERR     = -2,
    DTLOG_ERR_DRIVER_ERR    = -3,
    DTLOG_ERR_WRITE_ERR     = -4,
    DTLOG_ERR_INTERNAL_ERR  = -5,
    DTLOG_ERR_UNIMPLEMENTED = -6,
    DTLOG_ERR_ALREADY_OPEN  = -7,
    DTLOG_ERR_NOT_OPEN      = -8
};

////////////////////////////////////////////////////////////////
int dbgTimeLogOpen(unsigned int nNumTimers);
//
// Open the dbg time log subsystem and allocate space for nNumTimers
// number of timers.
//
// inputs:
//    nNumTimers -- total number of timers to provide for.
// outputs:
//    returns = DTLOG_ERR_SUCCESS i fsuccessful.....
//
////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
void dbgTimeLogClose();
//
// free any resources used by the dbgTimeLogOpen.
//
// inputs/outputs:
//     none.
////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
void dbgTimeLogStart(unsigned int nTimer, const char *pszTitle);
//
// Start the specified timer.  If the index is out of range
// this call is ignored...
//
// inputs:
//    nTimer -- timer number to start.
//    pszTitle -- optional title copied the first time we see one...
//
// outputs:
//    none.
//
//////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void dbgTimeLogStop(unsigned int nTimer);
//
// Stop the specified timer.
//
// inputs:
//    nTimer -- timer to stop.
// outputs:
//    none.
//
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
int dbgTimeLogWrite(const char *pszFileName);
//
// write the time log out to the file name specified...
//
// inputs:
//    pszFileName -- file name of the file to write.
// outputs:
//    returns -- DTLOG_ERR_SUCCESS if successful....
//
///////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
uint64_t dbgTimeLogNow();
//
// retrieve the time right now.
//
// inputs:
//    none.
// outputs:
//    returns -- time in native format (usually a cpu cycle counter).
//
//////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
uint64_t dbgTimeLogFreq();
//
// Retrieve the timer frequency of the native timer supplied
// by dbgTimeLogNow...  Frequency is in cycles per second....
//
// inputs:
//    none.
// outputs:
//    returns -- timer frequency..
//
///////////////////////////////////////////////////////////////

#ifdef __cplusplus
};
#endif

#endif

