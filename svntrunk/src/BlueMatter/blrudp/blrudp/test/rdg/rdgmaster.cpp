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
 ///////////////////////////////////////////////////////////////
//
// Very simple test program using fork so show sending
// and receiving data using Blrudp.
//
// #Id: $
//
// $Log$
// Revision 1.6  2003/02/21 12:43:21  blake
// master arg 1 is slave program name
//
// Revision 1.5  2003/02/19 22:11:45  blake
// took out signal handler install call
//
// Revision 1.4  2003/02/19 22:09:51  blake
// clean up a little
//
// Revision 1.3  2003/02/19 21:57:42  blake
// rdgmaster works with rdgslave
//
// Revision 1.2  2003/02/19 21:41:59  blake
// main.cpp copied over rdgmaster.cpp
//
// Revision 1.18  2003/02/19 21:37:27  blake
// take the packet send limit from rdgslave.cpp
//
// Revision 1.17  2003/02/18 18:11:06  blake
// sending this to ralph
//
// Revision 1.16  2003/02/18 17:07:17  blake
// works by system() starting rdgslave - but haults after a 200k msgs
//
// Revision 1.15  2003/02/11 18:41:45  blake
// remove redundant line
//
//////////////////////////////////////////////////////////////////

#include <iostream.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <stddef.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#include <BlErrors.h>
#include <blrudp.h>

#include "rdg_rudp.hpp"

#define SIZEOF_ARRAY(n) (sizeof(n)/sizeof(n[0]))
#define BLMAKE_IPADDR(n1,n2,n3,n4) (htonl( ((n1)<<24) | ((n2)<<16) | ((n3)<<8) | (n4)))

int GetALocalIpAddr( unsigned long & );

//
// semaphore to handle wheter we are in the termination handler.
//
static int g_bInTermHandler = false;

void SetSignalHandlers();

//////////////////////////////////////////////////////////////////////////////
void ExitHandler(void);
//
// We have to add signal handlers to the code to make sure that we don't leave
// any sockets open when a program aborts, gets killed, asserts or the debugger
// exits.
//
// SIGABRT -- happens on assert.
// SIGTERM -- happens on kill
//
typedef void (* PFN_SIGNAL_HANDLER)(int nSignal);
PFN_SIGNAL_HANDLER g_pfnNextTermHandler = NULL;
PFN_SIGNAL_HANDLER g_pfnNextAbrtHandler = NULL;
PFN_SIGNAL_HANDLER g_pfnNextIntHandler = NULL;

bool g_bKillMe = false;

void SigTermHandler(int nSigNum);

///////////////////////////////////////////////////////////////////////
//
// Handle all the processing for the parent fork.
//
// inputs:
//    nSlavePid -- process id of the child process.
// outputs:
//    returns -- exit code.
//
int
SetupMaster(BLRUDP_HANDLE  & hMasterHandle,
            unsigned long  & aListenIpAddr,
            unsigned short & aListenIpPort )
  {
  int nRet;
  int nErr = BLERR_INTERNAL_ERR;
  unsigned long  ListenIpAddr;
  unsigned short ListenIpPort;
  unsigned long  ulDestAddr;
  unsigned short nDestPort;
  //BLRUDP_HANDLE hMasterHandle = NULL;
  BLRUDP_HCONTEXT hListenContext = NULL;
  BLRUDP_OPTIONS Options[1];

  cerr << "SetupMaster entered." << endl;

  nRet = BlrudpNew( &hMasterHandle ,           //*phBlrudp
                    0,                   // nNumOptions,
                    NULL);               // pOptions[]

  if( nRet != BLERR_SUCCESS )
    {
    FAILERR(nErr, nRet);
    }

  cerr << "SetupMaster(): "
       << "calling BlrudpListen "
       << endl;
  //
  // window size in bytes.
  //
  Options[0].nOptionId =  BLRUDP_WINDOW_SIZE;
  Options[0].v.ulValue =  8*1024;                 // our window size..

  unsigned long LocalIpAddr;

  nRet = GetALocalIpAddr( LocalIpAddr );
  if( nRet != 0 )
    FAIL;

  nRet = BlrudpListen(hMasterHandle ,          // hBlrudp
                      &hListenContext,            // phContext
                      LocalIpAddr ,               // BLFILEIO_RECV_ADDR,         // ulIpAddr
                      0,                          // BLFILEIO_RECV_PORT,         // nPort
                      SIZEOF_ARRAY(Options),      // numoptions
                      Options,                    // options[]
                      Callback_ListenConnectReq,   // pfnListenConnectReq
                      (void *)"Master");                  // pUserData

  if( nRet != BLERR_SUCCESS )
    {
    cerr << "SetupMaster() : failed in BlrudpListen.  nRet = " << nRet << endl;
    FAILERR(nErr, nRet);
    }

  nRet = BlrudpGetContextAddr(  hListenContext,
                               &ListenIpAddr,
                               &ListenIpPort,
                               &ulDestAddr,
                               &nDestPort);

  if( nRet != BLERR_SUCCESS )
    {
    FAILERR(nErr, nRet);
    }

  aListenIpAddr = ListenIpAddr;
  aListenIpPort = ListenIpPort;

  cerr << "Master BlrdupGetContexAddr() returns : "
       << " ListenIpAddr "  << (void *)  ListenIpAddr
       << " ListenIpPort  " <<   ListenIpPort
       << " ulDestAddr"     << (void *)  ulDestAddr
       << " nDestPort "     <<   nDestPort
       << endl;


  cerr << "SetupMaster() returning" << endl;

  return(0);

  Fail:
    if( hMasterHandle )
      {
      if( hListenContext )
         BlrudpUnlisten(hListenContext);
      BlrudpDelete(hMasterHandle);
      }

  cerr << "SetupMaster : Returning Failed nErr = " << nErr << endl;
  return(nErr);
  }


