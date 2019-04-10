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
 #ifndef ARRAYGROWABLE_H
#define ARRAYGROWABLE_H

template <class Type>
class ArrayGrowable  
{
private:
  enum {InitialSize = 1024};
  int mSize;
  Type *mArray;
  int *mSetArray;
  int mNumSet;
  int mMaxIndex;

  // grow to accomodate new index
  // allow shrink
  void grow(const int NewIndex, const int Exact=0) {
    int NewSize;
    if (Exact)
      NewSize = NewIndex + 1;
    else
      NewSize = int(NewIndex/mSize)*2*mSize;

    Type *tmp = new Type[NewSize];
    memcpy(tmp, mArray, mSize*sizeof(Type));
    delete [] mArray;
    mArray = tmp;

    int *tmpSet = new int[NewSize];
    memset(tmpSet, 0, NewSize*sizeof(int));

    // make shrink safe
    int MaxCopy = (mSize < NewSize ? mSize : NewSize);
    memcpy(tmpSet, mSetArray, MaxCopy*sizeof(int));
    delete [] mSetArray;
    mSetArray = tmpSet;

    mSize = NewSize;
  }

public:
  ArrayGrowable(const int sz = InitialSize) : mSize(sz) {
    mArray = new Type[mSize];
    mSetArray = new int[mSize];
    clear();
  }
  
  ~ArrayGrowable() {
    if (mArray)
      delete [] mArray;
    if (mSetArray)
      delete [] mSetArray;
    mSize = mNumSet = 0;
    mMaxIndex = -1;
  }

  // resize to exact number of elements
  void resize(const int NewSize) {
    grow(NewSize-1, 1);
  }

  // grow if needed
  // overwrite previous values without warning, but keep good count of set values
  inline void set(const int index, const Type &e) {
    if (index >= mSize)
      grow(index);
    if (!mSetArray[index]) {
      mSetArray[index] = 1;
      if (index > mMaxIndex)
        mMaxIndex = index;
      mNumSet++;
    }
    mArray[index] = e;
  }

  // grow if needed
  // if value previously set, bail and return nonzero
  // Thus nonzero return means attempt to overwrite existing value
  inline int setIfNew(const int index, const Type &e) {
    int collide = 0;
    if (index >= mSize)
      grow(index);
    if (!mSetArray[index]) {
      mSetArray[index] = 1;
      if (index > mMaxIndex)
        mMaxIndex = index;
      mNumSet++;
    } else {
      collide = 1;
    }
    mArray[index] = e;
    return collide;
  }

  // grow if needed
  // if value previously set, return 1 if different value
  // otherwise ignore request
  // Thus nonzero return means attempt to overwrite previously set value
  inline int setCheck(const int index, const Type &e) {
    if (index >= mSize)
      grow(index);
    if (!mSetArray[index]) {
      mSetArray[index] = 1;
      if (index > mMaxIndex)
        mMaxIndex = index;
      mNumSet++;
    } else if (mArray[index].mPosition == e.mPosition && mArray[index].mVelocity == e.mVelocity) {
      return 0;
    } else {
      return 1;
    }

    mArray[index] = e;
    return 0;
  }

  // quick return of element without check for existence
  inline Type& operator[] (const int index) {
    return mArray[index];
  }

        ArrayGrowable<Type>& operator=( const ArrayGrowable<Type> &from)
        {
            mSize = from.mSize;
            mNumSet = from.mNumSet;
            mMaxIndex = from.mMaxIndex;
            mArray = new Type[mSize];
            memcpy(mArray, from.mArray, mSize*sizeof(Type));
            mSetArray = new int[mSize];
            memcpy(mSetArray, from.mSetArray, mSize*sizeof(int));
            return *this;
        }

  inline Type *getArray() {
    return mArray;
  }

  // get number of elements from 0 to mMaxIndex
  inline int getSize() {
    return mMaxIndex + 1;
  }

  inline int isSet(const int index) {
    return mSetArray[index];
  }

  // Have all values from 0 to mMaxIndex-1 been set?
  // Empty array has no gaps
  inline int hasGaps() {
    // cerr << "has gaps " << mNumSet << " " << mMaxIndex << endl << flush;
    return (((mNumSet-mMaxIndex) == 1) ? 0 : 1);
  }

  inline int numSet() {
    return mNumSet;
  }

  // clear contents but don't release memory
  inline void clear() {
    mNumSet = 0;
    mMaxIndex = -1;
    memset(mSetArray, 0, mSize*sizeof(int));
  }

  // release memory
  inline void free() {
    mNumSet = 0;
    mMaxIndex = -1;
    delete [] mArray;
    mArray = NULL;
    delete [] mSetArray;
    mSetArray = NULL;
  }
};

#endif
