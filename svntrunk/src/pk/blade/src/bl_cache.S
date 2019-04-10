/* -------------------------------------------------------------  */
/* Product(s):                                                    */
/* 5733-BG1                                                       */
/*                                                                */
/* (C)Copyright IBM Corp. 2004, 2004                              */
/* All rights reserved.                                           */
/* US Government Users Restricted Rights -                        */
/* Use, duplication or disclosure restricted                      */
/* by GSA ADP Schedule Contract with IBM Corp.                    */
/*                                                                */
/* Licensed Materials-Property of IBM                             */
/* -------------------------------------------------------------  */
//
//      File: blade/kernel/bl_cache.c NON-PRIVILEGED SUBSET
//
//   Purpose: L1 Instruction and Data Cache Management
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes: NON-PRIVILEGED SUBSET for blade_on_blrts
//
#include <blade_on_blrts.h>

#undef _ALLOW_PRIVILEGED


//------------------------------------------------------------------------------
// Data Cache Flush Assist Area in "Blind Device":
//
//  BlueLight has a physical address range of non-existent memory to assist
//  with flushes of large memory areas (PA_BLIND_*). The 2 TLB entries for
//  this device are pinned, User+Supervisor R+W, one Normal, one Transient.
//
//  On the PPC440, we divide the cache between locked, normal and transient regions,
//  but the kernel does not fully support those yet (as of 8/20/01).  When it does,
//  we will limit the number of dcbz's to the number of lines in each region,
//  which is determined at run-time.  The algorithm is to dcbz once for each
//  cache line in the region, then perform a dccci to eliminate memory bus
//  traffic.  However, that would also invalidate locked cache lines (which may
//  also cause a cache-locking exception), so if we support locking, we should
//  individually invalidate the lines just zeroed.
//
//------------------------------------------------------------------------------

#ifdef _ALLOW_PRIVILEGED
//------------------------------------------------------------------------------
// icache_flush_range: Flush both the I and D Caches between start and stop vaddrs
//
//  Note: Does nothing if start==stop.
//        Flushes entire I&D caches if stop < start, or lines to flush > cache lines
//        Normally used when installing breakpoints, so does both caches.
//
//  Syntax:  void icache_flush_range( void *start, void *stop );
//
//  Taken:     R3 = start vaddr, R4 = stop vaddr
//  Returned:  nothing
//  Destroyed: r3, r4, r5, r6, r7, cr0, count, and i&d cache states
//
        _PROLOG( icache_flush_range )
icache_flush_range:
        // sync the D cache to memory
        li     %r5,L1D_CACHE_LINE_SIZE-1      // Truncate R3 to cache line start
        andc   %r3,%r3,%r5                    // R3 &= ~L1D_Cache_Mask
        subf   %r4,%r3,%r4                    // R4 = stop - start
        add    %r4,%r4,%r5                    // R4 += L1D_Cache_Mask (ie round up bytes)
#if L1D_CACHE_LINE_SIZE != L1I_CACHE_LINE_SIZE
        mr     %r6,%r4                        // Save bytes for i-cache flush later
#endif
        srwi.  %r4,%r4,L1D_CACHE_ALIGN        // R4 >>= L1D_Cache_Align (ie get num lines)
        beqlr                                 // If (R4 == 0) return;
        cmpwi  %r4,L1D_CACHE_MAXFLUSH         // Are lines to flush > max flush lines?
        blt    1f                             //  no, flush this range
        mflr   %r7                            //  yes, save LR
        bl     dcache_flush_all               //   flush entire d-cache
        mtlr   %r7                            //   restore LR
        iccci  %r0,%r0                        //   flush entire i-cache
        isync                                 //   wait for invalidate to complete
        blr                                   //   and return to caller
1:      mtctr  %r4                            // Get lines into count register
        mr     %r5,%r3                        // Save start address for i-cache flush later
2:      dcbf   %r0,%r3                        // Flush and invalidate the d-cache line
        addi   %r3,%r3,L1D_CACHE_LINE_SIZE    // Advance to next d-cache line
        bdnz   2b                             // Loop for number of lines
        sync                                  // Wait for memory updates to complete
        // invalidate the i-cache range
#if L1D_CACHE_LINE_SIZE != L1I_CACHE_LINE_SIZE
        mr     %r4,%r6                        // Restore number of bytes
        srwi   %r4,%r4,L1I_CACHE_ALIGN        // R4 >>= L1I_Cache_Align (ie get num lines)
#endif
        cmpwi  %r4,L1I_CACHE_MAXFLUSH         // Are lines to flush > max flush lines?
        blt    3f                             //  no, go flush just these lines
        iccci  %r0,%r0                        //  yes, invalidate entire i-cache
        isync                                 //   wait for updates to complete
        blr                                   //   and return to caller.
