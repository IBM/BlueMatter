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
 * Module:          Grid2D
 *
 * Purpose:         A class that manages a logical 2D grid.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         032900 BGF Created.
 *
 ***************************************************************************/

#ifndef _GRID2D_HPP_
#define _GRID2D_HPP_

#include <pk/platform.hpp>
#include <math.h>

// What follows is a very limited 2d grid class.
// I expect to make an N dimentional grid class... that allows user names for dimentions.
// THIS CLASS IS NOT THREAD SAFE... yet.


// this grid is organized for the following 2d mapping to a linear array

//  0  1  2  3
//  4  5  6  7
//  8  9 10 11
// 12 13 14 15

class Grid2D
  {
  public:
    int mXDimension;
    int mYDimension;

    class Coordinate
      {
      public:
        int mX;
        int mY;

        int operator==(const Coordinate& other) const
          {
          int Rc = ( mX == other.mX ) && ( mY == other.mY ) ;
          return( Rc );
          }
      };

    class GroupAll
      {
      // Allumns are a group define as having a constant x value
      public:
        class Iterator
          {
          public:
            Grid2D::GroupAll* mGroupAll;
            int mNodeId;
          public:
            inline Iterator();
            inline Iterator(Grid2D::GroupAll& aGroupAll);
            inline Iterator(const Iterator& proto);
            inline Iterator& operator=(const Iterator& other);
            inline Iterator& operator++(); // prefix, I think
            inline int operator*() const;
            inline int operator==(const Iterator& other) const;
          };

      public:
        Grid2D   *Grid;
        Iterator mBegin;
        Iterator mEnd;

        GroupAll( Grid2D *aGrid2D )
          {
          Grid             = aGrid2D;
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
            Grid2D::GroupCol* mGroupCol;
            int mY;
          public:
            inline Iterator();
            inline Iterator(Grid2D::GroupCol& aGroupCol);
            inline Iterator(const Iterator& proto);
            inline Iterator& operator=(const Iterator& other);
            inline Iterator& operator++(); // prefix, I think
            inline int operator*() const;
            inline int operator==(const Iterator& other) const;
          };

      public:
        Grid2D *Grid;
        int mY;
        int mX; // for a row, this stays the same
        Iterator mBegin;
        Iterator mEnd;

        GroupCol( Grid2D *aGrid2D, int aX )
          {
          Grid = aGrid2D;
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
            Grid2D::GroupRow* mGroupRow;
            int mX;
          public:
            inline Iterator();
            inline Iterator(Grid2D::GroupRow& aGroupRow);
            inline Iterator(const Iterator& proto);
            inline Iterator& operator=(const Iterator& other);
            inline Iterator& operator++(); // prefix, I think
            inline int operator*() const;
            inline int operator==(const Iterator& other) const;
          };

      public:
        Grid2D *Grid;
        int mX;
        int mY; // for a row, this stays the same
        Iterator mBegin;
        Iterator mEnd;

        GroupRow( Grid2D *aGrid2D, int aY )
          {
          Grid             = aGrid2D;
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

    Grid2D()
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

    int operator==(const Grid2D& other) const
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

Grid2D::GroupRow::Iterator::Iterator() : mGroupRow(NULL), mX(0)
  {}
Grid2D::GroupRow::Iterator::Iterator(GroupRow& aGroupRow) :
  mGroupRow(&aGroupRow), mX(0)
  {}
Grid2D::GroupRow::Iterator::Iterator(const Iterator& proto)
  : mGroupRow(proto.mGroupRow), mX(proto.mX)
  {}
Grid2D::GroupRow::Iterator&
Grid2D::GroupRow::Iterator::operator=(const Grid2D::GroupRow::Iterator& other)
  {
  if (this == &other)
    {
    return *this;
    }
  mGroupRow = other.mGroupRow;
  mX = other.mX;
  return *this;
  }
Grid2D::GroupRow::Iterator&
Grid2D::GroupRow::Iterator::operator++() // prefix, I think
  {
  ++mX;
  return *this;
  }
int Grid2D::GroupRow::Iterator::operator*() const
  {
  int Rc = mGroupRow->Grid->AddressSpaceAt( mX, mGroupRow->mY );
  return( Rc );
  }
int Grid2D::GroupRow::Iterator::operator==(const Grid2D::GroupRow::Iterator& other) const
  {
  return(mX == other.mX);
  }

//////////////////////////////////////////////////////////////////
Grid2D::GroupCol::Iterator::Iterator() : mGroupCol(NULL), mY(0)
  {}
Grid2D::GroupCol::Iterator::Iterator(GroupCol& aGroupCol) :
  mGroupCol(&aGroupCol), mY(0)
  {}
Grid2D::GroupCol::Iterator::Iterator(const Iterator& proto)
  : mGroupCol(proto.mGroupCol), mY(proto.mY)
  {}
Grid2D::GroupCol::Iterator&
Grid2D::GroupCol::Iterator::operator=(const Grid2D::GroupCol::Iterator& other)
  {
  if (this == &other)
    {
    return *this;
    }
  mGroupCol = other.mGroupCol;
  mY = other.mY;
  return *this;
  }
Grid2D::GroupCol::Iterator&
Grid2D::GroupCol::Iterator::operator++() // prefix, I think
  {
  mY += mGroupCol->Grid->mXDimension;
  return *this;
  }
int Grid2D::GroupCol::Iterator::operator*() const
  {
  int Rc = mGroupCol->Grid->AddressSpaceAt( mY, mGroupCol->mX );
  return( Rc );
  }
int Grid2D::GroupCol::Iterator::operator==(const Grid2D::GroupCol::Iterator& other) const
  {
  return(mY == other.mY);
  }

//////////////////////////////////////////////////////////////////
Grid2D::GroupAll::Iterator::Iterator() : mGroupAll(NULL), mNodeId(0)
  {}
Grid2D::GroupAll::Iterator::Iterator(GroupAll& aGroupAll) :
  mGroupAll(&aGroupAll), mNodeId(0)
  {}
Grid2D::GroupAll::Iterator::Iterator(const Iterator& proto)
  : mGroupAll(proto.mGroupAll), mNodeId(proto.mNodeId)
  {}
Grid2D::GroupAll::Iterator&
Grid2D::GroupAll::Iterator::operator=(const Grid2D::GroupAll::Iterator& other)
  {
  if (this == &other)
    {
    return *this;
    }
  mGroupAll = other.mGroupAll;
  return *this;
  }
Grid2D::GroupAll::Iterator&
Grid2D::GroupAll::Iterator::operator++() // prefix, I think
  {
  mNodeId++;
  return *this;
  }
int Grid2D::GroupAll::Iterator::operator*() const
  {
  int Rc = mNodeId;
  return( Rc );
  }
int Grid2D::GroupAll::Iterator::operator==(const Grid2D::GroupAll::Iterator& other) const
  {
  return(mNodeId == other.mNodeId);
  }


#endif
