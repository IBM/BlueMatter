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
// Revision 1.14  2003/02/11 18:35:06  blake
// works with two slaves forked - doesn't control them right though
//
// Revision 1.13  2003/02/11 18:28:43  blake
// minor changes - still works
//
// Revision 1.12  2003/02/11 18:16:16  blake
// minor changes - still functions
//
// Revision 1.11  2003/02/11 17:12:10  blake
// minor changes
//
// Revision 1.10  2003/02/11 04:21:19  blake
// clean up - still way ugly
//
// Revision 1.9  2003/02/11 04:16:37  blake
// sender gets dynamic ip addr finding call
//
// Revision 1.8  2003/02/11 04:04:53  blake
// change the child to use dynamic port
//
// Revision 1.7  2003/02/11 02:23:53  blake
// moving packets between dynamic ip addr/port
//
// Revision 1.6  2003/02/10 19:08:22  blake
// ralphs added BlrudpContextGetAddr() call seems to work
//
// Revision 1.5  2003/02/07 19:17:58  blake
// added call to BlrudpGetContextAddr() which might be what we need
//
// Revision 1.4  2003/02/06 16:49:36  blake
// minor formatting
//
// Revision 1.3  2003/02/06 15:57:02  blake
// switch from printf to cerr
//
// Revision 1.2  2003/02/06 15:38:37  blake
// reformatted c++ code - still works
//
// Revision 1.1  2003/02/06 15:01:31  blake
// *** empty log message ***
//
// Revision 1.1  2003/01/31 18:21:59  blake
// add interesting parts of blrudp
//
// Revision 1.1  2003/01/23 17:59:33  ralphbel
// empty
//
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

#define FAIL goto Fail
#define FAILERR(nErr, nValue) {(nErr) = (nValue); goto Fail;}


#define SIZEOF_ARRAY(n) (sizeof(n)/sizeof(n[0]))
#define BLMAKE_IPADDR(n1,n2,n3,n4) (htonl( ((n1)<<24) | ((n2)<<16) | ((n3)<<8) | (n4)))

//=================================================

int
GetALocalIpAddr( unsigned long & LocalIpAddr )
  {
  static struct sockaddr_in my_sock_addr;
  int     namelength;
  struct  hostent hostent_copy;               /* Storage for hostent data.  */
  struct  hostent *hostentptr;                 /* Pointer to hostent data.   */
  static  char hostname[256];
  int     nRet;


  /*
   * Get the local host name.
   */
  nRet = gethostname( hostname, sizeof hostname );
  if( nRet )
    {
    perror ("gethostname");
    exit(-1);
    }

  cerr << "gethostbyname() returned >" << hostname << "<" << endl;

  /*
   * Get pointer to network data structure for local host.
   */
  if( ( hostentptr = gethostbyname( hostname ) ) == NULL)
    {
    perror( "gethostbyname");
    exit(-1);
    }


  /*
   * Copy hostent data to safe storage.
   */
  hostent_copy = *hostentptr;


  /*
   * Fill in the address structure for socket 2.
   */
  my_sock_addr.sin_family = hostent_copy.h_addrtype;
  my_sock_addr.sin_port = htons( 0 );
  my_sock_addr.sin_addr = * ((struct in_addr *) hostent_copy.h_addr);

  cerr << "hostent_copy.h_addr " << (void *) hostent_copy.h_addr << endl;

  LocalIpAddr = * ((unsigned long *) hostent_copy.h_addr);

  return( 0 );
  }
//=================================================



int DoSlaveFork();
int DoMasterFork(pid_t nProcId);

const char *ContextStateString(int nState);
//
// Call back functsion where blrudp notifies us when something has happened.
//
void Callback_ListenConnectReq(BLRUDP_HCONTEXT /*hListenContext*/,
                             BLRUDP_HCONTEXT hNewContext,
                             unsigned long /*ulAddr*/,
                             unsigned short /*nPort */,
                             void *pUserData);

void Callback_ConnectContextChanged(BLRUDP_HCONTEXT hContext,
                                  int nOldState,
                                  int nNewState,
                                  int nBlError,
                                  void * pUserData);
void Callback_PacketReceived(BLRUDP_HCONTEXT hContext,
                        void * pUserData );

void Callback_ContextSendBufferDone(BLRUDP_HCONTEXT hContext,
                                  unsigned long ulSendBufferID,
                                  void * pUserData);

