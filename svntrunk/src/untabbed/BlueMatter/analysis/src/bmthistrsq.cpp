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
 // ******************************************************************
// File: bmthistrsq.cpp
// Author: RSG
// Date: December 10, 2005
// Description: Creates histograms of the distance squared between
//              particle positions as a function of time (within a
//              time separation window defined on the command line)
//
// Command line arguments:  bmtName (input bmt file or stencil)
//                          window width (in snapshot periods)
//                          binWidth (in angstrom^2)
//                          estimatedBinCount
// ******************************************************************

#include <BlueMatter/BMT.hpp>
#include <BlueMatter/XYZ.hpp>

#include <iostream>
#include <vector>
#include  <cstdlib>

int main(int argc, char** argv)
{
  char* bmtName=NULL;
  int width = 0;
  double binWidth = 0;
  int binCount = 128;
  switch(argc)
    {
    case 5:
      bmtName = argv[1];
      width = std::atoi(argv[2]);
      binWidth = std::atof(argv[3]);
      binCount = std::atoi(argv[4]);
      break;
    default:
      std::cerr << argv[0] << " bmtName "
    << "windowWidthInSnapshotPeriods "
    << "binWidthInSqrAngstroms "
    << "estimatedBinCount "
    << std::endl;
      std::exit(-1);
    }

  // set up histogram data structures
  std::vector<std::vector<int> > histogram(width);
  for (std::vector<std::vector<int> >::iterator iter = histogram.begin();
       iter != histogram.end();
       ++iter)
    {
      iter->resize(binCount);
    }

  BMTReader bmt(bmtName);
  int natom = bmt.getNAtoms();
  // create circular buffer
  int buffCount = width+1;
  std::vector<std::vector<XYZ> > buffer(buffCount);
  for (std::vector<std::vector<XYZ> >::iterator iter = buffer.begin();
       iter != buffer.end();
       ++iter)
    {
      iter->resize(natom);
    }
  int head = -1;
  int nelts = 0;
  int nframe = -1;
  double invBinWidth = 1.0/binWidth;
  unsigned step = 0;
  unsigned firstStep = 0;
  unsigned lastStep = 0;
  int readerr = 0;
  while(!(readerr = bmt.ReadFrame()))
    {
      head = (head + 1) % buffCount;
      ++nframe;
      std::cerr << "frame:" << nframe << std::endl;
      bmt.getFrameAttrib("Step",&step);
      if (nframe == 0)
  {
    firstStep = step;
  }
      buffer[head].assign(bmt.getPositions(), bmt.getPositions()+natom);
      nelts = (nelts + 1) % buffCount;
      const std::vector<XYZ>& headArray = buffer[head];
      for (int diff = 1; diff < nelts; ++diff)
  {
    const std::vector<XYZ>& other = buffer[(head - diff + buffCount) % buffCount];
    for (int i = 0; i < buffer[head].size(); ++i)
      {
        
        int bin = headArray[i].DistanceSquared(other[i])*invBinWidth;
        if (bin >= histogram[diff-1].size())
    {
      histogram[diff-1].resize(1.25*bin+1);
    }
        if (bin > 256)
    {
      std::cerr << "incrementing histogram[" << diff-1
          << "][" << bin << "]" <<std::endl;
    }
        ++histogram[diff-1][bin];
      }
  }
    }
  lastStep = step;
  // now write it out for gnuplot (or whatever)
  std::cout << "# Histogram of squared distances traveled from "
      << bmtName << std::endl;
  std::cout << "# Window width: " << width << " snapshot intervals"
      << std::endl;
  std::cout << "# Number frames: " << nframe << std::endl;
  std::cout << "# Starting snapshot: " << firstStep << std::endl;
  std::cout << "# Ending snapshot " << lastStep << std::endl;
  std::cout << "# Histogram bin width: " << binWidth << std::endl;

  for (int i = 0; i < width; ++i)
    {
      std::cout << "# Index " << i << " for diff = " << i + 1 << std::endl;
      for (std::vector<int>::const_iterator iter = histogram[i].begin();
     iter != histogram[i].end();
     ++iter)
  {
    std::cout << (iter - histogram[i].begin()) << " \t"
        << *iter << std::endl;
  }
      std::cout << "\n\n" << std::endl;
    }
  return(0);
}
