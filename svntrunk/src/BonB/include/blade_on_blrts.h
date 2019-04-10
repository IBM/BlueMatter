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
 
#ifndef _BLADE_ON_BLRTS_H_  // Prevent multiple inclusion.
#define _BLADE_ON_BLRTS_H_

// Defines and macros for Host Compiler Support
#if defined(__cplusplus) && !defined(__ASSEMBLY__)

#define __BEGIN_DECLS extern "C" {

#define __END_DECLS   }

#else  // not __cplusplus or __ASSEMBLY__

#define __BEGIN_DECLS

#define __END_DECLS

#endif // __cplusplus

#ifndef __INLINE__
#define __INLINE__ extern inline
#endif

#ifndef __ASSEMBLY__

#include <stdio.h>
#include <bglpersonality.h>
#include <bglmemmap.h>
#include <BonB/BGLGi.h>
#include <BonB/blade_env.h>

__BEGIN_DECLS

#define KILO (1024)
#define MEGA (1024 * 1024)
#define GIGA (1024 * 1024 * 1024)

#define ALIGN16        __attribute__ ((aligned ( 16)))
#define ALIGN_QUADWORD __attribute__ ((aligned ( 16)))
#define ALIGN_CACHE    __attribute__ ((aligned ( 32)))
#define ALIGN_L1_CACHE __attribute__ ((aligned ( 32)))
#define ALIGN_L2_CACHE __attribute__ ((aligned (128)))
#define ALIGN_L3_CACHE __attribute__ ((aligned (128)))

#define K_TEXT
#define K_DATA

// Pack C data structures.  Use with care to avoid alignment problems.
#define PACKED __attribute__ ((packed))

typedef unsigned char   bit8;
typedef unsigned char   Bit8;

typedef unsigned short  bit16;
typedef unsigned short  Bit16;

typedef unsigned long   bit32;
typedef unsigned long   Bit32;

typedef unsigned long long  bit64;
typedef unsigned long long  Bit64;

typedef union T_QuadWord
               {
               Bit8   ub[16];
               Bit16  us[ 8];
               Bit32  ul[ 4];
               Bit64 ull[ 2];
               }
               ALIGN_QUADWORD _QuadWord;

typedef _QuadWord bit128;
typedef _QuadWord Bit128;

#define QUADWORD _QuadWord

//#ifndef PK_BGL
typedef struct T_iovec
                {
                char *base;
                int  len;
                }
                iovec_g;
//#endif

// Define the bstrap_flags used in _Arg_Info
#define BS_L1I_DISABLE       _BN( 0)    // Disable L1-I cache for Kernel
#define BS_L1D_DISABLE       _BN( 1)    // Disable L1-D cache for Kernel
#define BS_L2_DISABLE        _BN( 2)    // Disable L2 for Kernel
#define BS_L2_PF_OPT         _BN( 3)    // L2: Optimistic PF Mode (if not disabled)
#define BS_L3_DISABLE        _BN( 4)    // Disable L3 for Kernel
#define BS_L3_PF_DIS         _BN( 5)    // L3: Disable PF (if not disabled)
#define BS_ETH_ENABLE        _BN( 6)    // Ethernet Enable
#define BS_TS_ENABLE         _BN( 7)    // Torus Enable (not on I/O Node)
#define BS_TR_ENABLE         _BN( 8)    // Tree  Enable
#define BS_GI_ENABLE         _BN( 9)    // Global Interrupts Enable
#define BS_WARN_DISABLE      _BN(10)    // Disable Kernel Trace Warnings
#define BS_TSCA_ENABLE       _BN(11)    // Torus Capture Units Enable
#define BS_TRCA_ENABLE       _BN(12)    // Tree Capture Units Enable
#define BS_CA_LOOPBACK       _BN(13)    // Force Single-Node Loopback in Capture Units
#define BS_L3_PFC_SET        _BN(14)    // Set L3_PFC_DEPTH
#define BS_UPC_ENABLE        _BN(15)    // Enable UPC Configured via upc_info.
#define BS_TS_MESH_X         _BN(16)    // X Dimension of Torus is a Mesh (not a Torus)
#define BS_TS_MESH_Y         _BN(17)    // Y Dimension of Torus is a Mesh (not a Torus)
#define BS_TS_MESH_Z         _BN(18)    // Z Dimension of Torus is a Mesh (not a Torus)
#define BS_IO_NODE           _BN(19)    // This node is an I/O Node, not a Compute Node.
#define BS_SPLIT_MODE        _BN(20)    // Split Mode if set, Symmetric Mode if clear.
#define BS_CA_PRETRAINED     _BN(21)    // BootStrapper Started BiDi Training Sequence.
#define BS_POWER_SAVE        _BN(22)    // Enable Power-Down of Idle Units.
#define BS_L3_PFC_DEPTH(x)   _B3(25,x)  // L3 Prefetch Depth for Cores.
#define BS_DDR_SIZE(x)       _B2(27,x)  // Size of DDR as below:
#define BS_DDR_SIZE_256M     BS_DDR_SIZE(0)
#define BS_DDR_SIZE_512M     BS_DDR_SIZE(1)
#define BS_DDR_SIZE_1G       BS_DDR_SIZE(2)
#define BS_DDR_SIZE_2G       BS_DDR_SIZE(3)
#define BS_SCRATCH_8THS(x)   _B4(31,x)  // 8ths of L3 eDRAM to use as Scratch (8 Disables L3)

