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
 #ifndef __FORCETERMS_HPP__
#define __FORCETERMS_HPP__

#include <pk/platform.hpp>

#include <pk/fxlogger.hpp>

#include <BlueMatter/MolSysDef.hpp>
#include <BlueMatter/ForceParameters.hpp>
#include <BlueMatter/SimulationParameters.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/math.hpp>

//BGF: NEED: I don't see ANY reason for not coding these terms without
//           conditionals and macros.  We should decide fx at set up.
#include <BlueMatter/MolSysDef.hpp>

#ifndef PKFXLOG_FORCE_TERMS
#define PKFXLOG_FORCE_TERMS 0
#endif

typedef XYZ SitePosition;
typedef XYZ ForceVector;
typedef XYZ ChargePosition;
typedef XYZ CartesianVector;

XYZ ZeroXYZ;

// #define SWOPELAKE 1

// NEED: to move this to an RTP parm.
#define TOO_DAMN_CLOSE .03

// NOTE: This LennardJones function is coded to do many
// things based on an OpMask.  The goal here is to keep
// complex code implemented in one place.  If this implementation
// gets TOO COMPLEX, then we can split it out.

// NOTE: ALSO, it is best to be able to call this function
// with a constant OpMask at compile time.  This should
// allow the compiler compile out code that will not be used.
// It is expected that the caller will use a switch before
// invokation to allow the inlined call to have a constant
// OpMask at compile time.  In effect, this should give us
// the ability to get several tighly optimized versions of
// this function without duplicating code all over.  We'll see...

// Carefull thought should go into whether these should remain
// inlined - might be good to be able to easily control that somehow.
// DECISSIONS SHOULD BE MADE ONLY AFTER PERFORMANCE TESTS!

// NOTE: THERE ARE ENTRY POINTS FOR LJ FUNCTION THAT HAVE ALTERNATE SIGNATURES BELOW

inline
void
LennardJonesForce( unsigned                      aSimTick,
                   unsigned                      aOpMask,
                   double                        aLamda,
                   double                        aDLamda,
                   const LennardJonesParameters &aInitialLJPA,
                   const LennardJonesParameters &aInitialLJPB,
                   const LennardJonesParameters &aFinalLJPA,
                   const LennardJonesParameters &aFinalLJPB,
                   SitePosition                 &aSitePositionA,
                   SitePosition                 &aSitePositionB,
                   XYZ                          &aVectorAB,
                   double                        aDistanceAB,
                   ForceVector                  &aForceOnSiteA,
                   ForceVector                  &aForceOnSiteB,
                   double                       &aLennardJonesEnergy,
                   double                        aDHDLamda,
                   XYZ                          &aVirial               )
  {

#if 1

// JWP 2001
// have to mix initial and final state LJ params separately
//
  LennardJonesParameters aInitialLJPMix;
  LennardJonesParameters aFinalLJPMix;
//

  BegLogLine(0)
    << " Lamda LJ "
    << " aSimTick "             <<  aSimTick
    << " aOpMask "              <<  aOpMask
    << " aLamda "               <<  aLamda
    << " aDLamda "              <<  aDLamda
    << " aInitialLJPA "         <<  aInitialLJPA
    << " aInitialLJPB "         <<  aInitialLJPB
    << " aFinalLJPA "           <<  aFinalLJPA
    << " aFinalLJPB "           <<  aFinalLJPB
    << " aSitePositionA "       <<  aSitePositionA
    << " aSitePositionB "       <<  aSitePositionB
    << " aVectorAB "            <<  aVectorAB
    << " aDistanceAB "          <<  aDistanceAB
    << " aForceOnSiteA "        <<  aForceOnSiteA
    << " aForceOnSiteB "        <<  aForceOnSiteB
    << " aLennardJonesEnergy "  <<  aLennardJonesEnergy
    << " aDHDLamda "            <<  aDHDLamda
    << " aVirial "            <<  aVirial
    << EndLogLine;

  //******************************************************

  LennardJonesParameters aLJPMix;

  aLJPMix.mEpsilon      = sqrt( aInitialLJPA.mEpsilon * aInitialLJPB.mEpsilon );
  if( MSDIF::GetVDWCombine() == 0 )
    {     // CHARMM
    aLJPMix.mSigma        = aInitialLJPA.mSigma + aInitialLJPB.mSigma;
    }
  else if( MSDIF::GetVDWCombine() == 1 )
    { // OPLSAA
    // double rminA  = rminSigmaFactor * aInitialLJPA.mSigma;
    // double rminB  = rminSigmaFactor * aInitialLJPB.mSigma;
    // aLJPMix.mSigma        = 2.0*sqrt(rminA * rminB);
    aLJPMix.mSigma        = 2.0*sqrt(aInitialLJPA.mSigma * aInitialLJPB.mSigma);
    }
  else
    {
    assert( 0 );  // Unrecognized force field id in MolSysDef (.msd) file.
    }

  double tmp           = aLJPMix.mSigma / aDistanceAB;
  double tmp3          = tmp*tmp*tmp;
  double tmp6          = tmp3*tmp3;
  double tmp12         = tmp6*tmp6;
  double lje           = (aLJPMix.mEpsilon) * (tmp12 - 2.0*tmp6);
  aLennardJonesEnergy  = lje;


  double DEDR           =
                         (
                           (12 * aLJPMix.mEpsilon) *
                           (
                               (  tmp12)
                             - (  tmp6)
                           )
                         ) / (aDistanceAB * aDistanceAB) ;



  ForceVector    fab     = aVectorAB * DEDR;
  aForceOnSiteA          = fab;
  aForceOnSiteB          = fab * (-1);

  BegLogLine(PKFXLOG_FORCE_TERMS)
    << "FORCE LJ     "
    << aSimTick
    << " Energy " << lje
    << " PA " << aSitePositionA
    << " PB " << aSitePositionB
    << " FA " << aForceOnSiteA
    << EndLogLine;

  //******************************************************
#else

  //  JWP 2001
  //  mostly copied from the simple LJ force below
  //  includes mixing rules

  //  convention: lambda = 0 implies initial state
  //  convention: lambda = 1 implies final state

  //  H(lam) = (1.0 - lam)*HInitial + lam*HFinal;

  // Always do the 'Initial' LJ parm mix - these are the normal LJ parms
  // when not doing free energy - eg, Lamda = 0.

  // NOTE:  THIS FORCE FIELD DEPENDANT COMBINING RULE STUFF TO COME OUT.

  aInitialLJPMix.mEpsilon = sqrt( aInitialLJPA.mEpsilon * aInitialLJPB.mEpsilon );

  if( MSDIF::GetVDWCombine() == 0 )
    {     // CHARMM
    aInitialLJPMix.mSigma        = aInitialLJPA.mSigma + aInitialLJPB.mSigma;
    }
  else if( MSDIF::GetVDWCombine() == 1 )
    { // OPLSAA
    aInitialLJPMix.mSigma        = 2.0*sqrt(aInitialLJPA.mSigma * aInitialLJPB.mSigma);
    }
  else
    {
    assert( 0 );  // Unrecognized force field id in MolSysDef (.msd) file.
    }

// NOTE (JWP): LJ and Coulomb need separate enums?
// NOTE (JWP): we will want to be able to change LJ w/o Coulomb changing
// NOTE (JWP): and vice versa

  if( aOpMask & NonbondedOperationBitEnum::FreeEnergy )
    {
    aFinalLJPMix.mEpsilon = sqrt( aFinalLJPA.mEpsilon * aFinalLJPB.mEpsilon );

    if( MSDIF::GetVDWCombine() == 0 )
      {     // CHARMM
      aFinalLJPMix.mSigma          = aFinalLJPA.mSigma + aFinalLJPB.mSigma;
      }
    else if( MSDIF::GetVDWCombine() == 1 )
      { // OPLSAA
      aFinalLJPMix.mSigma        = 2.0*sqrt(aFinalLJPA.mSigma * aFinalLJPB.mSigma);
      }
    }

  // Setup Initial and Final Lamda factors - adjust if doing free energy.
  double InitialLam = 1.0;
  double FinalLam   = 0.0;
  if( aOpMask & NonbondedOperationBitEnum::FreeEnergy )
    {
    InitialLam -=  aLamda;
    FinalLam   = aLamda;
    }

  // tmp[I,F]* hold powers of the mixed sigma*(2.0)**1/6
  // sigma[I,F]6 is actually sigma**6
  // distAB[4,6] are 4th & 6th powers of true interparticle distance
  // dist[I,F]* are inverse of distance + soft-core scaling
  // rep[I,F] is repulsive vdW term
  // disp[I,F] is attractive vdW term
  //
  // note that the soft-core parameter for the Initial state is
  //              FinalLam*FinalLam*sigma**6
  // & vice versa

  double tmpI           = aInitialLJPMix.mSigma;
  double tmpI6          = pow(tmpI,6);
  double sigmaI6        = tmpI6/2.0;
  double tmpI12         = tmpI6*tmpI6;
  tmpI6                *= 2.0;
  double distAB6        = pow(aDistanceAB,6);
  double distI6         = 1.0 / ( distAB6 + FinalLam * FinalLam * sigmaI6);
  double distI12        = distI6 * distI6;
  double repI           = ( aInitialLJPMix.mEpsilon) * ( tmpI12 * distI12 );
  double dispI          = (-aInitialLJPMix.mEpsilon) * ( tmpI6  * distI6  );
  double ljeI           = repI + dispI;

  // assuming DEDR = -1.0 * deriv / r
  // i.e. gets multiplied by VectorAB to get the forces

  double distAB4 = pow(aDistanceAB,4);

  // or: double distAB4 = distAB6 / (aDistanceAB * aDistanceAB);
  // take your pick.  JWP

  double DEDR  = InitialLam *
                   (6.0 * (2.0 * repI - dispI) * distI6 *
                   distAB4) ;

  aLennardJonesEnergy  = InitialLam * ljeI;

  if( aOpMask & NonbondedOperationBitEnum::FreeEnergy )
    {
    double tmpF           = aFinalLJPMix.mSigma;
    double tmpF6          = pow(tmpF,6);
    double sigmaF6        = tmpF6/2.0;
    double tmpF12         = tmpF6*tmpF6;
    tmpF6                 *= 2.0;
    double distF6         = 1.0 / ( distAB6 + InitialLam*InitialLam*sigmaF6);
    double distF12        = distF6*distF6;
    double repF           = (aFinalLJPMix.mEpsilon) * (tmpF12*distF12);
    double dispF          = -(aFinalLJPMix.mEpsilon) * (tmpF6*distF6);
    double ljeF           = repF + dispF;

    aLennardJonesEnergy  += FinalLam * ljeF;

    DEDR +=      FinalLam *
                   (6.0 * (2.0 * repF - dispF) * distF6 *
                   distAB4);

    // NOTE: (JWP) checked and amended 26 Apr 01
    // fixed sign, lamda & factor 2.0 errors

    aDHDLamda =
                          ljeF - ljeI + 2.0 * InitialLam * FinalLam *
                          ( sigmaF6 * distF6 * ( 2.0 * repF - dispF )
                           -sigmaI6 * distI6 * ( 2.0 * repI - dispI ));
    }

  // NEED FUNCTIONAL FORM FOR THIS
  aVirial.Zero();

  ForceVector    fab     = aVectorAB * DEDR;
  aForceOnSiteA          = fab;
  aForceOnSiteB          = fab * (-1);

  BegLogLine(PKFXLOG_FORCE_TERMS)
    << "FORCE LJ     "
    << aSimTick
    << " Energy "   << aLennardJonesEnergy
    << " PA "       << aSitePositionA
    << " PB "       << aSitePositionB
    << " FA "       << aForceOnSiteA
    << " DHDLamda " << aDHDLamda
    << " Virial " << aVirial
    << EndLogLine;

// JWP 2001
#endif

  return;
  }



