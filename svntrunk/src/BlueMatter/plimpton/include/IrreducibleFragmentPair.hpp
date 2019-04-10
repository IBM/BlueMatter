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
 






THIS CODE IS DEAD : replaced by IFP.hpp










/***************************************************************************
 * Project:         BlueMatter
 *
 * Module:          IPartPair
 *
 * Purpose:         Manage pairwise interactions between irreducible partition pairs
 *                  during NonBonded Interactions
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010522 BGF Created from design.
 *
 ***************************************************************************/

// This file contains active message infrastructure and nonbonded engine
// thread code.

// Currently, this code does not have an initialization phase.  This leads to
// some extra data in operand packets and also makes it impossible to
// dynamically allocate some of the structures.  These are allocated based
// on a compilte time limit.  THIS CAN ALL BE CHANGED AS WE GO ALONG.

#define PKFXLOG_IPARTPAIR


#include <sys/mman.h> //FOR mmap() stuff - not strategic.

#ifndef _IRREDUCIBLE_PARTITION_PAIR_HPP_
#define _IRREDUCIBLE_PARTITION_PAIR_HPP_

#ifndef PKFXLOG_NBVM
#define PKFXLOG_NBVM 0
#endif

#ifndef PKFXLOG_SITEFORCES_NB
#define PKFXLOG_SITEFORCES_NB 0
#endif

#ifndef PKFXLOG_IPARTPAIR
#define PKFXLOG_IPARTPAIR 0
#endif

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/UDF_Binding.hpp>
#include <BlueMatter/NBVM_IF.hpp>

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <pk/Grid2D.hpp>
#include <pk/ActorMulticast.hpp>

//***************************

