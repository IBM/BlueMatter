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
 /***************************************************************************
 * Project:         pK
 *
 * Module:          Grid3D
 *
 * Purpose:         A class that manages a logical 3D grid.
 *                  NOTE: meant to also support 2D unfolded interface.  GACK!
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         032900 BGF Created.
 *
 ***************************************************************************/

#ifndef _GRID3D_HPP_
#define _GRID3D_HPP_

/////#include <pk/platform.hpp>
#include <math.h>

// What follows is a very limited 2d grid class.
// I expect to make an N dimentional grid class... that allows user names for dimentions.
// THIS CLASS IS NOT THREAD SAFE... yet.


// this grid is organized for the following 2d mapping to a linear array

//  0  1  2  3
//  4  5  6  7
//  8  9 10 11
// 12 13 14 15

class Grid3D
  {
  public:
    enum { X = 0, Y = 1, Z = 2 , DIMENSIONALITY = 3 };
  private:

    typedef int DIMVEC[ DIMENSIONALITY ];

    DIMVEC mExtentInDimension;

    class Coordinate
      {
      public:

        DIMVEC mOrdinal;

        int operator==(const Coordinate& other) const
          {
          for( int d = 0; d < DIMENSIONALITY; d++ )
            if( other.mOrdinal[d] != mOrdinal[d] )
              return( 0 );
          return( 1 );
          }

        static
        inline
        void
        Zero()
          {
          for(int d = 0; d < DIMENSIONALITY; d++ )
            {
            Ordinal[ d ] = 0;
            }
          }

        static
        inline
        int
        UnitVolume()
          {
          int rc = 1;
          for(int d = 0; d < DIMENSIONALITY; d++ )
            {
            rc *= Ordinal[ d ];
            }
          return( rc );
          }
      };


  public:

    Grid3D(int x, int y, int z)
      {
      mCS.mExtentInDimension[ X ] = x;
      mCS.mExtentInDimension[ Y ] = y;
      mCS.mExtentInDimension[ Z ] = z;
      }

    // Assign a ordinal to each element.
    inline
    int
    GetOrdinal( DIMVEC aDim )
      {
      int rc = aDim[ DIMENSIONALITY - 1 ];
      int PlaceValue = mExtentInDimension[ DIMENSIONALITY - 1 ];
      for( int d = DIMENSIONALITY - 2; d ; d-- )
        {
        rc += (aDim[ d ] * PlaceValue);
        PlaceValue += (PlaceValue * mExtentInDimension[ d ]) ;
        }
      return( rc );
      }

    inline
    Coordinate
    GetCoordinate( int aOrdinal )
      {
      DIMVEC ModValue;
      ModValue[ DIMENSIONALITY ] ;
      for( int d = Dimension; d < DIMENSIONALITY; d++ )
      for( int d = DIMENSIONALITY - 2; d ; d-- )
        {
        ModValue[ d ] *= mExtentInDimension[ d ] ;
        }
      }

    inline
    int
    Size()
      {
      return( UnitVolume() );
      }

    class GroupPlain
      {
      public:

        Grid3D *     mGrid3D;
        int          mFixedDimNo;
        int          mFixedDimOrdinal;

        GroupPlain( Grid3D *aGrid3D, int aFixedDimNo, int aFixedDimOrdinal )
          {
          mGrid3D          = aGrid3D;
          mFixedDimNo      = aFixedDimNo;
          mFixedDimOrdinal = aFixedDimOrdinal;
          }

        int
        Size() const
          {
          int rc = 1;
          for( int d = 0; d < DIMENSIONALITY; d++ )
            {
            if( d != mFixedDimNo )
              rc *= mGrid->mExtentInDimension[ d ];
            }
          return( rc );
          }

        int
        GetDimension( int aDimNo )
          {
          int rc ;
          if( aDimNo < aFixedDimNo )
            rc = mGrid3D.mExtentInDimension[ aDimNo ];
          else
            rc = mGrid3D.mExtentInDimension[ aFixedDimNo + aDimNo + 1 ];
          return( rc );
          }

        int operator[]( int aIndex ) const
          {
          int rc = 0;

          int PlaceValue =  1;
          for( int d = DIMENSIONALITY - 1; d >= 0 ; d-- )
            {
            PlaceValue *=  mGrid3D.mExtentInDimension[ DIMENSIONALITY - 1 ];
            int IndexPlaceValue = aIndex % PlaceValue;

            aIndex -= IndexPlaceValue;
            if( d != mFixedDimNo )
              {
              rc += IndexPlaceValue;
              }
            }
          assert( aIndex == 0 );
          return( rc );
          }

        int IsAMember( int aId ) const
          {
          int Rc = ( (aId % Grid->mXDimension) == mX );
          return( Rc );
          }
      };

    GroupPlain
    GetGroupPlain( int aFixedDim, int aFixedDimOrdinal )
      {
      GroupCol Rc( this, aFixedDim, aFixedDimOrdinal );
      return( Rc );
      }
  };


