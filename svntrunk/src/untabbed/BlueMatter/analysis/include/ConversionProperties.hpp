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
 #ifndef CONVERSIONPROPERTIES_HPP
#define CONVERSIONPROPERTIES_HPP

// frequently used ones
#define TRAJ16BIT  "16-bit RESCALE"
#define TRAJ32BIT  "32-bit POSITIONS"
#define TRAJ64BIT  "64-bit POSITIONS"

// add ons
#define TRAJRECENTER "RECENTER"
#define TRAJVELOCITIES "VELOCITIES"

using namespace std ;

class ConversionProperties
{
public:
  // centering specifies how to map into box and remove imaging
  // none does no mapping, others map relevant object into center of box and pack rest around it.
  enum CENTERINGTYPE {NOCENTERING, SIMPLE, ALLATOM, PROTEIN, NONWATER, FRAGMENTLIST};
  enum REDUCTIONTYPE {SHORT, FLOAT, DOUBLE};
  enum STARTTYPE {FIRST, SKIPFIRST, TARGETINTEGERTIME, TARGETREALTIME};
  enum SKIPTYPE  {NOSKIP, FACTOR, TARGETINTEGERSTEP, TARGETREALSTEP};

  REDUCTIONTYPE m_ReductionType;
  STARTTYPE     m_StartType;
  SKIPTYPE      m_SkipType;

  int           m_SkipFactor;
  double        m_TargetTimeStepInPicoSeconds;
  int           m_TargetStartStep;
  double        m_TargetStartTimeInNanoSeconds;

  CENTERINGTYPE m_Centering;
  bool          m_UseMassToCenter;
  bool          m_UseHeavyAtoms;
  bool          m_UseHysteresis;
  double        m_HysteresisDistance;

  // Note that some of the following are dependent on the contents 
  // of an input BMT, if converting from BMT to BMT.  Energies 
  // cannot be included if they aren't in the input bmt.
  bool          m_IncludeEnergies;
  bool          m_IncludePositions;
  bool          m_IncludeVelocities;
  bool          m_IncludeWaters;

  // AK: new variable to store the trajectoryproperties string
  string	      m_TRAJECTORYFORMATstring;

  REDUCTIONTYPE determineReductionType() {
          if(m_TRAJECTORYFORMATstring.find("32")!=string::npos) {
                  m_ReductionType=FLOAT;
          }
          else if(m_TRAJECTORYFORMATstring.find("64")!=string::npos) {
                  m_ReductionType=DOUBLE;
          }
          else m_ReductionType=SHORT;

    return m_ReductionType;
  }

  int SizeOf(REDUCTIONTYPE rt) {
    switch(rt) {
      case SHORT: return sizeof(short); 
      case FLOAT: return sizeof(float); 
      case DOUBLE: return sizeof(double); 
      default: return 0;
    }
  }

  bool determineIncludeVelocities() {
          if(m_TRAJECTORYFORMATstring.find("VELOCITIES")!=string::npos) 
      m_IncludeVelocities = true;
    else m_IncludeVelocities = false;
    return m_IncludeVelocities;
  }

};


#endif
