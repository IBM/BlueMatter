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
 #ifndef UDF_HELPERWATER_HPP
#define UDF_HELPERWATER_HPP

#ifndef PKFXLOG_SHAKE_RATTLE
#define PKFXLOG_SHAKE_RATTLE (0)
#endif

// Still problems with matrix shake/rattle, so making old code the default


#undef PERFORMANCE_SHAKE
#undef PERFORMANCE_RATTLE

#ifdef NON_MATRIX_SHAKE_RATTLE_WATER
#undef MATRIX_SHAKE
#undef MATRIX_RATTLE
#else
#define MATRIX_SHAKE
#define MATRIX_RATTLE
#endif

// This file provides some helper support for the UDF_Binding file, including
// template functions used by UDF's (such as water models) and other additions
// that would otherwise go into the HERE document of the UDFBind.pl perl script

class UDF_HelperWater
{
public:

    class WaterRigid
    {
    public:
        static double RIM[3][3]; // Rattle Inverse Matrix
	static int IsRattleInverseMatrixDefined;
        static const double GetROH();
        static const double GetRHH();
    };

    class WaterFlexible : public WaterRigid
    {
    public:
        static const double GetR0();
        static const double GetKR();
        static const double GetTH0();
        static const double GetKTH();
    };

    class StdFlexible : public WaterFlexible
    {
    public:
        static const double GetKR()  { return 450.0;         }
        static const double GetR0()  { return   0.9572;      }
        static const double GetKTH() { return  55.0;         }
        static const double GetTH0() { return   1.824218134; }
    };

    class TIP3P : public WaterRigid
    {
    // based on th = 104.52 degrees
    public:
        static const double GetROH() { return   0.9572;        }
        static const double GetRHH() { return   1.5139006545;  }
    };

    class SPC : public WaterRigid
    {
    // based on th = 109.47 degrees
    public:
        static const double GetROH() { return   1.0000;        }
        static const double GetRHH() { return   1.63298086184; }
    };

