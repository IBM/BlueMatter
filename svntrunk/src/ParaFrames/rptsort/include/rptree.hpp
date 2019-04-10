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
 #ifndef RPTREE_HPP_
#define RPTREE_HPP_

#include <builtins.h>
// 'builtins' for '__cntlz4(X)'
// which counts the number of leading zeros (0-32) in the binary representation of X

#ifndef PKFXLOG_RADIXSORT
#define PKFXLOG_RADIXSORT (0)
#endif

#ifndef PKFXLOG_RADIXSORT_WALK
#define PKFXLOG_RADIXSORT_WALK (1)
#endif

#ifndef PKFXLOG_RADIXSORT_DETAIL
#define PKFXLOG_RADIXSORT_DETAIL (0)
#endif

#ifndef PKFXLOG_RPT_ADD
#define PKFXLOG_RPT_ADD (0)
#endif

#ifndef PKFXLOG_RPTCURSOR
#define PKFXLOG_RPTCURSOR (0)
#endif

#ifndef PKFXLOG_RPTCURSOR_ABBREV
#define PKFXLOG_RPTCURSOR_ABBREV (0)
#endif


/* 
 * Radix Partition Tree support
 * 
 * For how this works, see (for example) UPDATE TREE in SA22-7832 z/Architecture Principles of Operation
 */
class rptIndexElement {
public:         
    unsigned int mKeyWord ;
    unsigned int mKeyBit ;
    unsigned int mNextKey[2] ;  // Set the high bit if it indicates a leaf
} ;

template<class Record, class Key=Record,unsigned int KeyArraySize=1, unsigned int KeyOffset=0> 
class rptIndex {
public:
    unsigned int mRootNode ;
    const unsigned int mCapacity ;
    unsigned int mCount ;
    Record * const mRecordArray ;
    rptIndexElement * const mIndexArray ;
    rptIndex(unsigned int aCapacity,
             Record aRecordArray[],
             rptIndexElement aIndexArray[]) : 
                  mRootNode(0x80000000),
                  mCapacity(aCapacity),
                  mCount(0),
                  mRecordArray(aRecordArray),
                  mIndexArray(aIndexArray)
                    { } ;
    unsigned int getNextFreeSlot(void) const { return mCount; }
    unsigned int getMin(unsigned int X, unsigned int Y) const { return (X<Y) ? X : Y ; }
    // Max cursor stack that this index could ever need (with mCapacity elements)
    unsigned int getMaxCursorStackSize(void) const { return getMin( 8*KeyArraySize*sizeof(Key)+1 , mCapacity ) ; }
    // Max cursor stack that this index needs at the moment (with mCount elements) 
    unsigned int getRequiredCursorStackSize(void) const { return getMin(8*KeyArraySize*sizeof(Key)+1, mCount )  ; } 
    unsigned int add(void) ;
    void addKey(unsigned int& aSplitNode,
                unsigned int aSplitIndex,
                unsigned int& aNewKeyNode,
                const Key new_rid[]) ;
    unsigned int walk(unsigned int Verbosity=0) const ;
    unsigned int walkSubTree(unsigned int rootNode, int depth, Key key_lo, Key & key_hi, unsigned int & leafCount, unsigned int & stackDepth, unsigned int Verbosity ) const  __attribute__((noinline));
};


template<class Record, class Key=Record,unsigned int KeyArraySize=1, unsigned int KeyOffset=0> 
class rptCursor {
public: 
    const rptIndex<Record,Key,KeyArraySize,KeyOffset>& mRptIndex ;
    unsigned int * const mStack ;
    const unsigned int mStackSize ;
    int mCurrentStackIndex ; // nominally unsigned, but goes to -1 at the end of the scan
    unsigned int mCurrentRecordIndex ;
    unsigned int mIsValid ;
    rptCursor(const rptIndex<Record,Key,KeyArraySize,KeyOffset>& aRptIndex,
              unsigned int * aStack,
              unsigned int aStackSize) : mRptIndex(aRptIndex),
                                         mStack(aStack),
                                         mStackSize(aStackSize)
                                         { } ;
    void setToFirst(void) ;
    void setToNext(void) ;
    unsigned int current(void) const { return mCurrentRecordIndex ; } 
    unsigned int isValid(void) { return mIsValid; } ;
} ;

