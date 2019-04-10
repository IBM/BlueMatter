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
 * Project:         BlueMatter
 *
 * Module:          IPartPair
 *
 * Purpose:         Manage pairwise interactions between irreducible partition pairs
 *                  during NonBonded Interactions, specialised for use when each pair
 *                  is a single water molecule
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         010817 TJCW Created from IrreduciblePartitionPair.
 *
 ***************************************************************************/

// This file contains active message infrastructure and nonbonded engine
// thread code.

// Currently, this code does not have an initialization phase.  This leads to
// some extra data in operand packets and also makes it impossible to
// dynamically allocate some of the structures.  These are allocated based
// on a compilte time limit.  THIS CAN ALL BE CHANGED AS WE GO ALONG.

#include <sys/mman.h> //FOR mmap() stuff - not strategic.

#ifndef _WATER_FRAGMENT_PAIR_HPP_
#define _WATER_FRAGMENT_PAIR_HPP_

#ifndef PKFXLOG_NBVM
#define PKFXLOG_NBVM 0
#endif

#ifndef PKFXLOG_SITEFORCES_NB
#define PKFXLOG_SITEFORCES_NB 0
#endif

#ifndef PKFXLOG_WATERPAIR
#define PKFXLOG_WATERPAIR 0
#endif

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/UDF_Binding.hpp>
#include <BlueMatter/NBVM_IF.hpp>
#include <BlueMatter/WATERVM_IF.hpp>

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>
#include <pk/Grid2D.hpp>
#include <pk/ActorMulticast.hpp>

//***************************

typedef struct {
   double p0 ;
   double p1 ;
   } LennardJonesForce_Params ;
typedef struct {
   double p0 ;
   } StdChargeForce_Params ;
const LennardJonesForce_Params LJParams_O_O = {
   1.0
   , 2.0
   } ;
const LennardJonesForce_Params LJParams_O_H = {
   1.0
   , 2.0
   } ;
const LennardJonesForce_Params LJParams_H_H = {
   1.0
   , 2.0
   } ;
const StdChargeForce_Params ChParams_O_O = {
   1.0
   } ;
const StdChargeForce_Params ChParams_O_H = {
   1.0
   } ;
const StdChargeForce_Params ChParams_H_H = {
   1.0
   } ;

