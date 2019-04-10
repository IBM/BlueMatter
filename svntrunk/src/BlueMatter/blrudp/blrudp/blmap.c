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
//
// NOTE: the core red-black tree alogrithm came from some code at the following 
// url: http://www.cusa.berkley.edu/~emin/source_code/red_black_tree/red_black_tree.c.
//
// The work this is based comes from:
//
// Introduction to Algorithms 
//             by
// Cormen, Lieserson, Rivest.
//
//
// See also http://www.geocities.com/Area51/Valut/3150/rbtc.htm
//
//
// Implementation note:  RB. 8/17/2001.
// This program was based on some C code that implemented sentinel nodes.  However,
// noticing all the checking of nodes against being == to the sentinal node pointer
// I am not sure that there is much benifit here to using them.  This needs to be
// further looked into....
//  



#include "hostcfg.h"

#include <stddef.h>

#if HAVE_MEMORY_H
    #include <memory.h>
#endif

#include "blrudp_memory.h"
#include "dbgPrint.h"
#include "BlErrors.h"

#include "blmap.h"

#include "bldefs.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0 
#endif
//
// signature to validate pointers to a tree.
//
#define BLTREE_SIG 'bltr'
//
// node tree structure.
//
//
typedef struct T_BL_RED_BLK_NODE {
    void* pKey;
    void* pItem;
    int   bIsRed; /* if red=0 then the node is black */
    struct T_BL_RED_BLK_NODE* pLeft;
    struct T_BL_RED_BLK_NODE* pRight;
    struct T_BL_RED_BLK_NODE* pParent;
} BL_RED_BLK_NODE;


//
// A sentinel is used for root and for nil.  These sentinels are            
// created when RBTreeCreate is called.  root->pLeft should always           
// point to the node which is the root of the tree.  nil points to a        
// node which should always be black but has aribtrary children and         
// parent and no key or info.  The point of using these sentinels is so     
// that the root and nil nodes do not require special cases in the code     
//


//
// structure for the tree.
typedef struct {
    PFN_BLMAP_COMPARE pfnCompare;                   // key comparision routine
    PFN_BLMAP_DELETE_KEY_AND_ITEM pfnDelKeyAndItem;   // key and item deletion routine.
    void *pUserData;
    BL_RED_BLK_NODE*  pRoot;             
    unsigned long ulNumNodes;
} BL_RED_BLK_TREE;

typedef struct {
    BL_RED_BLK_TREE *pTree;                         // pointer to the tree the iterator belongs to.
    BL_RED_BLK_NODE *pCurrNode;                     // current red black node.
}  BL_RED_BLK_ITERATOR;

#define IS_RED(x) ((x) && ((x)->bIsRed)) ? TRUE : FALSE


////////////////////////////////////////////////////////////
// private functions
void BlMapDeleteTreeContents(BL_RED_BLK_TREE *pTree,
                             BL_RED_BLK_NODE *pNode);
void BlMapLeftRotate(BL_RED_BLK_TREE* pTree, 
                     BL_RED_BLK_NODE* pX);
void BlMapInsertNode(BL_RED_BLK_TREE *pTree, 
                     BL_RED_BLK_NODE *pNode);
BL_RED_BLK_NODE *BlMapNextNode(BL_RED_BLK_TREE *pTree, 
                               BL_RED_BLK_NODE *pNode);
BL_RED_BLK_NODE *BlMapFindNode(BL_RED_BLK_TREE *pTree,
                               void *pKey);
void BlMapRightRotate(BL_RED_BLK_TREE* pTree, 
                     BL_RED_BLK_NODE* pY);
void BlMapLeftRotate(BL_RED_BLK_TREE* pTree, 
                     BL_RED_BLK_NODE* pY);
void BlMapRemoveNode(BL_RED_BLK_TREE *pTree, 
                     BL_RED_BLK_NODE *pNode);
void BlMapFixUp(BL_RED_BLK_TREE *pTree, 
                BL_RED_BLK_NODE *pNode);
BL_RED_BLK_TREE *BlMapGetTree(BLMAP_HANDLE hBlMap);


