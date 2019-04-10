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
 #ifndef __PKMULTICAST_HPP__
#define __PKMULTICAST_HPP__

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

// Multicast allows a user function to be triggered at
// a set of remote locations. There is no response.
// The intent is to carry data to a different partition map
// for the machine; e.g. from a 3d volume decomposition to
// a 2d cross with replication for Plimpton; the target node
// will know beforehand what to do with each data item it
// receives, when all of them have arrived, and what to do
// when all have arrived.
// The user must package the arguments to the function
// inside a class.
// PkMulticast is templatized by these classes.

// This version runs a 1-level point-to-point protocol; various
// multi-hop protocols, or hardware-assisted partials, can be imagined
// for some topologies.

template<class MulticastArg,
         class MulticastTarget
        >
class PkMulticast
  {
  public:
    typedef void (*MulticastUserFxPtr_t)( MulticastArg& Arg );

    class RequestMsg
      {
      public:                                     //This is the package sent to the remote
                                                  //location. It specifys the function to
                                                  //be executed and the arguments to be used.
        unsigned             Src;
        unsigned             Len;
        MulticastUserFxPtr_t UserFx;
        MulticastArg         Args;
      };

    // This physical actor actually executes the target function
    // in the correct process.
    static int
    RequestBackEnd(void *Msg )
      {                                                       //This function is triggered (in the
      RequestMsg  *ReqMsg = (RequestMsg *) Msg;              //remote locality).
      unsigned Src = ReqMsg->Src;
      unsigned Len = ReqMsg->Len;
      ReqMsg->UserFx( ReqMsg->Args );

      return(0);
      }

    //This is the function called by the user.
    static void
    Multicast( const MulticastTarget&     aTarget,             // Which multicast group to send to
             MulticastUserFxPtr_t   UserFx,                        // What function to drive there
             const MulticastArg&         aArgs               // What arguments to pass to the function
             )
      {
         RequestMsg   ReqMsg;

         ReqMsg.Args                   = aArgs;
         ReqMsg.UserFx                 = UserFx;

         ReqMsg.Len =sizeof( RequestMsg );
         ReqMsg.Src = Platform::Topology::GetAddressSpaceId() ;

         int TargetCount = aTarget.TargetCount() ;
         for (int x=0; x < TargetCount ; x += 1)
         {
            int aLocality = aTarget.Target(x) ;
            BegLogLine (1)
              << "Multicast sending to locality "
              << aLocality
              << EndLogLine ;

            Platform::Reactor::Trigger( aLocality,
                                         RequestBackEnd,
                                         (void *) &ReqMsg,
                                         sizeof( RequestMsg ) );
         } /* endfor */

      }
  };

#endif
