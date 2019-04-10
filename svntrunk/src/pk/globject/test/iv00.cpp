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
 #define RUNTIME_TYPE_CHECKING  // FOR GLOBJECT STUFF

//#include <pk/mpi.hpp>
//#include <pk/reactor.hpp>

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

//#include "pkdebug.hpp"

#include <pk/globject.hpp>


class MyThing
  {
  public:
    MyThing( int t )
      {
      BegLogLine(PKFXLOG_GLOBJECT)
        << " MyThing Constructed "
        << EndLogLine;
      x = t;
      }

    int x, y, z;
  };


// Declare the globalized type to the globject system.
///GLOBJECT_TYPE_DCL(MyThing);
GCM_TypeFreeBase * GCM< MyThing >::SingletonPtr = NULL;  //A98 required NULL def.

static int
MainBackEnd(void *Msg )
  {

  GlobjectReference *ref = (GlobjectReference *) Msg;

  BegLogLine( PKFXLOG_GLOBJECT )
    << " MainBackEnd Doing Fetch: "
    << " ref->HomeMemory "
    << ref->HomeMemory
    << " ref->TableIndex "
    << ref->TableKey
    << EndLogLine;

  MyThing X = GCM<MyThing>::Fetch( *ref );

  BegLogLine( PKFXLOG_GLOBJECT )
    << " MainBackEnd Done Fetch: "
    << " X.x "
    << X.x
    << " X.y "
    << X.y
    << " X.z "
    << X.z
    << EndLogLine;

  return(0);
  }

PkMain(int argc, char **argv, char **envp)
  {
  BegLogLine(PKFXLOG_GLOBJECT)
    << " Main[" << Platform::Topology::GetAddressSpaceId() << "] "
    << " Before: Instantiation, assignment f X  "
    << EndLogLine;
  MyThing X( 142 );
  X.x = 99; X.y = 999; X.z = 9999;
  BegLogLine(PKFXLOG_GLOBJECT)
    << " Main["  << Platform::Topology::GetAddressSpaceId() << "] "
    << " After: Instantiation, assignment X  "
    << EndLogLine;

  BegLogLine(PKFXLOG_GLOBJECT)
    << " Main[" << Platform::Topology::GetAddressSpaceId() << "] "
    << " Before: X Enroll  "
    << EndLogLine;
  GlobjectReference Xref = GCM<MyThing>::Enroll( &X );
  BegLogLine(PKFXLOG_GLOBJECT)
    << " Main[" << Platform::Topology::GetAddressSpaceId() << "] "
    << " After: X Enroll  "
    << EndLogLine;
  for( int j = 0; j < Platform::Topology::GetAddressSpaceCount(); j++ )
    {
    X.z = j;
    BegLogLine(PKFXLOG_GLOBJECT)
      << " Main[" << Platform::Topology::GetAddressSpaceId() << "] "
      << " Before Trigger of Globject Fetch to be done on "
      << j
      << EndLogLine;

    ////////PkActiveMsgSubSys::Trigger( j, MainBackEnd, (void *) &Xref, sizeof(  Xref  ) );
    Platform::Reactor::Trigger( j, MainBackEnd, (void *) &Xref, sizeof(  Xref  ) );

    BegLogLine(PKFXLOG_GLOBJECT)
      << " Main[" << Platform::Topology::GetAddressSpaceId() << "] "
      << " After  Trigger of Globject Fetch to be done on "
      << j
      << EndLogLine;
    sleep(1);
    }

  for(;;)
    {
    BegLogLine(PKFXLOG_GLOBJECT)
      << " Main[" << Platform::Topology::GetAddressSpaceId() << "] "
      << " dead waiting "
      << EndLogLine;
    sleep(1);
    }

  return(0);
  }

#if 0
    void *
    PhysicalThreadMain( void * arg )
      {
      int mpirc;
      MPI_Status Status;

      for(;;)
        {
        mpirc = MPI_Recv( RecvBufferSize,
                          RecvBufferArea,
                          MPI_CHAR,
                          MPI_ANY_SOURCE,
                          MyAddress.MpiType,
                          MPI_COMM_WORLD,
                          &Status);

        int TrueLen;
        MPI_Get_count( &Status, MPI_CHAR, &TrueLen );

        }
      return( NULL );
      }
#endif
#if 0
    ObjType *operator-> ()
      {
      if( CachedInst == NULL )
        {
        if( mRef.HomeMemory == ThisMemory() )
          {
          assert( mRef.UniqueNumber == ((Inst *) mRef)->Address.UniqueNumber );
          CachedInst = (ObjType *) mRef.Address;
          }
        else
          {
          assert(0);
          //static Cashed[1024];
          if( 0 ) //object is still refed in cached, reuse )
            {
            //CachedInst = Cashed. Address;
            }
          else
            {
            //CachedInst = new Inst;
            /MPI_Send(  )
            //MPI_Recv( Inst );
            //update Cashe
            }
          }
        }
      return( CachedInst );
      }
#endif
#if 0
typedef long long UniqueNumberType;

static inline
long long LLTime()
  {
  timebasestruct_t Now;
  read_real_time( &Now, TIMEBASE_SZ );
  time_base_to_time( &Now, TIMEBASE_SZ );
  long long llNow  = (long long)Now.tb_high * (long long)1000000000 + Now.tb_low ;
  return( llNow );
  }
#endif
