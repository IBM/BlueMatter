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
//      File: blade/spi/spi_ts_hints.c
//
//   Purpose: Implement Blade's Torus Packet Layer SPI.
//
//   Program: BlueGene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Mark E. Giampapa (giampapa@us.ibm.com)
//
//     Notes: See include/spi/BGL_TorusPktSPI.h
//
//   History: 05/15/2002: MEG - Created.
//
//
#include <blade_on_blrts.h>

__BEGIN_DECLS

#if defined(__GNUC__)
//
// Inline Status Access Routines internal to SPI
//
static inline void _ts_GetStatus0( void *stat )
{
   eieio();
   QuadMove( TS0_S0, stat, 0 );
}

extern inline void _ts_GetStatus1( void *stat )
{
   eieio();
   QuadMove( TS0_S1, stat, 0 );
}

extern inline void _ts_GetStatusBoth( void *s0, void *s1 )
{
   eieio();
   QuadLoad( TS0_S0, 0 );
   QuadLoad( TS0_S1, 1 );
   QuadStore( s0, 0 );
   QuadStore( s1, 1 );
}
#else
// xlC builtins to force parallel loads and stores
extern "builtin" double _Complex __lfpd(volatile double *) ;
extern "builtin" void __stfpd(volatile double *, double _Complex) ;
extern "builtin" void __sync(void) ;
inline static void _ts_GetStatusBoth(_BGL_TorusFifoStatus *stat0, _BGL_TorusFifoStatus *stat1 )
{
	__sync() ; 
	double _Complex status0 = __lfpd((volatile double *)TS0_S0) ;
	double _Complex status1 = __lfpd((volatile double *)TS0_S1) ;
	*(double _Complex *) stat0 = status0 ;
	*(double _Complex *) stat1 = status1 ;
}

#endif // __GNUC__

//
// Query if the Torus is fully connected, or if it is a Mesh.
//
//  Returns True   for any Dimension that is a Mesh,
//          False if the Dimension is a Torus.
//
void BGLTorusIsMesh( int *is_mesh_X, int *is_mesh_Y, int *is_mesh_Z )
{
   *is_mesh_X = _Blade_Config_Area.is_mesh_X;
   *is_mesh_Y = _Blade_Config_Area.is_mesh_Y;
   *is_mesh_Z = _Blade_Config_Area.is_mesh_Z;
}

/*---------------------------------------------------------------------------*
 *     Utility Functions for Torus Header Manipulation                       *
 *---------------------------------------------------------------------------*/

//
// Set hints for choosing an injection fifo
//
//  This is very similar to a Hint Bit calculation.
//
//@MG: DOES NOT DO SPLIT MODE YET!
//
void _ts_MakeSendHints( _BGL_TorusPktHdr *hdr )
{
   int fifos = 0;              // result
   int hints = hdr->hwh0.Hint; // get current hint bits

   // if the packet is HP, the choice is obvious.
   if ( hdr->hwh0.VC == _BGL_TORUS_PKT_VC_BP )
      {
      hdr->hwh1.Resvd0 = (_BGL_TORUS_FIFO_G0IH | _BGL_TORUS_FIFO_G1IH);
      return;
      }

   // if no hints were set, do the calculation.
   if ( !hints )
      hints = BGLTorusCalcHints( hdr->hwh0.X, hdr->hwh1.Y, hdr->hwh1.Z );

   // if *still* no hints, than all normal fifos are good (must be sending to self)
   if ( !hints )
      {
      fifos |= (_BGL_TORUS_FIFO_G0I0 | _BGL_TORUS_FIFO_G0I1 |_BGL_TORUS_FIFO_G0I2 |
                _BGL_TORUS_FIFO_G1I0 | _BGL_TORUS_FIFO_G1I1 |_BGL_TORUS_FIFO_G1I2   );

      }
   else
      {
      // based on direction, indicate suitable fifos
      if ( hints & _BGL_TORUS_PKT_HINT_XP ) fifos |= _BGL_TORUS_FIFO_G0I0;
      if ( hints & _BGL_TORUS_PKT_HINT_XM ) fifos |= _BGL_TORUS_FIFO_G0I1;
      if ( hints & _BGL_TORUS_PKT_HINT_YP ) fifos |= _BGL_TORUS_FIFO_G0I2;
      if ( hints & _BGL_TORUS_PKT_HINT_YM ) fifos |= _BGL_TORUS_FIFO_G1I0;
      if ( hints & _BGL_TORUS_PKT_HINT_ZP ) fifos |= _BGL_TORUS_FIFO_G1I1;
      if ( hints & _BGL_TORUS_PKT_HINT_ZM ) fifos |= _BGL_TORUS_FIFO_G1I2;
      }

   // return with bitmask of suitable fifos set into the reserved 0 field.
   hdr->hwh1.Resvd0 = fifos;
}

