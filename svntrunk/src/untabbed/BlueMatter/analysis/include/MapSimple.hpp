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
 #ifndef MAPSIMPLE_H
#define MAPSIMPLE_H

#include <iostream>
#include <iomanip>
// mKeyWrap is the number of slots
// First key filled is head and tail.  New slots filled within wrap/2 are well defined in terms of head or tail.
// Head and tail can move forward and backward.  Head/tail gap region must be greater than 4.

template <class Type>
class MapSimple
{
private:
  int mNumItems;
  int mKeyWrap;
  int mHeadKey;
  int mTailKey;
  int mGapSize;
  enum {MinGap = 4};

  // signed dist within +/- wrap/2
  // both values in range [0, mKeyWrap)
  int dist(int a, int b)
  {
      int d = b-a;
      int ad = abs(d);
      int w = mKeyWrap/2;
      if (ad < w)
    return d;
      if (d < 0)
    return d + mKeyWrap;
      else
    return d - mKeyWrap;
  }

  // If n is a new head or tail, adjust accordingly
  // mNumItems should be correct
  void setHeadTail(int n)
  {
      // cerr << "sht0 " << n << " " << mHeadKey << " " << mTailKey << " " << mGapSize << " " << mNumItems << endl;
      if (mNumItems < 2) {
    if (mNumItems == 0) {
        mTailKey = mHeadKey;
        mGapSize = mKeyWrap;
        return;
    }
    mHeadKey = n;
    mTailKey = n;
    mGapSize = mKeyWrap - 1;
      }
      int dh = dist(mHeadKey, n);
      int dt = dist(mTailKey, n);
      // if closer to head than tail
      if (abs(dh) < abs(dt)) {
    // set head and gap
    mHeadKey = n;
      } else {
    // set tail and gap
    mTailKey = n;
      }
      mGapSize = mKeyWrap - dist(mHeadKey, mTailKey) - 1;
      // cerr << "sht1 " << mHeadKey << " " << mTailKey << " " << mGapSize << " " << mNumItems << endl;
  }

  // remove an item and adjust head/tail accordingly if needed
  // item should already have been removed from list
  void removeHeadTail(int n)
  {
      if (n == mHeadKey) {
    int bestk = mKeyWrap;
    int delta = mKeyWrap;
    ListElement *ple = mList;
    while (ple) {
        // find next key behind head
        int d = dist(mHeadKey, ple->mKey);
        if (d > 0 && d < delta) {
      bestk = ple->mKey;
      delta = d;
        }
        ple = ple->mNext;
    }
    mHeadKey = bestk;
    setHeadTail(bestk);
      }
      if (n == mTailKey) {
    int bestk = mKeyWrap;
    int delta = mKeyWrap;
    ListElement *ple = mList;
    while (ple) {
        // find next key in front of tail
        int d = dist(ple->mKey, mTailKey);
        if (d > 0 && d < delta) {
      bestk = ple->mKey;
      delta = d;
        }
        ple = ple->mNext;
    }
    mTailKey = bestk;
    setHeadTail(bestk);
      }
      return;
  }

  class ListElement
  {
  public:
    int mKey;
    Type mItem;
    ListElement *mNext;
    ListElement *mPrev;
    ListElement(int Key = -1) : mKey(Key), mNext(NULL), mPrev(NULL), mItem(Key) {}
  };

  ListElement *mList;

  Type& addEnd(const int Key)
  {
    // cout << "MapAddEnd " << Key << endl;
    if (!mList) {
      mList = new ListElement(Key);
      mNumItems = 1;
      setHeadTail(Key);
      return mList->mItem;
    }

    ListElement *ple = mList;
    while (ple->mNext)
      ple = ple->mNext;
    ple->mNext = new ListElement(Key);
    ple->mNext->mPrev = ple;
    mNumItems++;
    setHeadTail(Key);
    return ple->mNext->mItem;
  }

