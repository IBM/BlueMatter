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
 #ifndef EXTERNALDATAGRAM_HELPER_HPP
#define EXTERNALDATAGRAM_HELPER_HPP

#include <BlueMatter/RandomNumberGenerator.hpp>

#define SetChain(pkt, POrV, ChainId, Index, SitesPerChain)              \
    if (Index < SitesPerChain)                                          \
        a ## POrV ## ChainId[Index] = pkt .m ## POrV ## ChainId ## Index;

#define ChainValue(POrV, ChainId, Index, SitesPerChain) \
    ((Index < SitesPerChain) ? POrV ## ChainId[Index] : 0.0)

#ifndef NO_PK_PLATFORM
inline void ED_Emit_DynamicVariablesTwoChainFragmentThermostat(
    const int aOuterTimeStep,
    const int aFragmentNumber,
    const int aVoxelId,
    const unsigned short aNumberOfSitesPerChain,
    const double *aPosition0,
    const double *aVelocity0,
    const double *aPosition1,
    const double *aVelocity1
    )
{

    ED_Emit_DynamicVariablesTwoChainFragmentThermostat(
  aOuterTimeStep,
  aFragmentNumber,
  aVoxelId,
  aNumberOfSitesPerChain,
  ChainValue(aPosition, 0, 0, aNumberOfSitesPerChain),
  ChainValue(aPosition, 0, 1, aNumberOfSitesPerChain),
  ChainValue(aPosition, 0, 2, aNumberOfSitesPerChain),
  ChainValue(aPosition, 0, 3, aNumberOfSitesPerChain),
  ChainValue(aPosition, 0, 4, aNumberOfSitesPerChain),
  ChainValue(aVelocity, 0, 0, aNumberOfSitesPerChain),
  ChainValue(aVelocity, 0, 1, aNumberOfSitesPerChain),
  ChainValue(aVelocity, 0, 2, aNumberOfSitesPerChain),
  ChainValue(aVelocity, 0, 3, aNumberOfSitesPerChain),
  ChainValue(aVelocity, 0, 4, aNumberOfSitesPerChain),
  ChainValue(aPosition, 1, 0, aNumberOfSitesPerChain),
  ChainValue(aPosition, 1, 1, aNumberOfSitesPerChain),
  ChainValue(aPosition, 1, 2, aNumberOfSitesPerChain),
  ChainValue(aPosition, 1, 3, aNumberOfSitesPerChain),
  ChainValue(aPosition, 1, 4, aNumberOfSitesPerChain),
  ChainValue(aVelocity, 1, 0, aNumberOfSitesPerChain),
  ChainValue(aVelocity, 1, 1, aNumberOfSitesPerChain),
  ChainValue(aVelocity, 1, 2, aNumberOfSitesPerChain),
  ChainValue(aVelocity, 1, 3, aNumberOfSitesPerChain),
  ChainValue(aVelocity, 1, 4, aNumberOfSitesPerChain)
    );
}
#endif

inline void ED_Open_DynamicVariablesTwoChainFragmentThermostat(
    char *&p,
    int &aFragmentNumber,
    int &aVoxelId,
    const unsigned short aNumberOfSitesPerChain,  // NOTE THIS IS CONST INPUT UNLIKE OTHERS! 
    double *aPosition0,
    double *aVelocity0,
    double *aPosition1,
    double *aVelocity1
    )
{
    ED_DynamicVariablesTwoChainFragmentThermostat ed_pkt;

    ED_OpenPacket_DynamicVariablesTwoChainFragmentThermostat(p, ed_pkt);
    aFragmentNumber = ed_pkt.mFragmentNumber;
    aVoxelId = ed_pkt.mVoxelId;
    assert(aNumberOfSitesPerChain == ed_pkt.mNumberOfSitesPerChain);
    SetChain(ed_pkt, Position, 0, 0, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Position, 0, 1, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Position, 0, 2, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Position, 0, 3, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Position, 0, 4, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Velocity, 0, 0, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Velocity, 0, 1, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Velocity, 0, 2, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Velocity, 0, 3, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Velocity, 0, 4, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Position, 1, 0, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Position, 1, 1, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Position, 1, 2, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Position, 1, 3, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Position, 1, 4, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Velocity, 1, 0, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Velocity, 1, 1, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Velocity, 1, 2, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Velocity, 1, 3, aNumberOfSitesPerChain);
    SetChain(ed_pkt, Velocity, 1, 4, aNumberOfSitesPerChain);
}


#define SetOneChain(pkt, POrV, Index, SitesPerChain)              \
    if (Index < SitesPerChain)                                          \
        a ## POrV[Index] = pkt .m ## POrV ## Index;

#define OneChainValue(POrV, Index, SitesPerChain) \
    ((Index < SitesPerChain) ? POrV[Index] : 0.0)


#ifndef NO_PK_PLATFORM
inline void ED_Emit_DynamicVariablesOneChainThermostat(
    const int aOuterTimeStep,
    const int aVoxelId,
    const unsigned short aNumberOfSitesPerChain,
    const double *aPosition,
    const double *aVelocity
    )
{

    ED_Emit_DynamicVariablesOneChainThermostat(
  aOuterTimeStep,
  aVoxelId,
        aNumberOfSitesPerChain,
  OneChainValue(aPosition, 0, aNumberOfSitesPerChain),
  OneChainValue(aPosition, 1, aNumberOfSitesPerChain),
  OneChainValue(aPosition, 2, aNumberOfSitesPerChain),
  OneChainValue(aPosition, 3, aNumberOfSitesPerChain),
  OneChainValue(aPosition, 4, aNumberOfSitesPerChain),
  OneChainValue(aVelocity, 0, aNumberOfSitesPerChain),
  OneChainValue(aVelocity, 1, aNumberOfSitesPerChain),
  OneChainValue(aVelocity, 2, aNumberOfSitesPerChain),
  OneChainValue(aVelocity, 3, aNumberOfSitesPerChain),
  OneChainValue(aVelocity, 4, aNumberOfSitesPerChain)
    );
}
#endif

inline void ED_Open_DynamicVariablesOneChainThermostat(
    char *&p,
    int &aVoxelId,
    const unsigned short aNumberOfSitesPerChain,  // NOTE THIS IS CONST INPUT UNLIKE OTHERS! 
    double *aPosition,
    double *aVelocity
    )
{
    ED_DynamicVariablesOneChainThermostat ed_pkt;

    ED_OpenPacket_DynamicVariablesOneChainThermostat(p, ed_pkt);
    aVoxelId = ed_pkt.mVoxelId;
    assert(aNumberOfSitesPerChain == ed_pkt.mNumberOfSitesPerChain);
    SetOneChain(ed_pkt, Position, 0, aNumberOfSitesPerChain);
    SetOneChain(ed_pkt, Position, 1, aNumberOfSitesPerChain);
    SetOneChain(ed_pkt, Position, 2, aNumberOfSitesPerChain);
    SetOneChain(ed_pkt, Position, 3, aNumberOfSitesPerChain);
    SetOneChain(ed_pkt, Position, 4, aNumberOfSitesPerChain);
    SetOneChain(ed_pkt, Velocity, 0, aNumberOfSitesPerChain);
    SetOneChain(ed_pkt, Velocity, 1, aNumberOfSitesPerChain);
    SetOneChain(ed_pkt, Velocity, 2, aNumberOfSitesPerChain);
    SetOneChain(ed_pkt, Velocity, 3, aNumberOfSitesPerChain);
    SetOneChain(ed_pkt, Velocity, 4, aNumberOfSitesPerChain);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_DynamicVariablesRandomState_Helper( const int aOuterTimeStep, RandomNumberState& aRNS )
{
    ED_Emit_DynamicVariablesRandomState( aOuterTimeStep,
                                         0,
                                         Platform::Topology::GetAddressSpaceId(),
                                         0,
                                         aRNS.mIset,
                                         aRNS.mGset,
                                         aRNS.mSeed,
                                         aRNS.mIdum2,
                                         aRNS.mIy,
                                         aRNS.mIv[ 0 ], aRNS.mIv[ 1 ], aRNS.mIv[ 2 ], aRNS.mIv[ 3 ],
                                         aRNS.mIv[ 4 ], aRNS.mIv[ 5 ], aRNS.mIv[ 6 ], aRNS.mIv[ 7 ],
                                         aRNS.mIv[ 8 ], aRNS.mIv[ 9 ], aRNS.mIv[ 10 ], aRNS.mIv[ 11 ],
                                         aRNS.mIv[ 12 ], aRNS.mIv[ 13 ], aRNS.mIv[ 14 ], aRNS.mIv[ 15 ],
                                         aRNS.mIv[ 16 ], aRNS.mIv[ 17 ], aRNS.mIv[ 18 ], aRNS.mIv[ 19 ],
                                         aRNS.mIv[ 20 ], aRNS.mIv[ 21 ], aRNS.mIv[ 22 ], aRNS.mIv[ 23 ],
                                         aRNS.mIv[ 24 ], aRNS.mIv[ 25 ], aRNS.mIv[ 26 ], aRNS.mIv[ 27 ],
                                         aRNS.mIv[ 28 ], aRNS.mIv[ 29 ], aRNS.mIv[ 30 ], aRNS.mIv[ 31 ] );
}
#endif



#endif