#if 0  //GRAVE YARD
/////////////////////////////// COLUMN ITER //////////////////////
    class GroupRow
      {
      public:
        class Iterator
          {
          public:
            Grid3D::GroupRow* mGroupRow;
            int mX;
          public:
            inline Iterator();
            inline Iterator(Grid3D::GroupRow& aGroupRow);
            inline Iterator(const Iterator& proto);
            inline Iterator& operator=(const Iterator& other);
            inline Iterator& operator++(); // prefix, I think
            inline int operator*() const;
            inline int operator==(const Iterator& other) const;
          };

      public:
        Grid3D *Grid;
        int mX;
        int mY; // for a row, this stays the same
        Iterator mBegin;
        Iterator mEnd;

        GroupRow( Grid3D *aGrid3D, int aY )
          {
          Grid             = aGrid3D;
          mBegin.mX        = 0;
          mBegin.mGroupRow = this;
          mEnd.mX          = Grid->mXDimension;
          mEnd.mGroupRow   = this;
          assert( ( aY >= 0 ) && ( aY < Grid->mYDimension ) ); // assert aY is in bounds
          mY = aY;
          mX = 0;  // we iterate along the row, or along X
          }
        int size() const
          {
          return( Grid->mXDimension );
          }
        const Iterator& begin() const
          {
          return mBegin;
          }
        const Iterator& end() const
          {
          return mEnd;
          }
        int operator[]( int aXIndex ) const
          {
          assert( aXIndex >= 0 );
          int Rc = Grid->AddressSpaceAt( aXIndex, mY );
          return( Rc );
          }
        int IsAMember( int aId ) const
          {
          int Rc = ( aId / Grid->mXDimension ) == mY;
          return( Rc );
          }
      };

    GroupRow
    GetRowGroupIF( int aYCoord )
      {
      GroupRow Rc( this, aYCoord );
      return( Rc );
      }

/////////////////////////////// COLUMN ITER //////////////////////

    Grid3D()
      {
      mXDimension = -1; // neg number to indicated that
      mYDimension = -1;
      }

    int
    IsInitialized()
      {
      if( mXDimension == -1 && mYDimension == -1 )
        return( 0 );
      return( 1 );
      }

    int operator==(const Grid3D& other) const
      {
      int Rc = ( mXDimension == other.mXDimension ) && ( mYDimension == other.mYDimension ) ;
      return( Rc );
      }

    int
    SetXDimension( int aXDimension )
      {
      assert( mXDimension == -1 ); // can't change size once set
      mXDimension = aXDimension;
      if( mYDimension != -1 )
        assert( mXDimension * mYDimension <= Platform::Topology::GetAddressSpaceCount() );
      return(0);
      }

    int
    SetYDimension( int aYDimension )
      {
      assert( mYDimension == -1 ); // can't change size once set
      mYDimension = aYDimension;
      if( mXDimension != -1 )
        assert( mXDimension * mYDimension <= Platform::Topology::GetAddressSpaceCount() );
      return(0);
      }

    int
    GetXDimension()
      {
      assert( mXDimension != -1 ); // can't change size once set
      return( mXDimension );
      }

    int
    GetYDimension()
      {
      assert( mYDimension != -1 ); // can't change size once set
      return( mYDimension );
      }

    int
    MaximumNearSquareWithColumnsLonger()
      {
      int Xmax = (int) sqrt( (float) Platform::Topology::GetAddressSpaceCount() ) ;  // cast seems to work... hmmmmm
      SetXDimension( Xmax );
      int Ymax = Platform::Topology::GetAddressSpaceCount() / Xmax;
      SetYDimension( Ymax );
      return(0);
      }

    int
    MaximumNearSquareWithRowsLonger() {  assert( 0 ); return(0); } // not yet supported

    int GetXCoordinate( int aAddressSpaceId )
      {
      int Rc =  aAddressSpaceId % mXDimension;
      return( Rc );
      }

    int GetYCoordinate( int aAddressSpaceId )
      {
      int Rc =  aAddressSpaceId / mXDimension;
      return( Rc );
      }


    int MyXCoordinate()
      {
      int Rc =  GetXCoordinate( Platform::Topology::GetAddressSpaceId() );
      return( Rc );
      }

    int MyYCoordinate()
      {
      int Rc =  GetYCoordinate( Platform::Topology::GetAddressSpaceId() );
      return( Rc );
      }

    Coordinate
    GetMyCoordinate()
      {
      Coordinate Rc;
      Rc.mX = MyXCoordinate();
      Rc.mY = MyYCoordinate();
      return( Rc );
      }

    Coordinate
    GetCoordinate( int aAddressSpaceId )
      {
      Coordinate Rc;
      Rc.mX = GetXCoordinate( aAddressSpaceId );
      Rc.mY = GetYCoordinate( aAddressSpaceId );
      return( Rc );
      }

    int
    AddressSpaceAt( int aX, int aY )
      {
      assert( aX >= 0 && aX < mXDimension );  // subscript out of bounds
      assert( aY >= 0 && aY < mYDimension );  // subscript out of bounds
      int Rc = aX + (aY * mXDimension);
      return( Rc );
      }

    int
    Size()
      {
      int Rc = mXDimension * mYDimension;
      return( Rc );
      }

    int
    IsMember( int aAddressSpaceId )
      {
      if( aAddressSpaceId < (mXDimension * mYDimension) )
        return(1);
      else
        return(0);
      }

  };

