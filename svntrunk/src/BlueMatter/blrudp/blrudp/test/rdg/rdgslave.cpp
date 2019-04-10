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
 
#include <iostream.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <stddef.h>
#include <unistd.h>
#include <netinet/in.h>

#include <BlErrors.h>
#include <blrudp.h>

#include "rdg_rudp.hpp"

bool g_bKillMe = false;

#define SIZEOF_ARRAY(n) (sizeof(n)/sizeof(n[0]))
#define BLMAKE_IPADDR(n1,n2,n3,n4) (htonl( ((n1)<<24) | ((n2)<<16) | ((n3)<<8) | (n4)))

int GetALocalIpAddr( unsigned long & );

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
                            unsigned long & ulPktNum)
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

  if( (ulPktNum % 10000) == 0)
    {
    cerr << "RdgSlave sent: " << ulPktNum << endl;
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
    cerr << "RdgSlave BlrudpGetSendBuffer Error = " << nRet << endl;      // should not really see this....

    if( nRet == BLERR_BLRUDP_NO_SEND_BUFFERS )
      {
      cerr << "RdgSlave noSendBuffers" << endl;              // we really should not see this if the status check worked
      }
    cerr << "RdgSlave BlrudpSendTo Failed Return Value = " << nRet << endl;
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
    cerr << "RdgSlave BlrudpSendTo Failed Return Value = " << nRet << endl;
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


///////////////////////////////////////////////////////////////////////
//
// Handle all the processing for the child fork.
//
// inputs:
//    none.
// outputs:
//    returns -- exit code.
//
int DoSlave( unsigned long   aListenIpAddr,
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
  cerr << "RdgSlave Hey this is the child fork" << endl;

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
                       Callback_SendBufferDone,      // pfnSendBufferDone,
                       (void *)"RdgSlave");                   // *pUserData);

  if( nRet != BLERR_SUCCESS )
    {
    cerr << "RdgSlave Slave : Filed to Connect to the remote server" << endl;
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
    cerr << "RdgSlave : Failed to get IpAddr " << endl;
    FAILERR(nErr, nRet);

    }

  cerr << "RdgSlave : BlrdupGetContexAddr() returns : "
       << " ulSrcAddr " << (void *)  ulSrcAddr
       << " nSrcPort  " <<   nSrcPort
       << " ulDestAddr" << (void *)  ulDestAddr
       << " nDestPort " <<   nDestPort
       << endl;

  //
  // wait until we have sent enough, or
  // we just the other end disconnected us..
  //
  cerr << "RdgSlave : Start sending packets " << endl;

  while( (!g_bKillMe) ) // && (ulPktNum <= MAX_SEND_PACKETS) )
    {
    // stuff
    // the send buffer.
    //
    do {
       nRet = SendPktBuffrerToContext(hSenderContext,      // hContext,
                                       ulPktNum);           // &ulPktNum)

       } while ((nRet == BLERR_SUCCESS)) ;      ////////  && (ulPktNum <= MAX_SEND_PACKETS));

    if( (nRet != BLERR_SUCCESS) && (nRet != BLERR_BLRUDP_NO_SEND_BUFFERS))
      {
      cerr << "RdgSlave : Send FAILED" << endl;
      FAILERR(nErr, nRet);
      }

    BlrudpRun(hBlrudp,          // hBlrudp
              true);            // bSleep);

    }

  cerr << "RdgSlave : Done sending packets g_bKillMe = " << g_bKillMe << endl;

  //
  // perform a graceful disconnect.  I.e. wait until all packets
  // queued have been sent.
  //
  BlrudpDisconnect(hSenderContext,    // hContext,
                   false);            // bImmediate);

  BlrudpDelete(hBlrudp);

  cerr << "RdgSlave : Returning 0 " << endl;

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

  cerr << "RdgSlave : Returning Failed nErr = " << nErr << endl;
  return(nErr);
  }

int
main(int argc, char **argv, char **envp )
  {
  if( argc != 3 )
    {
    cerr << argv[0] << " RdgSlave args: RdgMasterIpAddr RdgMasterIpPort" << endl;
    return( -1 );
    }

  int RdgSendIpAddr  = atoi( argv[ 1 ] );
  int RdgSendIpPort  = atoi( argv[ 2 ] );

  cerr << argv[ 0 ]
       << "RdgSlave "
       << " RdgSendIpAddr " << RdgSendIpAddr
       << " RdgSendIpPort " << RdgSendIpPort
       << endl;

  int rc = DoSlave( RdgSendIpAddr, RdgSendIpPort );

  cerr << argv[ 0 ] << " (RdgSlave) Stopping - Return Code = " << rc << endl;

  return( 0 );
  }
