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
 /***************************************************************************
 * Project:         FoxFm
 *
 * Module:          PhasedPipelineDriver
 *
 * Purpose:         This is the process undercarrage for pipeline elements.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         170297 BGF Created.
 *
 ***************************************************************************/
#ifndef __PHASEDPIPELINEDRIVER_HPP__
#define __PHASEDPIPELINEDRIVER_HPP__

#ifndef PKFXLOG_STATS_LIGHT
#define PKFXLOG_STATS_LIGHT (0)
#endif

#ifndef PKFXLOG_PPL_PD_CONFIG
#define PKFXLOG_PPL_PD_CONFIG (0)
#endif

#ifndef PKFXLOG_PPL_PD_PROTOCOL
#define PKFXLOG_PPL_PD_PROTOCOL (0)
#endif

#include "pkfxlog.hpp"
#include "pknew.hpp"
#include "pkrpc.hpp"

#include "PPL_SystemIF.hpp"

#include "pkqueue_set.hpp"

#define WorkingFlagSet       (0x01<<0)
#define WorkingFlagClear     (0)
#define PhaseMatchFlagSet    (0x01<<1)
#define PhaseMatchFlagClear  (0)
#define FirstOnQueueSet    (0x01<<2)
#define FirstOnQueueClear  (0)


#ifdef PPL_REGLOG
extern _pk_PPL_REGLOG_fd;
extern _pk_PPL_REGLOG_fd_LOCK;
#endif


