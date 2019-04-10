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
 #ifndef __LOCAL_TUPLE_LIST_DRIVER_HPP__
#define __LOCAL_TUPLE_LIST_DRIVER_HPP__

#ifndef PKFXLOG_REGRESS_UDF_RESULTS
#define PKFXLOG_REGRESS_UDF_RESULTS ( 0 )
#endif

#ifndef PKFXLOG_LTD_DRIVE_ONE_INIT
#define PKFXLOG_LTD_DRIVE_ONE_INIT   ( 0 )
#endif

#ifndef PKFXLOG_LOCALTUPLELISTDRIVER
#define PKFXLOG_LOCALTUPLELISTDRIVER ( 0 )
#endif

#ifndef PKFXLOG_LOCALTUPLELISTDRIVER_1
#define PKFXLOG_LOCALTUPLELISTDRIVER_1 ( 0 )
#endif

#ifdef PKFXLOG_BONDED_LISTS_DEBUG
#define PKFXLOG_BONDED_LISTS_DEBUG ( 0 )
#endif

#ifndef PKFXLOG_BONDED_SEQUENCE
#define PKFXLOG_BONDED_SEQUENCE ( 0 ) 
#endif

#ifndef PKFXLOG_BOND_VECTOR
#define PKFXLOG_BOND_VECTOR ( 0 )
#endif

#ifndef PKFXLOG_BONDED_REBUILD
#define PKFXLOG_BONDED_REBUILD ( 0 ) 
#endif

#ifndef USE_QSORT_LOCAL_TUPLE_LIST
#define USE_QSORT_LOCAL_TUPLE_LIST (0)
#endif

#include <math.h>
#include <rptree.hpp>

char ltldStrBuff[ 256 ] MEMORY_ALIGN(5);

#define INITIAL_SIZE                      ( 1024 )

// Caution, this structure contains a 2-byte 'hole'
struct FragmentTupleRec
{
    FragId         mFragId;
    unsigned int   mTupleIndex;
};


struct UdfCodeEnergy
  {
  int     mUdfCode;
  double  mEnergy;
  } ;

UdfCodeEnergy gUdfEnergy[ 32 ];
int gUdfEnergyCount;

static int CompareFrag(const void* elem1, const void* elem2)
  {
  FragId fA = ((FragmentTupleRec *) elem1)->mFragId;
  FragId fB = ((FragmentTupleRec *) elem2)->mFragId;

  if( fA < fB )
    return -1;
  else if( fA == fB )
    return 0;
  else return 1;
  }

template<class T>
inline int ExpandList( T** aList, int aSize )
{
  assert( *aList != NULL );

  int NewSize = 2 * aSize;
  T* aNewList = (T *) malloc( sizeof( T ) * NewSize);
  assert( aNewList != NULL );

  memcpy( (char *) aNewList, (char *) *aList, sizeof( T ) * aSize );

  free( *aList );

  *aList = aNewList;

  return NewSize;
}

class BondList
{
public: 	
  class BondAtom
  {
    public: 
    unsigned int mAbsSiteId ;
  } ;
  class BondVector {
    public: 
      BondAtom mAtom0 ; 
      BondAtom mAtom1 ;
      unsigned int MatchForward(BondVector aBondVector)
      {
        return ( 0 == ( (aBondVector.mAtom0.mAbsSiteId ^ mAtom0.mAbsSiteId) | (aBondVector.mAtom1.mAbsSiteId ^ mAtom1.mAbsSiteId) ) ) ;
      }
      unsigned int MatchReverse(BondVector aBondVector)
      {
        return ( 0 == ( (aBondVector.mAtom0.mAbsSiteId ^ mAtom1.mAbsSiteId) | (aBondVector.mAtom1.mAbsSiteId ^ mAtom0.mAbsSiteId) ) ) ;
      }
      unsigned int Matches(BondVector aBondVector)
      {
        return MatchForward(aBondVector) | MatchReverse(aBondVector) ;
      }
      template <class DynVarMgr>
      void Evaluate(DynVarMgr & DynVarMgrIF, XYZw & aVector)
      {
       #ifdef PK_PHASE4
          XYZ Pos0 = DynVarMgrIF.GetInteractionPositionForSite( mAtom0.mAbsSiteId );
          XYZ Pos1 = DynVarMgrIF.GetInteractionPositionForSite( mAtom1.mAbsSiteId );
        #else
          XYZ Pos0 = DynVarMgrIF.GetPosition( mAtom0.mAbsSiteId );
          XYZ Pos1 = DynVarMgrIF.GetPosition( mAtom1.mAbsSiteId );
        #endif
        NearestVectorInPeriodicVolume(Pos0,Pos1,aVector.mXYZ) ;
//        aVector.mW = 1.0/sqrt(aVector.mXYZ.LengthSquared()); 
        BegLogLine(PKFXLOG_BOND_VECTOR)
          << "BondVector::Evalaute " <<  aVector.mXYZ 
//          << " " << aVector.mW
          << EndLogLine ;
      }
  } ;
  // Table of bond vectors that we know about. Element 0 is a 'dummy'; the associated hash table
  // points to element 0 corresponding to an unallocated hash entry
  class BondVectorTable {
    public:
      unsigned int mSize ; 
      unsigned int mAllocatedSize ; 
      BondVector* mTable ;
      void Init(void) 
      {
        BegLogLine(PKFXLOG_BOND_VECTOR)
          << "BondVectorTable::Init"
          << EndLogLine ;
        mSize = 1 ; 
        mAllocatedSize = 0 ; 
        mTable = NULL ;
      }
      void Term(void)
      {
        BegLogLine(PKFXLOG_BOND_VECTOR)
          << "BondVectorTable::Term"
          << EndLogLine ;
        mSize = 0 ;
        mAllocatedSize = 0 ; 
        free(mTable) ; 
        mTable = NULL ;
      }
      void Clear(void) 
      {
        BegLogLine(PKFXLOG_BOND_VECTOR)
          << "BondVectorTable::Clear"
          << EndLogLine ;
        mSize = 1 ; 
      }
      void SetAllocation(unsigned int aAllocation)
      {
        BegLogLine(PKFXLOG_BOND_VECTOR)
          << "BondVectorTable::SetAllocation " << aAllocation
          << EndLogLine ;
        assert(mAllocatedSize == 0) ; 
        mTable = (BondVector *)malloc(aAllocation*sizeof(BondVector)) ; 
        assert(mTable != 0) ; 
        mAllocatedSize = aAllocation ; 
        mTable[0].mAtom0.mAbsSiteId = 0xffffffff ; // Make sure element 0 will come up as a hash miss
        mTable[0].mAtom1.mAbsSiteId = 0xffffffff ;
        mSize = 1 ; 
      }
      unsigned int Append(BondVector& aBondVector)
      {
        BegLogLine(PKFXLOG_BOND_VECTOR)
          << "BondVectorTable::Append " << mSize
          << " (" << aBondVector.mAtom0.mAbsSiteId 
          << "," << aBondVector.mAtom1.mAbsSiteId
          << ") "
          << EndLogLine ;
         if ( 0 == mAllocatedSize )
         {
          SetAllocation(128) ; 
         } else if ( mSize == mAllocatedSize )
         {
          mAllocatedSize = ExpandList(&mTable,mAllocatedSize) ;
         }
         mTable[mSize] = aBondVector ; 
         unsigned int result=mSize ;
         mSize += 1 ;
         return result ;
      }
      void FillRoots(XYZw* aVectors) 
      {
         double root_table[mSize] ; 
         for ( unsigned int s1=1;s1<mSize;s1+=1)
         {
          #pragma execution_frequency(very_high)
          root_table[s1] = 1.0/sqrt(aVectors[s1].mXYZ.LengthSquared()) ;
         }
         for ( unsigned int s2=1;s2<mSize;s2+=1)
         {
          aVectors[s2].mW = root_table[s2] ;
         }
      }
      template <class DynVarMgr>
      void Evaluate(DynVarMgr & DynVarMgrIF, XYZw* aVectors) 
      {
        BegLogLine(PKFXLOG_BOND_VECTOR)
          << "BondVectorTable::Evaluate"
          << EndLogLine ;
        // TODO: Check that the compiler builds good code for this loop
         for ( unsigned int s=1;s<mSize;s+=1 )
         {  
          #pragma execution_frequency(very_high)
          mTable[s].Evaluate(DynVarMgrIF, aVectors[s] ) ;
         }
         FillRoots(aVectors) ;
      }
  } ; 
  
  BondVectorTable mBondTable ; 
  
