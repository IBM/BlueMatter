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
 #ifndef __TORUSXYZ_HPP__
#define __TORUSXYZ_HPP__

#include <pk/XYZ.hpp>


typedef cXYZ TorusXYZ;

enum { X_PLUS = 0, X_MINUS = 1, Y_PLUS = 2, Y_MINUS = 3, Z_PLUS = 4, Z_MINUS = 5 };

// Return the link directioin from a Loc toward the Root.
static TorusXYZ
GetRootBoundTreeLink( TorusXYZ aPeriod, TorusXYZ aRoot, TorusXYZ aLoc )
  {
  TorusXYZ rc;
  rc.Set( 0, 0, 0 );

  if( aRoot != aLoc )
    {
    //////TorusXYZ mvec = aRoot - aLoc;
    // Need a minimum image manhattan vector ... from vector from aLoc to aRoot
    TorusXYZ mvec = aLoc.ManhattanVectorImaged( aPeriod, aRoot );

    // Figure out which is thee greates distance to go -- and send in that dir.
    // NOTE: there is bias in this that will effect link load balance!
    if( ( abs( mvec.mX ) > abs( mvec.mY ) ) && ( abs( mvec.mX ) > abs( mvec.mZ ) ) )
      {
      if( mvec.mX > 0 )
        rc.mX =  1;
      else
        rc.mX = -1;
      }
    else if( abs( mvec.mY ) > abs( mvec.mZ ) )
      {
      if( mvec.mY > 0 )
        rc.mY =  1;
      else
        rc.mY = -1;
      }
    else
      {
      if( mvec.mZ > 0 )
        rc.mZ =  1;
      else
        rc.mZ = -1;
      }
    }

  return( rc );
  }

#define ASSERT_VALID_LINK(txyz) (assert( (abs(txyz.mX)+abs(txyz.mY)+abs(txyz.mZ)) == 1 ) )

static int
GetLinkNumber( TorusXYZ aTXYZ )
  {
  ASSERT_VALID_LINK( aTXYZ );

  if( aTXYZ.mX ==  1 )
    return( X_PLUS  );
  if( aTXYZ.mX == -1 )
    return( X_MINUS );

  if( aTXYZ.mY ==  1 )
    return( Y_PLUS  );
  if( aTXYZ.mY == -1 )
    return( Y_MINUS );

  if( aTXYZ.mZ ==  1 )
    return( Z_PLUS  );
  if( aTXYZ.mZ == -1 )
    return( Z_MINUS );
  assert(!"Shouldn't get here");
  return(-1);
  }

static TorusXYZ
GetLinkFromNumber( int aLinkNumber )
  {
  assert( aLinkNumber >= 0 && aLinkNumber < 6 );  // 0..5 inclusive

  TorusXYZ rc;
  rc.Zero();

  switch( aLinkNumber )
    {
    case X_PLUS  :
      rc.mX =  1;
      break;
    case X_MINUS :
      rc.mX = -1;
      break;
    case Y_PLUS  :
      rc.mY =  1;
      break;
    case Y_MINUS :
      rc.mY = -1;
      break;
    case Z_PLUS  :
      rc.mZ =  1;
      break;
    case Z_MINUS :
      rc.mZ = -1;
      break;
    default:
      assert( 0 );
    };
  return( rc );
  }

static char *
GetLinkName( TorusXYZ aTXYZ )
  {

  if( aTXYZ == TorusXYZ::ZERO_VALUE() )
    return( "--" );

  ASSERT_VALID_LINK( aTXYZ );

  if( aTXYZ.mX ==  1 )
    return( "X+" );
  if( aTXYZ.mX == -1 )
    return( "X-" );

  if( aTXYZ.mY ==  1 )
    return( "Y+" );
  if( aTXYZ.mY == -1 )
    return( "Y-" );

  if( aTXYZ.mZ ==  1 )
    return( "Z+" );
  if( aTXYZ.mZ == -1 )
    return( "Z-" );
  assert(!"Shouldn't get here");
  return("Shouln't get here");
  }

#endif