//
// NOTE: we may want to do this inline.
//
///////////////////////////////////////////////////////////////////////
BL_RED_BLK_TREE *BlMapGetTree(BLMAP_HANDLE hBlMap)
//
// Validate the tree pointer referenced by the handle and
// return the pointer to it.
// 
// inputs:
//     hBlMap -- pointer to a map created by BlMapNew.
// outputs:
//     returns -- pointer to tree or NULL if the pointer is invalid
{
    BL_RED_BLK_TREE *pTree = NULL;  
    pTree = (BL_RED_BLK_TREE *)hBlMap;
    ASSERT(pTree);                      // hey, that's not right...

    if (pTree == NULL)  // punt if this is NULL...
        FAIL;

    return(pTree);
Fail:
    return(NULL);
}



/////////////////////////////////////////////////////////////////
BLMAP_HANDLE BlMapNew(PFN_BLMAP_COMPARE pfnCompare,
                      PFN_BLMAP_DELETE_KEY_AND_ITEM pfnDelKeyAndItem,
                      void *pUserData)
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
{
    BL_RED_BLK_TREE   *pTree = NULL;  
    BL_RED_BLK_NODE *pRoot = NULL;

    ASSERT(pfnCompare);
    if (pfnCompare == NULL)
        FAIL;

    pTree = BlrAllocMem(sizeof(*pTree));
    if (pTree == NULL)
        FAIL;           // oops out of memory....

    memset(pTree, 0, sizeof(*pTree));
    //
    // now initialize the tree base structure.
    //
    pTree->pfnCompare = pfnCompare;     // save function pointers.
    pTree->pfnDelKeyAndItem = pfnDelKeyAndItem;
    pTree->pUserData = pUserData;

    pTree->ulNumNodes = 0;


    pRoot  = BlrAllocMem(sizeof(*pRoot));   // allocate the root node...
    if (pRoot == NULL)                             // oops, out of memory.
        FAIL;

    pTree->pRoot = pRoot;       // connect this as the root node.
    pRoot->pParent = NULL;      // root node points to NIL in all directions.
    pRoot->pLeft   = NULL;
    pRoot->pRight  = NULL;      // roots never have right nodes....
    pRoot->pKey    = NULL;
    pRoot->pItem   = NULL;
    pRoot->bIsRed  = FALSE;

    return(pTree);
Fail:
    if (pTree)
        BlMapDelete(pTree);
    return(NULL);
}

///////////////////////////////////////////////////////////////////
void BlMapDelete(BLMAP_HANDLE hBlMap)
//
// Delete the map and all the memory associated with it.
//
// inputs:
//    hBlMap -- handle to map to delete.
// outputs:
//    none.
//
//////////////////////////////////////////////////////////////////////
{
    BL_RED_BLK_TREE *pTree = BlMapGetTree(hBlMap);  
    if (pTree == NULL)
        return;

    BlMapDeleteTreeContents(pTree, pTree->pRoot->pLeft);

    BlrFreeMem(pTree->pRoot);
    BlrFreeMem(pTree);
    return;
}

////////////////////////////////////////////////////////////////
void *BlMapFindItem(BLMAP_HANDLE hBlMap,
                    void *pKey)
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
{
    BL_RED_BLK_TREE *pTree;  
    BL_RED_BLK_NODE *pX;    

    pTree = BlMapGetTree(hBlMap);  
    if (pTree == NULL)
        FAIL;

    pX = BlMapFindNode(pTree, pKey);
    if (pX == NULL)
        FAIL;
    return(pX->pItem);
Fail:
    return(NULL);
}


//////////////////////////////////////////////////////////////
void *BlMapGetItem(BLMAP_ITERATOR hIterator)
//
// retrieve the item associated with the iterator.
//
// inputs:
//    hIterator -- iterator..
// outputs:
//    returns the item pointer associated with this iterator.
//
///////////////////////////////////////////////////////////////
{
    BL_RED_BLK_NODE *pNode = (BL_RED_BLK_NODE *)(hIterator);

    ASSERT(pNode);
    if (pNode == NULL)
        FAIL;

    return(pNode->pItem);
Fail:
    return(NULL);
}

//////////////////////////////////////////////////////////////
void *BlMapGetKey(BLMAP_ITERATOR hIterator)
//
// retrieve the Key associated with the iterator.
//
// inputs:
//    hIterator -- iterator..
// outputs:
//    returns the item pointer associated with this iterator.
//
///////////////////////////////////////////////////////////////
{
    BL_RED_BLK_NODE *pNode = (BL_RED_BLK_NODE *)(hIterator);

    ASSERT(pNode);
    if (pNode == NULL)
        FAIL;

    return(pNode->pKey);
Fail:
    return(NULL);
}

