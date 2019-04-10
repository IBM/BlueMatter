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
#include <hostcfg.h>

#if defined(WIN32) || defined(CYGWIN)
#include <windows.h>
#endif

#include "stdio.h"


#if HAVE_STDINT_H
    #include <stdint.h>
#elif USE_GDB_STDINT_H
    #include "gdb_stdint.h"
#endif

#if  HAVE_UNISTD_H
    #include <unistd.h>
#endif

#if HAVE_FCNTL_H 
    #include <fcntl.h>
#endif

#if HAVE_SYS_IOCTL_H
    #include <sys/ioctl.h>
#endif

#if HAVE_STDLIB_H
    #include <stdlib.h>
#endif

#if HAVE_STRING_H
    #include <string.h>
#endif

#if HAVE_ASM_MSR_H
    #include <asm/msr.h>
#endif

#include "dbgtimelog.h"
#include "pmc.h"

#define SIZEOF_ARRAY(n) (sizeof(n)/sizeof(n[0]))

#ifndef TRUE
#define TRUE 1
#define FALSE 0 
#endif

#define FAIL goto Fail
#define FAILERR(nErr, nValue) {(nErr) = (nValue); goto Fail;}

uint64_t dbgTimeLogNow();
uint64_t dbgTimeLogFreq();

//
// formatting long long format for MS and GNU compilers.
//
#if defined(WIN32) || defined(CYGWIN)
    #define LL "I64"
#else
    #define LL "ll"
#endif

enum {PEF_COUNTER = 0};

//
// time is kept in its raw form until it is read
// out into the log file....
//

//
// timer entry structure.  One of these for each timer...
//
typedef struct {
    uint64_t llStartTime;       // reference time this timer was started.....
    uint64_t llClumTime;        // cumulative time
    uint64_t llNumTimes;        // number of times this timer was started.
    uint32_t ulTimerStarted;    // timer started flag.. used for nested timers...
    char     szTitle[100];      // space for an optional title...
} DBG_TIMER_ENTRY;              

static uint32_t g_ulNumTimers = 0;
static DBG_TIMER_ENTRY *g_pTimers = NULL;     // pointer to array of timers....

////////////////////////////////////////////////////////////////
int dbgTimeLogOpen(unsigned int nNumTimers)
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
{
    int nErr = DTLOG_ERR_INTERNAL_ERR;
    if (g_pTimers)
        FAILERR(nErr, DTLOG_ERR_ALREADY_OPEN);

    if (nNumTimers == 0)
        FAILERR(nErr, DTLOG_ERR_PARAM_ERR);

    #if  (defined(LINUX) && (!HAVE_ASM_MSR_H))
    {
        int fd; /* file description for ioctl */
        int nRet;
        //
        // Open the device so we can talk to it.  Clearing the counters
        // is a privilged instruction so it has to be done by a driver..
        //
        fd  = open("/dev/pmc", O_RDONLY, 0666);

        if (fd < 0) 
            FAILERR(nErr, DTLOG_ERR_DRIVER_ERR);

        //
        // these IOCTLS clear the counters out... and programs the counters
        // for what we wish to count....
        //
        nRet = ioctl(fd,                 // file descripter.
                     PEF_COUNTER,        // counter number (0/1)
                     (CPU_CLK_UNHALTED | PMC_USER_MASK | // what we are counting + enables
                      PMC_OS_MASK | PMC_ENABLE_MASK)); 
        close(fd);
        if (nRet != 0)
            FAILERR(nErr, DTLOG_ERR_DRIVER_ERR);
    }
    #endif    

    g_pTimers = malloc(sizeof(*g_pTimers) * nNumTimers);
    if (g_pTimers == NULL)
        FAILERR(nErr, DTLOG_ERR_NO_MEMORY);
    memset(g_pTimers, 0, sizeof(*g_pTimers) * nNumTimers);
    g_ulNumTimers = nNumTimers;
    return(DTLOG_ERR_SUCCESS);
Fail:
    dbgTimeLogClose();      // close it down if it was opened...
    return(nErr);
}


