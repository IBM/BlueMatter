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
 #ifndef _XYZ_HPP_
#define _XYZ_HPP_

using namespace std ;
#include <math.h>

//#define DEAD_BEEF  0xdeadbeef
#define DEAD_BEEF  99999.0
#define INVALID_VAL HUGE_VAL //(( 1.0 ) * ( DEAD_BEEF + Platform::Topology::GetAddressSpaceId() ))

/////////////////////////////////
// typedef unsigned short FragId;
typedef unsigned short FragId;
typedef unsigned int   SiteId;
///////typedef int   VoxelIndex;
/////////////////////////////////

#undef max2
#undef min2
//#define max2(X,Y) ((X)>(Y)?(X):(Y))
//#define min2(X,Y) ((X)>(Y)?(Y):(X))

template<class type>
class tXYZ
  {
  public:
    static inline type max2(type X, type Y)
    {
      return (X>Y) ? X : Y ;
    }
    static inline type min2(type X, type Y)
    {
      return (X<Y) ? X : Y ;
    }
    // ***THIS CLASS MAY HAVE NO CONSTRUCTORS***
    // *** MAKE SURE YOUR CODE DOES IT'S OWN INITS****

    static inline const tXYZ<type> ZERO_VALUE() { tXYZ Rc = {0,0,0}; return( Rc ); }
    static inline const tXYZ<type> INVALID_VALUE() { tXYZ Rc = { INVALID_VAL, INVALID_VAL, INVALID_VAL }; return( Rc ); }

    type mX, mY, mZ;

// YUZH 10/13/2004: this seems to slow down code at the moment, so removing until things change
//     inline
//         tXYZ<type>& operator= (const tXYZ<type>& aOther)
//         {
//           //#pragma execution_frequency( very_high )
//             mX = aOther.mX;
//             mY = aOther.mY;
//             mZ = aOther.mZ;
//             return *this;
//         }
//     inline
//         tXYZ<type>& operator= (const tXYZ<type>& aOther)
//         {
//         	type amX = aOther.mX ;
//         	type amY = aOther.mY ; 
//         	type amZ = aOther.mZ ; 
//             mX = amX ;
//             mY = amY;
//             mZ = amZ;
//             return *this;
//         }

    inline
    void Set( const type aX, const type aY, const type aZ )
      {
        //#pragma execution_frequency( very_high )
      mX = aX;
      mY = aY;
      mZ = aZ;
      return;
      }

    inline
    int operator==( const tXYZ<type>& aOther ) const
      {
        //#pragma execution_frequency( very_high )
      int rc = mX == aOther.mX &&
               mY == aOther.mY &&
               mZ == aOther.mZ;
      return( rc );
      }

    inline
    int operator!=( const tXYZ<type>& aOther ) const
      {
//#pragma execution_frequency( very_high )
      int rc = mX != aOther.mX ||
               mY != aOther.mY ||
               mZ != aOther.mZ;
      return( rc );
      }

    inline
    tXYZ operator-=( const tXYZ<type>& aOther )
      {
//#pragma execution_frequency( very_high )
      mX -= aOther.mX;
      mY -= aOther.mY;
      mZ -= aOther.mZ;
      return( *this );
      }

    inline
    tXYZ operator-( const tXYZ<type>& aOther ) const
      {
//#pragma execution_frequency( very_high )
      tXYZ<type> rc;
      rc.mX = mX - aOther.mX;
      rc.mY = mY - aOther.mY;
      rc.mZ = mZ - aOther.mZ;
      return( rc );
      }

    inline
    tXYZ operator+=( const tXYZ<type>& aOther )
      {
//#pragma execution_frequency( very_high )
      mX += aOther.mX;
      mY += aOther.mY;
      mZ += aOther.mZ;
      return( *this );
      }

    inline
    tXYZ operator+( const tXYZ& aOther ) const
      {
//#pragma execution_frequency( very_high )
      tXYZ rc;
      rc.mX = mX + aOther.mX;
      rc.mY = mY + aOther.mY;
      rc.mZ = mZ + aOther.mZ;
      return( rc );
      }

    inline
    tXYZ operator-() const
    {
//#pragma execution_frequency( very_high )
    tXYZ rc;
    rc.mX = -mX;
    rc.mY = -mY;
    rc.mZ = -mZ;
    return rc;
    }

    inline
    type operator*( const tXYZ& other ) const
    {
//#pragma execution_frequency( very_high )
      return mX*other.mX + mY*other.mY + mZ*other.mZ;
    }

    inline
    type &operator[](const int a)
      {
      switch(a)
        {
        case 0: return mX; break;
        case 1: return mY; break;
        }
      return mZ;
      }

    inline
    type DotProduct( const tXYZ& aOther ) const
      {
      return (*this) * aOther;
      }
    // dot product

    inline
    tXYZ operator*( type aScalar ) const
      {
//#pragma execution_frequency( very_high )
      tXYZ rc;
      rc.mX = mX * aScalar;
      rc.mY = mY * aScalar;
      rc.mZ = mZ * aScalar;
      return( rc );
      }

    inline
    tXYZ operator*=( type aScalar )
      {
//#pragma execution_frequency( very_high )
      mX *= aScalar;
      mY *= aScalar;
      mZ *= aScalar;
      return( *this );
      }

    inline
    tXYZ operator/(const type& aScalar) const
    {
//#pragma execution_frequency( very_high )
      ///////////type reciprocal = type(1)/aScalar;
      double reciprocal = 1.0/aScalar;
      tXYZ rc;
      rc.mX = (*this).mX * reciprocal;
      rc.mY = (*this).mY * reciprocal;
      rc.mZ = (*this).mZ * reciprocal;
      return rc;
    }

    // inline
    tXYZ crossProduct(const tXYZ &other, int dumpLog=0) const
    {
//#pragma execution_frequency( very_high )
      tXYZ rc;
      rc.mX =    mY*other.mZ - mZ*other.mY;
      rc.mY =    mZ*other.mX - mX*other.mZ;
      rc.mZ =    mX*other.mY - mY*other.mX;

#if 0
      if( dumpLog )
       {
           BegLogLine( dumpLog )
               << " mX=" << mX
               << " mY=" << mY
               << " mZ=" << mZ
               << EndLogLine;

           BegLogLine( dumpLog )
               << " other.mX=" << other.mX
               << " other.mY=" << other.mY
               << " other.mZ=" << other.mZ
               << EndLogLine;

//            BegLogLine( dumpLog )
//                << " mY*other.mZ=" << mY*other.mZ
//                << " mZ*other.mY=" << mZ*other.mY
//                << " rc.mX=mY*other.mZ - mZ*other.mY="
//                << (mY*other.mZ - mZ*other.mY)
//                << EndLogLine;

//            BegLogLine( dumpLog )
//                << " mZ*other.mX=" << mZ*other.mX
//                << " mX*other.mZ=" << mX*other.mZ
//                << " rc.mY=mZ*other.mX - mX*other.mZ="
//                << (mZ*other.mX - mX*other.mZ)
//                << EndLogLine;

//            BegLogLine( dumpLog )
//                << " mX*other.mY=" << mX*other.mY
//                << " mY*other.mX=" << mY*other.mX
//                << " rc.mZ=mX*other.mY - mY*other.mX="
//                << (mX*other.mY - mY*other.mX)
//                << EndLogLine;

           BegLogLine( dumpLog )
               << " rc.mX=" << rc.mX
               << " rc.mY=" << rc.mY
               << " rc.mZ=" << rc.mZ
               << EndLogLine;

       }
#endif
      return(rc);
    }

    // Vector + scalar add. Add the scalar to all elements of the vector
    inline
    tXYZ scalarAdd( type aOther ) const
      {
//#pragma execution_frequency( very_high )
      tXYZ rc;
      rc.mX = mX + aOther;
      rc.mY = mY + aOther;
      rc.mZ = mZ + aOther;
      return( rc );
      }

    inline tXYZ fabs() const
    {
      tXYZ rc ;
      rc.mX = ::fabs(mX) ;
      rc.mY = ::fabs(mY) ;
      rc.mZ = ::fabs(mZ) ;
      return rc ;
    }
    
    inline
    type Length() const
      {
//#pragma execution_frequency( very_high )
      type lsq = mX * mX + mY * mY + mZ * mZ ;
      type l = sqrt( lsq );
      return( l );

      }
      
    inline 
    type Max() const
      {
         return max2(mX, max2(mY, mZ) ) ;
      }

    inline 
    type Min() const
      {
         return min2(mX, min2(mY, mZ) ) ;
      }

// This version works with 'reciprocal square root',
// which is only OK if the vector is not zero to start with
    inline
    type LengthKnownNotZero() const
      {
      type lsq = mX * mX + mY * mY + mZ * mZ ;
#if defined(PERFORMANCE_LENGTH_RECIP)
      type rl = 1.0/sqrt( lsq );
      type l = lsq * rl ;
#else
      type l = sqrt( lsq );
#endif
      return( l );

      }

    inline
    type ReciprocalLength() const
      {
//#pragma execution_frequency( very_high )
      type rl = 1.0/sqrt( mX * mX + mY * mY + mZ * mZ );
      return( rl );
      }

    inline
    type LengthSquared() const
    {
//#pragma execution_frequency( very_high )
      type ls =  mX * mX + mY * mY + mZ * mZ ;
      return( ls );
    }

    inline
    type Distance( const tXYZ& other ) const
      {
//#pragma execution_frequency( very_high )
      type rc = ( *this - other ).Length();
      return( rc );
      }

    inline
    type DistanceSquared( const tXYZ &other ) const
      {
//#pragma execution_frequency( very_high )
      type rc = ( *this - other ).LengthSquared();
      return( rc );
      }

    inline
    tXYZ
    ManhattanVector( const tXYZ& other ) const
      {
      tXYZ mvec = other - (*this);
      return( mvec );
      }

    inline
    tXYZ operator%( const tXYZ<type>& aOther )
      {
      mX %= aOther.mX;
      mY %= aOther.mY;
      mZ %= aOther.mZ;
      return( *this );
      }

    inline
    tXYZ
    GetNearestImageInPeriodicVolume(const tXYZ & aPeriod, const tXYZ &aPositionB ) const
      {
      // Internals are double precission for all types.
      tXYZ<double> Period    ;//= aPeriod;
      tXYZ<double> PositionB ;//= aPositionB;
      tXYZ<double> This      ;//= *this;

      Period.mX    = aPeriod.mX;
      Period.mY    = aPeriod.mY;
      Period.mZ    = aPeriod.mZ;

      PositionB.mX = aPositionB.mX;
      PositionB.mY = aPositionB.mY;
      PositionB.mZ = aPositionB.mZ;

      This.mX      = (*this).mX;
      This.mY      = (*this).mY;
      This.mZ      = (*this).mZ;

      tXYZ<double> TranslationVector;
      tXYZ<double> Posit;
      tXYZ<double> Nearest;
      TranslationVector.Zero();
      Posit = PositionB - This;
      int NoTransCount = 3;  // goes to zero if no translation is required.  A wee bit of preoptimization...

      if(  Posit.mX       <=    -Period.mX / 2.0 )
        TranslationVector.mX =   Period.mX;
      else if ( Posit.mX  >      Period.mX / 2.0 )
        TranslationVector.mX =  -Period.mX;
      else
        NoTransCount--;

      if(       Posit.mY     <= -Period.mY / 2.0 )
        TranslationVector.mY =   Period.mY;
      else if ( Posit.mY      >  Period.mY / 2.0 )
        TranslationVector.mY =  -Period.mY;
      else
        NoTransCount--;

      if(       Posit.mZ    <= -Period.mZ / 2.0 )
        TranslationVector.mZ =  Period.mZ;
      else if ( Posit.mZ     >  Period.mZ / 2.0 )
        TranslationVector.mZ = -Period.mZ;
      else
        NoTransCount--;

      if( NoTransCount )
        {
          Nearest = PositionB + TranslationVector;
        }
      else
        {
          Nearest = PositionB;
        }

      tXYZ rc;
      rc.mX = (type) Nearest.mX;
      rc.mY = (type) Nearest.mY;
      rc.mZ = (type) Nearest.mZ;

      return( rc );
      }

    inline
    tXYZ
    GetImageInPositiveOctant( const tXYZ& aPeriod ) const
      {
      tXYZ rc = *this;

      // if components are negative, bring them into positive space.
      // % (mod) seems not to work right for neagtive numbers.
      while( rc.mX < 0 )
        rc.mX += aPeriod.mX;

      while( rc.mY < 0 )
        rc.mY += aPeriod.mY;

      while( rc.mZ < 0 )
        rc.mZ += aPeriod.mZ;

      // if components are positive and greater than period, bring them into positive space unit vol.
      // could be % but don't expect this to happen often, so branch might be less expensive???
      while( rc.mX >= aPeriod.mX )
        rc.mX -= aPeriod.mX;

      while( rc.mY >= aPeriod.mY )
        rc.mY -= aPeriod.mY;

      while( rc.mZ >= aPeriod.mZ )
        rc.mZ -= aPeriod.mZ;

      return( rc );
      }



    inline
    tXYZ
    ManhattanVectorImaged( const tXYZ& aPeriod, const tXYZ& aOther ) const
      {
      tXYZ Nearest = GetNearestImageInPeriodicVolume( aPeriod, aOther );

      tXYZ Vector = Nearest - (*this);

      return( Vector );

      }
#if 0
    inline
    type ManhattanDistance( const tXYZ& other ) const
      {
      tXYZ mvec = ManhattanVector( other );
      type rc = ( mvec.mX >= 0 ? mvec.mX : -1 * mvec.mX )
              + ( mvec.mY >= 0 ? mvec.mY : -1 * mvec.mY )
              + ( mvec.mZ >= 0 ? mvec.mZ : -1 * mvec.mZ ) ;
      return( rc );
      }
#endif
    inline
    type ManhattanDistanceImaged( const tXYZ& aPeriod, const tXYZ& aOther ) const
      {
      tXYZ mvec = ManhattanVectorImaged( aPeriod, aOther );
      type rc = ( mvec.mX >= 0 ? mvec.mX : -1 * mvec.mX )
              + ( mvec.mY >= 0 ? mvec.mY : -1 * mvec.mY )
              + ( mvec.mZ >= 0 ? mvec.mZ : -1 * mvec.mZ ) ;
      return( rc );
      }



    inline
    int
    IntVolume()
      {
      // would be nice to just return the 'type' ... but that's hard on bytes
      int rc = mX * mY * mZ;
      assert( rc / mX == mY * mZ );
      return( rc );
      }

    inline
    void SetToScalar( type aScalar )
      {
      mX = aScalar; mY = aScalar; mZ = aScalar;
      }

    inline
    void Zero()
      {
      SetToScalar( 0 );
      }

    inline
    int IsInvalid()
    {
        unsigned rc =  (    ( mX >= ( 1.0 ) * INVALID_VAL )
                         || ( mY >= ( 1.0 ) * INVALID_VAL )
                         || ( mZ >= ( 1.0 ) * INVALID_VAL ) );
        return rc;
    }

#define REASONABLE_MD_XYZ_FOR_DEBUGGING 10000.0

    inline
    int IsReasonable(char* filename="None", int number=0 )
    {
      ///
      //   Widely abused , led to burn => disable
      //
      return 1;
        unsigned rc =  ( ( mX > REASONABLE_MD_XYZ_FOR_DEBUGGING )
                         || ( mY > REASONABLE_MD_XYZ_FOR_DEBUGGING )
                         || ( mZ > REASONABLE_MD_XYZ_FOR_DEBUGGING )
                         || ( mX < -REASONABLE_MD_XYZ_FOR_DEBUGGING )
                         || ( mY < -REASONABLE_MD_XYZ_FOR_DEBUGGING )
                         || ( mZ < -REASONABLE_MD_XYZ_FOR_DEBUGGING ) );

#if 0
        if( rc )
          {
          // Cause a seg fault to create a core dump
          BegLogLine( 1 )
            << "Called from file: " << filename
            << " number: " << number
            << "IsReasonable() "
            << *this
            << EndLogLine;

          int * Bomb = NULL;
          Bomb[ 0 ] = 0;
          }
#endif
        return !rc;
    }

    void Invalidate()
      {
      SetToScalar( INVALID_VAL );
      }

    inline double GetInvalidXYZLocal()
    {
    double RankFromInvalid = (mX - DEAD_BEEF);
    return RankFromInvalid;
    }

    inline
    int IsNonZero()
    {
      return ( ::fabs( mX ) != 0.0 ) || ( ::fabs( mY ) != 0.0 ) || ( ::fabs( mZ ) != 0.0 );
    }

    inline
    void IndexSwap(const int a, const int b)
    {
      type tmp   = (*this)[a];
      (*this)[a] = (*this)[b];
      (*this)[b] = tmp;
    }

    // return int 3-vector corresponding to index of largest to smallest dimension
    // first index will be bumped by 3 if chirality needs to be corrected by inverting first coord
    inline
    tXYZ<int> IndexOrder()
    {
      tXYZ<int> result;
      tXYZ local = *this;
      result.mX = 0;
      result.mY = 1;
      result.mZ = 2;

      int swapcount = 0;

      if (local.mX < local.mY)
      {
        local.IndexSwap(0,1);
        result.IndexSwap(0,1);
    swapcount++;
      }
      if (local.mX < local.mZ)
      {
        local.IndexSwap(0,2);
        result.IndexSwap(0,2);
    swapcount++;
      }
      if (local.mY < local.mZ)
      {
        local.IndexSwap(1,2);
        result.IndexSwap(1,2);
    swapcount++;
      }

      // encode chirality in the first index
      if (swapcount%2)
        result.mX += 3;

      return result;
    }

    // If o comes from a call to IndexOrder, then calling this will result in descending order of values
    // Thus you would call ReOrder on all vectors as they come in
    inline
    tXYZ &ReOrder(const tXYZ<int> &o)
    {
      tXYZ result;
      if (o.mX > 2)
        result.mX = -(*this)[o.mX-3];
      else
        result.mX =  (*this)[o.mX];
      result.mY = (*this)[o.mY];
      result.mZ = (*this)[o.mZ];
      (*this) = result;
      return (*this);
    }

    // The abs calls are for things that should stay positive, such as box dimensions
    inline
    tXYZ &ReOrderAbs(const tXYZ<int> &o)
    {
#if 1
      tXYZ result;
      if (o.mX > 2)
        result.mX = (*this)[o.mX-3];
      else
        result.mX =  (*this)[o.mX];
      result.mY = (*this)[o.mY];
      result.mZ = (*this)[o.mZ];
      (*this) = result;
#endif
      return (*this);
    }

    // ... similarly, call this on all vectors before shipping out
    inline
    tXYZ &ReOrderInverse(const tXYZ<int> &o)
    {
      tXYZ result;
      if (o.mX > 2)
        result[o.mX-3] = -mX;
      else
        result[o.mX] = mX;
      result[o.mY] = mY;
      result[o.mZ] = mZ;
      (*this) = result;
      return (*this);
    }

    inline
    tXYZ &ReOrderInverseAbs(const tXYZ<int> &o)
    {
      tXYZ result;
      if (o.mX > 2)
        result[o.mX-3] = mX;
      else
        result[o.mX] = mX;
      result[o.mY] = mY;
      result[o.mZ] = mZ;
      (*this) = result;
      return (*this);
    }

    // return int 3-vector corresponding to indices in same order as supplied vector
    // first index will be bumped by 3 if chirality needs to be corrected by inverting first coord
    // This is setup so that changes are made only if necessary
    inline
    tXYZ<int> MatchOrder(const tXYZ<int> &r)
    {
      tXYZ<int> result;
      tXYZ local = *this;
      result.mX = 0;
      result.mY = 1;
      result.mZ = 2;

      int swapcount = 0;
      int didswap;

      do {
        didswap = 0;
        if ( ((r.mX < r.mY) && (local.mX > local.mY)) || ((r.mX > r.mY) && (local.mX < local.mY)) )
        {
          local.IndexSwap(0,1);
          result.IndexSwap(0,1);
          swapcount++;
          didswap = 1;
        }
        if ( ((r.mY < r.mZ) && (local.mY > local.mZ)) || ((r.mY > r.mZ) && (local.mY < local.mZ)) )
        {
          local.IndexSwap(1,2);
          result.IndexSwap(1,2);
          swapcount++;
          didswap = 1;
        }
        if ( ((r.mX < r.mZ) && (local.mX > local.mZ)) || ((r.mX > r.mZ) && (local.mX < local.mZ)) )
        {
          local.IndexSwap(0,2);
          result.IndexSwap(0,2);
          swapcount++;
          didswap = 1;
        }
      } while (didswap);

      // encode chirality in the first index
      if (swapcount%2)
        result.mX += 3;

      return result;
    }

  };

