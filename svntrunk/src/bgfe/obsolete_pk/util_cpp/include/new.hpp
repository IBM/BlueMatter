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
 // provide a class providing interface to a
// long term dynamic memory allocations

#ifndef __PKNEW_HPP__
#define __PKNEW_HPP__

#include <sys/types.h>
#include <sys/mman.h>
#ifndef PKFXLOG_PKNEW
#define PKFXLOG_PKNEW (0)
#endif

#include <pk/fxlogger.hpp>

#define PKNEW_NAME_LEN (32)
#define CHECK_SUM_SEED 0xBFBFBFBF

//FOR LINUX, make default SAFETY_NET_OFF, this next line is ifNdef not ifdef
#ifndef SAFETY_NET_ON
class pknew
  {

  public:
    pknew() { }
    void
    CsmSet( )
      {
      }
    void
    CsmReset( )
      {
      }
    int CsmClear()
      {
      return(0);
      }

    int CsmVerify()
      {
      return( 1 );
      }
  };
#else
class pknew
  {
  struct
    {
    unsigned  CheckSumValue;
    size_t    mSize;
    } MetaData;
  public:
    pknew()
      {
      MetaData.CheckSumValue = 0;
      MetaData.mSize = 0;
      }

    void * operator new(size_t size)
      {
      // char * RealObject = new char [ size ];
      char * RealObject = (char *) mmap (
                                NULL     // System can choose where to map it
                                , size
                                , PROT_READ | PROT_WRITE  // We need to read and write the memory
                                , MAP_ANONYMOUS|MAP_VARIABLE // In paging space
                                , -1 // No associated file descriptor
                                , 0 // No associated offset
                                ) ;
      BegLogLine( PKFXLOG_PKNEW )
         << " new called "
         << " size "
         << size
         << " RealObject @: "
         << (void *) RealObject
         << EndLogLine;
      return( (void *) RealObject);
      }

    void operator delete( void *p, size_t size )
      {
      BegLogLine( PKFXLOG_PKNEW )
         << " delete called "
         << " size "
         << size
         << " address is: "
         << p
         << EndLogLine;
      // delete p;
      munmap(p, size) ;
      }

    // The rest of these fucntions expect meta data to be
    // stored in addresses below the 'this' pointer.
    void
    CsmSet( )
      {
      assert( MetaData.CheckSumValue == CHECK_SUM_SEED );
      MetaData.CheckSumValue += MetaData.mSize;
      for( int i = 0; i < MetaData.mSize ; i++ )
        MetaData.CheckSumValue += ((char *)this)[i];
      }

    void
    CsmReset( )
      {
      MetaData.CheckSumValue =  CHECK_SUM_SEED;
      MetaData.CheckSumValue += MetaData.mSize;
      for( int i = 0; i < MetaData.mSize ; i++ )
        MetaData.CheckSumValue += ((char *)this)[i];
      }

    int CsmClear()
      {
      assert( CsmVerify() );
      MetaData.CheckSumValue = CHECK_SUM_SEED;
      return(0);
      }

    int CsmVerify()
      {
      unsigned StackCheckSumValue =  CHECK_SUM_SEED;
      StackCheckSumValue += MetaData.mSize;
      for( int i = 0; i < MetaData.mSize ; i++ )
        StackCheckSumValue += ((char *)this)[i];
      return( MetaData.CheckSumValue == StackCheckSumValue );
      }
  };
#endif

#endif
