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
 // *********************************************************************
// File: xyz2ziterator.hpp
// Author: RSG
// Date: July 19, 2003
// Class: XYZ2ZIterator
// Description: Class encapsulating iteration over portions of
//              subvolume contained on a single node that are 
//              destined for a particular processor z-coordinate.
// *********************************************************************
#ifndef INCLUDE_FFT_XYZ2ZITERATOR_HPP
#define INCLUDE_FFT_XYZ2ZITERATOR_HPP

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <utility>

namespace fft
{
  template <class TSubVolume>
  class XYZ2ZRouter;

  template<class TSubVolume>
  class XYZ2ZIterator
  {
    friend class XYZ2ZRouter<TSubVolume>;
  public:
    typedef TSubVolume subVol_type;
    typedef typename TSubVolume::elt_type elt_type;
    typedef typename TSubVolume::fftmesh_type fftmesh_type;
    typedef typename TSubVolume::procmesh_type procmesh_type;
    enum {DNX = TSubVolume::DNX,
	  DNY = TSubVolume::DNY,
	  DNZ = TSubVolume::DNZ};

    enum {DXY = (DNX*DNY)/procmesh_type::PZ,
	  DXZ = (DNX*DNZ)/procmesh_type::PY,
	  DYZ = (DNY*DNZ)/procmesh_type::PX};

  private:
    elt_type (&d_xyz)[DNX][DNY][DNZ];
    int d_dx;
    int d_dy;
    int d_dz;
    int d_offset;
    inline XYZ2ZIterator<TSubVolume>& operator=(const XYZ2ZIterator<TSubVolume>&);
  public:
    inline XYZ2ZIterator(TSubVolume&);
    inline XYZ2ZIterator(const XYZ2ZIterator<TSubVolume>&);
    inline bool operator==(const  XYZ2ZIterator<TSubVolume>&) const;
    inline bool operator!=(const  XYZ2ZIterator<TSubVolume>&) const;
    inline elt_type& operator*();
    inline XYZ2ZIterator<TSubVolume>& operator++();
    inline int dx() const;
    inline int dy() const;
    inline int dz() const;
    inline bool isEndOfZ() const; // returns true if iterator is at
				  // last elt of current run of
				  // consecutive z vals 

    // once items are routed to appropriate destination nodes, we need
    // to be able decipher which x and y values have been placed where.
    static inline std::pair<int,int> xyFromOffset(int, int, int, int); // args are px, py, pz, dxy
      
  };

  template <class TSubVolume>
  std::pair<int,int> XYZ2ZIterator<TSubVolume>::xyFromOffset(int px, int py, int pz, int dxy)
  {
    std::pair<int, int> xy;
    int offset = TSubVolume::DXY*pz + dxy;
    xy.second = offset % TSubVolume::DNY;
    xy.first = ((offset - xy.second)/TSubVolume::DNY) + px*TSubVolume::DNX;
    xy.second = xy.second + py*TSubVolume::DNY; // convert to absolute y
    return xy;
  }
  
  template <class TSubVolume>
  class XYZ2ZRouter
  {
  private:
    TSubVolume& d_subV;
    XYZ2ZIterator<TSubVolume>* d_begin[TSubVolume::procmesh_type::PZ + 1];
  public:
    XYZ2ZRouter(TSubVolume&);
    inline TSubVolume& subVolume();
    // the integer arg is the target pz node coordinate (0 <= pz < PZ)
    inline const XYZ2ZIterator<TSubVolume>& begin(int) const;
    inline const XYZ2ZIterator<TSubVolume>& end(int) const;
  };

  template <class TSubVolume>
  TSubVolume& XYZ2ZRouter<TSubVolume>::subVolume()
  {
    return d_subV;
  }

