#include <BonB/bl_light_context_swap.h>

//
// Useful Assembler Support Macros
//

// Load an address or symbol into a register
//#define IMM32(R,V) \
//         addis R,%r0,V@h; \
//         ori   R,R,V@l;

#define IMM32(R,V) \
         addis R,%r0,V >> 16; \
         ori   R,R,V & 0xffff;

// Load an unsigned constant into a register.  This macro is provided so
//  you can change the const in some header file without having to change
//  the code needed to use that constant.
// Handles 16bit (upper or lower half) or 32bit constants.
// Note: Use "li" instr for known constant signed values < 16bits. In that
//  case, the assembler will generate the correct single instruction code.
#define IMM32C(R,V) \
.if (((V) & 0xFFFF8000) == 0 || ((V) & 0xFFFF8000) == 0xFFFF8000) ; /* 16 (or less) bit consts (addi sign extends) */ \
      addi  R,%r0,(V) ; \
.else ; \
      addis R,%r0,V@h ;  /* get upper bits, zeroing lower bits */ \
.if ((V) & 0x0000FFFF) ; \
      ori   R,R,V@l ;    /* get any remaining low bits */ \
.endif ; \
.endif

// Set an Interrupt Vector Register from the Low-Half of a Symbol
#define SET_IVOR( R, I, V )\
        li  R,V@l; \
        mtspr I,R;

// Stuff a 32-bit Address or Symbol into a Special Purposed Register
#define SET_SPR( R, S, V ) \
         IMM32( R, V ); \
         mtspr  S,R;

// Stuff a 32-bit Constant into a Special Purposed Register
#define SET_SPRC( R, S, V ) \
         IMM32C( R, V ); \
         mtspr  S,R;


#define _PROLOG_TEXT(fcn) \
         .section ".text","ax" ; \
         .type fcn,%function ; \
         .global fcn;


#define _EPILOG(fcn) \
         .size fcn,.-fcn ; \
         .previous


//------------------------------------------------------------------------------
// _BG_CoRoutine_Swap: Context Swap for CoRoutines
//
//   - save all ABI-defined non-volatile integer state in Co_From
//   - restore non-volatile state and return to Co_To.
//
//  Syntax:  void _BG_CoRoutine_Swap( _BG_CoRoutine_t *Co_From,   // R3
//                                    _BG_CoRoutine_t *Co_To    ) // R4
//
//  Taken:     See syntax above.
//  Returned:  resumes Co_To
//  Destroyed: most non-volatile reg state
//
        _PROLOG_TEXT( _BG_CoRoutine_Swap )