///////////////////////////////////////////////////////////////////////
//
int RunMaster(pid_t nSlavePid, BLRUDP_HANDLE & hMasterHandle)
  {
  int nErr = BLERR_INTERNAL_ERR;
  int nRet = 0;
  int nSlaveStatus = 0;


  cerr << "RunMaster : Entered - starting read loop " << endl;

  while( ! g_bKillMe )
    {
    BlrudpRun(hMasterHandle,          // hBlrudp
              true);            // bSleep);
    }

  cerr << "RunMaster : Exited recv loop : while(1){ BlrudpRun() }" << endl;

  ///////////////  BlrudpUnlisten( hListenContext );
  BlrudpDelete( hMasterHandle );

  //  waitpid(nSlavePid, &nSlaveStatus, 0);

  cerr << "RunMaster : Returning 0" << endl;

  return(0);

  Fail:
    if( hMasterHandle)
      {
      // In the RunMaster routine we don't know anything about hListenContext
      ////if( hListenContext )
      ////   BlrudpUnlisten(hListenContext);
      BlrudpDelete(hMasterHandle);
      }

  cerr << "RunMaster : Returning Failed nErr = " << nErr << endl;
  return(nErr);

  }


int main (int argc, char ** argv, char ** envp)
  {
  pid_t nProcId;
  int nRet;
  char myhostname[257];

  //SetSignalHandlers();

  cerr << argv[0] << " main() running" << endl;

  BLRUDP_HANDLE hMasterHandle;
  unsigned long  ListenIpAddr = 0;
  unsigned short ListenIpPort = 0;

  nRet = SetupMaster( hMasterHandle, ListenIpAddr, ListenIpPort );

  cerr
    << argv[0]
    << " SetupMaster() done - Listening on Addr " << (void *)ListenIpAddr
    << " Port " << ListenIpPort
    << endl;

  char cmd[1024];

  if( argc != 2 )
    {
    cerr
      << argv[0]
      << "usage: "
      << argv[0]
      << " <rdgslave_pathname> "
      << endl;
    exit( -1 );
    }

  sprintf( cmd, "%s %u %u 2>rdgslave.err >rdgslave.out &", argv[1], ListenIpAddr, ListenIpPort );

  cerr
    << argv[0]
    << " system( " << cmd << " ) "
    << endl;


  system( cmd );

  sleep(1);

  nRet = RunMaster(nProcId, hMasterHandle );

  exit(nRet);
  Fail:
    exit(-1);
  }

/////////////////////////////////////////////////////////////
// Handler for dealing with exiting an application.
// call BlrudpShutdown within this routine.
//
//
void ExitHandler(void)
  {
  if (g_bInTermHandler)           // just punt if we are in here.
          return;
  g_bInTermHandler = true;
  BlrudpShutdown();               // shut us down....

  // by by....

  }

//////////////////////////////////////////////////////////////////
//
// Termination signal handle....
//
// THis handles the following signals:
//
// SIGABRT -- abort.
// SIGTERM -- terminate
//
// it calls the Blrudp shutdown routine
void SigTermHandler(int nSigNum)
  {

  PFN_SIGNAL_HANDLER pfnTermHandler = NULL;

  if( g_bInTermHandler )           // just punt if we are in here.
    {
    return;
    }

  g_bInTermHandler = true;
  BlrudpShutdown();               // shut us down....
  g_bInTermHandler = false;

  switch (nSigNum)
    {
    case SIGABRT:
      {
      pfnTermHandler = g_pfnNextAbrtHandler;
      break;
      }
    case SIGTERM:
      {
      pfnTermHandler = g_pfnNextTermHandler;
      break;
      }
    }

  if (pfnTermHandler)
    {
    (*pfnTermHandler)(nSigNum);
    }
  else
    {
    exit(0);
    }
  }

/////////////////////////////////////////////////////////////////////////
//
// Setup the termination signal handlers so we can signal
// the bluelight protocol that we want to terminate and properly
// close all things down....
//
// inputs/outputs:
//    none.
//
void SetSignalHandlers()
  {
  //
  // this handles the debugger quitting on us... since it closes the stdin pipe.
  //
  //
  g_pfnNextTermHandler = signal(SIGTERM,          // sig
                            SigTermHandler);  // handler
  if (g_pfnNextTermHandler == SIG_ERR)
      g_pfnNextTermHandler = NULL;

  g_pfnNextAbrtHandler = signal(SIGABRT,          // sig
                            SigTermHandler);  // handler
  if (g_pfnNextAbrtHandler == SIG_ERR)
      g_pfnNextAbrtHandler = NULL;

  g_pfnNextIntHandler = signal(SIGINT,          // sig (CTRL-C)
                            SigTermHandler);  // handler
  if (g_pfnNextIntHandler == SIG_ERR)
      g_pfnNextIntHandler = NULL;

  // add an exit handler too.
  atexit(ExitHandler);

  }

