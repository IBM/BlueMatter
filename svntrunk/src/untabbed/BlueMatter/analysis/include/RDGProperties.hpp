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
 #ifndef RDGPROPERTIES_HPP
#define RDGPROPERTIES_HPP

#include <cstdio>
#include <iostream>
#include <ostream>
#include <BlueMatter/TrajectoryProperties.hpp>
#include <BlueMatter/BoundingBox.hpp>
#include <BlueMatter/XYZ.hpp>

using namespace std;

// This class captures properties of the original rdg
class RDGProperties
{
public:
    enum {MAXNUMENERGIES=64};
    enum {MAXENERGYLABEL=128};

    // AK: Comment: Is this member required here?
    TrajectoryProperties  m_TrajectoryProperties;
    unsigned int          m_ExecutableID;
    // AK: type made consistent with whats in ExternalDatagram.hpp
    unsigned int          m_SourceID;

    // AK: In TrajectoryProperties
    //BoundingBox  m_InitialBox;

    bool         m_HasDynamicBox;
    int          m_NEnergies;
    char         m_EnergyLabels[MAXNUMENERGIES][MAXENERGYLABEL];
    double       m_Energies[MAXNUMENERGIES];

    RDGProperties()
    {
  init();
    }

    void init()
    {
  m_TrajectoryProperties.init();
  m_ExecutableID = 0;
  m_SourceID = 0;
        // AK: In TrajectoryProperties
  //m_InitialBox.mMinBox = XYZ::ZERO_VALUE();
  //m_InitialBox.mMaxBox = XYZ::ZERO_VALUE();
  m_HasDynamicBox = false;
  m_NEnergies = 0;
    }
};
#endif