template<class SourceGroupIF, class TargetGroupIF, class ResultIF, class EnergyIF>
void
IrreducibleFragmentPair( int           aSimTick,
                         SourceGroupIF SourceFragmentIF,
                         TargetGroupIF TargetFragmentIF,
                         ResultIF     &aResultIF,
                         EnergyIF     &aEnergyIF,
                         XYZ          &aVirial,
                         XYZ          &aInterFragmentForce,
                         void       ** aPersistantStatePtr        )
{

  int TargetIrreducibleFragmentId = TargetFragmentIF.GetIrreducibleFragmentId();
  int SourceIrreducibleFragmentId = SourceFragmentIF.GetIrreducibleFragmentId();

  // Create a flag to be used to recognize when source and target fragments are the
  // same and thus we can do 1/2 the force term computations by applying the results
  // of each computation to both sits sites.

  int SourceIsTargetFlag = SourceIrreducibleFragmentId == TargetIrreducibleFragmentId;

  BegLogLine(PKFXLOG_IPARTPAIR)
    << "IrreducibleFragmentPair(): STARTING "
    << " IrreducibleFragmentId (targ/src) "
    << TargetIrreducibleFragmentId
    << " / "
    << SourceIrreducibleFragmentId
    << " PersistantStatePointer "
    << *aPersistantStatePtr
    << EndLogLine;

  int SourceFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( SourceIrreducibleFragmentId );
  int TargetFragmentSiteCount = MSD_IF::GetIrreducibleFragmentMemberSiteCount( TargetIrreducibleFragmentId );

  // If this is the first call or things have changed, the persistant state pointer will be NULL
  // and usefull state can be generated.
  char *  PairInteractionMaskArray  = NULL; // THIS IS OUR TYPED STATE POINTER!!!!! USED LATER, BUT NOT YET ASSIGNED!

  if( *aPersistantStatePtr != NULL )
    {
    PairInteractionMaskArray = (char *) *aPersistantStatePtr;
    }
  else
    {
    // For now, we use a byte as a pair interaction mask.
    // We can ad a bit for verlet list cutoff distances easily.

    // NOTE: where Source == Target, the following array could be smaller.
    int     PairInteractionMaskArraySize = TargetFragmentSiteCount * SourceFragmentSiteCount;

    PairInteractionMaskArray     = new char[ PairInteractionMaskArraySize ];

    if( PairInteractionMaskArray == NULL )
      {
      BegLogLine(1)
        << "IrreducibleFragmentPair(): "
        << " new FAILED for "
        <<  PairInteractionMaskArraySize
        << " bytes "
        << " Now trying mmap() "
        << EndLogLine;
#ifdef PK_AIX
      PairInteractionMaskArray = (char *) mmap(NULL,
                                      PairInteractionMaskArraySize,
                                      PROT_READ|PROT_WRITE,
                                      MAP_ANONYMOUS | MAP_VARIABLE,
                                      -1,
                                      0
                                      );
#else
      assert(0);
#endif
      }

    assert( PairInteractionMaskArray != NULL );

    // ASSIGN TO PERSISTANT STATE POINTER HELD BY OUTER LAYER
    *aPersistantStatePtr = (void *) PairInteractionMaskArray;

    int     PairInteractionMaskIndex     = 0;

    BegLogLine(PKFXLOG_IPARTPAIR)
      << "IrreducibleFragmentPair(): PairInteractionMaskArraySize "
      << PairInteractionMaskArraySize
      << EndLogLine;



    //***** NOTE: THESE LOOPS MUST MUST MUST INDEX EXACTLY THE SAME AS THOSE USED LATER TO READ THE PairInteractionMaskArray.

    for(int SourceFragmentSiteIndex = 0;
            SourceFragmentSiteIndex < SourceFragmentSiteCount;
            SourceFragmentSiteIndex++ )
      {
      BegLogLine(PKFXLOG_IPARTPAIR) << "SourceFragmentSiteIndex " << SourceFragmentSiteIndex << EndLogLine;

      int SourceAbsSiteId =
          MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex );

      // Scan the Target Fragment since that should be more local.
      for(int TargetFragmentSiteIndex = (SourceIsTargetFlag ? SourceFragmentSiteIndex + 1: 0 );
          TargetFragmentSiteIndex < TargetFragmentSiteCount;
          TargetFragmentSiteIndex++ )
        {
        BegLogLine(PKFXLOG_IPARTPAIR) << "TargetFragmentSiteIndex " << TargetFragmentSiteIndex << EndLogLine;

        int TargetAbsSiteId =
          MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex );

        unsigned PairInteractionMask =
          MSD_IF::GetInteractionCodeMaskNsq( TargetAbsSiteId, SourceAbsSiteId );

        PairInteractionMaskArray[ PairInteractionMaskIndex ++ ] = PairInteractionMask;

        BegLogLine(PKFXLOG_IPARTPAIR)
          << "IrreducibleFragmentPair(): "
          << " Interaction "
          << " Source "
          << SourceAbsSiteId
          << " Target "
          << TargetAbsSiteId
          << " PairIndex "
          << PairInteractionMaskIndex - 1
          << " Mask "
          << (void *) PairInteractionMask
          << EndLogLine;

        }
      }
    } // End generation of persistant state block;


  //NEED: CHRIS: We could run the loops for(source)for(terget) loops
  //      once here to develop the distances.
  //      NOTE: we would roll in the Periodic image call from below.
  //      NOTE: this would be the loop to deal with rechecking the
  //      verley lists when signal - signalling infrastructure to be added soon.
  //      This could look like creating and priming an array of NBVMs
  //      since all the NBVM really manages is the distance.


  // Reset PairInteractionMaskIndex and then re-run those loops.
  int PairInteractionMaskIndex = 0;


  //NEED: Should create an array of these suckers and fill out the distances
  // for the whole interaction as a vector computation.
  NBVM nbvm;

  // Need to find nearest image based on COM.  Save the translation vector and apply to all sites in target
  XYZ SourceFragmentCOM = SourceFragmentIF.GetCenterOfMassPosition();
  XYZ TargetFragmentCOM = TargetFragmentIF.GetCenterOfMassPosition();
  XYZ TargetFragmentCOMImage;

  aInterFragmentForce.Zero();

  NearestImageInPeriodicVolume( SourceFragmentCOM,
                                TargetFragmentCOM,
                                TargetFragmentCOMImage);

  XYZ SourceTargetImageCOMVector = TargetFragmentCOMImage - SourceFragmentCOM;
  double DistanceSquaredCOM = SourceTargetImageCOMVector.LengthSquared();

  double EwaldFragmentVirial = 0;

  int numberOfNsqUdfs = MSD_IF::GetNsqInvocationTableSize();

  for(int SourceFragmentSiteIndex = 0;
      SourceFragmentSiteIndex < SourceFragmentSiteCount;
      SourceFragmentSiteIndex++ )
    {
    BegLogLine(PKFXLOG_IPARTPAIR) << "SourceFragmentSiteIndex " << SourceFragmentSiteIndex << EndLogLine;

    // d_i - displacement vector of a site versus fragment COM
    XYZ SourceSiteDisplacement =
      *SourceFragmentIF.GetPositionPtr(SourceFragmentSiteIndex) - SourceFragmentIF.GetCenterOfMassPosition();

    // Scan the Target Fragment since that should be more local.
    for(int TargetFragmentSiteIndex = (SourceIsTargetFlag ? SourceFragmentSiteIndex + 1 : 0 );
        TargetFragmentSiteIndex < TargetFragmentSiteCount;
        TargetFragmentSiteIndex++ )
      {
      BegLogLine(PKFXLOG_IPARTPAIR) << "TargetFragmentSiteIndex " << TargetFragmentSiteIndex << EndLogLine;

      unsigned PairInteractionMask = PairInteractionMaskArray[ PairInteractionMaskIndex ++ ];

      if( PairInteractionMask == 0 )
   continue;

      // Apply COM-based cutoffs
      //          PairInteractionMask &= COMBasedInteractionMask;


      // Bring site B into consistant periodic image with A.
      // NEED: THIS SHOULD BE DONE ONCE AND ALL SITES OF THE TARGET SHOULD BE
      //       BROUGHT INTO A CONSITANT IMAGE WITH THE SOURCES.
      // NOTE: THIS SHOULD BE DONE BY CREATING AN IMAGE TRANSFORMATION VALUE
      //       AND NOT BY MODIFIYING EITHER RECORD.
      // suits - this is now handled by imaging based on COM

      XYZ SiteA = *SourceFragmentIF.GetPositionPtr( SourceFragmentSiteIndex );

      nbvm.SetPositionPtr( NBVM::SiteA,  & SiteA );

      // Now we do imaging based on site-pairs, but virial calculation based on COM imaging
      XYZ SiteImageB;
      NearestImageInPeriodicVolume( SiteA, *TargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex ), SiteImageB );

      nbvm.SetPositionPtr( NBVM::SiteB, & SiteImageB );

      BegLogLine(PKFXLOG_IPARTPAIR)
        << "WorkThread(): "
        << " NBVM:: "
        << " Posit A "
        << nbvm.GetPosition( NBVM::SiteA )
        << " Posit B "
        << nbvm.GetPosition( NBVM::SiteB )
        << " DistanceAB "
        << nbvm.GetDistance( NBVM::SiteA, NBVM::SiteB )
        << EndLogLine;

      double DistanceSquaredAB = nbvm.GetDistanceSquared( NBVM::SiteA, NBVM::SiteB );

      //================================================================

      int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex );
      int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex );

      BegLogLine(PKFXLOG_IPARTPAIR)
        << "*** ABOUT TO LJ *** "
        << " SourceAbsSiteId "
        <<   SourceAbsSiteId
        << " TargetAbsSiteId "
        <<   TargetAbsSiteId
        << EndLogLine;

      nbvm.SetSimTick( aSimTick );

      // Generic execution of Nsq udf codes
      for( int udfInvocationIndex = 0; udfInvocationIndex < numberOfNsqUdfs; udfInvocationIndex++ )
        {

        // Examine the interaction mask for the bit associated with this invocation table index.
        unsigned toDoUdf = (PairInteractionMask >> udfInvocationIndex) & 0x1;

        if( toDoUdf )
          {
          int udfCode = MSD_IF::GetUdfCodeForIndex( udfInvocationIndex );
          int nsqType = MSD_IF::GetNsqTypeForIndex( udfInvocationIndex );

          void* paramPtr = NULL;

          if( nsqType == MSD::NSQ2)
            {
            UDF_Binding::NSQ2_Param<char> params;
            params.A = (char *) MSD_IF::GetNSQ2ParamForIndexAtSiteId( udfInvocationIndex, SourceAbsSiteId );
            params.B = (char *) MSD_IF::GetNSQ2ParamForIndexAtSiteId( udfInvocationIndex, TargetAbsSiteId );
            paramPtr = &params;
            }
          else if( nsqType == MSD::NSQ1)
            {
            paramPtr = MSD_IF::GetNSQ1ParamForIndexAtSiteIds( udfInvocationIndex,
                                                              SourceAbsSiteId,
                                                              TargetAbsSiteId);
            }
          else if (nsqType == MSD::NSQ0)
            {
            //do nothing. Compiler will take care of this.
            }
          else
            {
            assert(0);
            }

          UDF_Binding::UDF_Execute(udfCode, nbvm, paramPtr);

          // ALWAYS apply force on Target
          aResultIF.AddForce( TargetFragmentSiteIndex,  nbvm.GetForce( NBVM::SiteB ) );
          aEnergyIF.AddEnergy( udfCode, nbvm.GetEnergy() * 0.5 );

          BegLogLine(PKFXLOG_SITEFORCES_NB)
              << " UDF Code "  <<  udfCode
              << " UDF Type "  <<  nsqType
              << " aSimTick "  <<  aSimTick
              << " Target "    <<  TargetAbsSiteId
              << " Source "    <<  SourceAbsSiteId
              << " Force "       <<  nbvm.GetForce( NBVM::SiteB )
              << EndLogLine;


          // Note: HACK: Different ways of doing the virial to accomodate the different
          // calculation of Ewald and Non-Ewald Virial.
          unsigned Ewald = ((( MSD_IF::Access().LongRangeForceMethod == PreMSD::EWALD ) ||
                             ( MSD_IF::Access().LongRangeForceMethod == PreMSD::P3ME )) &&
                            (udfCode == UDF_Binding::NSQEwaldCorrection_Code ||
                             udfCode == UDF_Binding::NSQEwaldRealSpace_Code ));

          if( Ewald )
            {
            EwaldFragmentVirial -= SourceSiteDisplacement * nbvm.GetForce( NBVM::SiteA );
            EwaldFragmentVirial += nbvm.GetEnergy() * 0.5;
            }

          //* <<<<<<< IrreducibleFragmentPair.hpp
          // Write forces into aResultIF
          // First check if the source is the same as the target - in which case, apply force on source site as well
          if( SourceIsTargetFlag )
            {
            aResultIF.AddForce( SourceFragmentSiteIndex,  nbvm.GetForce( NBVM::SiteA ) );
            aEnergyIF.AddEnergy( udfCode, nbvm.GetEnergy() * 0.5 );

            if( Ewald )
              {
              XYZ TargetSiteDisplacement =
                  *TargetFragmentIF.GetPositionPtr(TargetFragmentSiteIndex)
                  - TargetFragmentIF.GetCenterOfMassPosition();
              EwaldFragmentVirial -= TargetSiteDisplacement * nbvm.GetForce( NBVM::SiteB );
              EwaldFragmentVirial += nbvm.GetEnergy() * 0.5;
              }

            BegLogLine(PKFXLOG_SITEFORCES_NB)
                << " UDF Code "  <<  udfCode
                << " UDF Type "  <<  nsqType
                << " aSimTick "  <<  aSimTick
                << " Target "    <<  TargetAbsSiteId
                << " Source "    <<  SourceAbsSiteId
                << " Force "       <<  nbvm.GetForce( NBVM::SiteA )
                << EndLogLine;
            }
          else
            {
            // accumulate virial contribution
            if( ! Ewald )
              {
              aInterFragmentForce += nbvm.GetForce( NBVM::SiteB );
              }
            }
          }
        }
      }
    //* =======
