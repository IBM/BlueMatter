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
 #ifndef DATARECEIVERSITELOADER_HPP
#define DATARECEIVERSITELOADER_HPP

#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <BlueMatter/ArrayGrowable.hpp>
#include <BlueMatter/BoundingBox.hpp>
#include <BlueMatter/ExternalDatagram.hpp>

class DataReceiverSiteLoader : public DataReceiver
{
public:
  int NSites;
  // ED_DynamicVariablesSite Sites[100000];
  ArrayGrowable<ED_DynamicVariablesSite> Sites;
  int MaxSiteId;
  BoundingBox Box;

  virtual void init()
    {
      MaxSiteId = -1;
      NSites = 0;
    }

  virtual void informationRTP(ED_InformationRTP &rtp)
    {
      Box.mMinBox = rtp.mDynamicBoundingBoxMin;
      Box.mMaxBox = rtp.mDynamicBoundingBoxMax;
    }

  virtual void newSitePacket(ED_DynamicVariablesSite &sdv)
    {
      ++NSites;
      if (MaxSiteId <sdv.mSiteId) {
	MaxSiteId = sdv.mSiteId;
      }
      // printf("Loading %d %d\n", NSites, sdv.mSiteId);
      Sites.set(sdv.mSiteId, sdv);
      //      char buff[1024];
      //      ED_DynamicVariablesSite foo = Sites[sdv.mSiteId];
      //      sprintf(buff, "%6d %6d %16.8f %16.8f %16.8f     %16.8f %16.8f %16.8f",
      //      foo.mHeader.mShortTimeStep, foo.mSiteId,
      //      foo.mPosition.mX, foo.mPosition.mY, foo.mPosition.mZ,
      //      foo.mVelocity.mX, foo.mVelocity.mY, foo.mVelocity.mZ);
      //      cout << buff << endl;
    }

  virtual void final()
    {
      printf("done\n");
      assert(MaxSiteId == NSites -1);
    }
};

#endif