// A non-free energy entry point - this is mostly short hand.
inline
void
LennardJonesForce( unsigned                      aSimTick,
                   unsigned                      aOpMask,
                   const LennardJonesParameters &aLJPA,
                   const LennardJonesParameters &aLJPB,
                   SitePosition                 &aSitePositionA,
                   SitePosition                 &aSitePositionB,
                   XYZ                          &aVectorAB,
                   double                        aDistanceAB,
                   ForceVector                  &aForceOnSiteA,
                   ForceVector                  &aForceOnSiteB,
                   double                       &aLennardJonesEnergy,
                   XYZ                          &aVirial               )
  {
  assert( ! ( aOpMask & NonbondedOperationBitEnum::FreeEnergy ) );
  double Dummy_Lamda;
  double Dummy_DLamda;
  double Dummy_VdwDHDLamda;
  LennardJonesForce( aSimTick,
                     aOpMask & ~NonbondedOperationBitEnum::FreeEnergy,
                     Dummy_Lamda,
                     Dummy_DLamda,
                     aLJPA,
                     aLJPB,
                     aLJPA, // Dummy - Just use these references - inlining should disolve
                     aLJPB, // Dummy
                     aSitePositionA,
                     aSitePositionB,
                     aVectorAB,
                     aDistanceAB,
                     aForceOnSiteA,
                     aForceOnSiteB,
                     aLennardJonesEnergy,
                     Dummy_VdwDHDLamda,
                     aVirial              );
  }

// OLD STYLE SIG FOR INTRAMOLECULAR VM TO USE - THIS COMES OUT!
inline
void
LennardJonesForce( unsigned                      aSimTick,
                   const LennardJonesParameters &aLJPA,
                   const LennardJonesParameters &aLJPB,
                   SitePosition                 &aSitePositionA,
                   SitePosition                 &aSitePositionB,
                   ForceVector                  &aForceOnSiteA,
                   ForceVector                  &aForceOnSiteB,
                   double                       &aLennardJonesEnergy )
  {

  XYZ    VectorAB       = aSitePositionA - aSitePositionB;
  double DistanceAB     = VectorAB.Length();

  double Dummy_Lamda;
  double Dummy_DLamda;
  double Dummy_VdwDHDLamda;
  XYZ    Dummy_Virial;

  LennardJonesForce( aSimTick,
                     NonbondedOperationBitEnum::LJBasic,
                     Dummy_Lamda,
                     Dummy_DLamda,
                     aLJPA,
                     aLJPB,
                     aLJPA, // Dummy - Just use these references - inlining should disolve
                     aLJPB, // Dummy
                     aSitePositionA,
                     aSitePositionB,
                     VectorAB,
                     DistanceAB,
                     aForceOnSiteA,
                     aForceOnSiteB,
                     aLennardJonesEnergy,
                     Dummy_VdwDHDLamda,
                     Dummy_Virial              );
  }


//=============================================================================

inline
void
ChargeForce( unsigned             &aSimTick,
             unsigned             &aOpMask,
             double               &aLamda,
             double               &aDLamda,
             const double         &aInitialChargeA,
             const double         &aInitialChargeB,
             const double         &aFinalChargeA,
             const double         &aFinalChargeB,
             ChargePosition       &aChargePositionA,
             ChargePosition       &aChargePositionB,
             XYZ                  &aVectorAB,
             double               &aDistanceAB,
             ForceVector          &aForceOnChargeA,
             ForceVector          &aForceOnChargeB,
             double               &aChargeEnergy,
             double               &aDHDLamda,
             double               &aVirial               )
  {


  BegLogLine(1)
    <<  "Lamda Charge "
    <<  " aSimTick "           <<     aSimTick
    <<  " aOpMask "            <<     aOpMask
    <<  " aLamda "             <<     aLamda
    <<  " aDLamda "            <<     aDLamda
    <<  " aInitialChargeA "    <<     aInitialChargeA
    <<  " aInitialChargeB "    <<     aInitialChargeB
    <<  " aFinalChargeA "      <<     aFinalChargeA
    <<  " aFinalChargeB "      <<     aFinalChargeB
    <<  " aChargePositionA "   <<     aChargePositionA
    <<  " aChargePositionB "   <<     aChargePositionB
    <<  " aVectorAB "          <<     aVectorAB
    <<  " aDistanceAB "        <<     aDistanceAB
    <<  " aForceOnChargeA "    <<     aForceOnChargeA
    <<  " aForceOnChargeB "    <<     aForceOnChargeB
    <<  " aChargeEnergy "      <<     aChargeEnergy
    <<  " aDHDLamda "          <<     aDHDLamda
    <<  " aVirial "          <<     aVirial
    << EndLogLine;

// JWP 2001
//
// lambda-dependent electrostatic interactions
// linear dependence for now
//
// convention: lambda = 0.0 is initial state
// convention: lambda = 1.0 is final state
//
  double InitialLam = 1.0 - aLamda;
  double FinalLam = aLamda;
//
// H(lam) = (1.0 - lam) * HInitial + lam * HFinal
//
// check cutoff first
//

  if(aDistanceAB >= CHARGECUTOFF)
    {
      aChargeEnergy = 0.0;
      aForceOnChargeA.Zero();
      aForceOnChargeB.Zero();
      return;
    }

#ifdef CHECK_EVERY_DAMN_DISTANCE
// don't do this for perturbed atoms since overlaps are OK
#endif

  double caI             = (aInitialChargeA * aInitialChargeB) / aDistanceAB ;
  double caF             = (aFinalChargeA * aFinalChargeB) / aDistanceAB ;

  aChargeEnergy         = InitialLam*caI + FinalLam*caF ;  // assign energy to return location

  double DEDR           = InitialLam*caI / ( aDistanceAB * aDistanceAB )
                         +  FinalLam*caF / ( aDistanceAB * aDistanceAB );

  aDHDLamda             = caF - caI;

  ForceVector fab       = aVectorAB * DEDR;

  aForceOnChargeA       = fab;
  aForceOnChargeB       = fab * (-1);


  return;
  }