//           unsigned PairInteractionMask = PairInteractionMaskArray[ PairInteractionMaskIndex ++ ];

//           // Apply COM-based cutoffs
//        //          PairInteractionMask &= COMBasedInteractionMask;


//           // Bring site B into consistant periodic image with A.
//           // NEED: THIS SHOULD BE DONE ONCE AND ALL SITES OF THE TARGET SHOULD BE
//           //       BROUGHT INTO A CONSITANT IMAGE WITH THE SOURCES.
//           // NOTE: THIS SHOULD BE DONE BY CREATING AN IMAGE TRANSFORMATION VALUE
//           //       AND NOT BY MODIFIYING EITHER RECORD.
//           // suits - this is now handled by imaging based on COM

//           XYZ SiteA = *SourceFragmentIF.GetPositionPtr( SourceFragmentSiteIndex );

//           nbvm.SetPositionPtr( NBVM::SiteA,  & SiteA );

//        //cout << "Position of NBVM::SiteA: " <<  *(SourceFragmentIF.GetPositionPtr( SourceFragmentSiteIndex )) << endl;

//           // Do the COM-based image translation here
//           // XYZ SiteImageB = * TargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex ) + TargetTranslation;

//           // Now we do imaging based on site-pairs, but virial calculation based on COM imaging
//        XYZ SiteImageB;
//        NearestImageInPeriodicVolume( SiteA, *TargetFragmentIF.GetPositionPtr( TargetFragmentSiteIndex ), SiteImageB );