#include <builtins.h>
// 'builtins' for '__cntlz4(X)'
// which counts the number of leading zeros (0-32) in the binary representation of X

#include <rptree.hpp>

template<class Record, class Key,unsigned int KeyArraySize, unsigned int KeyOffset> 
void rptIndex<Record,Key,KeyArraySize,KeyOffset>::addKey(unsigned int& aSplitNode,
                      unsigned int aSplitIndex,
                      unsigned int &aNewKeyNode,
                      const Key * new_rid)
{
        if( (aSplitNode & 0x80000000) ||  
            mIndexArray[aSplitNode].mKeyWord*8*sizeof(Key)+mIndexArray[aSplitNode].mKeyBit >= aSplitIndex
            )
          {
                  // Ran into a leaf
            if( PKFXLOG_RPT_ADD)
              {
                 if( aSplitNode & 0x80000000 )
                   {
                     BegLogLine(PKFXLOG_RPT_ADD)
                         << "addKey reached leaf (0x" << (void *) aSplitNode
                         << ")"
                         << EndLogLine ;
                   }
                 else
                   {
                     BegLogLine(PKFXLOG_RPT_ADD)
                       << "addKey split tree (0x" << (void *) aSplitNode
                       << ") on bit " << (mIndexArray[aSplitNode].mKeyWord*8*sizeof(Key)+mIndexArray[aSplitNode].mKeyBit)
                       << "=0x" << (void *)(1 << (8*sizeof(Key)-1-mIndexArray[aSplitNode].mKeyBit) ) 
                       << EndLogLine ;                     
                   }
              }
                aNewKeyNode = aSplitNode ;
                aSplitNode = mCount-1 ;
          }
        else
          {
                  // Chase further down the tree
            unsigned int direction =
              ( new_rid[mIndexArray[aSplitNode].mKeyWord]
                        >>
                  (8*sizeof(Key)-1-mIndexArray[aSplitNode].mKeyBit)
              ) & (mIndexArray[aSplitNode].mKeyWord <  KeyArraySize) ;

            BegLogLine(PKFXLOG_RPT_ADD)
              << "addKey following tree (0x" << (void *) aSplitNode
              << ") on bit " << (mIndexArray[aSplitNode].mKeyWord*8*sizeof(Key)+(8*sizeof(Key)-1-mIndexArray[aSplitNode].mKeyBit))
              << "=0x" << (void *)(1 << (8*sizeof(Key)-1-mIndexArray[aSplitNode].mKeyBit) ) 
              << " direction " << direction
              << EndLogLine ;
            addKey<Record,Key,KeyArraySize,KeyOffset>(mIndexArray[aSplitNode].mNextKey[direction],
                            aSplitIndex,aNewKeyNode,new_rid) ;
          }
        }