  class BondVectorHashTable
  {
    public: 
      class HashGroup {
        public: 
        unsigned int mTag[8] ;
        void Clear(void)
        {
          for ( unsigned int a=0;a<8;a+=1)
          {
            mTag[a]=0 ;
          }
        }
        unsigned int FindFreeSlot(void)
        {
          unsigned int result = 
                 ( 0 == mTag[0] ) ? 0
               : ( 0 == mTag[1] ) ? 1 
               : ( 0 == mTag[2] ) ? 2
               : ( 0 == mTag[3] ) ? 3
               : ( 0 == mTag[4] ) ? 4
               : ( 0 == mTag[5] ) ? 5
               : ( 0 == mTag[6] ) ? 6
               : ( 0 == mTag[7] ) ? 7
               : 8 ;
               
        BegLogLine(PKFXLOG_BOND_VECTOR) 
          << "HashGroup::FindFreeSlot returns " << result
          << EndLogLine ;
           return result ;
        }
        
      } ;
      unsigned int mTablePower ;
      HashGroup * mHashGroup ;
      void Init(void) 
      {
        mTablePower = 0xffffffff ;
        mHashGroup = NULL ;
      }
      void Term(void)
      {
        mTablePower = 0xffffffff ;
        free(mHashGroup) ;
        mHashGroup = NULL ;
      }
      void Clear(void) 
      {
        for ( unsigned int x=0;x<(1<<mTablePower);x+=1)
        {
          mHashGroup[x].Clear() ;
        }
      }
      void Allocate(unsigned int aTablePower)
      {
        free(mHashGroup) ; 
        mTablePower=aTablePower ;
        unsigned int TableSize = 1<<mTablePower ;
        mHashGroup=(HashGroup *)malloc(TableSize*sizeof(HashGroup)) ;
        Clear() ; 
      }
      unsigned int HashFunction(BondVector aBondVector)
      {
         unsigned int t0 = aBondVector.mAtom0.mAbsSiteId ;
         unsigned int t1 = aBondVector.mAtom1.mAbsSiteId ;
         unsigned int t2 = ( t0 < t1 ) ? t0 : t1 ; // The lower atom of the pair
         unsigned int t3 = ( t0 < t1 ) ? t1 : t0 ; // The higher atom of the pair
         unsigned int t4 = t3 - t2 ; // The difference in atom numbers
         
         // t2 and t4 are independent.
         // t2 is 'random but clustered' on any given node.
         // t4 is 'mostly fairly near zero'
         
         unsigned int t5 = t4 + ( t2 << 20 ) ;
//  	  	 // Try 'modulo the largest prime lower than the power of 2' ...
//  	  	 switch ( mTablePower )
//  	  	 {
//  	  	 	case 1: return t5 % 2 ;
//  	  	 	case 2: return t5 % 3 ; 
//  	  	 	case 3: return t5 % 7 ; 
//  	  	 	case 4: return t5 % 13 ; 
//  	  	 	case 5: return t5 % 31 ; 
//  	  	 	case 6: return t5 % 61 ; 
//  	  	 	case 7: return t5 % 127 ; 
//  	  	 	case 8 : return t5 % 251 ; 
//  	  	 	case 9 : return t5 % 509 ; 
//  	  	 	case 10: return t5 % 1021 ; 
//  	  	 	case 11: return t5 % 2039 ; 
//  	  	 	case 12: return t5 % 4093 ;
//  	  	 	default: return t5 % ( ( 1 << mTablePower ) - 1 ) ; 
//  	  	 } ;
         // Try 'modulo one less than the power of 2' ...
         // This is set up as divide-by-constant to give the compiler an opportunity of 
         // transforming it to multiplications
         switch ( mTablePower )
         {
          case 1: return t5 % 2 ;
          case 2: return t5 % 3 ; 
          case 3: return t5 % 7 ; 
          case 4: return t5 % 15 ; 
          case 5: return t5 % 31 ; 
          case 6: return t5 % 63 ; 
          case 7: return t5 % 127 ; 
          case 8 : return t5 % 255 ; 
          case 9 : return t5 % 511 ; 
          case 10: return t5 % 1023 ; 
          case 11: return t5 % 2047 ; 
          case 12: return t5 % 4095 ;
          default: return t5 % ( ( 1 << mTablePower ) - 1 ) ; 
         } ;
         
//  	  	 unsigned int t5 = t4 * 0x11111111 ;
//  	  	 unsigned int t6 = t5 ^ t2 ; 
//  	  	 
//  	  	 return t6 % (1<<mTablePower) ;

//         unsigned int t5 = ( t2 << 3 ) ^ t4 ; 
//         return t5 % (1<<mTablePower) ;
      }
      
      // Try to rebuild the hash table at a given size
      unsigned int Rebuild(BondVectorTable& aBondVectorTable, unsigned int aTablePower)
      {
        BegLogLine(PKFXLOG_BONDED_REBUILD)
          << "BondVectorHashTable::Rebuild aTablePower=" << aTablePower
          << EndLogLine ;
        Allocate(aTablePower) ;
        for ( unsigned int x=1;x<aBondVectorTable.mSize;x+=1)
        {
          unsigned int Hash=HashFunction(aBondVectorTable.mTable[x]) ;
          unsigned int FreeSlot=mHashGroup[Hash].FindFreeSlot() ;
          if ( FreeSlot >= 8 ) 
          {
            BegLogLine(PKFXLOG_BONDED_REBUILD)
              << "Rebuild overflowed on bucket " << Hash
              << " at table entry " << x
              << " of " << aBondVectorTable.mSize
              << EndLogLine ;
            return 0 ; // Hash table not rebuildable at this size
          } 
          mHashGroup[Hash].mTag[FreeSlot] = x ;	
        }
        return 1 ; 
      }
      // Increase the size of the hash table until it will rebuild
      void Rebuild(BondVectorTable& aBondVectorTable)
      {
         for ( unsigned int CandidatePower=mTablePower+1 ;
               0 == Rebuild(aBondVectorTable,CandidatePower) ;
               CandidatePower += 1
             ) 
         { 
          // Nothing to do in the body; all a side effect of the termination condition
         }     
      }
  } ;
  
  BondVectorHashTable mBondHashTable ;
  
  unsigned int FindBondVector(BondVector aBondVector,unsigned int Hash)
  {
    unsigned int InTable0=mBondTable.mTable[mBondHashTable.mHashGroup[Hash].mTag[0]].Matches(aBondVector) * mBondHashTable.mHashGroup[Hash].mTag[0];
    unsigned int InTable1=mBondTable.mTable[mBondHashTable.mHashGroup[Hash].mTag[1]].Matches(aBondVector) * mBondHashTable.mHashGroup[Hash].mTag[1];
    unsigned int InTable2=mBondTable.mTable[mBondHashTable.mHashGroup[Hash].mTag[2]].Matches(aBondVector) * mBondHashTable.mHashGroup[Hash].mTag[2];
    unsigned int InTable3=mBondTable.mTable[mBondHashTable.mHashGroup[Hash].mTag[3]].Matches(aBondVector) * mBondHashTable.mHashGroup[Hash].mTag[3];
    unsigned int InTable4=mBondTable.mTable[mBondHashTable.mHashGroup[Hash].mTag[4]].Matches(aBondVector) * mBondHashTable.mHashGroup[Hash].mTag[4];
    unsigned int InTable5=mBondTable.mTable[mBondHashTable.mHashGroup[Hash].mTag[5]].Matches(aBondVector) * mBondHashTable.mHashGroup[Hash].mTag[5];
    unsigned int InTable6=mBondTable.mTable[mBondHashTable.mHashGroup[Hash].mTag[6]].Matches(aBondVector) * mBondHashTable.mHashGroup[Hash].mTag[6];
    unsigned int InTable7=mBondTable.mTable[mBondHashTable.mHashGroup[Hash].mTag[7]].Matches(aBondVector) * mBondHashTable.mHashGroup[Hash].mTag[7];
    return InTable0|InTable1|InTable2|InTable3|InTable4|InTable5|InTable6|InTable7 ;
   }
   unsigned int FindBondVector(BondVector aBondVector)
   {
    unsigned int Hash=mBondHashTable.HashFunction(aBondVector) ;
    return FindBondVector(aBondVector,Hash) ;
   }
   
   
   unsigned int AddBondVector(BondVector aBondVector)
   {
     unsigned int Hash=mBondHashTable.HashFunction(aBondVector) ;
     unsigned int Result=FindBondVector(aBondVector, Hash) ;	
     BegLogLine(PKFXLOG_BOND_VECTOR)
       << "SiteTupleDriver::AddBondVector (" << aBondVector.mAtom0.mAbsSiteId
       << "," << aBondVector.mAtom1.mAbsSiteId
       << ") InitialHash=" << Hash
       << " InitialSlot=" << Result
       << EndLogLine ;
     if ( 0 == Result ) 
     {
      // Vector not previously seen. Need to add it to the table.
      Result=mBondTable.Append(aBondVector) ; 
      // and add it to the hash table
      unsigned int FreeSlot=mBondHashTable.mHashGroup[Hash].FindFreeSlot() ; 
      if ( FreeSlot < 8 )
      {
        // There was a free slot. Fill it in.
        mBondHashTable.mHashGroup[Hash].mTag[FreeSlot] = Result ;
        } else {
          // The hash table overflowed. Rebuild it.
          mBondHashTable.Rebuild(mBondTable) ; 
      }
        
      
     } 
     BegLogLine(PKFXLOG_BOND_VECTOR)
       << "SiteTupleDriver::AddBondVector (" << aBondVector.mAtom0.mAbsSiteId
       << "," << aBondVector.mAtom1.mAbsSiteId
       << ") ActualSlot=" << Result
       << EndLogLine ;
     
     return Result ;
   }
   