3:      mtctr  %r4                            // Reload number of lines to count register
4:      icbi   %r0,%r5                        // Invalidate the i-cache block
        addi   %r5,%r5,L1I_CACHE_LINE_SIZE    // Advance to next i-cache line
        bdnz   4b                             // Loop for number of lines
        isync                                 // Wait until invalidates complete
        blr                                   // Return to caller
        _EPILOG( icache_flush_range )
#endif // _ALLOW_PRIVILEGED

//------------------------------------------------------------------------------
// dcache_flush_range: Flush the Data Cache between start and stop vaddrs
//
//  Note:  On BlueLight, this uses the special Cache-flush-assist memory area
//         when the range exceeds the size of the data cache.
//
//  Syntax:  void dcache_flush_range( void *start, void *stop );
//
//  Taken:     R3 = start vaddr, R4 = stop vaddr
//  Returned:  nothing
//  Destroyed: r3, r4, r5, r6, and d-cache state
//
        _PROLOG( dcache_flush_range )
dcache_flush_range:
        li     %r5,L1D_CACHE_LINE_SIZE-1      // Truncate R3 to cache line start
        andc   %r3,%r3,%r5                    // R3 &= ~L1D_Cache_Mask
        subf   %r4,%r3,%r4                    // R4 = stop - start
        add    %r4,%r4,%r5                    // R4 += L1D_Cache_Mask (ie round up bytes)
        srwi.  %r4,%r4,L1D_CACHE_ALIGN        // R4 >>= L1D_Cache_Align (ie get num lines)
        beqlr                                 // If (R4 == 0) return;
        cmpwi  %r4,L1D_CACHE_MAXFLUSH         // Are lines to flush > max flush lines?
        bgt    dcache_flush_all               //  yes, go wipe both caches
        mtctr  %r4                            //  no, get lines into count register
1:      dcbf   %r0,%r3                        // Flush and invalidate the d-cache line
        addi   %r3,%r3,L1D_CACHE_LINE_SIZE    // Advance to next d-cache line
        bdnz   1b                             // Loop for number of lines
        sync                                  // Wait for memory updates to complete
        blr
        _EPILOG( dcache_flush_range )

//------------------------------------------------------------------------------
// dcache_flush_bytes: Flush the Data Cache between start and stop vaddrs
//
//  Note:  On BlueLight, this may use the special Cache-flush-assist memory area
//         when the range exceeds the size of the data cache.
//
//  Syntax:  void dcache_flush_bytes( void *start, Bit32 bytes );
//
//  Taken:     R3 = start vaddr, R4 = bytes
//  Returned:  nothing
//  Destroyed: r3, r4, r5, count, and d-cache state
//
        _PROLOG( dcache_flush_bytes )
dcache_flush_bytes:
        li     %r5,L1D_CACHE_LINE_SIZE-1      // Truncate R3 to cache line start
        andc   %r3,%r3,%r5                    // R3 &= ~L1D_Cache_Mask
        add    %r4,%r4,%r5                    // R4 += L1D_Cache_Mask (ie round up bytes)
        srwi.  %r4,%r4,L1D_CACHE_ALIGN        // R4 >>= L1D_Cache_Align (ie get num lines)
        beqlr                                 // If (R4 == 0) return;
        cmpwi  %r4,L1D_CACHE_MAXFLUSH         // Are lines to flush > max flush lines?
        bgt    dcache_flush_all               //  yes, go fast-flush entire d-cache
        mtctr  %r4                            //  no, get lines into count register
1:      dcbf   %r0,%r3                        // Flush and invalidate the d-cache line
        addi   %r3,%r3,L1D_CACHE_LINE_SIZE    // Advance to next d-cache line
        bdnz   1b                             // Loop for number of lines
        sync                                  // Wait for memory updates to complete
        blr                                   // Return to caller
        _EPILOG( dcache_flush_bytes )

//------------------------------------------------------------------------------
// dcache_store_range: Flush the Data Cache between start and stop vaddrs
//
//  Syntax:  void dcache_store_range( void *start, void *stop );
//
//  Taken:     R3 = start vaddr, R4 = stop vaddr
//  Returned:  nothing
//  Destroyed: r3, r4, r5, r6, and d-cache state
//
        _PROLOG( dcache_store_range )