template<class Record, class Key,unsigned int KeyArraySize, unsigned int KeyOffset> 
unsigned int rptIndex<Record,Key,KeyArraySize,KeyOffset>::add(void) 
{
    BegLogLine(PKFXLOG_RPT_ADD)
      << "rptIndex::add mCount=" << mCount
      << " mCapacity=" << mCapacity
      << " root_node=" << (mRootNode >> 31 ) << " " << (mRootNode & 0x7fffffff)
      << " key=0x" << (void *)(*((const Key * ) ((char *)(mRecordArray+mCount)+KeyOffset)))
      << EndLogLine ;
    assert(mCount < mCapacity) ;
    unsigned int cand_offset = mRootNode ;

     if( 0 == mCount)
       {
        /*
         * First node in the tree
         */
             mRootNode = 0x80000000 ;
       }
     else
       {
         /*
          * Scan the tree to find the appropriate data node
          */
         const Key * new_rid = (const Key * ) ((char *)(mRecordArray+mCount)+KeyOffset) ;
             
         BegLogLine(PKFXLOG_RPT_ADD)
           << "New key=0x" << (void *) (new_rid[0])
           << " KeyArraySize=" << KeyArraySize
           << " sizeof(Key)=" << sizeof(Key)
          << EndLogLine ;
  
         while (0 == (cand_offset & 0x80000000) )
         {
            unsigned int cand_index = cand_offset ;
            unsigned int candKeyWord = mIndexArray[cand_index].mKeyWord ;
            unsigned int candKeyShift = mIndexArray[cand_index].mKeyBit ;
              // Set direction, but if we're off the end of the key (into dups) force direction to '0'
            unsigned int direction = ( new_rid [ candKeyWord ] >> (8*sizeof(Key)-1-candKeyShift) ) & ( candKeyWord < KeyArraySize ) ;
            cand_offset = mIndexArray[cand_index].mNextKey [ direction ] ;
            BegLogLine(PKFXLOG_RPT_ADD)
              << "examining word " << candKeyWord
              << " (0x" << (void *)(new_rid [ candKeyWord ])
              << ") bit " << candKeyShift
              << "=0x" << (void *)(1 << (8*sizeof(Key)-1-candKeyShift) ) 
              << " (" << direction
              << ")"
              << " next cand_offset is " << (cand_offset >> 31) 
              << " " << (cand_offset & 0x7fffffff)
              << EndLogLine ;
            
         } /* endwhile */
         /*
          * We have now descended to a data node. Find which is the
          * most significant difference bit.
          */
         unsigned int candidateRecordIndex = cand_offset & 0x7fffffff ;
         const Key * cand_rid = (const Key * ) ((char *)(mRecordArray+candidateRecordIndex)+KeyOffset) ;
         
         unsigned int new_word ;
         unsigned int cand_word ;
         unsigned int rid_x ;
         for (rid_x = 0; rid_x < KeyArraySize; rid_x += 1)
         {
            cand_word = cand_rid [ rid_x ] ;
            new_word = new_rid [ rid_x ] ;
            if (cand_word != new_word) break ;
         } /* endfor */
  
         unsigned int rid_y = __cntlz4(cand_word ^ new_word) ;
         unsigned int leftshift_need = 31 - rid_y ; // '1<<X' sits on top of the bit of interest
         unsigned int rightshift_need = rid_y - 8*(4-sizeof(Key)) ; // bit index into the keyword/half/byte starting at 0 for its high bit
         
         unsigned int targetBit = rid_x * sizeof(Key)* 8 + rightshift_need ;
         mIndexArray[mCount-1].mKeyWord = rid_x ;
         mIndexArray[mCount-1].mKeyBit = rightshift_need ;
         // Set direction, but force new dups to be added 'first'
         unsigned int branchBit = (new_word >> (31-rid_y)) & ( cand_word != new_word ) ;
         mIndexArray[mCount-1].mNextKey[branchBit] = mCount | 0x80000000 ;
         BegLogLine(PKFXLOG_RPT_ADD)
             << "Adding key, split at word " << rid_x
             << " rid_y=" << rid_y  
             << " bit " <<  rightshift_need
             << " (0x" << (void *) (1 << leftshift_need)
             << ") cand_word=0x" << (void *)cand_word
             << " new_word=0x" << (void *)new_word
             << ( (cand_word == new_word) ? " (duplicate)" : " (unique)" )
             << " branchBit=" << branchBit
             << EndLogLine ;
         addKey<Record,Key,KeyArraySize,KeyOffset>(mRootNode,
                     targetBit,
                     mIndexArray[mCount-1].mNextKey[1-branchBit],
                     new_rid
                     ) ;
     } /* endif */
     
    mCount += 1 ;
    return mCount ;
}