    template <class MDVM, class WATER> static inline void FlexibleForce( MDVM &aMDVM )
    {
        //  This code previously assumed OHH order and now assumes HOH
        //  changes were made only at the MDVM interface - not variable names

        XYZ    vAB = aMDVM.GetVector(  MDVM::SiteA, MDVM::SiteB);
        XYZ    vAC = aMDVM.GetVector(  MDVM::SiteC, MDVM::SiteB);
        XYZ RP = vAC.crossProduct(vAB);

        double RPL2 = RP.LengthSquared() ;

        double r2AB = vAB.LengthSquared() ;
        double r2AC = vAC.LengthSquared() ;

        double recipAB = 1.0/sqrt(r2AB) ;
        double recipAC = 1.0/sqrt(r2AC) ;
        // Caution: If the bond angle was 180 degrees, RPL2 will be zero so RPL will be Inf or NaN
        // This will propagate through SINTHE, atrig, AngleEnergy, DTD* but will all get
        // resolved in the IsVectorSquareLengthZero conditional
        double RPL = 1.0/sqrt(RPL2) ;

        double SINTHE = RPL2 * RPL * ( recipAB * recipAC) ;
        double rAB = recipAB * r2AB ;
        double rAC = recipAC * r2AC ;

        //===========================================================

        double DeltaFromEquilibrium  = rAB - WATER::GetR0();

        double Energy = ( WATER::GetKR() * DeltaFromEquilibrium ) * DeltaFromEquilibrium;

        double  DEDR  = -2.0 * ( WATER::GetKR() * DeltaFromEquilibrium ) * recipAB;

        XYZ ForceB = vAB * DEDR;
        XYZ ForceA = -ForceB;

        //===========================================================

        DeltaFromEquilibrium  = rAC - WATER::GetR0();

        Energy += ( WATER::GetKR() * DeltaFromEquilibrium ) * DeltaFromEquilibrium;

        DEDR    = -2.0 * ( WATER::GetKR() * DeltaFromEquilibrium ) * recipAC;

        XYZ ForceC  = vAC * DEDR;
            ForceA -= ForceC;

#ifdef DEBUG_UDF
        aMDVM.ReportUdfEnergy(UDF_Binding::StdHarmonicBondForce_Code ,Energy);
#endif
        //===========================================================

        double COSTHE = ( vAB.DotProduct( vAC ) ) * (recipAB * recipAC);

        double THETA = Math::atrig(SINTHE, COSTHE) ;
        double delta = THETA - WATER::GetTH0();

        // Beware, the energy will be NaN if the angle was 180 degrees
        double AngleEnergy = WATER::GetKTH() * Math::sqr(delta);

        double DEDTH = 2.0 * WATER::GetKTH() * (delta);

        double    R12R = -1.0 *  (recipAB * recipAB);
        double    R32R =  1.0 *  (recipAC * recipAC);
        double    DTD12X = R12R*(RPL*DEDTH) * (vAB.mY*RP.mZ - vAB.mZ*RP.mY);
        double    DTD12Y = R12R*(RPL*DEDTH) * (vAB.mZ*RP.mX - vAB.mX*RP.mZ);
        double    DTD12Z = R12R*(RPL*DEDTH) * (vAB.mX*RP.mY - vAB.mY*RP.mX);
        double    DTD32X = R32R*(RPL*DEDTH) * (vAC.mY*RP.mZ - vAC.mZ*RP.mY);
        double    DTD32Y = R32R*(RPL*DEDTH) * (vAC.mZ*RP.mX - vAC.mX*RP.mZ);
        double    DTD32Z = R32R*(RPL*DEDTH) * (vAC.mX*RP.mY - vAC.mY*RP.mX);

        if ( !  Heuristics::IsVectorSquareLengthZero(RPL2) )
          {
            ForceB.mX -=  DTD12X        ;
            ForceB.mY -=  DTD12Y        ;
            ForceB.mZ -=  DTD12Z        ;
            ForceA.mX -= -DTD12X-DTD32X ;
            ForceA.mY -= -DTD12Y-DTD32Y ;
            ForceA.mZ -= -DTD12Z-DTD32Z ;
            ForceC.mX -=         DTD32X ;
            ForceC.mY -=         DTD32Y ;
            ForceC.mZ -=         DTD32Z ;

#ifdef DEBUG_UDF
            aMDVM.ReportUdfEnergy(UDF_Binding::StdHarmonicAngleForce_Code,AngleEnergy );
#endif

            Energy += AngleEnergy ;
            aMDVM.ReportEnergy(Energy);

            aMDVM.ReportForce( MDVM::SiteA, ForceB );
            aMDVM.ReportForce( MDVM::SiteB, ForceA );
            aMDVM.ReportForce( MDVM::SiteC, ForceC );
          }
          else
          {
            // In the special case, we computed AngleEnergy as NaN; perform the correct computation
            // (Assume the angle is 180 degrees, since 0 degrees is not physical)
            // Also, do not add the angle force in; its direction is undefined.
            double CollinearAngleEnergy = WATER::GetKTH() * Math::sqr(M_PI - WATER::GetTH0() ) ;
#ifdef DEBUG_UDF
            aMDVM.ReportUdfEnergy(UDF_Binding::StdHarmonicAngleForce_Code,CollinearAngleEnergy );
#endif
            Energy += CollinearAngleEnergy ;
            aMDVM.ReportEnergy( Energy );

            aMDVM.ReportForce( MDVM::SiteA, ForceB );
            aMDVM.ReportForce( MDVM::SiteB, ForceA );
            aMDVM.ReportForce( MDVM::SiteC, ForceC );
          }

        return;
    }

#if defined(PERFORMANCE_SHAKE)
    template <class MDVM, class WATER> static inline void Rigid3SiteShake(
        MDVM &aMDVM)
    {

    // JWP August 2001

    // does FIRST velocity halfstep update w/ SHAKE for rigid water
    // adds dt/2 * f(t)/m to v(t)
    // calling routine is responsible to make sure f(t) is correct
    // velocity update is followed by SHAKE which adjusts the
    // velocities such that the positions r(t+dt) will satisfy the constraints

    //    const double  tolerance      = 0.00000001;
    //    const int     max_iterations = 1000;

    // really conservative default shake tolerance
    // 10^-6 can be OK but timestep dependent, no warranties, etc.

        // const double rOH   = 0.9572000;
        // const double rHH   = 1.5139006;
        const double rOH   = WATER::GetROH();
        const double rHH   = WATER::GetRHH();
        const double rOHsq = rOH * rOH;
        const double rHHsq = rHH * rHH;

        const double tolerance = UDF_State::Shake.Epsilon;
        const double tolerance_squared = tolerance*tolerance ;
        const int    max_iterations = UDF_State::Shake.MaxIterations;

    // above are constraint lengths (and length^2) for TIPnP water models

    // suits - this routine previously assumed OHH order, and is now switched at
    // the MDVM interface level. Variable names were not changed.

        XYZ    vA = aMDVM.GetVelocity( MDVM::SiteB );
        XYZ    vB = aMDVM.GetVelocity( MDVM::SiteA );
        XYZ    vC = aMDVM.GetVelocity( MDVM::SiteC );

        double    mA = aMDVM.GetHalfInverseMass( MDVM::SiteB );
        double    mB = aMDVM.GetHalfInverseMass( MDVM::SiteA );
        double    mC = aMDVM.GetHalfInverseMass( MDVM::SiteC );

        double dt = aMDVM.GetDeltaT();

    // Shake algorithm pretty much direct from Bill Swope's little water code, JWP
    // current position vectors

        XYZ    vecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB );
        XYZ    vecAC = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteB );
        XYZ    vecBC = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteA );


        BegLogLine(PKFXLOG_SHAKE_RATTLE)
          << "Rigid3SiteShake(MDBM&)"
          << " vecAB=" << vecAB
          << " vecAC=" << vecAC
          << " vecBC=" << vecBC
          << " vA=" << vA
          << " vB=" << vB
          << " vC=" << vC
          << EndLogLine ;

    // start with forces of constraint set to zero

        XYZ    fconA = XYZ::ZERO_VALUE();
        XYZ    fconB = XYZ::ZERO_VALUE();
        XYZ    fconC = XYZ::ZERO_VALUE();

    // these are the "free flight" positions of the vectors
    // i.e. where the vectors end up without the force of constraint
    // these are out here to save adding in dt*(vA-vB) at each iteration
    // but could be moved in to save memory

        XYZ    freeAB = vecAB + dt * (vB - vA);
        XYZ    freeAC = vecAC + dt * (vC - vA);
        XYZ    freeBC = vecBC + dt * (vC - vB);

    // initially assume all 3 constraints are violated,
    // ensuring at least one pass through the loop

    // it's traditional to keep track of the number of iterations and
    // vomit if it passes the maximum. . .

    // since the 3 constraints are coupled, if we make adjustments
    // to fix any one constraint, we have to check them all again

        int shake_iterations = 0;
        int violations = 3;

        XYZ    proAB;
        XYZ    proAC;
        XYZ    proBC;

        double lenproAB;
        double lenproAC;
        double lenproBC;

        double ABok ;
        double ACok ;
        double BCok ;

        for ( shake_iterations=0 ; shake_iterations < max_iterations ; shake_iterations += 1 )
        {

    // project the current values of the interparticle vectors
    // (includes contributions from any force of constraint we have generated
    //  so far)

    //  suits - each fcon is already weighted by inverse mass.
    //  fcon's have velocity units
    //  (heavy ones will smaller velocity boost)

            proAB = freeAB +  dt * (fconB - fconA);
            proAC = freeAC +  dt * (fconC - fconA);
            proBC = freeBC +  dt * (fconC - fconB);

    // get the lengths of these vectors to compare against the constraints

            lenproAB = proAB.LengthKnownNotZero();
            lenproAC = proAC.LengthKnownNotZero();
            lenproBC = proBC.LengthKnownNotZero();


    // check each vector against the corresponding constraint
    // if it is violated, calculate the additional "forces of constraint"
    // necessary to fix it

    // O - H1 vector

            {
                double SdotR =  -2.0 * dt * proAB.DotProduct(vecAB) * (mA + mB);
                double G = (lenproAB*lenproAB - rOHsq) * recip(SdotR);
                fconA -= G * mA * vecAB;
                fconB += G * mB * vecAB;
            }

            double ABdelta = lenproAB - rOH ;
            ABok = ABdelta*ABdelta - tolerance_squared ;

    // O - H2 vector

            {
                double SdotR =  -2.0 * dt * proAC.DotProduct(vecAC) * (mA + mC);
                double G = (lenproAC*lenproAC - rOHsq) * recip(SdotR);
                fconA -= G * mA * vecAC;
                fconC += G * mC * vecAC;
            }

            double ACdelta = lenproAC - rOH ;
            ACok = ACdelta*ACdelta - tolerance_squared ;

    // H1 - H2 vector
    // note that since both particles have the same mass we don't
    // have to mass-weight the force of constraint, just divide it by 2
    // suits - this is done with the extra 1/2 factor in G

            {
                double SdotR =  -2.0 * dt * proBC.DotProduct(vecBC) * (mB + mC);
                double G = (lenproBC*lenproBC - rHHsq) * recip(SdotR);
                fconB -= G * mB * vecBC;
                fconC += G * mC * vecBC;
            }

            double BCdelta = lenproBC - rHH ;
            BCok = BCdelta*BCdelta - tolerance_squared ;


            double r0 = fsel(ABok,ABok,ACok) ;
            double r1 = fsel(r0,r0,BCok) ;
            if (r1 <= 0.0 ) break ;
//          if (ABok <= 0.0 && ACok <= 0.0 && BCok <= 0.0 ) break ;


        } /* endfor */
    //
    // check if we hit max_iterations
    //
        if (ABok > 0.0 || ACok > 0.0 || BCok > 0.0 ) {
            BegLogLine(1)
              << "Rigid3SiteShake(MDBM&)"
              << " vecAB=" << vecAB
              << " vecAC=" << vecAC
              << " vecBC=" << vecBC
              << " vA=" << vA
              << " vB=" << vB
              << " vC=" << vC
              << " Shake iterations exceeded maximum"
              << EndLogLine ;

            PLATFORM_ABORT("Rigid3SiteShake(MDBM&) Shake iterations exceeded maximum");
        } else {

    // done with SHAKE iterations, converged before the iteration limit
    // now place influence of force of constraint in velocity arrays
    // remember fcon[A,B,C] are not real forces but f/m * dt/2
    //
    // note we are not including a virial contribution from the force of
    // constraint. . .

    // suits - originally the fcon's were added, but it seemed to push the wrong way.

            vA += fconA;
            vB += fconB;
            vC += fconC;

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteShake(MDBM&)"
              << " fconA=" << fconA
              << " fconB=" << fconB
              << " fconC=" << fconC
              << " shake_iterations=" << shake_iterations
              << EndLogLine ;

            aMDVM.ReportVelocity( MDVM::SiteB, vA );
            aMDVM.ReportVelocity( MDVM::SiteA, vB );
            aMDVM.ReportVelocity( MDVM::SiteC, vC );
        }

        return;
    }