int SendPktBuffrerToContext(BLRUDP_HCONTEXT hContext, unsigned long &ulPktNum);

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
//////////////////////////////////////////////////////////////
void SigTermHandler(int nSigNum);

///////////////////////////////////////////////////////////////////////
//
// Handle all the processing for the child fork.
//
// inputs:
//    none.
// outputs:
//    returns -- exit code.
//
int XXXDoSlaveFork( unsigned long   aListenIpAddr,
                 unsigned short  aListenIpPort )
  {
  int                 nRet;
  int                 nErr = BLERR_INTERNAL_ERR;
  BLRUDP_HANDLE       hBlrudp = NULL;
  BLRUDP_HCONTEXT     hSenderContext = NULL;
  unsigned long       ulPktNum = 0;
  const unsigned long MAX_SEND_PACKETS = 10000;
  unsigned long  ulSrcAddr;
  unsigned short nSrcPort;
  unsigned long  ulDestAddr;
  unsigned short nDestPort;

  BLRUDP_OPTIONS      Options[1];
  cerr << "Hey this is the child fork" << endl;

  nRet = BlrudpNew(&hBlrudp ,           //*phBlrudp
                   0,                   // nNumOptions,
                   NULL);               // pOptions[]

  //
  // set up and attempt connect...
  // window size in bytes.
  //
  Options[0].nOptionId =  BLRUDP_WINDOW_SIZE;
  Options[0].v.ulValue =  0x8*1024;


  // The following few lines try to figure out a local IP address.
  // The trouble is, how do we know ALL local addresses will be connected to the listener?
  unsigned long LocalIpAddr;

  nRet = GetALocalIpAddr( LocalIpAddr );
  if( nRet != 0 )
    FAIL;

  nRet = BlrudpConnect(hBlrudp,                           // hBlrudp,
                       &hSenderContext,                    // *phContext,
                       LocalIpAddr,         // BLFILEIO_SEND_ADDR,                 // ulSrcAddr,
                       0,                   // BLFILEIO_SEND_PORT,                 // nSrcPort,
                       aListenIpAddr,       // BLFILEIO_RECV_ADDR,                 // ulDestAddr,
                       aListenIpPort,       // BLFILEIO_RECV_PORT,                 // nDestPort,
                       SIZEOF_ARRAY(Options),              // numoptions
                       Options,                            // options[]
                       Callback_ConnectContextChanged,      // pfnContextChanged,
                       Callback_PacketReceived,                // pfnContextRecv,
                       Callback_ContextSendBufferDone,      // pfnSendBufferDone,
                       (void *)"Slave");                   // *pUserData);

  if( nRet != BLERR_SUCCESS )
    {
    cerr << "Slave : Filed to Connect to the remote server" << endl;
    FAIL;
    }
    //
    // retrieve the address and ports used here.
  nRet = BlrudpGetContextAddr(  hSenderContext,
                               &ulSrcAddr,
                               &nSrcPort,
                               &ulDestAddr,
                               &nDestPort);

  if( nRet != BLERR_SUCCESS )
    {
    cerr << "Slave : Failed to get IpAddr " << endl;
    FAILERR(nErr, nRet);

    }

  cerr << "Slave : BlrdupGetContexAddr() returns : "
       << " ulSrcAddr " << (void *)  ulSrcAddr
       << " nSrcPort  " <<   nSrcPort
       << " ulDestAddr" << (void *)  ulDestAddr
       << " nDestPort " <<   nDestPort
       << endl;

  //
  // wait until we have sent enough, or
  // we just the other end disconnected us..
  //
  cerr << "Slave : Start sending packets " << endl;

  while( (!g_bKillMe) && (ulPktNum <= MAX_SEND_PACKETS) )
    {
    // stuff
    // the send buffer.
    //
    do {
       nRet = SendPktBuffrerToContext(hSenderContext,      // hContext,
                                       ulPktNum);           // &ulPktNum)

       } while ((nRet == BLERR_SUCCESS) && (ulPktNum <= MAX_SEND_PACKETS));

    if( (nRet != BLERR_SUCCESS) && (nRet != BLERR_BLRUDP_NO_SEND_BUFFERS))
      {
      cerr << "Send failed" << endl;
      FAILERR(nErr, nRet);
      }

    BlrudpRun(hBlrudp,          // hBlrudp
              true);            // bSleep);
    }

  cerr << "Slave : Done sending packets " << endl;

  //
  // perform a graceful disconnect.  I.e. wait until all packets
  // queued have been sent.
  //
  BlrudpDisconnect(hSenderContext,    // hContext,
                   false);            // bImmediate);

  BlrudpDelete(hBlrudp);

  cerr << "Slave : Returning 0 " << endl;

  return(0);

  Fail:
    if( hBlrudp )
      {
      BlrudpDelete(hBlrudp);
      if( hSenderContext )
        {
        BlrudpDisconnect(hSenderContext,    // hContext,
                         true);             // bImmediate);
        }
      }

  cerr << "Slave : Returning Failed nErr = " << nErr << endl;
  return(nErr);
  }

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

  printf("BlrudpMaster listen - ListenIpAddr(%lx), ListenIpPort(%d), ulDestAddr(%lx), nDestPort(%d)\n",
            ListenIpAddr,
            ListenIpPort,
            ulDestAddr,
            nDestPort);

  aListenIpAddr = ListenIpAddr;
  aListenIpPort = ListenIpPort;

  cerr << "Master BlrdupGetContexAddr() returns : "
       << " ListenIpAddr " << (void *)  ListenIpAddr
       << " ListenIpPort  " <<   ListenIpPort
       << " ulDestAddr" << (void *)  ulDestAddr
       << " nDestPort " <<   nDestPort
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

    ////nRet = waitpid(nSlavePid, &nSlaveStatus,  WNOHANG);
    ////
    ////if( nRet )               // did our only child quit.
    ////  {
    ////  break;
    ////  }
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

  SetSignalHandlers();

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