  template <class TSubVolume>
  XYZ2ZRouter<TSubVolume>::XYZ2ZRouter(TSubVolume& subV) : d_subV(subV)
  {

    for (int i = 0; i <= TSubVolume::procmesh_type::PZ; ++i)
      {
	d_begin[i] = new XYZ2ZIterator<TSubVolume>(subV);
	assert(d_begin[i] != NULL);
	if (d_begin[i] == NULL)
	  {
	    std::cerr << "Unable to allocate memory for XYZ2ZIterator" << std::endl;
	    std::exit(-1);
	  }
      }
    d_begin[0]->d_dz = 0;
    d_begin[0]->d_dy = 0;
    d_begin[0]->d_dx = 0;
    d_begin[0]->d_offset = 0;

    int offset = TSubVolume::DXY*TSubVolume::DNZ;
    
    for (int i = 1; i <= TSubVolume::procmesh_type::PZ; ++i)
      {
	d_begin[i]->d_offset = offset;
	d_begin[i]->d_dz = offset % TSubVolume::DNZ;
	int off1 = (offset - d_begin[i]->d_dz)/TSubVolume::DNZ;
	d_begin[i]->d_dy = off1 % TSubVolume::DNY;
	d_begin[i]->d_dx = (off1 - d_begin[i]->d_dy)/TSubVolume::DNY;
	offset = offset + TSubVolume::DXY*TSubVolume::DNZ;
      }
  }

  template <class TSubVolume>
  const XYZ2ZIterator<TSubVolume>& XYZ2ZRouter<TSubVolume>::begin(int idx) const
  {
    assert((idx >= 0) && (idx < TSubVolume::procmesh_type::PZ));
    return *d_begin[idx];
  }

  template <class TSubVolume>
  const XYZ2ZIterator<TSubVolume>& XYZ2ZRouter<TSubVolume>::end(int idx) const
  {
    assert((idx >= 0) && (idx < TSubVolume::procmesh_type::PZ));
    return *d_begin[idx + 1];
  }

  template<class TSubVolume>
  XYZ2ZIterator<TSubVolume>::XYZ2ZIterator(TSubVolume& sVol) :
      d_xyz(sVol.d_xyz),
      d_dx(-1),
      d_dy(-1),
      d_dz(-1),
      d_offset(-1)
  {
  }

  template<class TSubVolume>
  XYZ2ZIterator<TSubVolume>::XYZ2ZIterator(const XYZ2ZIterator<TSubVolume>& proto)
    :
    d_xyz(proto.d_xyz),
    d_dx(proto.d_dx),
    d_dy(proto.d_dy),
    d_dz(proto.d_dz),
    d_offset(proto.d_offset)
  {
  }

  template<class TSubVolume>
  XYZ2ZIterator<TSubVolume>&
  XYZ2ZIterator<TSubVolume>::operator=(const XYZ2ZIterator<TSubVolume>& other)
      {
	return *this;
      }

  template<class TSubVolume>
  bool
  XYZ2ZIterator<TSubVolume>::operator==(const XYZ2ZIterator<TSubVolume>& other) const
  {
    return((&d_xyz == &other.d_xyz) &&
	   (d_offset == other.d_offset));
  }

  template<class TSubVolume>
  bool
  XYZ2ZIterator<TSubVolume>::operator!=(const XYZ2ZIterator<TSubVolume>& other) const
  {
    return(!(*this == other));
  }

  template<class TSubVolume>
  typename XYZ2ZIterator<TSubVolume>::elt_type&
  XYZ2ZIterator<TSubVolume>::operator*()
  {
    return d_xyz[d_dx][d_dy][d_dz];
  }
  
  template<class TSubVolume>
    int XYZ2ZIterator<TSubVolume>::dx() const
    {
      return(d_dx);
    }
  
  template<class TSubVolume>
    int XYZ2ZIterator<TSubVolume>::dy() const
    {
      return(d_dy);
    }
  
  template<class TSubVolume>
    int XYZ2ZIterator<TSubVolume>::dz() const
    {
      return(d_dz);
    }
  
  template<class TSubVolume>
    XYZ2ZIterator<TSubVolume>&
    XYZ2ZIterator<TSubVolume>::operator++()
      {
	d_dz = (++d_dz) % DNZ;
	if (d_dz == 0)
	  {
	    d_dy = (++d_dy) % DNY;
	    if (d_dy == 0)
	      {
		d_dx = (++d_dx) % DNX;
	      }
	  }
	++d_offset;
	return *this;
      }
  
  
  template<class TSubVolume>
    bool XYZ2ZIterator<TSubVolume>::isEndOfZ() const
    {
      return ((d_dz + 1) % DNZ == 0);
    }
  
  
}

#endif
