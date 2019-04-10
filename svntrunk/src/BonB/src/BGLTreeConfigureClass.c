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
//      File: blade/spi/BGLTreeConfigureClass.c
//
//   Purpose: Implement Blade's Tree Network System Programming Interface.
//
//   Program: Bluegene/L Advanced Diagnostics Environment (BLADE)
//
//    Author: Matthias A. Blumrich (blumrich@us.ibm.com)
//
//     Notes: This SPI is intended for system software development,
//             and in many cases, simplicity has been eschewed for performance
//             and explicit control over the Tree.
//
//   History: 04/11/2003: MAB - Created.
//
//
#include <BonB/blade_on_blrts.h>
#include <stdlib.h> // for exit

#define TrSPI_TRC( flag, args )
#define TrSPI_TRC_CFGCLASS( flag, args )

// Keep track of automatic class allocation.
Bit32 __BGL_Tree_Class_Allocation = 0;

/*---------------------------------------------------------------------------*
 *     Virtual Tree (Class) Construction Interfaces                          *
 *---------------------------------------------------------------------------*/

// Enable local loopback on a channel.
#undef  FN_NAME
#define FN_NAME "BGLTreeWrapChannel"
void BGLTreeWrapChannel( int channel )
{
   Bit32 dcr;

#ifdef _SPI_CHECK_INFO
   printf( FN_NAME"( %d ) called.\n", channel );
#endif

   dcr = mfdcr( DCR_TR_ARB_RCFG );
   switch( channel ) {
      case 0:
         mtdcr( DCR_TR_ARB_RCFG, dcr | _TR_ARB_RCFG_LB0 );
         break;
      case 1:
         mtdcr( DCR_TR_ARB_RCFG, dcr | _TR_ARB_RCFG_LB1 );
         break;
      case 2:
         mtdcr( DCR_TR_ARB_RCFG, dcr | _TR_ARB_RCFG_LB2 );
         break;
      default:
         TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": channel=%d is out of range 0-2.\n", channel ));
         _BLADE_ABORT( -1 );
   }
}

// Disable local loopback on a channel.
#undef  FN_NAME
#define FN_NAME "BGLTreeUnwrapChannel"
void BGLTreeUnwrapChannel( int channel )
{
   Bit32 dcr;

#ifdef _SPI_CHECK_INFO
   printf( FN_NAME"( %d ) called.\n", channel );
#endif

   dcr = mfdcr( DCR_TR_ARB_RCFG );

   switch( channel ) {
      case 0:
         mtdcr( DCR_TR_ARB_RCFG, dcr & ~_TR_ARB_RCFG_LB0 );
         break;
      case 1:
         mtdcr( DCR_TR_ARB_RCFG, dcr & ~_TR_ARB_RCFG_LB1 );
         break;
      case 2:
         mtdcr( DCR_TR_ARB_RCFG, dcr & ~_TR_ARB_RCFG_LB2 );
         break;
      default:
         TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": channel=%d is out of range 0-2.\n", channel ));
         _BLADE_ABORT( -1 );
   }
}

// Enable header class MSB inversion on a send channel (0, 1, or 2).  Note that inversion occurs
// before the header enters the send fifo!
#undef  FN_NAME
#define FN_NAME "BGLTreeInvertClassMsb"
void BGLTreeInvertClassMsb( int channel )
{
   Bit32 dcr;

#ifdef _SPI_CHECK_INFO
   printf( FN_NAME"( %d ) called.\n", channel );
#endif

   switch( channel ) {
      case 0:
         dcr = mfdcr( DCR_TR_CH0_SCTRL );
         mtdcr( DCR_TR_CH0_SCTRL, dcr |  _TR_SCTRL_INVMSB );
         break;
      case 1:
         dcr = mfdcr( DCR_TR_CH1_SCTRL );
         mtdcr( DCR_TR_CH1_SCTRL, dcr |  _TR_SCTRL_INVMSB );
         break;
      case 2:
         dcr = mfdcr( DCR_TR_CH2_SCTRL );
         mtdcr( DCR_TR_CH2_SCTRL, dcr |  _TR_SCTRL_INVMSB );
         break;
      default:
         TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": channel=%d is out of range 0-2.\n", channel ));
         _BLADE_ABORT( -1 );
   }
}