//  nProcId = fork();       // fork me.

//  if( nProcId > 0 ) // I'm master...
//    nProcId = fork();       // fork me.

//  if( nProcId < 0 )
//    {
//    cerr << argv[0] << " Fork failed " << endl;
//    FAIL;
//    }

//  if( nProcId > 0 ) // I'm master...
    {
    char cmd[1024];

    sprintf( cmd, "rdgslave.exe %u %u 2>rdgslave.err >rdgslave.out &", ListenIpAddr, ListenIpPort );

    cerr
      << argv[0]
      << " system( " << cmd << " ) "
      << endl;


    system( cmd );
    }
 // else
    {
    sleep(1);
    }

//  if( nProcId == 0 )       // are we the child.
//    {
//    nRet = DoSlaveFork( ListenIpAddr, ListenIpPort );
//    }
//  else
    {
    nRet = RunMaster(nProcId, hMasterHandle );
    }

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

////////////////////////////////////////////////////////////////
//
// Send a packet to the specified context.
//
// This function, first checks to see if it is clear to send.
// if so, it will attempt to send a packet to the specified context...
//
// inputs:
//   hContext -- context to send it to.
//   ulPktNum -- packet num to stuff into the memory...
// outputs:
//    ulPktNum++ -- if send was successful.
//    returns - BLERR_SUCCESS if successful.
//              BLERR_BLRUDP_NO_SEND_BUFFERS -- if we don't have buffers available
//              other errors if there was a problem sending or the context is no longer
//              open.
//
//
int SendPktBuffrerToContext(BLRUDP_HCONTEXT hContext,
                            unsigned long &ulPktNum)
  {
  int nErr = BLERR_INTERNAL_ERR;
  BLRUDP_HIOBUFFER hIoBuffer = NULL;
  unsigned char *pData;
  unsigned long ulBufferLen;
  int nRet;

  int nSendCount;
  hIoBuffer = NULL;

  //
  // check to see if there is space to send data.
  //
  nRet = BlrudpGetStatus(hContext,             // hContext,
                         NULL,                 // *pnState,
                         NULL,                 // *pnRecvCount,
                         &nSendCount);         // *pnSendCount);

  if( nRet != BLERR_SUCCESS)
    {
    FAIL;
    }

  if( nSendCount <= 0)            // this is a valid condition, wait until the rest is sent...
    {
      FAILERR(nErr, BLERR_BLRUDP_NO_SEND_BUFFERS);                     // nothing to do....
    }

  if( (ulPktNum % 1000) == 0)
    {
    cerr << "Send " << ulPktNum << endl;
    }
  //
  // We are going to send using the native buffer of Blrudp.
  // this avoids having to copy data.
  //
  nRet = BlrudpGetSendBuffer(hContext,          // hContext,
                             &hIoBuffer,        // *phIoBuffer,
                             &pData,            // **ppDataBuffer,
                             &ulBufferLen);     // *pulBufferLen);

  if( nRet != BLERR_SUCCESS )
    {
    cerr << "BlrudpGetSendBuffer Error = " << nRet << endl;      // should not really see this....

    if( nRet == BLERR_BLRUDP_NO_SEND_BUFFERS )
      {
      cerr << "noSendBuffers" << endl;              // we really should not see this if the status check worked
      }
    cerr << "BlrudpSendTo Failed Return Value = " << nRet << endl;
    FAILERR(nErr, nRet);
    }

  uint32_t *pMem;
  pMem = (uint32_t *)pData;
  *pMem = htonl(ulPktNum);                                        // tag the packet number.

  pData[ulBufferLen-1] = 0xFF;                                    // insert data....  // make sure we can tell the end..
  nRet = BlrudpBufferSendToContext(hContext,                      // hContext
                                    hIoBuffer,                    // hIoBuffer
                                    ulBufferLen,                  // nLen
                                    ulPktNum);                    // ulSendbufferID

  if (nRet != BLERR_SUCCESS)
    {
    cerr << "BlrudpSendTo Failed Return Value = " << nRet << endl;
    FAILERR(nErr, nRet);
    }

  hIoBuffer = NULL;       // successfully sent....

  ulPktNum++;         // next packet number.

  return(BLERR_SUCCESS);

  Fail:
    if (hIoBuffer)
        BlrudpReleaseSendBuffer(hIoBuffer);
    return(nErr);
  }

