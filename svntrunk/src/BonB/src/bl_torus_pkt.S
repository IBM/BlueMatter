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
//      File: blade/kernel/bl_torus_pkt.S
//
//   Purpose: Torus Packet Layer Assembler support routines.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes:
//
#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include <BonB/blade_on_blrts.h>

         .file "bl_torus_pkt.S"

#if 0
//------------------------------------------------------------------------------
            _DPROLOG( _ts0_bounce, 8 )
_ts0_bounce:
_ts0_r0:    .space _TS_PKT_MAX
_ts0_r1:    .space _TS_PKT_MAX
_ts0_r2:    .space _TS_PKT_MAX
_ts0_r3:    .space _TS_PKT_MAX
_ts0_r4:    .space _TS_PKT_MAX
_ts0_r5:    .space _TS_PKT_MAX
_ts0_rH:    .space _TS_PKT_MAX
_ts0_i0:    .space _TS_PKT_MAX
_ts0_i1:    .space _TS_PKT_MAX
_ts0_i2:    .space _TS_PKT_MAX
_ts0_iH:    .space _TS_PKT_MAX
            _DEPILOG( _ts0_bounce )
            //
            _DPROLOG( _ts1_bounce, 8 )
_ts1_bounce:
_ts1_r0:    .space _TS_PKT_MAX
_ts1_r1:    .space _TS_PKT_MAX
_ts1_r2:    .space _TS_PKT_MAX
_ts1_r3:    .space _TS_PKT_MAX
_ts1_r4:    .space _TS_PKT_MAX
_ts1_r5:    .space _TS_PKT_MAX
_ts1_rH:    .space _TS_PKT_MAX
_ts1_i0:    .space _TS_PKT_MAX
_ts1_i1:    .space _TS_PKT_MAX
_ts1_i2:    .space _TS_PKT_MAX
_ts1_iH:    .space _TS_PKT_MAX
            _DEPILOG( _ts1_bounce )
            //
//------------------------------------------------------------------------------
// _ts_put_quads_mid: Send a "Middle" Torus Packet via a "Put".
//
//   Middle packets are full (256 byte), and both src and dest addresses
//   are quad aligned.
//
//  Syntax: void _ts_put_quads_mid( Bit128  *fifo,     R3
//                                  _TS_Hdr *hdr,      R4
//                                  Bit128  *src_addr, R5
//                                  );
//
//  Taken:     See syntax above.
//  Returned:  nothing
//  Destroyed: most volatile register state
//
        _PROLOG( _ts_put_quads_mid )
_ts_put_quads_mid:
        li      %r6,16               // 16 bytes per quad
        lfpdx   %f0,%r0,%r4          // F0=Q0 load hdr (certain L1 hit)
        lfpdx   %f1,%r0,%r5          // F1=Q1 load
        lfpdux  %f2,%r5,%r6          // F2=Q2 load
        lfpdux  %f3,%r5,%r6          // F3=Q3 load
        stfpdx  %f0,%r0,%r3          // Q0 store
        lfpdux  %f4,%r5,%r6          // F4=Q4 load
        lfpdux  %f5,%r5,%r6          // F5=Q5 load
        lfpdux  %f6,%r5,%r6          // F6=Q6 load
        stfpdx  %f1,%r0,%r3          // Q1 store
        stfpdx  %f2,%r0,%r3          // Q2 store
        stfpdx  %f3,%r0,%r3          // Q3 store
        lfpdux  %f7,%r5,%r6          // F7=Q7 load
        lfpdux  %f8,%r5,%r6          // F8=Q8 load
        lfpdux  %f9,%r5,%r6          // F9=Q9 load
        stfpdx  %f4,%r0,%r3          // Q4 store
        stfpdx  %f5,%r0,%r3          // Q5 store
        stfpdx  %f6,%r0,%r3          // Q6 store
        lfpdux  %f10,%r5,%r6         // F10=Qa load
        lfpdux  %f11,%r5,%r6         // F11=Qb load
        lfpdux  %f12,%r5,%r6         // F12=Qc load
        stfpdx  %f7,%r0,%r3          // Q7 store
        stfpdx  %f8,%r0,%r3          // Q8 store
        stfpdx  %f9,%r0,%r3          // Q9 store
        lfpdux  %f1,%r5,%r6          // F1=Qd load
        lfpdux  %f2,%r5,%r6          // F2=Qe load
        lfpdux  %f3,%r5,%r6          // F3=Qf load
        stfpdx  %f10,%r0,%r3         // Qa store
        stfpdx  %f11,%r0,%r3         // Qb store
        stfpdx  %f12,%r0,%r3         // Qc store
        stfpdx  %f1,%r0,%r3          // Qd store
        stfpdx  %f2,%r0,%r3          // Qe store
        stfpdx  %f3,%r0,%r3          // Qf store
        blr
        _EPILOG( _ts_put_quads_mid )
#endif
//------------------------------------------------------------------------------
// _ts_quads_mid_receiver: Receiver for a "Middle" Torus Packet.
//
//   Middle packets are full (256 byte), and both src and dest addresses
//   are quad aligned.  We are called from the poll routine with the first
//   2 chunks of the packet in fpu registers.
//
//  Syntax: void _ts_quads_mid_receiver( Bit32 *fifo,         R3
//                                       Bit32  *dest );      R4
//
//  Taken:     See syntax above.
//  Returned:  nothing
//  Destroyed: r4,r6,f1-f12
//
        _PROLOG( _ts_quads_mid_receiver )
