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
 // *******************************************************************
// File: fftlayout.hpp
// Author: RSG
// Date: July 11, 2003
// Class: FFTLayout
// Description:  Class encapsulating per node storage and addressing
//               for a distributed memory implementation of a 3D-FFT
// *******************************************************************

#ifndef INCLUDE_FFTLAYOUT_HPP
#define INCLUDE_FFTLAYOUT_HPP

// putting in this compile-time assertion declaration here for now
// Taken from http://www.panelsoft.com/murphyslaw/apr01.htm
#define CASSERT(ex) { typedef char cassert_type[(ex) ? 1 : -1]; }

#include <fft/ffttypes.hpp>

namespace fft
{
  // for now, we will assume the following (and check using a
  // compile-time assert):
  // NX % PX = 0
  // NY % PY = 0
  // NZ % PZ = 0
  // DNX*DNY % PZ = 0
  // DNX*DNZ % PY = 0
  // DNY*DNZ % PX = 0

  template <int xdim, int ydim, int zdim>
    struct FFTMesh
    {
      enum {NX = xdim, NY = ydim, NZ = zdim};
    };

  template <int xproc, int yproc, int zproc>
    struct ProcMesh
    {
      enum {PX = xproc, PY = yproc, PZ = zproc};
    };

  // TElt is most likely complex or std::pair<double>
  template<class TElt, class TMesh, class TProc>
    union SubVolume
    {
      typedef TElt elt_type;
      typedef TMesh fftmesh_type;
      typedef TProc procmesh_type;

      enum {DNX = TMesh::NX/TProc::PX,
	  DNY = TMesh::NY/TProc::PY,
	  DNZ = TMesh::NZ/TProc::PZ};

      enum {DXY = (DNX*DNY)/TProc::PZ,
	  DXZ = (DNX*DNZ)/TProc::PY,
	  DYZ = (DNY*DNZ)/TProc::PX};



      // Data members (sharing storage)
      TElt d_xyz[DNX][DNY][DNZ]; // starting volume decomposition mesh
      TElt d_Zxy[DXY][TMesh::NZ]; // when a transform is done on Z/kz
      TElt d_Yxz[DXZ][TMesh::NY]; // when a transform is done on Y/ky
      TElt d_Xyz[DYZ][TMesh::NX]; // when a transform is done on X/kx
      //      TElt d_ZYX[DX][DY][NZ];
      //      TElt d_ZXY[DY][DX][NZ];
      //      TElt D_YZX[DX][DZ][NY];
      //      TElt D_YXZ[DZ][DX][NY];
      //      TElt D_XYZ[DZ][DY][NX];
      //      TElt D_XZY[DY][DZ][NX];

      // might need to add more data members to the union

      SubVolume()
	{
	  // do the compile-time checks
	  CASSERT( TMesh::NX % TProc::PX == 0 );
	  CASSERT( TMesh::NY % TProc::PY == 0 );
	  CASSERT( TMesh::NZ % TProc::PZ == 0 );
	  CASSERT( (DNX*DNY) % TProc::PZ == 0 );
	  CASSERT( (DNX*DNZ) % TProc::PY == 0 );
	  CASSERT( (DNY*DNZ) % TProc::PX== 0 );
	}

    };

#if 0
  template <class TEngine>
    class FFTLayout
    {
    public:
      // typedefs
      typedef TEngine::SubVolume SubVolume;
      typedef TEngine::XYZToZIterator XYZToZIterator;
      typedef TEngine::StridedZIterator StridedZIterator;
      typedef TEngine::StridedYIterator StridedYIterator;
      typedef TEngine::StridedXIterator StridedXIterator;

       // methods
      inline TEngine& asEngine();
      inline const TEngine& asEngine() const;
      inline SubVolume& subVolume();
      inline const SubVolume& subVolume() const;

      // For first phase of forward transform, to set up packets for
      // transform along z
      inline const XYZToZIterator& xyzToTransformZ_begin(int pz);

      inline const XYZToZIterator& xyzToTransformZ_end(int pz);


      // for second phase, to set up packets for transform along y
      // after transform along z
      inline const StridedZIterator& transformZToTransformY_begin(int py,
								  int pz);

      inline const StridedZIterator& transformZToTransformY_end(int py,
								int pz);

      // for third phase, to set up packets for transform along x
      // after transform along y
      inline const StridedYIterator& transformYToTransformX_begin(int px,
								  int py);

      inline const StridedYIterator& transformYToTransformX_end(int px,
								int py);

