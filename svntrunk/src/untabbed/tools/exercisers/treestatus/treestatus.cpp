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
 #include <bglmemmap.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


typedef unsigned int Bit32 ;

unsigned PA_TREE_VC1;

// volatile 32bit read
static inline uint32_t in32( uint32_t *vaddr );

#define TRx_DI      (0x00)    // Offset from Tree VCx for Data   Injection   (WO,Quad)
#define TRx_HI      (0x10)    // Offset from Tree VCx for Header Injection   (WO,Word)
#define TRx_DR      (0x20)    // Offset from Tree VCx for Data   Reception   (RO,Quad)
#define TRx_HR      (0x30)    // Offset from Tree VCx for Header Reception   (RO,Word)
#define TRx_Sx      (0x40)    // Offset from Tree VCx for Status             (RO,Word)
#define TRx_SO      (0x50)    // Offset from Tree VCx for Status of Other VC (RO,Word)

// Physical Addresses for Tree VC1 (P=V to use these addresses)
#define TR1_DI    (PA_TREE_VC1 | TRx_DI)
#define TR1_HI    (PA_TREE_VC1 | TRx_HI)
#define TR1_DR    (PA_TREE_VC1 | TRx_DR)
#define TR1_HR    (PA_TREE_VC1 | TRx_HR)
#define TR1_S1    (PA_TREE_VC1 | TRx_Sx)
#define TR1_S0    (PA_TREE_VC1 | TRx_SO)

typedef struct T_BGL_TreeFifoStatus
                {
                unsigned InjPyldCount  : 8; // Injection payload fifo entry (qword) count
                unsigned _notdefined1  : 4;
                unsigned InjHdrCount   : 4; // Injection header fifo entry count
                unsigned RecPyldCount  : 8; // Reception payload fifo entry (qword) count
                unsigned _notdefined2  : 3;
                unsigned RecIrq        : 1; // If non-zero, reception fifo contains an IRQ packet
                unsigned RecHdrCount   : 4; // Reception header fifo entry count
                }
                _BGL_TreeFifoStatus;

// Obtain status for Tree Virtual Channel 1
// #define _BGL_TREE_PKT_MAX_BYTES   256
// #define _BGL_TREE_PAYLOAD_SIZE ( _BGL_TREE_PKT_MAX_BYTES - sizeof( _BGL_TreeHwHdr ) )
// volatile 32bit read
static inline uint32_t in32( uint32_t *vaddr )
{
  volatile uint32_t *va = (volatile uint32_t *)vaddr;

//	_bgl_mbar();

  return( *va );
}


// Obtain status for Tree Virtual Channel 1
static inline void BGLTreeGetStatusVC1( _BGL_TreeFifoStatus *stat )
{
  Bit32 *status = (Bit32 *)stat;
  *status = in32( (uint32_t*) TR1_S1 );
}

// Obtain status for 'other' Tree Virtual Channel , i.e. VC0
static inline void BGLTreeGetStatusVC1Other( _BGL_TreeFifoStatus *stat)
{
  Bit32 *status = (Bit32 *)stat;
  *status = in32( (uint32_t*) TR1_S0 );
}


                
int main(int argc, const char **argv, const char **envp)
{
  int fd = open("/dev/tree1", O_RDWR );
  
  if ( fd < 0 )
     {
      int e=errno ;
      perror("Open /dev/tree1 failed") ;
      fprintf(stdout,"Open /dev/tree1 failed, errno=%d\n",errno) ;
      return 1 ;
     }
  
  PA_TREE_VC1 = (unsigned ) mmap( 0, 
          BGL_MEM_TREE_VC1_END - BGL_MEM_TREE_VC1_BASE, 
          PROT_READ | PROT_WRITE, 
          MAP_SHARED, 
          fd, 
          0 );
  if ( (unsigned int)MAP_FAILED ==  PA_TREE_VC1 )
  {
      int e=errno ;
      perror("mmap /dev/tree1 failed") ;
      fprintf(stdout,"mmap /dev/tree1 failed, errno=%d\n",errno) ;
      return 2 ;
  }
  _BGL_TreeFifoStatus stat;
  _BGL_TreeFifoStatus statOther;
  BGLTreeGetStatusVC1( &stat );
  BGLTreeGetStatusVC1( &statOther );

  fprintf(stdout, "VC1 stat=0x%08x InjPyldCount=%d _notdefined1=%d InjHdrCount=%d RecPyldCount=%d _notdefined2=%d RecIrq=%d RecHdrCount=%d\n",
              *(int *) (&stat), 
              stat.InjHdrCount, 
              stat._notdefined1,
              stat.InjHdrCount,
              stat.RecPyldCount,
              stat._notdefined2,
              stat.RecIrq,
              stat.RecHdrCount
              ) ;
  fprintf(stdout, "VC0 stat=0x%08x InjPyldCount=%d _notdefined1=%d InjHdrCount=%d RecPyldCount=%d _notdefined2=%d RecIrq=%d RecHdrCount=%d\n",
              *(int *) (&statOther), 
              statOther.InjHdrCount, 
              statOther._notdefined1,
              statOther.InjHdrCount,
              statOther.RecPyldCount,
              statOther._notdefined2,
              statOther.RecIrq,
              statOther.RecHdrCount
              ) ;
  return 0 ;
}
