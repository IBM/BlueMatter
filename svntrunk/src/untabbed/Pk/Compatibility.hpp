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
 #ifndef COMPATIBILITY_HPP_
#define COMPATIBILITY_HPP_

// 'Compatibility' items needed to get the FFT test case to build with Pk
#include <rts.h>
#include <BonB/BGL_PartitionSPI.h>
#include <new>
#define MEMORY_ALIGN(NUMBITS) __attribute__(( aligned(0x01<<NUMBITS )))

//// Point 'new' and 'delete' at the Pk heap rather than the 'malloc' heap
//void *operator new(std::size_t size) throw (std::bad_alloc)
//{
//	return PkHeapAllocate(size) ;
//}
//void* operator new[](std::size_t size) throw (std::bad_alloc)
//{
//	return PkHeapAllocate(size) ;
//}
//void operator delete(void *p) throw ()
//{
//	PkHeapFree(p) ;
//}
//void operator delete[](void *p) throw()
//{
//	PkHeapFree(p) ;
//}
//void* operator new(std::size_t size, const std::nothrow_t& nothrow) throw()
//{
//	return PkHeapAllocate(size) ;
//}
//void* operator new[](std::size_t size, const std::nothrow_t& nothrow) throw()
//{
//	return PkHeapAllocate(size) ;
//}
//void operator delete(void *p, const std::nothrow_t& nothrow) throw()
//{
//	PkHeapFree(p) ;
//}
//void operator delete[](void *p, const std::nothrow_t& nothrow) throw()
//{
//	PkHeapFree(p) ;
//}

class Platform
{
  public: 
  class Topology
  {
    public: 
    static void GetMyCoords(int * aPx, int * aPy, int * aPz) 
    {
      BGLPartitionGetCoords(aPx,aPy,aPz) ;
    }
    static void GetDimensions(int * aPx, int * aPy, int * aPz)
    {
      BGLPartitionGetDimensions(aPx,aPy,aPz) ;
    }
    static int GetAddressSpaceId(void) 
    {
      return BGLPartitionGetRank() ;
    }
    
  } ;
  class Control
  {
    public: 
    static void Barrier(void) 
    {
      PkNodeBarrier() ;
    } 
  } ;
  class Thread
  {
    public: 
    static int GetId(void) 
    {
      return rts_get_processor_id();
    }
  } ;
} ;

#endif /*COMPATIBILITY_HPP_*/