#elif defined(MATRIX_SHAKE)
    template <class MDVM, class WATER> static inline void Rigid3SiteShake(
        MDVM &aMDVM)
    {

    // JWP August 2001

    // does FIRST velocity halfstep update w/ SHAKE for rigid water
    // adds dt/2 * f(t)/m to v(t)
    // calling routine is responsible to make sure f(t) is correct
    // velocity update is followed by SHAKE which adjusts the
    // velocities such that the positions r(t+dt) will satisfy the constraints

    //    const double  tolerance      = 0.00000001;
    //    const int     max_iterations = 1000;

    // really conservative default shake tolerance
    // 10^-6 can be OK but timestep dependent, no warranties, etc.

        // const double rOH   = 0.9572000;
        // const double rHH   = 1.5139006;
        const double rOH   = WATER::GetROH();
        const double rHH   = WATER::GetRHH();
        const double rOHsq = rOH * rOH;
        const double rHHsq = rHH * rHH;

        const double tolerance = UDF_State::Shake.Epsilon;
        const int    max_iterations = UDF_State::Shake.MaxIterations;

    // above are constraint lengths (and length^2) for TIPnP water models

    // suits - this routine previously assumed OHH order, and is now switched at
    // the MDVM interface level. Variable names were not changed.

    // pitera -- note
    // internal A = oxygen
    // internal B = hydrogen 1
    // internal C = hydrogen 2

        XYZ    vA = aMDVM.GetVelocity( MDVM::SiteB );
        XYZ    vB = aMDVM.GetVelocity( MDVM::SiteA );
        XYZ    vC = aMDVM.GetVelocity( MDVM::SiteC );

        double    mA = aMDVM.GetHalfInverseMass( MDVM::SiteB );
        double    mB = aMDVM.GetHalfInverseMass( MDVM::SiteA );
        double    mC = aMDVM.GetHalfInverseMass( MDVM::SiteC );

    // pitera -- we need some combined masses to build the A-matrix
    // mH = mass of one hydrogen
    // mO = mass of one oxygen
    // mOH = 1/2 of H mass + 1/2 of O mass
    // mHH = 1/2 of H mass + 1/2 of H mass 

        double    mH = mB + mC;
        double    mO = 2.0 * mA;
        double    mOH = mH + mO;
        double    mHH = mH + mH;

        double dt = aMDVM.GetDeltaT();

    // M-Shake algorithm as implemented in ARC water code, JWP
    // current position vectors

        XYZ    vecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB );
        XYZ    vecAC = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteB );
        XYZ    vecBC = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteA );

#if 0
	XYZ rA = aMDVM.GetPosition(MDVM::SiteA);
	XYZ rB = aMDVM.GetPosition(MDVM::SiteB);
	XYZ rC = aMDVM.GetPosition(MDVM::SiteC);

	BegLogLine(0)
	<< "ORIG SHAKE" << aMDVM.GetSimTick() << " " << aMDVM.GetSiteId(MDVM::SiteA)  << "\n"  
	<< rA << " " << rB << " " << rC << "\n" 
	<< vA << " " << vB << " " << vC << EndLogLine;
#endif

    // AB = O - H1
    // AC = O - H2
    // BC = H1 - H2

        BegLogLine(PKFXLOG_SHAKE_RATTLE)
          << "Rigid3SiteShake(MDBM&)"
          << " vecAB=" << vecAB
          << " vecAC=" << vecAC
          << " vecBC=" << vecBC
          << " vA=" << vA
          << " vB=" << vB
          << " vC=" << vC
          << EndLogLine ;

    // start with forces of constraint set to zero

        XYZ    fconA = XYZ::ZERO_VALUE();
        XYZ    fconB = XYZ::ZERO_VALUE();
        XYZ    fconC = XYZ::ZERO_VALUE();

    // start with g-vector set to zero

        double gvec[3];
	gvec[0] = gvec[1] = gvec[2] = 0.0;

    // these are the "free flight" positions of the vectors
    // i.e. where the vectors end up without the force of constraint
    // these are out here to save adding in dt*(vA-vB) at each iteration
    // but could be moved in to save memory

        XYZ    freeAB = vecAB + dt * (vB - vA);
        XYZ    freeAC = vecAC + dt * (vC - vA);
        XYZ    freeBC = vecBC + dt * (vC - vB);

    // initially assume all 3 constraints are violated,
    // ensuring at least one pass through the loop

    // it's traditional to keep track of the number of iterations and
    // vomit if it passes the maximum. . .

    // since the 3 constraints are coupled, if we make adjustments
    // to fix any one constraint, we have to check them all again

        int shake_iterations = 0;
        int violations = 3;

    // projected inter-particle distances including latest constraint
    // update

        XYZ    proAB;
        XYZ    proAC;
        XYZ    proBC;

        double lenproAB;
        double lenproAC;
        double lenproBC;

        double lenproABsq;
        double lenproACsq;
        double lenproBCsq;

    // A matrix and its inverse

        double mtrxA[3][3];
        double minvA[3][3];

        while ((violations > 0) && (shake_iterations < max_iterations)) {

    // project the current values of the interparticle vectors
    // (includes contributions from any force of constraint we have generated
    //  so far)

    //  suits - each fcon is already weighted by inverse mass.
    //  fcon's have velocity units
    //  (heavy ones will smaller velocity boost)

            proAB = freeAB +  dt * (fconB - fconA);
            proAC = freeAC +  dt * (fconC - fconA);
            proBC = freeBC +  dt * (fconC - fconB);

    // get the lengths of these vectors to compare against the constraints

            lenproAB = proAB.Length();
            lenproAC = proAC.Length();
            lenproBC = proBC.Length();

            lenproABsq = lenproAB * lenproAB;
            lenproACsq = lenproAC * lenproAC;
            lenproBCsq = lenproBC * lenproBC;

    // check each vector against the corresponding constraint
    // if it is violated, calculate the additional "forces of constraint"
    // necessary to fix it

    // O - H1 vector

            if (((fabs(lenproAB - rOH) < tolerance) &&
                 (fabs(lenproAC - rOH) < tolerance)) &&
                 (fabs(lenproBC - rHH) < tolerance)) {
     
    // all constraints are obeyed
                violations = 0;

            } else {

    // some constraints violated, go to matrix routine to update fcons
                violations = 1;

                mtrxA[0][0] = mOH * vecAB.DotProduct(proAB);
		mtrxA[0][1] = mO  * vecAC.DotProduct(proAB);
		mtrxA[0][2] =-mH  * vecBC.DotProduct(proAB);
		mtrxA[1][0] = mO  * vecAB.DotProduct(proAC);
		mtrxA[1][1] = mOH * vecAC.DotProduct(proAC);
		mtrxA[1][2] = mH  * vecBC.DotProduct(proAC);
		mtrxA[2][0] =-mH  * vecAB.DotProduct(proBC);
		mtrxA[2][1] = mH  * vecAC.DotProduct(proBC);
		mtrxA[2][2] = mHH * vecBC.DotProduct(proBC);

     // invert the matrix, using cramer's rule

    double det = mtrxA[0][0]*(mtrxA[1][1]*mtrxA[2][2]-mtrxA[2][1]*mtrxA[1][2])
                -mtrxA[0][1]*(mtrxA[1][0]*mtrxA[2][2]-mtrxA[2][0]*mtrxA[1][2])
                +mtrxA[0][2]*(mtrxA[1][0]*mtrxA[2][1]-mtrxA[2][0]*mtrxA[1][1]);

     // should maybe check magnitude of determinant here

         double detinv = 1.0/det;

       minvA[0][0] = (mtrxA[1][1]*mtrxA[2][2]-mtrxA[2][1]*mtrxA[1][2])*detinv;
       minvA[0][1] =-(mtrxA[0][1]*mtrxA[2][2]-mtrxA[2][1]*mtrxA[0][2])*detinv;
       minvA[0][2] = (mtrxA[0][1]*mtrxA[1][2]-mtrxA[1][1]*mtrxA[0][2])*detinv;
       minvA[1][0] =-(mtrxA[1][0]*mtrxA[2][2]-mtrxA[2][0]*mtrxA[1][2])*detinv;
       minvA[1][1] = (mtrxA[0][0]*mtrxA[2][2]-mtrxA[2][0]*mtrxA[0][2])*detinv;
       minvA[1][2] =-(mtrxA[0][0]*mtrxA[1][2]-mtrxA[1][0]*mtrxA[0][2])*detinv;
       minvA[2][0] = (mtrxA[1][0]*mtrxA[2][1]-mtrxA[2][0]*mtrxA[1][1])*detinv;
       minvA[2][1] =-(mtrxA[0][0]*mtrxA[2][1]-mtrxA[2][0]*mtrxA[0][1])*detinv;
       minvA[2][2] = (mtrxA[0][0]*mtrxA[1][1]-mtrxA[1][0]*mtrxA[0][1])*detinv;
                    
     // update the g-vector using the inverted matrix

		for (int j = 0; j < 3; j++) {
			gvec[j] -= (minvA[j][0]*(rOHsq - lenproABsq)
			           +minvA[j][1]*(rOHsq - lenproACsq)
			           +minvA[j][2]*(rHHsq - lenproBCsq)
			           )/(2.0*dt);
		}

     // update the forces of constraint using the new g-vector

      fconA= mO*(gvec[0]*vecAB+gvec[1]*vecAC);
      fconB=-mH*(gvec[0]*vecAB-gvec[2]*vecBC);
      fconC=-mH*(gvec[1]*vecAC+gvec[2]*vecBC);

     // done with m-shake loop
            }

            shake_iterations++;

        }
    //
    // check if we hit max_iterations
    //
        if (shake_iterations >= max_iterations) {
            BegLogLine( 1 )
              << "Rigid3SiteShake(MDBM&)"
              << " vecAB=" << vecAB
              << " vecAC=" << vecAC
              << " vecBC=" << vecBC
              << " vA=" << vA
              << " vB=" << vB
              << " vC=" << vC
              << " Shake iterations exceeded maximum"
              << EndLogLine ;

            PLATFORM_ABORT("Rigid3SiteShake(MDBM&) Shake iterations exceeded maximum");
        } else {

    // done with SHAKE iterations, converged before the iteration limit
    // now place influence of force of constraint in velocity arrays
    // remember fcon[A,B,C] are not real forces but f/m * dt/2
    //
    // note we are not including a virial contribution from the force of
    // constraint. . .

    // suits - originally the fcon's were added, but it seemed to push the wrong way.

            vA += fconA;
            vB += fconB;
            vC += fconC;

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteShake(MDBM&)"
              << " fconA=" << fconA
              << " fconB=" << fconB
              << " fconC=" << fconC
              << " shake_iterations=" << shake_iterations
              << EndLogLine ;

            aMDVM.ReportVelocity( MDVM::SiteB, vA );
            aMDVM.ReportVelocity( MDVM::SiteA, vB );
            aMDVM.ReportVelocity( MDVM::SiteC, vC );
        }
