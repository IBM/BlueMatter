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
 #ifndef RPTREE_HPP_
#define RPTREE_HPP_

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

#ifndef PKFXLOG_RPT_ADD
#define PKFXLOG_RPT_ADD (1)
#endif

#ifndef PKFXLOG_RPT_WALK
#define PKFXLOG_RPT_WALK (1)
#endif

class rptIndexElement {
public: 	
	  unsigned int key_byte ;
	  unsigned int key_shift ;
	  unsigned int next_key[2] ;  // Set the high bit if it indicates a leaf
} ;

class rptIndex {
public:
<<<<<<< .mine
	  const unsigned int rid_size ;
	  const unsigned int record_size ;
//	  unsigned int data_offset ;
	  unsigned int root_node ;
	  unsigned int index_start ;
	  unsigned int max_tree_depth ;	
	  const unsigned int mCapacity ;
	  unsigned int mCount ;
	  const char * mRecordArray ;
	  const rptIndexElement * mIndexArray ;
	  rptIndex(unsigned int aRidSize,
			  unsigned int aRecordSize,
			  unsigned int aCapacity,
			  void * aRecordArray,
			  void * aIndexArray) : rid_size(aRidSize), 
			                        record_size(aRecordSize), 
			                        mCapacity(aCapacity),
			                        mRecordArray((char *)aRecordArray),
			                        mIndexArray((char *)aIndexArray),
			                        root_node(0x80000000),
			                        index_start(0),
			                        max_tree_depth(0),
			                        mCount(0)
			                        { } ;
      void * getNextFreeSlot(void) const { return (void *) (mRecordArray+mCount*record_size); }
      void add(void) ;
      void walk(void) ;
      void walkSubTree(unsigned int rootNode, unsigned int depth) ;
=======
	  const unsigned int rid_size ;
	  const unsigned int record_size ;
//	  unsigned int data_offset ;
	  unsigned int root_node ;
	  unsigned int index_start ;
	  unsigned int max_tree_depth ;	
	  const unsigned int mCapacity ;
	  unsigned int mCount ;
	  const char * mRecordArray ;
	  const rptIndexElement * mIndexArray ;
	  rptIndex(unsigned int aRidSize,
			  unsigned int aRecordSize,
			  unsigned int aCapacity,
			  void * aRecordArray,
			  void * aIndexArray) : rid_size(aRidSize), 
			                        record_size(aRecordSize), 
			                        mCapacity(aCapacity),
			                        mRecordArray((char *)aRecordArray),
			                        mIndexArray((char *)aIndexArray),
			                        root_node(0x80000000),
			                        index_start(0),
			                        max_tree_depth(0),
			                        mCount(0)
			                        { } ;
      void * getNextFreeSlot(void) const { return (void *) (mRecordArray+mCount*record_size); }
      void add(void) ;
>>>>>>> .r13031
};

