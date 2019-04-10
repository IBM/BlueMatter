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
 #ifndef __TREE_REDUCE__
#define __TREE_REDUCE__

// Operation code come from BGL_TreePktSPI.hpp 
// For reference:
// #define _BGL_TREE_OPCODE_OR      (0x1)
// #define _BGL_TREE_OPCODE_AND     (0x2)
// #define _BGL_TREE_OPCODE_XOR     (0x3)
// #define _BGL_TREE_OPCODE_MAX     (0x5)
// #define _BGL_TREE_OPCODE_ADD     (0x6)

void GlobalTreeReduce( unsigned long * aReduceIn, unsigned long * aReduceOut, int aSize, unsigned aOperation );
void GlobalTreeReduce64( long long * aReduceIn, long long * aReduceOut, int aSize, unsigned aOperation );
//int GlobalTreeReduce128( void * aReduceIn, void * aReduceOut, int aSize, unsigned aOperation );

#endif