inline
void
ChargeForce( unsigned             &aSimTick,
             const double         &aChargeA,
             const double         &aChargeB,
             ChargePosition       &aChargePositionA,
             ChargePosition       &aChargePositionB,
             XYZ                  &aVectorAB,
             double               &aDistanceAB,
             ForceVector          &aForceOnChargeA,
             ForceVector          &aForceOnChargeB,
             double               &aChargeEnergy )
  {

  if(aDistanceAB >= CHARGECUTOFF)
    {
      aChargeEnergy = 0.0;
      aForceOnChargeA.Zero();
      aForceOnChargeB.Zero();
      return;
    }

#if CHECK_EVERY_DAMN_DISTANCE
  if( aDistanceAB < TOO_DAMN_CLOSE )
  {
    BegLogLine(1)
    << "TOO DAMN CLOSE!!! at SimTick  "
    << aSimTick
    << " PA " << aChargePositionA
    << " PB " << aChargePositionB
    << " distance " << aDistanceAB
    << EndLogLine;
    assert( aDistanceAB < TOO_DAMN_CLOSE );
  }
#endif
  double ca             = (aChargeA * aChargeB) / aDistanceAB ;  // assign energy to return location
  aChargeEnergy         = ca ;  // assign energy to return location

  double DEDR           = (aChargeA * aChargeB) / ( aDistanceAB * aDistanceAB * aDistanceAB );

  ForceVector fab       = aVectorAB * DEDR;

  aForceOnChargeA       = fab;
  aForceOnChargeB       = fab * (-1);

#if 0

  BegLogLine(PKFXLOG_FORCE_TERMS)
    << "FORCE Charge "
    << aSimTick
    << " Energy " << ca
    << " PA " << aChargePositionA
    << " PB " << aChargePositionB
    << " FA " << aForceOnChargeA
    << EndLogLine;

#endif

  }



inline
void
ChargeForce( unsigned             &aSimTick,
             const double         &aChargeA,
             const double         &aChargeB,
             ChargePosition       &aChargePositionA,
             ChargePosition       &aChargePositionB,
             ForceVector          &aForceOnChargeA,
             ForceVector          &aForceOnChargeB,
             double               &aChargeEnergy )
  {

//////  assert(0); // Phase out mon.
  XYZ VectorAB          = aChargePositionA - aChargePositionB;

  double DistanceAB     = VectorAB.Length();

  ChargeForce( aSimTick,
               aChargeA,
               aChargeB,
               aChargePositionA,
               aChargePositionB,
               VectorAB,
               DistanceAB,
               aForceOnChargeA,
               aForceOnChargeB,
               aChargeEnergy );
  }


#if 0 // THINK ABOUT FOLDING THIS INTO ABOVE FX

inline
void
EwaldRealSpaceChargeForce
(            unsigned                     &aSimTick,
             double                     &alpha,
             const double               &aChargeA,
             const double               &aChargeB,
             ChargePosition       &aChargePositionA,
             ChargePosition       &aChargePositionB,
             ForceVector          &aForceOnChargeA,
             ForceVector          &aForceOnChargeB,
             double               &aChargeEnergy )
  {

  XYZ VectorAB          = aChargePositionA - aChargePositionB;

  double DistanceAB     = VectorAB.Length();
  double DistanceAB2    = DistanceAB * DistanceAB;
  double DistanceAB3    = DistanceAB * DistanceAB2;

  if(DistanceAB >= CHARGECUTOFF)
    {
      aChargeEnergy = 0.0;
      aForceOnChargeA.Zero();
      aForceOnChargeB.Zero();
      return;
    }

#if CHECK_EVERY_DAMN_DISTANCE
  if( DistanceAB < TOO_DAMN_CLOSE )
  {
    BegLogLine(1)
    << "TOO DAMN CLOSE!!! at SimTick  "
    << aSimTick
    << " PA " << aChargePositionA
    << " PB " << aChargePositionB
    << " distance " << DistanceAB
    << EndLogLine;
    assert( DistanceAB < TOO_DAMN_CLOSE );
  }
#endif
  double erfcAlphaR = myerfc(alpha * DistanceAB);

  double ca = (aChargeA * aChargeB) / DistanceAB * erfcAlphaR;
  aChargeEnergy = ca ;  // assign energy to return location

  double DEDR = (aChargeA * aChargeB) * ( erfcAlphaR / DistanceAB3
              + 2.0 * alpha / sqrtPI * exp(-alpha * alpha * DistanceAB2) / DistanceAB2);


  ForceVector fab  = VectorAB * DEDR;

  aForceOnChargeA  = fab;
  aForceOnChargeB  = fab * (-1);

#if 0

  BegLogLine(PKFXLOG_FORCE_TERMS)
    << "FORCE Charge "
    << aSimTick
    << " Energy " << ca
    << " PA " << aChargePositionA
    << " PB " << aChargePositionB
    << " FA " << aForceOnChargeA
    << EndLogLine;

#endif

  }
#endif
//=============================================================================

inline
void
BondForce( unsigned                     &aSimTick,
           unsigned                     &aOpMask,
           double                       &aLamda,
           double                       &aDLamda,
           const BondForceParameters    &aInitialBFP,
           const BondForceParameters    &aFinalBFP,
           SitePosition                 &aSitePositionA,
           SitePosition                 &aSitePositionB,
           ForceVector                  &aForceOnSiteA,
           ForceVector                  &aForceOnSiteB,
           double                       &aBondEnergy,
           double                       &aDHDLamda,
           double                       &aVirial               )
  {
  return;
  }

inline
void
BondForce( unsigned                     &aSimTick,
           const BondForceParameters    &aBFP,
           SitePosition                 &aSitePositionA,
           SitePosition                 &aSitePositionB,
           ForceVector                  &aForceOnSiteA,
           ForceVector                  &aForceOnSiteB,
           double                       &aBondEnergy )
  {

  XYZ VectorAB          = aSitePositionA - aSitePositionB;

  double DistanceAB            = VectorAB.Length();

#ifdef CHECK_EVERY_DAMN_DISTANCE

  if(DistanceAB < TOO_DAMN_CLOSE)
  {
    BegLogLine(1)
    << "TOO DAMN CLOSE!!! at SimTick  "
    << aSimTick
    << " PA " << aSitePositionA
    << " PB " << aSitePositionB
    << " distance " << DistanceAB
    << EndLogLine;
    assert(DistanceAB > TOO_DAMN_CLOSE);
  }
#endif

  double DeltaFromEquilibrium  = DistanceAB - aBFP.mEquilibriumDistance; // difference between eq len and actual len

  aBondEnergy           =    aBFP.mSpringConstant
                            * DeltaFromEquilibrium * DeltaFromEquilibrium;    // assign energy to return location

  double DEDR           = -2.0 * (aBFP.mSpringConstant * DeltaFromEquilibrium) / DistanceAB;  // minus the derivative of the energy divided by distance

  aForceOnSiteA          =    VectorAB * DEDR;
  aForceOnSiteB          =   (VectorAB * DEDR) * (-1);

  if(aBondEnergy < MAX_ENERGY_PER_TERM)
     {
       assert(aBondEnergy < MAX_ENERGY_PER_TERM);
     }

#if 1

  BegLogLine(PKFXLOG_FORCE_TERMS)
    << "FORCE BOND "
    << " Energy " << aBondEnergy
    << " FA " << aSitePositionA
    << " FB " << aSitePositionB
    << " FA " << aForceOnSiteA
    << " dR " << DeltaFromEquilibrium
    << EndLogLine;
#endif

  }