   unsigned int AddBondVector(unsigned int aSite0, unsigned int aSite1)
   {
      BondVector b ;
      b.mAtom0.mAbsSiteId=aSite0 ; 
      b.mAtom1.mAbsSiteId=aSite1 ;
      return AddBondVector(b) ; 
   }
   
} ;
  template<class MDVM, class UDFX_Bound >
  class DriveOne
    {
    public:

    typedef typename UDFX_Bound::TupleType D_TupleType;
    typedef typename UDFX_Bound::ParamType D_ParamType;

    D_TupleType*  mSites            MEMORY_ALIGN(5);
    D_ParamType*  mParams           MEMORY_ALIGN(5);
    // Support for feeding bond vectors to the bonded UDFs (rather than atom positions)
    typedef typename UDFX_Bound::BondIndexType D_BondIndexType;
    typedef typename UDFX_Bound::BondSenseType D_BondSenseType;
    D_BondIndexType* mBonds         MEMORY_ALIGN(5) ;
    D_BondSenseType* mBondSenses    MEMORY_ALIGN(5) ;

    int*           mIsFirstSiteLocal MEMORY_ALIGN(5);

    int           mLocalListAllocatedSize;
    int           mLocalListSize;

    int*          mFragmentIdToWorkorderMap[ NUMBER_OF_FRAGMENTS ];

    int*          mFragmentWorkorderHeap;
    int           mFragmentWorkorderHeapSize;

#ifdef PK_PHASE4
    template<class DynVarMgr>
    void
    SetupWorkorderMap( DynVarMgr & DynVarMgrIF )
      {
        BegLogLine(PKFXLOG_BONDED_SEQUENCE)
          << "LocalTupleListDriver::SetupWorkorderMap "
          << UDFX_Bound::UDF::UdfName
          << " TupleCount=" << UDFX_Bound::GetTupleCount()
          << EndLogLine ;
      int CalculateSize = 1;
      mFragmentWorkorderHeapSize = 0;
      int NextWorkorderHeapIndex = 0;

      if( UDFX_Bound::GetTupleCount() == 0 )
        return;

#if defined(BONDED_SETUP_IN_FREESTORE)
      FragmentTupleRec* FragmnentsOfFirstSiteInTuple = (FragmentTupleRec*) malloc( sizeof(FragmentTupleRec) * UDFX_Bound::GetTupleCount() );
      if( FragmnentsOfFirstSiteInTuple == NULL )
        PLATFORM_ABORT("FragmnentsOfFirstSiteInTuple == NULL");
#else        
      FragmentTupleRec FragmnentsOfFirstSiteInTuple[UDFX_Bound::GetTupleCount()] ;
#endif

      BegLogLine(PKFXLOG_SETUP)
        << "SetupWorkorderMap FragmnentsOfFirstSiteInTuple=" << FragmnentsOfFirstSiteInTuple
        << EndLogLine ; 
        
      for(int TupleIndex= 0; TupleIndex < UDFX_Bound::GetTupleCount(); TupleIndex++ )
        {
        UDFX_Bound::TupleType & SiteTuple = UDFX_Bound::GetTuple( TupleIndex );

        FragId fId = DynVarMgrIF.GetFragIdForSiteId( SiteTuple[ 1 ] );

        FragmnentsOfFirstSiteInTuple[ TupleIndex ].mFragId = fId;
        FragmnentsOfFirstSiteInTuple[ TupleIndex ].mTupleIndex = TupleIndex;
        }

#if USE_QSORT_LOCAL_TUPLE_LIST
      Platform::Algorithm::Qsort( FragmnentsOfFirstSiteInTuple, UDFX_Bound::GetTupleCount(), sizeof( FragmentTupleRec), CompareFrag );
#else      
//      Platform::Algorithm::Rsort( FragmnentsOfFirstSiteInTuple, UDFX_Bound::GetTupleCount(), sizeof( FragmentTupleRec), 0, sizeof(FragId) );
      RadixSort<FragmentTupleRec,FragId,1,0>( FragmnentsOfFirstSiteInTuple, UDFX_Bound::GetTupleCount() );
#endif
      mFragmentWorkorderHeapSize = NUMBER_OF_FRAGMENTS + UDFX_Bound::GetTupleCount();

      mFragmentWorkorderHeap = (int *) malloc( sizeof( int ) * mFragmentWorkorderHeapSize );
      if( mFragmentWorkorderHeap == NULL )
        {
        PLATFORM_ABORT("mFragmentWorkorderHeap == NULL");
        }

      int PrevFragId = -1;
      int CurFragId = -1;

      int FragTupleCountIndex = 0;
      int FirstTime = 1;

      for(int TupleIndex= 0; TupleIndex < UDFX_Bound::GetTupleCount(); TupleIndex++ )
        {
        CurFragId = FragmnentsOfFirstSiteInTuple[ TupleIndex ].mFragId;

        //BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 )
        BegLogLine( 0 )
          << UDFX_Bound::UDF::UdfName
          << " TupleIndex= " << TupleIndex
          << " CurFragId= " << CurFragId
          << " PrevFragId= " << PrevFragId
          << EndLogLine;

        for( int j=PrevFragId+1; j<CurFragId; j++ )
          {
          assert( &mFragmentWorkorderHeap[ NextWorkorderHeapIndex ] != NULL );
          assert( NextWorkorderHeapIndex >= 0 && NextWorkorderHeapIndex < mFragmentWorkorderHeapSize );

          mFragmentIdToWorkorderMap[ j ] = &mFragmentWorkorderHeap[ NextWorkorderHeapIndex ];

          mFragmentIdToWorkorderMap[ j ][ 0 ] = 0;

          // BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 && UDFX_Bound::UDF::Code == 7 )
          BegLogLine( 0 )
              << UDFX_Bound::UDF::UdfName
              << " j= " << j
              << " NextWorkorderHeapIndex= " << NextWorkorderHeapIndex
              << " mFragmentIdToWorkorderMap[ " << j << " ][ 0 ] " << mFragmentIdToWorkorderMap[ j ][ 0 ]
              << EndLogLine;

          NextWorkorderHeapIndex++;
          }

        if( FirstTime || CurFragId != PrevFragId )
          {
          // This is the first of a run
          // Allocate from heap and start filling it
          FragTupleCountIndex = 0;
          assert( &mFragmentWorkorderHeap[ NextWorkorderHeapIndex ] != NULL );
          assert( NextWorkorderHeapIndex >= 0 && NextWorkorderHeapIndex < mFragmentWorkorderHeapSize );

          mFragmentIdToWorkorderMap[ CurFragId ] = &mFragmentWorkorderHeap[ NextWorkorderHeapIndex ];
          mFragmentIdToWorkorderMap[ CurFragId ][ 0 ] = 0;
          NextWorkorderHeapIndex++;
          FirstTime = 0;
          }

        mFragmentIdToWorkorderMap[ CurFragId ][ FragTupleCountIndex + 1 ] = FragmnentsOfFirstSiteInTuple[ TupleIndex ].mTupleIndex;
        mFragmentIdToWorkorderMap[ CurFragId ][ 0 ]++;
        FragTupleCountIndex++;
        NextWorkorderHeapIndex++;

        // BegLogLine( Platform::Topology::GetAddressSpaceId() == 0 && UDFX_Bound::UDF::Code == 7 )
        BegLogLine( 0 )
          << UDFX_Bound::UDF::UdfName
          << " CurFragId= " << CurFragId
          << " PrevFragId= " << PrevFragId
          << " mFragmentIdToWorkorderMap[ " << CurFragId << " ][ " << FragTupleCountIndex << " + 1 ]"
          << " mFragmentIdToWorkorderMap[ " << CurFragId << " ][ 0 ] " << mFragmentIdToWorkorderMap[ CurFragId ][ 0 ]
          << EndLogLine;

        PrevFragId = CurFragId;
        }


      // Initialize the remaining fragments with count=0
      CurFragId++;
      while( CurFragId < NUMBER_OF_FRAGMENTS )
        {
        mFragmentIdToWorkorderMap[ CurFragId ] = &mFragmentWorkorderHeap[ NextWorkorderHeapIndex ];
        mFragmentIdToWorkorderMap[ CurFragId ][ 0 ] = 0;
        NextWorkorderHeapIndex++;
        CurFragId++;
        }


      for( int i=0; i<NUMBER_OF_FRAGMENTS; i++ )
        {
        if( mFragmentIdToWorkorderMap[ i ] == NULL )
          {
          BegLogLine( PKFXLOG_SETUP )
              << UDFX_Bound::UDF::UdfName
              << " FragID: " << i << " is NULL in mFragmentIdToWorkorderMap"
              << EndLogLine;
          assert(0);
          }
        }
        
      BegLogLine(PKFXLOG_SETUP)
        << "SetupWorkorderMap FragmnentsOfFirstSiteInTuple=" << FragmnentsOfFirstSiteInTuple
        << EndLogLine ; 

#if defined(BONDED_SETUP_IN_FREESTORE)
      if( FragmnentsOfFirstSiteInTuple != NULL )
        free( FragmnentsOfFirstSiteInTuple );
#endif

      BegLogLine( 0 )
        << "Leaving SetupWorkorderMap"
        << EndLogLine;
      }
#endif

    template<class DynVarMgr>
    void
    SetupLocalSitesParams( DynVarMgr & DynVarMgrIF, BondList& aBondList, int first=0 )
      {
        BegLogLine(PKFXLOG_BONDED_SEQUENCE)
          << "LocalTupleListDriver::SetupLocalSitesParams "
          << UDFX_Bound::UDF::UdfName
          << " TupleCount=" << UDFX_Bound::GetTupleCount()
          << EndLogLine ;
      // On the first pass figure out the sizes of lists
      // multiply the sizes of lists by 2 to account for the changing
      // of local fragments after load balancing
          BegLogLine( 0 )
            << "Entering SetupLocalSitesParams"
            << EndLogLine;

      mLocalListSize = 0;
#ifdef PK_PHASE4
      if( UDFX_Bound::GetTupleCount() == 0 )
        return;

     for( int fragIndex=0; fragIndex < DynVarMgrIF.mIntegratorStateManagerIF.GetNumberOfLocalFragments(); fragIndex++)
       {
       FragId fragId = DynVarMgrIF.mIntegratorStateManagerIF.GetNthFragmentId( fragIndex );

       assert( mFragmentIdToWorkorderMap[ fragId ] != NULL );
       int WorkorderSize = mFragmentIdToWorkorderMap[ fragId ][ 0 ];

       BegLogLine( 0 )
         << "fragIndex=" << fragIndex
         << " fragId= " << fragId
         << " WorkorderSize= " << WorkorderSize
         << EndLogLine;

       for( int wOrderIndex = 0; wOrderIndex < WorkorderSize; wOrderIndex++ )
         {
         int TupleIndex = mFragmentIdToWorkorderMap[ fragId ][ wOrderIndex + 1 ];

         UDFX_Bound::TupleType & SiteTuple = UDFX_Bound::GetTuple( TupleIndex );
#else
      for(int TupleIndex= 0; TupleIndex < UDFX_Bound::GetTupleCount(); TupleIndex++ )
        {
        UDFX_Bound::TupleType & SiteTuple = UDFX_Bound::GetTuple( TupleIndex );
        // This is done to compute all the the rigid sites on each task
        if( !DynVarMgrIF.IsSiteLocal( SiteTuple[ 0 ] ) )
          continue;
#endif
        BegLogLine( PKFXLOG_LOCALTUPLELISTDRIVER )
          << "TupleDriver working on " << " Code " <<  UDFX_Bound::UDF::Code
          << EndLogLine;

        if( mLocalListSize >= mLocalListAllocatedSize )
          {
          int NewSize = ExpandList( &mSites, mLocalListAllocatedSize );
          ExpandList( &mParams, mLocalListAllocatedSize );
#if !defined(OMIT_BOND_VECTOR_TABLE)
          ExpandList( &mBonds, mLocalListAllocatedSize ) ; 
          ExpandList( &mBondSenses, mLocalListAllocatedSize ) ; 
#endif          
          mLocalListAllocatedSize = NewSize;
          }

        for( int s = 0; s < UDFX_Bound::UDF::NumberOfSites; s++ )
          mSites[ mLocalListSize ][ s ] = SiteTuple[ s ];

#if !defined(OMIT_BOND_VECTOR_TABLE) 
        if ( UDFX_Bound::UDF::BondVectorOptimisationEnabled) 
        {
          switch ( UDFX_Bound::UDF::NumberOfSites ) 
          {
            case 2: 
             {
              unsigned int BondIndex= aBondList.AddBondVector(SiteTuple[0],SiteTuple[1]) ;
              mBonds[mLocalListSize ][ 0 ] = BondIndex ;
              mBondSenses[mLocalListSize ][ 0 ] =
                ( aBondList.mBondTable.mTable[BondIndex].mAtom0.mAbsSiteId == SiteTuple[0] ) ;
             } 
             break ; 
            case 3: 
             {
              unsigned int BondIndex0= aBondList.AddBondVector(SiteTuple[1],SiteTuple[0]) ;
              unsigned int BondIndex1= aBondList.AddBondVector(SiteTuple[1],SiteTuple[2]) ;
              mBonds[mLocalListSize ][ 0 ] = BondIndex0 ;
              mBonds[mLocalListSize ][ 1 ] = BondIndex1 ;
              mBondSenses[mLocalListSize ][ 0 ] =
                ( aBondList.mBondTable.mTable[BondIndex0].mAtom0.mAbsSiteId == SiteTuple[1] ) ;
              mBondSenses[mLocalListSize ][ 1 ] =
                ( aBondList.mBondTable.mTable[BondIndex1].mAtom0.mAbsSiteId == SiteTuple[1] ) ;
              
             }
             break ;
            case 4: 
             {
              if ( UDFX_Bound::UDF::IsExtendedKind )
              {
                // 'Torsion' UDFs take strung-out presentation
                // (ask the scientists why they always choose a reversed sign for 'Bond 2'.)
                unsigned int BondIndex0= aBondList.AddBondVector(SiteTuple[0],SiteTuple[1]) ;
                unsigned int BondIndex1= aBondList.AddBondVector(SiteTuple[1],SiteTuple[2]) ;
                unsigned int BondIndex2= aBondList.AddBondVector(SiteTuple[3],SiteTuple[2]) ;
                mBonds[mLocalListSize ][ 0 ] = BondIndex0 ;
                mBonds[mLocalListSize ][ 1 ] = BondIndex1 ;
                mBonds[mLocalListSize ][ 2 ] = BondIndex2 ;
                mBondSenses[mLocalListSize ][ 0 ] =
                  ( aBondList.mBondTable.mTable[BondIndex0].mAtom0.mAbsSiteId == SiteTuple[0] ) ;
                mBondSenses[mLocalListSize ][ 1 ] =
                  ( aBondList.mBondTable.mTable[BondIndex1].mAtom0.mAbsSiteId == SiteTuple[1] ) ;
                mBondSenses[mLocalListSize ][ 2 ] =
                  ( aBondList.mBondTable.mTable[BondIndex2].mAtom0.mAbsSiteId == SiteTuple[3] ) ;
                
                
              } else if ( UDFX_Bound::UDF::IsNoseFirstKind ) 
              {
                // 'Dihedral' UDFs take airplane presentation
                unsigned int BondIndex0= aBondList.AddBondVector(SiteTuple[0],SiteTuple[1]) ;
                unsigned int BondIndex1= aBondList.AddBondVector(SiteTuple[2],SiteTuple[1]) ;
                unsigned int BondIndex2= aBondList.AddBondVector(SiteTuple[3],SiteTuple[1]) ;
                mBonds[mLocalListSize ][ 0 ] = BondIndex0 ;
                mBonds[mLocalListSize ][ 1 ] = BondIndex1 ;
                mBonds[mLocalListSize ][ 2 ] = BondIndex2 ;
                mBondSenses[mLocalListSize ][ 0 ] =
                  ( aBondList.mBondTable.mTable[BondIndex0].mAtom0.mAbsSiteId == SiteTuple[0] ) ;
                mBondSenses[mLocalListSize ][ 1 ] =
                  ( aBondList.mBondTable.mTable[BondIndex1].mAtom0.mAbsSiteId == SiteTuple[2] ) ;
                mBondSenses[mLocalListSize ][ 2 ] =
                  ( aBondList.mBondTable.mTable[BondIndex2].mAtom0.mAbsSiteId == SiteTuple[3] ) ;
                
              } else { 
                assert(0) ; 
              } 
             }
             break ;  
            default: assert(0) ; 
          }
          
//	        for ( int s = 0 ; s < UDFX_Bound::UDF::NumberOfSites-1; s++ )
//	        {
//	        	unsigned int BondIndex= aBondList.AddBondVector(SiteTuple[0],SiteTuple[s+1]) ;
//	        	mBonds[mLocalListSize ][ s ] = BondIndex ;
//	        	mBondSenses[mLocalListSize ][ s ] = 
//	        	   ( aBondList.mBondTable.mTable[BondIndex].mAtom0.mAbsSiteId == SiteTuple[0] ) ;
//	        }
        } 
#endif        
        

        UDFX_Bound::ParamType param = UDFX_Bound::GetParam( TupleIndex );
        mParams[ mLocalListSize ] = param;

        mLocalListSize++;
        }

#ifdef PK_PHASE4
         }
#endif
       BegLogLine( 0 )
         << "Leaving SetupLocalSitesParams"
         << EndLogLine;
       }

    template<class DynVarMgr>
    void
    Init(DynVarMgr & DynVarMgrIF, BondList& aBondList)
      {
      BegLogLine( PKFXLOG_LTD_DRIVE_ONE_INIT || PKFXLOG_BONDED_SEQUENCE )
        << "LocalTupleListDriver::Init(): "
        << " STATS0 "
        << UDFX_Bound::UDF::UdfName
        << " Code "
        << UDFX_Bound::UDF::Code
        << " Respa Level "
        << MSD::RespaLevelByUDFCode[ UDFX_Bound::UDF::Code ]
        << " TupleCount "
        <<  UDFX_Bound::GetTupleCount()
        << " NumberOfSites "
        << UDFX_Bound::UDF::NumberOfSites
        << EndLogLine;

      UDFX_Bound::Init();

      mSites = (D_TupleType *) malloc( sizeof( D_TupleType ) * INITIAL_SIZE );
      assert( mSites != NULL );

      mParams = (D_ParamType *) malloc( sizeof( D_ParamType ) * INITIAL_SIZE );
      assert( mParams != NULL );
#if !defined(OMIT_BOND_VECTOR_TABLE)
      mBonds = (D_BondIndexType *) malloc( sizeof(D_BondIndexType) * INITIAL_SIZE );
      assert( mBonds != NULL) ;
      mBondSenses = (D_BondSenseType *) malloc( sizeof(D_BondSenseType) * INITIAL_SIZE );
      assert( mBondSenses != NULL) ;
#endif      

      mLocalListAllocatedSize = INITIAL_SIZE;
      mLocalListSize = 0;

#ifdef PK_PHASE4
      // Generate the list of local sites and local parameters
      SetupWorkorderMap( DynVarMgrIF );
#endif

      SetupLocalSitesParams( DynVarMgrIF, aBondList, 1 );
      }

// 'old' form of Execute ... no precomputed bond vectors
     
     template <class DynVarMgr>
     void
     Execute(DynVarMgr & DynVarMgrIF
            , int aSimTick
            , int aRespaLevel
            , double& E
        )  __attribute__((noinline)) ;
        
     template<class DynVarMgr>
     void
     ExecuteInline(DynVarMgr & DynVarMgrIF, int aSimTick, int aRespaLevel , double& E)
      {
        BegLogLine(PKFXLOG_BONDED_SEQUENCE)
          << "LocalTupleListDriver::Execute "
          << UDFX_Bound::UDF::UdfName
          << " mLocalListSize=" << mLocalListSize
          << EndLogLine ;
      if( UDFX_Bound::GetTupleCount() == 0 )
         return;

      int RespaLevel_Of_UDF = MSD::RespaLevelByUDFCode[ UDFX_Bound::UDF::Code ];

      if( RespaLevel_Of_UDF  >  aRespaLevel )
        return;

      MDVM mdvm;
      mdvm.Init();
      mdvm.SetSiteRegMap( 0, 1, 2, 3 );
      mdvm.SetSimTick( aSimTick );
      E = 0.0;
      XYZ V;
      V.Zero();

      for(int TupleIndex = 0; TupleIndex < mLocalListSize; TupleIndex++ )
        {
#pragma execution_frequency(very_high)
        UDFX_Bound::TupleType & SiteTuple = mSites[ TupleIndex ];

        BegLogLine( PKFXLOG_LOCALTUPLELISTDRIVER_1 )
          << "TupleDriver working on " << " Code " <<  UDFX_Bound::UDF::Code
          << EndLogLine;

        #ifdef PK_PHASE4
          XYZ Pos0;

          if( UDFX_Bound::UDF::ReportsForce )
              Pos0 = DynVarMgrIF.GetInteractionPositionForSite( SiteTuple[ 0 ] );
          else
              Pos0 = DynVarMgrIF.GetPositionForSite( SiteTuple[ 0 ] );
        #else
          XYZ Pos0 = DynVarMgrIF.GetPosition( SiteTuple[ 0 ] );
        #endif

        for( int s = 0; s < UDFX_Bound::UDF::NumberOfSites; s++ )
          {
          mdvm.SetSiteIdInRegister( s, SiteTuple[ s ] );

          if ( UDFX_Bound::UDF::UsesPositions )
            {
            if( UDFX_Bound::UDF::ReportsForce )
              {
              // Image all the positions with respect to the first position
              XYZ posImaged;
              #ifdef PK_PHASE4
                XYZ Pos = DynVarMgrIF.GetInteractionPositionForSite( SiteTuple[ s ] );
              #else
                XYZ Pos = DynVarMgrIF.GetPosition( SiteTuple[ s ] );
              #endif

              NearestImageInPeriodicVolume( Pos0, Pos, posImaged );
              mdvm.SetPositionRegister( s, posImaged );
              BegLogLine(PKFXLOG_BOND_VECTOR)
                << "DriveOne::Execute slot " << s
                << " posImaged " << posImaged
                << EndLogLine ;
              }
            else //NEED: THIS else IS USING AN *** ACCIDENTAL *** SIDE EFFECT OF REPORTS FORCE -- THAT IS, THAT THE SITES WON'T SPAN FRAGS -- NO GOOD!!!!
              {
              #ifdef PK_PHASE4
                XYZ Pos = DynVarMgrIF.GetPositionForSite( SiteTuple[ s ] );
              #else
                XYZ Pos = DynVarMgrIF.GetPosition( SiteTuple[ s ] );
              #endif

              mdvm.SetPositionRegister( s, Pos );
              }
            }

          if ( UDFX_Bound::UDF::UsesVelocity )
            {
            #ifdef PK_PHASE4
            XYZ & Vel = DynVarMgrIF.GetVelocityForSite( SiteTuple[ s ] );
            #else
            XYZ Vel = DynVarMgrIF.GetVelocity( SiteTuple[ s ] );
            #endif

            mdvm.SetVelocityRegister( s, Vel );
            }
          }

        UDFX_Bound::ParamType & param = mParams[ TupleIndex ];
        // UDFX_Bound::ParamType param = UDFX_Bound::GetParam( TupleIndex );

        UDFX_Bound::UDF::Execute( mdvm, param );

        if( UDFX_Bound::UDF::ReportsForce )
          {
          if( UDFX_Bound::UDF::NumberOfSites == 2 )
            {
            BegLogLine(PKFXLOG_REGRESS_UDF_RESULTS)
              << FormatString("UDFREG % 6d ") << aSimTick
              << FormatString(" % 2d ")       << UDFX_Bound::UDF::Code
              << FormatString(" S0 % 5d ")    << SiteTuple[ 0 ]
              << FormatString(" S1 % 5d ")    << SiteTuple[ 1 ]
              << mdvm.GetEnergy() << " "  // Hope everything that reports a force, reports an energy too!
              << " F0 " << mdvm.GetForce(0)
              << " F1 " << mdvm.GetForce(1)
              << EndLogLine;
            }
          else if( UDFX_Bound::UDF::NumberOfSites == 3 )
            {
            BegLogLine(PKFXLOG_REGRESS_UDF_RESULTS)
              << FormatString("UDFREG % 6d ") << aSimTick
              << FormatString(" % 2d ")       << UDFX_Bound::UDF::Code
              << FormatString(" S0 % 5d ")    << SiteTuple[ 0 ]
              << FormatString(" S1 % 5d ")    << SiteTuple[ 1 ]
              << FormatString(" S2 % 5d ")    << SiteTuple[ 2 ]
              << mdvm.GetEnergy() << " "  // Hope everything that reports a force, reports an energy too!
              << " F0 " << mdvm.GetForce(0)
              << " F1 " << mdvm.GetForce(1)
              << " F2 " << mdvm.GetForce(2)
              << EndLogLine;
            }
          else // All others assume 4 sites
            {
            BegLogLine(PKFXLOG_REGRESS_UDF_RESULTS)
              << FormatString("UDFREG % 6d ") << aSimTick
              << FormatString(" % 2d ")       << UDFX_Bound::UDF::Code
              << FormatString(" S0 % 5d ")    << SiteTuple[ 0 ]
              << FormatString(" S1 % 5d ")    << SiteTuple[ 1 ]
              << FormatString(" S2 % 5d ")    << SiteTuple[ 2 ]
              << FormatString(" S3 % 5d ")    << SiteTuple[ 3 ]
              << mdvm.GetEnergy() << " "  // Hope everything that reports a force, reports an energy too!
              << " F0 " << mdvm.GetForce(0)
              << " F1 " << mdvm.GetForce(1)
              << " F2 " << mdvm.GetForce(2)
              << " F3 " << mdvm.GetForce(3)
              << EndLogLine;
            }
          }

        //Write the forces back.
        //NEED: Logic to ask if the site is actually resident.
        for( int s = 0; s < UDFX_Bound::UDF::NumberOfSites; s++ )
          {
          if ( UDFX_Bound::UDF::ReportsForce )
            {

            #ifdef PK_PHASE4
              #if defined( DO_BONDED_ON_SECOND_CORE )
                DynVarMgrIF.AddForceForSiteOnCoreOne( SiteTuple[ s ],
                                                      mdvm.GetForce( s ) );
              #else
                DynVarMgrIF.AddForceForSite( SiteTuple[ s ],
                                             mdvm.GetForce( s ) );
              #endif
            #else
              DynVarMgrIF.AddForce( SiteTuple[ s ],
                                    aSimTick,
                                    RespaLevel_Of_UDF,
                                    mdvm.GetForce( s ) );
            #endif

            /////////////////////////////// FRAGMENT BASED VIRIAL
            #if MSDDEF_DoPressureControl
            XYZ Force = mdvm.AccessForceRegister( s );
            XYZ Position = mdvm.GetPosition( s );
            XYZ Virial;
            Virial.mX = Force.mX * Position.mX;
            Virial.mY = Force.mY * Position.mY;
            Virial.mZ = Force.mZ * Position.mZ;
            DynVarMgrIF.AddVirial( RespaLevel_Of_UDF, Virial );
            #endif
            ///////////////////////////////////////////////

            // THE FOLLOWING IS REQUIRED BECAUSE MDVM::SetForce() is += and not just = ...
            mdvm.SetForceRegister( s, XYZ::ZERO_VALUE() );
            }

          if ( UDFX_Bound::UDF::ReportsVelocity )
            {
            #ifdef PK_PHASE4
              DynVarMgrIF.SetVelocityForSite( SiteTuple[ s ], mdvm.GetVelocity( s ));
            #else
              DynVarMgrIF.SetVelocity( SiteTuple[ s ], mdvm.GetVelocity( s ));
            #endif
            }

          // This is only needed for WaterInit. So far.
          if ( UDFX_Bound::UDF::ReportsPositions )
            {
            #ifdef PK_PHASE4
              DynVarMgrIF.SetPositionForSite( SiteTuple[ s ], mdvm.GetPosition( s ));
            #else
              DynVarMgrIF.SetPosition( SiteTuple[ s ], mdvm.GetPosition( s ));
            #endif
            }
          }

        //NEED: the way energy is handled in the mdvm is a little screwy.
        if ( UDFX_Bound::UDF::ReportsEnergy )
          {
          double e = mdvm.GetEnergy();
          E += e;
          }

        if ( UDFX_Bound::UDF::ReportsVirial )
          V += mdvm.GetVirial();
        }


      if ( UDFX_Bound::UDF::ReportsVirial )
        DynVarMgrIF.AddVirial( RespaLevel_Of_UDF, V ); // took out the /3.0 since it's done in the UDF

       }
       
     template <class DynVarMgr>
     void EmitEnergyForUDF(DynVarMgr & DynVarMgrIF
            , int aSimTick
            ,double E
     ) {
      int CurrentTS = aSimTick /  MSD::SimTicksPerTimeStep;
      if( UDFX_Bound::UDF::ReportsEnergy && aSimTick % MSD::SimTicksPerTimeStep == 0 )
        {
        int CurrentTS = aSimTick /  MSD::SimTicksPerTimeStep;

        #if MSDDEF_ReplicaExchange
          DynVarMgrIF.AddToPotentialEnergy( aSimTick, E );
        #endif

        if( CurrentTS % RTG.mEmitEnergyTimeStepModulo == 0 )
          {
          BegLogLine( PKFXLOG_LOCALTUPLELISTDRIVER )
            << "TupleDriver Emit E SimTick " << aSimTick << " TimeStep " << CurrentTS
            << " Code " <<  UDFX_Bound::UDF::Code << " E " << E << EndLogLine;

          #if !defined( DO_BONDED_ON_SECOND_CORE )
            EmitEnergy( CurrentTS, UDFX_Bound::UDF::Code, E, 0 );
          #else
            gUdfEnergy[ gUdfEnergyCount ].mUdfCode = UDFX_Bound::UDF::Code;
            gUdfEnergy[ gUdfEnergyCount ].mEnergy  = E;
            gUdfEnergyCount++;
          #endif
          }
        }
      
     }

#if !defined(OMIT_BOND_VECTOR_TABLE)   
     template <class DynVarMgr>
     void
     Execute(DynVarMgr & DynVarMgrIF
            , int aSimTick
            , int aRespaLevel
            , double& E
            , XYZw* Vectors
        )  __attribute__((noinline)) ;
     // 'new' form, with precomputed bond vectors
     template<class DynVarMgr>
     void
     ExecuteInline(DynVarMgr & DynVarMgrIF
            , int aSimTick
            , int aRespaLevel
            , double& aE
            , XYZw* Vectors
        )  
      {
        BegLogLine(PKFXLOG_BONDED_SEQUENCE)
          << "LocalTupleListDriver::Execute "
          << UDFX_Bound::UDF::UdfName
          << " mLocalListSize=" << mLocalListSize
          << EndLogLine ;
      if( UDFX_Bound::GetTupleCount() == 0 )
         return;

      int RespaLevel_Of_UDF = MSD::RespaLevelByUDFCode[ UDFX_Bound::UDF::Code ];

      if( RespaLevel_Of_UDF  >  aRespaLevel )
        return;


      // Going to pick off some of the UDFs (start with 2-site) for loop optimisations
      if ( UDFX_Bound::UDF::BondVectorOptimisationEnabled )
      {
         double E = 0.0 ;
       XYZ V;
       V.Zero();
       int LocalListSize = mLocalListSize ;
       const float Senses[2] = { -1.0, 1.0 } ; 
       switch (UDFX_Bound::UDF::NumberOfSites) {
        case 2: 
          for(int TupleIndex = 0; TupleIndex < LocalListSize; TupleIndex++ )
            {
              // By this stage, we are 'Standard Harmonic Bond'. But we are not supposed
              // to know this ...
    #pragma execution_frequency(very_high)
               MDVM_BOND_2 mdvm;
               mdvm.Init() ; 
               double SquareLength=Vectors[mBonds[TupleIndex][0]].mXYZ.LengthSquared() ;
               double RecipLength=Vectors[mBonds[TupleIndex][0]].mW ;
               mdvm.Prime(SquareLength*RecipLength, RecipLength) ;
             UDFX_Bound::TupleType & SiteTuple = mSites[ TupleIndex ];
    
             BegLogLine( PKFXLOG_LOCALTUPLELISTDRIVER_1 )
              << "TupleDriver working on " << " Code " <<  UDFX_Bound::UDF::Code
              << EndLogLine;
               BegLogLine(PKFXLOG_BOND_VECTOR)
                 << "Tuple " << TupleIndex
                 << " bond_number " << mBonds[TupleIndex][0]
                 << " bond_vector " << Vectors[mBonds[TupleIndex][0]].mXYZ
                 << " SquareLength=" << SquareLength
                 << " RecipLength=" << RecipLength
                 << " Length=" << SquareLength*RecipLength
                 << EndLogLine ;
            UDFX_Bound::ParamType & param = mParams[ TupleIndex ];
              
             UDFX_Bound::UDF::Execute( mdvm, param );
             
             XYZ Force = Vectors[mBonds[TupleIndex][0]].mXYZ 
                       * ( mdvm.GetdEdr() 
                           * RecipLength
                           * Senses[mBondSenses[TupleIndex][0]] 
                         ) ; 
                BegLogLine(PKFXLOG_REGRESS_UDF_RESULTS)
                  << FormatString("UDFREG % 6d ") << aSimTick
                  << FormatString(" % 2d ")       << UDFX_Bound::UDF::Code
                  << FormatString(" S0 % 5d ")    << SiteTuple[ 0 ]
                  << FormatString(" S1 % 5d ")    << SiteTuple[ 1 ]
                  << mdvm.GetEnergy() << " "  // Hope everything that reports a force, reports an energy too!
                  << " F0 " << Force
                  << " F1 " << (-Force)
                  << EndLogLine;
                #ifdef PK_PHASE4
                  #if defined( DO_BONDED_ON_SECOND_CORE )
                    DynVarMgrIF.AddForceForSiteOnCoreOne( SiteTuple[ 0 ],
                                                          Force );
                    DynVarMgrIF.AddForceForSiteOnCoreOne( SiteTuple[ 1 ],
                                                          -Force );
                  #else
                    DynVarMgrIF.AddForceForSite( SiteTuple[ 0 ],
                                                 Force );
                    DynVarMgrIF.AddForceForSite( SiteTuple[ 1 ],
                                                 -Force );
                  #endif  
                #else
                  DynVarMgrIF.AddForce( SiteTuple[ 0 ],
                                        aSimTick,
                                        RespaLevel_Of_UDF,
                                        Force );
                  DynVarMgrIF.AddForce( SiteTuple[ 1 ],
                                        aSimTick,
                                        RespaLevel_Of_UDF,
                                        -Force );
                #endif
           if ( UDFX_Bound::UDF::ReportsEnergy )
           {
                 double e = mdvm.GetEnergy();
                 E += e;
           } 
               if ( UDFX_Bound::UDF::ReportsVirial ) V += mdvm.GetVirial();
               }
      
            break;  
          case 3: 
            // An 'angle' force
          for(int TupleIndex = 0; TupleIndex < LocalListSize; TupleIndex++ )
            {
              // By this stage, we are 'Standard Harmonic Angle'. But we are not supposed
              // to know this ...
    #pragma execution_frequency(very_high)
               MDVM_BOND_N<2> mdvm;
               mdvm.Init() ; 
               double SquareLength0=Vectors[mBonds[TupleIndex][0]].mXYZ.LengthSquared() ;
               double RecipLength0=Vectors[mBonds[TupleIndex][0]].mW ;
               double SquareLength1=Vectors[mBonds[TupleIndex][1]].mXYZ.LengthSquared() ;
               double RecipLength1=Vectors[mBonds[TupleIndex][1]].mW ;
               mdvm.SetVector(0
                , Vectors[mBonds[TupleIndex][0]].mXYZ * Senses[mBondSenses[TupleIndex][0]]
                , SquareLength0*RecipLength0
                , RecipLength0) ;
               mdvm.SetVector(1
                , Vectors[mBonds[TupleIndex][1]].mXYZ * Senses[mBondSenses[TupleIndex][1]]
                , SquareLength1*RecipLength1
                , RecipLength1) ;
             
             UDFX_Bound::TupleType & SiteTuple = mSites[ TupleIndex ];
    
             BegLogLine( PKFXLOG_LOCALTUPLELISTDRIVER_1 )
              << "TupleDriver working on " << " Code " <<  UDFX_Bound::UDF::Code
              << EndLogLine;
             UDFX_Bound::ParamType & param = mParams[ TupleIndex ];
              
             UDFX_Bound::UDF::Execute( mdvm, param );
               BegLogLine(PKFXLOG_REGRESS_UDF_RESULTS)
                << FormatString("UDFREG % 6d ") << aSimTick
                << FormatString(" % 2d ")       << UDFX_Bound::UDF::Code
                << FormatString(" S0 % 5d ")    << SiteTuple[ 0 ]
                << FormatString(" S1 % 5d ")    << SiteTuple[ 1 ]
                << FormatString(" S2 % 5d ")    << SiteTuple[ 2 ]
                << mdvm.GetEnergy() << " "  // Hope everything that reports a force, reports an energy too!
                << " F0 " << mdvm.GetForce(0)
                << " F1 " << mdvm.GetForce(1)
                << " F2 " << mdvm.GetForce(2)
                << EndLogLine;
             
             for( int s = 0; s < UDFX_Bound::UDF::NumberOfSites; s++ )
              {
              if ( UDFX_Bound::UDF::ReportsForce )
                {
    
                #ifdef PK_PHASE4
                  #if defined( DO_BONDED_ON_SECOND_CORE )
                    DynVarMgrIF.AddForceForSiteOnCoreOne( SiteTuple[ s ],
                                                          mdvm.GetForce( s ) );
                  #else
                    DynVarMgrIF.AddForceForSite( SiteTuple[ s ],
                                                 mdvm.GetForce( s ) );
                  #endif
                #else
                  DynVarMgrIF.AddForce( SiteTuple[ s ],
                                        aSimTick,
                                        RespaLevel_Of_UDF,
                                        mdvm.GetForce( s ) );
                #endif
                }
              }
                             if ( UDFX_Bound::UDF::ReportsEnergy )
               {
              
                 double e = mdvm.GetEnergy();
                 E += e;
               }
               if ( UDFX_Bound::UDF::ReportsVirial ) V += mdvm.GetVirial();
            } 
  
            break ; 
          case 4: 
            // A 'torsion' or 'dihedral' force
          for(int TupleIndex = 0; TupleIndex < LocalListSize; TupleIndex++ )
            {
              // By this stage, we are a torsion or dihedral. But we are not supposed
              // to know this ...
    #pragma execution_frequency(very_high)
               MDVM_BOND_N<3> mdvm;
               mdvm.Init() ; 
               double SquareLength0=Vectors[mBonds[TupleIndex][0]].mXYZ.LengthSquared() ;
               double RecipLength0=Vectors[mBonds[TupleIndex][0]].mW ;
               double SquareLength1=Vectors[mBonds[TupleIndex][1]].mXYZ.LengthSquared() ;
               double RecipLength1=Vectors[mBonds[TupleIndex][1]].mW ;
               double SquareLength2=Vectors[mBonds[TupleIndex][2]].mXYZ.LengthSquared() ;
               double RecipLength2=Vectors[mBonds[TupleIndex][2]].mW ;
               mdvm.SetVector(0
                , Vectors[mBonds[TupleIndex][0]].mXYZ * Senses[mBondSenses[TupleIndex][0]]
                , SquareLength0*RecipLength0
                , RecipLength0) ;
               mdvm.SetVector(1
                , Vectors[mBonds[TupleIndex][1]].mXYZ * Senses[mBondSenses[TupleIndex][1]]
                , SquareLength1*RecipLength1
                , RecipLength1) ;
             
               mdvm.SetVector(2
                , Vectors[mBonds[TupleIndex][2]].mXYZ * Senses[mBondSenses[TupleIndex][2]]
                , SquareLength2*RecipLength2
                , RecipLength2) ;
             
             UDFX_Bound::TupleType & SiteTuple = mSites[ TupleIndex ];
    
             BegLogLine( PKFXLOG_LOCALTUPLELISTDRIVER_1 )
              << "TupleDriver working on " << " Code " <<  UDFX_Bound::UDF::Code
              << EndLogLine;
             UDFX_Bound::ParamType & param = mParams[ TupleIndex ];
              
             UDFX_Bound::UDF::Execute( mdvm, param );
               BegLogLine(PKFXLOG_REGRESS_UDF_RESULTS)
                << FormatString("UDFREG % 6d ") << aSimTick
                << FormatString(" % 2d ")       << UDFX_Bound::UDF::Code
                << FormatString(" S0 % 5d ")    << SiteTuple[ 0 ]
                << FormatString(" S1 % 5d ")    << SiteTuple[ 1 ]
                << FormatString(" S2 % 5d ")    << SiteTuple[ 2 ]
                << FormatString(" S3 % 5d ")    << SiteTuple[ 3 ]
                << mdvm.GetEnergy() << " "  // Hope everything that reports a force, reports an energy too!
                << " F0 " << mdvm.GetForce(0)
                << " F1 " << mdvm.GetForce(1)
                << " F2 " << mdvm.GetForce(2)
                << " F3 " << mdvm.GetForce(3)
                << EndLogLine;
             
             for( int s = 0; s < UDFX_Bound::UDF::NumberOfSites; s++ )
              {
              if ( UDFX_Bound::UDF::ReportsForce )
                {
    
                #ifdef PK_PHASE4
                  #if defined( DO_BONDED_ON_SECOND_CORE )
                    DynVarMgrIF.AddForceForSiteOnCoreOne( SiteTuple[ s ],
                                                          mdvm.GetForce( s ) );
                  #else
                    DynVarMgrIF.AddForceForSite( SiteTuple[ s ],
                                                 mdvm.GetForce( s ) );
                  #endif
                #else
                  DynVarMgrIF.AddForce( SiteTuple[ s ],
                                        aSimTick,
                                        RespaLevel_Of_UDF,
                                        mdvm.GetForce( s ) );
                #endif
                }
              }
               if ( UDFX_Bound::UDF::ReportsEnergy )
               {
              
                 double e = mdvm.GetEnergy();
                 E += e;
               }
               if ( UDFX_Bound::UDF::ReportsVirial ) V += mdvm.GetVirial();
            } 
            break ; 
          default: 
            // If we get here, someone has set up a UDF for bond vector optimisation,
            // but not set up code for how to do it ...
            assert(0) ; 
        } 
//	      int CurrentTS = aSimTick /  MSD::SimTicksPerTimeStep;
//	      if( UDFX_Bound::UDF::ReportsEnergy && aSimTick % MSD::SimTicksPerTimeStep == 0 )
//	        {
//	        int CurrentTS = aSimTick /  MSD::SimTicksPerTimeStep;
//	
//	        #if MSDDEF_ReplicaExchange
//	          DynVarMgrIF.AddToPotentialEnergy( aSimTick, E );
//	        #endif
//	        
//	        if( CurrentTS % RTG.mEmitEnergyTimeStepModulo == 0 )
//	          {
//	          BegLogLine( PKFXLOG_LOCALTUPLELISTDRIVER )
//	            << "TupleDriver Emit E SimTick " << aSimTick << " TimeStep " << CurrentTS
//	            << " Code " <<  UDFX_Bound::UDF::Code << " E " << E << EndLogLine;
//	          
//	          #if !defined( DO_BONDED_ON_SECOND_CORE )
//	            EmitEnergy( CurrentTS, UDFX_Bound::UDF::Code, E, 0 );
//	          #else
//	            gUdfEnergy[ gUdfEnergyCount ].mUdfCode = UDFX_Bound::UDF::Code;
//	            gUdfEnergy[ gUdfEnergyCount ].mEnergy  = E;
//	            gUdfEnergyCount++;
//	          #endif
//	          }
//	        }
  
        if ( UDFX_Bound::UDF::ReportsVirial ) {
          DynVarMgrIF.AddVirial( RespaLevel_Of_UDF, V ); // took out the /3.0 since it's done in the UDF
        } 
        aE = E ;
      } else {
        Execute( DynVarMgrIF
            , aSimTick
            , aRespaLevel
            , aE) ;
     
      }


 
     }
#endif
    };
template<
         class MDVM,
         class UDF1_Bound,
         class UDF2_Bound,
         class UDF3_Bound,
         class UDF4_Bound,
         class UDF5_Bound,
         class UDF6_Bound,
         class UDF7_Bound,
         class UDF8_Bound,
         class UDF9_Bound
         >
class SiteTupleDriver
  {
  public:

  BondList mBondList ;
  


  DriveOne < MDVM, UDF1_Bound > DriveOne_UDF1 MEMORY_ALIGN(5);
  DriveOne < MDVM, UDF2_Bound > DriveOne_UDF2 MEMORY_ALIGN(5);
  DriveOne < MDVM, UDF3_Bound > DriveOne_UDF3 MEMORY_ALIGN(5);
  DriveOne < MDVM, UDF4_Bound > DriveOne_UDF4 MEMORY_ALIGN(5);
  DriveOne < MDVM, UDF5_Bound > DriveOne_UDF5 MEMORY_ALIGN(5);
  DriveOne < MDVM, UDF6_Bound > DriveOne_UDF6 MEMORY_ALIGN(5);
  DriveOne < MDVM, UDF7_Bound > DriveOne_UDF7 MEMORY_ALIGN(5);
  DriveOne < MDVM, UDF8_Bound > DriveOne_UDF8 MEMORY_ALIGN(5);
  DriveOne < MDVM, UDF9_Bound > DriveOne_UDF9 MEMORY_ALIGN(5);

  template<class DynVarMgr>
  void
  Init(DynVarMgr & DynVarMgrIF)
    {
#if !defined(OMIT_BOND_VECTOR_TABLE)    	
      mBondList.mBondTable.Init() ;
      mBondList.mBondHashTable.Init() ;
      mBondList.mBondTable.SetAllocation(1024) ;
      mBondList.mBondHashTable.Allocate(8) ;
#endif    	
    DriveOne_UDF1.Init( DynVarMgrIF, mBondList );
    DriveOne_UDF2.Init( DynVarMgrIF, mBondList );
    DriveOne_UDF3.Init( DynVarMgrIF, mBondList );
    DriveOne_UDF4.Init( DynVarMgrIF, mBondList );
    DriveOne_UDF5.Init( DynVarMgrIF, mBondList );
    DriveOne_UDF6.Init( DynVarMgrIF, mBondList );
    DriveOne_UDF7.Init( DynVarMgrIF, mBondList );
    DriveOne_UDF8.Init( DynVarMgrIF, mBondList );
    DriveOne_UDF9.Init( DynVarMgrIF, mBondList );

    #if defined( DO_BONDED_ON_SECOND_CORE )
      rts_dcache_evict_normal();
    #endif
    }

  template<class DynVarMgr>
  void
  UpdateLocalSitesParams(DynVarMgr & DynVarMgrIF )
    {
#if !defined(OMIT_BOND_VECTOR_TABLE)    	
      mBondList.mBondTable.Clear() ;
      mBondList.mBondHashTable.Clear() ; 
#endif    	
    DriveOne_UDF1.SetupLocalSitesParams( DynVarMgrIF, mBondList );
    DriveOne_UDF2.SetupLocalSitesParams( DynVarMgrIF, mBondList );
    DriveOne_UDF3.SetupLocalSitesParams( DynVarMgrIF, mBondList );
    DriveOne_UDF4.SetupLocalSitesParams( DynVarMgrIF, mBondList );
    DriveOne_UDF5.SetupLocalSitesParams( DynVarMgrIF, mBondList );
    DriveOne_UDF6.SetupLocalSitesParams( DynVarMgrIF, mBondList );
    DriveOne_UDF7.SetupLocalSitesParams( DynVarMgrIF, mBondList );
    DriveOne_UDF8.SetupLocalSitesParams( DynVarMgrIF, mBondList );
    DriveOne_UDF9.SetupLocalSitesParams( DynVarMgrIF, mBondList );

    #if defined( DO_BONDED_ON_SECOND_CORE )
      rts_dcache_evict_normal();
    #endif
    }
    
//     int NeedsEnergyThisSimTick(int aSimTick)
//     {
//     	int CurrentTS = aSimTick /  MSD::SimTicksPerTimeStep;
//     	if (aSimTick % MSD::SimTicksPerTimeStep == 0)
//     	{
//         #if MSDDEF_ReplicaExchange
//           return 1 ;
//         #endif
//     		if (CurrentTS % RTG.mEmitEnergyTimeStepModulo == 0)
//     		{
//     			return 1 ;
//     		}
//     	}
//     	return 0 ; 
//     }


template<class DynVarMgr>
void
Execute(DynVarMgr & DynVarMgrIF, int aSimTick, int aRespaLevel  )
  {
  static TraceClient LTLDStart;
  static TraceClient LTLDFinis;

  gUdfEnergyCount = 0;

#pragma execution_frequency(very_low)

  BegLogLine(PKFXLOG_LOCALTUPLELISTDRIVER)
    << "LocalFragmentDirectDriver "
    << " BEG "
    << " UDF1_Bound: "
    << UDF1_Bound::UDF::UdfName
    << " UDF2_Bound: "
    << UDF2_Bound::UDF::UdfName
    << " UDF3_Bound: "
    << UDF3_Bound::UDF::UdfName
    << " UDF4_Bound: "
    << UDF4_Bound::UDF::UdfName
    << " UDF5_Bound: "
    << UDF5_Bound::UDF::UdfName
    << " UDF6_Bound: "
    << UDF6_Bound::UDF::UdfName
    << " UDF7_Bound: "
    << UDF7_Bound::UDF::UdfName
    << " UDF8_Bound: "
    << UDF8_Bound::UDF::UdfName
    << " UDF9_Bound: "
    << UDF9_Bound::UDF::UdfName
    << EndLogLine;

  #ifndef DO_BONDED_ON_SECOND_CORE
    LTLDStart.HitOE( PKTRACE_LOCAL_TUPLE_LIST_DRIVER,
                     "LocalTupleListDriver",
                     Platform::Topology::GetAddressSpaceId(),
                     LTLDStart );
  #endif

  double E1, E2, E3, E4, E5, E6, E7, E8, E9 ;
#if !defined(OMIT_BOND_VECTOR_TABLE)
  XYZw Vectors[mBondList.mBondTable.mSize] ; 
//  // TODO: Check that the compiler builds good code for this loop
//  for ( unsigned int s=1;s<mBondList.mBondTable.mSize;s+=1 )
//  {
//  	#pragma execution_frequency(very_high) 
//   	mBondList.mBondTable.mTable[s].Evaluate(DynVarMgrIF, Vectors[s] ) ;
//  }
  
  mBondList.mBondTable.Evaluate(DynVarMgrIF, Vectors) ; 
  DriveOne_UDF1.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E1, Vectors );
  DriveOne_UDF2.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E2, Vectors );
  DriveOne_UDF3.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E3, Vectors );
  DriveOne_UDF4.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E4, Vectors );
  DriveOne_UDF5.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E5, Vectors );
  DriveOne_UDF6.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E6, Vectors );
  DriveOne_UDF7.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E7, Vectors );
  DriveOne_UDF8.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E8, Vectors );
  DriveOne_UDF9.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E9, Vectors );
  DriveOne_UDF1.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E1) ;
  DriveOne_UDF2.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E2) ;
  DriveOne_UDF3.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E3) ;
  DriveOne_UDF4.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E4) ;
  DriveOne_UDF5.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E5) ;
  DriveOne_UDF6.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E6) ;
  DriveOne_UDF7.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E7) ;
  DriveOne_UDF8.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E8) ;
  DriveOne_UDF9.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E9) ;
  
