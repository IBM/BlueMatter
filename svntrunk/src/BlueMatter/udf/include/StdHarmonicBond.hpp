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

#include <BlueMatter/UDF_Binding.hpp>

class StdHarmonicBond  // Boing
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
    typedef BondType Parameters;
#else
#endif


    static
    inline
    void
    Execute( MDVM &aMDVM,
             const StdHarmonicBond_Parameters            &aMSDP )
      {

      double DeltaFromEquilibrium  = aMDVM.GetDistance( MDVM::SiteA, MDVM::SiteB ) - aMSDP.mEquilibriumDistance;

      aMDVM.SetEnergy(  aMSDP.mSpringConstant
                      * DeltaFromEquilibrium * DeltaFromEquilibrium );

      double DEDR  =
        -2.0 * ( aMSDP.mSpringConstant * DeltaFromEquilibrium ) / aMDVM.GetDistance( MDVM::SiteA, MDVM::SiteB);

      XYZ Force = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ) * DEDR;

      aMDVM.SetForce( MDVM::SiteA, Force );
      aMDVM.SetForce( MDVM::SiteB, Force * (-1) );

      }
  };