template<class Record, class Key,unsigned int KeyArraySize, unsigned int KeyOffset> 
void rptCursor<Record,Key,KeyArraySize,KeyOffset>::setToFirst(void)
{
    BegLogLine(PKFXLOG_RPTCURSOR)
      << "rptCursor::setToFirst &mRptIndex=0x" << (void *)(&mRptIndex)
      << " mStack=0x" << (void *)mStack
      << " mStackSize=" << mStackSize
      << " count=" << mRptIndex.mCount
      << " rootNode=" << mRptIndex.mRootNode
      << EndLogLine ; 
    if( mRptIndex.mCount > 1 )
      {
        unsigned int treeIndex = mRptIndex.mRootNode ;
        mStack[0] = treeIndex ;
        BegLogLine(PKFXLOG_RPTCURSOR)
          << "mStack[0]=" << treeIndex
          << " --> (0x" << (void *)(mRptIndex.mIndexArray[treeIndex].mNextKey[0])
          << ",0x" << (void *)(mRptIndex.mIndexArray[treeIndex].mNextKey[1])
          << ")"
          << EndLogLine ; 
        unsigned int aStackIndex = 0 ;
        while( aStackIndex < mStackSize && 0 == (0x80000000 & mRptIndex.mIndexArray[treeIndex].mNextKey[0]))
             {
                 treeIndex = mRptIndex.mIndexArray[treeIndex].mNextKey[0] ;
                 aStackIndex += 1 ;
                 mStack[aStackIndex] = treeIndex ;
                 BegLogLine(PKFXLOG_RPTCURSOR)
                   << "mStack[" << aStackIndex
                   << "]=" << treeIndex
                   << " --> (0x" << (void *)(mRptIndex.mIndexArray[treeIndex].mNextKey[0])
                   << ",0x" << (void *)(mRptIndex.mIndexArray[treeIndex].mNextKey[1])
                   << ")"
                   << EndLogLine ; 
            }
        mIsValid = ( aStackIndex < mStackSize ) ;
        if( ! mIsValid )
          {
            BegLogLine(1)
              << "rptCursor::setToFirst warning: stack not large enough, aStackIndex=" << aStackIndex
              << " mStackSize=" << mStackSize
              << EndLogLine ;
          }
        mCurrentStackIndex = aStackIndex ;
        mCurrentRecordIndex = mRptIndex.mIndexArray[treeIndex].mNextKey[0] & 0x7fffffff ;
      }
    else if( mRptIndex.mCount == 1 )
      {
          mIsValid = 1 ;
          mCurrentStackIndex = -1 ;
          mCurrentRecordIndex = 0 ;
      }
    else
      {
        mIsValid = 0 ;
        mCurrentStackIndex = -1 ;
        BegLogLine(1)
          << "rptCursor::setToFirst warning: empty tree"
          << EndLogLine ; 
      }
    BegLogLine(PKFXLOG_RPTCURSOR || PKFXLOG_RPTCURSOR_ABBREV )
      << "rptCursor::setToFirst mIsValid=" << mIsValid
      << " mCurrentStackIndex=" << mCurrentStackIndex
      << " mCurrentRecordIndex=" << mCurrentRecordIndex
      << EndLogLine ; 
    if( PKFXLOG_RPTCURSOR) 
      {
        for(int x=0;x<=mCurrentStackIndex;x+=1)
           {
             BegLogLine(PKFXLOG_RPTCURSOR)
               << "mStack[" << x 
               << "]=" << mStack[x]
               << " --> (0x" << (void *)(mRptIndex.mIndexArray[mStack[x]].mNextKey[0])
               << ",0x" << (void *)(mRptIndex.mIndexArray[mStack[x]].mNextKey[1])
               << ")"
               << EndLogLine ;                  
           }
      }
    
}