//=============================================================================

inline
void
AngleForce( unsigned                     &aSimTick,
            unsigned                     &aOpMask,
            double                       &aLamda,
            double                       &aDLamda,
            const AngleForceParameters   &aInitialAFP,
            const AngleForceParameters   &aFinalAFP,
            SitePosition                 &aSitePositionB,
            SitePosition                 &aSitePositionA,  // YO YO YO THIS IS THE CENTER FOR CHARM
            SitePosition                 &aSitePositionC,
            ForceVector                  &aForceOnSiteB,
            ForceVector                  &aForceOnSiteA,  // YO YO YO THIS IS THE CENTER FOR CHARM
            ForceVector                  &aForceOnSiteC,
            double                       &aAngleEnergy,
            double                       &aDHDLamda,
            double                       &aVirial               )
  {
  }

inline
void
AngleForce( unsigned                     &aSimTick,
            const AngleForceParameters   &aAFP,
            SitePosition                 &aSitePositionB,
            SitePosition                 &aSitePositionA,  // YO YO YO THIS IS THE CENTER FOR CHARM
            SitePosition                 &aSitePositionC,
            ForceVector                  &aForceOnSiteB,
            ForceVector                  &aForceOnSiteA,  // YO YO YO THIS IS THE CENTER FOR CHARM
            ForceVector                  &aForceOnSiteC,
            double                       &aAngleEnergy )
  {

  XYZ    VectorAB    = aSitePositionA - aSitePositionB;
  double DistanceAB  = VectorAB.Length();

#ifdef CHECK_EVERY_DAMN_DISTANCE
  if(DistanceAB < TOO_DAMN_CLOSE)
  {
    BegLogLine(1)
    << "TOO DAMN CLOSE!!! at SimTick  "
    << aSimTick
    << " PA " << aSitePositionA
    << " PB " << aSitePositionB
    << " distance " << DistanceAB
    << EndLogLine;
    assert(DistanceAB > TOO_DAMN_CLOSE);
  }
#endif
  XYZ    VectorAC    = aSitePositionA - aSitePositionC;
  double DistanceAC  = VectorAC.Length();

#ifdef CHECK_EVERY_DAMN_DISTANCE
  if(DistanceAC < TOO_DAMN_CLOSE)
  {
    BegLogLine(1)
    << "TOO DAMN CLOSE!!! at SimTick  "
    << aSimTick
    << " PA " << aSitePositionA
    << " PB " << aSitePositionB
    << " distance " << DistanceAB
    << EndLogLine;
    assert(DistanceAC > TOO_DAMN_CLOSE);
  }
#endif

  //DETERMINE COS(THETA) = R12 DOT R32 / SQRT((R12**2)*(R32**2))

  double COSTHE = ( VectorAB.DotProduct( VectorAC ) ) / (DistanceAB * DistanceAC);

  //   DETERMINE THETA.  IT IS IN THE RANGE OF 0 TO PI RADIANS.

  double THETA = acos(COSTHE);

  aAngleEnergy = aAFP.mSpringConstant *
                         (THETA - aAFP.mEquilibriumAngle) * (THETA- aAFP.mEquilibriumAngle);

  double DEDTH = 2. * aAFP.mSpringConstant * (THETA - aAFP.mEquilibriumAngle);

  // COMPUTE AND NORMALIZE R(PERP) TO FACILITATE FORCE EVALUATION
  // R(PERP) = R32 CROSS R12

  XYZ RP;
  RP.mX = VectorAC.mY * VectorAB.mZ - VectorAC.mZ * VectorAB.mY;
  RP.mY = VectorAC.mZ * VectorAB.mX - VectorAC.mX * VectorAB.mZ;
  RP.mZ = VectorAC.mX * VectorAB.mY - VectorAC.mY * VectorAB.mX;

  // RPERP WILL NOT NORMALIZE IF 1-2-3 COLLINEAR; I.E., THETA=0 OR 180
  // FORCE CAN BE LEGITIMATELY SET TO ZERO IN THIS CASE.

  double RPL = RP.Length();
  if( RPL > 0.0000000001 )
    RPL = 1.0 / RPL;
  else
    RPL=0.0;

  //
  //    COMPUTE GRADIENTS OF ENERGY WITH RESPECT TO COMPONENTS OF VectorAB,RO2
  //
  double    R12R = -1.0 /  VectorAB.DotProduct(VectorAB);
  double    R32R =  1.0 /  VectorAC.DotProduct(VectorAC);
  double    DTD12X = R12R*(RPL*DEDTH) * (VectorAB.mY*RP.mZ - VectorAB.mZ*RP.mY);
  double    DTD12Y = R12R*(RPL*DEDTH) * (VectorAB.mZ*RP.mX - VectorAB.mX*RP.mZ);
  double    DTD12Z = R12R*(RPL*DEDTH) * (VectorAB.mX*RP.mY - VectorAB.mY*RP.mX);
  double    DTD32X = R32R*(RPL*DEDTH) * (VectorAC.mY*RP.mZ - VectorAC.mZ*RP.mY);
  double    DTD32Y = R32R*(RPL*DEDTH) * (VectorAC.mZ*RP.mX - VectorAC.mX*RP.mZ);
  double    DTD32Z = R32R*(RPL*DEDTH) * (VectorAC.mX*RP.mY - VectorAC.mY*RP.mX);
  //
  // COMPUTE FORCES DUE TO ANGLE ENERGY ON O, H1 AND H2
  //
  aForceOnSiteB.mX =  DTD12X        ;
  aForceOnSiteB.mY =  DTD12Y        ;
  aForceOnSiteB.mZ =  DTD12Z        ;
  aForceOnSiteA.mX = -DTD12X-DTD32X ;
  aForceOnSiteA.mY = -DTD12Y-DTD32Y ;
  aForceOnSiteA.mZ = -DTD12Z-DTD32Z ;
  aForceOnSiteC.mX =         DTD32X ;
  aForceOnSiteC.mY =         DTD32Y ;
  aForceOnSiteC.mZ =         DTD32Z ;

#if 0

  BegLogLine(PKFXLOG_FORCE_TERMS)
    << "FORCE BOND "
    << " Energy " << aAngleEnergy
    << " FA " << aSitePositionA
    << " FB " << aSitePositionB
    << " FC " << aSitePositionC
    << " FA " << aForceOnSiteA
    << EndLogLine;

#endif

  }

//=============================================================================

inline
void
UBForce(    unsigned                   &aSimTick,
            unsigned                   &aOpMask,
            double                     &aLamda,
            double                     &aDLamda,
            const AngleForceParameters &aInitialAFP,
            const AngleForceParameters &aFinalAFP,
            SitePosition               &aSitePositionB,
            SitePosition               &aSitePositionA,  // YO YO YO THIS IS THE CENTER FOR CHARM
            SitePosition               &aSitePositionC,
            ForceVector                &aForceOnSiteB,
            ForceVector                &aForceOnSiteA,  // YO YO YO THIS IS THE CENTER FOR CHARM
            ForceVector                &aForceOnSiteC,
            double                     &aUBEnergy,
            double                     &aDHDLamda,
            double                     &aVirial               )
  {
  }

inline
void
UBForce(  unsigned                     &aSimTick,
            const AngleForceParameters &aAFP,
            SitePosition               &aSitePositionB,
            SitePosition               &aSitePositionA,  // YO YO YO THIS IS THE CENTER FOR CHARM
            SitePosition               &aSitePositionC,
            ForceVector                &aForceOnSiteB,
            ForceVector                &aForceOnSiteA,  // YO YO YO THIS IS THE CENTER FOR CHARM
            ForceVector                &aForceOnSiteC,
            double                     &aUBEnergy )
  {

    // Urey-Bradley Force term between 1,3
    // UB term shares the same AngleType for Force Parameters

    XYZ    VectorBC    = aSitePositionB - aSitePositionC;
    double DistanceBC  = VectorBC.Length();

#ifdef CHECK_EVERY_DAMN_DISTANCE
    if(DistanceBC < TOO_DAMN_CLOSE)
      {
        BegLogLine(1)
          << "TOO DAMN CLOSE!!! at SimTick  "
          << aSimTick
          << " PB " << aSitePositionB
          << " PC " << aSitePositionC
          << " distance " << DistanceBC
          << EndLogLine;
        assert(DistanceBC > TOO_DAMN_CLOSE);
      }
#endif

    double DeltaUB  = DistanceBC - aAFP.mEquilibriumAngle; // UB shares the same AngleType with angle

    aUBEnergy = aAFP.mSpringConstant * DeltaUB * DeltaUB;

    double DEDR           = -2.0 * (aAFP.mSpringConstant * DeltaUB) / DistanceBC;

    aForceOnSiteA.mX = 0.0;
    aForceOnSiteA.mY = 0.0;
    aForceOnSiteA.mZ = 0.0;
    aForceOnSiteB          =    VectorBC * DEDR;
    aForceOnSiteC          =   (VectorBC * DEDR) * (-1);

    if(aUBEnergy < MAX_ENERGY_PER_TERM)
      {
        assert(aUBEnergy < MAX_ENERGY_PER_TERM);
      }

#if 1

    BegLogLine(PKFXLOG_FORCE_TERMS)
      << "FORCE Urey-Bradley "
      << " Energy " << aUBEnergy
      << " PB " << aSitePositionB
      << " PC " << aSitePositionC
      << " FA " << aForceOnSiteA
      << " FB " << aForceOnSiteB
      << " FC " << aForceOnSiteC
      << " UBK " << aAFP.mSpringConstant
      << " UBdR " << DeltaUB
      << EndLogLine;
#endif


  }