typedef union  T_BStrap_Flags
                {
                unsigned long flags;
                struct {
                       unsigned l1i_disable  :  1;
                       unsigned l1d_disable  :  1;
                       unsigned l2_disable   :  1;
                       unsigned l2_pf_opt    :  1;
                       unsigned l3_disable   :  1;
                       unsigned l3_pf_dis    :  1;
                       unsigned eth_enable   :  1;
                       unsigned ts_enable    :  1;
                       unsigned tr_enable    :  1;
                       unsigned gi_enable    :  1;
                       unsigned warn_disable :  1;
                       unsigned tsca_enable  :  1;
                       unsigned trca_enable  :  1;
                       unsigned ca_loopback  :  1;
                       unsigned l3_pfc_set   :  1;
                       unsigned upc_enable   :  1;
                       unsigned ts_mesh_x    :  1;
                       unsigned ts_mesh_y    :  1;
                       unsigned ts_mesh_z    :  1;
                       unsigned io_node      :  1;
                       unsigned split_mode   :  1;
                       unsigned ca_pretrain  :  1;
                       unsigned power_save   :  1;
                       unsigned l3_pfc_depth :  3;
                       unsigned ddr_size     :  2;
                       unsigned scratch_8ths :  4;
                       };
                }
                _BStrap_Flags;


// See blade_on_blrts.c (as opposed to bl_main.c in real BLADE).
//  This is a small IMPROPER SUBSET of BLADE's real kernel config structure.
typedef struct T_Blade_Kernel_Config
                {
                _BStrap_Flags bs_flags;

                int32_t hard_processor_id;

                uint32_t AppCodeSeg;

                int is_VirtualNodeMode;

                // Coordinates of this Node.
                int32_t X,
                        Y,         // Caution: these fields have different
                        Z,         //  interpretation if is_io_node is True.
                        Rank;

                int32_t real_Rank; // If Single-Node-Loopback, this holds our
                                   //  real rank in the partition, as opposed
                                   //  to the lies elsewhere.

                // Partition Info
                int32_t max_X,
                        max_Y,
                        max_Z,
                        max_Rank;

                int32_t nodes_X, // number of nodes per dimension
                        nodes_Y,
                        nodes_Z;

                int32_t is_mesh_X,  // dimension is a mesh when True,
                        is_mesh_Y,  //   else Torus when False.
                        is_mesh_Z;

                int32_t hint_cutoff_XP,
                        hint_cutoff_XM,
                        hint_cutoff_YP,
                        hint_cutoff_YM,
                        hint_cutoff_ZP,
                        hint_cutoff_ZM;


                int32_t is_io_node,       // is this node an I/O Node?
                        nodes_IO,         // number of I/O Nodes in this partition.
                        io_node_p2p_addr, // Point-to-Point Address of my I/O Node.
                        node_p2p_addr;    // this node's Point-to-Point Address.

                int32_t freq_in_MHz;

                int32_t have_GlobalInts;

                BGLPersonality mmcs_personality;
                }
                _Blade_Kernel_Config;


// see bl_main.c
extern _Blade_Kernel_Config _Blade_Config_Area;

#define hard_processor_id(void) (_Blade_Config_Area.hard_processor_id)
#define Sim_Terminate(rc) exit(rc)
#define s0printf          printf

#define __raw_readb(addr)   (*(volatile unsigned char  *)(addr))
#define __raw_reads(addr)   (*(volatile unsigned short *)(addr))
#define __raw_readl(addr)   (*(volatile unsigned long  *)(addr))

#define in8  __raw_readb
#define in16 __raw_reads
#define in32 __raw_readl

#define __raw_writeb(v, addr)   (*(volatile unsigned char  *)(addr) = (v))
#define __raw_writes(v, addr)   (*(volatile unsigned short *)(addr) = (v))
#define __raw_writel(v, addr)   (*(volatile unsigned long  *)(addr) = (v))

#define out8(addr,val)  __raw_writeb( (val), (addr) )
#define out16(addr,val) __raw_writes( (val), (addr) )
#define out32(addr,val) __raw_writel( (val), (addr) )

