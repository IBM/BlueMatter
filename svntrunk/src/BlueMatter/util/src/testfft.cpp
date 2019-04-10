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
 // ************************************************************************
// File: testfft.cpp
// Author: RSG
// Date: January 1, 2004
// Description: test drivers for fft utility routines.  These routines
//              use the infrastructure provided by the utilities under
//              cbc/utils
// ************************************************************************


#include <iostream>
#include <cstdlib>

#include <cbc/driverregistry.hpp>
#include <BlueMatter/fftlayout.hpp>
#include <BlueMatter/xyz2ziterator.hpp>

class TestFFTLayout
{
public:
  // data
  static cbc::DriverRegistry::Register<TestFFTLayout> s_register;
  // methods
  static int main(int, char**);
};

cbc::DriverRegistry::Register<TestFFTLayout> TestFFTLayout::s_register;

typedef fft::triple<int> TripleInt;

struct TestElt
{
  // data
  TripleInt d_coord;
  // operators
  inline bool operator==(const TestElt& other) const
  {
    return((d_coord.first == other.d_coord.first) &&
	   (d_coord.second == other.d_coord.second) &&
	   (d_coord.third == other.d_coord.third));
  }
  
  inline bool operator!=(const TestElt& other) const
  {
    return((d_coord.first != other.d_coord.first) ||
	   (d_coord.second != other.d_coord.second) ||
	   (d_coord.third != other.d_coord.third));
  }
};

std::ostream& operator<<(std::ostream&, const TestElt&);

std::ostream& operator<<(std::ostream& os, const TestElt& elt)
{
  os << elt.d_coord.first << " "
     << elt.d_coord.second << " "
     << elt.d_coord.third;
  return os;
}