template<class Record, class Key,unsigned int KeyArraySize, unsigned int KeyOffset> 
void rptCursor<Record,Key,KeyArraySize,KeyOffset>::setToNext(void)
{
    BegLogLine(PKFXLOG_RPTCURSOR)
      << "rptCursor::setToNext &mRptIndex=0x" << (void *)&mRptIndex
      << " mStack=0x" << (void *)mStack
      << " mStackSize=" << mStackSize
      << " mCurrentStackIndex=" << mCurrentStackIndex
      << EndLogLine ; 
    int aStackIndex = mCurrentStackIndex ;
    if( aStackIndex >= 0 )
      {
        unsigned int treeIndex = mStack[aStackIndex] ;
        unsigned int aScanHigher = mRptIndex.mIndexArray[treeIndex].mNextKey[1] ;
        BegLogLine(PKFXLOG_RPTCURSOR)
          << "aStackIndex=" << aStackIndex
          << " treeIndex=" << treeIndex
          << " mRptIndex.mIndexArray[" << treeIndex
          << "].mNextKey=(0x" << (void *)(mRptIndex.mIndexArray[treeIndex].mNextKey[0])
          << ",0x" << (void *)(mRptIndex.mIndexArray[treeIndex].mNextKey[1])
          << ")"
          << EndLogLine ;
        if( 0x80000000 & aScanHigher )
          {
                mCurrentStackIndex = aStackIndex - 1 ;
                mCurrentRecordIndex = aScanHigher & 0x7fffffff ;
                BegLogLine(PKFXLOG_RPTCURSOR)
                  << "Upper branch here is a leaf, dropping mCurrentStackIndex to " << mCurrentStackIndex
                  << EndLogLine ;
          }
        else
          {
                BegLogLine(PKFXLOG_RPTCURSOR)
                  << "Upper branch here is a branch, scanning ..."
                  << EndLogLine ;
                while( aStackIndex < (int)mStackSize && 0 == (0x80000000 & aScanHigher))
                    {
                        mStack[aStackIndex] = aScanHigher ;
                        aScanHigher=mRptIndex.mIndexArray[aScanHigher].mNextKey[0] ;
                        BegLogLine(PKFXLOG_RPTCURSOR)
                          << "mStack[" << aStackIndex
                          << "]=" << mStack[aStackIndex]
                          << EndLogLine ; 
                        aStackIndex += 1 ;
                     }
                mCurrentStackIndex = aStackIndex-1 ;
                mCurrentRecordIndex = aScanHigher & 0x7fffffff ;
                mIsValid = ( aStackIndex-1 < (int)mStackSize ) ;
                if( ! mIsValid )
                  {
                        BegLogLine(1)
                          << "rptCursor::setToNext warning: stack not large enough, aStackIndex=" << aStackIndex
                          << " mStackSize=" << mStackSize
                          << EndLogLine ;
                  }
                
            }
      }
    else
      {
        // At end of index
        mIsValid = 0 ;
      }
        
    BegLogLine(PKFXLOG_RPTCURSOR || PKFXLOG_RPTCURSOR_ABBREV )
      << "rptCursor::setToNext mIsValid=" << mIsValid
      << " mCurrentStackIndex=" << mCurrentStackIndex
      << " mCurrentRecordIndex=" << mCurrentRecordIndex
      << EndLogLine ; 
    if( PKFXLOG_RPTCURSOR) 
      {
        for(int x=0;x<=mCurrentStackIndex;x+=1)
        {
            BegLogLine(PKFXLOG_RPTCURSOR)
              << "mStack[" << x 
              << "]=" << mStack[x]
              << " --> (0x" << (void *)(mRptIndex.mIndexArray[mStack[x]].mNextKey[0])
              << ",0x" << (void *)(mRptIndex.mIndexArray[mStack[x]].mNextKey[1])
              << ")"
              << EndLogLine ; 
                
        }
      }
      
}


// Ascending and descending array-sorts