///////////////////////////////////////////////////////////////////////
int BlMapInsert(BLMAP_HANDLE hBlMap,
                void *pKey,
                void *pItem)
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
{
    int nErr = BLERR_INTERNAL_ERR;
    BL_RED_BLK_TREE *pTree;  
    BL_RED_BLK_NODE* pY;
    BL_RED_BLK_NODE* pX;
    BL_RED_BLK_NODE* pNewNode = NULL;
    BL_RED_BLK_NODE* pRoot;
    //
    // validate the tree pointer.
    pTree = BlMapGetTree(hBlMap);  
    if (pTree == NULL)
        FAILERR(nErr, BLERR_PARAM_ERR);

    pRoot = pTree->pRoot;

    pNewNode = BlrAllocMem(sizeof(*pNewNode));
    if (pNewNode == NULL)
        FAILERR(nErr, BLERR_NO_MEMORY);         // no memory, punt.

    pNewNode->pKey = pKey;
    pNewNode->pItem = pItem;        // save off some of the info.
    pNewNode->bIsRed = TRUE;
    pX = pNewNode;                      // start here.

    BlMapInsertNode(pTree, pNewNode);       // insert the node into the tree.

    // check to see if we need to re-balance
    while ((pX != pRoot) && (pX->pParent->bIsRed))
    {
        if (pX->pParent == pX->pParent->pParent->pLeft) 
        {
            pY = pX->pParent->pParent->pRight;
            if ((pY) && (pY->bIsRed))
            {
                pX->pParent->bIsRed=FALSE;
                pY->bIsRed = FALSE;;
                pX->pParent->pParent->bIsRed=TRUE;
                pX=pX->pParent->pParent;
            } 
            else 
            {
                if (pX == pX->pParent->pRight) 
                {
                    pX=pX->pParent;
                    BlMapLeftRotate(pTree,pX);
                }
                pX->pParent->bIsRed=FALSE;
                pX->pParent->pParent->bIsRed=TRUE;
                BlMapRightRotate(pTree,pX->pParent->pParent);
            } 
        } 
        else 
        { /* case for x->pParent == x->pParent->pParent->pRight */
            pY=pX->pParent->pParent->pLeft;
            if ((pY) && (pY->bIsRed))
            {
                pX->pParent->bIsRed = FALSE;
                pY->bIsRed=FALSE;
                pX->pParent->pParent->bIsRed=TRUE;
                pX=pX->pParent->pParent;
            } 
            else 
            {
                if (pX == pX->pParent->pLeft) 
                {
                    pX=pX->pParent;
                    BlMapRightRotate(pTree,pX);
                }
                pX->pParent->bIsRed=FALSE;
                pX->pParent->pParent->bIsRed=TRUE;
                BlMapLeftRotate(pTree,pX->pParent->pParent);
            } 
        }
    }
    pTree->pRoot->pLeft->bIsRed=FALSE;
    // tree is now inserted and re-balanced.

    return(BLERR_SUCCESS);
Fail:
    if ((pNewNode) && (pTree))                  // cleanup after ourselves.
        BlrFreeMem(pNewNode);
    return(nErr);
}

//////////////////////////////////////////////////////////////////////
int BlMapRemove(BLMAP_HANDLE hBlMap,
                void *pKey)
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
{
    int nErr = BLERR_INTERNAL_ERR;
    BL_RED_BLK_TREE *pTree;  
    BL_RED_BLK_NODE *pNode;
    
    //
    // validate the tree pointer.
    pTree = BlMapGetTree(hBlMap);  
    if (pTree == NULL)
        FAIL;

    pNode = BlMapFindNode(pTree, pKey);
    if (pNode == NULL)
        FAILERR(nErr, BLERR_NOT_FOUND);

    BlMapRemoveNode(pTree, pNode);    
    
    return(BLERR_SUCCESS);
Fail:
    return(nErr);
}