inline
void
TorsionForce(  unsigned                     &aSimTick,
            unsigned                        &aOpMask,
            double                          &aLamda,
            double                          &aDLamda,
            const  TorsionForceParameters   &aInitialTFP,
            const  TorsionForceParameters   &aFinalTFP,
            SitePosition                    &aSitePosition1,
            SitePosition                    &aSitePosition2,
            SitePosition                    &aSitePosition3,
            SitePosition                    &aSitePosition4,
            ForceVector                     &aForceOnSite1,
            ForceVector                     &aForceOnSite2,
            ForceVector                     &aForceOnSite3,
            ForceVector                     &aForceOnSite4,
            double                          &aTorsionEnergy,
            double                          &aDHDLamda,
            double                          &aVirial               )
  {
  }

inline
void
TorsionForce(  unsigned                     &aSimTick,
            const  TorsionForceParameters   &aTFP,
            SitePosition                    &aSitePosition1,
            SitePosition                    &aSitePosition2,
            SitePosition                    &aSitePosition3,
            SitePosition                    &aSitePosition4,
            ForceVector                     &aForceOnSite1,
            ForceVector                     &aForceOnSite2,
            ForceVector                     &aForceOnSite3,
            ForceVector                     &aForceOnSite4,
            double                          &aTorsionEnergy )
  {
    CartesianVector dVec2 = aSitePosition2-aSitePosition1,
      dVec3 = aSitePosition3 - aSitePosition2,
      dVec4 = aSitePosition4 - aSitePosition3;

    double CCC22 = dVec2*dVec2,
      CCC23 = dVec2*dVec3,
      CCC24 = dVec2*dVec4,
      CCC33 = dVec3*dVec3,
      CCC34 = dVec3*dVec4,
      CCC44 = dVec4*dVec4;
    double DDD23 = CCC22*CCC33 - CCC23*CCC23,
      DDD34 = CCC33*CCC44 - CCC34*CCC34;

    // collinear situation
    if (is_almost_zero(DDD23) || is_almost_zero(DDD34))
      return;

    double oneOverSqrtDDD34timesDDD23 = 1.0/sqrt(DDD23*DDD34);

    CartesianVector gradientCosDihedral_1 = -oneOverSqrtDDD34timesDDD23*
      (-CCC34*dVec3 + CCC33*dVec4 -
       1.0/DDD23*(CCC34*CCC23 - CCC24*CCC33)*(CCC23*dVec3 - CCC33*dVec2));

    CartesianVector gradientCosDihedral_2 = -oneOverSqrtDDD34timesDDD23*
      (-(CCC23+CCC33)*dVec4 + (CCC34+2.0*CCC24)*dVec3 - CCC34*dVec2 -
       1.0/DDD23*(CCC34*CCC23 - CCC24*CCC33)*((CCC33+CCC23)*dVec2 - (CCC22+CCC23)*dVec3) -
       1.0/DDD34*(CCC34*CCC23 - CCC24*CCC33)*(-CCC44*dVec3 + CCC34*dVec4));

    CartesianVector gradientCosDihedral_3 = -oneOverSqrtDDD34timesDDD23*
      (CCC23*dVec4 - (CCC23+2.0*CCC24)*dVec3 + (CCC34+CCC33)*dVec2 -
       1.0/DDD23*(CCC34*CCC23 - CCC24*CCC33)*(CCC22*dVec3 - CCC23*dVec2) -
       1.0/DDD34*(CCC34*CCC23 - CCC24*CCC33)*((CCC44+CCC34)*dVec3 - (CCC33+CCC34)*dVec4));

    CartesianVector gradientCosDihedral_4 = -oneOverSqrtDDD34timesDDD23*
      (CCC23*dVec3 - CCC33*dVec2 -
       1.0/DDD34*(CCC34*CCC23 - CCC24*CCC33)*(CCC33*dVec4 - CCC34*dVec3));

    double DEnergyOverDcosTheta;
    double cosDelta = cos(aTFP.delta), sinDelta = sin(aTFP.delta);
    // For most force field, sinDelta=0, for convenience we drop the
    // terms with sinDelta to avoid the singularity from division by
    // sinTheta. Bill Swope has an alternative way of computing the
    // torsional forces. too lazy to implement it now.
    CartesianVector dVec4CrossdVec3 = (dVec4.crossProduct(dVec3)),
      dVec3CrossdVec2 = dVec3.crossProduct(dVec2);
    double dVec4CrossdVec3MagtimesdVec3CrossdVec2Mag
      = sqrt(sqr(dVec4CrossdVec3)*sqr(dVec3CrossdVec2));
    double cosTheta = -(dVec4CrossdVec3*dVec3CrossdVec2)/dVec4CrossdVec3MagtimesdVec3CrossdVec2Mag;
    double sinTheta = (dVec4CrossdVec3.crossProduct(dVec3CrossdVec2)*dVec3)/
      (dVec4CrossdVec3MagtimesdVec3CrossdVec2Mag*sqrt(sqr(dVec3)));

    // split the following part into different functions. get rid of the switch block...
    switch (aTFP.multiplicity)
      {
      case 1:
        DEnergyOverDcosTheta = aTFP.k0*(cosDelta); // - sinDelta*cosTheta/sinTheta);
        aTorsionEnergy = aTFP.k0*(1 + cosDelta*cosTheta); // + sinDelta*sinTheta);
        break;
      case 2:
        DEnergyOverDcosTheta = 2.0*aTFP.k0*(2.0*cosDelta*cosTheta); // + sinDelta*(1-2.0*sqr(cosTheta))/sinTheta);
        aTorsionEnergy = aTFP.k0*(1 + cosDelta*(2.0*sqr(cosTheta)-1)); // + 2.0*sinDelta*sinTheta*cosTheta);
        break;
      case 3:
        DEnergyOverDcosTheta = 3.0*aTFP.k0*(cosDelta*(4.0*sqr(cosTheta)-1)); // + 4.0*sinDelta*cosTheta*(1-cosTheta)/sinTheta);
        aTorsionEnergy = aTFP.k0*(1 + cosDelta*(4.0*cube(cosTheta) - 3.0*cosTheta)); // + sinDelta*(4.0*sqr(cosTheta)-1)*sinTheta);
        break;
      case 4:
        DEnergyOverDcosTheta = 4.0*aTFP.k0*(cosDelta*(8.0*cube(cosTheta)-4.0*cosTheta)); // + sinDelta*(8.0*sqr(cosTheta)-8.0*sqr(sqr(cosTheta))-1)/sinTheta);
        aTorsionEnergy = aTFP.k0*(1 + cosDelta*(8.0*sqr(sqr(cosTheta))-8.0*sqr(cosTheta)+1)); // + sinDelta*4.0*(2.0*cube(cosTheta)-cosTheta)*sinTheta);
        break;
      }
    aForceOnSite1 = -DEnergyOverDcosTheta*gradientCosDihedral_1;
    aForceOnSite2 = -DEnergyOverDcosTheta*gradientCosDihedral_2;
    aForceOnSite3 = -DEnergyOverDcosTheta*gradientCosDihedral_3;
    aForceOnSite4 = -DEnergyOverDcosTheta*gradientCosDihedral_4;

  }