<<<<<<< .mine
void rptIndex::add(void) 
{
	BegLogLine(PKFXLOG_RPT_ADD)
	  << "rptIndex::add mCount=" << mCount
	  << " root_node=" << root_node
	  << EndLogLine ;
	   unsigned int cand_offset = root_node ;
=======
void * rptIndex::add(void) 
{
	BegLogLine(PKFXLOG_RPT_ADD)
	  << "rptIndex::add mCount=" << mCount
	  << " root_node=" << root_node
	  << EndLogLine ;
	   unsigned int cand_offset = root_node ;
>>>>>>> .r13031

<<<<<<< .mine
	   if (0 == mCount)
	   {
	      /*
	       * First node in the tree
	       */
		   root_node = 0x80000000 ;
		   mCount = 1 ;
	   }
	   else
	   {
	     /*
	      * Scan the tree to find the appropriate data node
	      */
		 char * new_rid = mRecordArray+record_size*mCount ;
		 

	     while (0 == (cand_offset & 0x80000000))
	     {
	    	unsigned int cand_index = cand_offset ;
	        unsigned int candKeyByte = mIndexArray[cand_index].key_byte ;
	        unsigned int candKeyShift = mIndexArray[cand_index].key_shift ;
	        cand_offset = mIndexArray[cand_index].next_key [
	          ( new_rid [ candKeyByte ] >> candKeyShift )
	            & 1
	          ] ;
	    	BegLogLine(PKFXLOG_RPT_ADD)
	    	  << "examining byte " << candKeyByte
	    	  << " (0x" << new_rid [ candKeyByte ]
	    	  << ") bit " << candKeyShift
	    	  << " (" << (new_rid [ candKeyByte ] >> candKeyShift ) & 1)
	    	  << ")"
	    	  << " next cand_offset is " << (cand_offset >> 31) << " " (cand_offset & 0x7fffffff)
	    	  << EndLogLine ;
	        
	     } /* endwhile */
	     /*
	      * We have now descended to a data node. Find which is the
	      * most significant difference bit.
	      */
	     unsigned int candidateRecordIndex = cand_offset & 0x7fffffff
	     char * cand_rid = mRecordArray+record_size*candidateRecordIndex ;
	     unsigned int new_byte ;
	     unsigned int cand_byte ;
	     for (rid_x = 0; rid_x < rid_size; rid_x += 1)
	     {
	        cand_byte = cand_rid [ rid_x ] ;
	        new_byte = new_rid [ rid_x ] ;
	        if (cand_byte != new_byte) break ;
	     } /* endfor */

	     if (rid_x < rid_size) /* A new key ... */
	     {
	       rid_xor = cand_byte ^ new_byte ;
	       rid_y = hibit[rid_xor] ;
	       BegLogLine(PKFXLOG_RPT_ADD)
	         << "Split at byte " << rid_x
	         << " bit " << " rid_y
	         << " from " << cand_byte
	       /*
	        * Scan from the root again to find the split point
	        */
	       unsigned int split_offset = root_node ;
//	       split_ofs_ptr = & VAL(table_shmat_head_t,shmat_address,root_node) ;
//	       split_ofs = VAL(table_shmat_head_t,shmat_address,root_node) ;

	       while (0 == (split_offset & 0x80000000))
	       {
		    	unsigned int splitIndex = split_offset ;
		        unsigned int splitKeyByte = mIndexArray[splitIndex].key_byte ;
		        unsigned int splitKeyShift = mIndexArray[splitIndex].key_shift ;
		        BegLogLine(PKFXLOG_RPT_ADD)
		          << "Scanning, splitIndex=" << splitIndex
		          << " splitKeyByte=" << splitKeyByte
		          << " splitKeyShift=" << splitKeyShift
		          << EndLogLine ;
//	         split_ptr = PTR(table_shmat_index_t *,shmat_address, split_ofs) ;
//	         split_key_byte = split_ptr -> key_byte ;
//	         split_key_shift = split_ptr -> key_shift ;
	         if (splitKeyByte > rid_x
	             ||
	            (splitKeyByte == rid_x && splitKeyShift < rid_y)
	         ) break ;

	         split_offset = mIndexArray[splitIndex].next_key [
		          ( new_rid [ splitKeyByte ] >> splitKeyShift )
		            & 1
		          ] ;
//	         split_ofs_ptr = & (
//	                      split_ptr -> next_key [
//	          ( new_rid [ split_key_byte ] >> split_key_shift )
//	            & 1
//	          ] ) ;splitIndex
//	         split_ofs = * split_ofs_ptr ;
	       } /* endwhile */
	       /*
	        * All being well, we should now be pointing at the index to split.
	        * This either establishes a new root node, or tacks into the tree
	        */
	       unsigned int freeNodeIndex = mCount ;
	       mIndexArray[freeNodeIndex].key_byte = rid_x ;
	       mIndexArray[freeNodeIndex].key_shift = rid_y ;
//	       free_node_ofs = * free_node_ofs_p ;
//	       free_node_p = PTR(table_shmat_index_t *,shmat_address,free_node_ofs) ;
//	       free_node_p -> key_byte = rid_x ;
//	       free_node_p -> key_shift = rid_y ;
	       if ((new_byte >> rid_y) & 1)
	       {
	    	   mIndexArray[freeNodeIndex].next_key[0] = split_ofs ;
	    	   mIndexArray[freeNodeIndex].next_key[1] = mCount ;
	       }
	       else
	       {
	    	   mIndexArray[freeNodeIndex].next_key[1] = split_ofs ;
	    	   mIndexArray[freeNodeIndex].next_key[0] = mCount ;
	       } /* endif */
	       if( split_offset == root_node ) 
	         { 
	    	   BegLogLine(PKFXLOG_RPT_ADD)
	    	     << "New root"
	    	     << EndLogLine ;
	    	   root_node = split_ofs ;
	         }
	       else
	         {
	    	   BegLogLine(PKFXLOG_RPT_ADD)
	    	     << "Tacked on below tree entry " << splitIndex
	    	     << EndLogLine ;
	    	   mIndexArray[splitIndex].next_key [
	    	                     		          ( new_rid [ splitKeyByte ] >> splitKeyShift )
	    	                     		            & 1
	    	                     		          ] = split_ofs ;
	         }
//	       * split_ofs_ptr = free_node_ofs ;
//	       * free_node_ofs_p = free_node_ofs + sizeof(table_shmat_index_t) ;
		    mCount += 1 ;
	     }
	     else
	     { /* A duplicate key. Ignore for the moment. */
           BegLogLine(PKFXLOG_RPT_ADD)
             << "Duplicate key, record not added"
             << EndLogLine ;
	     } /* endif */
	   } /* endif */
	}
}