#else
  DriveOne_UDF1.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E1 );
  DriveOne_UDF2.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E2 );
  DriveOne_UDF3.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E3 );
  DriveOne_UDF4.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E4 );
  DriveOne_UDF5.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E5 );
  DriveOne_UDF6.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E6 );
  DriveOne_UDF7.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E7 );
  DriveOne_UDF8.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E8 );
  DriveOne_UDF9.Execute( DynVarMgrIF, aSimTick, aRespaLevel, E9 );
  DriveOne_UDF1.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E1) ;
  DriveOne_UDF2.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E2) ;
  DriveOne_UDF3.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E3) ;
  DriveOne_UDF4.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E4) ;
  DriveOne_UDF5.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E5) ;
  DriveOne_UDF6.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E6) ;
  DriveOne_UDF7.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E7) ;
  DriveOne_UDF8.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E8) ;
  DriveOne_UDF9.EmitEnergyForUDF(DynVarMgrIF, aSimTick, E9) ;
#endif


  #if defined( DO_BONDED_ON_SECOND_CORE )
    rts_dcache_store( GET_BEGIN_PTR_ALIGNED( gUdfEnergy ),
                      GET_END_PTR_ALIGNED( gUdfEnergy + gUdfEnergyCount ) );

    rts_dcache_store( GET_BEGIN_PTR_ALIGNED( &gUdfEnergyCount ),
                      GET_END_PTR_ALIGNED( &gUdfEnergyCount + 1 ) );
  #endif

  #ifndef DO_BONDED_ON_SECOND_CORE
    LTLDFinis.HitOE( PKTRACE_LOCAL_TUPLE_LIST_DRIVER,
                     "LocalTupleListDriver",
                     Platform::Topology::GetAddressSpaceId(),
                     LTLDFinis );
  #endif  
    
  BegLogLine(PKFXLOG_LOCALTUPLELISTDRIVER)
    << "LocalFragmentDirectDriver "
    << " END "
    << " UDF1_Bound: "
    << UDF1_Bound::UDF::Code
    << " UDF2_Bound: "
    << UDF2_Bound::UDF::Code
    << " UDF3_Bound: "
    << UDF3_Bound::UDF::Code
    << " UDF4_Bound: "
    << UDF4_Bound::UDF::Code
    << " UDF5_Bound: "
    << UDF5_Bound::UDF::Code
    << " UDF6_Bound: "
    << UDF6_Bound::UDF::Code
    << " UDF7_Bound: "
    << UDF7_Bound::UDF::Code
    << " UDF8_Bound: "
    << UDF8_Bound::UDF::Code
    << " UDF9_Bound: "
    << UDF9_Bound::UDF::Code
    << EndLogLine;
  }
  };



