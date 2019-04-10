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
 #ifndef __PKRPC_HPP__
#define __PKRPC_HPP__

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

// PkRpc allows a user function to be triggered at
// a remote location. The results are returned to
// the source process.
// The user must package the arguments to the function
// inside a class. A class must also be defined to hold
// the results. PkRpc is templatized by these classes.
// To trigger a remote function the user calls Execute
// specifying a locality, function, and argument and result
// objects.

template<class RpcArg, class RpcResult>
class PkRpc
  {
  public:
    typedef RpcResult (*RpcUserFxPtr_t)( RpcArg& Arg );

    class ResponseMsg
      {
      public:                                     //This is the package returned by
        unsigned             Src;
        unsigned             Len;
        int            *PtrTo_ResponseDoneFlag;   //the remote process after it has
        RpcResult      *PtrTo_ResultMsgBuffer;    //executed the requested function.
        RpcResult       Result;
      };

    class RequestMsg
      {
      public:                                     //This is the package sent to the remote
        unsigned             Src;
        unsigned             Len;
        RpcUserFxPtr_t  UserFx;                   //location. It specifys the function to
        RpcResult      *PtrTo_ResultMsgBuffer;    //be executed and the arguments to be used.
        int            *PtrTo_ResponseDoneFlag;
        RpcArg          Args;
      };

    // This physical actor delivers the response and notifies
    // the thread which did the Execute that the return is ready.
    static int
    ResponseBackEnd(void *Msg )
      {                                                       //This function is triggered (in the
      ResponseMsg  *ResMsg = (ResponseMsg *) Msg;             //locality that called the Execute) by the
      unsigned Src = ResMsg->Src;
      unsigned Len = ResMsg->Len;

      *(ResMsg->PtrTo_ResultMsgBuffer) = ResMsg->Result;      //remote process. It copies the results
      // Do we need to a memory fence here?                   //into the results buffer and sets the
      *(ResMsg->PtrTo_ResponseDoneFlag) = 1;                  //ResponseDone flag.
      return(0);
      }

    // This physical actor actually executes the target function
    // in the correct process.
    static int
    RequestBackEnd(void *Msg )
      {                                                       //This function is triggered (in the
      RequestMsg  *ReqMsg = (RequestMsg *) Msg;                       //remote locality. It executes the
      unsigned Src = ReqMsg->Src;
      unsigned Len = ReqMsg->Len;
      ResponseMsg  ResMsg;                                            //users function with the supplied args
      ResMsg.PtrTo_ResultMsgBuffer  = ReqMsg->PtrTo_ResultMsgBuffer;  //and assembles a result message. It then
      ResMsg.PtrTo_ResponseDoneFlag = ReqMsg->PtrTo_ResponseDoneFlag; //sends the results back by triggering
      ResMsg.Result = ReqMsg->UserFx( ReqMsg->Args );                 //ResponseBackEnd in the source locality.

      ResMsg.Len =sizeof( ResponseMsg );
      ResMsg.Src = Platform::Topology::GetAddressSpaceId() ;

      Platform::Reactor::Trigger( Src,
                                  ResponseBackEnd,
                                  (void *) &ResMsg,
                                  sizeof( ResponseMsg ) );
      return(0);
      }

    static int
    Execute( int             aLocality,                               //This is the function called by the user.
             RpcUserFxPtr_t   UserFx,                                 //A locality, function, argument package
             RpcArg         &aArgs,                                   //and results package are specified.
             RpcResult      &aResult )                                //A RequestMaessage is constructed and
      {                                                               //RequestBackEnd is triggered at the
      RequestMsg   ReqMsg;                                            //appropriate locality.
      int ResponseDoneFlag = 0;  // Does this need to be volatile?    //When the user function has completed
                                                                      //its execution the remote process triggers
      ReqMsg.PtrTo_ResultMsgBuffer  = &aResult;                       //ResponseBackEnd at this locality. This copies
      ReqMsg.PtrTo_ResponseDoneFlag = &ResponseDoneFlag;              //the results into the ResultMsgBuffer and sets
      ReqMsg.Args                   = aArgs;                          //the ResponseDone flag.
      ReqMsg.UserFx                 = UserFx;

      ReqMsg.Len =sizeof( RequestMsg );
      ReqMsg.Src = Platform::Topology::GetAddressSpaceId() ;

      //Check locality - if bad, LOG and tank.
      if( aLocality > Platform::Topology::GetAddressSpaceCount() )
        {
        BegLogLine(1)
          << " PPL_PD::CreateProcess CONFIG FATAL ERROR "
          << " Locality "
          << aLocality
          << " to larger AddressSpaceCount "
          << Platform::Topology::GetAddressSpaceCount()
          << EndLogLine;

        PLATFORM_ABORT("RPC target locality out of range");
        }

      Platform::Reactor::Trigger( aLocality,
                                   RequestBackEnd,
                                   (void *) &ReqMsg,
                                   sizeof( RequestMsg ) );

#if _THREAD_SAFE
      // When threaded, wait.
      //A98:  while( ResponseDoneFlag == 0 ) pthread_yield();
      while( ResponseDoneFlag == 0 ) sched_yield();
#else
      // When not threaded, it better be done!!!.
      assert( ResponseDoneFlag != 0 );
#endif


      return(0);
      }
  };

#endif
