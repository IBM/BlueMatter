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
 #ifndef __RDG_RUDP_HPP__
#define __RDG_RUDP_HPP__

#define FAIL goto Fail
#define FAILERR(nErr, nValue) {(nErr) = (nValue); goto Fail;}

extern bool g_bKillMe;

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


//////////////////////////////////////////////////////////////////
//
// Callback indicating that a buffer sent by BlrudpSend...
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
void Callback_SendBufferDone(BLRUDP_HCONTEXT /*hContext*/,
                                  unsigned long /*ulSendBufferID*/,
                                  void * /*pUserData*/)
  {

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
                                  int nOldState,
                                  int nNewState,
                                  int nBlError,
                                  void * pUserData)
  {
  const char *pszMe;
  pszMe = (const char *)pUserData;
  if (pszMe == NULL)
      pszMe = "Who?";

  cerr
       << pszMe << " : "
       << "STATE Changed : "
       << hContext
       << " ( "
       <<    ContextStateString(nOldState)
       << " -> "
       <<    ContextStateString(nNewState)
       << " ) Error Code = "
       <<    nBlError
       << endl;
  //
  // for this very simple test we are connecting one sender and one receiver.
  //
  // as soon as we receive a close we stop

  if( nNewState == BLRUDP_STATE_CLOSED )
    {
    cerr
         << pszMe << " : "
         << "Reporting STATE Changed : "
         << hContext
         << " NewState is BLRUDP_STATE_CLOSED "
         << "Setting g_bKillMe to TRUE "
         << endl;

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

//void Callback_Recv(BLRUDP_HCONTEXT hContext,
void Callback_PacketReceived(BLRUDP_HCONTEXT hContext,
                             void * pUserData)
  {
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

    if( (ulPktNum % 10000) == 0)
      {
      cerr << "Recv PktNum: " << ulPktNum << endl;
      }

    nRet = BlrudpReleaseRecvBuffer(hIoBuffer);

    if( nRet != BLERR_SUCCESS)
      {
      cerr << "BlrudpReleaseRecvBufferFailed Failed - Return Value =" << nRet << endl;
      break;
      }
    }
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

  cerr <<"Callback_ListenConnectReq pszMe: hListenContext("
       << hListenContext << "), hNewContext(" << hNewContext
       << "), ulAddr(" << (void *) ulAddr << "), nPort(" << (void*) nPort << ")" << endl;

  nRet = BlrudpAccept(hNewContext,                    // hContext
                      Callback_ConnectContextChanged,  // pfnContextChanged,
                      Callback_PacketReceived,         //Callback_Recv,            // pfnContextRecv,
                      Callback_SendBufferDone,  // pfnSendBufferDone,
                      pUserData);                     // *pUserData);

  if (nRet != BLERR_SUCCESS)
      FAIL;
  return;
  Fail:
    return;
  }


#endif