// bill swope's method for computing torsional forces
inline
void
SwopeTorsionForce( unsigned                     &aSimTick,
                   unsigned                     &aOpMask,
                   double                       &aLamda,
                   double                       &aDLamda,
                   const TorsionForceParameters &aInitialTFP,
                   const TorsionForceParameters &aFinalTFP,
                   SitePosition                 &aSitePosition1,
                   SitePosition                 &aSitePosition2,
                   SitePosition                 &aSitePosition3,
                   SitePosition                 &aSitePosition4,
                   ForceVector                  &aForceOnSite1,
                   ForceVector                  &aForceOnSite2,
                   ForceVector                  &aForceOnSite3,
                   ForceVector                  &aForceOnSite4,
                   double                       &aTorsionEnergy,
                   double                       &aDHDLamda,
                   double                       &aVirial               )
  {
  }


// bill swope's method for computing torsional forces
inline
void
SwopeTorsionForce( unsigned                     &aSimTick,
                   const TorsionForceParameters &aTFP,
                   SitePosition                 &aSitePosition1,
                   SitePosition                 &aSitePosition2,
                   SitePosition                 &aSitePosition3,
                   SitePosition                 &aSitePosition4,
                   ForceVector                  &aForceOnSite1,
                   ForceVector                  &aForceOnSite2,
                   ForceVector                  &aForceOnSite3,
                   ForceVector                  &aForceOnSite4,
                   double                       &aTorsionEnergy )
  {

  CartesianVector dVec43 = aSitePosition4 - aSitePosition3,
    dVec23 = aSitePosition2 - aSitePosition3,
    dVec12 = aSitePosition1 - aSitePosition2;
  CartesianVector dVecT = dVec43.crossProduct(dVec23),
    dVecU = dVec12.crossProduct(dVec23);
  double dVecTMag = sqrt(sqr(dVecT)),
    dVecUMag = sqrt(sqr(dVecU));

  // collinear situation
  if (is_almost_zero(dVecTMag) || is_almost_zero(dVecUMag))
    return;

//    if(aTFP.multiplicity == 6 || (is_almost_zero(aTFP.k0)))
//      {
//        aForceOnSite1.Zero();
//        aForceOnSite2.Zero();
//        aForceOnSite3.Zero();
//        aForceOnSite4.Zero();
//        return;
//      }

  CartesianVector dVec23Unit = dVec23/sqrt(sqr(dVec23)),
    dVecTUnit = dVecT/dVecTMag,
    dVecUUnit = dVecU/dVecUMag;
  CartesianVector gradientPhiOverdVecT =
    1.0/sqr(dVecT)*(dVecT.crossProduct(dVec23Unit)),
    gradientPhiOverdVecU =
    -1.0/sqr(dVecU)*(dVecU.crossProduct(dVec23Unit));

  CartesianVector gradientPhiOverAP4 =
    -gradientPhiOverdVecT.crossProduct(dVec23),

    gradientPhiOverAP3 =
    gradientPhiOverdVecT.crossProduct(dVec23 - dVec43) -
    gradientPhiOverdVecU.crossProduct(dVec12),

    gradientPhiOverAP2 =
    gradientPhiOverdVecT.crossProduct(dVec43) +
    gradientPhiOverdVecU.crossProduct(dVec23 + dVec12),

    gradientPhiOverAP1 = -(gradientPhiOverAP4 + gradientPhiOverAP3
                           + gradientPhiOverAP2);

    double cosDelta = cos(aTFP.delta), sinDelta = sin(aTFP.delta);
    double cosPhi = dVecTUnit*dVecUUnit,
      sinPhi = -dVec23Unit*(dVecUUnit.crossProduct(dVecTUnit));
    double DEnergyOverDPhi;
    // split the following part into different functions. get rid of the switch block...
    switch (aTFP.multiplicity)
      {
      case 1:
        aTorsionEnergy = aTFP.k0*(1 + cosDelta*cosPhi + sinDelta*sinPhi);
        DEnergyOverDPhi = aTFP.k0*(-cosDelta*sinPhi + sinDelta*cosPhi);
        break;
      case 2:
        aTorsionEnergy = aTFP.k0*(1 + cosDelta*(2.0*sqr(cosPhi)-1) + 2.0*sinDelta*sinPhi*cosPhi);
        DEnergyOverDPhi = 2.0*aTFP.k0*(-2.0*cosDelta*cosPhi*sinPhi + sinDelta*(2.0*sqr(cosPhi)-1));
        break;
      case 3:
        aTorsionEnergy = aTFP.k0*(1 + cosDelta*(4.0*cube(cosPhi) - 3.0*cosPhi) + sinDelta*(4.0*sqr(cosPhi)-1)*sinPhi);
        DEnergyOverDPhi = 3.0*aTFP.k0*(-cosDelta*(4.0*sqr(cosPhi)-1)*sinPhi + 4.0*sinDelta*cosPhi*(cosPhi-1));
        break;
      case 4:
        aTorsionEnergy = aTFP.k0*(1 + cosDelta*(8.0*sqr(sqr(cosPhi))-8.0*sqr(cosPhi)+1) + sinDelta*4.0*(2.0*cube(cosPhi)-cosPhi)*sinPhi);
        DEnergyOverDPhi = 4.0*aTFP.k0*(-cosDelta*(8.0*cube(cosPhi)-4.0*cosPhi)*sinPhi - sinDelta*(8.0*sqr(cosPhi)-8.0*sqr(sqr(cosPhi))-1));
        break;
      case 6:
        aTorsionEnergy = aTFP.k0*(1+cosDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) -1) + sinDelta*(32*sinPhi*sqr(cosPhi)*cube(cosPhi) - 32*cube(cosPhi)*sinPhi + 6*cosPhi*sinPhi));
        DEnergyOverDPhi = 6.0*aTFP.k0*(cosDelta*(-32*sqr(cosPhi)*cube(cosPhi)*sinPhi + 32*cube(cosPhi)*sinPhi - 6*cosPhi*sinPhi) + sinDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) - 1));
        break;

      default:
        assert(0);
        break;
      }
    aForceOnSite1 = -DEnergyOverDPhi*gradientPhiOverAP1;
    aForceOnSite2 = -DEnergyOverDPhi*gradientPhiOverAP2;
    aForceOnSite3 = -DEnergyOverDPhi*gradientPhiOverAP3;
    aForceOnSite4 = -DEnergyOverDPhi*gradientPhiOverAP4;

    BegLogLine(0) //aTorsionEnergy > 3.0)
    << "FORCE SWOPE TORSION "
    << " PA " << aSitePosition1
    << " PB " << aSitePosition2
    << " PC " << aSitePosition3
    << " PD " << aSitePosition4
    << " K0 " << aTFP.k0
    << " Mult " << aTFP.multiplicity
    << " delta " << aTFP.delta
    << " FA " << aForceOnSite1
    << " FB " << aForceOnSite2
    << " FC " << aForceOnSite3
    << " FD " << aForceOnSite4
    << " Etors " << aTorsionEnergy
    << EndLogLine;

  }


inline
void
OplsTorsionForce(
            unsigned                     &aSimTick,
            unsigned                     &aOpMask,
            double                       &aLamda,
            double                       &aDLamda,
            const TorsionForceParameters &aInitialTFP,
            const TorsionForceParameters &aFinalTFP,
            SitePosition                 &aSitePosition1,
            SitePosition                 &aSitePosition2,
            SitePosition                 &aSitePosition3,
            SitePosition                 &aSitePosition4,
            ForceVector                  &aForceOnSite1,
            ForceVector                  &aForceOnSite2,
            ForceVector                  &aForceOnSite3,
            ForceVector                  &aForceOnSite4,
            double                       &aTorsionEnergy,
            double                       &aDHDLamda,
            double                       &aVirial               )
  {
  }