#if 0
	BegLogLine(0)
	<< "AFTER SHAKE" << aMDVM.GetSimTick() << " " << aMDVM.GetSiteId(MDVM::SiteA)  << "\n"  
	<< rA << " " << rB << " " << rC << "\n" 
	<< vA << " " << vB << " " << vC << EndLogLine;
#endif
        return;
    }
#else
    template <class MDVM, class WATER> static inline void Rigid3SiteShake(
        MDVM &aMDVM)
    {

    // JWP August 2001

    // does FIRST velocity halfstep update w/ SHAKE for rigid water
    // adds dt/2 * f(t)/m to v(t)
    // calling routine is responsible to make sure f(t) is correct
    // velocity update is followed by SHAKE which adjusts the
    // velocities such that the positions r(t+dt) will satisfy the constraints

    //    const double  tolerance      = 0.00000001;
    //    const int     max_iterations = 1000;

    // really conservative default shake tolerance
    // 10^-6 can be OK but timestep dependent, no warranties, etc.

        // const double rOH   = 0.9572000;
        // const double rHH   = 1.5139006;
        const double rOH   = WATER::GetROH();
        const double rHH   = WATER::GetRHH();
        const double rOHsq = rOH * rOH;
        const double rHHsq = rHH * rHH;

        const double tolerance = UDF_State::Shake.Epsilon;
        const int    max_iterations = UDF_State::Shake.MaxIterations;

    // above are constraint lengths (and length^2) for TIPnP water models

    // suits - this routine previously assumed OHH order, and is now switched at
    // the MDVM interface level. Variable names were not changed.

        XYZ    vA = aMDVM.GetVelocity( MDVM::SiteB );
        XYZ    vB = aMDVM.GetVelocity( MDVM::SiteA );
        XYZ    vC = aMDVM.GetVelocity( MDVM::SiteC );

        double    mA = aMDVM.GetHalfInverseMass( MDVM::SiteB );
        double    mB = aMDVM.GetHalfInverseMass( MDVM::SiteA );
        double    mC = aMDVM.GetHalfInverseMass( MDVM::SiteC );

        double dt = aMDVM.GetDeltaT();

    // Shake algorithm pretty much direct from Bill Swope's little water code, JWP
    // current position vectors

        XYZ    vecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB );
        XYZ    vecAC = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteB );
        XYZ    vecBC = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteA );


        BegLogLine(PKFXLOG_SHAKE_RATTLE)
          << "Rigid3SiteShake(MDBM&)"
          << " vecAB=" << vecAB
          << " vecAC=" << vecAC
          << " vecBC=" << vecBC
          << " vA=" << vA
          << " vB=" << vB
          << " vC=" << vC
          << EndLogLine ;

    // start with forces of constraint set to zero

        XYZ    fconA = XYZ::ZERO_VALUE();
        XYZ    fconB = XYZ::ZERO_VALUE();
        XYZ    fconC = XYZ::ZERO_VALUE();

    // these are the "free flight" positions of the vectors
    // i.e. where the vectors end up without the force of constraint
    // these are out here to save adding in dt*(vA-vB) at each iteration
    // but could be moved in to save memory

        XYZ    freeAB = vecAB + dt * (vB - vA);
        XYZ    freeAC = vecAC + dt * (vC - vA);
        XYZ    freeBC = vecBC + dt * (vC - vB);

    // initially assume all 3 constraints are violated,
    // ensuring at least one pass through the loop

    // it's traditional to keep track of the number of iterations and
    // vomit if it passes the maximum. . .

    // since the 3 constraints are coupled, if we make adjustments
    // to fix any one constraint, we have to check them all again

        int shake_iterations = 0;
        int violations = 3;

        XYZ    proAB;
        XYZ    proAC;
        XYZ    proBC;

        double lenproAB;
        double lenproAC;
        double lenproBC;

        while ((violations > 0) && (shake_iterations < max_iterations)) {

    // project the current values of the interparticle vectors
    // (includes contributions from any force of constraint we have generated
    //  so far)

    //  suits - each fcon is already weighted by inverse mass.
    //  fcon's have velocity units
    //  (heavy ones will smaller velocity boost)

            proAB = freeAB +  dt * (fconB - fconA);
            proAC = freeAC +  dt * (fconC - fconA);
            proBC = freeBC +  dt * (fconC - fconB);

    // get the lengths of these vectors to compare against the constraints

            lenproAB = proAB.Length();
            lenproAC = proAC.Length();
            lenproBC = proBC.Length();

            int testAB = 1;
            int testAC = 1;
            int testBC = 1;

    // check each vector against the corresponding constraint
    // if it is violated, calculate the additional "forces of constraint"
    // necessary to fix it

    // O - H1 vector

            if (fabs(lenproAB - rOH) > tolerance) {
                double SdotR =  -2.0 * dt * proAB.DotProduct(vecAB) * (mA + mB);
                double G = (lenproAB*lenproAB - rOHsq)/SdotR;
                fconA -= G * mA * vecAB;
                fconB += G * mB * vecAB;
            } else {
                testAB = 0;
            }

    // O - H2 vector

            if (fabs(lenproAC - rOH) > tolerance) {
                double SdotR =  -2.0 * dt * proAC.DotProduct(vecAC) * (mA + mC);
                double G = (lenproAC*lenproAC - rOHsq)/SdotR;
                fconA -= G * mA * vecAC;
                fconC += G * mC * vecAC;
            } else {
                testAC = 0;
            }

    // H1 - H2 vector
    // note that since both particles have the same mass we don't
    // have to mass-weight the force of constraint, just divide it by 2
    // suits - this is done with the extra 1/2 factor in G

            if (fabs(lenproBC - rHH) > tolerance) {
                double SdotR =  -2.0 * dt * proBC.DotProduct(vecBC) * (mB + mC);
                double G = (lenproBC*lenproBC - rHHsq)/SdotR;
                fconB -= G * mB * vecBC;
                fconC += G * mC * vecBC;
            } else {
                testBC = 0;
            }

            violations = testAB + testAC + testBC;
            shake_iterations++;

        }
    //
    // check if we hit max_iterations
    //
        if (shake_iterations >= max_iterations) {
            BegLogLine( 1 )
              << "Rigid3SiteShake(MDBM&)"
              << " vecAB=" << vecAB
              << " vecAC=" << vecAC
              << " vecBC=" << vecBC
              << " vA=" << vA
              << " vB=" << vB
              << " vC=" << vC
              << " Shake iterations exceeded maximum"
              << EndLogLine ;

            PLATFORM_ABORT("Rigid3SiteShake(MDBM&) Shake iterations exceeded maximum");
        } else {

    // done with SHAKE iterations, converged before the iteration limit
    // now place influence of force of constraint in velocity arrays
    // remember fcon[A,B,C] are not real forces but f/m * dt/2
    //
    // note we are not including a virial contribution from the force of
    // constraint. . .

    // suits - originally the fcon's were added, but it seemed to push the wrong way.

            vA += fconA;
            vB += fconB;
            vC += fconC;

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteShake(MDBM&)"
              << " fconA=" << fconA
              << " fconB=" << fconB
              << " fconC=" << fconC
              << " shake_iterations=" << shake_iterations
              << EndLogLine ;

            aMDVM.ReportVelocity( MDVM::SiteB, vA );
            aMDVM.ReportVelocity( MDVM::SiteA, vB );
            aMDVM.ReportVelocity( MDVM::SiteC, vC );
        }

        return;
    }