/////////////////////////////////////////////////////////////////////////
//
// Map a context state to a context state string.
//
// inputs:
//    nState -- context state to map.
// outputs:
//    returns -- pointer to context state string.
//
const char *ContextStateString(int nState)
  {
  const char *pszState;

  switch (nState)
    {
    case BLRUDP_STATE_CLOSED     : pszState = "CLOSED    "; break;
    case BLRUDP_STATE_LISTENING  : pszState = "LISTENING "; break;
    case BLRUDP_STATE_CONNECTING : pszState = "CONNECTING"; break;
    case BLRUDP_STATE_CONNECT    : pszState = "CONNECT   "; break;
    case BLRUDP_STATE_CLOSING    : pszState = "CLOSING   "; break;
    default: pszState = "????unknown????"; break;
    }
  return(pszState);
  }

/////////////////////////////////////////////////////////////////////
//
// Callback_ indicating the listening port has an incomming connection
// reqest that the client application has to either accept or reject.
//
// The callback either calls BlrudpAccept or BlrudpReject
// to accept or reject the conversation.
//
// If neither accept or reject is called on the context during this call
// then Reject is assumed.
//
// inputs:
//    hContext      -- listening context
//    hNewContext   -- new incomming context.
//    nAddr         -- remote address of the incomming connection.
//    nRemotePort   -- remote port of the incomming connection.
//    pUserData -- pointer to user data passed in when the callback was setup.
// outputs:
//              A BLRUDP_CONTEXT_CHANGED callback will be called
//              when a new Active context is created and placed into
//              the ...CONNECTING state.
//
void Callback_ListenConnectReq(BLRUDP_HCONTEXT hListenContext,
                             BLRUDP_HCONTEXT hNewContext,
                             unsigned long ulAddr,
                             unsigned short nPort ,
                             void * pUserData)
  {
  const char *pszMe;
  int nRet;

  pszMe = (const char *)pUserData;
  if (pszMe == NULL)
      pszMe = "Huh?";

  cerr <<"Callback_ListenConnectReq() " << pszMe
       << " hListenContext(" << hListenContext
       << "), hNewContext("  << hNewContext
       << "), ulAddr("       << (void *) ulAddr
       << "), nPort(" << (void*) nPort << ")" << endl;

  nRet = BlrudpAccept(hNewContext,                    // hContext
                      Callback_ConnectContextChanged,  // pfnContextChanged,
                      Callback_PacketReceived,            // pfnContextRecv,
                      Callback_ContextSendBufferDone,  // pfnSendBufferDone,
                      pUserData);                     // *pUserData);

  cerr << "Callback_ListenConnectReq() : Called BlrudpAccept() - nRet = " << nRet << endl;

  if (nRet != BLERR_SUCCESS)
      FAIL;
  return;
  Fail:
    return;
  }