///////////////////////////////////////////////////////////////
void dbgTimeLogClose()
//
// free any resources used by the dbgTimeLogOpen.
//
// inputs/outputs:
//     none.
////////////////////////////////////////////////////////////////
{
    if (g_pTimers)
        free(g_pTimers);
    g_ulNumTimers = 0;
}


///////////////////////////////////////////////////////////////
void dbgTimeLogStart(unsigned int nTimer, const char *pszTitle)
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
{
    DBG_TIMER_ENTRY  *pTimer;
    if ((g_pTimers == NULL) || (nTimer >= g_ulNumTimers))
        FAIL;

    //
    // this variation does not count recursions, but keeps
    // crediting time to the function until its outer most
    // entry exits...
    pTimer = &g_pTimers[nTimer];
    if ((pTimer->ulTimerStarted++) == 0)        // test, then increment...
    {
        if ((pTimer->szTitle[0] == 0) && (pszTitle))
            strncpy(pTimer->szTitle, pszTitle, sizeof(pTimer->szTitle)-1);
        pTimer->llStartTime = dbgTimeLogNow();     // sample the time...
    }

    return;
Fail:
    return;
}


///////////////////////////////////////////////////////////////
void dbgTimeLogStop(unsigned int nTimer)
//
// Stop the specified timer.
//
// inputs:
//    nTimer -- timer to stop.
// outputs:
//    none.
//
///////////////////////////////////////////////////////////////
{
    DBG_TIMER_ENTRY  *pTimer;
    if ((g_pTimers == NULL) || (nTimer >= g_ulNumTimers))
        FAIL;
    
    pTimer = &g_pTimers[nTimer];
    if (pTimer->ulTimerStarted)
    {
        pTimer->ulTimerStarted--;
        if (pTimer->ulTimerStarted == 0)        // counted all the way down..
        {
            uint64_t llDelta;
            pTimer->llNumTimes++;
            llDelta = dbgTimeLogNow() - pTimer->llStartTime;
            if ((llDelta & ((uint64_t)1<<63)) == 0)    // guard against time going backwards...
                pTimer->llClumTime += llDelta;
        }
    }
    return;
Fail:
    return;
}

///////////////////////////////////////////////////////////////
int dbgTimeLogWrite(const char *pszFileName)
//
// write the time log out to the file name specified...
//
// inputs:
//    pszFileName -- file name of the file to write.
// outputs:
//    returns -- DTLOG_ERR_SUCCESS if successful....
//
///////////////////////////////////////////////////////////////
{
    int nErr = DTLOG_ERR_INTERNAL_ERR;
    uint32_t n;
    double fTime;
    uint64_t llFreq;
    DBG_TIMER_ENTRY  *pTimer;
    FILE *pFile = NULL;
    int nCount = 0;

    if (g_pTimers == NULL)
        FAILERR(nErr, DTLOG_ERR_NOT_OPEN);


    if (pszFileName == NULL)
        FAILERR(nErr, DTLOG_ERR_PARAM_ERR);
    pFile = fopen(pszFileName,"w");     // open output file for writing....
    if (pFile == NULL)
        FAILERR(nErr, DTLOG_ERR_WRITE_ERR);

    llFreq = dbgTimeLogFreq();


    for (n = 0, pTimer = g_pTimers; 
         n < g_ulNumTimers; 
         n++, pTimer++)
    {
        //
        // only print anything IF there were any timers actually used...
        //
        if (pTimer->llNumTimes)        
        {

            double fAvgTime;
            char *pszHeader = 
                "Tmr#:            Count      Total(ms)        Avg(ms)        Title \n"
                "------------------------------------------------------------------\n";
            char *pszFmt = "Timer[%3d]: %10" LL "d %12.3f %15.5f : %s\n";
            int64_t llTime = pTimer->llClumTime;

            if (nCount++ == 0)       // first one, print the headers..
            {
                printf(pszHeader);
                fprintf(pFile,
                        pszHeader);
            }
            fTime = (double)(llTime);
            fTime = (fTime * 1000)/ (int64_t)llFreq;     // convert to milliseconds...
            if (pTimer->llNumTimes)        
                fAvgTime = fTime / (int64_t)pTimer->llNumTimes;
            else
                fAvgTime = 0;
            printf(pszFmt,
                   n,
                   pTimer->llNumTimes,
                   fTime,
                   fAvgTime,
                   pTimer->szTitle);
            fprintf(pFile, 
                   pszFmt,
                   n,
                   pTimer->llNumTimes,
                   fTime,
                   fAvgTime,
                   pTimer->szTitle);
        }   // if (pTimer->llNumTimes...
    }
    fclose(pFile);
    return(DTLOG_ERR_SUCCESS);
Fail:
    if (pFile)
        fclose(pFile);
    return(nErr);
}