#endif

#if defined(PERFORMANCE_RATTLE)
    template <class MDVM, class WATER> static inline void Rigid3SiteRattle( MDVM &aMDVM )
    {

    // JWP August 2001

    // normal velocity update is followed by RATTLE, which adjusts the
    // velocities v(t+dt) so they have no components along the constraints

        // const double tolerance      = 0.00000001;
        // const int    max_iterations = 1000;
        int check = 0;

    // really conservative default SHAKE tolerance
    // 10^-6 can be OK but timestep dependent, no warranties, etc.

        // const double rOH   = 0.9572000;
        // const double rHH   = 1.5139006;
        const double rOH = WATER::GetROH();
        const double rHH = WATER::GetRHH();
        const double rOHsq = rOH * rOH;
        const double rHHsq = rHH * rHH;

        const double tolerance = UDF_State::Rattle.Epsilon;
        const int    max_iterations = UDF_State::Rattle.MaxIterations;

    // above are constraint lengths (and length^2) for TIPnP water models

    // suits - this routine previously assumed OHH order, and has been
    // switched to HOH order at the MDVM interface level.  Variable names not changed


    XYZ rA = aMDVM.GetPosition(MDVM::SiteB);
    XYZ rB = aMDVM.GetPosition(MDVM::SiteA);
    XYZ rC = aMDVM.GetPosition(MDVM::SiteC);

        XYZ    vA = aMDVM.GetVelocity( MDVM::SiteB );
        XYZ    vB = aMDVM.GetVelocity( MDVM::SiteA );
        XYZ    vC = aMDVM.GetVelocity( MDVM::SiteC );


        double    mA = aMDVM.GetHalfInverseMass( MDVM::SiteB );
        double    mB = aMDVM.GetHalfInverseMass( MDVM::SiteA );
        double    mC = aMDVM.GetHalfInverseMass( MDVM::SiteC );

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteRattle(MDBM&)"
              << " rathead mA=" << mA
              << " mB=" << mB
              << " mC=" << mC
              << " rA=" << rA
              << " rB=" << rB
              << " rC=" << rC
              << " vA=" << vA
              << " vB=" << vB
              << " vC=" << vC
              << EndLogLine ;
    // done with normal UpdateVelocityHalfStep, on to RATTLE
    // pretty much direct from Bill Swope's little water code, JWP

        // Half factor since we want things good after half time step
        double dt = aMDVM.GetDeltaT();

    // convert tolerance to velocities. . .

        double velocity_tolerance = tolerance/dt;

    // current position vectors at t+dt

        XYZ    vecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB );
        XYZ    vecAC = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteB );
        XYZ    vecBC = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteA );

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteRattle(MDBM&)"
              << " rattl vecAB=" << vecAB
              << " vecAC=" << vecAC
              << " vecBC=" << vecBC
              << " vA=" << vA
              << " vB=" << vB
              << " vC=" << vC
              << EndLogLine ;

    // start with forces of constraint set to zero

        XYZ    fconA = XYZ::ZERO_VALUE();
        XYZ    fconB = XYZ::ZERO_VALUE();
        XYZ    fconC = XYZ::ZERO_VALUE();

    // get the relative velocities along each constraint

        XYZ    velAB = vA - vB;
        XYZ    velAC = vA - vC;
        XYZ    velBC = vB - vC;

    // initially assume all 3 constraints are violated,
    // ensuring at least one pass through the loop

    // it's traditional to keep track of the number of iterations and
    // vomit if it passes the maximum. . .

    // since the 3 constraints are coupled, if we make adjustments
    // to fix any one constraint, we have to check them all again

        int rattle_iterations = 0;
        int violations        = 3;

        const double vtrOH = velocity_tolerance*rOH ;
        const double vtrOH2 = vtrOH*vtrOH ;
        const double vtrHH = velocity_tolerance*rHH ;
        const double vtrHH2 = vtrHH*vtrHH ;
        double ABok ;
        double ACok ;
        double BCok ;
        for (rattle_iterations=0; rattle_iterations < max_iterations ; rattle_iterations += 1) {
    // project the current values of the interparticle velocities
    // (includes contributions from any force of constraint we have generated
    // so far)

    // suits - removed factor of dt that multiplied delta fcon's
    // As in shake, fcon's have velocity units and are pre-weighted by inverse mass

            //    XYZ    proAB = vecAB + dt*(velAB + fconA - fconB);
            //    XYZ    proAC = vecAC + dt*(velAC + fconA - fconC);
            //    XYZ    proBC = velBC + dt*(velBC + fconB - fconC);

         XYZ provAB = velAB + (fconA - fconB);
         XYZ provAC = velAC + (fconA - fconC);
         XYZ provBC = velBC + (fconB - fconC);

         // XYZ proAB = vecAB + dt*provAB;
         // XYZ proAC = vecAC + dt*provAC;
         // XYZ proBC = vecBC + dt*provBC;


            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteRattle(MDBM&)"
              << " fcon_pro rattle_iterations=" << rattle_iterations
              << " fconA=" << fconA
              << " fconB=" << fconB
              << " fconC=" << fconC
              << " provAB=" << provAB
              << " provAC=" << provAC
              << " provBC=" << provBC
              << EndLogLine ;

    // get the dot product of each velocity on the corresponding
    // constraint to check against veloc_tolerance

         // double devproAB = proAB.DotProduct(vecAB);
         // double devproAC = proAC.DotProduct(vecAC);
         // double devproBC = proBC.DotProduct(vecBC);

         // suits - below seemed to take > 100 iterations
         // double devproAB = proAB.DotProduct(provAB);
         // double devproAC = proAC.DotProduct(provAC);
         // double devproBC = proBC.DotProduct(provBC);

         double devproAB = vecAB.DotProduct(provAB);
         double devproAC = vecAC.DotProduct(provAC);
         double devproBC = vecBC.DotProduct(provBC);

    // check each vector against the corresponding constraint
    // if it is violated, calculate the additional "forces of constraint"
    // necessary to fix it

    // O - H1 vector

        {
            double G = 0.5 * devproAB * recip(rOHsq * (mA + mB));
            fconA -= G * mA * vecAB;
            fconB += G * mB * vecAB;
        }

        ABok = devproAB*devproAB - vtrOH2 ;



    // O - H2 vector

        {
            double G = 0.5 * devproAC * recip(rOHsq * (mA + mC));
            fconA -= G * mA * vecAC;
            fconC += G * mC * vecAC;
        }

        ACok = devproAC*devproAC - vtrOH2 ;

    // H1 - H2 vector
    // note that since both particles have the same mass we don't
    // have to mass-weight the force of constraint, just divide it by 2
    // suits - done by explicit factor of 1/2 in G

        {
            double G =  0.5 * devproBC * recip(rHHsq * (mB + mC));
            fconB -= G * mB * vecBC;
            fconC += G * mC * vecBC;
        }

        BCok = devproBC*devproBC - vtrHH2 ;

        double r0 = fsel(ABok,ABok,ACok) ;
        double r1 = fsel(r0,r0,BCok) ;
        if (r1 <= 0.0 ) break ;
//      if (ABok <= 0.0 && ACok <= 0.0 && BCok <= 0.0 ) break ;


        }

    // check if we hit max_iterations

        if (ABok > 0.0 || ACok > 0.0 || BCok > 0.0 ) {
            BegLogLine( 1 )
              << "Rigid3SiteRattle(MDBM&)"
              << " Rattle iterations exceeded maximum"
              << " rA: " << rA
              << " rB: " << rB
              << " rC: " << rB
              << " vA: " << vA
              << " vB: " << vB
              << " vC: " << vB
              << EndLogLine ;

            PLATFORM_ABORT("Rigid3SiteRattle(MDBM&) Rattle iterations exceeded maximum");
        } else {

    // done with RATTLE iterations, converged before the iteration limit
    // now place influence of force of constraint in velocity arrays
    // remember fcon[A,B,C] are not real forces but f/m * dt/2

    // note we are not including a virial contribution from the force of
    // constraint. . .

    // suits - note that here the fcon's are added, which is opposite to
    // shake.  We should check this.

            vA += fconA;
            vB += fconB;
            vC += fconC;

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteRattle(MDBM&)"
              << " newvabc_fcons vA=" << vA
              << " vB=" << vB
              << " vC=" << vC
              << " fconA=" << fconA
              << " fconB=" << fconB
              << " fconC=" << fconC
              << " rattle_iterations=" << rattle_iterations
              << EndLogLine ;

            aMDVM.ReportVelocity( MDVM::SiteB, vA );
            aMDVM.ReportVelocity( MDVM::SiteA, vB );
            aMDVM.ReportVelocity( MDVM::SiteC, vC );
        }

        return;

    }