//           // NearestImageInPeriodicVolume( nbvm.GetPosition(NBVM::SiteA),
//           //                               SiteImageB,
//           //                               SiteImageB);

//        //      cout << "Position of NBVM::SiteB: " << SiteImageB << endl;


//           nbvm.SetPositionPtr( NBVM::SiteB, & SiteImageB );



//           BegLogLine(PKFXLOG_IPARTPAIR)
//             << "WorkThread(): "
//             << " NBVM:: "
//             << " Posit A "
//             << nbvm.GetPosition( NBVM::SiteA )
//             << " Posit B "
//             << nbvm.GetPosition( NBVM::SiteB )
//             << " DistanceAB "
//             << nbvm.GetDistance( NBVM::SiteA, NBVM::SiteB )
//             << EndLogLine;

//           double DistanceSquaredAB = nbvm.GetDistanceSquared( NBVM::SiteA, NBVM::SiteB );

//           //================================================================

//           int SourceAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( SourceIrreducibleFragmentId, SourceFragmentSiteIndex );
//           int TargetAbsSiteId =  MSD_IF::GetIrreducibleFragmentMemberSiteId( TargetIrreducibleFragmentId, TargetFragmentSiteIndex );

//           BegLogLine(PKFXLOG_IPARTPAIR)
//             << "*** ABOUT TO LJ *** "
//             << " SourceAbsSiteId "
//             <<   SourceAbsSiteId
//             << " TargetAbsSiteId "
//             <<   TargetAbsSiteId
//             << EndLogLine;