dcache_store_range:
        li     %r5,L1D_CACHE_LINE_SIZE-1      // Truncate R3 to cache line start
        andc   %r3,%r3,%r5                    // R3 &= ~L1D_Cache_Mask
        subf   %r4,%r3,%r4                    // R4 = stop - start
        add    %r4,%r4,%r5                    // R4 += L1D_Cache_Mask (ie round up bytes)
        srwi.  %r4,%r4,L1D_CACHE_ALIGN        // R4 >>= L1D_Cache_Align (ie get num lines)
        beqlr                                 // If (R4 == 0) return;
        mtctr  %r4                            // Get lines into count register
1:      dcbst  %r0,%r3                        // Store the d-cache line (leaves copy in L1D)
        addi   %r3,%r3,L1D_CACHE_LINE_SIZE    // Advance to next d-cache line
        bdnz   1b                             // Loop for number of lines
        sync                                  // Wait for memory updates to complete
        blr
        _EPILOG( dcache_store_range )

#ifdef _ALLOW_PRIVILEGED
//------------------------------------------------------------------------------
// dcache_invalidate_range: Invalidate the Data Cache between start and stop vaddrs
//
//  Syntax:  void dcache_invalidate_range( void *start, void *stop );
//
//  Taken:     R3 = start vaddr, R4 = stop vaddr
//  Returned:  nothing
//  Destroyed: r3, r4, r5, r6, and d-cache state
//
        _PROLOG( dcache_invalidate_range )
dcache_invalidate_range:
        li     %r5,L1D_CACHE_LINE_SIZE-1      // Truncate R3 to cache line start
        andc   %r3,%r3,%r5                    // R3 &= ~L1D_Cache_Mask
        subf   %r4,%r3,%r4                    // R4 = stop - start
        add    %r4,%r4,%r5                    // R4 += L1D_Cache_Mask (ie round up bytes)
        srwi.  %r4,%r4,L1D_CACHE_ALIGN        // R4 >>= L1D_Cache_Align (ie get num lines)
        beqlr                                 // If (R4 == 0) return;
        mtctr  %r4                            // Get lines into count register
1:      dcbi   %r0,%r3                        // Invalidate the d-cache line
        addi   %r3,%r3,L1D_CACHE_LINE_SIZE    // Advance to next d-cache line
        bdnz   1b                             // Loop for number of lines
        sync                                  // Wait for memory updates to complete
        blr
        _EPILOG( dcache_invalidate_range )
#endif // _ALLOW_PRIVILEGED

//------------------------------------------------------------------------------
// dcache_store_bytes: Flush the Data Cache between start and stop vaddrs
//
//  Syntax:  void dcache_store_bytes( void *start, Bit32 bytes );
//
//  Taken:     R3 = start vaddr, R4 = bytes
//  Returned:  nothing
//  Destroyed: r3, r4, r5, count, and d-cache state
//
        _PROLOG( dcache_store_bytes )
dcache_store_bytes:
        li     %r5,L1D_CACHE_LINE_SIZE-1      // Truncate R3 to cache line start
        andc   %r3,%r3,%r5                    // R3 &= ~L1D_Cache_Mask
        add    %r4,%r4,%r5                    // R4 += L1D_Cache_Mask (ie round up bytes)
        srwi.  %r4,%r4,L1D_CACHE_ALIGN        // R4 >>= L1D_Cache_Align (ie get num lines)
        beqlr                                 // If (R4 == 0) return;
        mtctr  %r4                            // Get lines into count register
1:      dcbst  %r0,%r3                        // Flush and invalidate the d-cache line
        addi   %r3,%r3,L1D_CACHE_LINE_SIZE    // Advance to next d-cache line
        bdnz   1b                             // Loop for number of lines
        sync                                  // Wait for memory updates to complete
        blr                                   // Return to caller
        _EPILOG( dcache_store_bytes )

#ifdef _ALLOW_PRIVILEGED
//------------------------------------------------------------------------------
// dcache_invalidate_bytes: Invalidate the Data Cache between start and stop vaddrs
//
//  Note:  On BlueLight, this may use the special Cache-flush-assist memory area
//         when the range exceeds the size of the data cache.
//
//  Syntax:  void dcache_invalidate_bytes( void *start, Bit32 bytes );
//
//  Taken:     R3 = start vaddr, R4 = bytes
//  Returned:  nothing
//  Destroyed: r3, r4, r5, count, and d-cache state
//
        _PROLOG( dcache_invalidate_bytes )