// Disable header class MSB inversion on a send channel (0, 1, or 2).
#undef  FN_NAME
#define FN_NAME "BGLTreeUnnvertClassMsb"
void BGLTreeUninvertClassMsb( int channel )
{
   Bit32 dcr;

#ifdef _SPI_CHECK_INFO
   printf( FN_NAME"( %d ) called.\n", channel );
#endif

   switch( channel ) {
      case 0:
         dcr = mfdcr( DCR_TR_CH0_SCTRL );
         mtdcr( DCR_TR_CH0_SCTRL, dcr & ~_TR_SCTRL_INVMSB );
         break;
      case 1:
         dcr = mfdcr( DCR_TR_CH1_SCTRL );
         mtdcr( DCR_TR_CH1_SCTRL, dcr & ~_TR_SCTRL_INVMSB );
         break;
      case 2:
         dcr = mfdcr( DCR_TR_CH2_SCTRL );
         mtdcr( DCR_TR_CH2_SCTRL, dcr & ~_TR_SCTRL_INVMSB );
         break;
      default:
         TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": channel=%d is out of range 0-2.\n", channel ));
         _BLADE_ABORT( -1 );
   }
}

// Specify class route using a bit vector of the _BGL_RDR_* definitions.
#undef  FN_NAME
#define FN_NAME "BGLTreeConfigureClass"
int BGLTreeConfigureClass(
        int  vt,              // Virtual tree number (12-15 are reserved for OS)
        int  SrcTgtEnable )   // Bit vector defining sources and targets.
{
   Bit32 dcr;
   Bit32 mask;

#ifdef _SPI_CHECK_INFO
   printf( FN_NAME"( %d, 0x%x ) called.\n", vt, SrcTgtEnable );
#endif

   if ( SrcTgtEnable & ~_BGL_TREE_RDR_ACCEPT )
      {
      TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": invalid source/target specification (0x%.8x).\n", SrcTgtEnable ));
      _BLADE_ABORT( -1 );
      }

   // Note: Routes come in with the bits in the upper (0..15) side.
   // Route Descriptor: 0..15 is Route N, 16-31 is Route N+1
   if ( vt & 0x1 )
      {
      // Route N+1, so keep upper half of DCR, clear lower half,
      mask = 0xffff0000;
      }
   else
      {
      // Route N, so keep lower-half, clear upper half,
      //  and shift spec up to upper-half.
      mask = 0x0000ffff;
      SrcTgtEnable <<= 16;
      }

   switch ( vt >> 1 )
     {
     case 0:  // classes 0, 1
        dcr = mfdcr(DCR_TR_CLASS_RDR0) & mask;
        mtdcr(DCR_TR_CLASS_RDR0, dcr | SrcTgtEnable);
        break;
     case 1:  // classes 2, 3
        dcr = mfdcr(DCR_TR_CLASS_RDR1) & mask;
        mtdcr(DCR_TR_CLASS_RDR1, dcr | SrcTgtEnable);
        break;
     case 2:  // classes 4, 5
        dcr = mfdcr(DCR_TR_CLASS_RDR2) & mask;
        mtdcr(DCR_TR_CLASS_RDR2, dcr | SrcTgtEnable);
        break;
     case 3:  // classes 6, 7
        dcr = mfdcr(DCR_TR_CLASS_RDR3) & mask;
        mtdcr(DCR_TR_CLASS_RDR3, dcr | SrcTgtEnable);
        break;
     case 4:  // classes 8, 9
        dcr = mfdcr(DCR_TR_CLASS_RDR4) & mask;
        mtdcr(DCR_TR_CLASS_RDR4, dcr | SrcTgtEnable);
        break;
     case 5:  // classes 10, 11
        dcr = mfdcr(DCR_TR_CLASS_RDR5) & mask;
        mtdcr(DCR_TR_CLASS_RDR5, dcr | SrcTgtEnable);
        break;
     case 6:  // classes 12, 13
        dcr = mfdcr(DCR_TR_CLASS_RDR6) & mask;
        mtdcr(DCR_TR_CLASS_RDR6, dcr | SrcTgtEnable);
        break;
     case 7:  // classes 14, 15
        dcr = mfdcr(DCR_TR_CLASS_RDR7) & mask;
        mtdcr(DCR_TR_CLASS_RDR7, dcr | SrcTgtEnable);
        break;
     default:
        TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": virtual tree=%d out of range 0-15.\n", vt ));
        _BLADE_ABORT( -1 );
      }

  return( 0 );
}