void rptIndex::walkSubTree(unsigned int aNode, int depth)
{
	if( aNode & 0x80000000 )
	  {
		  unsigned int leafIndex = aNode & 0x7fffffff ;
		  char * rid_ptr = mRecordArray+record_size*leafIndex ;
		  unsigned int rid_value = *(unsigned int *) rid_ptr ;
		  BegLogLine(PKFXLOG_RPT_WALK)
		    << "Leaf, index=0x" << (void *) leafIndex 
		    << " depth=0x" << (void *) depth
		    << " key=0x" << (void *) rid_value
		    << EndLogLine ;
	  }
	else
	  {
		walksubtree(mIndexArray[aNode].next_key[0],depth+1) ;
		walksubtree(mIndexArray[aNode].next_key[1],depth+1) ;
	  }
}
void rptIndex::walk(void)
{
    BegLogLine(PKFXLOG_RPT_WALK)
      << "Walking tree, root_node=" << root_node
      << " count=" << mCount
      << EndLogLine ;
    if( mCount > 0 )
      {
	     walksubtree(root_node,0) ;
      } 
    else
      {
        BegLogLine(PKFXLOG_RPT_WALK)
          << "Empty"
          << EndLogLine ;
    	
      }
}

=======
	   if (0 == mCount)
	   {
	      /*
	       * First node in the tree
	       */
		   root_node = 0x80000000 ;
	   }
	   else
	   {
	     /*
	      * Scan the tree to find the appropriate data node
	      */
		 char * new_rid = mIndexArray+record_size*mCount ;
		 

	     while (0 == (cand_offset & 0x80000000))
	     {
	    	unsigned int cand_index = cand_offset ;
	        unsigned int candKeyByte = mIndexArray[cand_index].key_byte ;
	        unsigned int candKeyShift = mIndexArray[cand_index].key_shift ;
	    	BegLogLine(PKFXLOG_RPT_ADD)
	    	  << "examining byte " << candKeyByte
	    	  << " (0x" << new_rid [ candKeyByte ]
	    	  << ") bit " << candKeyShift
	    	  << " (" << (new_rid [ candKeyByte ] >> candKeyShift ) & 1)
	    	  << ")"
	    	  << " next cand_offset is " << (cand_offset >> 31) << " " (cand_offset & 0x7fffffff)
	    	  << EndLogLine ;
	        
	        DIAG(diag_scan,("examining byte %i bit %i\n",
	             cand_index -> key_byte ,
	             cand_index -> key_shift)) ;
	        cand_offset = mIndexArray[cand_index].next_key [
	          ( new_rid [ candKeyByte ] >> candKeyShift )
	            & 1
	          ] ;
	     } /* endwhile */
	     /*
	      * We have now descended to a data node. Find which is the
	      * most significant difference bit.
	      */
	     cand_rid = PTR(table_shmat_rid_t *,shmat_address,cand_offset) -> rid_field ;
	     for (rid_x = 0; rid_x < rid_size; rid_x += 1)
	     {
	        cand_byte = cand_rid [ rid_x ] ;
	        new_byte = new_rid [ rid_x ] ;
	        if (cand_byte != new_byte) break ;
	     } /* endfor */

	     if (rid_x < rid_size) /* A new key ... */
	     {
	       rid_xor = cand_byte ^ new_byte ;
	       rid_y = hibit[rid_xor] ;
	       DIAG(diag_scan,("Split at byte %i bit %i from %i\n",rid_x, rid_y, cand_byte)) ;
	       /*
	        * Scan from the root again to find the split point
	        */
	       split_ofs_ptr = & VAL(table_shmat_head_t,shmat_address,root_node) ;
	       split_ofs = VAL(table_shmat_head_t,shmat_address,root_node) ;

	       while (split_ofs >= index_start)
	       {
	         split_ptr = PTR(table_shmat_index_t *,shmat_address, split_ofs) ;
	         split_key_byte = split_ptr -> key_byte ;
	         split_key_shift = split_ptr -> key_shift ;
	         if (split_key_byte > rid_x
	             ||
	            (split_key_byte == rid_x && split_key_shift < rid_y)
	         ) break ;

	         split_ofs_ptr = & (
	                      split_ptr -> next_key [
	          ( new_rid [ split_key_byte ] >> split_key_shift )
	            & 1
	          ] ) ;
	         split_ofs = * split_ofs_ptr ;
	       } /* endwhile */
	       /*
	        * All being well, we should now be pointing at the index to split !
	        */
	       free_node_ofs = * free_node_ofs_p ;
	       free_node_p = PTR(table_shmat_index_t *,shmat_address,free_node_ofs) ;
	       free_node_p -> key_byte = rid_x ;
	       free_node_p -> key_shift = rid_y ;
	       if ((new_byte >> rid_y) & 1)
	       {
	          free_node_p -> next_key[0] = split_ofs ;
	          free_node_p -> next_key[1] = current_record_offset ;
	       }
	       else
	       {
	          free_node_p -> next_key[1] = split_ofs ;
	          free_node_p -> next_key[0] = current_record_offset ;
	       } /* endif */
	       * split_ofs_ptr = free_node_ofs ;
	       * free_node_ofs_p = free_node_ofs + sizeof(table_shmat_index_t) ;
	     }
	     else
	     { /* A duplicate key. Ignore for the moment. */

	     } /* endif */
	   } /* endif */
	    mCount += 1 ;
	}
}

>>>>>>> .r13031
#endif /*RPTREE_HPP_*/