/* Zero-Instruction GCC Optimizer barrier */
#define barrier() do { asm volatile("": : :"memory"); } while(0)

/*
 * Enforce In-order Execution of I/O:
 * Acts as a barrier to ensure all previous I/O accesses have
 * completed before any further ones are issued.
 */
extern inline void eieio(void)
{
   asm volatile("mbar" : : : "memory");
}
// alias for eieio()
extern inline void msync(void)
{
   asm volatile("msync" : : : "memory");
}

#define wmb eieio

/* Heavy-weight barrier: memory + context synchronization. */
extern inline void isync( void )
{
   asm volatile("isync" : : : "memory");
}


#define STR(s) #s

//@MG 20030606: Applied Matt's changes to clobber list.
#if defined(PK_XLC)
// For xlC, the floating-point regs are called "f0" etc.; and you cannot separate the 'quad load' from the 'quad store'
#define QuadMove(src,dst,f)  asm volatile( "lfpdx  " STR(f) ",0,%0;" \
                                           "stfpdx " STR(f) ",0,%1;" \
                                           :                         \
                                           : "r" (src),              \
                                             "r" (dst)               \
                                           : "memory",               \
                                             "f" STR(f) )

//@MG 20030606: Imported Martin's changes from sysbringup's blade_workarounds.h
#define _bl_mov128b(si,sb,di,db) \
do { \
  asm volatile("lfpdx 0,%0,%1;stfpdx 0,%2,%3": :"Ob" (si), "r" (sb), "Ob" (di), "r" (db):"f0","memory"); \
} while(0)

#define _bl_mov128b_update(si,sb,di,db) \
do { \
  asm volatile("lfpdux 0,%0,%1;stfpdux 0,%2,%3": :"Ob" (si), "r" (sb), "Ob" (di), "r" (db):"f0","memory"); \
} while(0)
#else
#define QuadLoad(v,f)  asm volatile( "lfpdx " STR(f) ",0,%0" :: "r" (v) : "fr" STR(f) )

#define QuadStore(v,f) asm volatile( "stfpdx " STR(f) ",0,%0" :: "r" (v) : "memory" )

#define QuadMove(src,dst,f)  asm volatile( "lfpdx  " STR(f) ",0,%0;" \
                                           "stfpdx " STR(f) ",0,%1;" \
                                           :                         \
                                           : "r" (src),              \
                                             "r" (dst)               \
                                           : "memory",               \
                                             "fr" STR(f) )

//@MG 20030606: Imported Martin's changes from sysbringup's blade_workarounds.h
#define _bl_mov128b(si,sb,di,db) \
do { \
  asm volatile("lfpdx 0,%0,%1;stfpdx 0,%2,%3": :"Ob" (si), "r" (sb), "Ob" (di), "r" (db):"fr0","memory"); \
} while(0)

#define _bl_mov128b_update(si,sb,di,db) \
do { \
  asm volatile("lfpdux 0,%0,%1;stfpdux 0,%2,%3": :"Ob" (si), "r" (sb), "Ob" (di), "r" (db):"fr0","memory"); \
} while(0)
#endif


// commented out calls are privileged
#define dcache_zero_line_index(v,i) do { asm volatile("dcbz %1,%0" : : "r" (v), "Ob" (i) : "memory"); } while(0)
#define dcache_zero_line(v)         do { asm volatile("dcbz  0,%0" : : "r" (v) : "memory"); } while(0)
#define dcache_flush_line(v)        do { asm volatile("dcbf  0,%0" : : "r" (v) : "memory"); } while(0)
#define dcache_store_line(v)        do { asm volatile("dcbst 0,%0" : : "r" (v) : "memory"); } while(0)
#define dcache_touch_line(v)        do { asm volatile("dcbt  0,%0" : : "r" (v)           ); } while(0)
#define dcache_invalidate_line(v)   do { asm volatile("dcbi  0,%0" : : "r" (v) : "memory"); } while(0)
//#define icache_invalidate_line(v)   do { asm volatile("icbi  0,%0" : : "r" (v) : "memory"); } while(0)
#define icache_touch_line(v)        do { asm volatile("icbt  0,%0" : : "r" (v)           ); } while(0)

// subset routines from kernel/bl_cache.S
extern void dcache_flush_range( void *start, void *stop  );
extern void dcache_flush_bytes( void *start, Bit32 bytes );
extern void dcache_store_range( void *start, void *stop  );
extern void dcache_store_bytes( void *start, Bit32 bytes );
//extern void dcache_invalidate_bytes( void *start, Bit32 bytes );
//extern void dcache_invalidate_range( void *start, void *stop );
extern void dcache_flush_all( void );
//extern void icache_flush_range( void *start, void *stop  ); // also flushes D-Cache (eg install breakpoints etc).
//extern void icache_flush_all( void ); // does both i&d caches (eg install breakpoints)
//extern void icache_invalidate_range( void *start, void *stop  );
//extern void icache_invalidate_bytes( void *start, Bit32 bytes );
extern void icache_touch_range( void *start, void *stop  );
extern void icache_touch_bytes( void *start, Bit32 bytes );


