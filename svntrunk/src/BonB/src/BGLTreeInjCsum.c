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
//      File: blade/spi/BGLTreeInjCsum.c
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
//   History: 08/21/2003: MAB - Created.
//
//
#include <BonB/blade_on_blrts.h>

#include <stdio.h> // printf

Bit32 _bgl_csum[4] ALIGN_L1_CACHE;

////////////////////////////////////////////////////////////////////////////////
// Initialize header and payload injection checksums.
////////////////////////////////////////////////////////////////////////////////
void BGLTreeInjectionCsumInit( int vc, Bit32 hdr_init, Bit32 pyld_init )
{
//   dcache_invalidate_line( _bgl_csum );
   if ( vc == 0 ) {
      _bgl_csum[0] = hdr_init;
      _bgl_csum[1] = pyld_init;
   } else {
      _bgl_csum[2] = hdr_init;
      _bgl_csum[3] = pyld_init;
   }
   dcache_store_line( _bgl_csum );
}


////////////////////////////////////////////////////////////////////////////////
// Add header and payload to injection checksums.
////////////////////////////////////////////////////////////////////////////////
void BGLTreeInjectionCsumAdd( int vc, Bit32 hdr, Bit16 *pyld, int bytes )
{
   int i;
   int words;

   // Round bytes to next multiple of 16.
   words = bytes & 0xfffffff0;
   if( bytes != words ) {
      words += 16;
   }
   words >>= 1;

//   dcache_invalidate_line( _bgl_csum );
   if ( vc == 0 ) {
      _bgl_csum[0] += hdr;
      for ( i = 0; i < words; i++ ) {
         _bgl_csum[1] += (Bit32)*pyld;
         pyld++;
      }
   } else {
      _bgl_csum[2] += hdr;
      for ( i = 0; i < words; i++ ) {
         _bgl_csum[3] += (Bit32)*pyld;
         pyld++;
      }
   }
   dcache_store_line( _bgl_csum );
}


////////////////////////////////////////////////////////////////////////////////
// Return a calculated checksum.
////////////////////////////////////////////////////////////////////////////////
Bit32 BGLTreeInjectionCsumGet( int offset )
{
//   dcache_invalidate_line( _bgl_csum );
   return( _bgl_csum[offset] );
}


////////////////////////////////////////////////////////////////////////////////
// Print all calculated checksums.
////////////////////////////////////////////////////////////////////////////////
void BGLTreeInjectionCsumPrint( char *preprint, int print_header )
{
//   dcache_invalidate_line( _bgl_csum );
   if( print_header ) {
      printf( "%s %10s %10s %10s %10s\n", preprint, "HDR_VC0", "PYLD_VC0", "HDR_VC1", "PYLD_VC1" );
   }
   printf( "%s %10.8x %10.8x %10.8x %10.8x\n", preprint, (unsigned int) _bgl_csum[0], (unsigned int) _bgl_csum[1], (unsigned int) _bgl_csum[2], (unsigned int) _bgl_csum[3] );
}

