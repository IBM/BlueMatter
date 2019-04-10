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

class UreyBradley
  {
  public:

#if 0
    // THIS IS THE TYPE TO BE AGREED ON BETWEEN XML AND PROBSPEC.
    class Parameters
      {
      public:
        double mSpringConstant;
        double mEquilibriumDistance;
      };
    typedef AngleType Parameters;
#endif



    static
    inline
    void
    Execute(   MDVM &aMDVM,
               const UreyBradley_Parameters           &aMSDP )
      {
      // These lines get the distances from the MDVM.
      double DistanceBC = aMDVM.GetDistance( MDVM::SiteB, MDVM::SiteC );

      // These vectors will have been computed in the MDVM as a
      // byproduct of the above distances.
      XYZ VectorBC = aMDVM.GetVector( MDVM::SiteB, MDVM::SiteC );

      double DeltaUB  = DistanceBC - aMSDP.mEquilibriumDistance;

      double Energy = aMSDP.mSpringConstant * DeltaUB * DeltaUB;

      aMDVM.SetEnergy( Energy );

      double DEDR           = -2.0 * (aMSDP.mSpringConstant * DeltaUB) / DistanceBC;

      XYZ ForceOnSiteA;
      XYZ ForceOnSiteB;
      XYZ ForceOnSiteC;

      ForceOnSiteA.mX = 0.0;
      ForceOnSiteA.mY = 0.0;
      ForceOnSiteA.mZ = 0.0;
      ForceOnSiteB          =    VectorBC * DEDR;
      ForceOnSiteC          =   (VectorBC * DEDR) * (-1);

      aMDVM.SetForce( MDVM::SiteB, ForceOnSiteB );
      aMDVM.SetForce( MDVM::SiteA, ForceOnSiteA );
      aMDVM.SetForce( MDVM::SiteC, ForceOnSiteC );

      return;
      }
  };


