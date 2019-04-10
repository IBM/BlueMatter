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
 
#include <BlueMatter/MDVM_IF.hpp>

#include <BlueMatter/UDF_Registry.hpp>

class StdHarmonicAngle
  {
  public:

    static
    inline
    void
    Execute(   MDVM &aMDVM,
               const StdHarmonicAngle_Parameters           &aMSDP )
      {
      /////DETERMINE COS(THETA) = R12 DOT R32 / SQRT((R12**2)*(R32**2))

      // These lines get the distances from the MDVM.
      double DistanceAB = aMDVM.GetDistance( MDVM::SiteA, MDVM::SiteB );
      double DistanceAC = aMDVM.GetDistance( MDVM::SiteA, MDVM::SiteC );

      // These vectors will have been computed in the MDVM as a
      // byproduct of the above distances.
      XYZ VectorAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB );
      XYZ VectorAC = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteC );

      double COSTHE = ( VectorAB.DotProduct( VectorAC ) ) / (DistanceAB * DistanceAC);

      /////   DETERMINE THETA.  IT IS IN THE RANGE OF 0 TO PI RADIANS.

      double THETA = acos(COSTHE);

      aMDVM.SetEnergy( aMSDP.mSpringConstant *
                       (THETA - aMSDP.mEquilibriumAngle) * (THETA- aMSDP.mEquilibriumAngle) );

      double DEDTH = 2.0 * aMSDP.mSpringConstant * (THETA - aMSDP.mEquilibriumAngle);

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

      //    COMPUTE GRADIENTS OF ENERGY WITH RESPECT TO COMPONENTS OF VectorAB,RO2

      double    R12R = -1.0 /  VectorAB.DotProduct(VectorAB);
      double    R32R =  1.0 /  VectorAC.DotProduct(VectorAC);
      double    DTD12X = R12R*(RPL*DEDTH) * (VectorAB.mY*RP.mZ - VectorAB.mZ*RP.mY);
      double    DTD12Y = R12R*(RPL*DEDTH) * (VectorAB.mZ*RP.mX - VectorAB.mX*RP.mZ);
      double    DTD12Z = R12R*(RPL*DEDTH) * (VectorAB.mX*RP.mY - VectorAB.mY*RP.mX);
      double    DTD32X = R32R*(RPL*DEDTH) * (VectorAC.mY*RP.mZ - VectorAC.mZ*RP.mY);
      double    DTD32Y = R32R*(RPL*DEDTH) * (VectorAC.mZ*RP.mX - VectorAC.mX*RP.mZ);
      double    DTD32Z = R32R*(RPL*DEDTH) * (VectorAC.mX*RP.mY - VectorAC.mY*RP.mX);

      // COMPUTE FORCES DUE TO ANGLE ENERGY ON O, H1 AND H2

      XYZ ForceOnA;
      XYZ ForceOnB;
      XYZ ForceOnC;

      ForceOnB.mX =  DTD12X        ;
      ForceOnB.mY =  DTD12Y        ;
      ForceOnB.mZ =  DTD12Z        ;
      ForceOnA.mX = -DTD12X-DTD32X ;
      ForceOnA.mY = -DTD12Y-DTD32Y ;
      ForceOnA.mZ = -DTD12Z-DTD32Z ;
      ForceOnC.mX =         DTD32X ;
      ForceOnC.mY =         DTD32Y ;
      ForceOnC.mZ =         DTD32Z ;

      aMDVM.SetForce( MDVM::SiteB, ForceOnB );
      aMDVM.SetForce( MDVM::SiteA, ForceOnA );
      aMDVM.SetForce( MDVM::SiteC, ForceOnC );

      return;
      }
  };