_BG_CoRoutine_Swap:
        stw    %r1,_BG_COR_OFS_GPR1(%r3)
        mflr   %r5
        mfcr   %r6
        mfctr  %r7
        mfxer  %r8
        stw    %r5,_BG_COR_OFS_LR(%r3)
        stw    %r6,_BG_COR_OFS_CR(%r3)
        stw    %r7,_BG_COR_OFS_CTR(%r3)
        stw    %r8,_BG_COR_OFS_XER(%r3)
        //
        stw    %r10,_BG_COR_OFS_GPR10(%r3)
        stw    %r11,_BG_COR_OFS_GPR11(%r3)
        stw    %r12,_BG_COR_OFS_GPR12(%r3)
        stw    %r13,_BG_COR_OFS_GPR13(%r3)
        stw    %r14,_BG_COR_OFS_GPR14(%r3)
        stw    %r15,_BG_COR_OFS_GPR15(%r3)
        stw    %r16,_BG_COR_OFS_GPR16(%r3)
        stw    %r17,_BG_COR_OFS_GPR17(%r3)
        stw    %r18,_BG_COR_OFS_GPR18(%r3)
        stw    %r19,_BG_COR_OFS_GPR19(%r3)
        stw    %r20,_BG_COR_OFS_GPR20(%r3)
        stw    %r21,_BG_COR_OFS_GPR21(%r3)
        stw    %r22,_BG_COR_OFS_GPR22(%r3)
        stw    %r23,_BG_COR_OFS_GPR23(%r3)
        stw    %r24,_BG_COR_OFS_GPR24(%r3)
        stw    %r25,_BG_COR_OFS_GPR25(%r3)
        stw    %r26,_BG_COR_OFS_GPR26(%r3)
        stw    %r27,_BG_COR_OFS_GPR27(%r3)
        stw    %r28,_BG_COR_OFS_GPR28(%r3)
        stw    %r29,_BG_COR_OFS_GPR29(%r3)
        stw    %r30,_BG_COR_OFS_GPR30(%r3)
        stw    %r31,_BG_COR_OFS_GPR31(%r3)
        //
        // la       %r6,_BG_COR_OFS_FPR0(%r3)
        la       %r6,_BG_COR_OFS_FPR14(%r3)
        li       %r0,0
        li       %r7,16
        //
        //stfpdx    %f0,%r0,%r6
        //stfpdux   %f1,%r6,%r7
        //stfpdux   %f2,%r6,%r7
        //stfpdux   %f3,%r6,%r7
        //stfpdux   %f4,%r6,%r7
        //stfpdux   %f5,%r6,%r7
        //stfpdux   %f6,%r6,%r7
        //stfpdux   %f7,%r6,%r7
        //stfpdux   %f8,%r6,%r7
        //stfpdux   %f9,%r6,%r7
        //stfpdux  %f10,%r6,%r7
        //stfpdux  %f11,%r6,%r7
        //stfpdux  %f12,%r6,%r7
        //stfpdux  %f13,%r6,%r7
        //stfpdux  %f14,%r6,%r7
        stfpdx   %f14,%r6,%r7
        stfpdux  %f15,%r6,%r7
        stfpdux  %f16,%r6,%r7
        stfpdux  %f17,%r6,%r7
        stfpdux  %f18,%r6,%r7
        stfpdux  %f19,%r6,%r7
        stfpdux  %f20,%r6,%r7
        stfpdux  %f21,%r6,%r7
        stfpdux  %f22,%r6,%r7
        stfpdux  %f23,%r6,%r7
        stfpdux  %f24,%r6,%r7
        stfpdux  %f25,%r6,%r7
        stfpdux  %f26,%r6,%r7
        stfpdux  %f27,%r6,%r7
        stfpdux  %f28,%r6,%r7
        stfpdux  %f29,%r6,%r7
        stfpdux  %f30,%r6,%r7
        stfpdux  %f31,%r6,%r7
        //
        // Begin Restore
        //
        // la       %r6,_BG_COR_OFS_FPR0(%r4)
        la       %r6,_BG_COR_OFS_FPR14(%r4)
        //
        // lfpdx     %f0,%r0,%r6
        // lfpdux    %f1,%r6,%r7
        // lfpdux    %f2,%r6,%r7
        // lfpdux    %f3,%r6,%r7
        // lfpdux    %f4,%r6,%r7
        // lfpdux    %f5,%r6,%r7
        // lfpdux    %f6,%r6,%r7
        // lfpdux    %f7,%r6,%r7
        // lfpdux    %f8,%r6,%r7
        // lfpdux    %f9,%r6,%r7
        // lfpdux   %f10,%r6,%r7
        // lfpdux   %f11,%r6,%r7
        // lfpdux   %f12,%r6,%r7
        // lfpdux   %f13,%r6,%r7
        // lfpdux   %f14,%r6,%r7
        lfpdx    %f14,%r6,%r7
        lfpdux   %f15,%r6,%r7
        lfpdux   %f16,%r6,%r7
        lfpdux   %f17,%r6,%r7
        lfpdux   %f18,%r6,%r7
        lfpdux   %f19,%r6,%r7
        lfpdux   %f20,%r6,%r7
        lfpdux   %f21,%r6,%r7
        lfpdux   %f22,%r6,%r7
        lfpdux   %f23,%r6,%r7
        lfpdux   %f24,%r6,%r7
        lfpdux   %f25,%r6,%r7
        lfpdux   %f26,%r6,%r7
        lfpdux   %f27,%r6,%r7
        lfpdux   %f28,%r6,%r7
        lfpdux   %f29,%r6,%r7
        lfpdux   %f30,%r6,%r7
        lfpdux   %f31,%r6,%r7
        //
        lwz    %r5,_BG_COR_OFS_LR(%r4)
        lwz    %r6,_BG_COR_OFS_CR(%r4)
        lwz    %r7,_BG_COR_OFS_CTR(%r4)
        lwz    %r8,_BG_COR_OFS_XER(%r4)
        mtlr   %r5
        mtcr   %r6
        mtctr  %r7
        mtxer  %r8
        //
        lwz    %r1,_BG_COR_OFS_GPR1(%r4)
        lwz    %r10,_BG_COR_OFS_GPR10(%r4)
        lwz    %r11,_BG_COR_OFS_GPR11(%r4)
        lwz    %r12,_BG_COR_OFS_GPR12(%r4)
        lwz    %r13,_BG_COR_OFS_GPR13(%r4)
        lwz    %r14,_BG_COR_OFS_GPR14(%r4)
        lwz    %r15,_BG_COR_OFS_GPR15(%r4)
        lwz    %r16,_BG_COR_OFS_GPR16(%r4)
        lwz    %r17,_BG_COR_OFS_GPR17(%r4)
        lwz    %r18,_BG_COR_OFS_GPR18(%r4)
        lwz    %r19,_BG_COR_OFS_GPR19(%r4)
        lwz    %r20,_BG_COR_OFS_GPR20(%r4)
        lwz    %r21,_BG_COR_OFS_GPR21(%r4)
        lwz    %r22,_BG_COR_OFS_GPR22(%r4)
        lwz    %r23,_BG_COR_OFS_GPR23(%r4)
        lwz    %r24,_BG_COR_OFS_GPR24(%r4)
        lwz    %r25,_BG_COR_OFS_GPR25(%r4)
        lwz    %r26,_BG_COR_OFS_GPR26(%r4)
        lwz    %r27,_BG_COR_OFS_GPR27(%r4)
        lwz    %r28,_BG_COR_OFS_GPR28(%r4)
        lwz    %r29,_BG_COR_OFS_GPR29(%r4)
        lwz    %r30,_BG_COR_OFS_GPR30(%r4)
        lwz    %r31,_BG_COR_OFS_GPR31(%r4)
        //
        lwz    %r3,_BG_COR_OFS_GPR3(%r4)
        //
        blr
        _EPILOG( _BG_CoRoutine_Swap )