inline
void
OplsTorsionForce(
            unsigned                     &aSimTick,
            const TorsionForceParameters & aTFP,
            SitePosition                 &aSitePosition1,
            SitePosition                 &aSitePosition2,
            SitePosition                 &aSitePosition3,
            SitePosition                 &aSitePosition4,
            ForceVector                  &aForceOnSite1,
            ForceVector                  &aForceOnSite2,
            ForceVector                  &aForceOnSite3,
            ForceVector                  &aForceOnSite4,
            double                       &aTorsionEnergy )
  {

  CartesianVector dVec43 = aSitePosition4 - aSitePosition3,
    dVec23 = aSitePosition2 - aSitePosition3,
    dVec12 = aSitePosition1 - aSitePosition2;
  CartesianVector dVecT = dVec43.crossProduct(dVec23),
    dVecU = dVec12.crossProduct(dVec23);
  double dVecTMag = sqrt(sqr(dVecT)),
    dVecUMag = sqrt(sqr(dVecU));

  // collinear situation
  if (is_almost_zero(dVecTMag) || is_almost_zero(dVecUMag))
    return;

  CartesianVector dVec23Unit = dVec23/sqrt(sqr(dVec23)),
    dVecTUnit = dVecT/dVecTMag,
    dVecUUnit = dVecU/dVecUMag;
  CartesianVector gradientPhiOverdVecT =
    1.0/sqr(dVecT)*(dVecT.crossProduct(dVec23Unit)),
    gradientPhiOverdVecU =
    -1.0/sqr(dVecU)*(dVecU.crossProduct(dVec23Unit));

  CartesianVector gradientPhiOverAP4 =
    -gradientPhiOverdVecT.crossProduct(dVec23),

    gradientPhiOverAP3 =
    gradientPhiOverdVecT.crossProduct(dVec23 - dVec43) -
    gradientPhiOverdVecU.crossProduct(dVec12),

    gradientPhiOverAP2 =
    gradientPhiOverdVecT.crossProduct(dVec43) +
    gradientPhiOverdVecU.crossProduct(dVec23 + dVec12),

    gradientPhiOverAP1 = -(gradientPhiOverAP4 + gradientPhiOverAP3
                           + gradientPhiOverAP2);

    double cosDelta = cos(aTFP.delta), sinDelta = sin(aTFP.delta);
    double cosPhi = dVecTUnit*dVecUUnit,
      sinPhi = -dVec23Unit*(dVecUUnit.crossProduct(dVecTUnit));
    double DEnergyOverDPhi;
    // split the following part into different functions. get rid of the switch block...
    switch (aTFP.multiplicity)
      {
      case 0:
        aTorsionEnergy = aTFP.k0;
        DEnergyOverDPhi = 0.0;
      case 1:
        aTorsionEnergy = (0.50*aTFP.k0)*(1 + cosDelta*cosPhi + sinDelta*sinPhi);
        DEnergyOverDPhi = (0.50*aTFP.k0)*(-cosDelta*sinPhi + sinDelta*cosPhi);
        break;
      case 2:
        aTorsionEnergy = (0.50*aTFP.k0)*(1 + cosDelta*(2.0*sqr(cosPhi)-1) + 2.0*sinDelta*sinPhi*cosPhi);
        DEnergyOverDPhi = 2.0*(0.50*aTFP.k0)*(-2.0*cosDelta*cosPhi*sinPhi + sinDelta*(2.0*sqr(cosPhi)-1));
        break;
      case 3:
        aTorsionEnergy = (0.50*aTFP.k0)*(1 + cosDelta*(4.0*cube(cosPhi) - 3.0*cosPhi) + sinDelta*(4.0*sqr(cosPhi)-1)*sinPhi);
        DEnergyOverDPhi = 3.0*(0.50*aTFP.k0)*(-cosDelta*(4.0*sqr(cosPhi)-1)*sinPhi + 4.0*sinDelta*cosPhi*(cosPhi-1));
        break;
      case 4:
        aTorsionEnergy = (0.50*aTFP.k0)*(1 + cosDelta*(8.0*sqr(sqr(cosPhi))-8.0*sqr(cosPhi)+1) + sinDelta*4.0*(2.0*cube(cosPhi)-cosPhi)*sinPhi);
        DEnergyOverDPhi = 4.0*(0.50*aTFP.k0)*(-cosDelta*(8.0*cube(cosPhi)-4.0*cosPhi)*sinPhi - sinDelta*(8.0*sqr(cosPhi)-8.0*sqr(sqr(cosPhi))-1));
        break;
      case 6:
        aTorsionEnergy = (0.50*aTFP.k0)*(1+cosDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) -1) + sinDelta*(32*sinPhi*sqr(cosPhi)*cube(cosPhi) - 32*cube(cosPhi)*sinPhi + 6*cosPhi*sinPhi));
        DEnergyOverDPhi = 6.0*(0.50*aTFP.k0)*(cosDelta*(-32*sqr(cosPhi)*cube(cosPhi)*sinPhi + 32*cube(cosPhi)*sinPhi - 6*cosPhi*sinPhi) + sinDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) - 1));
        break;

      default:
        assert(0);
        break;
      }
    aForceOnSite1 = -DEnergyOverDPhi*gradientPhiOverAP1;
    aForceOnSite2 = -DEnergyOverDPhi*gradientPhiOverAP2;
    aForceOnSite3 = -DEnergyOverDPhi*gradientPhiOverAP3;
    aForceOnSite4 = -DEnergyOverDPhi*gradientPhiOverAP4;

    BegLogLine(0) //aTorsionEnergy > 3.0)
    << "FORCE SWOPE TORSION "
    << " PA " << aSitePosition1
    << " PB " << aSitePosition2
    << " PC " << aSitePosition3
    << " PD " << aSitePosition4
    << " K0 " << aTFP.k0
    << " Mult " << aTFP.multiplicity
    << " delta " << aTFP.delta
    << " FA " << aForceOnSite1
    << " FB " << aForceOnSite2
    << " FC " << aForceOnSite3
    << " FD " << aForceOnSite4
    << " Etors " << aTorsionEnergy
    << EndLogLine;

}

inline
void
ImproperDihedralForce(
            unsigned                      &aOpMask,
            double                        &aLamda,
            double                        &aDLamda,
            unsigned                      &aSimTick,
            const ImproperForceParameters &aInitialIFP,
            const ImproperForceParameters &aFinalIFP,
            SitePosition                  &aSitePosition2,
            SitePosition                  &aSitePosition1, // central atom
            SitePosition                  &aSitePosition3,
            SitePosition                  &aSitePosition4,
            ForceVector                   &aForceOnSite2,
            ForceVector                   &aForceOnSite1,
            ForceVector                   &aForceOnSite3,
            ForceVector                   &aForceOnSite4,
            double                        &aImproperTorsionEnergy,
            double                        &aDHDLamda,
            double                        &aVirial               )
  {
  }


inline
void
ImproperDihedralForce(
            unsigned                      &aSimTick,
            const ImproperForceParameters &aIFP,
            SitePosition                  &aSitePosition2,
            SitePosition                  &aSitePosition1, // central atom
            SitePosition                  &aSitePosition3,
            SitePosition                  &aSitePosition4,
            ForceVector                   &aForceOnSite2,
            ForceVector                   &aForceOnSite1,
            ForceVector                   &aForceOnSite3,
            ForceVector                   &aForceOnSite4,
            double                        &aImproperTorsionEnergy )
  {
  // the improper dihedral angle is defined to be between the plane
  // 2-1-3 and 3-1-4
  //printf("Improper parm= %f %f %f\n", aIFP.Kpsi, aIFP.multiplicity, aIFP.psi0);
  CartesianVector dVec21 = aSitePosition2 - aSitePosition1,
    dVec31 = aSitePosition3 - aSitePosition1,
    dVec41 = aSitePosition4 - aSitePosition1;
  CartesianVector dVecT = dVec21.crossProduct(dVec31),
    dVecU = dVec31.crossProduct(dVec41);
  double dVecTMag = sqrt(sqr(dVecT)),
    dVecUMag = sqrt(sqr(dVecU));

  // collinear situation
  if (is_almost_zero(dVecTMag) || is_almost_zero(dVecUMag))
    return;

  CartesianVector dVec31Unit = dVec31/sqrt(sqr(dVec31)),
    dVecTUnit = dVecT/dVecTMag,
    dVecUUnit = dVecU/dVecUMag;
  double sinPsi = dVec31Unit*(dVecTUnit.crossProduct(dVecUUnit)),
    psi = asin(sinPsi);

  CartesianVector gradientPsiOverdVecT =
    -1.0/sqr(dVecT)*(dVecT.crossProduct(dVec31Unit)),
    gradientPsiOverdVecU =
    1.0/sqr(dVecU)*(dVecU.crossProduct(dVec31Unit));

  CartesianVector gradientPsiOverAP1 =
    gradientPsiOverdVecT.crossProduct(dVec31 - dVec21) +
    gradientPsiOverdVecU.crossProduct(dVec41 - dVec31),

    gradientPsiOverAP2 =
    -gradientPsiOverdVecT.crossProduct(dVec31),

    gradientPsiOverAP3 =
    gradientPsiOverdVecT.crossProduct(dVec21) -
    gradientPsiOverdVecU.crossProduct(dVec41),

    gradientPsiOverAP4 =
    gradientPsiOverdVecU.crossProduct(dVec31);


  aImproperTorsionEnergy = aIFP.Kpsi*sqr(psi - aIFP.psi0);

  double DenergyOverDpsi = 2.0*aIFP.Kpsi*(psi - aIFP.psi0);

  aForceOnSite1 = -DenergyOverDpsi*gradientPsiOverAP1;
  aForceOnSite2 = -DenergyOverDpsi*gradientPsiOverAP2;
  aForceOnSite3 = -DenergyOverDpsi*gradientPsiOverAP3;
  aForceOnSite4 = -DenergyOverDpsi*gradientPsiOverAP4;

  BegLogLine(0)
    << "FORCE IMPROPER TORSION "
    << " PA " << aSitePosition1
    << " PB " << aSitePosition2
    << " PC " << aSitePosition3
    << " PD " << aSitePosition4
    << " K0 " << aIFP.Kpsi
    << " psi0 " << aIFP.psi0
    << " FA " << aForceOnSite1
    << " FB " << aForceOnSite2
    << " FC " << aForceOnSite3
    << " FD " << aForceOnSite4
    << " E " << aImproperTorsionEnergy
    << EndLogLine;


  }