/////////////////////////////////////////////////////////////////
uint64_t dbgTimeLogNow()
//
// retrieve the time right now.
//
// inputs:
//    none.
// outputs:
//    returns -- time in native format (usually a cpu cycle counter).
//
{
    uint64_t llNow = 0L;
    if (g_pTimers == NULL)
        return(0);
    #if defined(WIN32) || defined(CYGWIN)
        {
        LARGE_INTEGER llPerfCounter;

        //
        // At 1Ghz clock this performance counter is good for 586 years.
        // I won't be around when this times out.  
        //
        int bRet;
        bRet = QueryPerformanceCounter(&llPerfCounter);
        if (bRet)
            llNow = llPerfCounter.QuadPart;
        }
    #endif

    #if  defined(LINUX)
    #if HAVE_ASM_MSR_H
    {
        rdtscll(llNow);
    }
    #else
    
        {
            uint32_t eax;
            uint32_t edx;
            int nCounter = PEF_COUNTER;
    
            //
            // read the performance counter register.  
            __asm("mov %2, %%ecx\n\t"
                "rdpmc\n\t" 
                "mov %%eax, %0\n\t"
                "and $255, %%edx\n\t"
                "mov %%edx, %1\n\t"
                : "=m" (eax), "=m" (edx), "=m" (nCounter)
                : /* no inputs */
                :  "eax", "ecx", "edx"); /* eax, ecx, edx clobbered */
            llNow = ((uint64_t) edx << 32)  | eax;
        }
    #endif
    #endif
    return(llNow);
}
/////////////////////////////////////////////////////////////////
uint64_t dbgTimeLogFreq()
//
// Retrieve the timer frequency of the native timer supplied
// by dbgTimeLogNow...  Frequency is in cycles per second....
//
// inputs:
//    none.
// outputs:
//    returns -- timer frequency..
//
{
    uint64_t llLogFreq = 1;
    if (g_pTimers == NULL)
        return(1);
    #if defined(WIN32) || defined(CYGWIN)
    {
        LARGE_INTEGER llFrequency;
        int bRet;
        bRet = QueryPerformanceFrequency(&llFrequency);

        if (bRet)
            llLogFreq = llFrequency.QuadPart;
    }
    #endif

    #if  defined(LINUX)
    {
        FILE *pFile;
        llLogFreq = 600000000L;     // default to 600Mhz if all else fails...

        //
        // This opens the /proc/cpuinfo psuedo file and reads
        // out the information there...
        //
        // here is a snapshot of what the proc file looks like...
        // this is a VERY CRUDE parse....
        //
        //  processor   : 0
        //  vendor_id   : GenuineIntel
        //  cpu family  : 6
        //  model       : 8
        //  model name  : Pentium III (Coppermine)
        //  stepping    : 6
        //  cpu MHz     : 930.976   <<--- this is what we are after...
        //  cache size  : 256 KB
        //...   more to follow
        pFile = fopen("/proc/cpuinfo","r");
        if (pFile)          // file succesfully open
        {
            int n;          // process the contents...
            float fp;
            char sz[100];
            for (n = 0; n < 6; n++)         // skip the first 6 lines...    
                fgets(sz,  sizeof(sz), pFile);       
            fscanf(pFile, "%s %s : %f", sz, sz, &fp);
            llLogFreq = fp * 1000000;
            fclose(pFile);
        }
    }
    #endif
    return(llLogFreq);
}