template<class Record, class Key,unsigned int KeyArraySize, unsigned int KeyOffset> 
static void RadixSort(Record aBase[], unsigned int aCount)
  {
    BegLogLine(PKFXLOG_RADIXSORT)
      << "RadixSort sizeof(Record)=" << sizeof(Record)
      << " sizeof(Key)=" << sizeof(Key)
      << " KeyArraySize=" << KeyArraySize
      << " KeyOffset=" << KeyOffset
      << " aBase=0x" << (void *) aBase
      << " aCount=" << aCount
      << EndLogLine ;
    if( aCount > 1 )
      {
        // Make the index
        
        void * aBaseVoid = (Record *) aBase ;
        rptIndexElement IndexTable[ aCount-1 ] ;
        rptIndex<Record, Key, KeyArraySize, KeyOffset> myIndex(aCount,aBase,IndexTable) ;
        for( unsigned int a=0;a<aCount;a+=1)
           {
                  myIndex.add() ;
           }
        // Copy the data to temp in index order
        Record sortedCopy[aCount] ;
        unsigned int cursorStackSize = myIndex.getRequiredCursorStackSize() ;
        unsigned int cursorStack[cursorStackSize] ;
        rptCursor<Record, Key, KeyArraySize, KeyOffset> myCursor(myIndex,cursorStack,cursorStackSize) ;
        myCursor.setToFirst() ;
        for( int b= 0 ;b<aCount;b+=1 )
           {
              assert(myCursor.isValid() ) ;
              sortedCopy[b] = aBase[myCursor.current()] ;
              myCursor.setToNext() ;
           }
        assert(! myCursor.isValid() ) ;
        // Copy the data back to where it came from, but in ascending order 
        for( unsigned int c=0;c<aCount;c+=1 )
          {
            aBase[c] = sortedCopy[c] ;
          }
      }
  }

template<class Record, class Key,unsigned int KeyArraySize, unsigned int KeyOffset> 
static void RadixSortDescending(Record aBase[], unsigned int aCount)
  {
    BegLogLine(PKFXLOG_RADIXSORT)
      << "RadixSortDescending sizeof(Record)=" << sizeof(Record)
      << " sizeof(Key)=" << sizeof(Key)
      << " KeyArraySize=" << KeyArraySize
      << " KeyOffset=" << KeyOffset
      << " aBase=0x" << (void *) aBase
      << " aCount=" << aCount
      << EndLogLine ;
    if( aCount > 1 )
      {
        // Make the index
        
        void * aBaseVoid = (Record *) aBase ;
        rptIndexElement IndexTable[ aCount-1 ] ;
        rptIndex<Record, Key, KeyArraySize, KeyOffset> myIndex(aCount,aBase,IndexTable) ;
        for( unsigned int a=0;a<aCount;a+=1)
           {
                  myIndex.add() ;
           }
        // Copy the data to temp in index order
        Record sortedCopy[aCount] ;
        unsigned int cursorStackSize = myIndex.getRequiredCursorStackSize() ;
        unsigned int cursorStack[cursorStackSize] ;
        rptCursor<Record, Key, KeyArraySize, KeyOffset> myCursor(myIndex,cursorStack,cursorStackSize) ;
        myCursor.setToFirst() ;
        for( int b= 0 ;b<aCount;b+=1 )
           {
              assert(myCursor.isValid() ) ;
              sortedCopy[b] = aBase[myCursor.current()] ;
              myCursor.setToNext() ;
           }
        assert(! myCursor.isValid() ) ;
        // Copy the data back to where it came from, but in descending order 
            for( unsigned int c=0;c<aCount;c+=1 )
              {
                aBase[c] = sortedCopy[aCount-1-c] ;
              }
      }
  }

static const char * spaceString(unsigned int count)
{
        const char * spaces = "                                                                                " ;
        return ( count > 80 ) ? spaces : (spaces+80-count) ;
}