template <class type>
inline tXYZ<type> minXYZ ( const tXYZ<type>& veca, const tXYZ<type>& vecb )
{
//#pragma execution_frequency( very_high )
    tXYZ<type> result ;
    result.mX = tXYZ<type>::min2(veca.mX,vecb.mX) ;
    result.mY = tXYZ<type>::min2(veca.mY,vecb.mY) ;
    result.mZ = tXYZ<type>::min2(veca.mZ,vecb.mZ) ;
    return result ;
}

template <class type>
inline tXYZ<type> maxXYZ ( const tXYZ<type>& veca, const tXYZ<type>& vecb )
{
//#pragma execution_frequency( very_high )
    tXYZ<type> result ;
    result.mX = tXYZ<type>::max2(veca.mX,vecb.mX) ;
    result.mY = tXYZ<type>::max2(veca.mY,vecb.mY) ;
    result.mZ = tXYZ<type>::max2(veca.mZ,vecb.mZ) ;
    return result ;
}

template<class type>
inline tXYZ<type> operator*( type aScalar, const tXYZ<type>& vec )
{
//#pragma execution_frequency( very_high )
return vec*aScalar;
}

template<class type>
inline type sqr( const tXYZ<type> &vec )
{
//#pragma execution_frequency( very_high )
return vec*vec;
}

