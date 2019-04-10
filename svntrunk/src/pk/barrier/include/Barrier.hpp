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
 #ifndef _THREADBARRIER_HPP_
#define _THREADBARRIER_HPP_

class ThreadBarrier
  {
//    enum { ParticipantsNotSet = 0xFFFFFFFF };
//BGF try this to get around fact that constructor not called when we split compiles
// makes the value look like the zero that comes with static data
    enum { ParticipantsNotSet = 0  };
    AtomicInteger mCounter[2];
    unsigned mParticipants;
  public:

    ThreadBarrier( )
      {
      mCounter[0].AtomicSet(0);
      mCounter[1].AtomicSet(0);
      mParticipants = ParticipantsNotSet;
      }

    void
    Init( unsigned aParticipants )
      {
      assert(    mParticipants == ParticipantsNotSet
              || mParticipants == aParticipants );
      mParticipants = aParticipants;
      }

    void
    Wait(unsigned Value)
      {
      int Select = Value & 0x00000001;
      assert( mParticipants != ParticipantsNotSet );
      assert( mCounter[ Select ].SafeFetch()  >=  (Value/2)  * mParticipants );
      assert( mCounter[ Select ].SafeFetch()  <=  (Value/2+1) * mParticipants );
      // Could check here that the other mCounter is in range as well...
      ////mCounter[ Select ]++;
      mCounter[ Select ].AtomicAdd( 1 );
      Platform::Memory::ExportFence();  // NEED: CHRIS CHECK THIS OUT!!!!!!!
      unsigned Exit = ((Value/2)+1) * mParticipants;
      ////while( mCounter[ Select ] < Exit )
      BegLogLine(0)
        << "ThreadBarrier::Wait() Begin SeqNo " << Value
        << " Counter " << mCounter[ Select ].SafeFetch()
        << " Participants " << mParticipants
        << " Exit " << Exit
        << EndLogLine;

      while( mCounter[ Select ].SafeFetch() < Exit )
        {
        Platform::Thread::Yield();
        Platform::Memory::ImportFence();  //NEED: CHRIS CHECK THIS OUT??????
        }
      BegLogLine(0)
        << "ThreadBarrier::Wait() Finished SeqNo " << Value
        << " Counter " << mCounter[ Select ].SafeFetch()
        << " Participants " << mParticipants
        << " Exit " << Exit
        << EndLogLine;
      }

    void
    WaitToBeLast(unsigned Value) // Nobody's gonna like this... obviously only one participant can... real ugly...
      {
      assert( mParticipants != ParticipantsNotSet );
      int Select = Value & 0x00000001;
      assert( mCounter[ Select ].SafeFetch()  >=  (Value/2)  * mParticipants );
      assert( mCounter[ Select ].SafeFetch()  <=  (Value/2+1) * mParticipants );
      // Could check here that the other mCounter is in range as well...
      ////// don't do this mCounter[ Select ]++;
      unsigned Exit = ((Value/2)+1) * mParticipants;
      /////while( mCounter[ Select ] < (Exit-1) )
      BegLogLine(0)
        << "ThreadBarrier::WaitToBeLast() Begin SeqNo " << Value
        << " Counter " << mCounter[ Select ].SafeFetch()
        << " Participants " << mParticipants
        << " Exit " << Exit
        << EndLogLine;
      while( mCounter[ Select ].SafeFetch() < (Exit-1) )
        Platform::Thread::Yield();
      BegLogLine(0)
        << "ThreadBarrier::WaitToBeLast() Begin SeqNo " << Value
        << " Counter " << mCounter[ Select ].SafeFetch()
        << " Participants " << mParticipants
        << " Exit " << Exit
        << EndLogLine;
      }
  };

#endif