template<class Record, class Key,unsigned int KeyArraySize, unsigned int KeyOffset> 
unsigned int rptIndex<Record,Key,KeyArraySize,KeyOffset>::walkSubTree(unsigned int aNode, int depth, Key key_lo, Key & key_hi, unsigned int & leafCount, unsigned int & stackDepth, unsigned int Verbosity ) const
{
        unsigned int rc=0 ;
        if(depth < 100)
          {
            if( aNode & 0x80000000 )
              {
                  unsigned int leafIndex = aNode & 0x7fffffff ;
                  const Key * rid_ptr = (const Key *)((const char *)(mRecordArray+leafIndex)+KeyOffset) ;
                  Key rid_value = * rid_ptr ;
                  if( Verbosity > 0 )
                    {
                      BegLogLine(PKFXLOG_RADIXSORT_WALK)
                        << spaceString(depth)
                        << "Leaf, index=0x" << (void *) leafIndex
                        << " depth=" << depth
                        << " key=0x" << (void *) rid_value
                        << EndLogLine ;
                    }
                  if( rid_value < key_lo )
                    {
                        BegLogLine(1)
                          << spaceString(depth)
                          << "Out of sequence"
                          << EndLogLine ;
                        rc = 1 ;
                    }
                  key_hi = rid_value ;
                  leafCount = 1 ;
                  stackDepth = 1 ;
              }
            else
              {
                if( Verbosity > 0 )
                  {
                    BegLogLine(PKFXLOG_RADIXSORT_WALK)
                      << spaceString(depth)
                      << "Branch, depth=" << depth
                      << " aNode=" << aNode
                      << " key_word=" << mIndexArray[aNode].mKeyWord
                      << " key_bit=" << mIndexArray[aNode].mKeyBit
                      << "=0x" << (void *)(1 << (8*sizeof(Key)-1-mIndexArray[aNode].mKeyBit) )
                      << " , [" << (mIndexArray[aNode].mNextKey[0] >> 31)
                      << " " << (mIndexArray[aNode].mNextKey[0] & 0x7fffffff)
                      << "," << (mIndexArray[aNode].mNextKey[1] >> 31)
                      << " " << (mIndexArray[aNode].mNextKey[1] & 0x7fffffff)
                      << "]"
                      << EndLogLine ;
                  }
                  assert(mIndexArray[aNode].mNextKey[0] != mIndexArray[aNode].mNextKey[1]) ;
                  Key key_mid ;
                  unsigned int leafCount1 ;

                  unsigned int stackDepth1 ;
                  int rc1 = walkSubTree<Record,Key,KeyArraySize,KeyOffset,Verbosity>(mIndexArray[aNode].mNextKey[0],depth+1, key_lo, key_mid, leafCount1, stackDepth1, Verbosity) ;
                  if( Verbosity > 0 )
                    {
                      BegLogLine(PKFXLOG_RADIXSORT_WALK)
                        << spaceString(depth)
                        << "--- depth=" << depth
                        << " aNode=" << aNode
                        << " key_word=" << mIndexArray[aNode].mKeyWord
                        << " key_bit=" << mIndexArray[aNode].mKeyBit
                        << "=0x" << (void *)(1 << (8*sizeof(Key)-1-mIndexArray[aNode].mKeyBit) )
                        << EndLogLine ;
                    }
                  unsigned int leafCount2 ;
                  unsigned int stackDepth2 ;
                  int rc2 = walkSubTree<Record,Key,KeyArraySize,KeyOffset,Verbosity>(mIndexArray[aNode].mNextKey[1],depth+1, key_mid, key_hi, leafCount2, stackDepth2, Verbosity ) ;
                  rc = rc1|rc2 ;
                  leafCount = leafCount1 + leafCount2 ;
                  stackDepth = ( stackDepth2 > stackDepth1+1) ? stackDepth2 : (stackDepth1+1) ;
                  if( Verbosity > 0 )
                    {
                      BegLogLine(PKFXLOG_RADIXSORT_WALK)
                        << spaceString(depth)
                        << "End branch, depth=" << depth
                        << " aNode=" << aNode
                        << " key_word=" << mIndexArray[aNode].mKeyWord
                        << " key_bit=" << mIndexArray[aNode].mKeyBit
                        << "=0x" << (void *)(1 << (8*sizeof(Key)-1-mIndexArray[aNode].mKeyBit) )
                        << " leafcount (" << leafCount1
                        << "+" << leafCount2
                        << ")=" << (leafCount1+leafCount2)
                        << " stackDepth (" << stackDepth1
                        << "," << stackDepth2
                        << ")-->" << stackDepth
                        << EndLogLine ;
                    }

              }
          }
        else
          {
                BegLogLine(1)
                  << spaceString(depth)
                  << "Depth limit exceeded"
                  << EndLogLine ;
                rc=1 ;
                leafCount=0 ;
          }

        return rc ;
}

