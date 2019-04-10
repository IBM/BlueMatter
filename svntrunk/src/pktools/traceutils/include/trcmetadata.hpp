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
 // *************************************************************************
// File: trcmetadata.hpp
// Author: RSG
// Date: January 14, 2005
// Description: Class encapsulating metadata extracted from trace
//              stream and methods to parse trace stream to extract
//              that data.
// Namespace: trc
// *************************************************************************
#ifndef INCLUDE_TRC_TRCMETADATA_HPP
#define INCLUDE_TRC_TRCMETADATA_HPP

#include <iostream>
#include <string>
#include <vector>

namespace trc
{
  class TraceMetaData;
}

std::ostream& operator<<(std::ostream&, const trc::TraceMetaData&);

namespace trc
{
  class TraceMetaData
  {
    friend std::ostream& ::operator<<(std::ostream&, const TraceMetaData&);
  private:
    int d_sourceId;
    int d_p_x;
    int d_p_y;
    int d_p_z;
    int d_fft_x;
    int d_fft_y;
    int d_fft_z;
    int d_fft_step_x;
    int d_fft_step_y;
    int d_fft_step_z;
    int d_vox_x;
    int d_vox_y;
    int d_vox_z;
    bool d_seenOne;
  public:
    TraceMetaData();
    int parse(const char*);
  };
}

#endif
