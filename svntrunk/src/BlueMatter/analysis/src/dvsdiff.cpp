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
 
#include <assert.h>
#include <fcntl.h>
#include <cstdio>
using namespace std ;

#include <BlueMatter/ExternalDatagram.hpp>

// #include <pk/fxlogger.hpp>

#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/SimulationParser.hpp>
#include <BlueMatter/DataReceiverSiteLoader.hpp>

int VecTol(XYZ Vec1, XYZ Vec2, double TolAbs, double &MaxAbsErr);
int Tol (double x1, double x2, double TolAbs, double &MaxAbsErr);

main(int argc, char **argv, char **envp)
{

  int TolPosFail  = 0;
  int TolVelFail  = 0;
  int FileDiff = 0;
  double MaxAbsPosErr = -99999.0;
  double MaxAbsVelErr = -99999.0;
  const double TolAbs = 1.0e-12;

  if (argc < 3) {
    printf ("dvsdiff dvsfile1 dvsfile2\n");
    return 1;
  }

  SimulationParser sp1 = SimulationParser(argv[1], 5, 0);

  DataReceiverSiteLoader DRSiteLoader1;

  sp1.addReceiver(&DRSiteLoader1);

  sp1.init();

  sp1.run();

  sp1.final();

  SimulationParser sp2 = SimulationParser(argv[2], 1, 0);

  DataReceiverSiteLoader DRSiteLoader2;

  sp2.addReceiver(&DRSiteLoader2);

  sp2.init();

  sp2.run();

  sp2.final();


  assert(DRSiteLoader1.NSites == DRSiteLoader2.NSites);

  for (int i = 0; i < DRSiteLoader1.NSites; i++) {
    if (!(DRSiteLoader1.Sites[i].mPosition == DRSiteLoader2.Sites[i].mPosition) ||
        !(DRSiteLoader1.Sites[i].mVelocity == DRSiteLoader2.Sites[i].mVelocity)) {

      FileDiff = 1;
      //      cout << "The files differ at site " << i << endl;
      TolPosFail |= VecTol(DRSiteLoader1.Sites[i].mPosition, DRSiteLoader2.Sites[i].mPosition, TolAbs, MaxAbsPosErr);
      TolVelFail |= VecTol(DRSiteLoader1.Sites[i].mVelocity, DRSiteLoader2.Sites[i].mVelocity, TolAbs, MaxAbsVelErr);
    }
  }

  if (FileDiff == 0) {
    cout << "The files contains identical dynamic variables" << endl;
  } else if ((FileDiff == 1) && ((TolPosFail == 1) || ( TolVelFail == 1))) {
    cout << "The files contains different dynamic variables " << endl;
  } else if ((FileDiff == 1) && ((TolPosFail == 0) && ( TolVelFail == 0))) {
    cout << "The files contains dynamic variables that are within acceptable tolerance of " << TolAbs << endl;
    cout << "Maximum absolute deviation for all sites in position is " << MaxAbsPosErr << endl;
    cout << "Maximum absolute deviation for all sites in velocity is " << MaxAbsVelErr << endl;
  }
}

int VecTol(XYZ Vec1, XYZ Vec2, double TolAbs, double &MaxAbsErr)
{
  int TolFail;
  TolFail  = Tol(Vec1.mX, Vec2.mX, TolAbs, MaxAbsErr);
  TolFail |= Tol(Vec1.mY, Vec2.mY, TolAbs, MaxAbsErr);
  TolFail |= Tol(Vec1.mZ, Vec2.mZ, TolAbs, MaxAbsErr);
  return TolFail;
}

int Tol(double x1, double x2, double TolAbs, double &MaxAbsErr)
{

  double dx;

  dx  = fabs(x1-x2);
  if (dx  > MaxAbsErr) MaxAbsErr = dx;
  if (MaxAbsErr > TolAbs) {
    return 1;
  } else {
    return 0;
  }
}