template<class DomainT> //, class DomainInputType, class DomainOutputType>
class PhasedPipelineDriver : public PhysicalThreadContext,
                             public pknew
  {
  public:
    class CreateMsgType
      {
      public:
        CreateMsgType(){}
        char                       Name[128];
        unsigned                   Stage;
        unsigned                   Ordinal;
        unsigned                   ConnectorCount;
        QueueReaderRefArray        OutputQueueRefArr;
        unsigned                   PhaseMask;
        DomainT::CreateMsgType     DomainCMsg;
        CreateMsgType
            (
            unsigned               aStage,
            unsigned               aOrdinal,  // this must be supplied to make preconnected circuts
            unsigned               aConnectorCount,
            char                  *aName,
            QueueReaderRefArray   aOutputQueueRefArr,
            unsigned               aPhaseMask,
            DomainT::CreateMsgType aDomainCMsg
            )
          {
          Stage               = aStage;
          Ordinal             = aOrdinal;
          ConnectorCount      = aConnectorCount;
          OutputQueueRefArr = aOutputQueueRefArr;
          PhaseMask           = aPhaseMask;
          strncpy( Name, aName, sizeof( Name ) );
          DomainCMsg          = aDomainCMsg;
          }
      };


  public:
    typedef FlatAsChar<DomainT::InputType>  FlatDomainInputType;
    typedef FlatAsChar<DomainT::OutputType>  FlatDomainOutputType;

    typedef PhasedPipelineItem<DomainT::InputType> PD_InputType;
    typedef PhasedPipelineItem<DomainT::OutputType> PD_OutputType;

    unsigned   LastCount;

    CreateMsgType                     mCMsg;

    int                               mOrdinal;
    char                             *mName;

    DomainT                           *DomainPtr;

// NOTE: THESE VARS ARE USED A LOT... WOULD BE GOOD IF THEY WERE ON
// AS FEW CACHELINES AS POSSIBLE
    int                                Stats_XActCount;
    int                                Stats_InputItemCount;
    int                                Stats_OutputItemCount;
    unsigned PhaseMask;
    unsigned CurrentPhase;
    unsigned WorkingFlag;

    QueueReaderSetDescriptor      MyAddress;

    QueueReaderSet<PD_InputType>  InputQueueSet;

    QueueWriterSet<PD_OutputType> OutputChSet;

    PhasedPipelineItemHeader OutputItemHeader;

    // Create a process in any Pk locality.
    static
    QueueReaderSetDescriptor
    CreateProcess( int Locality, CreateMsgType aCMsg )
      {
      QueueReaderSetDescriptor Rc;

      BegLogLine( PKFXLOG_PPL_PD_CONFIG )
        << " PPL_PD::CreateProcess "
        << " Locality "
        << Locality
        << EndLogLine;

      PkRpc<CreateMsgType,
             QueueReaderSetDescriptor >::Execute
               ( Locality % MpiSubSys::GetTaskCnt(),
                 BackEnd_CreateProcess,
                 aCMsg,
                 Rc ); // This gets filled in with actors comm address.

      return( Rc );
      }

    // Create a process in the locality of the caller.
    static
    QueueReaderSetDescriptor
    CreateProcess( CreateMsgType aCMsg )
      {
      return( BackEnd_CreateProcess( aCMsg ) );
      }
    static
    QueueReaderSetDescriptor
    BackEnd_CreateProcess( CreateMsgType &aCMsg )
      {
      PhasedPipelineDriver<DomainT> *This =
        new PhasedPipelineDriver<DomainT>();
      assert( This != NULL );
      This->CsmReset();
      This->CsmClear();

      This->mCMsg = aCMsg;

      This->InputQueueSet.Init( This->mCMsg.ConnectorCount, This->mCMsg.Name );

      This->MyAddress.QueueReaderSetPtr = (unsigned) This->InputQueueSet.GetAddress();
      This->MyAddress.CommResource      = MpiSubSys::GetTaskNo();

      BegLogLine( PKFXLOG_PPL_PD_CONFIG )
        << " PPL_PD::BackEndCreateProcess "
        << " QueueReaderSetPtr "
        << (void *) (This->MyAddress.QueueReaderSetPtr)
        << " CommResource "
        << This->MyAddress.CommResource
        <<EndLogLine;

#ifdef PPL_REGLOG
      if( _pk_PPL_REGLOG_fd == 0 )
        {
        YieldLock( _pk_PPL_REGLOG_fd_LOCK );  // Protect against other threads doing init.
        if( _pk_PPL_REGLOG_fd == 0 )          // Check again after gaining lock.
          {
          errno = 0;
          _pk_PPL_REGLOG_fd = open( "PPL_REGLOG" ,
                          O_CREAT | O_TRUNC | O_WRONLY | O_APPEND,  // O_APPEND Required to ensure proper writes by threaded apps
                          S_IRUSR | S_IWUSR );
          if( errno ) printf( "LOG file open errno %d\n", strerror( errno ) );
          fflush(stdout);
          assert( _pk_PPL_REGLOG_fd > 0 );
          }
        UnYieldLock( _pk_PPL_REGLOG_fd_LOCK );
        }
#endif

      This->CsmSet();

      PhysicalThreadManager::GetInterface()->Start( This, NULL );

      return( This->MyAddress );
      }


    void *
    PhysicalThreadMain( void * arg )
      {
      CsmClear();

      OutputChSet.Init( mCMsg.Name,
                        mCMsg.Ordinal,
                        mCMsg.OutputQueueRefArr,
                        1024 );

      DomainPtr = new DomainT( mCMsg.Ordinal, mCMsg.DomainCMsg );
      assert( DomainPtr != NULL );

      LastCount    = 0;

      PhaseMask    = mCMsg.PhaseMask;
      CurrentPhase = 0;
      WorkingFlag  = 0;

      int InputMsgCount = 0;

      PD_InputType *Msg;

      Stats_XActCount = 0;

      for(;;)
        {

        CsmSet();
        Msg = InputQueueSet.GetNextItem();
        CsmClear();

        InputMsgCount++;

        unsigned OutputReadyFlag;

        unsigned Transition =
            WorkingFlag
          | ( Msg->Header.FirstFlag ? FirstOnQueueSet  : 0 );

        BegLogLine( PKFXLOG_PPL_PD_PROTOCOL )
          << mCMsg.Name << " InputCount " << InputMsgCount
          << " CurPhase " << CurrentPhase
          << " MsgPhase " << Msg->Header.Phase
          << " 1stOPhase " << ( Msg->Header.FirstFlag ? 1 : 0 )
          << " Working " << (WorkingFlag ? 1 : 0 )
          << " LastCount " << LastCount
          << " Trans "
          << (void *) Transition
          << EndLogLine;

        // Note: this state machine has been somewhat violated in
        // an effort to execute the least intstructions possible
        // on the way to the AddInputItem().  One could make the
        // Msg->Header.Phase == CurrentPhase a bit used in selecting
        // the transition, but that would implicate a fetch of CurrentPhase
        // and a comparision with Msg->HeaderPhase even when it isn't
        // possible for these to be out of sync in the protocol.
        // This is all probably a nit in performance, but it also
        // seems to make the code more intuative albeit allowing more
        // 'if's inside the transitions.

        // The 'if' for noticing the FlushFlag gets the same argument.
        // The flush flag is somewhat dangerous becuase a higher level
        // protocol module is able to have it set - and thus, might
        // not set it consistantly across all the elements of a stage
        // causing problems.  A more exacting state machine could catch this.
        // Is it worth catching?  Right now, I don't think so.

        switch( Transition )
          {
          //------------------------------------------
          case WorkingFlagClear | FirstOnQueueClear:
          //------------------------------------------
          default :
            assert(0);

          //------------------------------------------
          case WorkingFlagClear | FirstOnQueueSet  :
          //------------------------------------------
            {
            // This is the first FirstOnQueueSet received
            // so it kicks of the transaction for this pipeline
            // element.  Other FirstOnQueueSet messages will be
            // ignored.
            assert(Msg->Header.Phase == CurrentPhase);

            // Prepare to forward the FirstOnQueueSet down stream
            OutputChSet.ReserveAll();
            OutputChSet->Header              = Msg->Header;
            OutputChSet->FMLSeq.First.GloRef = Msg->FMLSeq.First.GloRef;

            // Save Header of the message for use during end of phase processing.
            OutputItemHeader = Msg->Header;
            OutputItemHeader.FirstFlag = 0;

            // If a previous stage hasn't set the Flush flag,
            // so we prepare to be active in this element in this phase.
            if( ! Msg->Header.FlushFlag )
              {
              // Output Items always carry the same header except with the
              // FirstFlag turned off.

              BegLogLine( PKFXLOG_STATS_LIGHT )
                << "STATS "
                << (void *) Stats_XActCount << " "
                << mCMsg.Name      << " "
                << mCMsg.Ordinal
                << " InputItemCount "
                << Stats_InputItemCount
                << " OutputItemCount "
                << Stats_OutputItemCount
                << EndLogLine;

              Stats_XActCount++;
              Stats_InputItemCount   = 0;
              Stats_OutputItemCount  = 0;

              PPLUSERIF_RC foprc = DomainPtr->
                      SetOpContext( Msg->Header.UserBits,
                                    Msg->FMLSeq.First.GloRef );
              assert( foprc == PPLUSERIF_RC_Accept ||
                      foprc == PPLUSERIF_RC_AcceptAndFlushRestOfPipeline   );

              // This is where we no-op out the remainder of the pipeline.
              if( foprc == PPLUSERIF_RC_AcceptAndFlushRestOfPipeline )
                OutputChSet->Header.FlushFlag = 1;
              }

            //NEED: to think about a better form of flush.  For now, the lead of
            // each packet shall flush what is ahead.  Esp for dist mem, we'd like
            // to be lazier about this.
            //OutputChSet.PushAll();

            OutputChSet.PushFlushAll();

            WorkingFlag = WorkingFlagSet;
            InputQueueSet.ReleaseItem();
            break;
            }

          //-------------------------------------------
          case WorkingFlagSet   | FirstOnQueueClear :
          //-------------------------------------------
            {
            // would be easy to develope something to check rest of header too.
            assert(Msg->Header.Phase == CurrentPhase);

#ifdef PPL_REGLOG
            char Buffer[1024 + 2 * sizeof( PD_InputType ) ];
            char *bp = Buffer;
            bp += sprintf( Buffer,"%08X %08x %08X RECV",
              Stats_XActCount,mCMsg.Stage, mCMsg.Ordinal );
            for( int i = 0; i < Msg->FMLSeq.Middle.PayloadItem.SizeOf(); i++ ) //sizeof(PD_InputType); i++ )
              bp += sprintf( bp, // " %02X", ((char *)Msg)[i] );
                             " %02X",
                             Msg->FMLSeq.Middle.PayloadItem.AsChar(i) );
            bp += sprintf(bp, "\n");
            write( _pk_PPL_REGLOG_fd, Buffer, bp - Buffer );
#endif


            PPLUSERIF_RC aiirc = DomainPtr->
                AddInputItem( Msg->FMLSeq.Middle.PayloadItem.AsTyped() );

            if( aiirc == PPLUSERIF_RC_AcceptWithOutputReady )
                aiirc = DrainOutputLoop( Msg );

            assert( aiirc == PPLUSERIF_RC_Accept );  // Domain MUST accept this request.
            Stats_InputItemCount++;

            InputQueueSet.ReleaseItem();
            break;
            }

          //-------------------------------------------
          case WorkingFlagSet   | FirstOnQueueSet   :
          //-------------------------------------------
            {
            // This transition indicates that the message is
            // either a duplicate first of phase or a
            // first of phase for the next phase.
            if( Msg->Header.Phase == CurrentPhase )
              {
              // This duplicate 'FirstOnQueueSet' is ignored.
              // Could assert the header was the same as the one we
              // used for the SetOpContext call???
              InputQueueSet.ReleaseItem();
              }
            else
              {
              // This is a first of phase for the next phase
              // and so indicates a 'Last' for the current phase.
              // Increment the LastCount and if it is less than
              // the number of connectors, suspend the queue.
              // If LastCount is equal to the number of connectors,
              // Finalize and drain and resume all.
              // NOTE: In all cases, the message is item on the queue.
              LastCount++;
              if( LastCount < mCMsg.ConnectorCount )
                {
                InputQueueSet.Suspend();
                }
              else
                {
                // When this isn't a flushed phase, call the domain.
                if( ! OutputItemHeader.FlushFlag )
                  {
                  PPLUSERIF_RC fprc = DomainPtr->FinalizeProcessing();

                  if( fprc == PPLUSERIF_RC_AcceptWithOutputReady )
                    fprc = DrainOutputLoop( Msg );
                  assert( fprc == PPLUSERIF_RC_Accept );

                  PPLUSERIF_RC cocrc = DomainPtr->ClearOpContext();
                  assert( cocrc == PPLUSERIF_RC_Accept );
                  }

                LastCount  = 0;

                WorkingFlag    = WorkingFlagClear;
                CurrentPhase   = (CurrentPhase + 1) & PhaseMask;

                InputQueueSet.ResumeAll();
                } // endif:
              }
            break;
            }

          } // End of switch
        } // End of for(;;)

      assert(0); // NOT SUPPOSED TO RETURN FROM DRIVER
      return(NULL); // KEEP COMPILER QUIET
      } // End of thread process body


    // This method is called any time the PPL User need to output to
    // outbound channel.
    PPLUSERIF_RC
    DrainOutputLoop(PD_InputType *Msg)
      {
      PPLUSERIF_RC UserRc = PPLUSERIF_RC_AcceptWithOutputReady;

      while( UserRc == PPLUSERIF_RC_AcceptWithOutputReady )
        {
        // Ask User for destination of output item.
        PPL_ORDINAL OutputItemDest = DomainPtr->GetOutputItemDest();
        assert( OutputItemDest >= 0 );

        OutputChSet.Reserve( OutputItemDest );
        OutputChSet->Header = OutputItemHeader;
        OutputChSet->Header.FirstFlag = 0;

        // Ask User for a pointer to output data.

        UserRc = DomainPtr->
          GetOutputItem( OutputChSet->FMLSeq.Middle.PayloadItem.AsTyped() );
        assert(   UserRc == PPLUSERIF_RC_Accept
               || UserRc == PPLUSERIF_RC_AcceptWithOutputReady );

        Stats_OutputItemCount++;

#ifdef PPL_REGLOG
        char Buffer[1024 + 2 * sizeof( PD_OutputType ) ];
        char *bp = Buffer;
        bp += sprintf( Buffer,"%08X %08x %08X SEND",
          Stats_XActCount, mCMsg.Stage, mCMsg.Ordinal );
        for( int i = 0; i < OutputChSet->FMLSeq.Middle.PayloadItem.SizeOf(); i++) ///sizeof( PD_OutputType ); i++ )
          bp += sprintf( bp,
                         " %02X",
                         OutputChSet->FMLSeq.Middle.PayloadItem.AsChar(i) );
                 //        ((char *)(OutputChSet.GetCurrentItemPtr()))[i] );
        bp += sprintf(bp, "\n");
        write( _pk_PPL_REGLOG_fd, Buffer, bp - Buffer );
#endif

        OutputChSet.Push();

        } // End of while output items ready.

      return( UserRc );
      }
  };

#endif