#if defined(BONDED_COMPILE_EXECUTE) || !defined(BONDED_SEPARATE_EXECUTE)
#if !defined(OMIT_BOND_VECTOR_TABLE)
template<class MDVM,class UDFX_Bound>
   template<class DynVarMgr>
     void
     DriveOne<MDVM, UDFX_Bound>::Execute(DynVarMgr & DynVarMgrIF
                                          , int aSimTick
                                          , int aRespaLevel
                                          , double& aEnergy
                                          , XYZw* Vectors 
                                           ) 
     {
      ExecuteInline(DynVarMgrIF,aSimTick,aRespaLevel,aEnergy,Vectors) ;
     }
#endif
template<class MDVM,class UDFX_Bound>
   template<class DynVarMgr>
     void
     DriveOne<MDVM, UDFX_Bound>::Execute(DynVarMgr & DynVarMgrIF
                                          , int aSimTick
                                          , int aRespaLevel
                                          , double& aEnergy
                                           ) 
     {
      ExecuteInline(DynVarMgrIF,aSimTick,aRespaLevel,aEnergy) ;
     }
     
#endif
     
#if defined(BONDED_COMPILE_EXECUTE) && defined(BONDED_SEPARATE_EXECUTE)

#if !defined(OMIT_BOND_VECTOR_TABLE)
// 'New' format (drive with vectors) UDFs
template void DriveOne<
  MDVM, StdHarmonicBondForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy
        , XYZw* Vectors ) ; 
        