//////////////////////////////////////////////////////////////////
//
// Callback_ indicating the state of the indicated context has changed.
//
// inputs:
//    nOldState -- the previous state of the context.
//    nNewState -- the new state of the context.
//    nBlError  -- bluelight error condition.
//    pUserData -- pointer to user data passed in when the callback was setup.
// outputs:
//    none.
//
// NOTES:
//    Most all state transitions will report Blerror == BLERR_SUCCESS however,
//    some transitions to the CLOSED state are due to error conditions and
//    these are reported by the nBlError variable.
//
//
void Callback_ConnectContextChanged(BLRUDP_HCONTEXT hContext,
                                  int              nOldState,
                                  int              nNewState,
                                  int              nBlError,
                                  void           * pUserData)
  {
  const char *pszMe;
  pszMe = (const char *)pUserData;
  if (pszMe == NULL)
      pszMe = "Huh?";

  cerr << "Callback_ConnectContextChanged(): " << pszMe << " : "
       << hContext
       << " ( "
       <<    ContextStateString( nOldState )
       << " -> "
       <<    ContextStateString(nNewState)
       << " ) Error Code = "
       <<    nBlError
       << endl;
  //
  // for this very simple test we are connecting one sender and one receiver.
  //
  // as soon as we receive a close we stop

  if ( nNewState == BLRUDP_STATE_CLOSED )
    {
    cerr << pszMe << " Callback_ConnectContextChanged() New State is CLOSED so set g_bKillMe" << endl;
    g_bKillMe = true;                   // set the semaphore that tells us to stop
    }
  }

////////////////////////////////////////////////////////////////
//
// Notification to the client that there are packets waiting to be received
// in the receive queue for the context hContext.
//
// inputs:
//    hContext -- context that has data to receive.
//    pUserData -- pointer to user data passed in when the callback was setup.
// outputs:
//    none.
//
void Callback_PacketReceived(BLRUDP_HCONTEXT hContext,
                         void * pUserData)
  {
  // cerr << "Callback_PacketReceived() entered" << endl;
  const char *pszMe;
  pszMe = (const char *)pUserData;
  if (pszMe == NULL)
      pszMe = "Huh?";

  while (true)
    {
    BLRUDP_HIOBUFFER hIoBuffer = NULL;
    unsigned char *pData = NULL;
    int nDataLen;
    int nRet;

    //
    // Get the data buffer pointer.
    //
    nRet = BlrudpRecvBufferFromContext(hContext,            // hContext
                                      &hIoBuffer,           // *phIoBuffer
                                      &pData,               // **ppData
                                      &nDataLen);           // *pDataLen
    if( nRet != BLERR_SUCCESS)
      {
      if( nRet != BLERR_BLRUDP_NO_RECVDATA)
        {
        cerr << "BlrudpRecvFromContext Failed - Return Value = " << nRet << endl;
        }
      break;
      }

    //
    // Now we would normally do something with the data.
    // but for example we will just ignore the received data...
    //
    uint32_t *pMem;
    uint32_t ulPktNum;
    pMem = (uint32_t *)pData;
    ulPktNum = ntohl(*pMem);

    if( (ulPktNum % 1000) == 0)
      {
      cerr << "Recv PktNum: " << ulPktNum << endl;
      }

    nRet = BlrudpReleaseRecvBuffer( hIoBuffer );

    if( nRet != BLERR_SUCCESS)
      {
      cerr << "BlrudpReleaseRecvBufferFailed Failed - Return Value =" << nRet << endl;
      break;
      }
    }
  // cerr << "Callback_PacketReceived() exited" << endl;
  }

//////////////////////////////////////////////////////////////////
//
// Callback_ indicating that a buffer sent by BlrudpSend...
// has been sent and acknowledged.  The buffer has actually been returned
// to the free pool of send buffers.
//
// inputs:
//    hContext -- context this buffer corresponds to.
//    ulSendBufferID -- identifier passed into the BlrudpSend.. function.
//    pUserData -- pointer to user data passed in when the callback was setup.
// outputs:
//    none.
//
void Callback_ContextSendBufferDone( BLRUDP_HCONTEXT /* hContext */,
                                    unsigned long   /* ulSendBufferID */,
                                    void *          /* pUserData */)
  {
  //  cerr << "Callback_ContextSendBufferDone() executed" << endl;
  }


