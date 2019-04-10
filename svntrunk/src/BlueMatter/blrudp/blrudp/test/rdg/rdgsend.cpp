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


///////////////////////////////////////////////////////////////////////
//
// Handle all the processing for the child fork.
//
// inputs:
//    none.
// outputs:
//    returns -- exit code.
//
int Sender(unsigned window_size,
           unsigned SendIpAddr,
           unsigned SendIpPort,
           unsigned RecvIpAddr,
           unsigned RecvIpPort )
  {
  int                 nRet;
  int                 nErr = BLERR_INTERNAL_ERR;
  BLRUDP_HANDLE       hBlrudp = NULL;
  BLRUDP_HCONTEXT     hSenderContext = NULL;
  unsigned long       ulPktNum = 0;
  const unsigned long MAX_SEND_PACKETS = 10000;

  cerr << "Hey this is the child fork\n";

  cerr << "BlrudpNew(child) = " << "\n";

  nRet = BlrudpNew(&hBlrudp ,           //*phBlrudp
                   0,                   // nNumOptions,
                   NULL);               // pOptions[]

  //
  // attempting connect...
  //

  //
  // window size in bytes.
  //

  BLRUDP_OPTIONS      Options[1];
  Options[0].nOptionId =  BLRUDP_WINDOW_SIZE;
  Options[0].v.ulValue =  0x8*1024;
  int TotalOptionCount = 1;

  nRet = BlrudpConnect(hBlrudp,                           // hBlrudp,
                       &hSenderContext,                    // *phContext,
                       SendIpAddr,            // BLFILEIO_SEND_ADDR,                 // ulSrcAddr, // local
                       SendIpPort,            // BLFILEIO_SEND_PORT,                 // nSrcPort,  // local
                       RecvIpAddr,            // BLFILEIO_RECV_ADDR,                 // ulDestAddr,// remote
                       RecvIpPort,            // BLFILEIO_RECV_PORT,                 // nDestPort, // remote
                       TotalOptionCount,      // SIZEOF_ARRAY(Options),              // numoptions
                       Options,                            // options[]
                       CallbackConnectContextChanged,      // pfnContextChanged,
                       CallbackContextRecv,                // pfnContextRecv,
                       CallbackContextSendBufferDone,      // pfnSendBufferDone,
                       (void *)"Child");                   // *pUserData);

  if( nRet != BLERR_SUCCESS )
    {
    cerr << "Connect failed - Return Value = " << nRet << endl;
    goto Fail;
    }

  //
  // wait until we have sent enough, or
  // we just the other end disconnected us..
  //
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
      cerr << "Send failed - Return Value = " << nRet << endl;
      goto Fail;
      }

    BlrudpRun(hBlrudp,          // hBlrudp
              true);            // bSleep);
    }

  //
  // perform a graceful disconnect.  I.e. wait until all packets
  // queued have been sent.
  //
  BlrudpDisconnect(hSenderContext,    // hContext,
                   false);            // bImmediate);

  BlrudpDelete(hBlrudp);

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
  return(nErr);
  }

int
main(int argc, char **argv, char **envp )
  {
  if( argc != 6 )
    {
    cerr << argv[0] << " args: window_size SendIpAddr SendIpPort RecvIpAddr RecvIpPort " << endl;
    return( -1 );
    }

  int window_size = atoi( argv[ 1 ] );
  int SendIpAddr  = atoi( argv[ 2 ] );
  int SendIpPort  = atoi( argv[ 3 ] );
  int RecvIpAddr  = atoi( argv[ 4 ] );
  int RecvIpPort  = atoi( argv[ 5 ] );

  cerr << argv[ 0 ]
       << " SendIpAddr " << SendIpAddr
       << " SendIpPort " << SendIpPort
       << " RecvIpAddr " << RecvIpAddr
       << " RecvIpPort " << RecvIpPort
       << endl;

  int rc = Sender( window_size, SendIpAddr, SendIpPort, RecvIpAddr, RecvIpPort );

  cerr << argv[ 0 ] << " Stopping - Return Code = " << rc << endl;

  return( 0 );
  }