template<class Record, class Key,unsigned int KeyArraySize, unsigned int KeyOffset> 
unsigned int rptIndex<Record,Key,KeyArraySize,KeyOffset>::walk(unsigned int Verbosity) const
{
    BegLogLine(PKFXLOG_RADIXSORT_WALK)
      << "Walking tree, root_node=" << (mRootNode >> 31 ) << " " << (mRootNode & 0x7fffffff)
      << " count=" << mCount
      << EndLogLine ;
    if( Verbosity > 0 )
      {
        for(  int aNode=0;aNode<(int)mCount-1;aNode+=1 )
           {
             const Key * rid_0 = (const Key * ) ((char *)(mRecordArray+(mIndexArray[aNode].mNextKey[0] & 0x7fffffff))+KeyOffset) ;
             const Key * rid_1 = (const Key * ) ((char *)(mRecordArray+(mIndexArray[aNode].mNextKey[1] & 0x7fffffff))+KeyOffset) ;
              BegLogLine(PKFXLOG_RADIXSORT_WALK)
                << "Branch[" << aNode
                << "] key_word=" << mIndexArray[aNode].mKeyWord
                << " key_bit=" << mIndexArray[aNode].mKeyBit
                << "=0x" << (void *)(1 << (8*sizeof(Key)-1-mIndexArray[aNode].mKeyBit) )
                << " , [" << (mIndexArray[aNode].mNextKey[0] >> 31)
                << " " << (mIndexArray[aNode].mNextKey[0] & 0x7fffffff)
                << "," << (mIndexArray[aNode].mNextKey[1] >> 31)
                << " " << (mIndexArray[aNode].mNextKey[1] & 0x7fffffff)
                << "] 0x" << (void *)((mIndexArray[aNode].mNextKey[0] & 0x80000000) ? (rid_0[0]) : 0xffffffff)
                << " 0x" << (void *)((mIndexArray[aNode].mNextKey[1] & 0x80000000) ? (rid_1[0]) : 0xffffffff)
                << EndLogLine ;
           }
       }
    if( mCount > 0 )
      {
        Key key_hi ;
        unsigned int leafCount ;
        unsigned int stackDepth ;
        unsigned int rc=walkSubTree<Record,Key,KeyArraySize,KeyOffset>(mRootNode,0,0,key_hi,leafCount,stackDepth, Verbosity) ;
        BegLogLine(PKFXLOG_RADIXSORT_WALK)
          << "Walk complete, walkSubTree rc=" << rc
          << " leafCount=" << leafCount
          << " count=" << mCount
          << " stackDepth=" << stackDepth
          << " key_hi=0x" << (void *) key_hi
          << EndLogLine ;
        return ( rc | (leafCount != mCount) ) ;
      }
    else
      {
        BegLogLine(PKFXLOG_RADIXSORT_WALK)
          << "Empty"
          << EndLogLine ;
        return 0 ;
      }
}

#endif /*RPTREE_HPP_*/
