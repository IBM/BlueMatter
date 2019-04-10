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
 #ifndef __IMPORT_EXPORT_FENCE__
#define __IMPORT_EXPORT_FENCE__
#include <sys/atomic_op.h>

void
ImportFence()
  {
  int a = 0 ;
  (void) _check_lock(&a, 0, 0) ;
  }

void
ExportFence()
   {
   int a;
  _clear_lock(&a,0) ;
  }

class Lock
{
  // As a user of the platform:: interface, you should probably use MUTEX.
  // This is because these locks don't fence memory - bad for RS6K.
public:
  
#ifdef PK_SPI
  typedef volatile unsigned int LockWord;  // per spi/locks.h ... would like an opaque SPI type.
#else
  typedef int LockWord;
#endif
  
  //Takes a reference to a LockWord
  //Returns TRUE for success and FALSE for failure.
  static inline int
  TryLock( LockWord &x )
    {      
      // printf("TryLock::LockWord::x: %08x, val: %d\n", &x, x);
      int rc = 0;
      int y = 0, z = 1;
      rc =  ! _check_lock ( &x, y, z );
      return( rc );
    }
  
  static inline void
  Unlock( LockWord &x )
    {
      // printf("UnLock::LockWord::x: %08x, val: %d\n", &x, x);
      int y = 0;
      _clear_lock( &x, y ) ;
    }
  
};

class Mutex
{
public:
  typedef Lock::LockWord MutexWord;
  
  static inline int
  TryLock( Lock::LockWord &aLockWord )
    {
      int rc = Lock::TryLock( aLockWord );
      ImportFence();
      return( rc );
    }

  static inline void
  YieldLock( Lock::LockWord &aLockWord )
    {
      while( ! Lock::TryLock( aLockWord ) )
        sched_yield();
      ImportFence();
    }
    
  static inline void
  Unlock( Lock::LockWord &aLockWord )
    {
      ExportFence();
      Lock::Unlock( aLockWord );
    }  
};

void AtomicAdd(int &aAtomicInteger, int aValue )
{
  fetch_and_add( &aAtomicInteger, aValue );  
}
#endif