/////////////////////////////////////////////////////////////////
BLMAP_ITERATOR BlMapIterNew(BLMAP_HANDLE hBlMap)
//
// Create and initialize the an iterator for this map object.
//
// Call BlMapIterDelete when done with the iterator.
//
// inputs:
//     hBlMap -- pointer to a map created by BlMapNew.
// outputs:
//     returns -- newly created iterator.
//
////////////////////////////////////////////////////////////////
{
    BL_RED_BLK_TREE *pTree;  
    BL_RED_BLK_ITERATOR *pIter;
    //
    // validate the tree pointer.
    pTree = BlMapGetTree(hBlMap);  
    if (pTree == NULL)
        FAIL;

    pIter = BlrAllocMem(sizeof(*pIter));
    if (pIter == NULL)
        FAIL;

    pIter->pTree = pTree;       // remember the tree
    pIter->pCurrNode = NULL;    // need to call first or next next.


    return(pIter);
Fail:
    return(NULL);
}

/////////////////////////////////////////////////////////////////
void BlMapIterDelete(BLMAP_ITERATOR hIterator)
//
// Delete an iterator created by BlMapIterNew
//
// inputs:
//     hBlMap -- pointer to a map created by BlMapNew.
// outputs:
//     returns -- newly created iterator.
//
////////////////////////////////////////////////////////////////
{
    BL_RED_BLK_ITERATOR *pIter = (BL_RED_BLK_ITERATOR *)hIterator;
    if (pIter == NULL)
        FAIL;

    BlrFreeMem(pIter);                       // dump the memory.    
    
    return;
Fail:
    return;
}

///////////////////////////////////////////////////////////
void *BlMapFirst(BLMAP_ITERATOR hIterator,
                 void **pKey)
//
// Retrieve the first map key item pair.
//
// inputs:
//    hIterator -- iterator for this object.
//    pKey -- pointer to where to stuff the pionter to the
//            key value. NULL if we don't want a key returned.
//
// outputs:
//    returns -- pointer to the item or NULL if no more items.
//    *pKey -- pointer to key
//
////////////////////////////////////////////////////////////////////
{
    BL_RED_BLK_ITERATOR *pIter = (BL_RED_BLK_ITERATOR *)hIterator;
    BL_RED_BLK_NODE *pX;
    BL_RED_BLK_NODE *pNode = NULL;


    if (pIter == NULL)
        FAIL;

    pX = pIter->pTree->pRoot->pLeft;

    //
    // Go down the left side until we hit a sentinal.  
    // the one immediatly before it is the one we want.
    //
    while (pX != NULL) 
    {   
        pNode = pX;       // remmber second to last.
        pX = pX->pLeft;
    }

    pIter->pCurrNode = pNode;           // remmber this.
    if (pNode == NULL)
        FAIL;

    if (pKey)
        *pKey = pNode->pKey;
    return(pNode->pItem);
Fail:
    return(NULL);    
}
/////////////////////////////////////////////////////////////////
void *BlMapNext(BLMAP_ITERATOR hIterator,
                void **pKey)
//
// Retrieve the next map pair of keys and item.
//
// inputs:
//    hIterator -- iterator for this object.
//    pKey -- pointer to where to stuff the pionter to the
//            key value. NULL if we don't want a key returned.
//
// outputs:
//    returns -- pointer to the item or NULL if no more items.
//    *pKey -- pointer to key
//
////////////////////////////////////////////////////////////////////
{
    BL_RED_BLK_ITERATOR *pIter = (BL_RED_BLK_ITERATOR *)hIterator;
    BL_RED_BLK_TREE *pTree;  
    BL_RED_BLK_NODE *pNode = NULL;


    if (pIter == NULL)
        FAIL;

    if (pIter->pCurrNode == NULL)
        FAIL;

    pTree = pIter->pTree;

    pNode = BlMapNextNode(pIter->pTree,
                           pIter->pCurrNode);

    pIter->pCurrNode = pNode;
    if (pNode == NULL)
        FAIL;

    if (pKey)
        *pKey = pNode->pKey;
    return(pNode->pItem);
Fail:
    return(NULL);
}
///////////////////////////////////////////////////////////////////
unsigned long BlMapSize(BLMAP_HANDLE hBlMap)
//
// Retrieve the number of items in a the item map.
//
// inputs:
//     hBlMap -- pointer to a map created by BlMapNew.
// output:
//     returns -- the total number of items in the map.
//
/////////////////////////////////////////////////////////////////
{
    BL_RED_BLK_TREE *pTree = BlMapGetTree(hBlMap);  
    if (pTree == NULL)
        FAIL;

    return(pTree->ulNumNodes);
Fail:
    return(0);
}

////////////////////////////////////////////////
void BlMapDeleteTreeContents(BL_RED_BLK_TREE *pTree,
                             BL_RED_BLK_NODE *pNode)