#elif defined(MATRIX_RATTLE)
    template <class MDVM, class WATER> static inline void Rigid3SiteRattle( MDVM &aMDVM )
    {

    // JWP August 2001

    // normal velocity update is followed by RATTLE, which adjusts the
    // velocities v(t+dt) so they have no components along the constraints

        // const double tolerance      = 0.00000001;
        // const int    max_iterations = 1000;
        int check = 0;

    // really conservative default SHAKE tolerance
    // 10^-6 can be OK but timestep dependent, no warranties, etc.

        // const double rOH   = 0.9572000;
        // const double rHH   = 1.5139006;
        const double rOH = WATER::GetROH();
        const double rHH = WATER::GetRHH();
        const double rOHsq = rOH * rOH;
        const double rHHsq = rHH * rHH;

        const double tolerance = UDF_State::Rattle.Epsilon;
        const int    max_iterations = UDF_State::Rattle.MaxIterations;

    // above are constraint lengths (and length^2) for TIPnP water models

    // suits - this routine previously assumed OHH order, and has been
    // switched to HOH order at the MDVM interface level.  Variable names not changed

	// internal site A = O  = external site B
        // internal site B = H1 = external site A
        // internal site C = H2 = external site C

        XYZ rA = aMDVM.GetPosition(MDVM::SiteB);
        XYZ rB = aMDVM.GetPosition(MDVM::SiteA);
        XYZ rC = aMDVM.GetPosition(MDVM::SiteC);

        XYZ vA = aMDVM.GetVelocity( MDVM::SiteB );
        XYZ vB = aMDVM.GetVelocity( MDVM::SiteA );
        XYZ vC = aMDVM.GetVelocity( MDVM::SiteC );

#if 0
	BegLogLine(0)
	<< "ORIG RATTLE" << aMDVM.GetSimTick() << " " << aMDVM.GetSiteId(MDVM::SiteA)  << "\n"  
	<< rA << " " << rB << " " << rC << "\n" 
	<< vA << " " << vB << " " << vC << EndLogLine;
#endif
        double mA = aMDVM.GetHalfInverseMass( MDVM::SiteB );
        double mB = aMDVM.GetHalfInverseMass( MDVM::SiteA );
        double mC = aMDVM.GetHalfInverseMass( MDVM::SiteC );

    // pitera -- we need some combined inverse masses to build the A-matrix

        double    mH = mB + mC;
        double    mO = 2.0 * mA;
        double    mOH = mH + mO;
        double    mHH = mH + mH;

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteRattle(MDBM&)"
              << " rathead mA=" << mA
              << " mB=" << mB
              << " mC=" << mC
              << " rA=" << rA
              << " rB=" << rB
              << " rC=" << rC
              << " vA=" << vA
              << " vB=" << vB
              << " vC=" << vC
              << EndLogLine ;
    // done with normal UpdateVelocityHalfStep, on to RATTLE
    // pretty much direct from Bill Swope's little water code, JWP

        double dt = aMDVM.GetDeltaT();

    // convert tolerance to velocities. . .

        double velocity_tolerance = tolerance/dt;

    // current position vectors at t+dt

        XYZ    vecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB );
        XYZ    vecAC = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteB );
        XYZ    vecBC = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteA );

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteRattle(MDBM&)"
              << " rattl vecAB=" << vecAB
              << " vecAC=" << vecAC
              << " vecBC=" << vecBC
              << " vA=" << vA
              << " vB=" << vB
              << " vC=" << vC
              << EndLogLine ;

    // start with forces of constraint set to zero

        XYZ    fconA = XYZ::ZERO_VALUE();
        XYZ    fconB = XYZ::ZERO_VALUE();
        XYZ    fconC = XYZ::ZERO_VALUE();

    // get the relative velocities along each constraint

        XYZ    velAB = vB - vA;
        XYZ    velAC = vC - vA;
        XYZ    velBC = vC - vB;

    // M-RATTLE is noniterative, so just assume there are violations
    // and go through the routine

    if (!WATER::IsRattleInverseMatrixDefined)
    {
	WATER::IsRattleInverseMatrixDefined = 1;
	// note that the A-matrix & its inverse should be constant for
	// all rigid waters -- we calculate it here but we shouldn't have
	// to, really. . . could be done @ startup

	// setup A-matrix, inverse A-matrix

        double mtrxA[3][3];

        // JWP note that this is unnecessary, could pre-build and
        // invert matrix

	// cosine of angle
	double cs = 1.0 - 0.5*WATER::GetRHH()*WATER::GetRHH()/WATER::GetROH()/WATER::GetROH();
	double ss = sqrt(1.0-cs*cs);

	XYZ rrA = XYZ::ZERO_VALUE();

	XYZ rrB, rrC;
	rrC.mX = WATER::GetROH();
	rrC.mY = rrC.mZ = 0;
	rrB.mX = WATER::GetROH()*cs;
	rrB.mY = WATER::GetROH()*ss;
	rrB.mZ = 0;

        XYZ pHH  = rrC-rrB;
        XYZ pOH1 = rrB-rrA;
        XYZ pOH2 = rrC-rrA;

        mtrxA[0][0] = mOH * pOH1.DotProduct(pOH1); 
        mtrxA[0][1] = mO  * pOH1.DotProduct(pOH2);
        mtrxA[0][2] = -mH * pOH1.DotProduct(pHH);
        mtrxA[1][0] = mO  * pOH1.DotProduct(pOH2);
        mtrxA[1][1] = mOH * pOH2.DotProduct(pOH2);
        mtrxA[1][2] = mH  * pOH2.DotProduct(pHH);
        mtrxA[2][0] = -mH * pOH1.DotProduct(pHH);
        mtrxA[2][1] = mH  * pOH2.DotProduct(pHH);
        mtrxA[2][2] = mHH * pHH .DotProduct(pHH);

	// invert the A-matrix using Cramer's rule

	double det = mtrxA[0][0]*(mtrxA[1][1]*mtrxA[2][2]-mtrxA[2][1]*mtrxA[1][2])
		    -mtrxA[0][1]*(mtrxA[1][0]*mtrxA[2][2]-mtrxA[2][0]*mtrxA[1][2])
		    +mtrxA[0][2]*(mtrxA[1][0]*mtrxA[2][1]-mtrxA[2][0]*mtrxA[1][1]);

	// should maybe check magnitude of determinant here

        double detinv = 1.0/det;

	WATER::RIM[0][0] = (mtrxA[1][1]*mtrxA[2][2]-mtrxA[2][1]*mtrxA[1][2])*detinv;
	WATER::RIM[0][1] =-(mtrxA[0][1]*mtrxA[2][2]-mtrxA[2][1]*mtrxA[0][2])*detinv;
	WATER::RIM[0][2] = (mtrxA[0][1]*mtrxA[1][2]-mtrxA[1][1]*mtrxA[0][2])*detinv;
	WATER::RIM[1][0] =-(mtrxA[1][0]*mtrxA[2][2]-mtrxA[2][0]*mtrxA[1][2])*detinv;
	WATER::RIM[1][1] = (mtrxA[0][0]*mtrxA[2][2]-mtrxA[2][0]*mtrxA[0][2])*detinv;
	WATER::RIM[1][2] =-(mtrxA[0][0]*mtrxA[1][2]-mtrxA[1][0]*mtrxA[0][2])*detinv;
	WATER::RIM[2][0] = (mtrxA[1][0]*mtrxA[2][1]-mtrxA[2][0]*mtrxA[1][1])*detinv;
	WATER::RIM[2][1] =-(mtrxA[0][0]*mtrxA[2][1]-mtrxA[2][0]*mtrxA[0][1])*detinv;
	WATER::RIM[2][2] = (mtrxA[0][0]*mtrxA[1][1]-mtrxA[1][0]*mtrxA[0][1])*detinv;

#if 0
	BegLogLine(0) << "RIM MATRIX" << EndLogLine;
	for (int i=0; i<3; i++) {
	    for (int j=0; j<3; j++) {
		BegLogLine(0) << WATER::RIM[i][j] << EndLogLine;
	    }
	}
#endif
    }
                    
        
        int rattle_iterations = 0;
        int violations        = 3;

    // project the current values of the interparticle velocities
    // (includes contributions from any force of constraint we have generated
    // so far)

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteRattle(MDBM&)"
              << " fcon_pro rattle_iterations=" << rattle_iterations
              << " fconA=" << fconA
              << " fconB=" << fconB
              << " fconC=" << fconC
              << " velAB=" << velAB
              << " velAC=" << velAC
              << " velBC=" << velBC
              << EndLogLine ;

    // get the dot product of each velocity on the corresponding
    // constraint to check against veloc_tolerance

        double devproAB = vecAB.DotProduct(velAB);
        double devproAC = vecAC.DotProduct(velAC);
        double devproBC = vecBC.DotProduct(velBC);

        double gvec[3];

        for (int j = 0; j < 3; j++) {
		gvec[j] =  WATER::RIM[j][0]*devproAB
                          +WATER::RIM[j][1]*devproAC
                          +WATER::RIM[j][2]*devproBC;
	}

    //    for (int j = 0; j < 3; j++) {
    //          fconA[j] = mO*(gvec[1]*vecAB[j]+gvec[1]*vecAC[j]);
    //          fconB[j] =-mH*(gvec[1]*vecAB[j]-gvec[3]*vecBC[j]);
    //          fconC[j] =-mH*(gvec[1]*vecAC[j]+gvec[3]*vecBC[j]);
    //    } 
        
        fconA = mO*(gvec[0]*vecAB+gvec[1]*vecAC);
        fconB =-mH*(gvec[0]*vecAB-gvec[2]*vecBC);
        fconC =-mH*(gvec[1]*vecAC+gvec[2]*vecBC);

    // we are done setting up fcons

    // check if we hit max_iterations

        if (rattle_iterations >= max_iterations) {
            BegLogLine( 1 )
              << "Rigid3SiteRattle(MDBM&)"
              << " rA: " << rA
              << " rB: " << rB
              << " rC: " << rB
              << " vA: " << vA
              << " vB: " << vB
              << " vC: " << vB
              << " Rattle iterations exceeded maximum"
              << EndLogLine ;

            PLATFORM_ABORT("Rigid3SiteRattle(MDBM&) Rattle iterations exceeded maximum");
        } else {

    // done with RATTLE iterations, converged before the iteration limit
    // now place influence of force of constraint in velocity arrays
    // remember fcon[A,B,C] are not real forces but f/m * dt/2

    // note we are not including a virial contribution from the force of
    // constraint. . .

    // suits - note that here the fcon's are added, which is opposite to
    // shake.  We should check this.

            vA += fconA;
            vB += fconB;
            vC += fconC;

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteRattle(MDBM&)"
              << " newvabc_fcons vA=" << vA
              << " vB=" << vB
              << " vC=" << vC
              << " fconA=" << fconA
              << " fconB=" << fconB
              << " fconC=" << fconC
              << " rattle_iterations=" << rattle_iterations
              << EndLogLine ;

            aMDVM.ReportVelocity( MDVM::SiteB, vA );
            aMDVM.ReportVelocity( MDVM::SiteA, vB );
            aMDVM.ReportVelocity( MDVM::SiteC, vC );
#if 0
	    BegLogLine(0)
	    << "AFTER RATTLE " << aMDVM.GetSimTick() << " " << aMDVM.GetSiteId(MDVM::SiteA)  << "\n"  
	    << rA << " " << rB << " " << rC << "\n" 
	    << vA << " " << vB << " " << vC << EndLogLine;
#endif
        }

        return;

    }