template<class SourceGroupIF, class TargetGroupIF, class ResultIF, class EnergyIF>
void
WaterFragmentPair       ( int           aSimTick,
                          SourceGroupIF SourcePartitionElementIF,
                          TargetGroupIF TargetPartitionElementIF,
                          ResultIF     &aResultIF,
                          EnergyIF     &aEnergyIF
                          )
  {

//int TargetIrreduciblePartitionElementId = TargetPartitionElementIF.GetIrreduciblePartitionElementId();
//int SourceIrreduciblePartitionElementId = SourcePartitionElementIF.GetIrreduciblePartitionElementId();

//// Create a flag to be used to recognize when source and target fragments are the
//// same; for water this means nothing to compute.

//int SourceIsTargetFlag = SourceIrreduciblePartitionElementId == TargetIrreduciblePartitionElementId;

  BegLogLine(PKFXLOG_WATERPAIR)
    << "WaterFragmentPair(): STARTING "
//  << " WaterFragmentElementId (targ/src) "
//  << TargetIrreduciblePartitionElementId
//  << " / "
//  << SourceIrreduciblePartitionElementId
    << EndLogLine;

//if ( SourceIsTargetFlag ) return ;        // Nothing to compute in the NBVM between water and itself

//int SourceFragmentSiteCount = MSD_IF::GetIrreduciblePartitionElementMemberSiteCount( SourceIrreduciblePartitionElementId );
//int TargetFragmentSiteCount = MSD_IF::GetIrreduciblePartitionElementMemberSiteCount( TargetIrreduciblePartitionElementId );
//
//assert(3 == SourceFragmentSiteCount ) ;  // Water has 3 atoms
//assert(3 == TargetFragmentSiteCount ) ;  // Water has 3 atoms

  // Fetch the atom positions
  XYZ Position_Source_OX = * SourcePartitionElementIF.GetPositionPtr(0) ;
  XYZ Position_Source_H1 = * SourcePartitionElementIF.GetPositionPtr(1) ;
  XYZ Position_Source_H2 = * SourcePartitionElementIF.GetPositionPtr(2) ;

  XYZ Position_Target_OX = * TargetPartitionElementIF.GetPositionPtr(0) ;
  XYZ Position_Target_H1 = * TargetPartitionElementIF.GetPositionPtr(1) ;
  XYZ Position_Target_H2 = * TargetPartitionElementIF.GetPositionPtr(2) ;

  // Find the nearest image of the oxygen atom
  XYZ Nearest_Target_OX ;
  NearestImageInPeriodicVolume(Position_Source_OX, Position_Target_OX, Nearest_Target_OX) ;

  // Translation vector to move target molecule to its nearest image
  XYZ Translation_To_Nearest = Nearest_Target_OX - Position_Target_OX ;

  // Coherent nearest image of hydrogens
  XYZ Nearest_Target_H1 = Position_Target_H1 + Translation_To_Nearest ;
  XYZ Nearest_Target_H2 = Position_Target_H2 + Translation_To_Nearest ;

  // Vectors from source atoms to target atoms
  XYZ Vector_OX_OX = Nearest_Target_OX - Position_Source_OX ;
  XYZ Vector_OX_H1 = Nearest_Target_H1 - Position_Source_OX ;
  XYZ Vector_OX_H2 = Nearest_Target_H2 - Position_Source_OX ;
  XYZ Vector_H1_OX = Nearest_Target_OX - Position_Source_H1 ;
  XYZ Vector_H1_H1 = Nearest_Target_H1 - Position_Source_H1 ;
  XYZ Vector_H1_H2 = Nearest_Target_H2 - Position_Source_H1 ;
  XYZ Vector_H2_OX = Nearest_Target_OX - Position_Source_H2 ;
  XYZ Vector_H2_H1 = Nearest_Target_H1 - Position_Source_H2 ;
  XYZ Vector_H2_H2 = Nearest_Target_H2 - Position_Source_H2 ;

  // Squared lengths of these vectors
  double L2_OX_OX = Vector_OX_OX.LengthSquared() ; // !!! Having evaluated this, we can specialise 4 ways
                                                   // 0: Within both cutoff radii, evaluate LJ and electrostatic
                                                   // 1: Outside LJ range, inside electrostatic
                                                   // 2: Inside LJ range, outside electrostatic - physically implausible
                                                   // 3: Outside both ranges - duck out, no force or energy
                                                   //    (applying smoothing if desired)
  double L2_OX_H1 = Vector_OX_H1.LengthSquared() ;
  double L2_OX_H2 = Vector_OX_H1.LengthSquared() ;
  double L2_H1_OX = Vector_H1_OX.LengthSquared() ;
  double L2_H1_H1 = Vector_H1_H1.LengthSquared() ;
  double L2_H1_H2 = Vector_H1_H1.LengthSquared() ;
  double L2_H2_OX = Vector_H2_OX.LengthSquared() ;
  double L2_H2_H1 = Vector_H2_H1.LengthSquared() ;
  double L2_H2_H2 = Vector_H2_H1.LengthSquared() ;

  // Reciprocal lengths of these vectors
  double R_OX_OX = 1.0 / sqrt(L2_OX_OX) ;
  double R_OX_H1 = 1.0 / sqrt(L2_OX_H1) ;
  double R_OX_H2 = 1.0 / sqrt(L2_OX_H2) ;
  double R_H1_OX = 1.0 / sqrt(L2_H1_OX) ;
  double R_H1_H1 = 1.0 / sqrt(L2_H1_H1) ;
  double R_H1_H2 = 1.0 / sqrt(L2_H1_H2) ;
  double R_H2_OX = 1.0 / sqrt(L2_H2_OX) ;
  double R_H2_H1 = 1.0 / sqrt(L2_H2_H1) ;
  double R_H2_H2 = 1.0 / sqrt(L2_H2_H2) ;

  // If the same water molecule is passed in as source and target, duck out here
  if (0.0 == L2_OX_OX )
  {
     return ;
  } /* endif */

  // Reciprocal square lengths of these vectors
  double R2_OX_OX = R_OX_OX * R_OX_OX ;
  double R2_OX_H1 = R_OX_H1 * R_OX_H1 ;
  double R2_OX_H2 = R_OX_H2 * R_OX_H2 ;
  double R2_H1_OX = R_H1_OX * R_H1_OX ;
  double R2_H1_H1 = R_H1_H1 * R_H1_H1 ;
  double R2_H1_H2 = R_H1_H2 * R_H1_H2 ;
  double R2_H2_OX = R_H2_OX * R_H2_OX ;
  double R2_H2_H1 = R_H2_H1 * R_H2_H1 ;
  double R2_H2_H2 = R_H2_H2 * R_H2_H2 ;

  // MDVM's for each pair

  WATERNBVM VM_OX_OX(Vector_OX_OX, L2_OX_OX, R2_OX_OX, R_OX_OX) ;
  WATERNBVM VM_OX_H1(Vector_OX_H1, L2_OX_H1, R2_OX_H1, R_OX_H1) ;
  WATERNBVM VM_OX_H2(Vector_OX_H2, L2_OX_H2, R2_OX_H2, R_OX_H2) ;
  WATERNBVM VM_H1_OX(Vector_H1_OX, L2_H1_OX, R2_H1_OX, R_H1_OX) ;
  WATERNBVM VM_H1_H1(Vector_H1_H1, L2_H1_H1, R2_H1_H1, R_H1_H1) ;
  WATERNBVM VM_H1_H2(Vector_H1_H2, L2_H1_H2, R2_H1_H2, R_H1_H2) ;
  WATERNBVM VM_H2_OX(Vector_H2_OX, L2_H2_OX, R2_H2_OX, R_H2_OX) ;
  WATERNBVM VM_H2_H1(Vector_H2_H1, L2_H2_H1, R2_H2_H1, R_H2_H1) ;
  WATERNBVM VM_H2_H2(Vector_H2_H2, L2_H2_H2, R2_H2_H2, R_H2_H2) ;

  // Lennard-Jones forces
  UDF_Binding::UDF_Execute( UDF_Binding::LennardJonesForce_Code, VM_OX_OX, (void *) &LJParams_O_O) ;
  UDF_Binding::UDF_Execute( UDF_Binding::LennardJonesForce_Code, VM_OX_H1, (void *) &LJParams_O_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::LennardJonesForce_Code, VM_OX_H2, (void *) &LJParams_O_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::LennardJonesForce_Code, VM_H1_OX, (void *) &LJParams_O_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::LennardJonesForce_Code, VM_H1_H1, (void *) &LJParams_H_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::LennardJonesForce_Code, VM_H1_H2, (void *) &LJParams_H_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::LennardJonesForce_Code, VM_H2_OX, (void *) &LJParams_O_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::LennardJonesForce_Code, VM_H2_H1, (void *) &LJParams_H_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::LennardJonesForce_Code, VM_H2_H2, (void *) &LJParams_H_H) ;

  // Pull out the LJ's to make room for the charge-charge forces
  XYZ Force_OX = VM_OX_OX.GetForce(0) + VM_OX_H1.GetForce(0) + VM_OX_H2.GetForce(0) ;
  XYZ Force_H1 = VM_H1_OX.GetForce(0) + VM_H1_H1.GetForce(0) + VM_H1_H2.GetForce(0) ;
  XYZ Force_H2 = VM_H2_OX.GetForce(0) + VM_H2_H1.GetForce(0) + VM_H2_H2.GetForce(0) ;

  // Apply the energy for LJ
  aEnergyIF.AddEnergy( UDF_Binding::LennardJonesForce_Code, 0.5 *
   ( VM_OX_OX.GetEnergy() + VM_OX_H1.GetEnergy() + VM_OX_H2.GetEnergy()
   + VM_H1_OX.GetEnergy() + VM_H1_H1.GetEnergy() + VM_H1_H2.GetEnergy()
   + VM_H2_OX.GetEnergy() + VM_H2_H1.GetEnergy() + VM_H2_H2.GetEnergy() )
   ) ;

  // Charge forces
  UDF_Binding::UDF_Execute( UDF_Binding::StdChargeForce_Code, VM_OX_OX, (void *) &ChParams_O_O) ;
  UDF_Binding::UDF_Execute( UDF_Binding::StdChargeForce_Code, VM_OX_H1, (void *) &ChParams_O_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::StdChargeForce_Code, VM_OX_H2, (void *) &ChParams_O_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::StdChargeForce_Code, VM_H1_OX, (void *) &ChParams_O_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::StdChargeForce_Code, VM_H1_H1, (void *) &ChParams_H_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::StdChargeForce_Code, VM_H1_H2, (void *) &ChParams_H_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::StdChargeForce_Code, VM_H2_OX, (void *) &ChParams_O_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::StdChargeForce_Code, VM_H2_H1, (void *) &ChParams_H_H) ;
  UDF_Binding::UDF_Execute( UDF_Binding::StdChargeForce_Code, VM_H2_H2, (void *) &ChParams_H_H) ;

  // Apply the energy for charge
  aEnergyIF.AddEnergy( UDF_Binding::StdChargeForce_Code, 0.5 *
   ( VM_OX_OX.GetEnergy() + VM_OX_H1.GetEnergy() + VM_OX_H2.GetEnergy()
   + VM_H1_OX.GetEnergy() + VM_H1_H1.GetEnergy() + VM_H1_H2.GetEnergy()
   + VM_H2_OX.GetEnergy() + VM_H2_H1.GetEnergy() + VM_H2_H2.GetEnergy() )
   ) ;

  // We now have the complete pairwise force
  aResultIF.AddForce(0, Force_OX + VM_OX_OX.GetForce(0) + VM_OX_H1.GetForce(0) + VM_OX_H2.GetForce(0) );
  aResultIF.AddForce(1, Force_H1 + VM_H1_OX.GetForce(0) + VM_H1_H1.GetForce(0) + VM_H1_H2.GetForce(0) );
  aResultIF.AddForce(2, Force_H2 + VM_H2_OX.GetForce(0) + VM_H2_H1.GetForce(0) + VM_H2_H2.GetForce(0) );

  BegLogLine(PKFXLOG_WATERPAIR)
    << "WaterFragmentPair(): FINISHED "
//  << " IrreduciblePartitionElementId (targ/src) "
//  << TargetIrreduciblePartitionElementId
//  << " / "
//  << SourceIrreduciblePartitionElementId
    << EndLogLine;

  return;
  }
//******************************

#endif