//
// Choose a Normal Priority Injection Fifo based on Header Contents and Chunks
//
_BGL_TorusFifo *_ts_ChooseInjectionFifo( _BGL_TorusPktHdr *hdr )
{
   int fifos  = hdr->hwh1.Resvd0;
   int chunks = hdr->hwh0.Chunks + 1;

   // update our status cache
   _ts_GetStatusBoth( &_ts0_stat0_bounce, &_ts0_stat1_bounce );

   //@MG: needs cntlz()
   if ( fifos & _BGL_TORUS_FIFO_G0IH )
      if ( (chunks + _ts0_stat0_bounce.iH) <= _BGL_TORUS_FIFO_CHUNKS ) return( (_BGL_TorusFifo *)TS0_IH );

   if ( fifos & _BGL_TORUS_FIFO_G1IH )
      if ( (chunks + _ts0_stat1_bounce.iH) <= _BGL_TORUS_FIFO_CHUNKS ) return( (_BGL_TorusFifo *)TS1_IH );

   if ( fifos & _BGL_TORUS_FIFO_G0I0 )
      if ( (chunks + _ts0_stat0_bounce.i0) <= _BGL_TORUS_FIFO_CHUNKS ) return( (_BGL_TorusFifo *)TS0_I0 );

   if ( fifos & _BGL_TORUS_FIFO_G0I1 )
      if ( (chunks + _ts0_stat0_bounce.i1) <= _BGL_TORUS_FIFO_CHUNKS ) return( (_BGL_TorusFifo *)TS0_I1 );

   if ( fifos & _BGL_TORUS_FIFO_G0I2 )
      if ( (chunks + _ts0_stat0_bounce.i2) <= _BGL_TORUS_FIFO_CHUNKS ) return( (_BGL_TorusFifo *)TS0_I2 );

   if ( fifos & _BGL_TORUS_FIFO_G1I0 )
      if ( (chunks + _ts0_stat1_bounce.i0) <= _BGL_TORUS_FIFO_CHUNKS ) return( (_BGL_TorusFifo *)TS1_I0 );

   if ( fifos & _BGL_TORUS_FIFO_G1I1 )
      if ( (chunks + _ts0_stat1_bounce.i1) <= _BGL_TORUS_FIFO_CHUNKS ) return( (_BGL_TorusFifo *)TS1_I1 );

   if ( fifos & _BGL_TORUS_FIFO_G1I2 )
      if ( (chunks + _ts0_stat1_bounce.i2) <= _BGL_TORUS_FIFO_CHUNKS ) return( (_BGL_TorusFifo *)TS1_I2 );

   return( (_BGL_TorusFifo *)0 );
}


/*---------------------------------------------------------------------------*
 *     Torus Header Hint-Bit Calculators                                     *
 *---------------------------------------------------------------------------*/

//
// Get the current Hint-Bit Cut-Off Settings.
//
void BGLTorusGetHintCutoffPlus( int *cutoff_xp, int *cutoff_yp, int *cutoff_zp )
{
   *cutoff_xp = _Blade_Config_Area.hint_cutoff_XP;
   *cutoff_yp = _Blade_Config_Area.hint_cutoff_YP;
   *cutoff_zp = _Blade_Config_Area.hint_cutoff_ZP;
}