#if 0 // GRAVEYARD

//***** NOTE: SWOPE LAKE NOT COMPILED IN SO LOOK AT OTHER SIDE OF IF

  aInitialLJPMix.mSigma        = 0.5*(aInitialLJPA.mSigma + aInitialLJPB.mSigma);
  aFinalLJPMix.mSigma          = 0.5*(aFinalLJPA.mSigma + aFinalLJPB.msigma);

// tmp[I,F]* hold powers of the mixed sigma
// dist[I,F]* are inverse of distance + soft-core scaling
// rep[I,F] is repulsive vdW term
// disp[I,F] is attractive vdW term
//
// note that the soft-core parameter for the Initial state is
//              FinalLam*FinalLam*sigma**6
// & vice versa

  double tmpI           = aInitialLJPMix.mSigma;
  double tmpI3          = tmpI*tmpI*tmpI;
  double tmpI6          = tmpI3*tmpI3;
  double tmpI12         = tmpI6*tmpI6;
  double distI6         = 1.0 / (DistanceAB**6 + FinalLam*FinalLam*tmpI6);
  double distI12        = distI6*distI6;
  double repI           = (4.0 * aInitialLJPMix.mEpsilon) * (tmpI12*distI12);
  double dispI          = -(4.0 * aInitialLJPMix.mEpsilon) * (tmpI6*distI6);
  double ljeI           = repI + dispI;

  double tmpF           = aFinalLJPMix.mSigma;
  double tmpF3          = tmpF*tmpF*tmpF;
  double tmpF6          = tmpF3*tmpF3;
  double tmpF12         = tmpF6*tmpF6;
  double distF6         = 1.0 / (DistanceAB**6 + InitialLam*InitialLam*tmpF6);
  double distF12        = distF6*distF6;
  double repF           = (4.0 * aFinalLJPMix.mEpsilon) * (tmpF12*distF12);
  double dispF          = -(4.0 * aFinalLJPMix.mEpsilon) * (tmpF6*distF6);
  double ljeF           = repF + dispF;

  aLennardJonesEnergy  = InitialLam * ljeI + FinalLam * ljeF;

// assuming DEDR = -1.0 * deriv

  double DEDR           =
                         InitialLam *
                           (6.0 * (2.0 * repI - dispI) * distI6 *
                            DistanceAB * DistanceAB * DistanceAB) +
                         FinalLam *
                           (6.0 * (2.0 * repF - dispF) * distF6 *
                            DistanceAB * DistanceAB * DistanceAB);
// NOTE: check this!!

  double aDHDLamda =
                        - ljeI + 2.0 * tmpI6 * distI6 *
                        ( 2.0 * repI - dispI ) * InitialLam * InitialLam
                        + ljeF - 2.0 * tmpF6 * distF6 *
                        ( 2.0 * repF - dispF ) * FinalLam * FinalLam;

//************************************************************


  aLJPMix.mSigma        = 0.5*(aLJPA.mSigma + aLJPB.mSigma);
  double tmp           = aLJPMix.mSigma / DistanceAB;
  double tmp3          = tmp*tmp*tmp;
  double tmp6          = tmp3*tmp3;
  double tmp12         = tmp6*tmp6;
  double lje           = (4.0 * aLJPMix.mEpsilon) * (tmp12 - tmp6);
  aLennardJonesEnergy  = lje;


  double DEDR           =
                         (
                           (24 * aLJPMix.mEpsilon) *
                           (
                               ( 2 * tmp12)
                             - (  tmp6)
                           )
                         ) / (DistanceAB * DistanceAB) ;




inline
void
LennardJonesForce( unsigned                     &aSimTick,
                   unsigned                     &aOpMask,
                   const LennardJonesParameters &aLJPA,
                   const LennardJonesParameters &aLJPB,
                   SitePosition                 &aSitePositionA,
                   SitePosition                 &aSitePositionB,
                   XYZ                          &VectorAB,
                   double                       &DistanceAB,
                   ForceVector                  &aForceOnSiteA,
                   ForceVector                  &aForceOnSiteB,
                   double                       &aLennardJonesEnergy,
                   XYZ                          &aVirial               )
  {
  LennardJonesParameters aLJPMix;

  aLJPMix.mEpsilon      = sqrt( aLJPA.mEpsilon * aLJPB.mEpsilon );
  if( MSDIF::GetVDWCombine() == 0 )
    {     // CHARMM
    aLJPMix.mSigma        = aLJPA.mSigma + aLJPB.mSigma;
    }
  else if( MSDIF::GetVDWCombine() == 1 )
    { // OPLSAA
    // double rminA  = rminSigmaFactor * aLJPA.mSigma;
    // double rminB  = rminSigmaFactor * aLJPB.mSigma;
    // aLJPMix.mSigma        = 2.0*sqrt(rminA * rminB);
    aLJPMix.mSigma        = 2.0*sqrt(aLJPA.mSigma * aLJPB.mSigma);
    }
  else
    {
    assert( 0 );  // Unrecognized force field id in MolSysDef (.msd) file.
    }

  double tmp           = aLJPMix.mSigma / DistanceAB;
  double tmp3          = tmp*tmp*tmp;
  double tmp6          = tmp3*tmp3;
  double tmp12         = tmp6*tmp6;
  double lje           = (aLJPMix.mEpsilon) * (tmp12 - 2.0*tmp6);
  aLennardJonesEnergy  = lje;


  double DEDR           =
                         (
                           (12 * aLJPMix.mEpsilon) *
                           (
                               (  tmp12)
                             - (  tmp6)
                           )
                         ) / (DistanceAB * DistanceAB) ;



  ForceVector    fab     = VectorAB * DEDR;
  aForceOnSiteA          = fab;
  aForceOnSiteB          = fab * (-1);

  BegLogLine(PKFXLOG_FORCE_TERMS)
    << "FORCE LJ     "
    << aSimTick
    << " Energy " << lje
    << " PA " << aSitePositionA
    << " PB " << aSitePositionB
    << " FA " << aForceOnSiteA
    << EndLogLine;

  }

//=============================================================================

// This front-end helps the compiler identify the terms we hope it will inline.
inline
void
LennardJonesForce( unsigned                     &aSimTick,
                   const LennardJonesParameters &aLJPA,
                   const LennardJonesParameters &aLJPB,
                   SitePosition                 &aSitePositionA,
                   SitePosition                 &aSitePositionB,
                   ForceVector                  &aForceOnSiteA,
                   ForceVector                  &aForceOnSiteB,
                   double                       &aLennardJonesEnergy )
  {
  // This is here to be inlined when called near the ChargeForce funtion.
  assert(0);  // PHASE OUT...
  XYZ VectorAB          = aSitePositionA - aSitePositionB;
  double DistanceAB     = VectorAB.Length();

  // Call with method signature that includes VectorAB and DistanceAB
  LennardJonesForce( aSimTick,
                     aLJPA,
                     aLJPB,
                     aSitePositionA,
                     aSitePositionB,
                     VectorAB,
                     DistanceAB,
                     aForceOnSiteA,
                     aForceOnSiteB,
                     aLennardJonesEnergy);

  }



#endif


#endif