#else
    template <class MDVM, class WATER> static inline void Rigid3SiteRattle( MDVM &aMDVM )
    {

    // JWP August 2001

    // normal velocity update is followed by RATTLE, which adjusts the
    // velocities v(t+dt) so they have no components along the constraints

        // const double tolerance      = 0.00000001;
        // const int    max_iterations = 1000;
        int check = 0;

    // really conservative default SHAKE tolerance
    // 10^-6 can be OK but timestep dependent, no warranties, etc.

        // const double rOH   = 0.9572000;
        // const double rHH   = 1.5139006;
        const double rOH = WATER::GetROH();
        const double rHH = WATER::GetRHH();
        const double rOHsq = rOH * rOH;
        const double rHHsq = rHH * rHH;

        const double tolerance = UDF_State::Rattle.Epsilon;
        const int    max_iterations = UDF_State::Rattle.MaxIterations;

    // above are constraint lengths (and length^2) for TIPnP water models

    // suits - this routine previously assumed OHH order, and has been
    // switched to HOH order at the MDVM interface level.  Variable names not changed


    XYZ rA = aMDVM.GetPosition(MDVM::SiteB);
    XYZ rB = aMDVM.GetPosition(MDVM::SiteA);
    XYZ rC = aMDVM.GetPosition(MDVM::SiteC);

        XYZ    vA = aMDVM.GetVelocity( MDVM::SiteB );
        XYZ    vB = aMDVM.GetVelocity( MDVM::SiteA );
        XYZ    vC = aMDVM.GetVelocity( MDVM::SiteC );


        double    mA = aMDVM.GetHalfInverseMass( MDVM::SiteB );
        double    mB = aMDVM.GetHalfInverseMass( MDVM::SiteA );
        double    mC = aMDVM.GetHalfInverseMass( MDVM::SiteC );

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteRattle(MDBM&)"
              << " rathead mA=" << mA
              << " mB=" << mB
              << " mC=" << mC
              << " rA=" << rA
              << " rB=" << rB
              << " rC=" << rC
              << " vA=" << vA
              << " vB=" << vB
              << " vC=" << vC
              << EndLogLine ;
    // done with normal UpdateVelocityHalfStep, on to RATTLE
    // pretty much direct from Bill Swope's little water code, JWP

        // Half factor since we want things good after half time step
        double dt = aMDVM.GetDeltaT();

    // convert tolerance to velocities. . .

        double velocity_tolerance = tolerance/dt;

    // current position vectors at t+dt

        XYZ    vecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB );
        XYZ    vecAC = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteB );
        XYZ    vecBC = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteA );

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteRattle(MDBM&)"
              << " rattl vecAB=" << vecAB
              << " vecAC=" << vecAC
              << " vecBC=" << vecBC
              << " vA=" << vA
              << " vB=" << vB
              << " vC=" << vC
              << EndLogLine ;

    // start with forces of constraint set to zero

        XYZ    fconA = XYZ::ZERO_VALUE();
        XYZ    fconB = XYZ::ZERO_VALUE();
        XYZ    fconC = XYZ::ZERO_VALUE();

    // get the relative velocities along each constraint

        XYZ    velAB = vA - vB;
        XYZ    velAC = vA - vC;
        XYZ    velBC = vB - vC;

    // initially assume all 3 constraints are violated,
    // ensuring at least one pass through the loop

    // it's traditional to keep track of the number of iterations and
    // vomit if it passes the maximum. . .

    // since the 3 constraints are coupled, if we make adjustments
    // to fix any one constraint, we have to check them all again

        int rattle_iterations = 0;
        int violations        = 3;

        while ((violations > 0) && (rattle_iterations < max_iterations)) {

    // project the current values of the interparticle velocities
    // (includes contributions from any force of constraint we have generated
    // so far)

    // suits - removed factor of dt that multiplied delta fcon's
    // As in shake, fcon's have velocity units and are pre-weighted by inverse mass

            //    XYZ    proAB = vecAB + dt*(velAB + fconA - fconB);
            //    XYZ    proAC = vecAC + dt*(velAC + fconA - fconC);
            //    XYZ    proBC = velBC + dt*(velBC + fconB - fconC);

         XYZ provAB = velAB + (fconA - fconB);
         XYZ provAC = velAC + (fconA - fconC);
         XYZ provBC = velBC + (fconB - fconC);

         // XYZ proAB = vecAB + dt*provAB;
         // XYZ proAC = vecAC + dt*provAC;
         // XYZ proBC = vecBC + dt*provBC;


            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteRattle(MDBM&)"
              << " fcon_pro rattle_iterations=" << rattle_iterations
              << " fconA=" << fconA
              << " fconB=" << fconB
              << " fconC=" << fconC
              << " provAB=" << provAB
              << " provAC=" << provAC
              << " provBC=" << provBC
              << EndLogLine ;

    // get the dot product of each velocity on the corresponding
    // constraint to check against veloc_tolerance

         // double devproAB = proAB.DotProduct(vecAB);
         // double devproAC = proAC.DotProduct(vecAC);
         // double devproBC = proBC.DotProduct(vecBC);

         // suits - below seemed to take > 100 iterations
         // double devproAB = proAB.DotProduct(provAB);
         // double devproAC = proAC.DotProduct(provAC);
         // double devproBC = proBC.DotProduct(provBC);

         double devproAB = vecAB.DotProduct(provAB);
         double devproAC = vecAC.DotProduct(provAC);
         double devproBC = vecBC.DotProduct(provBC);

        int testAB = 1;
        int testAC = 1;
        int testBC = 1;

    // check each vector against the corresponding constraint
    // if it is violated, calculate the additional "forces of constraint"
    // necessary to fix it

    // O - H1 vector

        if (fabs(devproAB) > velocity_tolerance*rOH) {
            double G = 0.5 * devproAB / (rOHsq * (mA + mB));
            fconA -= G * mA * vecAB;
            fconB += G * mB * vecAB;
        } else {
            testAB = 0;
        }

    // O - H2 vector

        if (fabs(devproAC) > velocity_tolerance*rOH) {
            double G = 0.5 * devproAC / (rOHsq * (mA + mC));
            fconA -= G * mA * vecAC;
            fconC += G * mC * vecAC;
        } else {
            testAC = 0;
        }

    // H1 - H2 vector
    // note that since both particles have the same mass we don't
    // have to mass-weight the force of constraint, just divide it by 2
    // suits - done by explicit factor of 1/2 in G

        if (fabs(devproBC) > velocity_tolerance*rHH) {
            double G =  0.5 * devproBC / (rHHsq * (mB + mC));
            fconB -= G * mB * vecBC;
            fconC += G * mC * vecBC;
        } else {
            testBC = 0;
        }

        violations = testAB + testAC + testBC;
        rattle_iterations++;

        }

    // check if we hit max_iterations

        if (rattle_iterations >= max_iterations) {
            BegLogLine( 1 )
              << "Rigid3SiteRattle(MDBM&)"
              << " rA: " << rA
              << " rB: " << rB
              << " rC: " << rB
              << " vA: " << vA
              << " vB: " << vB
              << " vC: " << vB
              << " Rattle iterations exceeded maximum"
              << EndLogLine ;

            PLATFORM_ABORT("Rigid3SiteRattle(MDBM&) Rattle iterations exceeded maximum");
        } else {

    // done with RATTLE iterations, converged before the iteration limit
    // now place influence of force of constraint in velocity arrays
    // remember fcon[A,B,C] are not real forces but f/m * dt/2

    // note we are not including a virial contribution from the force of
    // constraint. . .

    // suits - note that here the fcon's are added, which is opposite to
    // shake.  We should check this.

            vA += fconA;
            vB += fconB;
            vC += fconC;

            BegLogLine(PKFXLOG_SHAKE_RATTLE)
              << "Rigid3SiteRattle(MDBM&)"
              << " newvabc_fcons vA=" << vA
              << " vB=" << vB
              << " vC=" << vC
              << " fconA=" << fconA
              << " fconB=" << fconB
              << " fconC=" << fconC
              << " rattle_iterations=" << rattle_iterations
              << EndLogLine ;

            aMDVM.ReportVelocity( MDVM::SiteB, vA );
            aMDVM.ReportVelocity( MDVM::SiteA, vB );
            aMDVM.ReportVelocity( MDVM::SiteC, vC );
        }

        return;

    }
