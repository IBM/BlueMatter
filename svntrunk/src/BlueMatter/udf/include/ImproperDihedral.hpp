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


class ImproperDihedral
  {
  public:

    typedef XYZ CartesianVector;

    // THIS IS THE TYPE TO BE AGREED ON BETWEEN XML AND PROBSPEC.
#if 0
    class Parameters
      {
      public:
        double k0;
        double delta;
        int multiplicity;
      };
    typedef ImproperType Parameters;
#endif

    static
    inline
    void
    Execute(   MDVM &aMDVM,
               const ImproperDihedral_Parameters               &aMSDP )
      {

      // For now, just create the symbols used by the regressed code
      // and connect them with what is in the MDVM.
      // IF these access don't compile out, change references in code below.
      XYZ aSitePosition1 = aMDVM.GetPosition( MDVM::SiteA );
      XYZ aSitePosition2 = aMDVM.GetPosition( MDVM::SiteB );  // !!! CENTRAL SITE !!!
      XYZ aSitePosition3 = aMDVM.GetPosition( MDVM::SiteC );
      XYZ aSitePosition4 = aMDVM.GetPosition( MDVM::SiteD );
//****************************
    #if 0
    inline
    void
    ImproperDihedralForce(
                unsigned                      &aSimTick,
                const ImproperForceParameters &aIFP,
                SitePosition                  &aSitePosition1,
                SitePosition                  &aSitePosition2, // central atom
                SitePosition                  &aSitePosition3,
                SitePosition                  &aSitePosition4,
                ForceVector                   &aForceOnSite1,
                ForceVector                   &aForceOnSite2,
                ForceVector                   &aForceOnSite3,
                ForceVector                   &aForceOnSite4,
                double                        &aImproperTorsionEnergy )
      {
    #endif

      // the improper dihedral angle is defined to be between the plane
      // 1-2-3 and 3-2-4
      //printf("Improper parm= %f %f %f\n", aMSDP.Kpsi, aMSDP.multiplicity, aMSDP.psi0);

//NEED: ****** !!!!!!!! USE VECTORS AND DISTANCES PROVED BY THE MDVM INTERFACE !!!!!!!! ********
//NEED: ****** !!!!!!!! USE VECTORS AND DISTANCES PROVED BY THE MDVM INTERFACE !!!!!!!! ********
//NEED: ****** !!!!!!!! USE VECTORS AND DISTANCES PROVED BY THE MDVM INTERFACE !!!!!!!! ********
//NEED: ****** !!!!!!!! USE VECTORS AND DISTANCES PROVED BY THE MDVM INTERFACE !!!!!!!! ********

      CartesianVector dVec12 = aSitePosition1 - aSitePosition2,
        dVec32 = aSitePosition3 - aSitePosition2,
        dVec42 = aSitePosition4 - aSitePosition2;
      CartesianVector dVecT = dVec12.crossProduct(dVec32),
        dVecU = dVec32.crossProduct(dVec42);
      double dVecTMag = sqrt(sqr(dVecT)),
        dVecUMag = sqrt(sqr(dVecU));

      // collinear situation
      if (is_almost_zero(dVecTMag) || is_almost_zero(dVecUMag))
        return;

      CartesianVector dVec32Unit = dVec32/sqrt(sqr(dVec32)),
        dVecTUnit = dVecT/dVecTMag,
        dVecUUnit = dVecU/dVecUMag;
      double sinPsi = dVec32Unit*(dVecTUnit.crossProduct(dVecUUnit)),
        psi = asin(sinPsi);

      CartesianVector gradientPsiOverdVecT =
        -1.0/sqr(dVecT)*(dVecT.crossProduct(dVec32Unit)),
        gradientPsiOverdVecU =
        1.0/sqr(dVecU)*(dVecU.crossProduct(dVec32Unit));

      CartesianVector gradientPsiOverAP2 =
        gradientPsiOverdVecT.crossProduct(dVec32 - dVec12) +
        gradientPsiOverdVecU.crossProduct(dVec42 - dVec32),

        gradientPsiOverAP1 =
        -gradientPsiOverdVecT.crossProduct(dVec32),

        gradientPsiOverAP3 =
        gradientPsiOverdVecT.crossProduct(dVec12) -
        gradientPsiOverdVecU.crossProduct(dVec42),

        gradientPsiOverAP4 =
        gradientPsiOverdVecU.crossProduct(dVec32);


      double ImproperDihedralEnergy = aMSDP.Kpsi*sqr(psi - aMSDP.psi0);

      double DenergyOverDpsi = 2.0*aMSDP.Kpsi*(psi - aMSDP.psi0);

      XYZ ForceOnSite1 = -DenergyOverDpsi*gradientPsiOverAP1;
      XYZ ForceOnSite2 = -DenergyOverDpsi*gradientPsiOverAP2;
      XYZ ForceOnSite3 = -DenergyOverDpsi*gradientPsiOverAP3;
      XYZ ForceOnSite4 = -DenergyOverDpsi*gradientPsiOverAP4;

//****************************


      aMDVM.SetEnergy( ImproperDihedralEnergy );

      aMDVM.SetForce( MDVM::SiteA , ForceOnSite1 ) ;
      aMDVM.SetForce( MDVM::SiteB , ForceOnSite2 ) ;
      aMDVM.SetForce( MDVM::SiteC , ForceOnSite3 ) ;
      aMDVM.SetForce( MDVM::SiteD , ForceOnSite4 ) ;

      return;
      }
  };

