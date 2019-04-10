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
// Revision 1.2  2003/02/10 19:12:34  blake
// ralphs added BlrudpContextGetAddr() call seems to work
//
// Revision 1.2  2003/02/10 13:55:56  ralphbel
// add dynamic port access to the system
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

#include "BlErrors.h"
#include "blrudp.h"

#define FAIL goto Fail
#define FAILERR(nErr, nValue) {(nErr) = (nValue); goto Fail;}


#define SIZEOF_ARRAY(n) (sizeof(n)/sizeof(n[0]))
#define BLMAKE_IPADDR(n1,n2,n3,n4) (htonl( ((n1)<<24) | ((n2)<<16) | ((n3)<<8) | (n4)))

//
// the ports to use.
//
#define BLFILEIO_RECV_ADDR      BLMAKE_IPADDR(127,0,0,1)
#define BLFILEIO_SEND_ADDR      BLMAKE_IPADDR(127,0,0,1)


//
// since we are doing this on the same machine we need different port.
//
#if 0
#define BLFILEIO_RECV_PORT    10030
#define BLFILEIO_SEND_PORT    10031
#else
#define BLFILEIO_RECV_PORT    10030
#define BLFILEIO_SEND_PORT    0             // sending port is dynamic in this case..
#endif


int DoChildFork();
int DoParentFork(pid_t nProcId);



const char *ContextStateString(int nState);
//
// Call back functsion where blrudp notifies us when something has happened.
//
void CallbackListenConnectReq(BLRUDP_HCONTEXT /*hListenContext*/,
                             BLRUDP_HCONTEXT hNewContext,
                             unsigned long /*ulAddr*/,
                             unsigned short /*nPort */,
                             void *pUserData);

void CallbackConnectContextChanged(BLRUDP_HCONTEXT hContext,
                                  int nOldState,
                                  int nNewState,
                                  int nBlError,
                                  void * pUserData);
void CallbackContextRecv(BLRUDP_HCONTEXT hContext,
                        void * pUserData );