// Return the _BGL_RDR_* definition of the specified Class.
#undef  FN_NAME
#define FN_NAME "BGLTreeGetClass"
int BGLTreeGetClass(
        int  vt )             // Virtual tree number (12-15 are reserved for OS)
{
   Bit32 dcr;

#ifdef _SPI_CHECK_INFO
   printf( FN_NAME"( %d ) called.\n", vt );
#endif
   switch ( vt )
      {
      case 0:  // classes 0, 1
      case 1:
         dcr = mfdcr(DCR_TR_CLASS_RDR0);
         break;
      case 2:  // classes 2, 3
      case 3:
         dcr = mfdcr(DCR_TR_CLASS_RDR1);
         break;
      case 4:  // classes 4, 5
      case 5:
         dcr = mfdcr(DCR_TR_CLASS_RDR2);
         break;
      case 6:  // classes 6, 7
      case 7:
        dcr = mfdcr(DCR_TR_CLASS_RDR3);
        break;
      case 8:  // classes 8, 9
      case 9:
         dcr = mfdcr(DCR_TR_CLASS_RDR4);
         break;
      case 10:  // classes 10, 11
      case 11:
         dcr = mfdcr(DCR_TR_CLASS_RDR5);
         break;
      case 12:  // classes 12, 13
      case 13:
         dcr = mfdcr(DCR_TR_CLASS_RDR6);
         break;
      case 14:  // classes 14, 15
      case 15:
         dcr = mfdcr(DCR_TR_CLASS_RDR7);
         break;
     default:
         TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": virtual tree=%d out of range 0-15.\n", vt ));
         _BLADE_ABORT( -1 );
      }

   if ( vt & 0x1 )
      {
      // Route N+1, so return lower half.
      dcr &= 0x0000ffff;
      }
   else
      {
      // Route N, so return upper half.
      dcr &= 0xffff0000;
      dcr >>= 16;
      }

  return( dcr );
}


#if 0
// Allocates and returns the lowest-numbered available "application" class (those less
// than _BGL_TREE_PRIV_CLASS_START).
#undef  FN_NAME
#define FN_NAME "BGLTreeAllocateClass"
int BGLTreeAllocateClass( void )
{
   int i;
   Bit32 ca = 0x1;

   for( i=0; i<_BGL_TREE_PRIV_CLASS_START; i++ ) {
      if( !(__BGL_Tree_Class_Allocation & ca) ) {
         __BGL_Tree_Class_Allocation |= ca;
         return( i );
      }
      ca <<= 1;
   }

   TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": no application classes available.\n" ));

   return( -1 );
}

// Just like calling BGLTreeConfigureClass( BGLTreeAllocateClass(), SrcTgtEnable ).
#undef  FN_NAME
#define FN_NAME "BGLTreeAllocateAndConfigureClass"
int BGLTreeAllocateAndConfigureClass( int SrcTgtEnable )
{
   int vt;

   vt = BGLTreeAllocateClass();

   if( vt >= 0 )
      vt = BGLTreeConfigureClass( vt, SrcTgtEnable );

   return( vt );
}

