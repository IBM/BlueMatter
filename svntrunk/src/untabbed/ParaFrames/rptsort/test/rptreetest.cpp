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
 #include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

#ifndef PKFXLOG_RPTREETEST
#define PKFXLOG_RPTREETEST (0)
#endif

#ifndef PKFXLOG_RPTREETEST_SUMMARY
#define PKFXLOG_RPTREETEST_SUMMARY (1)
#endif


#include <rptree.hpp>
extern "C" {
void *
PkMain(int argc, char** argv, char** envp) ;
} ;

void *
PkMain(int argc, char** argv, char** envp)
{
  enum { k_TableSize = 100 } ;
  enum { k_StackSize = k_TableSize } ;
  unsigned int Values[k_TableSize] ;
  rptIndexElement IndexTable[k_TableSize] ;
  rptIndex myIndex(sizeof(unsigned int),sizeof(unsigned int),k_TableSize,Values,IndexTable);
  
  unsigned int key=1 ;
  const unsigned int kPolynomial = 0x80000005 ;
  for (int x=0;x<k_TableSize;x+=1)
  {
    unsigned int myCursorStack[k_StackSize] ;
    rptCursor myCursor(myIndex,myCursorStack,k_TableSize) ;
    myCursor.setToFirst() ;
    unsigned int v0 = 0 ; 
    unsigned int q=0 ;
    BegLogLine(PKFXLOG_RPTREETEST_SUMMARY)
      << "Keys added to tree: " << x 
      << EndLogLine
    while( myCursor.isValid() )
         {
          BegLogLine(PKFXLOG_RPTREETEST)
            << "q=" << q
            << " Values[" << myCursor.current()
            << "]=" << (void *)(Values[myCursor.current()])
            << EndLogLine ;
          q += 1 ;
          unsigned int v1 = Values[myCursor.current()] ;
          assert(v0 <= v1) ;
          assert(q <= x ) ;
          v0 = v1 ; 
          myCursor.setToNext() ;
         }
    assert(q == x) ;
    BegLogLine(PKFXLOG_RPTREETEST_SUMMARY)
      << "Adding Values[" << myIndex.getNextFreeSlot()
      << "]=0x" << (void *)key 
      << EndLogLine ;
    Values[myIndex.getNextFreeSlot()] = key ; myIndex.add() ; 
    unsigned int disorder = myIndex.walk() ;
    assert(0 == disorder) ;
    key=(key << 1) ^ ( ( key & 0x80000000 ) ? kPolynomial : 0 ) ;
  }
  unsigned int myCursorStack[k_StackSize] ;
  rptCursor myCursor(myIndex,myCursorStack,k_TableSize) ;
  myCursor.setToFirst() ;
  unsigned int v0 = 0 ; 
  unsigned int q=0 ;
  while( myCursor.isValid() )
       {
        BegLogLine(PKFXLOG_RPTREETEST_SUMMARY)
              << "q=" << q
          << " Values[" << myCursor.current()
          << "]=0x" << (void *)(Values[myCursor.current()])
          << EndLogLine ;
        q += 1 ;
        unsigned int v1 = Values[myCursor.current()] ;
        assert(v0 <= v1) ;
        v0 = v1 ; 
        myCursor.setToNext() ;
       }
//	Values[0] = 0 ;myIndex.add() ; myIndex.walk() ;
//	Values[1] = 1 ;myIndex.add() ; myIndex.walk() ;
//	Values[2] = 2 ;myIndex.add() ; myIndex.walk() ;
//	Values[3] = 100;myIndex.add() ; myIndex.walk() ;
//	Values[4] = 50;myIndex.add() ; myIndex.walk() ;
  
  
    return NULL ;
}
