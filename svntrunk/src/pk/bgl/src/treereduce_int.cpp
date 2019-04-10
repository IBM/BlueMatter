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
 /*
 * File: treereduce_int.cpp
 *
 * Purpose: Implement integer-point 'reduce' on the BLADE tree SPI
 *
 * Author: Alex Rayshubskiy (arayshu@us.ibm.com)
 *         (c) IBM, 2003
 *
 * History : 12/23/2003: arayshu - Created
 *           
 */


#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }

#include <sys/blade_types.h>
#include <pk/treereduce.hpp>
#include <spi/BGL_TreePktSPI.h>

extern "C" 
{
  void s0printf(const char * , ...) ;
}

_BGL_TreeHwHdr addReduceHdr __attribute__((aligned(0x01 << 5)));

_BGL_TreeHwHdr addReduceHdrReturned __attribute__((aligned(0x01 << 5)));

char ReduceBuff[ _BGL_TREE_PKT_MAX_BYTES ] __attribute__((aligned(0x01 << 5)));

// And a version callable from C
void tr_AllReduceCharInit( unsigned int aChannel ,unsigned int aClass )
{  
  BGLTreeMakeAluHdr( &addReduceHdr,
                     aClass,                // class
                     _BGL_TREE_OPCODE_ADD,  // opcode
                     _BGL_TREE_OPSIZE_BIT64 // size
                     ) ;
}

void tr_AllReduceChar (
   char * InOut,
   unsigned int Length,
   unsigned int Channel
)
{

  int FullBlocks   = Length / _BGL_TREE_PKT_MAX_BYTES;
  int PartialBlockSize = Length % _BGL_TREE_PKT_MAX_BYTES;
  int PartialBlockStart = FullBlocks * _BGL_TREE_PKT_MAX_BYTES;
  
  int i;
  for( i=0; i < FullBlocks; i++ )
    {
      
      int maxSendResult = BGLTreeSend( Channel,                // class
                                       &addReduceHdr,          // Header we made before
                                       &InOut[ i * _BGL_TREE_PKT_MAX_BYTES ],  // Operands being sent
                                       _BGL_TREE_PKT_MAX_BYTES // appropriate number of blocks, by construction
                                     ) ;
      
      int maxRecvResult = BGLTreeReceive( Channel,                // class
                                          &addReduceHdrReturned,  // Header after circulating the tree
                                          &InOut[ i * _BGL_TREE_PKT_MAX_BYTES ], // Operands being received
                                          _BGL_TREE_PKT_MAX_BYTES // appropriate number of blocks, by construction
                                          ) ;  
    }    
  
  if ( PartialBlockSize > 0 )
    {       
//         s0printf( "PartialBlockSize=%d _BGL_TREE_PKT_MAX_BYTES=%d\n", 
//                   PartialBlockSize,  
//                   _BGL_TREE_PKT_MAX_BYTES );
        
        char * InOutEndPkt = (char *) &(InOut[ i * _BGL_TREE_PKT_MAX_BYTES] ) ;
        char * EndPktBuf   = (char *) ReduceBuff;

        for(int i=0; i < PartialBlockSize; i++ )
          {                               
          EndPktBuf[i] = InOutEndPkt[i];
          }
        
      int maxSendResult = BGLTreeSend( Channel,                // class
                                       &addReduceHdr,          // Header we made before
                                       ReduceBuff,            // Operands being sent
                                       _BGL_TREE_PKT_MAX_BYTES // appropriate number of blocks, by construction
                                       ) ;
      
      int maxRecvResult = BGLTreeReceive( Channel,                // class
                                          &addReduceHdrReturned,  // Header after circulating the tree
                                          ReduceBuff,            // Operands being received
                                          _BGL_TREE_PKT_MAX_BYTES // appropriate number of blocks, by construction
                                          ) ;        

      for(int i=0; i < PartialBlockSize; i++ )
        {                               
        InOutEndPkt[i] = EndPktBuf[i];
        }
    }
}