// Allocates and returns the lowest-numbered available "privileged" class (those
// beginning with _BGL_TREE_PRIV_CLASS_START).  Returns a negative number if there are
// no privileged classes available.
#undef  FN_NAME
#define FN_NAME "BGLTreeAllocateClassPrivileged"
int BGLTreeAllocateClassPrivileged( void )
{
   int i;
   Bit32 ca = 0x1 << _BGL_TREE_PRIV_CLASS_START;

   for( i=_BGL_TREE_PRIV_CLASS_START; i<16; i++ ) {
      if( !(__BGL_Tree_Class_Allocation & ca) ) {
         __BGL_Tree_Class_Allocation |= ca;
         return( i );
      }
      ca <<= 1;
   }

   TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": no privileged classes available.\n" ));

   return( -1 );
}

// Forces the allocation of a specific class.  Non-zero return indicates that the class
// has already been allocated, and no action has been taken.
#undef  FN_NAME
#define FN_NAME "BGLTreeAllocateThisClass"
int BGLTreeAllocateThisClass( int vt )
{
   Bit32 ca = 0x1 << vt;

#ifdef _SPI_CHECK
   if( (vt < 0) || (vt > 15) ) {
      TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": class=%d is out of range 0-15.\n", vt ));
      _BLADE_ABORT( -1 );
   }
#endif

   if( !(__BGL_Tree_Class_Allocation & ca) ) {
      __BGL_Tree_Class_Allocation |= ca;
      return( vt );
   }

   TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": class %d is already allocated.\n", vt ));

   return( -1 );
}

// De-allocates the specified class, and clears the associated RDR register bits.
#undef  FN_NAME
#define FN_NAME "BGLTreeFreeClass"
void BGLTreeFreeClass( int vt )
{
   Bit32 ca = 0x1 << vt;

#ifdef _SPI_CHECK
   if( (vt < 0) || (vt >= 16) ) {
      TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": class=%d is out of range 0-15.\n", vt ));
      _BLADE_ABORT( -1 );
   }
   if( !(__BGL_Tree_Class_Allocation & ca) ) {
      TrSPI_TRC( TrSPI_TRC_CFGCLASS,( FN_NAME": class=%d is already free.\n", vt ));
   }
#endif

   BGLTreeConfigureClass( vt, 0 );
   __BGL_Tree_Class_Allocation &= ~ca;
}

#undef  FN_NAME
#define FN_NAME "BGLTreeClassesAllocated"
int BGLTreeClassesAllocated( void )
{
   int i, count = 0;
   Bit32 ca = 0x1;

   for( i=0; i<_BGL_TREE_PRIV_CLASS_START; i++ ) {
      if( __BGL_Tree_Class_Allocation & ca )
         count++;
      ca <<= 1;
   }

   return( count );
}

#undef  FN_NAME
#define FN_NAME "BGLTreeClassesAllocatedPrivileged"
int BGLTreeClassesAllocatedPrivileged( void )
{
   int i, count = 0;
   Bit32 ca = 0x1 << _BGL_TREE_PRIV_CLASS_START;

   for( i=_BGL_TREE_PRIV_CLASS_START; i<16; i++ ) {
      if( __BGL_Tree_Class_Allocation & ca )
         count++;
      ca <<= 1;
   }

   return( count );
}

#undef  FN_NAME
#define FN_NAME "BGLTreeClassesFree"
int BGLTreeClassesFree( void )
{
   return( _BGL_TREE_PRIV_CLASS_START - BGLTreeClassesAllocated() );
}

int BGLTreeClassesFreePrivileged( void )
{
   return( 16 - _BGL_TREE_PRIV_CLASS_START - BGLTreeClassesAllocatedPrivileged() );
}
#endif


