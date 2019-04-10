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
 #ifndef _BLMAP_H_
#define _BLMAP_H_

/////////////////////////////////////////////////////////////////////
// 
// This file implements an abstract function for the creation 
// and management of an associative map between abstract key values
// and abstract objects.
//
// This map module has no knowledge of the contents of the mapped keys
// or objects that it controls.  The calling function has
// no knowledge of the the internal data structrues used to control
// the map, so either can change independantly.
//
// This is to allow the flexibility to use either trees, hash tables
// or other technologies without affecting the underlying application.
//
//
// NOTE: With all these abstract pointers, there is little or no
// type checking so BE CAREFUL...
//

//#include <memory.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *BLMAP_HANDLE;		// abstract handle to a map object.
typedef void *BLMAP_ITERATOR;


///////////////////////////////////////////////////////////////////// 
typedef void (*PFN_BLMAP_DELETE_KEY_AND_ITEM)(void *pKey, 
                                              void *pItem, 
                                              void *pUserData);
//
// Optional Callback function in BlMapDelete and BlMapRemove
// if the caller wants to provide a function to delete
// what is pointed to by pKey and pItem.
//
// inputs:
//    pKey -- pointer to key to delete.
//    pItem -- pointer to item to delete.
//    pUserData -- pointer to user data.
// outputs:
//     none.
//
///////////////////////////////////////////////////////////////////// 


////////////////////////////////////////////////////////////////////
typedef int (*PFN_BLMAP_COMPARE)(void *pKey1, void *pKey2);
//
// pointer to key comparision function to compare two abstract
// keys.  This is supplied to the BlMapNew function and is used
// to determine greater than, less than or equal to values for the
// keys.
//
//
// inputs:
//     pKey1, pKey2 -- keys to compare.
// returns:
//     < 0 -- Key1 is less than Key2
//     > 0 -- Key1 is greater than Key2
//     ==0 -- Key1 is equal to Key2.
//
//////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
BLMAP_HANDLE BlMapNew(PFN_BLMAP_COMPARE pfnCompare,
                      PFN_BLMAP_DELETE_KEY_AND_ITEM pfnDelKeyAndItem,
                      void *pUserData);
//
// Create a empty map object.
//
// inputs:
//   pfnCompare -- pointer to compare routine to comapre to 
//               keys.
//   pfnDelKeyAndItem -- optional pointer to function to delete what is
//                      pointed to by key and item.
//   pUserData -- user data supplied when performing the callback
//                for pfnDelKeyAndItem. 
// outputs:
//   returns -- handle to new blmap object.
//              or NULL if we are out memory. 
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
void BlMapDelete(BLMAP_HANDLE hBlMap);
//
// Delete the map and all the memory associated with it.
//
// inputs:
//    hBlMap -- handle to map to delete.
//    
// outputs:
//    none.
//
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
void *BlMapGetItem(BLMAP_ITERATOR hIterator);
//
// retrieve the item associated with the iterator.
//
// inputs:
//    hIterator -- iterator..
// outputs:
//    returns the item pointer associated with this iterator.
//
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void *BlMapGetKey(BLMAP_ITERATOR hIterator);
//
// retrieve the Key associated with the iterator.
//
// inputs:
//    hIterator -- iterator..
// outputs:
//    returns the item pointer associated with this iterator.
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
void *BlMapFindItem(BLMAP_HANDLE hBlMap,
                    void *pKey);
//
// Find a map item from the key indicated.
//
// inputs:
//     hBlMap -- pointer to a map created by BlMapNew.
//     pKey -- key pointer (passed to pfnCompare for key comparisions).
// outputs:
//     returns -- returns the item pointer directly.
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
int BlMapInsert(BLMAP_HANDLE hBlMap,
                void *pKey,
                void *pItem);
//
// Insert a map item associated with the key pKey into the map.
// This will fail if there is either not enought memory to allocate
// a map entry or the key is a duplicate.
//
// inputs:
//     hBlMap -- pointer to a map created by BlMapNew.
//     pKey -- key pointer (passed to pfnCompare for key comparisions).
//     pItem -- pointer to item to associate with the key.
//
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//                BLERR_NO_MEMORY if out of memory.
//                BLERR_DUPLICATE_ENTRY there is already an entry
//                with the same key.
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
int BlMapRemove(BLMAP_HANDLE hBlMap,
                void *pKey);
//
// Remove the item associated with the key.
//
// inputs:
//     hBlMap -- pointer to a map created by BlMapNew.
//     pKey -- key pointer (passed to pfnCompare for key comparisions).
// outputs:
//     returns -- BLERR_SUCCESS if successful.
//                BLERR_NOT_FOUND if not found.
//
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
BLMAP_ITERATOR BlMapIterNew(BLMAP_HANDLE hBlMap);
//
// Create and initialize the an iterator for this map object.
//
// Call BlMapIterDelete when done with the iterator.
// Must call BlMapFirst to position the iterator to
// the first element.
//
// inputs:
//     hBlMap -- pointer to a map created by BlMapNew.
// outputs:
//     returns -- newly created iterator.
//
////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
void BlMapIterDelete(BLMAP_ITERATOR Iterator);
//
// Delete an iterator created by BlMapIterNew
//
// inputs:
//     hBlMap -- pointer to a map created by BlMapNew.
// outputs:
//     returns -- newly created iterator.
//
////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void *BlMapFirst(BLMAP_ITERATOR Iterator,
                 void **pKey);
//
// Retrieve the first map key item pair.
//
// inputs:
//    Iterator -- iterator for this object.
//    pKey -- pointer to where to stuff the pionter to the
//            key value. NULL if we don't want a key returned.
//
// outputs:
//    returns -- pointer to the item or NULL if no more items.
//    *pKey -- pointer to key
//
////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
void *BlMapNext(BLMAP_ITERATOR Iterator,
              void **pKey);
//
// Retrieve the next map pair of keys and item.
//
// inputs:
//    Iterator -- iterator for this object.
//    pKey -- pointer to where to stuff the pionter to the
//            key value. NULL if we don't want a key returned.
//
// outputs:
//    returns -- pointer to the item or NULL if no more items.
//    *pKey -- pointer to key
//
////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
unsigned long BlMapSize(BLMAP_HANDLE hBlMap);
//
// Retrieve the number of items in a the item map.
//
// inputs:
//     hBlMap -- pointer to a map created by BlMapNew.
// output:
//     returns -- the total number of items in the map.
//
/////////////////////////////////////////////////////////////////


#ifdef __cplusplus
};
#endif


#endif
