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
 #ifndef UDF_HELPERGROUP_HPP
#define UDF_HELPERGROUP_HPP

#ifndef PKFXLOG_SHAKE_RATTLE
#define PKFXLOG_SHAKE_RATTLE (0)
#endif

// Adapted by Rayshubskiy (Aug 2002)
// Pitman - Aug 2002  Generalized UDF_HelperWater (Suits, Pitera) for X-H[1-3] groups

// This file provides some helper support for the UDF_Binding file, including
// template functions used by UDF's (rigid group models) and other additions
// that would otherwise go into the HERE document of the UDFBind.pl perl script

template <int PARAM_COUNT> class ConstraintGroupParams
{
public:
    double rEQ[ PARAM_COUNT-1 ];
};

class UDF_HelperGroup
{
public:

  // Base class holding inner loops of SHAKE and RATTLE
  // Groups of rigid bonds should be defined as derived classes
  // with the enum NSITES specifying the number of sites in the group
  // and the required working arrays dimensioned statically.
  class RigidBond
  {
  public:
    static inline  int ShakeXH(double tolerance, double dt,
                               XYZ &fconX, XYZ &fconH,
                               XYZ &vX, XYZ &vH, XYZ &vecXH,
                               double rXH, double rXHsq, double mX, double mH)
    {

      // these are the "free flight" positions of the vectors
      // i.e. where the vectors end up without the force of constraint
      // these are out here to save adding in dt*(vA-vB) at each iteration
      // but could be moved in to save memory

      XYZ    freeXH = vecXH + dt * (vH - vX);

      // initially assume constraint are violated,
      // ensuring at least one pass through the loop

      XYZ    proXH    = freeXH +  dt * (fconH - fconX);
      double lenproXH = proXH.Length();

      if (fabs(lenproXH - rXH) > tolerance)
        {
          double SdotR =  -2.0 * dt * proXH.DotProduct(vecXH) * (mX + mH);
          double G = (lenproXH*lenproXH - rXHsq)/SdotR;
          fconX -= G * mX * vecXH;
          fconH += G * mH * vecXH;
        }
      else {    return 0;      }

      return 1;
    }


    static inline  int RattleXH(double tolerance, double dt,
                                XYZ &fconX, XYZ &fconH,
                                XYZ &vecXH, XYZ &velXH,
                                double rXH, double rXHsq,
                                double mX, double mH)
    {

         XYZ  provXH = velXH + ( fconX - fconH );

   // get the dot product of each velocity on the corresponding
    // constraint to check against veloc_tolerance
         double devproXH = vecXH.DotProduct(provXH);


      // initially assume constraint are violated,
      // ensuring at least one pass through the loop

      if (fabs(devproXH) > tolerance * rXH)
        {
            double G = 0.5 * devproXH / (rXHsq * (mX + mH));
            fconX -= G * mX * vecXH;
            fconH += G * mH * vecXH;
        }
      else {    return 0; }

      return 1;
    }

  };

  //////////////////////////////////////////////////////////////
  ///////////////////////  ShakeRGroup /////////////////////////
  //////////////////////////////////////////////////////////////
  // template function for SHAKE on a rigid group.
  // RGRP is one of the derived classes RigidBond (RigidXH, RigidXH2, or RigidXH3)
  // The number of sites is taken from RGRP::NSITES.
  // The number of constraints (bonds) is assumed to be NSITES - 1.
  //       NOTE: rings are not supported
  // The topology is assumed to be the first atom (the heavy atom)
  //       is connected to the remaining atoms.
  // It is assumed that the RigidGroup has been initialized.