//           nbvm.SetSimTick( aSimTick );

//        // Generic execution of Nsq udf codes
//        for (int udfInvocationIndex = 0; udfInvocationIndex < numberOfNsqUdfs; udfInvocationIndex++ )
//            {

//                unsigned toDoUdf = (PairInteractionMask >> udfInvocationIndex) & 0x1;
//                if(toDoUdf)
//                 {
//                     int udfCode = MSD_IF::GetUdfCodeForIndex( udfInvocationIndex );
//                     int nsqType = MSD_IF::GetNsqTypeForIndex( udfInvocationIndex );

//                     void* paramPtr = NULL;

//                     if ( nsqType == MSD::NSQ2)
//                         {
//                             UDF_Binding::NSQ2_Param<char> params;
//                             params.A = (char *) MSD_IF::GetNSQ2ParamForIndexAtSiteId( udfInvocationIndex, SourceAbsSiteId );
//                             params.B = (char *) MSD_IF::GetNSQ2ParamForIndexAtSiteId( udfInvocationIndex, TargetAbsSiteId );
//                             paramPtr = &params;
//                         }
//                     else if (nsqType == MSD::NSQ1)
//                         {
//                             paramPtr = MSD_IF::GetNSQ1ParamForIndexAtSiteIds( udfInvocationIndex,
//                                                                               SourceAbsSiteId,
//                                                                               TargetAbsSiteId);
//                         }
//                     else if (nsqType == MSD::NSQ0)
//                         {
//                             //do nothing. Compiler will take care of this.
//                         }
//                     else
//                         {
//                             assert(0);
//                         }

//                     UDF_Binding::UDF_Execute(udfCode, nbvm, paramPtr);

//                     // ALWAYS apply force on Target
//                     aResultIF.AddForce( TargetFragmentSiteIndex,  nbvm.GetForce( NBVM::SiteB ) );
//                     aEnergyIF.AddEnergy( udfCode, nbvm.GetEnergy() * 0.5 );