template void DriveOne<
  MDVM, StdHarmonicAngleForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy
        , XYZw* Vectors ) ; 
                
#if defined(NUMBER_OF_UreyBradleyForce_sites_SITE_TUPLES)            
template void DriveOne<
  MDVM, UreyBradleyForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy
        , XYZw* Vectors ) ; 
    
#endif    
#if defined(NUMBER_OF_SwopeTorsionForce_sites_SITE_TUPLES)            
template void DriveOne<
  MDVM, SwopeTorsionForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy
        , XYZw* Vectors ) ; 
        
#endif    
#if defined(NUMBER_OF_OPLSTorsionForce_sites_SITE_TUPLES)            
template void DriveOne<
  MDVM, OPLSTorsionForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy
        , XYZw* Vectors ) ; 
        
#endif    
#if defined(NUMBER_OF_OPLSImproperForce_sites_SITE_TUPLES)            
template void DriveOne<
  MDVM, OPLSImproperForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy
        , XYZw* Vectors ) ; 
        
#endif    
#if defined(NUMBER_OF_ImproperDihedralForce_sites_SITE_TUPLES)            
template void DriveOne<
  MDVM, ImproperDihedralForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy
        , XYZw* Vectors ) ; 
        
#endif    
template void DriveOne<
  MDVM, Coulomb14UDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy
        , XYZw* Vectors ) ; 
        