    template <class MDVM, int NSITES>
    static inline int ShakeRGroup(MDVM &aMDVM, ConstraintGroupParams<NSITES> aRGRP)
    {
      const double tolerance      = UDF_State::Shake.Epsilon;
      const int    max_iterations = UDF_State::Shake.MaxIterations;
      int shake_iterations        = 0;

      int violations              = NSITES - 1;
      int testcon[NSITES - 1];
      double hinvm[NSITES];
      double rXHsq[NSITES - 1];
      XYZ f[NSITES];
      XYZ v[NSITES];
      XYZ vecXH[NSITES - 1];
      int siteIds[ NSITES ];

//       BegLogLine(PKFXLOG_SHAKE_RATTLE)
//      << "ShakeRGroup(MDBM&)"
//      << " NSITES" << NSITES
//      << EndLogLine ;

      for(int i = 0; i< NSITES; i++)
        {
            // Assumes heave atom first, then the hydrogen
          siteIds[i] = aMDVM.GetSiteId( i );
          // v[i]  = aMDVM.GetVelocity( aRGRP.mId[i] );
          v[i]  = aMDVM.GetVelocity( i );
          // hinvm[i]  = aMDVM.GetHalfInverseMass( aRGRP.mId[i] );
          hinvm[i]  = aMDVM.GetHalfInverseMass( i );
          f[i] = XYZ::ZERO_VALUE();

//        BegLogLine(PKFXLOG_SHAKE_RATTLE)
//          << "ShakeRGroup(MDBM&)"
//          << " Velocity " << i << " "
//          << v[i]
//          << EndLogLine ;
        }
      for(int i = 0; i< NSITES-1; i++)
        {
          testcon[i] = 1;
          rXHsq[i]  = aRGRP.rEQ[i] * aRGRP.rEQ[i];
          vecXH[i] = aMDVM.GetVector(i+1, 0);  // swapped
//        BegLogLine(PKFXLOG_SHAKE_RATTLE)
//          << "ShakeRGroup(MDBM&)"
//          << " Vector " << i << " "
//          << vecXH[i]
//          << EndLogLine ;
        }


      double dt = aMDVM.GetDeltaT();
      while ((violations > 0) && (shake_iterations < max_iterations))
        {
          violations = 0;
          for(int i = 0; i < NSITES - 1; i++)
            {
              if(testcon[ i ] > 0)
                {
                  testcon[ i ] = RigidBond::ShakeXH(tolerance, dt ,
                                                    f[ 0 ] , f[ i+1 ] ,
                                                    v[ 0 ] , v[ i+1 ] ,
                                                    vecXH[ i ] ,
                                                    aRGRP.rEQ[ i ] , rXHsq[i],
                                                    hinvm[ 0 ] , hinvm[ i+1 ] );

                  violations += testcon[ i ];
                }
            }
          shake_iterations++;
        }
      //
      // check if we hit max_iterations
      //
      if (shake_iterations >= max_iterations)
        {
          BegLogLine( 1 )
            << "ShakeRGroup(MDBM&) Shake iterations exceeded maximum"
            << EndLogLine  ;
          
          PLATFORM_ABORT("ShakeRGroup(MDBM&) Shake iterations exceeded maximum");
        }
      else
        {

          // done with SHAKE iterations, converged before the iteration limit
          // now place influence of force of constraint in velocity arrays
          // remember f[A,B,C] are not real forces but f/m * dt/2
          //
          // note we are not including a virial contribution from the force of
          // constraint. . .

          // suits - originally the fcon's were added, but it seemed to push the wrong way.
          for(int i = 0; i < NSITES; i++)
            {
              v[ i ] += f[ i ];
              aMDVM.ReportVelocity( i , v[i] );
//            if(i < NSITES - 1)
//              {
//                double d = vecXH[i].Length();
//                double dr = d - sqrt(rXHsq[i]);
//                printf(" Bond length from equil = %f\n", dr);
//              }

//            BegLogLine(PKFXLOG_SHAKE_RATTLE)
//              << "ShakeRGroup(MDBM&)"
//              << " f[ " << i << " ]= " << f[ i ]
//              << EndLogLine ;
            }
//        BegLogLine(PKFXLOG_SHAKE_RATTLE)
//          << " shake_iterations=" << shake_iterations
//          << EndLogLine ;
        }

      return shake_iterations;
    }

  //////////////////////////////////////////////////////////////
  ///////////////////////  RattleRGroup ////////////////////////
  //////////////////////////////////////////////////////////////
  // template function for RATTLE on a rigid group.
  // RGRP is one of the derived classes RigidBond (RigidXH, RigidXH2, or RigidXH3)
  // The number of sites is taken from NSITES.
  // The number of constraints (bonds) is assumed to be NSITES - 1.
  //       NOTE: rings are not supported
  // The topology is assumed to be the first atom (the heavy atom)
  //       is connected to the remaining atoms.
  // It is assumed that the RigidGroup has been initialized.

