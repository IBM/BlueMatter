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
 

#include <BlueMatter/UDF.hpp>
#include <BlueMatter/MDVM_UDF.hpp>

class CharmmTorsion
  {
  public:

    // THIS IS THE TYPE TO BE AGREED ON BETWEEN XML AND PROBSPEC.
    class Parameters
      {
      public:
        double k0;
        double delta;
        int multiplicity;
      };

    static
    inline
    void
    Execute(   const MolecularDynamicsVirtualMachineContext &aMDVM,
               const CharmmTorsion::Parameters              &aMSDP )
      {

      // For now, just create the symbols used by the regressed code
      // and connect them with what is in the MDVM.
      // IF these access don't compile out, change references in code below.
      XYZ aSitePosition1 = aMDVM.Position( SiteA );
      XYZ aSitePosition2 = aMDVM.Position( SiteB );
      XYZ aSitePosition3 = aMDVM.Position( SiteC );
      XYZ aSitePosition4 = aMDVM.Position( SiteD );

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

      aMDVM.AccessForce(SiteA) = -DEnergyOverDcosTheta*gradientCosDihedral_1;
      aMDVM.AccessForce(SiteB) = -DEnergyOverDcosTheta*gradientCosDihedral_2;
      aMDVM.AccessForce(SiteC) = -DEnergyOverDcosTheta*gradientCosDihedral_3;
      aMDVM.AccessForce(SiteD) = -DEnergyOverDcosTheta*gradientCosDihedral_4;

      return;
      }
  };

