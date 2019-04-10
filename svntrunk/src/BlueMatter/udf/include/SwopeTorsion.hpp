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
 void SwopeTorsion(
	MDVM_IF aMDVM_IF,
	double k0,
	double delta,
	int multiplicity
	)
      {

      // For now, just create the symbols used by the regressed code
      // and connect them with what is in the MDVM.
      // IF these access don't compile out, change references in code below.
      XYZ aSitePosition1 = aMDVM_IF.GetPosition( aMDVM_IF.SiteA );
      XYZ aSitePosition2 = aMDVM_IF.GetPosition( aMDVM_IF.SiteB );
      XYZ aSitePosition3 = aMDVM_IF.GetPosition( aMDVM_IF.SiteC );
      XYZ aSitePosition4 = aMDVM_IF.GetPosition( aMDVM_IF.SiteD );


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

    //    if(multiplicity == 6 || (is_almost_zero(k0)))
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

        double cosDelta = cos(delta), sinDelta = sin(delta);
        double cosPhi = dVecTUnit*dVecUUnit,
          sinPhi = -dVec23Unit*(dVecUUnit.crossProduct(dVecTUnit));
        double DEnergyOverDPhi;
        // split the following part into different functions. get rid of the switch block...
        double aTorsionEnergy = 0.0;
        switch (multiplicity)
          {
          case 1:
            aTorsionEnergy = k0*(1 + cosDelta*cosPhi + sinDelta*sinPhi);
            DEnergyOverDPhi = k0*(-cosDelta*sinPhi + sinDelta*cosPhi);
            break;
          case 2:
            aTorsionEnergy = k0*(1 + cosDelta*(2.0*sqr(cosPhi)-1) + 2.0*sinDelta*sinPhi*cosPhi);
            DEnergyOverDPhi = 2.0*k0*(-2.0*cosDelta*cosPhi*sinPhi + sinDelta*(2.0*sqr(cosPhi)-1));
            break;
          case 3:
            aTorsionEnergy = k0*(1 + cosDelta*(4.0*cube(cosPhi) - 3.0*cosPhi) + sinDelta*(4.0*sqr(cosPhi)-1)*sinPhi);
            DEnergyOverDPhi = 3.0*k0*(-cosDelta*(4.0*sqr(cosPhi)-1)*sinPhi + 4.0*sinDelta*cosPhi*(cosPhi-1));
            break;
          case 4:
            aTorsionEnergy = k0*(1 + cosDelta*(8.0*sqr(sqr(cosPhi))-8.0*sqr(cosPhi)+1) + sinDelta*4.0*(2.0*cube(cosPhi)-cosPhi)*sinPhi);
            DEnergyOverDPhi = 4.0*k0*(-cosDelta*(8.0*cube(cosPhi)-4.0*cosPhi)*sinPhi - sinDelta*(8.0*sqr(cosPhi)-8.0*sqr(sqr(cosPhi))-1));
            break;
          case 5:
             aTorsionEnergy = k0*(1 + cosDelta*cosPhi*(5.0*sqr(sqr(sinPhi)) - 10.0*sqr(cosPhi*sinPhi) + sqr(sqr(cosPhi))) + Params.sinDelta*sinPhi*(5.0*sqr(sqr(cosPhi))- 10.0*sqr(cosPhi*sinPhi) + sqr(sqr(sinPhi))));
             DEnergyOverDPhi = 5.0*k0*( -cosDelta*sinPhi*(sqr(sqr(sinPhi)) - 10.0*sqr(sinPhi*cosPhi) + 5.0*sqr(sqr(cosPhi))) + sinDelta*cosPhi*(sqr(sqr(cosPhi))- 10.0*sqr(sinPhi*cosPhi) + 5.0*sqr(sqr(sinPhi))));
             break;

          case 6:
            aTorsionEnergy = k0*(1+cosDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) -1) + sinDelta*(32*sinPhi*sqr(cosPhi)*cube(cosPhi) - 32*cube(cosPhi)*sinPhi + 6*cosPhi*sinPhi));
            DEnergyOverDPhi = 6.0*k0*(cosDelta*(-32*sqr(cosPhi)*cube(cosPhi)*sinPhi + 32*cube(cosPhi)*sinPhi - 6*cosPhi*sinPhi) + sinDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) - 1));
            break;

          default:
            assert(0);
            break;
          }

      aMDVM_IF.SetEnergy( aTorsionEnergy );

      aMDVM_IF.SetForce( aMDVM_IF.SiteA ,  -DEnergyOverDPhi*gradientPhiOverAP1 ) ;
      aMDVM_IF.SetForce( aMDVM_IF.SiteB ,  -DEnergyOverDPhi*gradientPhiOverAP2 ) ;
      aMDVM_IF.SetForce( aMDVM_IF.SiteC ,  -DEnergyOverDPhi*gradientPhiOverAP3 ) ;
      aMDVM_IF.SetForce( aMDVM_IF.SiteD ,  -DEnergyOverDPhi*gradientPhiOverAP4 ) ;

      return;
      }
  };

