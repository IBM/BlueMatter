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
 #ifndef __PK_NODE_HPP__
#define __PK_NODE_HPP__
// Per Node structure contains information kept once per node or address space.
// This is read-only from any core/fiber

#include <BonB/BGL_PartitionSPI.h>
//#include <Pk/FxLogger.hpp>

#if !defined(PKFXLOG_NODE)
#define PKFXLOG_NODE (0) 
#endif
// Ability to convert from a sequential node number to a torus target
class PkNodeXYZ
{
  public: 
//	 unsigned int mShiftX : 5 ;
     unsigned int mMaskX : 5 ; 
   unsigned int mShiftY : 5 ;
   unsigned int mMaskY : 5 ; 
   unsigned int mShiftZ : 5 ;
//	 unsigned int mMaskZ : 5 ;
   
   // Count how many leading zeros 'a' has.
   unsigned int cntlz(unsigned int a) const
   {
    unsigned int result = 0 ; 
    for ( result=0; (result<32) && ( 0 == ( (0x80000000 >> result) & a ) )  ; result += 1 )
    {
    } 
    return result ; 
   }
   unsigned int TargetX(unsigned int aTargetNode) const 
   {
//	 	return ( aTargetNode >> mShiftX ) & ( ( (unsigned int) 0xffffffff) >> mMaskX ) ;
    unsigned int result = ( aTargetNode            ) & ( ( (unsigned int) 0xffffffff) >> mMaskX ) ;
//	 	BegLogLine(PKFXLOG_NODE)
//	 	  << "PkNodeXYZ::TargetX aTargetNode=" << aTargetNode
//	 	  << " result=" << result
//	 	  << EndLogLine ;
    return result ; 
   } 

   unsigned int TargetY(unsigned int aTargetNode) const 
   {
    unsigned int result = ( aTargetNode >> mShiftY ) & ( ( (unsigned int) 0xffffffff) >> mMaskY ) ;
//	 	BegLogLine(PKFXLOG_NODE)
//	 	  << "PkNodeXYZ::TargetY aTargetNode=" << aTargetNode
//	 	  << " result=" << result
//	 	  << EndLogLine ;
    return result ; 
   } 

   unsigned int TargetZ(unsigned int aTargetNode) const 
   {
//  	return ( aTargetNode >> mShiftZ ) & ( ( (unsigned int) 0xffffffff) >> mMaskZ ) ;
    unsigned int result = ( aTargetNode >> mShiftZ )                                              ;
//	 	BegLogLine(PKFXLOG_NODE)
//	 	  << "PkNodeXYZ::TargetZ aTargetNode=" << aTargetNode
//	 	  << " result=" << result
//	 	  << EndLogLine ;
    return result ; 
   } 
   
   // For eaxmple with an 8x8x8 partution we need to end up with
   // mMaskX = 29
   // mMaskY = 29
   // mMaskZ = 29
     // mShiftX = 0
     // mShiftY = 3
     // mShiftZ = 6
   void Init(void)
   {
    int xNodes, yNodes, zNodes ;
    BGLPartitionGetDimensions(&xNodes, &yNodes, &zNodes) ;
    mMaskX = cntlz(xNodes)+1 ; 
    mMaskY = cntlz(yNodes)+1 ; 
//	 	mMaskZ = cntlz(zNodes) ;
//  	mShiftX = 0 ; 
    mShiftY = 32-mMaskX ;
    mShiftZ = 32-mMaskY+mShiftY ;
//	 	BegLogLine(PKFXLOG_NODE)
//	 	  << "PkNodeXYZ::Init xNodes=" << xNodes
//	 	  << " yNodes=" << yNodes
//	 	  << " zNodes=" << zNodes
//	 	  << " mMaskX=" << mMaskX
//	 	  << " mMaskY=" << mMaskY
//	 	  << " mShiftY=" << mShiftY
//	 	  << " mShiftZ=" << mShiftZ
//	 	  << EndLogLine ;
    
   }

} ;

struct PkNodeT
  {
  double StartCachePadding[ 32 ];

  int     Main_ArgC;
  char ** Main_ArgV;
  char ** Main_EnvP;

  int     PkArgCount;

  //PkType_TimeRaw StartTimeRaw;
  int  LocalId;
  int  Count;
  
  PkNodeXYZ NodeXYZ ;

  double EndCachePadding[ 32 ];
  };

extern PkNodeT pkNode;

//
//
// PkNodeGetId() : Get the node id containing caller - this is a counting number
//
inline
int
PkNodeGetId()
  {
  return( pkNode.LocalId );
  }
//
//
// PkNodeGetCount() : Get the number of nodes in the partition
//
inline
int
PkNodeGetCount()
  {
  return( pkNode.Count );
  }
//
//
// PkNodeBarrier() : Get the number of nodes in the partition
//
#endif