//
// Delete the contents of a red black tree. 
//
// This is a recursive routine that allows us to delete 
// the entire contents of a red black tree.   This deletes
// the contents, and leaves the root intact.
//
//
// inputs:
//    pTree -- pointer to the tree to delete
{
    if (pNode != NULL)
    {
        BlMapDeleteTreeContents(pTree, pNode->pLeft);       // do the recursion thing.
        BlMapDeleteTreeContents(pTree, pNode->pRight);
        if (pTree->pfnDelKeyAndItem)      // do we have a callback to delete
        {                               // the item and key...
            (*pTree->pfnDelKeyAndItem)(pNode->pKey,       // yes, do it.
                                     pNode->pItem,
                                     pTree->pUserData);
        }
    }

}




///////////////////////////////////////////////////////////
void BlMapLeftRotate(BL_RED_BLK_TREE* pTree UNUSED,
                     BL_RED_BLK_NODE* pX) 
//
// This takes a tree so that it can access the appropriate 
// root and nil pointers, and the node to rotate on. 
// 
// Rotates as described in _Introduction_To_Algorithms by 
// Cormen, Leiserson, Rivest (Chapter 14).  Basically this 
// makes the parent of x be to the left of x, x the parent of 
// its parent before the rotation and fixes other pointers 
// accordingly. 
//
// inputs:
//    pTree -- pointer to tree which we are rotating.
//    pX -- node around which we are rotating.
// outputs:
//    none.
//
{
    BL_RED_BLK_NODE* pY;


    pY=pX->pRight;
    pX->pRight = pY->pLeft;

    if (pY->pLeft != NULL) 
        pY->pLeft->pParent=pX; 

    pY->pParent=pX->pParent;   

    // instead of checking if pX->pParent is the root as in the book, we 
    // count on the root sentinel to implicitly take care of this case 
    if (pX == pX->pParent->pLeft) 
        pX->pParent->pLeft=pY;
    else 
        pX->pParent->pRight=pY;

    pY->pLeft=pX;
    pX->pParent=pY;

}

///////////////////////////////////////////////////////////
void BlMapRightRotate(BL_RED_BLK_TREE* pTree UNUSED, 
                     BL_RED_BLK_NODE* pY) 
//
// This takes a tree so that it can access the appropriate 
// root and nil pointers, and the node to rotate on. 
// 
// Rotates as described in _Introduction_To_Algorithms by 
// Cormen, Leiserson, Rivest (Chapter 14).  Basically this 
// makes the parent of x be to the left of x, x the parent of 
// its parent before the rotation and fixes other pointers 
// accordingly. 
//
// inputs:
//    pTree -- pointer to tree which we are rotating.
//    pX -- node around which we are rotating.
// outputs:
//    none.
{
    BL_RED_BLK_NODE* pX;

    pX=pY->pLeft;
    pY->pLeft=pX->pRight;

    if (NULL != pX->pRight)  
        pX->pRight->pParent=pY; 

    pX->pParent=pY->pParent;
    if( pY == pY->pParent->pLeft) 
        pY->pParent->pLeft=pX;
    else 
        pY->pParent->pRight=pX;

    pX->pRight=pY;
    pY->pParent=pX;

}

////////////////////////////////////////////////////////////////
BL_RED_BLK_NODE *BlMapFindNode(BL_RED_BLK_TREE *pTree,
                               void *pKey)
//
// Find a the tree node corresponding to this key.
//
// inputs:
//     pTree -- pointer to the tree.
//     pKey -- key pointer (passed to pfnCompare for key comparisions).
// outputs:
//     returns -- pointer the node
//
///////////////////////////////////////////////////////////////////
{
    BL_RED_BLK_NODE *pX;    
    PFN_BLMAP_COMPARE pfnCompare;
    int nRet;

    pX = pTree->pRoot->pLeft;           // start on the left
    pfnCompare = pTree->pfnCompare;

    if (pX == NULL)                      // nothing here, punt...
        FAIL;
    nRet = (*pfnCompare)(pX->pKey, pKey);  // do the first one to prime the pump
    while (nRet != 0)                       // until we find it or punt.
    {
        if (nRet > 0)           // pX->key > pKey
            pX = pX->pLeft;     
        else
            pX = pX->pRight;
        if (pX == NULL)
            FAIL;
        nRet = (*pfnCompare)(pX->pKey, pKey);  // do the next one...
    }
    return(pX);
Fail:
    return(NULL);
}