dcache_invalidate_bytes:
        li     %r5,L1D_CACHE_LINE_SIZE-1      // Truncate R3 to cache line start
        andc   %r3,%r3,%r5                    // R3 &= ~L1D_Cache_Mask
        add    %r4,%r4,%r5                    // R4 += L1D_Cache_Mask (ie round up bytes)
        srwi.  %r4,%r4,L1D_CACHE_ALIGN        // R4 >>= L1D_Cache_Align (ie get num lines)
        beqlr                                 // If (R4 == 0) return;
        cmpwi  %r4,L1D_CACHE_MAXFLUSH         // Are lines to flush > max flush lines?
        bgt    dcache_flush_all               //  yes, go fast-flush entire d-cache
        mtctr  %r4                            //  no, get lines into count register
1:      dcbi   %r0,%r3                        // Invalidate the d-cache line
        addi   %r3,%r3,L1D_CACHE_LINE_SIZE    // Advance to next d-cache line
        bdnz   1b                             // Loop for number of lines
        sync                                  // Wait for memory updates to complete
        blr                                   // Return to caller
        _EPILOG( dcache_invalidate_bytes )
#endif // _ALLOW_PRIVILEGED

//------------------------------------------------------------------------------
// dcache_flush_all: Flush the entire Data Cache
//
//  Note:  On BlueLight, we use the Cache-flush-assist memory area.
//         ON PPC-440 WE MUST SEPARATELY FLUSH NORMAL AND TRANSIENT AREAS.
//         THIS DOES NOT SUPPORT LOCKED LINES IN THE CACHE ON A PPC-440!
//
//  Syntax:  void dcache_flush_all( void );
//
//  Taken:     R7 has caller's link reg when called from dcache_flush_range
//             so we must not destroy it.
//  Returned:  nothing
//  Destroyed: r3,r4.r5,r6,r8,r9,count, and all d-cache state
//
        _PROLOG( dcache_flush_all )
dcache_flush_all:
        li      %r0,0
        //
        //@MG: We need to load number of "normal" lines allowed in L1-D.
        //  Right now, that's all of them, but this will become a variable.
        //
        IMM32C( %r4, L1D_CACHE_LINES/4)       // Get number of cache lines into R4
        IMM32C( %r3, PA_BLIND_NORM )          // Get addr of Blind Normal flush area into R3
        //IMM32C( %r5, PA_BLIND_TRANS )         //   and Transient Area into R5
        li      %r6,L1D_CACHE_LINE_SIZE
        li      %r8,L1D_CACHE_LINE_SIZE*2
        li      %r9,L1D_CACHE_LINE_SIZE*3
        mtctr   %r4                           // Get number of cache lines into CTR
1:      dcbz    %r0,%r3                       // Zero 4 cache lines, evicting dirty lines
        dcbz    %r6,%r3
        dcbz    %r8,%r3
        dcbz    %r9,%r3
#ifdef __ALLOW_PRIVILEGED
        msync
        dcbi    %r0,%r3                       // Invalidate the lines allocated above
        dcbi    %r6,%r3
        dcbi    %r8,%r3
        dcbi    %r9,%r3
#endif // __ALLOW_PRIVILEGED
        addi    %r3,%r3,L1D_CACHE_LINE_SIZE*4 // Advance to next group of cache lines
        bdnz    1b                            // Loop over number of cache lines
        msync                                 // Allow memory traffic to complete
        blr                                   // Return to caller
        _EPILOG( dcache_flush_all )

#ifdef _ALLOW_PRIVILEGED
//------------------------------------------------------------------------------
// icache_invalidate_all: Invalidate the entire Instruction Cache
//
//  Syntax:  void icache_invalidate_all( void );
//
//  Taken:     nothing
//  Returned:  nothing
//  Destroyed: i-cache state
//
        _PROLOG( icache_invalidate_all )
icache_invalidate_all:
        iccci   %r0,%r0                // Flash-Invalidate entire I-Cache
        isync                          // Wait for invalidate to complete
        blr
        _EPILOG( icache_invalidate_all )

//------------------------------------------------------------------------------
// icache_invalidate_range: Invalidate the Instruction Cache between start and stop vaddrs
//
//  Syntax:  void icache_flush_range( void *start, void *stop );
//
//  Taken:     R3 = start vaddr, R4 = stop vaddr
//  Returned:  nothing
//  Destroyed: r3, r4, r5, and i-cache state
//
        _PROLOG( icache_invalidate_range )
icache_invalidate_range:
        li     %r5,L1I_CACHE_LINE_SIZE-1      // Truncate R3 to cache line start
        andc   %r3,%r3,%r5                    // R3 &= ~L1D_Cache_Mask
        subf   %r4,%r3,%r4                    // R4 = stop - start
        add    %r4,%r4,%r5                    // R4 += L1I_Cache_Mask (ie round up bytes)
        srwi.  %r4,%r4,L1I_CACHE_ALIGN        // R4 >>= L1I_Cache_Align (ie get num lines)
        beqlr                                 // If (R4 == 0) return;
        mtctr  %r4                            // Get lines into count register