int TestFFTLayout::main(int argc, char** argv)
{
  bool verbose = false;
  int px = 0;
  int py = 0;
  switch(argc)
    {
    case 4:
      verbose = true;
    case 3:
      px = std::atoi(argv[1]);
      py = std::atoi(argv[2]);
      break;
    default:
      std::cerr << argv[0] << " px py (for test) <verbose=on>" << std::endl;
      return(-1);
    }

  // ********************************************************************
  // ********************************************************************
  // The following enums are the only ones required to change the
  // dimensions of the FFT mesh (NX,NY,NZ) 
  // or the processor mesh (PX,PY,PZ):
  // ********************************************************************
  // ********************************************************************

  enum {NX = 192, NY = 192, NZ = 192};
  enum {PX = 16, PY = 16, PZ = 16};

  if ((px < 0 ) || (px >= PX) || (py < 0) || (py >= PY))
    {
      std::cerr << "Illegal values of px and/or py, max(px) = " << PX
		<< " max(py) = " << PY << std::endl;
      std::exit(-1);
    }
  typedef fft::FFTMesh<NX,NY,NZ> fftMesh;
  typedef fft::ProcMesh<PX,PY,PZ> procMesh;
  typedef fft::SubVolume<TestElt, fftMesh, procMesh> SubVolume;
  typedef fft::XYZ2ZRouter<SubVolume> XYZ2ZRouter;
  typedef fft::XYZ2ZIterator<SubVolume> XYZ2ZIterator;

  SubVolume* startVol =
    new SubVolume[procMesh::PZ];
  assert(startVol != NULL);
  if (startVol != NULL)
    {
      if(verbose)
	{
	  std::cerr << "startVol array allocated successfully" << std::endl;
	}
    }
  else
    {
      std::cerr << "startVol array allocation failed!!!" << std::endl;
      std::exit(-1);
    }

  // set up the initial mesh values for all nodes in the selected column
  //  TripleInt defaultDest;
  //  defaultDest.first = -1;
  //  defaultDest.second = -1;
  //  defaultDest.third = -1;
  int offset = 0;
  for (int pz = 0; pz < procMesh::PZ; ++pz)
    {
      for (int x = 0; x < SubVolume::DNX; ++x)
	{
	  for (int y = 0; y < SubVolume::DNY; ++y)
	    {
	      for (int z = 0; z < SubVolume::DNZ; ++z)
		{
		  TripleInt foo;
		  foo.first = x + px*SubVolume::DNX;
		  foo.second = y + py*SubVolume::DNY;
		  foo.third = z + pz*SubVolume::DNZ;
		  startVol[offset].d_xyz[x][y][z].d_coord = foo;
		  //		startVol[offset].d_xyz[x][y][z].d_dest = defaultDest;
		}
	    }
	}
      ++offset;
    }

  if (verbose)
    {
      std::cerr << "Initialization of startVols completed" << std::endl;
    }

  // now set up routers for this column of subvolumes (defined
  // by px and py)

  XYZ2ZRouter* router[procMesh::PZ];

  for (int pz = 0; pz < procMesh::PZ; ++pz)
    {
      router[pz] =
	new XYZ2ZRouter(startVol[pz]);
      assert(router[pz] != NULL);
      if (router[pz] != NULL)
	{
	  if (verbose)
	    {
	      std::cerr << "XYZ2ZRouter[" << pz << "] allocation successful"
			<< std::endl;
	    }
	}
      else
	{
	  std::cerr << "XYZ2ZRouter[" << pz << "] allocation FAILED!!" << std::endl;
	  std::exit(-1);
	}
    }

  // set up destination subvolumes
  SubVolume* destVol = new SubVolume[procMesh::PZ];
  assert(destVol != NULL);
  if (destVol != NULL)
    {
      if(verbose)
	{
	  std::cerr << "destVol array allocated successfully" << std::endl;
	}
    }
  else
    {
      std::cerr << "destVol array allocation failed!!!" << std::endl;
      std::exit(-1);
    }

  int errorCount = 0;
  if (verbose)
    {
      std::cerr << "About to start loading up destination subvolumes"
		<< std::endl;
    }
  // load up destination subvolumes with data from starting subvolumes
  // and carry out Stage Ia tests
  for (int startZ = 0; startZ < procMesh::PZ; ++startZ)
    {
      if (verbose)
	{
	  std::cerr << "Starting loads from starting volume "
		    << startZ << std::endl;
	}
      for (int destZ = 0; destZ < procMesh::PZ; ++destZ)
	{
	  if (verbose)
	    {
	      std::cerr << "starting to load destination " << destZ << std::endl;
	    }
	  int count = 0;
	  for (XYZ2ZIterator iter(router[startZ]->begin(destZ));
	       iter != router[startZ]->end(destZ);
	       ++iter)
	    {
	      ++count;
	      int dxy = iter.dy() + iter.dx()*SubVolume::DNY;
	      int x = iter.dx() + px*SubVolume::DNX;
	      int y = iter.dy() + py*SubVolume::DNY;
	      int z = iter.dz() + startZ*SubVolume::DNZ;
	      TestElt& elt = destVol[destZ].d_Zxy[dxy - destZ*SubVolume::DXY][z];
	      elt.d_coord.first = (*iter).d_coord.first;
	      elt.d_coord.second = (*iter).d_coord.second;
	      elt.d_coord.third = (*iter).d_coord.third;

	      if (verbose)
		{
		  std::cerr << "source: (" << px 
			    << ", " << py << ", " << startZ << ") "
			    << "dest: (" << px << ", " << py << ", "
			    << destZ << ") "
			    << "(dx, dy, dz): (" << iter.dx() << ", "
			    << iter.dy() << ", " << iter.dz() << ") "
			    << "dxy: " << dxy << " (x,y,z): ("
			    << x << ", " << y << ", " << z << ") "
			    << "*iter: " << *iter << " elt: "
			    << elt << std::endl;
		}
	      // test for agreement with element values
	      if ((elt.d_coord.first != x) || 
		  (elt.d_coord.second != y) || 
		  (elt.d_coord.third != z))
		{
		  std::cerr << "px = " << px << " ";
		  std::cerr << "py = " << py << " ";
		  std::cerr << "startZ = " << startZ << " ";
		  std::cerr << "destZ = " << destZ << " ";
		  std::cerr << "(dx, dy, dz) = ("
			    << iter.dx() << ", "
			    << iter.dy() << ", "
			    << iter.dz() << ") ";
		  std::cerr << "elt(x, y, z) = ("
			    << x << ", " << y << ", " << z << ") ";
		  std::cerr << "NOT EQUAL TO router(x, y, z) = ("
			    << elt.d_coord.first << ", "
			    << elt.d_coord.second << ", "
			    << elt.d_coord.third << ")" << std::endl;
		  ++errorCount;
		}
	    }
	  if (verbose)
	    {
	      std::cerr << "finished loading destination " << destZ 
			<< " from starting volume " << startZ 
			<< " with " << count << " elements" << std::endl;
	    }
	}
    }

  std::cerr << "Stage Ia Test completed with error count = " << errorCount << std::endl;
  errorCount = 0; // reset error count for stage Ib test

  // Stage Ib tests involve stepping through the destination
  // subvolumes and checking that the layout makes sense

  for (int destZ = 0; destZ < procMesh::PZ; ++destZ)
    {
      for (int dxy = 0; dxy < SubVolume::DXY; ++dxy)
	{
	  std::pair<int,int> predxy = 
	    XYZ2ZIterator::xyFromOffset(px, py, destZ, dxy);

	  // check to see if predxy correspond to the
	  // coordinates in the test elements
	  TestElt trial;
	  trial.d_coord.first = predxy.first;
	  trial.d_coord.second = predxy.second;
	  trial.d_coord.third = 0;
	  for (int z = 0; z < SubVolume::fftmesh_type::NZ; ++z)
	    {
	      trial.d_coord.third = z;
	      if (destVol[destZ].d_Zxy[dxy][z] != trial)
		{
		  std::cerr << "destVol[" << destZ 
			    << "].d_Zxy[" << dxy
			    << "][" << z << "] = "
			    << destVol[destZ].d_Zxy[dxy][z]
			    << " NOT EQUAL to absolute coords: "
			    << trial << std::endl;
		  ++errorCount;
		}
	    }
	} // end of loop over dxy
    } // end of loop over destZ

  std::cerr << "Stage Ib Test completed with error count = " << errorCount << std::endl;


  // cleanup
  for (int pz = 0; pz < procMesh::PZ; ++pz)
    {
      delete router[pz];
    }
  delete [] startVol;
  return(0);
}