template void DriveOne<
  MDVM, LennardJones14UDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy
        , XYZw* Vectors ) ; 

#if ( defined( MSDDEF_EWALD ) || defined( MSDDEF_P3ME ) )        
template void DriveOne<
  MDVM, EwaldCorrectionUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy
        , XYZw* Vectors ) ; 
#endif
#endif

// 'Old' format (drive with atom positions)
template void DriveOne<
  MDVM, StdHarmonicBondForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy) ; 
        
template void DriveOne<
  MDVM, StdHarmonicAngleForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy) ; 

#if defined(NUMBER_OF_UreyBradleyForce_sites_SITE_TUPLES)            
template void DriveOne<
  MDVM, UreyBradleyForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy) ; 
     
#endif   
#if defined(NUMBER_OF_SwopeTorsionForce_sites_SITE_TUPLES)            
template void DriveOne<
  MDVM, SwopeTorsionForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy) ; 

#endif   
#if defined(NUMBER_OF_OPLSTorsionForce_sites_SITE_TUPLES)            
template void DriveOne<
  MDVM, OPLSTorsionForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy) ; 

#endif   
#if defined(NUMBER_OF_OPLSImproperForce_sites_SITE_TUPLES)            
template void DriveOne<
  MDVM, OPLSImproperForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy) ; 

#endif   
        
#if defined(NUMBER_OF_ImproperDihedralForce_sites_SITE_TUPLES)            
template void DriveOne<
  MDVM, ImproperDihedralForceUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy) ; 
#endif
        
template void DriveOne<
  MDVM, Coulomb14UDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy) ; 
        
template void DriveOne<
  MDVM, LennardJones14UDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy) ; 

#if ( defined( MSDDEF_EWALD ) || defined( MSDDEF_P3ME ) )        
template void DriveOne<
  MDVM, EwaldCorrectionUDF_Bound
>::Execute(
        TDynamicVariableManager<NUMBER_OF_SITES,NUMBER_OF_FRAGMENTS,1> & DynVarMgrIF
        , int aSimTick
        , int aRespaLevel
        , double& aEnergy) ; 
#endif
#endif     

#endif