1:      icbi   %r0,%r3                        // Invalidate the i-cache line
        addi   %r3,%r3,L1I_CACHE_LINE_SIZE    // Advance to next i-cache line
        bdnz   1b                             // Loop for number of lines
        isync                                 // Wait for completion
        blr
        _EPILOG( icache_invalidate_range )

//------------------------------------------------------------------------------
// icache_invalidate_bytes: Invalidate the Instruction Cache over start to (start+bytes) vaddrs
//
//  Syntax:  void icache_invalidate_bytes( void *start, Bit32 bytes );
//
//  Taken:     R3 = start vaddr, R4 = bytes
//  Returned:  nothing
//  Destroyed: r3, r4, r5, count, and d-cache state
//
        _PROLOG( icache_invalidate_bytes )
icache_invalidate_bytes:
        li     %r5,L1I_CACHE_LINE_SIZE-1      // Truncate R3 to cache line start
        andc   %r3,%r3,%r5                    // R3 &= ~L1I_Cache_Mask
        add    %r4,%r4,%r5                    // R4 += L1I_Cache_Mask (ie round up bytes)
        srwi.  %r4,%r4,L1I_CACHE_ALIGN        // R4 >>= L1I_Cache_Align (ie get num lines)
        beqlr                                 // If (R4 == 0) return;
        mtctr  %r4                            // Get lines into count register
1:      icbi   %r0,%r3                        // Invalidate the i-cache line
        addi   %r3,%r3,L1D_CACHE_LINE_SIZE    // Advance to next i-cache line
        bdnz   1b                             // Loop for number of lines
        isync                                 // Wait for completion
        blr                                   // Return to caller
        _EPILOG( icache_invalidate_bytes )
#endif // _ALLOW_PRIVILEGED

//------------------------------------------------------------------------------
// icache_touch_range: Touch the Instruction Cache between start and stop vaddrs
//
//  Syntax:  void icache_touch_range( void *start, void *stop );
//
//  Taken:     R3 = start vaddr, R4 = stop vaddr
//  Returned:  nothing
//  Destroyed: r3, r4, r5, and i-cache state
//
        _PROLOG( icache_touch_range )
icache_touch_range:
        li     %r5,L1I_CACHE_LINE_SIZE-1      // Truncate R3 to cache line start
        andc   %r3,%r3,%r5                    // R3 &= ~L1D_Cache_Mask
        subf   %r4,%r3,%r4                    // R4 = stop - start
        add    %r4,%r4,%r5                    // R4 += L1I_Cache_Mask (ie round up bytes)
        srwi.  %r4,%r4,L1I_CACHE_ALIGN        // R4 >>= L1I_Cache_Align (ie get num lines)
        beqlr                                 // If (R4 == 0) return;
        mtctr  %r4                            // Get lines into count register
1:      icbt   %r0,%r3                        // Touch the i-cache line
        addi   %r3,%r3,L1I_CACHE_LINE_SIZE    // Advance to next i-cache line
        bdnz   1b                             // Loop for number of lines
        isync                                 // Wait for completion
        blr
        _EPILOG( icache_touch_range )

//------------------------------------------------------------------------------
// icache_touch_bytes: Touch the Instruction Cache between start and (start+bytes) vaddrs
//
//  Syntax:  void icache_touch_bytes( void *start, Bit32 bytes );
//
//  Taken:     R3 = start vaddr, R4 = stop vaddr
//  Returned:  nothing
//  Destroyed: r3, r4, r5, and i-cache state
//
        _PROLOG( icache_touch_bytes )
icache_touch_bytes:
        li     %r5,L1I_CACHE_LINE_SIZE-1      // Truncate R3 to cache line start
        andc   %r3,%r3,%r5                    // R3 &= ~L1I_Cache_Mask
        add    %r4,%r4,%r5                    // R4 += L1I_Cache_Mask (ie round up bytes)
        srwi.  %r4,%r4,L1I_CACHE_ALIGN        // R4 >>= L1I_Cache_Align (ie get num lines)
        beqlr                                 // If (R4 == 0) return;
        mtctr  %r4                            // Get lines into count register
1:      icbt   %r0,%r3                        // Touch the i-cache line
        addi   %r3,%r3,L1D_CACHE_LINE_SIZE    // Advance to next i-cache line
        bdnz   1b                             // Loop for number of lines
        isync                                 // Wait for completion
        blr                                   // Return to caller
        _EPILOG( icache_touch_bytes )

//------------------------------------------------------------------------------