    template <class MDVM, int NSITES >
    static inline int RattleRGroup(MDVM &aMDVM, const ConstraintGroupParams<NSITES> aRGRP)
    {
      const double tolerance      = UDF_State::Rattle.Epsilon;
      const int    max_iterations = UDF_State::Rattle.MaxIterations;
      int rattle_iterations        = 0;

      int violations              = NSITES - 1;
      int testcon[NSITES - 1];
      double hinvm[NSITES];
      double rXHsq[NSITES - 1];
      XYZ f[NSITES];
      XYZ v[NSITES];
      XYZ velXH[NSITES - 1];
      XYZ vecXH[NSITES - 1];
      XYZ provXH[NSITES - 1];
      int siteIds[ NSITES ];

//       BegLogLine(PKFXLOG_SHAKE_RATTLE)
//      << "RattleRGroup(MDBM&)"
//      << " NSITES" << NSITES
//      << EndLogLine ;

      for(int i = 0; i< NSITES; i++)
        {
          siteIds[i] = aMDVM.GetSiteId( i );
          v[i]  = aMDVM.GetVelocity( i );
          hinvm[i]  = aMDVM.GetHalfInverseMass( i );
          f[i] = XYZ::ZERO_VALUE();


//        BegLogLine(PKFXLOG_SHAKE_RATTLE)
//          << "RattleRGroup(MDBM&)"
//          << " Velocity " << i << " "
//          << v[i]
//          << EndLogLine ;
        }
      for(int i = 0; i< NSITES-1; i++)
        {
          testcon[i] = 1;
          rXHsq[i]  = aRGRP.rEQ[i] * aRGRP.rEQ[i];

    // current position vectors at t+dt

          vecXH[i] = aMDVM.GetVector( i+1, 0 ); //swapped
          //velXH[i] = v[i+1] - v[0];
          velXH[i] = v[0] - v[i+1];
//        BegLogLine(PKFXLOG_SHAKE_RATTLE)
//          << "ShakeRGroup(MDBM&)"
//          << " Vector " << i << " "
//          << vecXH[i]
//          << EndLogLine ;
        }


    double dt = aMDVM.GetDeltaT();
    // convert tolerance to velocities. . .

    double velocity_tolerance = tolerance/dt;


    while ((violations > 0) && (rattle_iterations < max_iterations))
      {
        violations = 0;

    // project the current values of the interparticle velocities
    // (includes contributions from any force of constraint we have generated
    // so far)

    // suits - removed factor of dt that multiplied delta fcon's
    // As in shake, fcon's have velocity units and are pre-weighted by inverse mass

        for(int i = 0; i < NSITES - 1; i++)
          {

            if(testcon[ i ] > 0)
              {
                testcon[ i ] = RigidBond::RattleXH(tolerance, dt ,
                                                   f[ 0 ] , f[ i+1 ] ,
                                                   vecXH[ i ] , velXH[ i ],
                                                   aRGRP.rEQ[ i ], rXHsq[i],
                                                   hinvm[ 0 ] , hinvm[ i+1 ] );

                violations += testcon[ i ];
              }
          }
        rattle_iterations++;
      }
    //
    // check if we hit max_iterations
    //
        if (rattle_iterations >= max_iterations)
          {
            BegLogLine(1)
              << "RattleRGroup(MDBM&) Rattle iterations exceeded maximum"
              << EndLogLine ;

            PLATFORM_ABORT("RattleRGroup(MDBM&) Rattle iterations exceeded maximum");
          }
        else
          {

            // done with RATTLE iterations, converged before the iteration limit
            // now place influence of force of constraint in velocity arrays
            // remember f[A,B,C] are not real forces but f/m * dt/2
            //
            // note we are not including a virial contribution from the force of
            // constraint. . .

            // suits - originally the fcon's were added, but it seemed to push the wrong way.

            for(int i = 0; i < NSITES; i++)
              {
                v[ i ] += f[ i ];
                aMDVM.ReportVelocity( i , v[i] );

//              BegLogLine(PKFXLOG_SHAKE_RATTLE)
//                << "RattleRGroup(MDBM&)"
//                << " f[ " << i << " ]= " << f[ i ]
//                << EndLogLine ;
              }

            //      XYZ V = v[0] - v[1];
            //      double dd = V.DotProduct(vecXH[0]);
            //      printf("relative velocity Dot vec \t%f\n",dd);

//          BegLogLine(PKFXLOG_SHAKE_RATTLE)
//            << " rattle_iterations=" << rattle_iterations
//            << EndLogLine ;
          }

        return rattle_iterations;
    }
};
#if 0
  ////////////////////////////////////////////////////////
  // XH bond in rigid group
  class RigidXH : public RigidBond
  {
    enum {NSITES = 2};
  public:
    int     mId[NSITES];
    double  XH3Eq[NSITES - 1];
    void         Init(int xid, int hid, double xhEq)
    {
      mId[0] = xid;
      mId[1] = hid; mXHEq[0] = xhEq;
    }
  };

  ////////////////////////////////////////////////////////
  // XH2 rigid group
  class RigidXH2 : public RigidBond
  {
    enum {NSITES = 3};
  public:
    int     mId[NSITES];
    double  XH3Eq[NSITES - 1];
    void         Init(int xid, int h1id, double xh1Eq
                      int h2id, double xh2Eq)
    {
      mId[0] = xid;
      mId[1] = h1id; mXHEq[0] = xh1Eq;
      mId[2] = h2id; mXHEq[1] = xh2Eq;
    };

  ////////////////////////////////////////////////////////
  // XH3 rigid group
    class RigidXH3 : public RigidBond
    {
      enum {NSITES = 4};
    public:
      int     mId[NSITES];
      double  XH3Eq[NSITES - 1];
      void         Init(int xid, int h1id, double xh1Eq
                        int h2id, double xh2Eq,
                        int h3id, double xh3Eq)
      {
        mId[0] = xid;
        mId[1] = h1id; mXHEq[0] = xh1Eq;
        mId[2] = h2id; mXHEq[1] = xh2Eq;
        mId[3] = h3id; mXHEq[2] = xh3Eq;
      };
    };
#endif
#endif