_ts_quads_mid_receiver:
        li       %r6,16                 // R6 = sizeof(quad)
        lfpdx    %f4,%r0,%r3            // F4 = Q4
        lfpdx    %f5,%r0,%r3            // F5 = Q5
        lfpdx    %f6,%r0,%r3            // F6 = Q6
        stfpdx   %f1,%r0,%r4            // Q1 store
        stfpdux  %f2,%r4,%r6            // Q2 store
        stfpdux  %f3,%r4,%r6            // Q3 store
        lfpdx    %f7,%r0,%r3            // F7 = Q7
        lfpdx    %f8,%r0,%r3            // F8 = Q8
        lfpdx    %f9,%r0,%r3            // F9 = Q9
        stfpdux  %f4,%r4,%r6            // Q4 store
        stfpdux  %f5,%r4,%r6            // Q5 store
        stfpdux  %f6,%r4,%r6            // Q6 store
        lfpdx    %f10,%r0,%r3           // F7 = Q10
        lfpdx    %f11,%r0,%r3           // F8 = Q11
        lfpdx    %f12,%r0,%r3           // F9 = Q12
        stfpdux  %f7,%r4,%r6            // Q7 store
        stfpdux  %f8,%r4,%r6            // Q8 store
        stfpdux  %f9,%r4,%r6            // Q9 store
        lfpdx    %f1,%r0,%r3            // F1 = Q13
        lfpdx    %f2,%r0,%r3            // F2 = Q14
        lfpdx    %f3,%r0,%r3            // F3 = Q15
        stfpdux  %f10,%r4,%r6           // Q10 store
        stfpdux  %f11,%r4,%r6           // Q11 store
        stfpdux  %f12,%r4,%r6           // Q12 store
        stfpdux  %f1,%r4,%r6            // Q13 store
        stfpdux  %f2,%r4,%r6            // Q14 store
        stfpdux  %f3,%r4,%r6            // Q15 store
        blr
        _EPILOG( _ts_quads_mid_receiver )

#if 0
//------------------------------------------------------------------------------
// _ts0_pkt_recv: Receive a Packet from the Indicated Group 0 Fifo.
//
//   On pkt available, read the first 2 chunks then branch to the active function.
//
//  Syntax: void _ts0_pkt_recv( Bit128 *fifo ); // R3
//
//  Taken:     address of fifo containing packet
//  Returned:  returns thru the Active Message Function
//  Destroyed: r4-r8,ctr,f0-f3
//
        _PROLOG( _ts0_pkt_recv )
_ts0_pkt_recv:
        lfpdx     %f0,%r0,%r3             // F0 = pkt_quad0 (hw & sw hdrs)
        IMM32(    %r6, _ts0_bounce )      // R6 = bounce address
        lfpdx     %f1,%r0,%r3             // F1 = pkt_quad1
        dcbz      %r0,%r6                 // place zeroed bounce in L1
        lfpdx     %f2,%r0,%r3             // F2 = pkt_quad2
        // there's really nothing to do here
        stfpdx    %f0,%r0,%r6             // store headers to our bounce address
        // nothing to do here either
        lwz       %r7,_TS_SWH_FCN(%r6)    // load s/w hdr fcn+extra from L1
        lwz       %r4,_TS_SWH_ARG(%r6)    // load s/w arg from L1
        clrlslwi  %r8,%r7,10,2            // r8 = 0b00000000||fcn||0b00
        extrwi    %r5,%r7,10,0            // r5 = 0[0-21]||extra
        mtctr     %r8                     // move fcn ptr to count register
        lfpdx     %f3,%r0,%r3             // F3 = pkt_quad3
        bctr                              // GOTO FCN WHO WILL RETURN TO OUR CALLER!
        _EPILOG( _ts0_pkt_recv )
        //
//------------------------------------------------------------------------------
// _ts1_pkt_recv: Receive a Packet from the Indicated Group 1 Fifo.
//
//   On pkt available, read the first chunk then branch to the active function.
//
//  Syntax: void _ts1_pkt_recv( Bit128 *fifo ); // R3
//
//  Taken:     address of fifo containing packet
//  Returned:  does not return directly
//  Destroyed: all volatile register state
//
        _PROLOG( _ts1_pkt_recv )
_ts1_pkt_recv:
        lfpdx     %f0,%r0,%r3             // F0 = pkt_quad0 (hw & sw hdrs)
        IMM32(    %r6, _ts1_bounce )      // R6 = bounce address
        lfpdx     %f1,%r0,%r3             // F1 = pkt_quad1
        dcbz      %r0,%r6                 // place zeroed bounce in L1
        lfpdx     %f2,%r0,%r3             // F2 = pkt_quad2
        // there's really nothing to do here
        stfpdx    %f0,%r0,%r6             // store hwh to our bounce address
        // nothing to do here either
        lwz       %r7,_TS_SWH_FCN(%r6)    // load s/w hdr fcn+extra from L1
        lwz       %r4,_TS_SWH_ARG(%r6)    // load s/w arg from L1
        clrlslwi  %r8,%r7,10,2            // r8 = 0b00000000||fcn||0b00
        extrwi    %r5,%r7,10,0            // r5 = 0[0-21]||extra
        mtctr     %r8                     // move fcn ptr to count register
        lfpdx     %f3,%r0,%r3             // F3 = pkt_quad3
        bctr                              // GOTO FCN WHO WILL RETURN TO OUR CALLER!
        _EPILOG( _ts1_pkt_recv )

#endif