template<class type>
inline tXYZ<type> PairwiseProduct( const tXYZ<type>& veca, const tXYZ<type>& vecb )
{
//#pragma execution_frequency( very_high )
    tXYZ<type> result ;
    result.mX = veca.mX * vecb.mX ;
    result.mY = veca.mY * vecb.mY ;
    result.mZ = veca.mZ * vecb.mZ ;
    return result ;
}

template<class type>
inline tXYZ<type> SetToScalar( type aScalar )
{
//#pragma execution_frequency( very_high )
    tXYZ<type> result ;
    result.mX = aScalar ;
    result.mY = aScalar ;
    result.mZ = aScalar ;
    return result ;
}

// Add up 'Count' elements of a vector of tXYZ's
template<class type>
inline tXYZ<type> VectorSum( const tXYZ<type> * vecA, unsigned int Count )
{
  tXYZ<type> result ; 
  if ( Count > 0 )
  {
    type sX = vecA[0].mX ;
    type sY = vecA[0].mY ;
    type sZ = vecA[0].mZ ;
    for ( unsigned int j=1;j<Count;j+=1)
    {
      sX += vecA[j].mX ;
      sY += vecA[j].mY ;
      sZ += vecA[j].mZ ;
    }
    result.mX = sX ;
    result.mY = sY ;
    result.mZ = sZ ;
  } else {
    result.Zero() ;
  }
  return result ;
}