//                     BegLogLine(PKFXLOG_SITEFORCES_NB)
//                         << " UDF Code "  <<  udfCode
//                         << " UDF Type "  <<  nsqType
//                         << " aSimTick "  <<  aSimTick
//                         << " Target "    <<  TargetAbsSiteId
//                         << " Source "    <<  SourceAbsSiteId
//                         << " Force "       <<  nbvm.GetForce( NBVM::SiteB )
//                         << EndLogLine;


// //                  if ( udfCode == UDF_Binding::NSQEwaldCorrection_Code )
// //                      {
// //                          cout << "EwaldCorrection Sites: " << TargetFragmentSiteIndex <<","<< SourceFragmentSiteIndex << endl;
// //                      }

// //                  if ( udfCode == UDF_Binding::NSQEwaldRealSpace_Code )
// //                      {
// //                          cout << "EwaldRealSpace Sites: " << TargetFragmentSiteIndex <<","<< SourceFragmentSiteIndex << endl;
// //                      }

// //                  if ( udfCode == UDF_Binding::NSQCoulomb_Code )
// //                      {
// //                          cout << "Coulomb Sites: " << TargetFragmentSiteIndex <<","<< SourceFragmentSiteIndex << endl;
// //                      }

//                     // Write forces into aResultIF
//                     // First check if the source is the same as the target - in which case, apply force on source site as well
//                     if( SourceIsTargetFlag )
//                         {
//                             aResultIF.AddForce( SourceFragmentSiteIndex,  nbvm.GetForce( NBVM::SiteA ) );
//                             aEnergyIF.AddEnergy( udfCode, nbvm.GetEnergy() * 0.5 );

//                             BegLogLine(PKFXLOG_SITEFORCES_NB)
//                                 << " UDF Code "  <<  udfCode
//                                 << " UDF Type "  <<  nsqType
//                                 << " aSimTick "  <<  aSimTick
//                                 << " Target "    <<  TargetAbsSiteId
//                                 << " Source "    <<  SourceAbsSiteId
//                                 << " Force "       <<  nbvm.GetForce( NBVM::SiteA )
//                                 << EndLogLine;
//                         }
//                     else
//                         {
//                             // accumulate virial contribution
//                                aInterFragmentForce += nbvm.GetForce( NBVM::SiteB );
//                         }
//                 }
//            }
//      }
// >>>>>>> 1.19.2.2
    }

  if( MSD_IF::Access().DoPressureControl)
    {
    // 1/2 for interfragment virial since count it twice for each pair
    double FragmentVirial = 0.5 * aInterFragmentForce.DotProduct( SourceTargetImageCOMVector ) + EwaldFragmentVirial;

    // Copy scalar values into the vector virial for now
    aVirial.mY = aVirial.mZ = aVirial.mX = FragmentVirial;

    //* <<<<<<< IrreducibleFragmentPair.hpp
    BegLogLine(PKFXLOG_IPARTPAIR)
      << "IrreducibleFragmentPair(): FINISHED "
      << " IrreducibleFragmentId (targ/src) "
      << TargetIrreducibleFragmentId
      << " / "
      << SourceIrreducibleFragmentId
      << " VirialForce "
      << aInterFragmentForce
      << " FragmentVirial "
      << FragmentVirial
      << EndLogLine;
    }
  }
// =======
//   // 1/2 for interfragment virial since count it twice for each pair
//   double FragmentVirial = 0.5 * aInterFragmentForce.DotProduct( SourceTargetImageCOMVector );

//   // Copy scalar values into the vector virial for now
//   aVirial.mY = aVirial.mZ = aVirial.mX = FragmentVirial;

//   BegLogLine(PKFXLOG_IPARTPAIR)
//     << "IrreducibleFragmentPair(): FINISHED "
//     << " IrreducibleFragmentId (targ/src) "
//     << TargetIrreducibleFragmentId
//     << " / "
//     << SourceIrreducibleFragmentId
//     << " VirialForce "
//     << aInterFragmentForce
//     << " FragmentVirial "
//     << FragmentVirial
//     << EndLogLine;
// }
// >>>>>>> 1.19.2.2


#endif
