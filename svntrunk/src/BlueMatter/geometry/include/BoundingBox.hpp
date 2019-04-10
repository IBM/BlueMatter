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
 #ifndef _BOUNDINGBOX_HPP_
#define _BOUNDINGBOX_HPP_

#include <iostream>
#include <BlueMatter/XYZ.hpp>

// This class manages two XYZs that form the
// extream corners of a bounding box.

// NOTE:

class BoundingBox
  {
  public:
    XYZ mMinBox, mMaxBox;

    inline double MinX() { return( mMinBox.mX < mMaxBox.mX ? mMinBox.mX : mMaxBox.mX ); }
    inline double MaxX() { return( mMinBox.mX > mMaxBox.mX ? mMinBox.mX : mMaxBox.mX ); }
    inline double MinY() { return( mMinBox.mY < mMaxBox.mY ? mMinBox.mY : mMaxBox.mY ); }
    inline double MaxY() { return( mMinBox.mY > mMaxBox.mY ? mMinBox.mY : mMaxBox.mY ); }
    inline double MinZ() { return( mMinBox.mZ < mMaxBox.mZ ? mMinBox.mZ : mMaxBox.mZ ); }
    inline double MaxZ() { return( mMinBox.mZ > mMaxBox.mZ ? mMinBox.mZ : mMaxBox.mZ ); }
    
    inline double DimX() { return( MaxX() - MinX() ); }
    inline double DimY() { return( MaxY() - MinY() ); }
    inline double DimZ() { return( MaxZ() - MinZ() ); }

    inline
    XYZ
    GetCenter() 
    {
        XYZ rc;
        rc.mX = 0.5 * (mMinBox.mX + mMaxBox.mX);
        rc.mY = 0.5 * (mMinBox.mY + mMaxBox.mY);
        rc.mZ = 0.5 * (mMinBox.mZ + mMaxBox.mZ);
        return ( rc );
    }

    inline
    XYZ
    GetDimensionVector()
      {
      XYZ rc;
      rc.mX = DimX();
      rc.mY = DimY();
      rc.mZ = DimZ();
      return( rc );
      }

    inline
    void
    Init( XYZ aFirst )
      {
      mMinBox = aFirst;
      mMaxBox = aFirst;
      }

    inline
    void
    Update( XYZ aSample )
      {
      if( mMaxBox.mX < aSample.mX )
          mMaxBox.mX = aSample.mX;
      if( mMaxBox.mY < aSample.mY )
          mMaxBox.mY = aSample.mY;
      if( mMaxBox.mZ < aSample.mZ )
          mMaxBox.mZ = aSample.mZ;
      if( mMinBox.mX > aSample.mX )
          mMinBox.mX = aSample.mX;
      if( mMinBox.mY > aSample.mY )
          mMinBox.mY = aSample.mY;
      if( mMinBox.mZ > aSample.mZ )
          mMinBox.mZ = aSample.mZ;
      }

    // Note this contains assumes > min and <= max
    inline
    int
    Contains( XYZ aPoint )
      {
      ////BegLogLine(1)
      ////  << "BoundingBox::Contains "
      ////  << aPoint.mX  << " "
      ////  << aPoint.mY  << " "
      ////  << aPoint.mZ
      ////  << EndLogLine;
      ////BegLogLine(1)
      ////  << "BoundingBox Min       "
      ////  << mMinBox.mX << " "
      ////  << mMinBox.mY << " "
      ////  << mMinBox.mZ
      ////  << EndLogLine;
      ////BegLogLine(1)
      ////  << "BoundingBox Max       "
      ////  << mMaxBox.mX << " "
      ////  << mMaxBox.mY << " "
      ////  << mMaxBox.mZ
      ////  << EndLogLine;




      int Rc = 0;
      if(    aPoint.mX >  mMinBox.mX
          && aPoint.mY >  mMinBox.mY
          && aPoint.mZ >  mMinBox.mZ
          && aPoint.mX <= mMaxBox.mX
          && aPoint.mY <= mMaxBox.mY
          && aPoint.mZ <= mMaxBox.mZ
        )
        Rc = 1; // Does contain this point.

      return(Rc);
      }

    // This uses min <= x < max
    inline
    int
    ContainsLeft( const XYZ &aPoint )
      {
      int Rc = 0;
      if(    aPoint.mX >=  mMinBox.mX
          && aPoint.mY >=  mMinBox.mY
          && aPoint.mZ >=  mMinBox.mZ
          && aPoint.mX < mMaxBox.mX
          && aPoint.mY < mMaxBox.mY
          && aPoint.mZ < mMaxBox.mZ
        )
        Rc = 1; // Does contain this point.

      return(Rc);
      }

      // AK: overload "=" operators

      inline
      BoundingBox& operator= (const BoundingBox& aOther) {
		mMinBox.mX = aOther.mMinBox.mX;
		mMinBox.mY = aOther.mMinBox.mY;
		mMinBox.mZ = aOther.mMinBox.mZ;
		mMaxBox.mX = aOther.mMaxBox.mX;
		mMaxBox.mY = aOther.mMaxBox.mY;
		mMaxBox.mZ = aOther.mMaxBox.mZ;

		return *this;
      }

  };

// AK: overload cout "<<" operator (outside class definition)
      static ostream &operator<< (ostream &os, const BoundingBox &xyz) {
	os << " " << xyz.mMinBox << " ,  " << xyz.mMaxBox << " " << endl; 
  	return os;
      }


#endif