//////////////////////////////////////////////////////////////
void BlMapInsertNode(BL_RED_BLK_TREE *pTree, 
                     BL_RED_BLK_NODE *pNode)
//
// Insert a node into the tree....
//
// inputs:
//   pTree -- the tree to insert this silly node into.
//   pNode -- the node to insert.
// outputs:
//   none.
{
    BL_RED_BLK_NODE *pX;
    BL_RED_BLK_NODE *pY;
    PFN_BLMAP_COMPARE pfnCompare;

    

    pNode->pLeft = pNode->pRight = NULL;
    pY = pTree->pRoot;
    pX = pTree->pRoot->pLeft;

    pfnCompare = pTree->pfnCompare;

    while( pX != NULL) 
    {
        pY = pX;
        if ((*pfnCompare)(pX->pKey,pNode->pKey) > 0)  /* x.key > pNode.key */
            pX = pX->pLeft;
        else
            pX = pX->pRight;
    }
    pNode->pParent=pY;

    // yKey > zKey.  
    if ((pY == pTree->pRoot) || ((*pfnCompare)(pY->pKey,pNode->pKey) > 0))  
        pY->pLeft = pNode;
    else
        pY->pRight=pNode;

    pTree->ulNumNodes++;
}


//////////////////////////////////////////////////////////////////////
void BlMapRemoveNode(BL_RED_BLK_TREE *pTree, 
                     BL_RED_BLK_NODE *pNode)
//
// Remove the indicated node from the tree.
//
// inputs:
//    pTree -- tree to remove the node from.
//    pNode -- node to remove.
// outputs:
//    returns -- none.  Hey, what could go wrong.
//
{
    BL_RED_BLK_NODE *pY;
    BL_RED_BLK_NODE *pX;
    BL_RED_BLK_NODE *pRoot = pTree->pRoot;

    if ((pNode->pLeft == NULL) || (pNode->pRight == NULL))
        pY = pNode;
    else
    {
        // find the tree successor with a nil node as a child.
        pY = pNode->pRight;
        while (pY->pLeft != NULL)
            pY = pY->pLeft;
    }

    if (pY->pLeft == NULL)
        pX = pY->pRight;
    else
        pX = pY->pLeft;

    if (pX != NULL)
        pX->pParent = pY->pParent;
    if (pRoot == pY->pParent) 
    { 
        pRoot->pLeft=pX;
    } 
    else 
    {
        if (pY == pY->pParent->pLeft) 
            pY->pParent->pLeft = pX;
        else 
            pY->pParent->pRight=pX;
    }
    ASSERT(pY != NULL);
    if (pY != pNode)  /* pY should not be nil in this case */
    {

        ASSERT( (pY!=NULL)); //"pY is nil in RBDelete\n");
        /* pY is the node to splice out and pX is its child */

        //
        // do we need to re-balance....
        if ((!(pY->bIsRed))  && (pX != NULL))
            BlMapFixUp(pTree,pX);

        if (pTree->pfnDelKeyAndItem)      // do we have a callback to delete
        {                               // the item and key...
            (*pTree->pfnDelKeyAndItem)(pNode->pKey,       // yes, do it.
                                     pNode->pItem,
                                     pTree->pUserData);
        }  
        pY->pLeft = pNode->pLeft;
        pY->pRight = pNode->pRight;
        pY->pParent = pNode->pParent;
        pY->bIsRed = pNode->bIsRed;
        if (pNode->pLeft)
            pNode->pLeft->pParent =pY;
        if (pNode->pRight)
            pNode->pRight->pParent = pY;
        if (pNode == pNode->pParent->pLeft) 
            pNode->pParent->pLeft=pY; 
        else 
            pNode->pParent->pRight=pY;
        BlrFreeMem(pNode); 
    } 
    else 
    {
        if (pTree->pfnDelKeyAndItem)      // do we have a callback to delete
        {                               // the item and key...
            (*pTree->pfnDelKeyAndItem)(pY->pKey,       // yes, do it.
                                     pY->pItem,
                                     pTree->pUserData);
        }  
        if ((!(pY->bIsRed))  && (pX != NULL))
            BlMapFixUp(pTree,pX);
        BlrFreeMem(pY);
    }


    pTree->ulNumNodes--;
}    


//////////////////////////////////////////////////////////////////////
void BlMapFixUp(BL_RED_BLK_TREE *pTree, 
                BL_RED_BLK_NODE *pNode)