  // The following assumes you have walked the list
  // and found an element that is too big without
  // finding the element you want
  Type& addBefore(ListElement *TooBig, const int Key)
  {
    // cout << "MapAddBefore " << Key << endl;
    if (!mList || !TooBig) {
      mList = new ListElement(Key);
      mNumItems = 1;
      setHeadTail(Key);
      return mList->mItem;
    }

    ListElement *ple = TooBig->mPrev;
    ListElement *pNewElement = new ListElement(Key);
    if (ple) {
      ple->mNext = pNewElement;
      TooBig->mPrev = pNewElement;
      pNewElement->mPrev = ple;
      pNewElement->mNext = TooBig;
    } else {
      ple = mList;
      mList = pNewElement;
      ple->mPrev = pNewElement;
      pNewElement->mNext = TooBig;
    }

    mNumItems++;
    setHeadTail(Key);
    return pNewElement->mItem;
  }


public:
  MapSimple(int wrap=256) : mNumItems(0), mList(NULL), mKeyWrap(wrap), mHeadKey(0), mTailKey(0), mGapSize(wrap) {}

  // get the element corresponding to the key
  // create a new one if it doesn't exist
  Type& operator[] (const int Key) {
    // cout << "Map access Key, nitems " << Key << " " << mNumItems << endl;
    ListElement *ple = mList;
    while (ple && (ple->mKey < Key))
      ple = ple->mNext;
    if (!ple)
      return addEnd(Key);
    if (ple->mKey == Key)
      return ple->mItem;
    return addBefore(ple, Key);
  }

  void deleteItem(const int Key)
  {
    // cerr << "MapDeleteItem key head " << Key << " " << mHeadKey << endl;
    ListElement *ple = mList;
    if (ple->mKey == Key) 
        mList = ple->mNext;
    while (ple && (ple->mKey < Key))
      ple = ple->mNext;
    if (!ple || ple->mKey != Key) {
      cerr << "Could not find key to delete: " << Key << endl;
      exit(-1);
      return;
    }
    if (ple->mPrev)
      ple->mPrev->mNext = ple->mNext;
    if (ple->mNext)
      ple->mNext->mPrev = ple->mPrev;
    delete ple;
    mNumItems--;
    if (!mNumItems)
      mList = NULL;
    removeHeadTail(Key);
  }

  Type *getOrdinal(int n)
  {
    if (n < 0 || n >= mNumItems)
      return NULL;
    ListElement *ple = mList;
    for (int i=0; ple && i<n; i++)
      ple = ple->mNext;
    if (ple)
      return &ple->mItem;
    else
      return NULL;
  }

  void getItemAndKey(int n, Type *&t, int &key)
  {
    if (n < 0 || n >= mNumItems) {
      t = NULL;
      key = 0;
    }
    ListElement *ple = mList;
    for (int i=0; ple && i<n; i++)
      ple = ple->mNext;
    if (ple) {
      t =  &ple->mItem;
      key = ple->mKey;
    } else {
      t = NULL;
      key = 0;
    }
  }

  inline int numItems()
  {
    return mNumItems;
  }

  inline int overstuffed()
  {
      // cerr << "overstuff gapsize " << mGapSize << endl;
      return mGapSize < MinGap;
  }

  // delta specifies wrap
  inline int isHead(int k)
  {
      // cerr << "IsHead: head, query, numItems: " << mHeadKey << " " << k << " " << mNumItems << endl;
      return (k == mHeadKey) ? 1 : 0;
  }

  Type *getHead()
  {
      if (mNumItems == 0)
    return NULL;
      ListElement *ple = mList;
      if (mNumItems == 1)
    return &ple->mItem;
      while (ple && (ple->mKey < mHeadKey))
    ple = ple->mNext;
      if (ple->mKey != mHeadKey) {
    cerr << "Head key not found.  Key, numitems: " << mHeadKey << " " << mNumItems << endl;
    exit(-1);
      }
      return &ple->mItem;
  }

        ~MapSimple()
        {
            ListElement *ple = mList;
            while (ple) {
                ListElement *p = ple->mNext;
                delete ple;
                ple = p;
            }
            mNumItems = 0;
            mList = NULL;
        }

};

#endif

