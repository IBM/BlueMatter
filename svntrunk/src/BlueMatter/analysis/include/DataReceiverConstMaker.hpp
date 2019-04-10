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
 #ifndef DATARECEIVERCONSTMAKER_HPP
#define DATARECEIVERCONSTMAKER_HPP

#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <iomanip>

class DataReceiverConstMaker : public DataReceiver
{
   enum {
      kMaxAtoms = 100000
      } ;
   XYZ mPositions[kMaxAtoms] ;
   XYZ mVelocities[kMaxAtoms] ;
   int mTimeStepThisAtom[kMaxAtoms] ;

   unsigned int mFreeIndex ;
   int mLastTimeStep ;
   int mFault ;
public:
   DataReceiverConstMaker(void)
   {
      mFault = 0 ;
      mFreeIndex = 0 ;
      mLastTimeStep = -1 ;
   } ;

  virtual void newSitePacket(ED_DynamicVariablesSite &sdv)
    {
       unsigned int SiteId = sdv.mSiteId ;
       if ( SiteId < kMaxAtoms)           // Bounds check, should always be
       {
          if (SiteId > mFreeIndex)        // Skipping some sites, may fill in later
          {
             for (unsigned int x=mFreeIndex; x<SiteId; x+=1)
             {
                mTimeStepThisAtom[x] = -1 ; // Note that they are skipped
             } /* endfor */
          } /* endif */
          if (SiteId >= mFreeIndex)
          {
             mFreeIndex = SiteId + 1 ;
          } /* endif */
          mLastTimeStep = sdv.mHeader.mShortTimeStep ;
          mTimeStepThisAtom[SiteId] = sdv.mHeader.mShortTimeStep ;
          mPositions[SiteId] = sdv.mPosition ;
          mVelocities[SiteId] = sdv.mVelocity ;
       }
       else
       {
          cerr << "Read record for site "
               << SiteId
               << " but only configured for "
               << kMaxAtoms
               << " sites\n" ;
          mFault = 1 ;
       } /* endif */
      DONEXT_1(newSitePacket, sdv);
    } ;

    void showXYZ(XYZ& a)
    {
       cout << " { "
            << setprecision(20) << setw(20) <<  a.mX
            << " , "
            << setprecision(20) << setw(20) << a.mY
            << " , "
            << setprecision(20) << setw(20) << a.mZ
            << " }" ;
    } ;

    void showXYZarray(XYZ* a, unsigned int q)
    {
       cout << "  " ;
       showXYZ(a[0]) ;
       cout << "  // Site " << 0 ;
       for (unsigned int x=1; x<q; x+=1)
       {
          cout << "\n ," ;
          showXYZ(a[x]) ;
       cout << "  // Site " << x ;
       } /* endfor */
    } ;

    ~DataReceiverConstMaker()
    {
       // Check that we have some records
       if (0 == mFreeIndex)
       {
          cerr << "No site records\n" ;
          mFault = 1 ;
       } /* endif */
       // Check that we have a record for every atom at the last time step
       for (int x=0; x<mFreeIndex; x+=1)
       {
          if (mTimeStepThisAtom[x] != mLastTimeStep)
          {
             cerr << "Site " << x
                  << " has a record for time step " << mTimeStepThisAtom[x]
                  << " but should have been for time step " << mLastTimeStep
                  << "\n" ;
             mFault = 1 ;
          } /* endif */
       } /* endfor */

       // If everything is OK, write out the structured constants for compiling into a kernel
       if (0 == mFault)
       {
          cout << "#include <BlueMatter/XYZ.hpp>\n"
               << "enum {\n"
                  "   kSites = " << mFreeIndex
               << "\n} ;\n"
               << "int aDVSSiteCount = kSites;\n"
               << "// Initial conditions taken from time step " << mLastTimeStep
               << "\nXYZ aStartingPositions[kSites] = {\n" ;
          showXYZarray(mPositions,mFreeIndex) ;
          cout << "\n} ;\n"
                  "XYZ aStartingVelocities[kSites] = {\n" ;
          showXYZarray(mVelocities,mFreeIndex) ;
          cout << "\n} ;\n" ;


       } /* endif */
    } ;

};

#endif