Grid3D::GroupRow::Iterator::Iterator() : mGroupRow(NULL), mX(0)
  {}
Grid3D::GroupRow::Iterator::Iterator(GroupRow& aGroupRow) :
  mGroupRow(&aGroupRow), mX(0)
  {}
Grid3D::GroupRow::Iterator::Iterator(const Iterator& proto)
  : mGroupRow(proto.mGroupRow), mX(proto.mX)
  {}
Grid3D::GroupRow::Iterator&
Grid3D::GroupRow::Iterator::operator=(const Grid3D::GroupRow::Iterator& other)
  {
  if (this == &other)
    {
    return *this;
    }
  mGroupRow = other.mGroupRow;
  mX = other.mX;
  return *this;
  }
Grid3D::GroupRow::Iterator&
Grid3D::GroupRow::Iterator::operator++() // prefix, I think
  {
  ++mX;
  return *this;
  }
int Grid3D::GroupRow::Iterator::operator*() const
  {
  int Rc = mGroupRow->Grid->AddressSpaceAt( mX, mGroupRow->mY );
  return( Rc );
  }
int Grid3D::GroupRow::Iterator::operator==(const Grid3D::GroupRow::Iterator& other) const
  {
  return(mX == other.mX);
  }

//////////////////////////////////////////////////////////////////
Grid3D::GroupCol::Iterator::Iterator() : mGroupCol(NULL), mY(0)
  {}
Grid3D::GroupCol::Iterator::Iterator(GroupCol& aGroupCol) :
  mGroupCol(&aGroupCol), mY(0)
  {}
Grid3D::GroupCol::Iterator::Iterator(const Iterator& proto)
  : mGroupCol(proto.mGroupCol), mY(proto.mY)
  {}
Grid3D::GroupCol::Iterator&
Grid3D::GroupCol::Iterator::operator=(const Grid3D::GroupCol::Iterator& other)
  {
  if (this == &other)
    {
    return *this;
    }
  mGroupCol = other.mGroupCol;
  mY = other.mY;
  return *this;
  }
Grid3D::GroupCol::Iterator&
Grid3D::GroupCol::Iterator::operator++() // prefix, I think
  {
  mY += mGroupCol->Grid->mXDimension;
  return *this;
  }
int Grid3D::GroupCol::Iterator::operator*() const
  {
  int Rc = mGroupCol->Grid->AddressSpaceAt( mY, mGroupCol->mX );
  return( Rc );
  }
int Grid3D::GroupCol::Iterator::operator==(const Grid3D::GroupCol::Iterator& other) const
  {
  return(mY == other.mY);
  }

//////////////////////////////////////////////////////////////////
Grid3D::GroupAll::Iterator::Iterator() : mGroupAll(NULL), mNodeId(0)
  {}
Grid3D::GroupAll::Iterator::Iterator(GroupAll& aGroupAll) :
  mGroupAll(&aGroupAll), mNodeId(0)
  {}
Grid3D::GroupAll::Iterator::Iterator(const Iterator& proto)
  : mGroupAll(proto.mGroupAll), mNodeId(proto.mNodeId)
  {}
Grid3D::GroupAll::Iterator&
Grid3D::GroupAll::Iterator::operator=(const Grid3D::GroupAll::Iterator& other)
  {
  if (this == &other)
    {
    return *this;
    }
  mGroupAll = other.mGroupAll;
  return *this;
  }
Grid3D::GroupAll::Iterator&
Grid3D::GroupAll::Iterator::operator++() // prefix, I think
  {
  mNodeId++;
  return *this;
  }
int Grid3D::GroupAll::Iterator::operator*() const
  {
  int Rc = mNodeId;
  return( Rc );
  }
int Grid3D::GroupAll::Iterator::operator==(const Grid3D::GroupAll::Iterator& other) const
  {
  return(mNodeId == other.mNodeId);
  }


#endif
#endif
