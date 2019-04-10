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
 #ifndef TRAJECTORYPROPERTIES_HPP
#define TRAJECTORYPROPERTIES_HPP

#include <istream>
#include <ostream>
#include <BlueMatter/BoundingBox.hpp>

using namespace std;

class TrajectoryProperties
{
public:
    int          m_InitialStep;
    double       m_InitialSimulationTimeInNanoSeconds;
    double       m_TimeStepInPicoSeconds;
    int          m_TimeStep;
    // AK Change: Commented 
    //int          m_NAtoms;
    //int          m_NSteps;
    int 	m_Reverse;
    // AK Change: Add Declaration 
    //		types made consistent with whats in ExternalDatagram.hpp
    int		 m_SimulationStartStep;
    unsigned int m_SnapshotPeriodInOTS;
    double       m_OuterTimeStepInPicoSeconds;

    BoundingBox m_InitialBox;
    float m_BoxWidth;
    bool m_MapIntoBox;

    TrajectoryProperties()
    {
  init();
    }

    void init()
    {
  m_InitialStep = 0;
  m_InitialSimulationTimeInNanoSeconds = 0;
  m_TimeStepInPicoSeconds = 1.0;
  m_TimeStep = 0;
  //m_NAtoms = 0;
  //m_NSteps = 0;
  m_Reverse = 0;
  // AK Change: Initialization
        m_SimulationStartStep = 0;
        m_InitialBox.mMinBox = XYZ::ZERO_VALUE();
        m_InitialBox.mMaxBox = XYZ::ZERO_VALUE();
  m_MapIntoBox = false;
    }

    bool getMapIntoBox() { 
  return m_MapIntoBox;
    }

    void setMapIntoBox(bool b) { 
  m_MapIntoBox=b;
    }

};

#endif