#endif


    template <class MDVM, class WATER> static inline void Rigid3SiteInit( MDVM &aMDVM )
    {
        // Peformance not important in this function since it's only run at startup

        const double rOH = WATER::GetROH();
        const double rHH = WATER::GetRHH();
        const double rOHsq = rOH * rOH;
        const double rHHsq = rHH * rHH;
        const double rPERP = sqrt(rOHsq - rHHsq/4);

        XYZ rA = aMDVM.GetPosition(MDVM::SiteB);
        XYZ rB = aMDVM.GetPosition(MDVM::SiteA);
        XYZ rC = aMDVM.GetPosition(MDVM::SiteC);

        double  mA = 2/aMDVM.GetHalfInverseMass( MDVM::SiteB );
        double  mB = 2/aMDVM.GetHalfInverseMass( MDVM::SiteA );
        double  mC = 2/aMDVM.GetHalfInverseMass( MDVM::SiteC );

        // find com offset from O
        double rCOMShift = (mB + mC) * rPERP / (mA + mB + mC);

        XYZ rCOM = (mA*rA + mB*rB + mC*rC)/(mA + mB + mC);

        XYZ vAB = rB - rA;
        XYZ vAC = rC - rA;

        XYZ vNorm = vAC.crossProduct(vAB);
        vNorm = vNorm / vNorm.Length();

        XYZ vOCOM = rCOM - rA;
        XYZ vOCOMNorm = vOCOM / vOCOM.Length();

        XYZ vX = vOCOMNorm.crossProduct(vNorm);
        XYZ vXNorm = vX / vX.Length();

        XYZ pA = rCOM - rCOMShift * vOCOMNorm;
        XYZ pB = rCOM + ( rPERP - rCOMShift ) * vOCOMNorm - 0.5 * rHH * vXNorm;
        XYZ pC = rCOM + ( rPERP - rCOMShift ) * vOCOMNorm + 0.5 * rHH * vXNorm;

        aMDVM.ReportPosition( MDVM::SiteB, pA );
        aMDVM.ReportPosition( MDVM::SiteA, pB );
        aMDVM.ReportPosition( MDVM::SiteC, pC );

//         cout << "Position A: " << pA
//              << "Position B: " << pB
//              << "Position C: " << pC << endl;

        Rigid3SiteRattle<MDVM, WATER>( aMDVM );
        //        cout << "Finished Water Init" << endl;
        return;
    }

};

#ifndef MSD_COMPILE_CODE_ONLY
double UDF_HelperWater::WaterRigid::RIM[3][3];
int UDF_HelperWater::WaterRigid::IsRattleInverseMatrixDefined = 0;
#endif
#endif