//
// Performs rotations and changes colors to restore red-black 
// input:
//    pTree -- tree is the tree to fix and 
//    pNode -- is the child of the spliced 
//             out node in RBTreeDelete.
//
// ouputs:
//   none.
{
    BL_RED_BLK_NODE *pRoot = pTree->pRoot->pLeft;
    BL_RED_BLK_NODE *pW;

    while( (!pNode->bIsRed) && (pRoot!= pNode)) 
    {
        if (pNode == pNode->pParent->pLeft) 
        {
            pW=pNode->pParent->pRight;
            if (pW == NULL)
            {
                pNode = pNode->pParent;
                continue;
            }
            if (pW->bIsRed) 
            {
                pW->bIsRed = FALSE;;
                pNode->pParent->bIsRed=1;
                BlMapLeftRotate(pTree,pNode->pParent);
                pW=pNode->pParent->pRight;
                if (pW == NULL)
                {
                    pNode = pNode->pParent;
                    continue;
                }
            }
            if ( (!IS_RED(pW->pRight)) && (!IS_RED(pW->pLeft)) ) 
            { 
                pW->bIsRed=1;
                pNode=pNode->pParent;
            } 
            else 
            {
                if (!IS_RED(pW->pRight)) 
                {
                    if (pW->pLeft)
                        pW->pLeft->bIsRed = FALSE;;
                    pW->bIsRed=1;
                    BlMapRightRotate(pTree,pW);
                    pW=pNode->pParent->pRight;
                }
                pW->bIsRed=pNode->pParent->bIsRed;
                pNode->pParent->bIsRed = FALSE;;
                if (pW->pRight)
                    pW->pRight->bIsRed = FALSE;;
                BlMapLeftRotate(pTree,pNode->pParent);
                pNode=pRoot; /* this is to exit while loop */
            }
        } 
        else 
        { /* the code below is has left and right switched from above */
            pW=pNode->pParent->pLeft;
            if (pW == NULL)
            {
                pNode = pNode->pParent;
                continue;
            }
            if (pW->bIsRed)
            {
                pW->bIsRed = FALSE;;
                pNode->pParent->bIsRed=1;
                BlMapRightRotate(pTree,pNode->pParent);
                pW=pNode->pParent->pLeft;
                if (pW == NULL)
                {
                    pNode = pNode->pParent;
                    continue;
                }
            }
            if ( (!IS_RED(pW->pRight)) && (!IS_RED(pW->pLeft)) ) 
            { 
                pW->bIsRed=1;
                pNode=pNode->pParent;
            } 
            else 
            {
                if (!IS_RED(pW->pLeft)) 
                {
                    if (pW->pRight)
                        pW->pRight->bIsRed = FALSE;;
                    pW->bIsRed=1;
                    BlMapLeftRotate(pTree,pW);
                    pW=pNode->pParent->pLeft;
                }
                pW->bIsRed=pNode->pParent->bIsRed;
                pNode->pParent->bIsRed = FALSE;;
                if (pW->pLeft)
                    pW->pLeft->bIsRed = FALSE;;
                BlMapRightRotate(pTree,pNode->pParent);
                pNode=pRoot; /* this is to exit pWhile loop */
            }
        }
    }
    pNode->bIsRed = FALSE;;

}
////////////////////////////////////////////////////////////////////
BL_RED_BLK_NODE *BlMapNextNode(BL_RED_BLK_TREE *pTree, 
                               BL_RED_BLK_NODE *pNode)
//
// given a starting node, return the next node.
//
// inputs:
//    pTree -- tree this node is a part of.
//    pNode
// outputs:
//    returns -- pointer to the next node.
//               if == pTree->nil then this is the end of the line.
//
{
    BL_RED_BLK_NODE *pY;
    BL_RED_BLK_NODE *pRoot = pTree->pRoot;

    pY = pNode->pRight;
    if (NULL != pY) 
    {     
        while(pY->pLeft != NULL) // returns the minium of the right subtree of pNode
            pY=pY->pLeft;
        return(pY);
    } 
    else 
    {
        pY=pNode->pParent;
        while(pNode == pY->pRight) // sentinel used instead of checking for nil 
        { 
            pNode=pY;
            pY = pY->pParent;        // work up the tree.
        }
        if (pY == pRoot) 
            return(NULL);
        return(pY);
    }
}