void BGLTorusGetHintCutoffMinus( int *cutoff_xm, int *cutoff_ym, int *cutoff_zm )
{
   *cutoff_xm = _Blade_Config_Area.hint_cutoff_XM;
   *cutoff_ym = _Blade_Config_Area.hint_cutoff_YM;
   *cutoff_zm = _Blade_Config_Area.hint_cutoff_ZM;
}

#if 0
//
// Set the Hint-Bit Cut-Off Settings.
//
//  Note: for advanced users only. The SPI installs suitable defaults.
//
void BGLTorusSetHintCutoffPlus( int *cutoff_xp, int *cutoff_yp, int *cutoff_zp )
{
   Bit32 ts_tmp = (_TS_CTRL_HINT_PLUS_X( *cutoff_xp ) |
                   _TS_CTRL_HINT_PLUS_Y( *cutoff_yp ) |
                   _TS_CTRL_HINT_PLUS_Z( *cutoff_zp )  );

   mtdcr( DCR_TS_CTRL_HINT_PLUS, ts_tmp );
}

void BGLTorusSetHintCutoffMinus( int *cutoff_xm, int *cutoff_ym, int *cutoff_zm )
{
   Bit32 ts_tmp = (_TS_CTRL_HINT_MINUS_X( *cutoff_xm ) |
                   _TS_CTRL_HINT_MINUS_Y( *cutoff_ym ) |
                   _TS_CTRL_HINT_MINUS_Z( *cutoff_zm )  );

   mtdcr( DCR_TS_CTRL_HINT_MINUS, ts_tmp );
}
#endif

//
// BGLTorusCalcHints: Return HintBits for Profitable Routes.
//
//  Taken:    Destination node's physical x,y,z coordinates.
//  Returned: Hint bits set for use in a packet header.
//
//  Note: If sending to yourself, you must manually set a hintbit.
//
unsigned int BGLTorusCalcHints( int dest_x,
                                int dest_y,
                                int dest_z  )
{
   int hints = 0;           // return value
   int nx, ny, nz;          // node coords
   int co_xp, co_yp, co_zp; // Hint-Bit Cut-Offs Plus
   int co_xm, co_ym, co_zm; // Hint-Bit Cut-Offs Minus

   // get this node's coordinates and routing cut-offs
   BGLPartitionGetCoords( &nx, &ny, &nz );
   BGLTorusGetHintCutoffPlus(  &co_xp, &co_yp, &co_zp );
   BGLTorusGetHintCutoffMinus( &co_xm, &co_ym, &co_zm );

   // X
   if ( nx < dest_x )
      {
      if ( dest_x <= co_xp )
         hints |= _BGL_TORUS_PKT_HINT_XP;
      else
         hints |= _BGL_TORUS_PKT_HINT_XM;
      }
   else if ( nx > dest_x )
      {
      if ( dest_x >= co_xm )
         hints |= _BGL_TORUS_PKT_HINT_XM;
      else
         hints |= _BGL_TORUS_PKT_HINT_XP;
      }

   // Y
   if ( ny < dest_y )
      {
      if ( dest_y <= co_yp )
         hints |= _BGL_TORUS_PKT_HINT_YP;
      else
         hints |= _BGL_TORUS_PKT_HINT_YM;
      }
   else if ( ny > dest_y )
      {
      if ( dest_y >= co_ym )
         hints |= _BGL_TORUS_PKT_HINT_YM;
      else
         hints |= _BGL_TORUS_PKT_HINT_YP;
      }

   // Z
   if ( nz < dest_z )
      {
      if ( dest_z <= co_zp )
         hints |= _BGL_TORUS_PKT_HINT_ZP;
      else
         hints |= _BGL_TORUS_PKT_HINT_ZM;
      }
   else if ( nz > dest_z )
      {
      if ( dest_z >= co_zm )
         hints |= _BGL_TORUS_PKT_HINT_ZM;
      else
         hints |= _BGL_TORUS_PKT_HINT_ZP;
      }

   return( hints );
}

__END_DECLS