template<class type, class streamclass>
static streamclass& operator<<(streamclass& os, const tXYZ<type> &xyz)
{

// #ifdef PK_BGL
// os << "[ "
//    << FormatString("% 16.14f") << xyz.mX << ' '
//    << FormatString("% 16.14f") << xyz.mY << ' '
//    << FormatString("% 16.14f") << xyz.mZ << " ]";
// #else
os << "[ "
   << xyz.mX << ' '
   << xyz.mY << ' '
   << xyz.mZ << " ]";
// #endif
  //BGF: Above seems better.  os << "[" << xyz.mX << " " << xyz.mY << " " << xyz.mZ << "]";
  return(os);

}


typedef tXYZ<double>           XYZ;
typedef tXYZ<double>          dXYZ;
typedef tXYZ<int>             iXYZ;
typedef tXYZ<float>           fXYZ;
typedef tXYZ<short>           sXYZ;
typedef tXYZ<unsigned short> usXYZ;
typedef tXYZ<unsigned char>  ucXYZ;
typedef tXYZ<signed char>     cXYZ;

// An 'XYZ' with an extra 'double', which might be used for a reciprocal-length
class XYZw {
  public: 
  XYZ mXYZ ;
  double mW ;
} ; 
// An 'XYZ' with an 'assign' method, becuase the compiler tends to do byte-at-a-time otherwise
class XYZAssign: public XYZ
{
  public: 
       inline
         XYZ& operator= ( const XYZ& aOther )
         {
          double amX = aOther.mX ;
          double amY = aOther.mY ; 
          double amZ = aOther.mZ ; 
             mX = amX ;
             mY = amY;
             mZ = amZ;
             return *this;
         }
  
} ;
#endif
