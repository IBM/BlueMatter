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
 * Module:          GridND
 *
 * Purpose:         A class that manages a logical 2D grid.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         032900 BGF Created.
 *
 ***************************************************************************/

#ifndef _GridND_HPP_
#define _GridND_HPP_

#include <pk/platform.hpp>
#include <math.h>

// What follows is a very limited 2d grid class.
// I expect to make an N dimentional grid class... that allows user names for dimentions.
// THIS CLASS IS NOT THREAD SAFE... yet.

// THINK ABOUT DIVIDING THE FUNCTION INTO TWO CLASSES, ONE FOR GRIDs AND THE OTHER
// FOR MAPPING.

// DON'T FORGET THE GROUP SET OP


// this grid is organized for the following 2d mapping to a linear array

//  0  1  2  3
//  4  5  6  7
//  8  9 10 11
// 12 13 14 15

template<int NOD>
class GridND
  {
  public:
    enum { NumberOfDimensions = NOD };

    int mExtentInDemension[ NumberOfDimensions ];

    class Coordinate
      {
      public:
        int mIndex[ NumberOfDimensions ];

        int operator==(const Coordinate& other) const
          {
          int Rc = 1;
          for( int i = 0; i < NumberOfDimensions; i++ )
            {
            if( mIndex[ i ] != other.mIndex[ i ] )
              {
              Rc = 0;
              break;
              }
            }
          return( Rc );
          }
      };

    class GroupAll
      {
      public:
        class Iterator
          {
          public:
            GridND::GroupCol* mGroupAll;
            int mY;
          public:

            Iterator() : mGroupAll(NULL), mNodeId(0)
              {}

            Iterator(GroupAll& aGroupAll) : mGroupAll(&aGroupAll), mNodeId(0)
              {}

            Iterator(const Iterator& proto) : mGroupAll(proto.mGroupAll), mNodeId(proto.mNodeId)
              {}

            GridND::GroupAll::Iterator&
            operator=(const GridND::GroupAll::Iterator& other)
              {
              if (this == &other)
                {
                return *this;
                }
              mGroupAll = other.mGroupAll;
              return *this;
              }

            GridND::GroupAll::Iterator&
            operator++() // prefix, I think
              {
              mNodeId++;
              return *this;
              }

            int operator*() const
              {
              int Rc = mNodeId;
              return( Rc );
              }

            int operator==(const GridND::GroupAll::Iterator& other) const
              {
              return(mNodeId == other.mNodeId);
              }
          };

      public:
        GridND   *mGrid;
        Iterator  mBegin;
        Iterator  mEnd;

        GroupAll( GridND *aGridND )
          {
          Grid             = aGridND;
          mBegin.mNodeId   = 0;
          mBegin.mGroupAll = this;
          mEnd.mNodeId     = Grid->Size();
          mEnd.mGroupAll   = this;
          }
        int size() const
          {
          return( Grid->Size() );
          }
        const Iterator& begin() const
          {
          return mBegin;
          }
        const Iterator& end() const
          {
          return mEnd;
          }
        int operator[]( int aIndex ) const
          {
          assert( aIndex >= 0 );
          int Rc = aIndex;
          return( Rc );
          }
        int IsAMember( int aId ) const
          {
          int Rc = ( aId < Grid->Size() );
          return( Rc );
          }
      };

    GroupAll
    GetAllGroupIF( int aXCoord )
      {
      GroupAll Rc( this );
      return( Rc );
      }

/////////////////////////////// Column ITER //////////////////////
    class GroupCol
      {
      // Columns are a group define as having a constant x value
      public:
        class Iterator
          {
          public:
            GridND::GroupCol* mGroupCol;
            int mY;
          public:
            inline Iterator();
            inline Iterator(GridND::GroupCol& aGroupCol);
            inline Iterator(const Iterator& proto);
            inline Iterator& operator=(const Iterator& other);
            inline Iterator& operator++(); // prefix, I think
            inline int operator*() const;
            inline int operator==(const Iterator& other) const;
          };

      public:
        GridND *Grid;
        int mY;
        int mX; // for a row, this stays the same
        Iterator mBegin;
        Iterator mEnd;

        GroupCol( GridND *aGridND, int aX )
          {
          Grid = aGridND;
          mBegin.mY=0;
          mBegin.mGroupCol=this;
          mEnd.mY = Grid->mYDimension;
          mEnd.mGroupCol=this;
          assert( ( aX >= 0 ) && ( aX < Grid->mXDimension ) ); // assert aX is in bounds
          mX = aX;
          mY = 0;  // we iterate along the col, or along Y
          }
        int size() const
          {
          return( Grid->mYDimension );
          }
        const Iterator& begin() const
          {
          return mBegin;
          }
        const Iterator& end() const
          {
          return mEnd;
          }
        int operator[]( int aYIndex ) const
          {
          assert( aYIndex >= 0 );
          int Rc = Grid->AddressSpaceAt( mX, aYIndex );
          return( Rc );
          }
        int IsAMember( int aId ) const
          {
          int Rc = ( (aId % Grid->mXDimension) == mX );
          return( Rc );
          }
      };

    GroupCol
    GetColGroupIF( int aXCoord )
      {
      GroupCol Rc( this, aXCoord );
      return( Rc );
      }

/////////////////////////////// COLUMN ITER //////////////////////
    class GroupRow
      {
      public:
        class Iterator
          {
          public:
            GridND::GroupRow* mGroupRow;
            int mX;
          public:
            inline Iterator();
            inline Iterator(GridND::GroupRow& aGroupRow);
            inline Iterator(const Iterator& proto);
            inline Iterator& operator=(const Iterator& other);
            inline Iterator& operator++(); // prefix, I think
            inline int operator*() const;
            inline int operator==(const Iterator& other) const;
          };

      public:
        GridND *Grid;
        int mX;
        int mY; // for a row, this stays the same
        Iterator mBegin;
        Iterator mEnd;

        GroupRow( GridND *aGridND, int aY )
          {
          Grid             = aGridND;
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

    GridND()
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

    int operator==(const GridND& other) const
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

GridND::GroupRow::Iterator::Iterator() : mGroupRow(NULL), mX(0)
  {}
GridND::GroupRow::Iterator::Iterator(GroupRow& aGroupRow) :
  mGroupRow(&aGroupRow), mX(0)
  {}
GridND::GroupRow::Iterator::Iterator(const Iterator& proto)
  : mGroupRow(proto.mGroupRow), mX(proto.mX)
  {}
GridND::GroupRow::Iterator&
GridND::GroupRow::Iterator::operator=(const GridND::GroupRow::Iterator& other)
  {
  if (this == &other)
    {
    return *this;
    }
  mGroupRow = other.mGroupRow;
  mX = other.mX;
  return *this;
  }
GridND::GroupRow::Iterator&
GridND::GroupRow::Iterator::operator++() // prefix, I think
  {
  ++mX;
  return *this;
  }
int GridND::GroupRow::Iterator::operator*() const
  {
  int Rc = mGroupRow->Grid->AddressSpaceAt( mX, mGroupRow->mY );
  return( Rc );
  }
int GridND::GroupRow::Iterator::operator==(const GridND::GroupRow::Iterator& other) const
  {
  return(mX == other.mX);
  }

//////////////////////////////////////////////////////////////////
GridND::GroupCol::Iterator::Iterator() : mGroupCol(NULL), mY(0)
  {}
GridND::GroupCol::Iterator::Iterator(GroupCol& aGroupCol) :
  mGroupCol(&aGroupCol), mY(0)
  {}
GridND::GroupCol::Iterator::Iterator(const Iterator& proto)
  : mGroupCol(proto.mGroupCol), mY(proto.mY)
  {}
GridND::GroupCol::Iterator&
GridND::GroupCol::Iterator::operator=(const GridND::GroupCol::Iterator& other)
  {
  if (this == &other)
    {
    return *this;
    }
  mGroupCol = other.mGroupCol;
  mY = other.mY;
  return *this;
  }
GridND::GroupCol::Iterator&
GridND::GroupCol::Iterator::operator++() // prefix, I think
  {
  mY += mGroupCol->Grid->mXDimension;
  return *this;
  }
int GridND::GroupCol::Iterator::operator*() const
  {
  int Rc = mGroupCol->Grid->AddressSpaceAt( mY, mGroupCol->mX );
  return( Rc );
  }
int GridND::GroupCol::Iterator::operator==(const GridND::GroupCol::Iterator& other) const
  {
  return(mY == other.mY);
  }

//////////////////////////////////////////////////////////////////

#endif