      // for first phase of reverse transform (possibly after
      // in-place convolution), to set up packets for invtransform along y (ky)
      // after inverse transform along x (kx)
      inline const StridedXIterator& transformXToTransformY_begin(int px,
								  int py);

      inline const StridedXIterator& transformXToTransformY_end(int py,
								int px);

      // for second phase of reverse transform, to set up packets for
      // invtransform along z (kz) after inverse transform along y (ky)
      inline const StridedYIterator& transformYToTransformZ_begin(int py,
								  int pz);

      inline const StridedYIterator& transformYToTransformZ_end(int py,
								int pz);
      // for final phase of reverse transform, to set up packets for
      // return of transformed data to "volume" distribution over kx,
      // ky, kz by communication along z axis only
      inline const ZToXYZIterator& transformZToXYZ_begin(int pz);

      inline const ZToXYZIterator& transformZToXYZ_end(int pz);
     };

  template <class TEngine>
  TEngine& FFTLayout<TEngine>::asEngine()
  {
    return static_cast<TEngine&>(*this);
  }

  template <class TEngine>
  const TEngine& FFTLayout<TEngine>::asEngine() const
  {
    return static_cast<const TEngine&>(*this);
  }

  template <class TEngine>
  const SubVolume& FFTLayout<TEngine>::subVolume() const
  {
    return asEngine().subVolume();
  }
  // For first phase of forward transform, to set up packets for
  // transform along z
  template <class TEngine>
  const XYZToZIterator& FFTLayout<TEngine>::xyzToTransformZ_begin(int pz)
  {
    return asEngine().xyzToTransformZ_begin(pz);
  }
  template <class TEngine>
  const XYZToZIterator& FFTLayout<TEngine>::xyzToTransformZ_end(int pz)
  {
    return asEngine().xyzToTransformZ_end(pz);
  }

  // for second phase, to set up packets for transform along y
  // after transform along z
  template <class TEngine>
  const StridedZIterator& FFTLayout<TEngine>::transformZToTransformY_begin(int py,
									   int pz)
  {
    return asEngine().transformZToTransformY_begin(py, pz);
  }
  template <class TEngine>
  const StridedZIterator& FFTLayout<TEngine>::transformZToTransformY_end(int py,
									 int pz)
  {
    return asEngine().transformZToTransformY_end(py, pz);
  }
  // for third phase, to set up packets for transform along x
  // after transform along y
  template <class TEngine>
  const StridedYIterator& FFTLayout<TEngine>::transformYToTransformX_begin(int px,
									   int py)
  {
    return asEngine().transformYToTransformX_begin(px, py);
  }
  template <class TEngine>
  const StridedYIterator& FFTLayout<TEngine>::transformYToTransformX_end(int px,
									 int py)
  {
    return asEngine().transformYToTransformX_end(px, py);
  }
  // for first phase of reverse transform (possibly after
  // in-place convolution), to set up packets for invtransform along y (ky)
  // after inverse transform along x (kx)
  template <class TEngine>
  const StridedXIterator& FFTLayout<TEngine>::transformXToTransformY_begin(int px,
									   int py)
  {
    return asEngine().transformXToTransformY_begin(px, py);
  }
  template <class TEngine>
  const StridedXIterator& FFTLayout<TEngine>::transformXToTransformY_end(int py,
									 int px)
  {
    return asEngine().transformXToTransformY_end(px, py);
  }
  // for second phase of reverse transform, to set up packets for
  // invtransform along z (kz) after inverse transform along y (ky)
  template <class TEngine>
  const StridedYIterator& FFTLayout<TEngine>::transformYToTransformZ_begin(int py,
									   int pz)
  {
    return asEngine().transformYToTransformZ_begin(py, pz);
  }
  template <class TEngine>
  const StridedYIterator& FFTLayout<TEngine>::transformYToTransformZ_end(int py,
									 int pz)
  {
    return asEngine().transformYToTransformZ_end(py, pz);
  }
  
  // for final phase of reverse transform, to set up packets for
  // return of transformed data to "volume" distribution over kx,
  // ky, kz by communication along z axis only

  template <class TEngine>
  const ZToXYZIterator& FFYLayout<TEngine>transformZToXYZ_begin(int pz)
  {
    return asEngine().transformZToXYZ_begin(pz);
  }

  template <class TEngine>
  const ZToXYZIterator& transformZToXYZ_end(int pz)
  {
    return asEngine().transformZToXYZ_end(pz);
  }  
#endif

}
#endif