void CallbackContextSendBufferDone(BLRUDP_HCONTEXT hContext,
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
int DoChildFork()
//
// Handle all the processing for the child fork.
//
// inputs:
//    none.
// outputs:
//    returns -- exit code.
//
{
    int nRet;
    int nErr = BLERR_INTERNAL_ERR;
    BLRUDP_HANDLE hBlrudp = NULL;
    BLRUDP_HCONTEXT hSenderContext = NULL;
    unsigned long ulPktNum = 0;
    const unsigned long MAX_SEND_PACKETS = 10000;
    unsigned long ulSrcAddr;
    unsigned short nSrcPort;
    unsigned long ulDestAddr;
    unsigned short nDestPort;
    
    BLRUDP_OPTIONS Options[1];
    cout << "Hey this is the child fork\n";
    
    cout << "BlrudpNew(child) = " << "\n";
    nRet = BlrudpNew(&hBlrudp ,           //*phBlrudp
                     0,                   // nNumOptions,                  
                     NULL);               // pOptions[]

    
    //
    // attempting connect...
    //
    
    //
    // window size in bytes.
    //
    Options[0].nOptionId =  BLRUDP_WINDOW_SIZE;
    Options[0].v.ulValue =  0x8*1024;

    nRet = BlrudpConnect(hBlrudp,                           // hBlrudp,                               
                        &hSenderContext,                    // *phContext,                          
                        BLFILEIO_SEND_ADDR,                 // ulSrcAddr,                                    
                        BLFILEIO_SEND_PORT,                 // nSrcPort,                                        
                        BLFILEIO_RECV_ADDR,                 // ulDestAddr,                                     
                        BLFILEIO_RECV_PORT,                 // nDestPort,                                       
                        SIZEOF_ARRAY(Options),              // numoptions
                        Options,                            // options[]
                        CallbackConnectContextChanged,      // pfnContextChanged,        
                        CallbackContextRecv,                // pfnContextRecv,              
                        CallbackContextSendBufferDone,      // pfnSendBufferDone,       
                        (void *)"Child");                   // *pUserData); 
    if (nRet != BLERR_SUCCESS)
    {
        printf("Filed to Connect to the remote server\r\n");
        FAIL;
    }
    //
    // retrieve the address and ports used here.
    nRet = BlrudpGetContextAddr(hSenderContext,         // hContext
                                &ulSrcAddr,             // *pulSrcAddr
                                &nSrcPort,              // *nSrcPort
                                &ulDestAddr,            // ulDestAddr,
                                &nDestPort);            // nDestPort

    if (nRet != BLERR_SUCCESS)
    {
        printf("BlrudpGetContextAddr failed\r\n");
        FAIL;
    }
    
    printf("Connect - ulSrcAddr(%lx), nSrcPort(%d), ulDestAddr(%lx), nDestPort(%d)\n",
            ulSrcAddr,             
            nSrcPort,              
            ulDestAddr,            
            nDestPort);            
                    
    
    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);

    //
    // wait until we have sent enough, or
    // we just the other end disconnected us..
    //
    while ((!g_bKillMe) && (ulPktNum <= MAX_SEND_PACKETS))
    {
        // stuff
        // the send buffer.
        //
        do {
            nRet = SendPktBuffrerToContext(hSenderContext,      // hContext, 
                                           ulPktNum);           // &ulPktNum)
            
        } while ((nRet == BLERR_SUCCESS) && (ulPktNum <= MAX_SEND_PACKETS));
        if ((nRet != BLERR_SUCCESS) && (nRet != BLERR_BLRUDP_NO_SEND_BUFFERS))
        {   
            printf("Send failed\n");
            FAILERR(nErr, nRet);
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
    if (hBlrudp)
    {
        BlrudpDelete(hBlrudp);
        if (hSenderContext)
        {
            BlrudpDisconnect(hSenderContext,    // hContext,
                             true);             // bImmediate);
        }                            
    }        
    return(nErr);
}

///////////////////////////////////////////////////////////////////////
int  DoParentFork(pid_t nChildPid)
//
// Handle all the processing for the parent fork.
//
// inputs:
//    nChildPid -- process id of the child process.
// outputs:
//    returns -- exit code.
//
{
    int nChildStatus;
    int nRet;
    int nErr = BLERR_INTERNAL_ERR;
    unsigned long ulSrcAddr;
    unsigned short nSrcPort;
    unsigned long ulDestAddr;
    unsigned short nDestPort;
    BLRUDP_HANDLE hBlrudp = NULL;
    BLRUDP_HCONTEXT hListenContext = NULL;
    BLRUDP_OPTIONS Options[1];

    cout << "BlrudpNew(Parent) " << "\n";
    nRet = BlrudpNew(&hBlrudp ,           //*phBlrudp
                     0,                   // nNumOptions,                  
                     NULL);               // pOptions[]

    if (nRet != BLERR_SUCCESS)
        FAILERR(nErr, nRet);
      
    
    cout << "BlrudpListen = " << nChildPid << "\n";
    //
    // window size in bytes.
    //
    Options[0].nOptionId =  BLRUDP_WINDOW_SIZE;
    Options[0].v.ulValue =  8*1024;                 // our window size..

    nRet = BlrudpListen(hBlrudp ,          // hBlrudp
                        &hListenContext,             // phContext
                        BLFILEIO_RECV_ADDR,         // ulIpAddr
                        BLFILEIO_RECV_PORT,         // nPort
                        SIZEOF_ARRAY(Options),      // numoptions
                        Options,                    // options[]
                        CallbackListenConnectReq,   // pfnListenConnectReq
                        (void *)"Parent");                  // pUserData

    if (nRet != BLERR_SUCCESS)
    {
        printf("BlrudpListen failed\r\n");
        FAIL;
    }
    //
    // retrieve the address and ports used here.
    nRet = BlrudpGetContextAddr(hListenContext,         // hContext
                                &ulSrcAddr,             // *pulSrcAddr
                                &nSrcPort,              // *nSrcPort
                                &ulDestAddr,            // ulDestAddr,
                                &nDestPort);            // nDestPort

    if (nRet != BLERR_SUCCESS)
    {
        printf("BlrudpGetContextAddr failed\r\n");
        FAIL;
    }
    
    printf("listen - ulSrcAddr(%lx), nSrcPort(%d), ulDestAddr(%lx), nDestPort(%d)\n",
            ulSrcAddr,             
            nSrcPort,              
            ulDestAddr,            
            nDestPort);            
    
    while (!g_bKillMe)
    {
        BlrudpRun(hBlrudp,          // hBlrudp
                  true);            // bSleep);
        nRet = waitpid(nChildPid, &nChildStatus,  WNOHANG);            
        if (nRet)               // did our only child quit.
            break;
                    
    }                  
    
    BlrudpUnlisten(hListenContext);
    BlrudpDelete(hBlrudp);
    
    waitpid(nChildPid, &nChildStatus, 0);
    cout << "parent exiting\n";
    return(0);
    
Fail:
    if (hBlrudp)
    {
        if (hListenContext)
            BlrudpUnlisten(hListenContext);
        BlrudpDelete(hBlrudp);
    }        
    return(nErr);
}


int main (int /**argc*/, char */*argv*/[])
{
    pid_t nProcId;
    int nRet;


    SetSignalHandlers();
    
    cout << "hello, before fork\n";
    
    
    nProcId = fork();       // fork me.
    if (nProcId < 0)
    {
    
        perror("Fork error\n");   
        FAIL;
    }
    if (nProcId == 0)       // are we the child.
       nRet = DoChildFork();
    else
        nRet = DoParentFork(nProcId);
    
    exit(nRet);
Fail:
    exit(1);    
}
/////////////////////////////////////////////////////////////
void ExitHandler(void)
// Handler for dealing with exiting an application.
// call BlrudpShutdown within this routine.
//
//
{
	if (g_bInTermHandler)		// just punt if we are in here.
		return;
	g_bInTermHandler = true;
	BlrudpShutdown();		// shut us down....

	// by by....

}

//////////////////////////////////////////////////////////////////
void SigTermHandler(int nSigNum)
//
// Termination signal handle....
//
// THis handles the following signals:
//
// SIGABRT -- abort.
// SIGTERM -- terminate
// 
// it calls the Blrudp shutdown routine
{

	PFN_SIGNAL_HANDLER pfnTermHandler = NULL;

	if (g_bInTermHandler)		// just punt if we are in here.
		return;
	
	g_bInTermHandler = true;
	BlrudpShutdown();		// shut us down....
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
		(*pfnTermHandler)(nSigNum);
	else
		exit(0);

}

/////////////////////////////////////////////////////////////////////////
void SetSignalHandlers()
//
// Setup the termination signal handlers so we can signal
// the bluelight protocol that we want to terminate and properly
// close all things down....
//
// inputs/outputs:
//    none.
//
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
int SendPktBuffrerToContext(BLRUDP_HCONTEXT hContext, 
                            unsigned long &ulPktNum)
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

    if (nRet != BLERR_SUCCESS)
    {
        FAIL;
    }
    if (nSendCount <= 0)            // this is a valid condition, wait until the rest is sent...
        FAILERR(nErr, BLERR_BLRUDP_NO_SEND_BUFFERS);                     // nothing to do....

    
    if ((ulPktNum % 1000) == 0)
        printf("Send: %ld\n", ulPktNum);    
    //
    // We are going to send using the native buffer of Blrudp.
    // this avoids having to copy data.
    //
    nRet = BlrudpGetSendBuffer(hContext,          // hContext,        
                               &hIoBuffer,        // *phIoBuffer,    
                               &pData,            // **ppDataBuffer,    
                               &ulBufferLen);     // *pulBufferLen);    
    if (nRet != BLERR_SUCCESS)
    {

        printf ("BlrudpGetSendBuffer Error = %d\r\n", nRet);      // should not really see this....

        if (nRet == BLERR_BLRUDP_NO_SEND_BUFFERS)
            printf("noSendBuffers\n");              // we really should not see this if the status check worked
        printf("BlrudpSendTo Failed(%d)\n", nRet);
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
        printf("BlrudpSendTo Failed(%d)\n", nRet);
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
const char *ContextStateString(int nState)
//
// Map a context state to a context state string.
//
// inputs:
//    nState -- context state to map.
// outputs:
//    returns -- pointer to context state string.
//
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
void CallbackListenConnectReq(BLRUDP_HCONTEXT hListenContext,
                             BLRUDP_HCONTEXT hNewContext,
                             unsigned long ulAddr,
                             unsigned short nPort ,
                             void * pUserData)
//
// Callback indicating the listening port has an incomming connection
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
{
    const char *pszMe;
    int nRet;
    
    pszMe = (const char *)pUserData;
    if (pszMe == NULL)
        pszMe = "Huh?";
    printf("CallbackListenConnectReq %s: hListenContext(%p), hNewContext(%p), ulAddr(%lx), nPort(%x) \n",
           pszMe,
           hListenContext,
           hNewContext,
           ulAddr,
           nPort);
                   
    nRet = BlrudpAccept(hNewContext,                    // hContext
                        CallbackConnectContextChanged,  // pfnContextChanged,        
                        CallbackContextRecv,            // pfnContextRecv,              
                        CallbackContextSendBufferDone,  // pfnSendBufferDone,       
                        pUserData);                     // *pUserData);                                    
    if (nRet != BLERR_SUCCESS)
        FAIL;
    return;        
Fail:      
    return;  
}

//////////////////////////////////////////////////////////////////
void CallbackConnectContextChanged(BLRUDP_HCONTEXT hContext,
                                  int nOldState,
                                  int nNewState,
                                  int nBlError,
                                  void * pUserData)
//
// Callback indicating the state of the indicated context has changed.  
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
{
    const char *pszMe;
    pszMe = (const char *)pUserData;
    if (pszMe == NULL)
        pszMe = "Huh?";
    
    printf("CtxChanged: %s: %p, (%s->%s, %d)\n",
             pszMe,
             hContext,
             ContextStateString(nOldState),
             ContextStateString(nNewState),
             nBlError);    
    //
    // for this very simple test we are connecting one sender and one receiver.
    //
    // as soon as we receive a close we stop
    if (nNewState == BLRUDP_STATE_CLOSED)
        g_bKillMe = true;                   // set the semaphore that tells us to stop             
}

////////////////////////////////////////////////////////////////
void CallbackContextRecv(BLRUDP_HCONTEXT hContext,
                         void * pUserData)
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
        if (nRet != BLERR_SUCCESS)
        {
            if (nRet != BLERR_BLRUDP_NO_RECVDATA)
            {
                printf("BlrudpRecvFromContext Failed(%d)\n", nRet);
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

        if ((ulPktNum % 1000) == 0)
            printf("Recv: %d\n", ulPktNum);
            
        nRet = BlrudpReleaseRecvBuffer(hIoBuffer);
        if (nRet != BLERR_SUCCESS)
        {
            printf("BlrudpReleaseRecvBufferFailed(%d)\n", nRet);
            break;
        }
    }
    

}

//////////////////////////////////////////////////////////////////
void CallbackContextSendBufferDone(BLRUDP_HCONTEXT /*hContext*/,
                                  unsigned long /*ulSendBufferID*/,
                                  void * /*pUserData*/)
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
{

}