extern Bit32 mfdcr( unsigned dcrn );
extern void mtdcr( unsigned dcrn, unsigned val );

#include <BonB/bl_dcrmap.h>
#include <BonB/bl_physmap.h>
#include <BonB/bl_lockbox.h>
#include <BonB/bl_torus.h>
#include <BonB/bl_tree.h>
#include <BonB/BGL_PartitionSPI.h>
#include <BonB/BGL_TorusPktSPI.h>
#include <BonB/BGL_TreePktSPI.h>

//
// See blade_on_blrts.c in libblade_spi.a
//
//   Returns 0 on successful initialization.
//   Non-Zero return code is fatal!
//
int _blade_on_blrts_init( void );

__END_DECLS

#else  // __ASSEMBLY__

#define PA_BLIND_NORM        (0xC0000000)
#define PA_BLIND_TRANS       (0xC0010000)
#define PA_LOCKBOX           (0xD0001000)  // address user half under blrts

// PPC440 32K Level 1 I-Cache: Virtual Tag = A[0..22], Set = A[23..26]
#define L1I_CACHE_LINES           1024 // 64 ways of 16 sets per way
#define L1I_CACHE_LINE_SIZE         32 // 8 words
#define L1I_CACHE_MASK      0xFFFFFFE0 // mask for 32 byte alignment
#define L1I_CACHE_MAXFLUSH         512 // above this num lines, invalidate the whole thing
#define L1I_CACHE_BYTES    (L1I_CACHE_LINES * L1I_CACHE_LINE_SIZE)

// PPC440 32K Level 1 D-Cache: Physical Tag = A[0..22], Set = A[23..26]
#define L1D_CACHE_LINES           1024 // 64 ways of 16 sets per way
#define L1D_CACHE_LINE_SIZE         32 // 8 words
#define L1D_CACHE_MASK      0xFFFFFFE0 // mask for 32 byte alignment
#define L1D_CACHE_MAXFLUSH         512 // above this num lines, use fast flush-assist
#define L1D_CACHE_BYTES    (L1D_CACHE_LINES * L1D_CACHE_LINE_SIZE)
#define L1D_CACHE_ASSOC             64 // L1-D is 64-way set-associative
#define L1D_CACHE_WAYS              64 // L1-D has 64 ways in each set
#define L1D_CACHE_SETS              16 // L1-D has 16 sets

#define L1I_CACHE_ALIGN              5 // zero bits for  32 byte alignment
#define L1D_CACHE_ALIGN              5 // zero bits for  32 byte alignment
#define L2_CACHE_ALIGN               7 // zero bits for 128 byte alignment
#define L3_CACHE_ALIGN               7 // zero bits for 128 byte alignment


#define _PROLOG(fcn) \
         .section ".text","ax" ; \
         .align L1I_CACHE_ALIGN ; \
         .type fcn,%function ; \
         .global fcn

#define _EPILOG(fcn) \
         .size fcn,.-fcn ; \
         .previous


// Load an unsigned constant into a register.  This macro is provided so
//  you can change the const in some header file without having to change
//  the code needed to use that constant.
// Handles 16bit (upper or lower half) or 32bit constants.
// Note: Use "li" instr for known constant signed values < 16bits. In that
//  case, the assembler will generate the correct single instruction code.
//#define IMM32C(R,V) \
//.if (((V) & 0xFFFF8000) == 0 || ((V) & 0xFFFF8000) == 0xFFFF8000) ; /* 16 (or less) bit consts (addi sign extends) */ \
//      addi  R,%r0,(V) ; \
//.else ; \
//      addis R,%r0,V@h ;  /* get upper bits, zeroing lower bits */ \
//.if ((V) & 0x0000FFFF) ; \
//      ori   R,R,V@l ;    /* get any remaining low bits */ \
//.endif ; \
//.endif
#define IMM32C(R,V) \
.if (((V) & 0xFFFF8000) == 0 || ((V) & 0xFFFF8000) == 0xFFFF8000) ; /* 16 (or less) bit consts (addi sign extends) */ \
      addi  R,%r0,(V) ; \
.else ; \
      addis R,%r0,V >> 16 ;  /* get upper bits, zeroing lower bits */ \
.if ((V) & 0x0000FFFF) ; \
      ori   R,R,V & 0xffff ;    /* get any remaining low bits */ \
.endif ; \
.endif


#endif // __ASSEMBLY__

#endif // Add nothing below this line.
