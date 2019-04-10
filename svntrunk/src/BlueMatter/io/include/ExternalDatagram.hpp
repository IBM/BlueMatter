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
 #ifndef EXTERNALDATAGRAMS_HPP
#define EXTERNALDATAGRAMS_HPP

// DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT
// DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT
// DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT  DO NOT EDIT
//
// Automatically generated file
// Created by BlueMatter/analysis/src/MakePacketStructures.pl
// Based on contents of BlueMatter/analysis/src/DatagramSpec.txt
//
// If changes are needed, please communicate them to the ExternalDatagram Control Authority

#ifndef NO_PK_PLATFORM
#include <pk/platform.hpp>
#include <BlueMatter/RunTimeGlobals.hpp>
#endif

#include <assert.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#define read _read
#endif

#include <fcntl.h>
// for memcpy?
#include <memory.h>
#include <BlueMatter/XYZ.hpp>

#ifndef BM_SWAP_IN
#define BM_SWAP_IN 0
#endif
#ifndef BM_SWAP_OUT
#define BM_SWAP_OUT 0
#endif
#ifndef BM_SWAP_INOUT
#define BM_SWAP_INOUT 0
#endif

// This collects calls to move packets to the BlueMatter Monitor and will later be moved.
#ifndef NO_PK_PLATFORM
extern void SendDatagramToMonitor(int fd, char *buff, int len );
#endif

// This is a stand-in for memcpy()
// We could write our own that did better things based on size, etc.
inline void *BM_memcpy(void *dest, void *src, size_t count)
{
    return memcpy(dest, src, count);
}

struct ArrivalStatus
{
    enum {
        UNKNOWN     =  0,
        EXPECTED    =  1,
        NOTEXPECTED =  2,
        ARRIVING    =  4,
        RECEIVED    =  8,
        SENT        = 16
    };
};


//*********************************************************************
//*** Header definition ****
//*********************************************************************
typedef enum
{
    ED_BITS_FOR_LEN   = 8,
    ED_BITS_FOR_CLASS = 8,
    ED_BITS_FOR_TYPE  = 8,
    ED_BITS_FOR_STEP  = 8
} ED_BITS;

//*********************************************************************
// Define the header
//*********************************************************************

typedef struct
{
    unsigned mLen            : ED_BITS_FOR_LEN   ;  // Length in bytes including the length.
    unsigned mClass          : ED_BITS_FOR_CLASS ;  // Class - e.g. dynamic variable
    unsigned mType           : ED_BITS_FOR_TYPE  ;  // Type of the packet - from above enum
    unsigned mShortTimeStep  : ED_BITS_FOR_STEP  ;  // Lower bits of current time step
} ED_Header;

#define DCL_ED_HEADER ED_Header mHeader

//NEED: A bit more thought but in principle, the datagram
//shall contain a format that is so unlikely to be randomly
//created that if found, can used to sync the record stream.
enum { eInvariantMagicNumber = 0xABCDEFED }; // To remain the same for all time.
enum { eVariableMagicNumber  = 0x01010001 }; // To be incremented when these formats change.

struct FrameContents
{
    enum {
        UDFs        =  1,
        Sites       =  2,
        Pressure    =  4,
        Constraints =  8,
        RTP         = 16,
	RandomState = 32,
	BirthBox    = 64
    };
};

class BM_PacketPrimitive
{
private:

    enum {
        Size_int            = 4,
        Size_unsigned_int   = 4,
        Size_unsigned_long  = 8,
        Size_float          = 4,
        Size_double         = 8,
        Size_short          = 2,
        Size_unsigned_short = 2
    };

    static inline void Pack_2(char *&p, const char *s)
    {
        if (BM_SWAP_OUT) {
            p[0] = s[1];
            p[1] = s[0];
        } else {
            BM_memcpy((void *)p, (void *)s, 2);
        }
        p += 2;
    }

    static inline void Pack_4(char *&p, const char *s)
    {
        if (BM_SWAP_OUT) {
            p[0] = s[3];
            p[1] = s[2];
            p[2] = s[1];
            p[3] = s[0];
        } else {
            BM_memcpy((void *)p, (void *)s, 4);
        }
        p += 4;
    }

    static inline void Pack_8(char *&p, const char *s)
    {
        if (BM_SWAP_OUT) {
            p[0] = s[7];
            p[1] = s[6];
            p[2] = s[5];
            p[3] = s[4];
            p[4] = s[3];
            p[5] = s[2];
            p[6] = s[1];
            p[7] = s[0];
        } else {
            BM_memcpy((void *)p, (void *)s, 8);
        }
        p += 8;
    }

    static inline void Unpack_2(char *&p, char *s)
    {
        if (BM_SWAP_IN) {
            s[0] = p[1];
            s[1] = p[0];
        } else {
            BM_memcpy((void *)s, (void *)p, 2);
        }
        p += 2;
    }

    static inline void Unpack_4(char *&p, char *s)
    {
        if (BM_SWAP_IN) {
            s[0] = p[3];
            s[1] = p[2];
            s[2] = p[1];
            s[3] = p[0];
        } else {
            BM_memcpy((void *)s, (void *)p, 4);
        }
        p += 4;
    }

    static inline void Unpack_8(char *&p, char *s)
    {
        if (BM_SWAP_IN) {
            s[0] = p[7];
            s[1] = p[6];
            s[2] = p[5];
            s[3] = p[4];
            s[4] = p[3];
            s[5] = p[2];
            s[6] = p[1];
            s[7] = p[0];
        } else {
            BM_memcpy((void *)s, (void *)p, 8);
        }
        p += 8;
    }

    static inline void Pack_4_NoSwap(char *&p, const char *s)
    {
        BM_memcpy((void *)p, (void *)s, 4);
        p += 4;
    }

    static inline void Unpack_4_NoSwap(char *&p, char *s)
    {
        BM_memcpy((void *)s, (void *)p, 4);
        p += 4;
    }


public:

    // Sanity call to make sure these hard-coded sizes are for real
    static void AssertIntegrity()
    {
        assert(sizeof(int)    == Size_int);
        assert(sizeof(float)  == Size_float);
        assert(sizeof(double) == Size_double);
        assert(sizeof(short)  == Size_short);
    }

    static inline void Pack(char *&p, const short &i)          { Pack_2  (p, (char *)&i); }

    static inline void Unpack(char *&p, short &i)              { Unpack_2(p, (char *)&i); }

    static inline void Pack(char *&p, const unsigned short &i) { Pack_2  (p, (char *)&i); }

    static inline void Unpack(char *&p, unsigned short &i)     { Unpack_2(p, (char *)&i); }

    static inline void Pack(char *&p, const int &i)            { Pack_4  (p, (char *)&i); }

    static inline void Unpack(char *&p, int &i)                { Unpack_4(p, (char *)&i); }

    static inline void Pack(char *&p, const unsigned int &i)   { Pack_4  (p, (char *)&i); }

    static inline void Unpack(char *&p, unsigned int &i)       { Unpack_4(p, (char *)&i); }

    static inline void Pack(char *&p, const unsigned long &i)  { Pack_8  (p, (char *)&i); }

    static inline void Unpack(char *&p, unsigned long &i)      { Unpack_8(p, (char *)&i); }

    static inline void Pack(char *&p, const float &f)          { Pack_4  (p, (char *)&f); }

    static inline void Unpack(char *&p, float &f)              { Unpack_4(p, (char *)&f); }

    static inline void Pack(char *&p, const double &d)         { Pack_8  (p, (char *)&d); }

    static inline void Unpack(char *&p, double &d)             { Unpack_8(p, (char *)&d); }

    static inline void Pack(char *&p, const ED_Header &edh)    { Pack_4_NoSwap  (p, (char *)&edh);}

    static inline void Unpack(char *&p, ED_Header &edh)        { Unpack_4_NoSwap(p, (char *)&edh);}

    // ========================================================================

    // packing for special user types
    // When new types are added to BlueMatter, a corresponding overloaded Pack/Unpack
    // should be created here.

    // Load contents of v into p and bump p
    static inline void Pack(char *&p, const XYZ &v)
    {
#ifndef NO_PK_PLATFORM
	XYZ tmp = v;
	tmp.ReOrderInverse(RTG.mBoxIndexOrder);
        Pack(p, tmp.mX);
        Pack(p, tmp.mY);
        Pack(p, tmp.mZ);
#else
        Pack(p, v.mX);
        Pack(p, v.mY);
        Pack(p, v.mZ);
#endif
    }

    static inline void Unpack(char *&p, XYZ &v)
    {
        Unpack(p, v.mX);
        Unpack(p, v.mY);
        Unpack(p, v.mZ);
    }
};

class TaggedPacket
{
public:
    enum {
        SPECIALTAG = 0xffff,
        TAGSIZE = 2
    };
    inline unsigned short GetTag(char *&p)
    {
        unsigned short tag;
        BM_PacketPrimitive::Unpack(p, tag);
        return tag;
    }
};

class TriggerActions
{
public:
    enum {
        GO = 0,
        STOP = 1,
        HALT = 2
    };
};



struct ED_CLASS {
enum {
    Unknown = 0,
    Control = 1,
    DynamicVariables = 2,
    Energy = 3,
    Information = 4,
    UDF = 5,
    RTPValues = 6
    };
};

struct ED_UNKNOWN_TYPES {
enum {
    Unknown = 0
    };
};

struct ED_CONTROL_TYPES {
enum {
    Unknown = 0,
    SyncId = 1,
    TheEnd = 2,
    Pressure = 3,
    TimeStamp = 4,
    Trigger = 5,
    PortAssignment = 6,
    ReplicaExchangePeriodInOTS = 7,
    ReplicaExchangeAttempt = 8
    };
};

struct ED_DYNAMICVARIABLES_TYPES {
enum {
    Unknown = 0,
    Site = 1,
    Force = 2,
    Momentum = 3,
    FloatSite = 4,
    BoundingBox = 5,
    ShortSite = 6,
    TwoChainFragmentThermostat = 7,
    OneChainThermostat = 8,
    RandomState = 9
    };
};

struct ED_ENERGY_TYPES {
enum {
    Unknown = 0,
    Bonded = 1,
    NonBonded = 2,
    Free = 3
    };
};

struct ED_INFORMATION_TYPES {
enum {
    Unknown = 0,
    MolecularSystemDefinition = 1,
    RunTimeConfiguration = 2,
    UDFCount = 3,
    FrameContents = 4,
    Constraints = 5,
    RTP = 6,
    Site = 7,
    Bond = 8
    };
};

struct ED_UDF_TYPES {
enum {
    Unknown = 0,
    d1 = 1,
    d2 = 2
    };
};

struct ED_RTPVALUES_TYPES {
enum {
    Unknown = 0,
    VelocityResampleTargetTemperature = 1,
    NumberOfOuterTimeSteps = 2,
    SnapshotPeriodInOTS = 3,
    EmitEnergyTimeStepModulo = 4,
    VelocityResampleRandomSeed = 5
    };
};

#define TOTALNUMPACKETS     35
#define MAXPACKEDPACKETSIZE 174
#define MAXPACKEDPACKETNAME DynamicVariablesTwoChainFragmentThermostat
#define MINPACKEDPACKETSIZE 8
#define MINPACKEDPACKETNAME ControlTheEnd
// Mean Packet Size = 43.7647058823529


//=============================================================================
//====        1      1 ControlSyncId                                       ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mIMC1;
    unsigned  mFullOuterTimeStep;
    unsigned  mIMC2;
    unsigned  mIMC3;
    unsigned  mCheckSum;
    enum {PackedSize = 24,
          PackedTaggedSize = 24+TaggedPacket::TAGSIZE};
} ED_ControlSyncId;

inline void ED_OpenPacket_ControlSyncId(char *&p, ED_ControlSyncId &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mIMC1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mFullOuterTimeStep);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mIMC2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mIMC3);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mCheckSum);
}

inline void ED_FillPacket_ControlSyncId(char *p, ED_ControlSyncId &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mIMC1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mFullOuterTimeStep);
    BM_PacketPrimitive::Pack(p, ed_pkt.mIMC2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mIMC3);
    BM_PacketPrimitive::Pack(p, ed_pkt.mCheckSum);
}

inline void ED_OpenPacket_ControlSyncId(
    char *&p,
    unsigned &aIMC1,
    unsigned &aFullOuterTimeStep,
    unsigned &aIMC2,
    unsigned &aIMC3,
    unsigned &aCheckSum
    )
{
    BM_PacketPrimitive::Unpack(p, aIMC1);
    BM_PacketPrimitive::Unpack(p, aFullOuterTimeStep);
    BM_PacketPrimitive::Unpack(p, aIMC2);
    BM_PacketPrimitive::Unpack(p, aIMC3);
    BM_PacketPrimitive::Unpack(p, aCheckSum);
}

inline void ED_FillPacket_ControlSyncId(
    char *p,
    const unsigned aIMC1,
    const unsigned aFullOuterTimeStep,
    const unsigned aIMC2,
    const unsigned aIMC3,
    const unsigned aCheckSum
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_ControlSyncId::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Control;
    ed_hdr.mType   = ED_CONTROL_TYPES::SyncId;
    ed_hdr.mShortTimeStep   = aFullOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aIMC1);
    BM_PacketPrimitive::Pack(p, aFullOuterTimeStep);
    BM_PacketPrimitive::Pack(p, aIMC2);
    BM_PacketPrimitive::Pack(p, aIMC3);
    BM_PacketPrimitive::Pack(p, aCheckSum);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_ControlSyncId(
    const unsigned aIMC1,
    const unsigned aFullOuterTimeStep,
    const unsigned aIMC2,
    const unsigned aIMC3,
    const unsigned aCheckSum
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_ControlSyncId::PackedSize];

    ED_FillPacket_ControlSyncId(
    buff,
    aIMC1,
    aFullOuterTimeStep,
    aIMC2,
    aIMC3,
    aCheckSum
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlSyncId::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlSyncId::PackedSize );
#endif
}

inline void ED_Emit_Tagged_ControlSyncId(
    const unsigned short aTag,
    const unsigned aIMC1,
    const unsigned aFullOuterTimeStep,
    const unsigned aIMC2,
    const unsigned aIMC3,
    const unsigned aCheckSum
    )
{
    char buff[ED_ControlSyncId::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_ControlSyncId(
    p,
    aIMC1,
    aFullOuterTimeStep,
    aIMC2,
    aIMC3,
    aCheckSum
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlSyncId::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlSyncId::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_ControlSyncId &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mIMC1 << " ";
    os << ed_pkt.mFullOuterTimeStep << " ";
    os << ed_pkt.mIMC2 << " ";
    os << ed_pkt.mIMC3 << " ";
    os << ed_pkt.mCheckSum;
    return (os);
}

//=============================================================================
//====        1      2 ControlTheEnd                                       ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mFullOuterTimeStep;
    enum {PackedSize = 8,
          PackedTaggedSize = 8+TaggedPacket::TAGSIZE};
} ED_ControlTheEnd;

inline void ED_OpenPacket_ControlTheEnd(char *&p, ED_ControlTheEnd &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mFullOuterTimeStep);
}

inline void ED_FillPacket_ControlTheEnd(char *p, ED_ControlTheEnd &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mFullOuterTimeStep);
}

inline void ED_OpenPacket_ControlTheEnd(
    char *&p,
    unsigned &aFullOuterTimeStep
    )
{
    BM_PacketPrimitive::Unpack(p, aFullOuterTimeStep);
}

inline void ED_FillPacket_ControlTheEnd(
    char *p,
    const unsigned aFullOuterTimeStep
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_ControlTheEnd::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Control;
    ed_hdr.mType   = ED_CONTROL_TYPES::TheEnd;
    ed_hdr.mShortTimeStep   = aFullOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aFullOuterTimeStep);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_ControlTheEnd(
    const unsigned aFullOuterTimeStep
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_ControlTheEnd::PackedSize];

    ED_FillPacket_ControlTheEnd(
    buff,
    aFullOuterTimeStep
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlTheEnd::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlTheEnd::PackedSize );
#endif
}

inline void ED_Emit_Tagged_ControlTheEnd(
    const unsigned short aTag,
    const unsigned aFullOuterTimeStep
    )
{
    char buff[ED_ControlTheEnd::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_ControlTheEnd(
    p,
    aFullOuterTimeStep
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlTheEnd::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlTheEnd::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_ControlTheEnd &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mFullOuterTimeStep;
    return (os);
}

//=============================================================================
//====        1      3 ControlPressure                                     ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mFullOuterTimeStep;
    XYZ  mVirial;
    XYZ  mVolumePosition;
    XYZ  mVolumeVelocity;
    XYZ  mVolumeAcceleration;
    XYZ  mInternalPressure;
    enum {PackedSize = 128,
          PackedTaggedSize = 128+TaggedPacket::TAGSIZE};
} ED_ControlPressure;

inline void ED_OpenPacket_ControlPressure(char *&p, ED_ControlPressure &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mFullOuterTimeStep);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVirial);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVolumePosition);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVolumeVelocity);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVolumeAcceleration);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mInternalPressure);
}

inline void ED_FillPacket_ControlPressure(char *p, ED_ControlPressure &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mFullOuterTimeStep);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVirial);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVolumePosition);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVolumeVelocity);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVolumeAcceleration);
    BM_PacketPrimitive::Pack(p, ed_pkt.mInternalPressure);
}

inline void ED_OpenPacket_ControlPressure(
    char *&p,
    unsigned &aFullOuterTimeStep,
    XYZ &aVirial,
    XYZ &aVolumePosition,
    XYZ &aVolumeVelocity,
    XYZ &aVolumeAcceleration,
    XYZ &aInternalPressure
    )
{
    BM_PacketPrimitive::Unpack(p, aFullOuterTimeStep);
    BM_PacketPrimitive::Unpack(p, aVirial);
    BM_PacketPrimitive::Unpack(p, aVolumePosition);
    BM_PacketPrimitive::Unpack(p, aVolumeVelocity);
    BM_PacketPrimitive::Unpack(p, aVolumeAcceleration);
    BM_PacketPrimitive::Unpack(p, aInternalPressure);
}

inline void ED_FillPacket_ControlPressure(
    char *p,
    const unsigned aFullOuterTimeStep,
    const XYZ aVirial,
    const XYZ aVolumePosition,
    const XYZ aVolumeVelocity,
    const XYZ aVolumeAcceleration,
    const XYZ aInternalPressure
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_ControlPressure::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Control;
    ed_hdr.mType   = ED_CONTROL_TYPES::Pressure;
    ed_hdr.mShortTimeStep   = aFullOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aFullOuterTimeStep);
    BM_PacketPrimitive::Pack(p, aVirial);
    BM_PacketPrimitive::Pack(p, aVolumePosition);
    BM_PacketPrimitive::Pack(p, aVolumeVelocity);
    BM_PacketPrimitive::Pack(p, aVolumeAcceleration);
    BM_PacketPrimitive::Pack(p, aInternalPressure);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_ControlPressure(
    const unsigned aFullOuterTimeStep,
    const XYZ aVirial,
    const XYZ aVolumePosition,
    const XYZ aVolumeVelocity,
    const XYZ aVolumeAcceleration,
    const XYZ aInternalPressure
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_ControlPressure::PackedSize];

    ED_FillPacket_ControlPressure(
    buff,
    aFullOuterTimeStep,
    aVirial,
    aVolumePosition,
    aVolumeVelocity,
    aVolumeAcceleration,
    aInternalPressure
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlPressure::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlPressure::PackedSize );
#endif
}

inline void ED_Emit_Tagged_ControlPressure(
    const unsigned short aTag,
    const unsigned aFullOuterTimeStep,
    const XYZ aVirial,
    const XYZ aVolumePosition,
    const XYZ aVolumeVelocity,
    const XYZ aVolumeAcceleration,
    const XYZ aInternalPressure
    )
{
    char buff[ED_ControlPressure::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_ControlPressure(
    p,
    aFullOuterTimeStep,
    aVirial,
    aVolumePosition,
    aVolumeVelocity,
    aVolumeAcceleration,
    aInternalPressure
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlPressure::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlPressure::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_ControlPressure &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mFullOuterTimeStep << " ";
    os << ed_pkt.mVirial << " ";
    os << ed_pkt.mVolumePosition << " ";
    os << ed_pkt.mVolumeVelocity << " ";
    os << ed_pkt.mVolumeAcceleration << " ";
    os << ed_pkt.mInternalPressure;
    return (os);
}

//=============================================================================
//====        1      4 ControlTimeStamp                                    ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mFullOuterTimeStep;
    unsigned  mTag1;
    unsigned  mTag2;
    unsigned  mSeconds;
    unsigned  mNanoSeconds;
    enum {PackedSize = 24,
          PackedTaggedSize = 24+TaggedPacket::TAGSIZE};
} ED_ControlTimeStamp;

inline void ED_OpenPacket_ControlTimeStamp(char *&p, ED_ControlTimeStamp &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mFullOuterTimeStep);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mTag1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mTag2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSeconds);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mNanoSeconds);
}

inline void ED_FillPacket_ControlTimeStamp(char *p, ED_ControlTimeStamp &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mFullOuterTimeStep);
    BM_PacketPrimitive::Pack(p, ed_pkt.mTag1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mTag2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSeconds);
    BM_PacketPrimitive::Pack(p, ed_pkt.mNanoSeconds);
}

inline void ED_OpenPacket_ControlTimeStamp(
    char *&p,
    unsigned &aFullOuterTimeStep,
    unsigned &aTag1,
    unsigned &aTag2,
    unsigned &aSeconds,
    unsigned &aNanoSeconds
    )
{
    BM_PacketPrimitive::Unpack(p, aFullOuterTimeStep);
    BM_PacketPrimitive::Unpack(p, aTag1);
    BM_PacketPrimitive::Unpack(p, aTag2);
    BM_PacketPrimitive::Unpack(p, aSeconds);
    BM_PacketPrimitive::Unpack(p, aNanoSeconds);
}

inline void ED_FillPacket_ControlTimeStamp(
    char *p,
    const unsigned aFullOuterTimeStep,
    const unsigned aTag1,
    const unsigned aTag2,
    const unsigned aSeconds,
    const unsigned aNanoSeconds
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_ControlTimeStamp::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Control;
    ed_hdr.mType   = ED_CONTROL_TYPES::TimeStamp;
    ed_hdr.mShortTimeStep   = aFullOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aFullOuterTimeStep);
    BM_PacketPrimitive::Pack(p, aTag1);
    BM_PacketPrimitive::Pack(p, aTag2);
    BM_PacketPrimitive::Pack(p, aSeconds);
    BM_PacketPrimitive::Pack(p, aNanoSeconds);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_ControlTimeStamp(
    const unsigned aFullOuterTimeStep,
    const unsigned aTag1,
    const unsigned aTag2,
    const unsigned aSeconds,
    const unsigned aNanoSeconds
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_ControlTimeStamp::PackedSize];

    ED_FillPacket_ControlTimeStamp(
    buff,
    aFullOuterTimeStep,
    aTag1,
    aTag2,
    aSeconds,
    aNanoSeconds
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlTimeStamp::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlTimeStamp::PackedSize );
#endif
}

inline void ED_Emit_Tagged_ControlTimeStamp(
    const unsigned short aTag,
    const unsigned aFullOuterTimeStep,
    const unsigned aTag1,
    const unsigned aTag2,
    const unsigned aSeconds,
    const unsigned aNanoSeconds
    )
{
    char buff[ED_ControlTimeStamp::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_ControlTimeStamp(
    p,
    aFullOuterTimeStep,
    aTag1,
    aTag2,
    aSeconds,
    aNanoSeconds
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlTimeStamp::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlTimeStamp::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_ControlTimeStamp &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mFullOuterTimeStep << " ";
    os << ed_pkt.mTag1 << " ";
    os << ed_pkt.mTag2 << " ";
    os << ed_pkt.mSeconds << " ";
    os << ed_pkt.mNanoSeconds;
    return (os);
}

//=============================================================================
//====        1      5 ControlTrigger                                      ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mFullOuterTimeStep;
    unsigned short  mAction;
    unsigned  mTotalPayload;
    enum {PackedSize = 14,
          PackedTaggedSize = 14+TaggedPacket::TAGSIZE};
} ED_ControlTrigger;

inline void ED_OpenPacket_ControlTrigger(char *&p, ED_ControlTrigger &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mFullOuterTimeStep);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mAction);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mTotalPayload);
}

inline void ED_FillPacket_ControlTrigger(char *p, ED_ControlTrigger &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mFullOuterTimeStep);
    BM_PacketPrimitive::Pack(p, ed_pkt.mAction);
    BM_PacketPrimitive::Pack(p, ed_pkt.mTotalPayload);
}

inline void ED_OpenPacket_ControlTrigger(
    char *&p,
    unsigned &aFullOuterTimeStep,
    unsigned short &aAction,
    unsigned &aTotalPayload
    )
{
    BM_PacketPrimitive::Unpack(p, aFullOuterTimeStep);
    BM_PacketPrimitive::Unpack(p, aAction);
    BM_PacketPrimitive::Unpack(p, aTotalPayload);
}

inline void ED_FillPacket_ControlTrigger(
    char *p,
    const unsigned aFullOuterTimeStep,
    const unsigned short aAction,
    const unsigned aTotalPayload
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_ControlTrigger::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Control;
    ed_hdr.mType   = ED_CONTROL_TYPES::Trigger;
    ed_hdr.mShortTimeStep   = aFullOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aFullOuterTimeStep);
    BM_PacketPrimitive::Pack(p, aAction);
    BM_PacketPrimitive::Pack(p, aTotalPayload);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_ControlTrigger(
    const unsigned aFullOuterTimeStep,
    const unsigned short aAction,
    const unsigned aTotalPayload
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_ControlTrigger::PackedSize];

    ED_FillPacket_ControlTrigger(
    buff,
    aFullOuterTimeStep,
    aAction,
    aTotalPayload
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlTrigger::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlTrigger::PackedSize );
#endif
}

inline void ED_Emit_Tagged_ControlTrigger(
    const unsigned short aTag,
    const unsigned aFullOuterTimeStep,
    const unsigned short aAction,
    const unsigned aTotalPayload
    )
{
    char buff[ED_ControlTrigger::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_ControlTrigger(
    p,
    aFullOuterTimeStep,
    aAction,
    aTotalPayload
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlTrigger::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlTrigger::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_ControlTrigger &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mFullOuterTimeStep << " ";
    os << ed_pkt.mAction << " ";
    os << ed_pkt.mTotalPayload;
    return (os);
}

//=============================================================================
//====        1      6 ControlPortAssignment                               ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mPortID;
    unsigned short  mTrajectoryID;
    int  mDummy1;
    int  mDummy2;
    int  mDummy3;
    int  mDummy4;
    int  mDummy5;
    int  mDummy6;
    enum {PackedSize = 34,
          PackedTaggedSize = 34+TaggedPacket::TAGSIZE};
} ED_ControlPortAssignment;

inline void ED_OpenPacket_ControlPortAssignment(char *&p, ED_ControlPortAssignment &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPortID);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mTrajectoryID);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy3);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy4);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy5);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy6);
}

inline void ED_FillPacket_ControlPortAssignment(char *p, ED_ControlPortAssignment &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPortID);
    BM_PacketPrimitive::Pack(p, ed_pkt.mTrajectoryID);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy3);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy4);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy5);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy6);
}

inline void ED_OpenPacket_ControlPortAssignment(
    char *&p,
    unsigned &aPortID,
    unsigned short &aTrajectoryID,
    int &aDummy1,
    int &aDummy2,
    int &aDummy3,
    int &aDummy4,
    int &aDummy5,
    int &aDummy6
    )
{
    BM_PacketPrimitive::Unpack(p, aPortID);
    BM_PacketPrimitive::Unpack(p, aTrajectoryID);
    BM_PacketPrimitive::Unpack(p, aDummy1);
    BM_PacketPrimitive::Unpack(p, aDummy2);
    BM_PacketPrimitive::Unpack(p, aDummy3);
    BM_PacketPrimitive::Unpack(p, aDummy4);
    BM_PacketPrimitive::Unpack(p, aDummy5);
    BM_PacketPrimitive::Unpack(p, aDummy6);
}

inline void ED_FillPacket_ControlPortAssignment(
    char *p,
    const int aOuterTimeStep,
    const unsigned aPortID,
    const unsigned short aTrajectoryID,
    const int aDummy1,
    const int aDummy2,
    const int aDummy3,
    const int aDummy4,
    const int aDummy5,
    const int aDummy6
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_ControlPortAssignment::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Control;
    ed_hdr.mType   = ED_CONTROL_TYPES::PortAssignment;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aPortID);
    BM_PacketPrimitive::Pack(p, aTrajectoryID);
    BM_PacketPrimitive::Pack(p, aDummy1);
    BM_PacketPrimitive::Pack(p, aDummy2);
    BM_PacketPrimitive::Pack(p, aDummy3);
    BM_PacketPrimitive::Pack(p, aDummy4);
    BM_PacketPrimitive::Pack(p, aDummy5);
    BM_PacketPrimitive::Pack(p, aDummy6);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_ControlPortAssignment(
    const int aOuterTimeStep,
    const unsigned aPortID,
    const unsigned short aTrajectoryID,
    const int aDummy1,
    const int aDummy2,
    const int aDummy3,
    const int aDummy4,
    const int aDummy5,
    const int aDummy6
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_ControlPortAssignment::PackedSize];

    ED_FillPacket_ControlPortAssignment(
    buff,
    aOuterTimeStep,
    aPortID,
    aTrajectoryID,
    aDummy1,
    aDummy2,
    aDummy3,
    aDummy4,
    aDummy5,
    aDummy6
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlPortAssignment::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlPortAssignment::PackedSize );
#endif
}

inline void ED_Emit_Tagged_ControlPortAssignment(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const unsigned aPortID,
    const unsigned short aTrajectoryID,
    const int aDummy1,
    const int aDummy2,
    const int aDummy3,
    const int aDummy4,
    const int aDummy5,
    const int aDummy6
    )
{
    char buff[ED_ControlPortAssignment::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_ControlPortAssignment(
    p,
    aOuterTimeStep,
    aPortID,
    aTrajectoryID,
    aDummy1,
    aDummy2,
    aDummy3,
    aDummy4,
    aDummy5,
    aDummy6
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlPortAssignment::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlPortAssignment::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_ControlPortAssignment &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mPortID << " ";
    os << ed_pkt.mTrajectoryID << " ";
    os << ed_pkt.mDummy1 << " ";
    os << ed_pkt.mDummy2 << " ";
    os << ed_pkt.mDummy3 << " ";
    os << ed_pkt.mDummy4 << " ";
    os << ed_pkt.mDummy5 << " ";
    os << ed_pkt.mDummy6;
    return (os);
}

//=============================================================================
//====        1      7 ControlReplicaExchangePeriodInOTS                   ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mPeriod;
    enum {PackedSize = 8,
          PackedTaggedSize = 8+TaggedPacket::TAGSIZE};
} ED_ControlReplicaExchangePeriodInOTS;

inline void ED_OpenPacket_ControlReplicaExchangePeriodInOTS(char *&p, ED_ControlReplicaExchangePeriodInOTS &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPeriod);
}

inline void ED_FillPacket_ControlReplicaExchangePeriodInOTS(char *p, ED_ControlReplicaExchangePeriodInOTS &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPeriod);
}

inline void ED_OpenPacket_ControlReplicaExchangePeriodInOTS(
    char *&p,
    unsigned &aPeriod
    )
{
    BM_PacketPrimitive::Unpack(p, aPeriod);
}

inline void ED_FillPacket_ControlReplicaExchangePeriodInOTS(
    char *p,
    const int aOuterTimeStep,
    const unsigned aPeriod
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_ControlReplicaExchangePeriodInOTS::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Control;
    ed_hdr.mType   = ED_CONTROL_TYPES::ReplicaExchangePeriodInOTS;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aPeriod);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_ControlReplicaExchangePeriodInOTS(
    const int aOuterTimeStep,
    const unsigned aPeriod
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_ControlReplicaExchangePeriodInOTS::PackedSize];

    ED_FillPacket_ControlReplicaExchangePeriodInOTS(
    buff,
    aOuterTimeStep,
    aPeriod
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlReplicaExchangePeriodInOTS::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlReplicaExchangePeriodInOTS::PackedSize );
#endif
}

inline void ED_Emit_Tagged_ControlReplicaExchangePeriodInOTS(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const unsigned aPeriod
    )
{
    char buff[ED_ControlReplicaExchangePeriodInOTS::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_ControlReplicaExchangePeriodInOTS(
    p,
    aOuterTimeStep,
    aPeriod
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlReplicaExchangePeriodInOTS::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlReplicaExchangePeriodInOTS::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_ControlReplicaExchangePeriodInOTS &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mPeriod;
    return (os);
}

//=============================================================================
//====        1      8 ControlReplicaExchangeAttempt                       ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mFullOuterTimeStep;
    unsigned  mID1;
    unsigned  mID2;
    double  mT1;
    double  mT2;
    double  mE1;
    unsigned  mSwapAttempts;
    unsigned  mSwapAccepts;
    enum {PackedSize = 48,
          PackedTaggedSize = 48+TaggedPacket::TAGSIZE};
} ED_ControlReplicaExchangeAttempt;

inline void ED_OpenPacket_ControlReplicaExchangeAttempt(char *&p, ED_ControlReplicaExchangeAttempt &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mFullOuterTimeStep);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mID1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mID2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mT1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mT2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mE1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSwapAttempts);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSwapAccepts);
}

inline void ED_FillPacket_ControlReplicaExchangeAttempt(char *p, ED_ControlReplicaExchangeAttempt &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mFullOuterTimeStep);
    BM_PacketPrimitive::Pack(p, ed_pkt.mID1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mID2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mT1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mT2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mE1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSwapAttempts);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSwapAccepts);
}

inline void ED_OpenPacket_ControlReplicaExchangeAttempt(
    char *&p,
    unsigned &aFullOuterTimeStep,
    unsigned &aID1,
    unsigned &aID2,
    double &aT1,
    double &aT2,
    double &aE1,
    unsigned &aSwapAttempts,
    unsigned &aSwapAccepts
    )
{
    BM_PacketPrimitive::Unpack(p, aFullOuterTimeStep);
    BM_PacketPrimitive::Unpack(p, aID1);
    BM_PacketPrimitive::Unpack(p, aID2);
    BM_PacketPrimitive::Unpack(p, aT1);
    BM_PacketPrimitive::Unpack(p, aT2);
    BM_PacketPrimitive::Unpack(p, aE1);
    BM_PacketPrimitive::Unpack(p, aSwapAttempts);
    BM_PacketPrimitive::Unpack(p, aSwapAccepts);
}

inline void ED_FillPacket_ControlReplicaExchangeAttempt(
    char *p,
    const unsigned aFullOuterTimeStep,
    const unsigned aID1,
    const unsigned aID2,
    const double aT1,
    const double aT2,
    const double aE1,
    const unsigned aSwapAttempts,
    const unsigned aSwapAccepts
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_ControlReplicaExchangeAttempt::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Control;
    ed_hdr.mType   = ED_CONTROL_TYPES::ReplicaExchangeAttempt;
    ed_hdr.mShortTimeStep   = aFullOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aFullOuterTimeStep);
    BM_PacketPrimitive::Pack(p, aID1);
    BM_PacketPrimitive::Pack(p, aID2);
    BM_PacketPrimitive::Pack(p, aT1);
    BM_PacketPrimitive::Pack(p, aT2);
    BM_PacketPrimitive::Pack(p, aE1);
    BM_PacketPrimitive::Pack(p, aSwapAttempts);
    BM_PacketPrimitive::Pack(p, aSwapAccepts);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_ControlReplicaExchangeAttempt(
    const unsigned aFullOuterTimeStep,
    const unsigned aID1,
    const unsigned aID2,
    const double aT1,
    const double aT2,
    const double aE1,
    const unsigned aSwapAttempts,
    const unsigned aSwapAccepts
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_ControlReplicaExchangeAttempt::PackedSize];

    ED_FillPacket_ControlReplicaExchangeAttempt(
    buff,
    aFullOuterTimeStep,
    aID1,
    aID2,
    aT1,
    aT2,
    aE1,
    aSwapAttempts,
    aSwapAccepts
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlReplicaExchangeAttempt::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlReplicaExchangeAttempt::PackedSize );
#endif
}

inline void ED_Emit_Tagged_ControlReplicaExchangeAttempt(
    const unsigned short aTag,
    const unsigned aFullOuterTimeStep,
    const unsigned aID1,
    const unsigned aID2,
    const double aT1,
    const double aT2,
    const double aE1,
    const unsigned aSwapAttempts,
    const unsigned aSwapAccepts
    )
{
    char buff[ED_ControlReplicaExchangeAttempt::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_ControlReplicaExchangeAttempt(
    p,
    aFullOuterTimeStep,
    aID1,
    aID2,
    aT1,
    aT2,
    aE1,
    aSwapAttempts,
    aSwapAccepts
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_ControlReplicaExchangeAttempt::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_ControlReplicaExchangeAttempt::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_ControlReplicaExchangeAttempt &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mFullOuterTimeStep << " ";
    os << ed_pkt.mID1 << " ";
    os << ed_pkt.mID2 << " ";
    os << ed_pkt.mT1 << " ";
    os << ed_pkt.mT2 << " ";
    os << ed_pkt.mE1 << " ";
    os << ed_pkt.mSwapAttempts << " ";
    os << ed_pkt.mSwapAccepts;
    return (os);
}

//=============================================================================
//====        2      1 DynamicVariablesSite                                ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mVoxelId;
    int  mSiteId;
    XYZ  mPosition;
    XYZ  mVelocity;
    enum {PackedSize = 60,
          PackedTaggedSize = 60+TaggedPacket::TAGSIZE};
} ED_DynamicVariablesSite;

inline void ED_OpenPacket_DynamicVariablesSite(char *&p, ED_DynamicVariablesSite &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVoxelId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSiteId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity);
}

inline void ED_FillPacket_DynamicVariablesSite(char *p, ED_DynamicVariablesSite &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVoxelId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSiteId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity);
}

inline void ED_OpenPacket_DynamicVariablesSite(
    char *&p,
    int &aVoxelId,
    int &aSiteId,
    XYZ &aPosition,
    XYZ &aVelocity
    )
{
    BM_PacketPrimitive::Unpack(p, aVoxelId);
    BM_PacketPrimitive::Unpack(p, aSiteId);
    BM_PacketPrimitive::Unpack(p, aPosition);
    BM_PacketPrimitive::Unpack(p, aVelocity);
}

inline void ED_FillPacket_DynamicVariablesSite(
    char *p,
    const int aOuterTimeStep,
    const int aVoxelId,
    const int aSiteId,
    const XYZ aPosition,
    const XYZ aVelocity
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_DynamicVariablesSite::PackedSize;
    ed_hdr.mClass  = ED_CLASS::DynamicVariables;
    ed_hdr.mType   = ED_DYNAMICVARIABLES_TYPES::Site;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aVoxelId);
    BM_PacketPrimitive::Pack(p, aSiteId);
    BM_PacketPrimitive::Pack(p, aPosition);
    BM_PacketPrimitive::Pack(p, aVelocity);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_DynamicVariablesSite(
    const int aOuterTimeStep,
    const int aVoxelId,
    const int aSiteId,
    const XYZ aPosition,
    const XYZ aVelocity
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_DynamicVariablesSite::PackedSize];

    ED_FillPacket_DynamicVariablesSite(
    buff,
    aOuterTimeStep,
    aVoxelId,
    aSiteId,
    aPosition,
    aVelocity
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesSite::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesSite::PackedSize );
#endif
}

inline void ED_Emit_Tagged_DynamicVariablesSite(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aVoxelId,
    const int aSiteId,
    const XYZ aPosition,
    const XYZ aVelocity
    )
{
    char buff[ED_DynamicVariablesSite::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_DynamicVariablesSite(
    p,
    aOuterTimeStep,
    aVoxelId,
    aSiteId,
    aPosition,
    aVelocity
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesSite::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesSite::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_DynamicVariablesSite &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mVoxelId << " ";
    os << ed_pkt.mSiteId << " ";
    os << ed_pkt.mPosition << " ";
    os << ed_pkt.mVelocity;
    return (os);
}

//=============================================================================
//====        2      2 DynamicVariablesForce                               ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mVoxelId;
    int  mSiteId;
    int  mRESPALevel;
    XYZ  mForce;
    enum {PackedSize = 40,
          PackedTaggedSize = 40+TaggedPacket::TAGSIZE};
} ED_DynamicVariablesForce;

inline void ED_OpenPacket_DynamicVariablesForce(char *&p, ED_DynamicVariablesForce &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVoxelId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSiteId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mRESPALevel);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mForce);
}

inline void ED_FillPacket_DynamicVariablesForce(char *p, ED_DynamicVariablesForce &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVoxelId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSiteId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mRESPALevel);
    BM_PacketPrimitive::Pack(p, ed_pkt.mForce);
}

inline void ED_OpenPacket_DynamicVariablesForce(
    char *&p,
    int &aVoxelId,
    int &aSiteId,
    int &aRESPALevel,
    XYZ &aForce
    )
{
    BM_PacketPrimitive::Unpack(p, aVoxelId);
    BM_PacketPrimitive::Unpack(p, aSiteId);
    BM_PacketPrimitive::Unpack(p, aRESPALevel);
    BM_PacketPrimitive::Unpack(p, aForce);
}

inline void ED_FillPacket_DynamicVariablesForce(
    char *p,
    const int aOuterTimeStep,
    const int aVoxelId,
    const int aSiteId,
    const int aRESPALevel,
    const XYZ aForce
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_DynamicVariablesForce::PackedSize;
    ed_hdr.mClass  = ED_CLASS::DynamicVariables;
    ed_hdr.mType   = ED_DYNAMICVARIABLES_TYPES::Force;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aVoxelId);
    BM_PacketPrimitive::Pack(p, aSiteId);
    BM_PacketPrimitive::Pack(p, aRESPALevel);
    BM_PacketPrimitive::Pack(p, aForce);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_DynamicVariablesForce(
    const int aOuterTimeStep,
    const int aVoxelId,
    const int aSiteId,
    const int aRESPALevel,
    const XYZ aForce
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_DynamicVariablesForce::PackedSize];

    ED_FillPacket_DynamicVariablesForce(
    buff,
    aOuterTimeStep,
    aVoxelId,
    aSiteId,
    aRESPALevel,
    aForce
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesForce::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesForce::PackedSize );
#endif
}

inline void ED_Emit_Tagged_DynamicVariablesForce(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aVoxelId,
    const int aSiteId,
    const int aRESPALevel,
    const XYZ aForce
    )
{
    char buff[ED_DynamicVariablesForce::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_DynamicVariablesForce(
    p,
    aOuterTimeStep,
    aVoxelId,
    aSiteId,
    aRESPALevel,
    aForce
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesForce::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesForce::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_DynamicVariablesForce &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mVoxelId << " ";
    os << ed_pkt.mSiteId << " ";
    os << ed_pkt.mRESPALevel << " ";
    os << ed_pkt.mForce;
    return (os);
}

//=============================================================================
//====        2      3 DynamicVariablesMomentum                            ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mSiteId;
    XYZ  mMomentum;
    enum {PackedSize = 32,
          PackedTaggedSize = 32+TaggedPacket::TAGSIZE};
} ED_DynamicVariablesMomentum;

inline void ED_OpenPacket_DynamicVariablesMomentum(char *&p, ED_DynamicVariablesMomentum &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSiteId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mMomentum);
}

inline void ED_FillPacket_DynamicVariablesMomentum(char *p, ED_DynamicVariablesMomentum &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSiteId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mMomentum);
}

inline void ED_OpenPacket_DynamicVariablesMomentum(
    char *&p,
    int &aSiteId,
    XYZ &aMomentum
    )
{
    BM_PacketPrimitive::Unpack(p, aSiteId);
    BM_PacketPrimitive::Unpack(p, aMomentum);
}

inline void ED_FillPacket_DynamicVariablesMomentum(
    char *p,
    const int aOuterTimeStep,
    const int aSiteId,
    const XYZ aMomentum
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_DynamicVariablesMomentum::PackedSize;
    ed_hdr.mClass  = ED_CLASS::DynamicVariables;
    ed_hdr.mType   = ED_DYNAMICVARIABLES_TYPES::Momentum;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aSiteId);
    BM_PacketPrimitive::Pack(p, aMomentum);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_DynamicVariablesMomentum(
    const int aOuterTimeStep,
    const int aSiteId,
    const XYZ aMomentum
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_DynamicVariablesMomentum::PackedSize];

    ED_FillPacket_DynamicVariablesMomentum(
    buff,
    aOuterTimeStep,
    aSiteId,
    aMomentum
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesMomentum::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesMomentum::PackedSize );
#endif
}

inline void ED_Emit_Tagged_DynamicVariablesMomentum(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aSiteId,
    const XYZ aMomentum
    )
{
    char buff[ED_DynamicVariablesMomentum::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_DynamicVariablesMomentum(
    p,
    aOuterTimeStep,
    aSiteId,
    aMomentum
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesMomentum::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesMomentum::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_DynamicVariablesMomentum &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mSiteId << " ";
    os << ed_pkt.mMomentum;
    return (os);
}

//=============================================================================
//====        2      4 DynamicVariablesFloatSite                           ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mSiteId;
    float  mX;
    float  mY;
    float  mZ;
    enum {PackedSize = 20,
          PackedTaggedSize = 20+TaggedPacket::TAGSIZE};
} ED_DynamicVariablesFloatSite;

inline void ED_OpenPacket_DynamicVariablesFloatSite(char *&p, ED_DynamicVariablesFloatSite &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSiteId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mX);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mY);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mZ);
}

inline void ED_FillPacket_DynamicVariablesFloatSite(char *p, ED_DynamicVariablesFloatSite &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSiteId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mX);
    BM_PacketPrimitive::Pack(p, ed_pkt.mY);
    BM_PacketPrimitive::Pack(p, ed_pkt.mZ);
}

inline void ED_OpenPacket_DynamicVariablesFloatSite(
    char *&p,
    int &aSiteId,
    float &aX,
    float &aY,
    float &aZ
    )
{
    BM_PacketPrimitive::Unpack(p, aSiteId);
    BM_PacketPrimitive::Unpack(p, aX);
    BM_PacketPrimitive::Unpack(p, aY);
    BM_PacketPrimitive::Unpack(p, aZ);
}

inline void ED_FillPacket_DynamicVariablesFloatSite(
    char *p,
    const int aOuterTimeStep,
    const int aSiteId,
    const float aX,
    const float aY,
    const float aZ
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_DynamicVariablesFloatSite::PackedSize;
    ed_hdr.mClass  = ED_CLASS::DynamicVariables;
    ed_hdr.mType   = ED_DYNAMICVARIABLES_TYPES::FloatSite;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aSiteId);
    BM_PacketPrimitive::Pack(p, aX);
    BM_PacketPrimitive::Pack(p, aY);
    BM_PacketPrimitive::Pack(p, aZ);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_DynamicVariablesFloatSite(
    const int aOuterTimeStep,
    const int aSiteId,
    const float aX,
    const float aY,
    const float aZ
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_DynamicVariablesFloatSite::PackedSize];

    ED_FillPacket_DynamicVariablesFloatSite(
    buff,
    aOuterTimeStep,
    aSiteId,
    aX,
    aY,
    aZ
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesFloatSite::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesFloatSite::PackedSize );
#endif
}

inline void ED_Emit_Tagged_DynamicVariablesFloatSite(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aSiteId,
    const float aX,
    const float aY,
    const float aZ
    )
{
    char buff[ED_DynamicVariablesFloatSite::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_DynamicVariablesFloatSite(
    p,
    aOuterTimeStep,
    aSiteId,
    aX,
    aY,
    aZ
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesFloatSite::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesFloatSite::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_DynamicVariablesFloatSite &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mSiteId << " ";
    os << ed_pkt.mX << " ";
    os << ed_pkt.mY << " ";
    os << ed_pkt.mZ;
    return (os);
}

//=============================================================================
//====        2      5 DynamicVariablesBoundingBox                         ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    XYZ  mOrigin;
    XYZ  mSize;
    enum {PackedSize = 52,
          PackedTaggedSize = 52+TaggedPacket::TAGSIZE};
} ED_DynamicVariablesBoundingBox;

inline void ED_OpenPacket_DynamicVariablesBoundingBox(char *&p, ED_DynamicVariablesBoundingBox &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mOrigin);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSize);
}

inline void ED_FillPacket_DynamicVariablesBoundingBox(char *p, ED_DynamicVariablesBoundingBox &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mOrigin);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSize);
}

inline void ED_OpenPacket_DynamicVariablesBoundingBox(
    char *&p,
    XYZ &aOrigin,
    XYZ &aSize
    )
{
    BM_PacketPrimitive::Unpack(p, aOrigin);
    BM_PacketPrimitive::Unpack(p, aSize);
}

inline void ED_FillPacket_DynamicVariablesBoundingBox(
    char *p,
    const int aOuterTimeStep,
    const XYZ aOrigin,
    const XYZ aSize
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_DynamicVariablesBoundingBox::PackedSize;
    ed_hdr.mClass  = ED_CLASS::DynamicVariables;
    ed_hdr.mType   = ED_DYNAMICVARIABLES_TYPES::BoundingBox;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aOrigin);
    BM_PacketPrimitive::Pack(p, aSize);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_DynamicVariablesBoundingBox(
    const int aOuterTimeStep,
    const XYZ aOrigin,
    const XYZ aSize
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_DynamicVariablesBoundingBox::PackedSize];

    ED_FillPacket_DynamicVariablesBoundingBox(
    buff,
    aOuterTimeStep,
    aOrigin,
    aSize
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesBoundingBox::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesBoundingBox::PackedSize );
#endif
}

inline void ED_Emit_Tagged_DynamicVariablesBoundingBox(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const XYZ aOrigin,
    const XYZ aSize
    )
{
    char buff[ED_DynamicVariablesBoundingBox::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_DynamicVariablesBoundingBox(
    p,
    aOuterTimeStep,
    aOrigin,
    aSize
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesBoundingBox::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesBoundingBox::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_DynamicVariablesBoundingBox &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mOrigin << " ";
    os << ed_pkt.mSize;
    return (os);
}

//=============================================================================
//====        2      6 DynamicVariablesShortSite                           ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mSiteId;
    unsigned short  mX0;
    unsigned short  mY0;
    unsigned short  mZ0;
    unsigned short  mX1;
    unsigned short  mY1;
    unsigned short  mZ1;
    unsigned short  mX2;
    unsigned short  mY2;
    unsigned short  mZ2;
    unsigned short  mX3;
    unsigned short  mY3;
    unsigned short  mZ3;
    enum {PackedSize = 32,
          PackedTaggedSize = 32+TaggedPacket::TAGSIZE};
} ED_DynamicVariablesShortSite;

inline void ED_OpenPacket_DynamicVariablesShortSite(char *&p, ED_DynamicVariablesShortSite &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSiteId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mX0);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mY0);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mZ0);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mX1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mY1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mZ1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mX2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mY2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mZ2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mX3);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mY3);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mZ3);
}

inline void ED_FillPacket_DynamicVariablesShortSite(char *p, ED_DynamicVariablesShortSite &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSiteId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mX0);
    BM_PacketPrimitive::Pack(p, ed_pkt.mY0);
    BM_PacketPrimitive::Pack(p, ed_pkt.mZ0);
    BM_PacketPrimitive::Pack(p, ed_pkt.mX1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mY1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mZ1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mX2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mY2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mZ2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mX3);
    BM_PacketPrimitive::Pack(p, ed_pkt.mY3);
    BM_PacketPrimitive::Pack(p, ed_pkt.mZ3);
}

inline void ED_OpenPacket_DynamicVariablesShortSite(
    char *&p,
    int &aSiteId,
    unsigned short &aX0,
    unsigned short &aY0,
    unsigned short &aZ0,
    unsigned short &aX1,
    unsigned short &aY1,
    unsigned short &aZ1,
    unsigned short &aX2,
    unsigned short &aY2,
    unsigned short &aZ2,
    unsigned short &aX3,
    unsigned short &aY3,
    unsigned short &aZ3
    )
{
    BM_PacketPrimitive::Unpack(p, aSiteId);
    BM_PacketPrimitive::Unpack(p, aX0);
    BM_PacketPrimitive::Unpack(p, aY0);
    BM_PacketPrimitive::Unpack(p, aZ0);
    BM_PacketPrimitive::Unpack(p, aX1);
    BM_PacketPrimitive::Unpack(p, aY1);
    BM_PacketPrimitive::Unpack(p, aZ1);
    BM_PacketPrimitive::Unpack(p, aX2);
    BM_PacketPrimitive::Unpack(p, aY2);
    BM_PacketPrimitive::Unpack(p, aZ2);
    BM_PacketPrimitive::Unpack(p, aX3);
    BM_PacketPrimitive::Unpack(p, aY3);
    BM_PacketPrimitive::Unpack(p, aZ3);
}

inline void ED_FillPacket_DynamicVariablesShortSite(
    char *p,
    const int aOuterTimeStep,
    const int aSiteId,
    const unsigned short aX0,
    const unsigned short aY0,
    const unsigned short aZ0,
    const unsigned short aX1,
    const unsigned short aY1,
    const unsigned short aZ1,
    const unsigned short aX2,
    const unsigned short aY2,
    const unsigned short aZ2,
    const unsigned short aX3,
    const unsigned short aY3,
    const unsigned short aZ3
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_DynamicVariablesShortSite::PackedSize;
    ed_hdr.mClass  = ED_CLASS::DynamicVariables;
    ed_hdr.mType   = ED_DYNAMICVARIABLES_TYPES::ShortSite;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aSiteId);
    BM_PacketPrimitive::Pack(p, aX0);
    BM_PacketPrimitive::Pack(p, aY0);
    BM_PacketPrimitive::Pack(p, aZ0);
    BM_PacketPrimitive::Pack(p, aX1);
    BM_PacketPrimitive::Pack(p, aY1);
    BM_PacketPrimitive::Pack(p, aZ1);
    BM_PacketPrimitive::Pack(p, aX2);
    BM_PacketPrimitive::Pack(p, aY2);
    BM_PacketPrimitive::Pack(p, aZ2);
    BM_PacketPrimitive::Pack(p, aX3);
    BM_PacketPrimitive::Pack(p, aY3);
    BM_PacketPrimitive::Pack(p, aZ3);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_DynamicVariablesShortSite(
    const int aOuterTimeStep,
    const int aSiteId,
    const unsigned short aX0,
    const unsigned short aY0,
    const unsigned short aZ0,
    const unsigned short aX1,
    const unsigned short aY1,
    const unsigned short aZ1,
    const unsigned short aX2,
    const unsigned short aY2,
    const unsigned short aZ2,
    const unsigned short aX3,
    const unsigned short aY3,
    const unsigned short aZ3
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_DynamicVariablesShortSite::PackedSize];

    ED_FillPacket_DynamicVariablesShortSite(
    buff,
    aOuterTimeStep,
    aSiteId,
    aX0,
    aY0,
    aZ0,
    aX1,
    aY1,
    aZ1,
    aX2,
    aY2,
    aZ2,
    aX3,
    aY3,
    aZ3
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesShortSite::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesShortSite::PackedSize );
#endif
}

inline void ED_Emit_Tagged_DynamicVariablesShortSite(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aSiteId,
    const unsigned short aX0,
    const unsigned short aY0,
    const unsigned short aZ0,
    const unsigned short aX1,
    const unsigned short aY1,
    const unsigned short aZ1,
    const unsigned short aX2,
    const unsigned short aY2,
    const unsigned short aZ2,
    const unsigned short aX3,
    const unsigned short aY3,
    const unsigned short aZ3
    )
{
    char buff[ED_DynamicVariablesShortSite::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_DynamicVariablesShortSite(
    p,
    aOuterTimeStep,
    aSiteId,
    aX0,
    aY0,
    aZ0,
    aX1,
    aY1,
    aZ1,
    aX2,
    aY2,
    aZ2,
    aX3,
    aY3,
    aZ3
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesShortSite::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesShortSite::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_DynamicVariablesShortSite &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mSiteId << " ";
    os << ed_pkt.mX0 << " ";
    os << ed_pkt.mY0 << " ";
    os << ed_pkt.mZ0 << " ";
    os << ed_pkt.mX1 << " ";
    os << ed_pkt.mY1 << " ";
    os << ed_pkt.mZ1 << " ";
    os << ed_pkt.mX2 << " ";
    os << ed_pkt.mY2 << " ";
    os << ed_pkt.mZ2 << " ";
    os << ed_pkt.mX3 << " ";
    os << ed_pkt.mY3 << " ";
    os << ed_pkt.mZ3;
    return (os);
}

//=============================================================================
//====        2      7 DynamicVariablesTwoChainFragmentThermostat          ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mFragmentNumber;
    int  mVoxelId;
    unsigned short  mNumberOfSitesPerChain;
    double  mPosition00;
    double  mPosition01;
    double  mPosition02;
    double  mPosition03;
    double  mPosition04;
    double  mVelocity00;
    double  mVelocity01;
    double  mVelocity02;
    double  mVelocity03;
    double  mVelocity04;
    double  mPosition10;
    double  mPosition11;
    double  mPosition12;
    double  mPosition13;
    double  mPosition14;
    double  mVelocity10;
    double  mVelocity11;
    double  mVelocity12;
    double  mVelocity13;
    double  mVelocity14;
    enum {PackedSize = 174,
          PackedTaggedSize = 174+TaggedPacket::TAGSIZE};
} ED_DynamicVariablesTwoChainFragmentThermostat;

inline void ED_OpenPacket_DynamicVariablesTwoChainFragmentThermostat(char *&p, ED_DynamicVariablesTwoChainFragmentThermostat &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mFragmentNumber);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVoxelId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mNumberOfSitesPerChain);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition00);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition01);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition02);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition03);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition04);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity00);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity01);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity02);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity03);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity04);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition10);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition11);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition12);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition13);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition14);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity10);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity11);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity12);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity13);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity14);
}

inline void ED_FillPacket_DynamicVariablesTwoChainFragmentThermostat(char *p, ED_DynamicVariablesTwoChainFragmentThermostat &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mFragmentNumber);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVoxelId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mNumberOfSitesPerChain);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition00);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition01);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition02);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition03);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition04);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity00);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity01);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity02);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity03);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity04);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition10);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition11);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition12);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition13);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition14);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity10);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity11);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity12);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity13);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity14);
}

inline void ED_OpenPacket_DynamicVariablesTwoChainFragmentThermostat(
    char *&p,
    int &aFragmentNumber,
    int &aVoxelId,
    unsigned short &aNumberOfSitesPerChain,
    double &aPosition00,
    double &aPosition01,
    double &aPosition02,
    double &aPosition03,
    double &aPosition04,
    double &aVelocity00,
    double &aVelocity01,
    double &aVelocity02,
    double &aVelocity03,
    double &aVelocity04,
    double &aPosition10,
    double &aPosition11,
    double &aPosition12,
    double &aPosition13,
    double &aPosition14,
    double &aVelocity10,
    double &aVelocity11,
    double &aVelocity12,
    double &aVelocity13,
    double &aVelocity14
    )
{
    BM_PacketPrimitive::Unpack(p, aFragmentNumber);
    BM_PacketPrimitive::Unpack(p, aVoxelId);
    BM_PacketPrimitive::Unpack(p, aNumberOfSitesPerChain);
    BM_PacketPrimitive::Unpack(p, aPosition00);
    BM_PacketPrimitive::Unpack(p, aPosition01);
    BM_PacketPrimitive::Unpack(p, aPosition02);
    BM_PacketPrimitive::Unpack(p, aPosition03);
    BM_PacketPrimitive::Unpack(p, aPosition04);
    BM_PacketPrimitive::Unpack(p, aVelocity00);
    BM_PacketPrimitive::Unpack(p, aVelocity01);
    BM_PacketPrimitive::Unpack(p, aVelocity02);
    BM_PacketPrimitive::Unpack(p, aVelocity03);
    BM_PacketPrimitive::Unpack(p, aVelocity04);
    BM_PacketPrimitive::Unpack(p, aPosition10);
    BM_PacketPrimitive::Unpack(p, aPosition11);
    BM_PacketPrimitive::Unpack(p, aPosition12);
    BM_PacketPrimitive::Unpack(p, aPosition13);
    BM_PacketPrimitive::Unpack(p, aPosition14);
    BM_PacketPrimitive::Unpack(p, aVelocity10);
    BM_PacketPrimitive::Unpack(p, aVelocity11);
    BM_PacketPrimitive::Unpack(p, aVelocity12);
    BM_PacketPrimitive::Unpack(p, aVelocity13);
    BM_PacketPrimitive::Unpack(p, aVelocity14);
}

inline void ED_FillPacket_DynamicVariablesTwoChainFragmentThermostat(
    char *p,
    const int aOuterTimeStep,
    const int aFragmentNumber,
    const int aVoxelId,
    const unsigned short aNumberOfSitesPerChain,
    const double aPosition00,
    const double aPosition01,
    const double aPosition02,
    const double aPosition03,
    const double aPosition04,
    const double aVelocity00,
    const double aVelocity01,
    const double aVelocity02,
    const double aVelocity03,
    const double aVelocity04,
    const double aPosition10,
    const double aPosition11,
    const double aPosition12,
    const double aPosition13,
    const double aPosition14,
    const double aVelocity10,
    const double aVelocity11,
    const double aVelocity12,
    const double aVelocity13,
    const double aVelocity14
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_DynamicVariablesTwoChainFragmentThermostat::PackedSize;
    ed_hdr.mClass  = ED_CLASS::DynamicVariables;
    ed_hdr.mType   = ED_DYNAMICVARIABLES_TYPES::TwoChainFragmentThermostat;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aFragmentNumber);
    BM_PacketPrimitive::Pack(p, aVoxelId);
    BM_PacketPrimitive::Pack(p, aNumberOfSitesPerChain);
    BM_PacketPrimitive::Pack(p, aPosition00);
    BM_PacketPrimitive::Pack(p, aPosition01);
    BM_PacketPrimitive::Pack(p, aPosition02);
    BM_PacketPrimitive::Pack(p, aPosition03);
    BM_PacketPrimitive::Pack(p, aPosition04);
    BM_PacketPrimitive::Pack(p, aVelocity00);
    BM_PacketPrimitive::Pack(p, aVelocity01);
    BM_PacketPrimitive::Pack(p, aVelocity02);
    BM_PacketPrimitive::Pack(p, aVelocity03);
    BM_PacketPrimitive::Pack(p, aVelocity04);
    BM_PacketPrimitive::Pack(p, aPosition10);
    BM_PacketPrimitive::Pack(p, aPosition11);
    BM_PacketPrimitive::Pack(p, aPosition12);
    BM_PacketPrimitive::Pack(p, aPosition13);
    BM_PacketPrimitive::Pack(p, aPosition14);
    BM_PacketPrimitive::Pack(p, aVelocity10);
    BM_PacketPrimitive::Pack(p, aVelocity11);
    BM_PacketPrimitive::Pack(p, aVelocity12);
    BM_PacketPrimitive::Pack(p, aVelocity13);
    BM_PacketPrimitive::Pack(p, aVelocity14);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_DynamicVariablesTwoChainFragmentThermostat(
    const int aOuterTimeStep,
    const int aFragmentNumber,
    const int aVoxelId,
    const unsigned short aNumberOfSitesPerChain,
    const double aPosition00,
    const double aPosition01,
    const double aPosition02,
    const double aPosition03,
    const double aPosition04,
    const double aVelocity00,
    const double aVelocity01,
    const double aVelocity02,
    const double aVelocity03,
    const double aVelocity04,
    const double aPosition10,
    const double aPosition11,
    const double aPosition12,
    const double aPosition13,
    const double aPosition14,
    const double aVelocity10,
    const double aVelocity11,
    const double aVelocity12,
    const double aVelocity13,
    const double aVelocity14
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_DynamicVariablesTwoChainFragmentThermostat::PackedSize];

    ED_FillPacket_DynamicVariablesTwoChainFragmentThermostat(
    buff,
    aOuterTimeStep,
    aFragmentNumber,
    aVoxelId,
    aNumberOfSitesPerChain,
    aPosition00,
    aPosition01,
    aPosition02,
    aPosition03,
    aPosition04,
    aVelocity00,
    aVelocity01,
    aVelocity02,
    aVelocity03,
    aVelocity04,
    aPosition10,
    aPosition11,
    aPosition12,
    aPosition13,
    aPosition14,
    aVelocity10,
    aVelocity11,
    aVelocity12,
    aVelocity13,
    aVelocity14
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesTwoChainFragmentThermostat::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesTwoChainFragmentThermostat::PackedSize );
#endif
}

inline void ED_Emit_Tagged_DynamicVariablesTwoChainFragmentThermostat(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aFragmentNumber,
    const int aVoxelId,
    const unsigned short aNumberOfSitesPerChain,
    const double aPosition00,
    const double aPosition01,
    const double aPosition02,
    const double aPosition03,
    const double aPosition04,
    const double aVelocity00,
    const double aVelocity01,
    const double aVelocity02,
    const double aVelocity03,
    const double aVelocity04,
    const double aPosition10,
    const double aPosition11,
    const double aPosition12,
    const double aPosition13,
    const double aPosition14,
    const double aVelocity10,
    const double aVelocity11,
    const double aVelocity12,
    const double aVelocity13,
    const double aVelocity14
    )
{
    char buff[ED_DynamicVariablesTwoChainFragmentThermostat::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_DynamicVariablesTwoChainFragmentThermostat(
    p,
    aOuterTimeStep,
    aFragmentNumber,
    aVoxelId,
    aNumberOfSitesPerChain,
    aPosition00,
    aPosition01,
    aPosition02,
    aPosition03,
    aPosition04,
    aVelocity00,
    aVelocity01,
    aVelocity02,
    aVelocity03,
    aVelocity04,
    aPosition10,
    aPosition11,
    aPosition12,
    aPosition13,
    aPosition14,
    aVelocity10,
    aVelocity11,
    aVelocity12,
    aVelocity13,
    aVelocity14
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesTwoChainFragmentThermostat::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesTwoChainFragmentThermostat::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_DynamicVariablesTwoChainFragmentThermostat &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mFragmentNumber << " ";
    os << ed_pkt.mVoxelId << " ";
    os << ed_pkt.mNumberOfSitesPerChain << " ";
    os << ed_pkt.mPosition00 << " ";
    os << ed_pkt.mPosition01 << " ";
    os << ed_pkt.mPosition02 << " ";
    os << ed_pkt.mPosition03 << " ";
    os << ed_pkt.mPosition04 << " ";
    os << ed_pkt.mVelocity00 << " ";
    os << ed_pkt.mVelocity01 << " ";
    os << ed_pkt.mVelocity02 << " ";
    os << ed_pkt.mVelocity03 << " ";
    os << ed_pkt.mVelocity04 << " ";
    os << ed_pkt.mPosition10 << " ";
    os << ed_pkt.mPosition11 << " ";
    os << ed_pkt.mPosition12 << " ";
    os << ed_pkt.mPosition13 << " ";
    os << ed_pkt.mPosition14 << " ";
    os << ed_pkt.mVelocity10 << " ";
    os << ed_pkt.mVelocity11 << " ";
    os << ed_pkt.mVelocity12 << " ";
    os << ed_pkt.mVelocity13 << " ";
    os << ed_pkt.mVelocity14;
    return (os);
}

//=============================================================================
//====        2      8 DynamicVariablesOneChainThermostat                  ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mVoxelId;
    unsigned short  mNumberOfSitesPerChain;
    double  mPosition0;
    double  mPosition1;
    double  mPosition2;
    double  mPosition3;
    double  mPosition4;
    double  mVelocity0;
    double  mVelocity1;
    double  mVelocity2;
    double  mVelocity3;
    double  mVelocity4;
    enum {PackedSize = 90,
          PackedTaggedSize = 90+TaggedPacket::TAGSIZE};
} ED_DynamicVariablesOneChainThermostat;

inline void ED_OpenPacket_DynamicVariablesOneChainThermostat(char *&p, ED_DynamicVariablesOneChainThermostat &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVoxelId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mNumberOfSitesPerChain);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition0);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition3);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPosition4);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity0);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity3);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocity4);
}

inline void ED_FillPacket_DynamicVariablesOneChainThermostat(char *p, ED_DynamicVariablesOneChainThermostat &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVoxelId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mNumberOfSitesPerChain);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition0);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition3);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPosition4);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity0);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity3);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocity4);
}

inline void ED_OpenPacket_DynamicVariablesOneChainThermostat(
    char *&p,
    int &aVoxelId,
    unsigned short &aNumberOfSitesPerChain,
    double &aPosition0,
    double &aPosition1,
    double &aPosition2,
    double &aPosition3,
    double &aPosition4,
    double &aVelocity0,
    double &aVelocity1,
    double &aVelocity2,
    double &aVelocity3,
    double &aVelocity4
    )
{
    BM_PacketPrimitive::Unpack(p, aVoxelId);
    BM_PacketPrimitive::Unpack(p, aNumberOfSitesPerChain);
    BM_PacketPrimitive::Unpack(p, aPosition0);
    BM_PacketPrimitive::Unpack(p, aPosition1);
    BM_PacketPrimitive::Unpack(p, aPosition2);
    BM_PacketPrimitive::Unpack(p, aPosition3);
    BM_PacketPrimitive::Unpack(p, aPosition4);
    BM_PacketPrimitive::Unpack(p, aVelocity0);
    BM_PacketPrimitive::Unpack(p, aVelocity1);
    BM_PacketPrimitive::Unpack(p, aVelocity2);
    BM_PacketPrimitive::Unpack(p, aVelocity3);
    BM_PacketPrimitive::Unpack(p, aVelocity4);
}

inline void ED_FillPacket_DynamicVariablesOneChainThermostat(
    char *p,
    const int aOuterTimeStep,
    const int aVoxelId,
    const unsigned short aNumberOfSitesPerChain,
    const double aPosition0,
    const double aPosition1,
    const double aPosition2,
    const double aPosition3,
    const double aPosition4,
    const double aVelocity0,
    const double aVelocity1,
    const double aVelocity2,
    const double aVelocity3,
    const double aVelocity4
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_DynamicVariablesOneChainThermostat::PackedSize;
    ed_hdr.mClass  = ED_CLASS::DynamicVariables;
    ed_hdr.mType   = ED_DYNAMICVARIABLES_TYPES::OneChainThermostat;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aVoxelId);
    BM_PacketPrimitive::Pack(p, aNumberOfSitesPerChain);
    BM_PacketPrimitive::Pack(p, aPosition0);
    BM_PacketPrimitive::Pack(p, aPosition1);
    BM_PacketPrimitive::Pack(p, aPosition2);
    BM_PacketPrimitive::Pack(p, aPosition3);
    BM_PacketPrimitive::Pack(p, aPosition4);
    BM_PacketPrimitive::Pack(p, aVelocity0);
    BM_PacketPrimitive::Pack(p, aVelocity1);
    BM_PacketPrimitive::Pack(p, aVelocity2);
    BM_PacketPrimitive::Pack(p, aVelocity3);
    BM_PacketPrimitive::Pack(p, aVelocity4);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_DynamicVariablesOneChainThermostat(
    const int aOuterTimeStep,
    const int aVoxelId,
    const unsigned short aNumberOfSitesPerChain,
    const double aPosition0,
    const double aPosition1,
    const double aPosition2,
    const double aPosition3,
    const double aPosition4,
    const double aVelocity0,
    const double aVelocity1,
    const double aVelocity2,
    const double aVelocity3,
    const double aVelocity4
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_DynamicVariablesOneChainThermostat::PackedSize];

    ED_FillPacket_DynamicVariablesOneChainThermostat(
    buff,
    aOuterTimeStep,
    aVoxelId,
    aNumberOfSitesPerChain,
    aPosition0,
    aPosition1,
    aPosition2,
    aPosition3,
    aPosition4,
    aVelocity0,
    aVelocity1,
    aVelocity2,
    aVelocity3,
    aVelocity4
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesOneChainThermostat::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesOneChainThermostat::PackedSize );
#endif
}

inline void ED_Emit_Tagged_DynamicVariablesOneChainThermostat(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aVoxelId,
    const unsigned short aNumberOfSitesPerChain,
    const double aPosition0,
    const double aPosition1,
    const double aPosition2,
    const double aPosition3,
    const double aPosition4,
    const double aVelocity0,
    const double aVelocity1,
    const double aVelocity2,
    const double aVelocity3,
    const double aVelocity4
    )
{
    char buff[ED_DynamicVariablesOneChainThermostat::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_DynamicVariablesOneChainThermostat(
    p,
    aOuterTimeStep,
    aVoxelId,
    aNumberOfSitesPerChain,
    aPosition0,
    aPosition1,
    aPosition2,
    aPosition3,
    aPosition4,
    aVelocity0,
    aVelocity1,
    aVelocity2,
    aVelocity3,
    aVelocity4
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesOneChainThermostat::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesOneChainThermostat::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_DynamicVariablesOneChainThermostat &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mVoxelId << " ";
    os << ed_pkt.mNumberOfSitesPerChain << " ";
    os << ed_pkt.mPosition0 << " ";
    os << ed_pkt.mPosition1 << " ";
    os << ed_pkt.mPosition2 << " ";
    os << ed_pkt.mPosition3 << " ";
    os << ed_pkt.mPosition4 << " ";
    os << ed_pkt.mVelocity0 << " ";
    os << ed_pkt.mVelocity1 << " ";
    os << ed_pkt.mVelocity2 << " ";
    os << ed_pkt.mVelocity3 << " ";
    os << ed_pkt.mVelocity4;
    return (os);
}

//=============================================================================
//====        2      9 DynamicVariablesRandomState                         ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mVoxelId;
    int  mMachineContext;
    int  mMoleculeContext;
    int  mHaveDeviate;
    double  mStoredNormalDeviate;
    int  mSeed;
    unsigned int  mX0_N0;
    unsigned int  mX0_N1;
    unsigned int  mX0_N2;
    unsigned int  mX1_N0;
    unsigned int  mX1_N1;
    unsigned int  mX1_N2;
    int  mDummy4;
    int  mDummy5;
    int  mDummy6;
    int  mDummy7;
    int  mDummy8;
    int  mDummy9;
    int  mDummy10;
    int  mDummy11;
    int  mDummy12;
    int  mDummy13;
    int  mDummy14;
    int  mDummy15;
    int  mDummy16;
    int  mDummy17;
    int  mDummy18;
    int  mDummy19;
    int  mDummy20;
    int  mDummy21;
    int  mDummy22;
    int  mDummy23;
    int  mDummy24;
    int  mDummy25;
    int  mDummy26;
    int  mDummy27;
    int  mDummy28;
    int  mDummy29;
    int  mDummy30;
    int  mDummy31;
    enum {PackedSize = 168,
          PackedTaggedSize = 168+TaggedPacket::TAGSIZE};
} ED_DynamicVariablesRandomState;

inline void ED_OpenPacket_DynamicVariablesRandomState(char *&p, ED_DynamicVariablesRandomState &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVoxelId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mMachineContext);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mMoleculeContext);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mHaveDeviate);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mStoredNormalDeviate);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSeed);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mX0_N0);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mX0_N1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mX0_N2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mX1_N0);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mX1_N1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mX1_N2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy4);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy5);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy6);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy7);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy8);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy9);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy10);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy11);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy12);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy13);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy14);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy15);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy16);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy17);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy18);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy19);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy20);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy21);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy22);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy23);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy24);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy25);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy26);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy27);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy28);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy29);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy30);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy31);
}

inline void ED_FillPacket_DynamicVariablesRandomState(char *p, ED_DynamicVariablesRandomState &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVoxelId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mMachineContext);
    BM_PacketPrimitive::Pack(p, ed_pkt.mMoleculeContext);
    BM_PacketPrimitive::Pack(p, ed_pkt.mHaveDeviate);
    BM_PacketPrimitive::Pack(p, ed_pkt.mStoredNormalDeviate);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSeed);
    BM_PacketPrimitive::Pack(p, ed_pkt.mX0_N0);
    BM_PacketPrimitive::Pack(p, ed_pkt.mX0_N1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mX0_N2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mX1_N0);
    BM_PacketPrimitive::Pack(p, ed_pkt.mX1_N1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mX1_N2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy4);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy5);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy6);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy7);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy8);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy9);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy10);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy11);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy12);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy13);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy14);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy15);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy16);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy17);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy18);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy19);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy20);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy21);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy22);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy23);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy24);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy25);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy26);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy27);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy28);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy29);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy30);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy31);
}

inline void ED_OpenPacket_DynamicVariablesRandomState(
    char *&p,
    int &aVoxelId,
    int &aMachineContext,
    int &aMoleculeContext,
    int &aHaveDeviate,
    double &aStoredNormalDeviate,
    int &aSeed,
    unsigned int &aX0_N0,
    unsigned int &aX0_N1,
    unsigned int &aX0_N2,
    unsigned int &aX1_N0,
    unsigned int &aX1_N1,
    unsigned int &aX1_N2,
    int &aDummy4,
    int &aDummy5,
    int &aDummy6,
    int &aDummy7,
    int &aDummy8,
    int &aDummy9,
    int &aDummy10,
    int &aDummy11,
    int &aDummy12,
    int &aDummy13,
    int &aDummy14,
    int &aDummy15,
    int &aDummy16,
    int &aDummy17,
    int &aDummy18,
    int &aDummy19,
    int &aDummy20,
    int &aDummy21,
    int &aDummy22,
    int &aDummy23,
    int &aDummy24,
    int &aDummy25,
    int &aDummy26,
    int &aDummy27,
    int &aDummy28,
    int &aDummy29,
    int &aDummy30,
    int &aDummy31
    )
{
    BM_PacketPrimitive::Unpack(p, aVoxelId);
    BM_PacketPrimitive::Unpack(p, aMachineContext);
    BM_PacketPrimitive::Unpack(p, aMoleculeContext);
    BM_PacketPrimitive::Unpack(p, aHaveDeviate);
    BM_PacketPrimitive::Unpack(p, aStoredNormalDeviate);
    BM_PacketPrimitive::Unpack(p, aSeed);
    BM_PacketPrimitive::Unpack(p, aX0_N0);
    BM_PacketPrimitive::Unpack(p, aX0_N1);
    BM_PacketPrimitive::Unpack(p, aX0_N2);
    BM_PacketPrimitive::Unpack(p, aX1_N0);
    BM_PacketPrimitive::Unpack(p, aX1_N1);
    BM_PacketPrimitive::Unpack(p, aX1_N2);
    BM_PacketPrimitive::Unpack(p, aDummy4);
    BM_PacketPrimitive::Unpack(p, aDummy5);
    BM_PacketPrimitive::Unpack(p, aDummy6);
    BM_PacketPrimitive::Unpack(p, aDummy7);
    BM_PacketPrimitive::Unpack(p, aDummy8);
    BM_PacketPrimitive::Unpack(p, aDummy9);
    BM_PacketPrimitive::Unpack(p, aDummy10);
    BM_PacketPrimitive::Unpack(p, aDummy11);
    BM_PacketPrimitive::Unpack(p, aDummy12);
    BM_PacketPrimitive::Unpack(p, aDummy13);
    BM_PacketPrimitive::Unpack(p, aDummy14);
    BM_PacketPrimitive::Unpack(p, aDummy15);
    BM_PacketPrimitive::Unpack(p, aDummy16);
    BM_PacketPrimitive::Unpack(p, aDummy17);
    BM_PacketPrimitive::Unpack(p, aDummy18);
    BM_PacketPrimitive::Unpack(p, aDummy19);
    BM_PacketPrimitive::Unpack(p, aDummy20);
    BM_PacketPrimitive::Unpack(p, aDummy21);
    BM_PacketPrimitive::Unpack(p, aDummy22);
    BM_PacketPrimitive::Unpack(p, aDummy23);
    BM_PacketPrimitive::Unpack(p, aDummy24);
    BM_PacketPrimitive::Unpack(p, aDummy25);
    BM_PacketPrimitive::Unpack(p, aDummy26);
    BM_PacketPrimitive::Unpack(p, aDummy27);
    BM_PacketPrimitive::Unpack(p, aDummy28);
    BM_PacketPrimitive::Unpack(p, aDummy29);
    BM_PacketPrimitive::Unpack(p, aDummy30);
    BM_PacketPrimitive::Unpack(p, aDummy31);
}

inline void ED_FillPacket_DynamicVariablesRandomState(
    char *p,
    const int aOuterTimeStep,
    const int aVoxelId,
    const int aMachineContext,
    const int aMoleculeContext,
    const int aHaveDeviate,
    const double aStoredNormalDeviate,
    const int aSeed,
    const unsigned int aX0_N0,
    const unsigned int aX0_N1,
    const unsigned int aX0_N2,
    const unsigned int aX1_N0,
    const unsigned int aX1_N1,
    const unsigned int aX1_N2,
    const int aDummy4,
    const int aDummy5,
    const int aDummy6,
    const int aDummy7,
    const int aDummy8,
    const int aDummy9,
    const int aDummy10,
    const int aDummy11,
    const int aDummy12,
    const int aDummy13,
    const int aDummy14,
    const int aDummy15,
    const int aDummy16,
    const int aDummy17,
    const int aDummy18,
    const int aDummy19,
    const int aDummy20,
    const int aDummy21,
    const int aDummy22,
    const int aDummy23,
    const int aDummy24,
    const int aDummy25,
    const int aDummy26,
    const int aDummy27,
    const int aDummy28,
    const int aDummy29,
    const int aDummy30,
    const int aDummy31
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_DynamicVariablesRandomState::PackedSize;
    ed_hdr.mClass  = ED_CLASS::DynamicVariables;
    ed_hdr.mType   = ED_DYNAMICVARIABLES_TYPES::RandomState;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aVoxelId);
    BM_PacketPrimitive::Pack(p, aMachineContext);
    BM_PacketPrimitive::Pack(p, aMoleculeContext);
    BM_PacketPrimitive::Pack(p, aHaveDeviate);
    BM_PacketPrimitive::Pack(p, aStoredNormalDeviate);
    BM_PacketPrimitive::Pack(p, aSeed);
    BM_PacketPrimitive::Pack(p, aX0_N0);
    BM_PacketPrimitive::Pack(p, aX0_N1);
    BM_PacketPrimitive::Pack(p, aX0_N2);
    BM_PacketPrimitive::Pack(p, aX1_N0);
    BM_PacketPrimitive::Pack(p, aX1_N1);
    BM_PacketPrimitive::Pack(p, aX1_N2);
    BM_PacketPrimitive::Pack(p, aDummy4);
    BM_PacketPrimitive::Pack(p, aDummy5);
    BM_PacketPrimitive::Pack(p, aDummy6);
    BM_PacketPrimitive::Pack(p, aDummy7);
    BM_PacketPrimitive::Pack(p, aDummy8);
    BM_PacketPrimitive::Pack(p, aDummy9);
    BM_PacketPrimitive::Pack(p, aDummy10);
    BM_PacketPrimitive::Pack(p, aDummy11);
    BM_PacketPrimitive::Pack(p, aDummy12);
    BM_PacketPrimitive::Pack(p, aDummy13);
    BM_PacketPrimitive::Pack(p, aDummy14);
    BM_PacketPrimitive::Pack(p, aDummy15);
    BM_PacketPrimitive::Pack(p, aDummy16);
    BM_PacketPrimitive::Pack(p, aDummy17);
    BM_PacketPrimitive::Pack(p, aDummy18);
    BM_PacketPrimitive::Pack(p, aDummy19);
    BM_PacketPrimitive::Pack(p, aDummy20);
    BM_PacketPrimitive::Pack(p, aDummy21);
    BM_PacketPrimitive::Pack(p, aDummy22);
    BM_PacketPrimitive::Pack(p, aDummy23);
    BM_PacketPrimitive::Pack(p, aDummy24);
    BM_PacketPrimitive::Pack(p, aDummy25);
    BM_PacketPrimitive::Pack(p, aDummy26);
    BM_PacketPrimitive::Pack(p, aDummy27);
    BM_PacketPrimitive::Pack(p, aDummy28);
    BM_PacketPrimitive::Pack(p, aDummy29);
    BM_PacketPrimitive::Pack(p, aDummy30);
    BM_PacketPrimitive::Pack(p, aDummy31);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_DynamicVariablesRandomState(
    const int aOuterTimeStep,
    const int aVoxelId,
    const int aMachineContext,
    const int aMoleculeContext,
    const int aHaveDeviate,
    const double aStoredNormalDeviate,
    const int aSeed,
    const unsigned int aX0_N0,
    const unsigned int aX0_N1,
    const unsigned int aX0_N2,
    const unsigned int aX1_N0,
    const unsigned int aX1_N1,
    const unsigned int aX1_N2,
    const int aDummy4,
    const int aDummy5,
    const int aDummy6,
    const int aDummy7,
    const int aDummy8,
    const int aDummy9,
    const int aDummy10,
    const int aDummy11,
    const int aDummy12,
    const int aDummy13,
    const int aDummy14,
    const int aDummy15,
    const int aDummy16,
    const int aDummy17,
    const int aDummy18,
    const int aDummy19,
    const int aDummy20,
    const int aDummy21,
    const int aDummy22,
    const int aDummy23,
    const int aDummy24,
    const int aDummy25,
    const int aDummy26,
    const int aDummy27,
    const int aDummy28,
    const int aDummy29,
    const int aDummy30,
    const int aDummy31
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_DynamicVariablesRandomState::PackedSize];

    ED_FillPacket_DynamicVariablesRandomState(
    buff,
    aOuterTimeStep,
    aVoxelId,
    aMachineContext,
    aMoleculeContext,
    aHaveDeviate,
    aStoredNormalDeviate,
    aSeed,
    aX0_N0,
    aX0_N1,
    aX0_N2,
    aX1_N0,
    aX1_N1,
    aX1_N2,
    aDummy4,
    aDummy5,
    aDummy6,
    aDummy7,
    aDummy8,
    aDummy9,
    aDummy10,
    aDummy11,
    aDummy12,
    aDummy13,
    aDummy14,
    aDummy15,
    aDummy16,
    aDummy17,
    aDummy18,
    aDummy19,
    aDummy20,
    aDummy21,
    aDummy22,
    aDummy23,
    aDummy24,
    aDummy25,
    aDummy26,
    aDummy27,
    aDummy28,
    aDummy29,
    aDummy30,
    aDummy31
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesRandomState::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesRandomState::PackedSize );
#endif
}

inline void ED_Emit_Tagged_DynamicVariablesRandomState(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aVoxelId,
    const int aMachineContext,
    const int aMoleculeContext,
    const int aHaveDeviate,
    const double aStoredNormalDeviate,
    const int aSeed,
    const unsigned int aX0_N0,
    const unsigned int aX0_N1,
    const unsigned int aX0_N2,
    const unsigned int aX1_N0,
    const unsigned int aX1_N1,
    const unsigned int aX1_N2,
    const int aDummy4,
    const int aDummy5,
    const int aDummy6,
    const int aDummy7,
    const int aDummy8,
    const int aDummy9,
    const int aDummy10,
    const int aDummy11,
    const int aDummy12,
    const int aDummy13,
    const int aDummy14,
    const int aDummy15,
    const int aDummy16,
    const int aDummy17,
    const int aDummy18,
    const int aDummy19,
    const int aDummy20,
    const int aDummy21,
    const int aDummy22,
    const int aDummy23,
    const int aDummy24,
    const int aDummy25,
    const int aDummy26,
    const int aDummy27,
    const int aDummy28,
    const int aDummy29,
    const int aDummy30,
    const int aDummy31
    )
{
    char buff[ED_DynamicVariablesRandomState::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_DynamicVariablesRandomState(
    p,
    aOuterTimeStep,
    aVoxelId,
    aMachineContext,
    aMoleculeContext,
    aHaveDeviate,
    aStoredNormalDeviate,
    aSeed,
    aX0_N0,
    aX0_N1,
    aX0_N2,
    aX1_N0,
    aX1_N1,
    aX1_N2,
    aDummy4,
    aDummy5,
    aDummy6,
    aDummy7,
    aDummy8,
    aDummy9,
    aDummy10,
    aDummy11,
    aDummy12,
    aDummy13,
    aDummy14,
    aDummy15,
    aDummy16,
    aDummy17,
    aDummy18,
    aDummy19,
    aDummy20,
    aDummy21,
    aDummy22,
    aDummy23,
    aDummy24,
    aDummy25,
    aDummy26,
    aDummy27,
    aDummy28,
    aDummy29,
    aDummy30,
    aDummy31
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_DynamicVariablesRandomState::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_DynamicVariablesRandomState::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_DynamicVariablesRandomState &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mVoxelId << " ";
    os << ed_pkt.mMachineContext << " ";
    os << ed_pkt.mMoleculeContext << " ";
    os << ed_pkt.mHaveDeviate << " ";
    os << ed_pkt.mStoredNormalDeviate << " ";
    os << ed_pkt.mSeed << " ";
    os << ed_pkt.mX0_N0 << " ";
    os << ed_pkt.mX0_N1 << " ";
    os << ed_pkt.mX0_N2 << " ";
    os << ed_pkt.mX1_N0 << " ";
    os << ed_pkt.mX1_N1 << " ";
    os << ed_pkt.mX1_N2 << " ";
    os << ed_pkt.mDummy4 << " ";
    os << ed_pkt.mDummy5 << " ";
    os << ed_pkt.mDummy6 << " ";
    os << ed_pkt.mDummy7 << " ";
    os << ed_pkt.mDummy8 << " ";
    os << ed_pkt.mDummy9 << " ";
    os << ed_pkt.mDummy10 << " ";
    os << ed_pkt.mDummy11 << " ";
    os << ed_pkt.mDummy12 << " ";
    os << ed_pkt.mDummy13 << " ";
    os << ed_pkt.mDummy14 << " ";
    os << ed_pkt.mDummy15 << " ";
    os << ed_pkt.mDummy16 << " ";
    os << ed_pkt.mDummy17 << " ";
    os << ed_pkt.mDummy18 << " ";
    os << ed_pkt.mDummy19 << " ";
    os << ed_pkt.mDummy20 << " ";
    os << ed_pkt.mDummy21 << " ";
    os << ed_pkt.mDummy22 << " ";
    os << ed_pkt.mDummy23 << " ";
    os << ed_pkt.mDummy24 << " ";
    os << ed_pkt.mDummy25 << " ";
    os << ed_pkt.mDummy26 << " ";
    os << ed_pkt.mDummy27 << " ";
    os << ed_pkt.mDummy28 << " ";
    os << ed_pkt.mDummy29 << " ";
    os << ed_pkt.mDummy30 << " ";
    os << ed_pkt.mDummy31;
    return (os);
}

//=============================================================================
//====        3      1 EnergyBonded                                        ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mFragmentId;
    int  mFragmentCount;
    double  mKineticEnergy;
    double  mBondEnergy;
    double  mAngleEnergy;
    double  mUBEnergy;
    double  mTorsionEnergy;
    double  mImproperEnergy;
    double  mExcludedChargeEnergy;
    double  mExcludedLennardJonesEnergy;
    double  m14ChargeEnergy;
    double  m14LennardJonesEnergy;
    enum {PackedSize = 92,
          PackedTaggedSize = 92+TaggedPacket::TAGSIZE};
} ED_EnergyBonded;

inline void ED_OpenPacket_EnergyBonded(char *&p, ED_EnergyBonded &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mFragmentId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mFragmentCount);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mKineticEnergy);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mBondEnergy);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mAngleEnergy);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mUBEnergy);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mTorsionEnergy);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mImproperEnergy);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mExcludedChargeEnergy);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mExcludedLennardJonesEnergy);
    BM_PacketPrimitive::Unpack(p, ed_pkt.m14ChargeEnergy);
    BM_PacketPrimitive::Unpack(p, ed_pkt.m14LennardJonesEnergy);
}

inline void ED_FillPacket_EnergyBonded(char *p, ED_EnergyBonded &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mFragmentId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mFragmentCount);
    BM_PacketPrimitive::Pack(p, ed_pkt.mKineticEnergy);
    BM_PacketPrimitive::Pack(p, ed_pkt.mBondEnergy);
    BM_PacketPrimitive::Pack(p, ed_pkt.mAngleEnergy);
    BM_PacketPrimitive::Pack(p, ed_pkt.mUBEnergy);
    BM_PacketPrimitive::Pack(p, ed_pkt.mTorsionEnergy);
    BM_PacketPrimitive::Pack(p, ed_pkt.mImproperEnergy);
    BM_PacketPrimitive::Pack(p, ed_pkt.mExcludedChargeEnergy);
    BM_PacketPrimitive::Pack(p, ed_pkt.mExcludedLennardJonesEnergy);
    BM_PacketPrimitive::Pack(p, ed_pkt.m14ChargeEnergy);
    BM_PacketPrimitive::Pack(p, ed_pkt.m14LennardJonesEnergy);
}

inline void ED_OpenPacket_EnergyBonded(
    char *&p,
    int &aFragmentId,
    int &aFragmentCount,
    double &aKineticEnergy,
    double &aBondEnergy,
    double &aAngleEnergy,
    double &aUBEnergy,
    double &aTorsionEnergy,
    double &aImproperEnergy,
    double &aExcludedChargeEnergy,
    double &aExcludedLennardJonesEnergy,
    double &a14ChargeEnergy,
    double &a14LennardJonesEnergy
    )
{
    BM_PacketPrimitive::Unpack(p, aFragmentId);
    BM_PacketPrimitive::Unpack(p, aFragmentCount);
    BM_PacketPrimitive::Unpack(p, aKineticEnergy);
    BM_PacketPrimitive::Unpack(p, aBondEnergy);
    BM_PacketPrimitive::Unpack(p, aAngleEnergy);
    BM_PacketPrimitive::Unpack(p, aUBEnergy);
    BM_PacketPrimitive::Unpack(p, aTorsionEnergy);
    BM_PacketPrimitive::Unpack(p, aImproperEnergy);
    BM_PacketPrimitive::Unpack(p, aExcludedChargeEnergy);
    BM_PacketPrimitive::Unpack(p, aExcludedLennardJonesEnergy);
    BM_PacketPrimitive::Unpack(p, a14ChargeEnergy);
    BM_PacketPrimitive::Unpack(p, a14LennardJonesEnergy);
}

inline void ED_FillPacket_EnergyBonded(
    char *p,
    const int aOuterTimeStep,
    const int aFragmentId,
    const int aFragmentCount,
    const double aKineticEnergy,
    const double aBondEnergy,
    const double aAngleEnergy,
    const double aUBEnergy,
    const double aTorsionEnergy,
    const double aImproperEnergy,
    const double aExcludedChargeEnergy,
    const double aExcludedLennardJonesEnergy,
    const double a14ChargeEnergy,
    const double a14LennardJonesEnergy
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_EnergyBonded::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Energy;
    ed_hdr.mType   = ED_ENERGY_TYPES::Bonded;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aFragmentId);
    BM_PacketPrimitive::Pack(p, aFragmentCount);
    BM_PacketPrimitive::Pack(p, aKineticEnergy);
    BM_PacketPrimitive::Pack(p, aBondEnergy);
    BM_PacketPrimitive::Pack(p, aAngleEnergy);
    BM_PacketPrimitive::Pack(p, aUBEnergy);
    BM_PacketPrimitive::Pack(p, aTorsionEnergy);
    BM_PacketPrimitive::Pack(p, aImproperEnergy);
    BM_PacketPrimitive::Pack(p, aExcludedChargeEnergy);
    BM_PacketPrimitive::Pack(p, aExcludedLennardJonesEnergy);
    BM_PacketPrimitive::Pack(p, a14ChargeEnergy);
    BM_PacketPrimitive::Pack(p, a14LennardJonesEnergy);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_EnergyBonded(
    const int aOuterTimeStep,
    const int aFragmentId,
    const int aFragmentCount,
    const double aKineticEnergy,
    const double aBondEnergy,
    const double aAngleEnergy,
    const double aUBEnergy,
    const double aTorsionEnergy,
    const double aImproperEnergy,
    const double aExcludedChargeEnergy,
    const double aExcludedLennardJonesEnergy,
    const double a14ChargeEnergy,
    const double a14LennardJonesEnergy
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_EnergyBonded::PackedSize];

    ED_FillPacket_EnergyBonded(
    buff,
    aOuterTimeStep,
    aFragmentId,
    aFragmentCount,
    aKineticEnergy,
    aBondEnergy,
    aAngleEnergy,
    aUBEnergy,
    aTorsionEnergy,
    aImproperEnergy,
    aExcludedChargeEnergy,
    aExcludedLennardJonesEnergy,
    a14ChargeEnergy,
    a14LennardJonesEnergy
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_EnergyBonded::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_EnergyBonded::PackedSize );
#endif
}

inline void ED_Emit_Tagged_EnergyBonded(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aFragmentId,
    const int aFragmentCount,
    const double aKineticEnergy,
    const double aBondEnergy,
    const double aAngleEnergy,
    const double aUBEnergy,
    const double aTorsionEnergy,
    const double aImproperEnergy,
    const double aExcludedChargeEnergy,
    const double aExcludedLennardJonesEnergy,
    const double a14ChargeEnergy,
    const double a14LennardJonesEnergy
    )
{
    char buff[ED_EnergyBonded::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_EnergyBonded(
    p,
    aOuterTimeStep,
    aFragmentId,
    aFragmentCount,
    aKineticEnergy,
    aBondEnergy,
    aAngleEnergy,
    aUBEnergy,
    aTorsionEnergy,
    aImproperEnergy,
    aExcludedChargeEnergy,
    aExcludedLennardJonesEnergy,
    a14ChargeEnergy,
    a14LennardJonesEnergy
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_EnergyBonded::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_EnergyBonded::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_EnergyBonded &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mFragmentId << " ";
    os << ed_pkt.mFragmentCount << " ";
    os << ed_pkt.mKineticEnergy << " ";
    os << ed_pkt.mBondEnergy << " ";
    os << ed_pkt.mAngleEnergy << " ";
    os << ed_pkt.mUBEnergy << " ";
    os << ed_pkt.mTorsionEnergy << " ";
    os << ed_pkt.mImproperEnergy << " ";
    os << ed_pkt.mExcludedChargeEnergy << " ";
    os << ed_pkt.mExcludedLennardJonesEnergy << " ";
    os << ed_pkt.m14ChargeEnergy << " ";
    os << ed_pkt.m14LennardJonesEnergy;
    return (os);
}

//=============================================================================
//====        3      2 EnergyNonBonded                                     ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mOrdinal;
    int  mCount;
    double  mChargeEnergyPartialSum;
    double  mLennardJonesEnergyPartialSum;
    double  mEwaldKSpaceEnergyPartialSum;
    enum {PackedSize = 36,
          PackedTaggedSize = 36+TaggedPacket::TAGSIZE};
} ED_EnergyNonBonded;

inline void ED_OpenPacket_EnergyNonBonded(char *&p, ED_EnergyNonBonded &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mOrdinal);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mCount);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mChargeEnergyPartialSum);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mLennardJonesEnergyPartialSum);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mEwaldKSpaceEnergyPartialSum);
}

inline void ED_FillPacket_EnergyNonBonded(char *p, ED_EnergyNonBonded &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mOrdinal);
    BM_PacketPrimitive::Pack(p, ed_pkt.mCount);
    BM_PacketPrimitive::Pack(p, ed_pkt.mChargeEnergyPartialSum);
    BM_PacketPrimitive::Pack(p, ed_pkt.mLennardJonesEnergyPartialSum);
    BM_PacketPrimitive::Pack(p, ed_pkt.mEwaldKSpaceEnergyPartialSum);
}

inline void ED_OpenPacket_EnergyNonBonded(
    char *&p,
    int &aOrdinal,
    int &aCount,
    double &aChargeEnergyPartialSum,
    double &aLennardJonesEnergyPartialSum,
    double &aEwaldKSpaceEnergyPartialSum
    )
{
    BM_PacketPrimitive::Unpack(p, aOrdinal);
    BM_PacketPrimitive::Unpack(p, aCount);
    BM_PacketPrimitive::Unpack(p, aChargeEnergyPartialSum);
    BM_PacketPrimitive::Unpack(p, aLennardJonesEnergyPartialSum);
    BM_PacketPrimitive::Unpack(p, aEwaldKSpaceEnergyPartialSum);
}

inline void ED_FillPacket_EnergyNonBonded(
    char *p,
    const int aOuterTimeStep,
    const int aOrdinal,
    const int aCount,
    const double aChargeEnergyPartialSum,
    const double aLennardJonesEnergyPartialSum,
    const double aEwaldKSpaceEnergyPartialSum
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_EnergyNonBonded::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Energy;
    ed_hdr.mType   = ED_ENERGY_TYPES::NonBonded;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aOrdinal);
    BM_PacketPrimitive::Pack(p, aCount);
    BM_PacketPrimitive::Pack(p, aChargeEnergyPartialSum);
    BM_PacketPrimitive::Pack(p, aLennardJonesEnergyPartialSum);
    BM_PacketPrimitive::Pack(p, aEwaldKSpaceEnergyPartialSum);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_EnergyNonBonded(
    const int aOuterTimeStep,
    const int aOrdinal,
    const int aCount,
    const double aChargeEnergyPartialSum,
    const double aLennardJonesEnergyPartialSum,
    const double aEwaldKSpaceEnergyPartialSum
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_EnergyNonBonded::PackedSize];

    ED_FillPacket_EnergyNonBonded(
    buff,
    aOuterTimeStep,
    aOrdinal,
    aCount,
    aChargeEnergyPartialSum,
    aLennardJonesEnergyPartialSum,
    aEwaldKSpaceEnergyPartialSum
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_EnergyNonBonded::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_EnergyNonBonded::PackedSize );
#endif
}

inline void ED_Emit_Tagged_EnergyNonBonded(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aOrdinal,
    const int aCount,
    const double aChargeEnergyPartialSum,
    const double aLennardJonesEnergyPartialSum,
    const double aEwaldKSpaceEnergyPartialSum
    )
{
    char buff[ED_EnergyNonBonded::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_EnergyNonBonded(
    p,
    aOuterTimeStep,
    aOrdinal,
    aCount,
    aChargeEnergyPartialSum,
    aLennardJonesEnergyPartialSum,
    aEwaldKSpaceEnergyPartialSum
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_EnergyNonBonded::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_EnergyNonBonded::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_EnergyNonBonded &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mOrdinal << " ";
    os << ed_pkt.mCount << " ";
    os << ed_pkt.mChargeEnergyPartialSum << " ";
    os << ed_pkt.mLennardJonesEnergyPartialSum << " ";
    os << ed_pkt.mEwaldKSpaceEnergyPartialSum;
    return (os);
}

//=============================================================================
//====        3      3 EnergyFree                                          ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    double  mDHDLambda;
    enum {PackedSize = 12,
          PackedTaggedSize = 12+TaggedPacket::TAGSIZE};
} ED_EnergyFree;

inline void ED_OpenPacket_EnergyFree(char *&p, ED_EnergyFree &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDHDLambda);
}

inline void ED_FillPacket_EnergyFree(char *p, ED_EnergyFree &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDHDLambda);
}

inline void ED_OpenPacket_EnergyFree(
    char *&p,
    double &aDHDLambda
    )
{
    BM_PacketPrimitive::Unpack(p, aDHDLambda);
}

inline void ED_FillPacket_EnergyFree(
    char *p,
    const int aOuterTimeStep,
    const double aDHDLambda
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_EnergyFree::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Energy;
    ed_hdr.mType   = ED_ENERGY_TYPES::Free;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aDHDLambda);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_EnergyFree(
    const int aOuterTimeStep,
    const double aDHDLambda
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_EnergyFree::PackedSize];

    ED_FillPacket_EnergyFree(
    buff,
    aOuterTimeStep,
    aDHDLambda
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_EnergyFree::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_EnergyFree::PackedSize );
#endif
}

inline void ED_Emit_Tagged_EnergyFree(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const double aDHDLambda
    )
{
    char buff[ED_EnergyFree::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_EnergyFree(
    p,
    aOuterTimeStep,
    aDHDLambda
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_EnergyFree::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_EnergyFree::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_EnergyFree &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mDHDLambda;
    return (os);
}

//=============================================================================
//====        4      1 InformationMolecularSystemDefinition                ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mMSDCheckSum;
    unsigned  mSiteCount;
    unsigned  mFragmentCount;
    enum {PackedSize = 16,
          PackedTaggedSize = 16+TaggedPacket::TAGSIZE};
} ED_InformationMolecularSystemDefinition;

inline void ED_OpenPacket_InformationMolecularSystemDefinition(char *&p, ED_InformationMolecularSystemDefinition &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mMSDCheckSum);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSiteCount);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mFragmentCount);
}

inline void ED_FillPacket_InformationMolecularSystemDefinition(char *p, ED_InformationMolecularSystemDefinition &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mMSDCheckSum);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSiteCount);
    BM_PacketPrimitive::Pack(p, ed_pkt.mFragmentCount);
}

inline void ED_OpenPacket_InformationMolecularSystemDefinition(
    char *&p,
    unsigned &aMSDCheckSum,
    unsigned &aSiteCount,
    unsigned &aFragmentCount
    )
{
    BM_PacketPrimitive::Unpack(p, aMSDCheckSum);
    BM_PacketPrimitive::Unpack(p, aSiteCount);
    BM_PacketPrimitive::Unpack(p, aFragmentCount);
}

inline void ED_FillPacket_InformationMolecularSystemDefinition(
    char *p,
    const int aOuterTimeStep,
    const unsigned aMSDCheckSum,
    const unsigned aSiteCount,
    const unsigned aFragmentCount
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_InformationMolecularSystemDefinition::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Information;
    ed_hdr.mType   = ED_INFORMATION_TYPES::MolecularSystemDefinition;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aMSDCheckSum);
    BM_PacketPrimitive::Pack(p, aSiteCount);
    BM_PacketPrimitive::Pack(p, aFragmentCount);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_InformationMolecularSystemDefinition(
    const int aOuterTimeStep,
    const unsigned aMSDCheckSum,
    const unsigned aSiteCount,
    const unsigned aFragmentCount
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_InformationMolecularSystemDefinition::PackedSize];

    ED_FillPacket_InformationMolecularSystemDefinition(
    buff,
    aOuterTimeStep,
    aMSDCheckSum,
    aSiteCount,
    aFragmentCount
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationMolecularSystemDefinition::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationMolecularSystemDefinition::PackedSize );
#endif
}

inline void ED_Emit_Tagged_InformationMolecularSystemDefinition(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const unsigned aMSDCheckSum,
    const unsigned aSiteCount,
    const unsigned aFragmentCount
    )
{
    char buff[ED_InformationMolecularSystemDefinition::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_InformationMolecularSystemDefinition(
    p,
    aOuterTimeStep,
    aMSDCheckSum,
    aSiteCount,
    aFragmentCount
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationMolecularSystemDefinition::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationMolecularSystemDefinition::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_InformationMolecularSystemDefinition &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mMSDCheckSum << " ";
    os << ed_pkt.mSiteCount << " ";
    os << ed_pkt.mFragmentCount;
    return (os);
}

//=============================================================================
//====        4      2 InformationRunTimeConfiguration                     ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mVoxelCount;
    unsigned  mNonBondedEngineCount;
    enum {PackedSize = 12,
          PackedTaggedSize = 12+TaggedPacket::TAGSIZE};
} ED_InformationRunTimeConfiguration;

inline void ED_OpenPacket_InformationRunTimeConfiguration(char *&p, ED_InformationRunTimeConfiguration &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVoxelCount);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mNonBondedEngineCount);
}

inline void ED_FillPacket_InformationRunTimeConfiguration(char *p, ED_InformationRunTimeConfiguration &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVoxelCount);
    BM_PacketPrimitive::Pack(p, ed_pkt.mNonBondedEngineCount);
}

inline void ED_OpenPacket_InformationRunTimeConfiguration(
    char *&p,
    unsigned &aVoxelCount,
    unsigned &aNonBondedEngineCount
    )
{
    BM_PacketPrimitive::Unpack(p, aVoxelCount);
    BM_PacketPrimitive::Unpack(p, aNonBondedEngineCount);
}

inline void ED_FillPacket_InformationRunTimeConfiguration(
    char *p,
    const int aOuterTimeStep,
    const unsigned aVoxelCount,
    const unsigned aNonBondedEngineCount
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_InformationRunTimeConfiguration::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Information;
    ed_hdr.mType   = ED_INFORMATION_TYPES::RunTimeConfiguration;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aVoxelCount);
    BM_PacketPrimitive::Pack(p, aNonBondedEngineCount);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_InformationRunTimeConfiguration(
    const int aOuterTimeStep,
    const unsigned aVoxelCount,
    const unsigned aNonBondedEngineCount
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_InformationRunTimeConfiguration::PackedSize];

    ED_FillPacket_InformationRunTimeConfiguration(
    buff,
    aOuterTimeStep,
    aVoxelCount,
    aNonBondedEngineCount
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationRunTimeConfiguration::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationRunTimeConfiguration::PackedSize );
#endif
}

inline void ED_Emit_Tagged_InformationRunTimeConfiguration(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const unsigned aVoxelCount,
    const unsigned aNonBondedEngineCount
    )
{
    char buff[ED_InformationRunTimeConfiguration::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_InformationRunTimeConfiguration(
    p,
    aOuterTimeStep,
    aVoxelCount,
    aNonBondedEngineCount
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationRunTimeConfiguration::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationRunTimeConfiguration::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_InformationRunTimeConfiguration &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mVoxelCount << " ";
    os << ed_pkt.mNonBondedEngineCount;
    return (os);
}

//=============================================================================
//====        4      3 InformationUDFCount                                 ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mCount;
    enum {PackedSize = 8,
          PackedTaggedSize = 8+TaggedPacket::TAGSIZE};
} ED_InformationUDFCount;

inline void ED_OpenPacket_InformationUDFCount(char *&p, ED_InformationUDFCount &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mCount);
}

inline void ED_FillPacket_InformationUDFCount(char *p, ED_InformationUDFCount &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mCount);
}

inline void ED_OpenPacket_InformationUDFCount(
    char *&p,
    int &aCount
    )
{
    BM_PacketPrimitive::Unpack(p, aCount);
}

inline void ED_FillPacket_InformationUDFCount(
    char *p,
    const int aOuterTimeStep,
    const int aCount
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_InformationUDFCount::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Information;
    ed_hdr.mType   = ED_INFORMATION_TYPES::UDFCount;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aCount);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_InformationUDFCount(
    const int aOuterTimeStep,
    const int aCount
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_InformationUDFCount::PackedSize];

    ED_FillPacket_InformationUDFCount(
    buff,
    aOuterTimeStep,
    aCount
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationUDFCount::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationUDFCount::PackedSize );
#endif
}

inline void ED_Emit_Tagged_InformationUDFCount(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aCount
    )
{
    char buff[ED_InformationUDFCount::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_InformationUDFCount(
    p,
    aOuterTimeStep,
    aCount
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationUDFCount::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationUDFCount::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_InformationUDFCount &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mCount;
    return (os);
}

//=============================================================================
//====        4      4 InformationFrameContents                            ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mContents;
    enum {PackedSize = 8,
          PackedTaggedSize = 8+TaggedPacket::TAGSIZE};
} ED_InformationFrameContents;

inline void ED_OpenPacket_InformationFrameContents(char *&p, ED_InformationFrameContents &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mContents);
}

inline void ED_FillPacket_InformationFrameContents(char *p, ED_InformationFrameContents &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mContents);
}

inline void ED_OpenPacket_InformationFrameContents(
    char *&p,
    unsigned &aContents
    )
{
    BM_PacketPrimitive::Unpack(p, aContents);
}

inline void ED_FillPacket_InformationFrameContents(
    char *p,
    const int aOuterTimeStep,
    const unsigned aContents
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_InformationFrameContents::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Information;
    ed_hdr.mType   = ED_INFORMATION_TYPES::FrameContents;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aContents);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_InformationFrameContents(
    const int aOuterTimeStep,
    const unsigned aContents
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_InformationFrameContents::PackedSize];

    ED_FillPacket_InformationFrameContents(
    buff,
    aOuterTimeStep,
    aContents
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationFrameContents::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationFrameContents::PackedSize );
#endif
}

inline void ED_Emit_Tagged_InformationFrameContents(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const unsigned aContents
    )
{
    char buff[ED_InformationFrameContents::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_InformationFrameContents(
    p,
    aOuterTimeStep,
    aContents
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationFrameContents::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationFrameContents::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_InformationFrameContents &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mContents;
    return (os);
}

//=============================================================================
//====        4      5 InformationConstraints                              ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mConstraintCount;
    unsigned  mDegreesOfFreedomCount;
    unsigned  mDummy1;
    enum {PackedSize = 16,
          PackedTaggedSize = 16+TaggedPacket::TAGSIZE};
} ED_InformationConstraints;

inline void ED_OpenPacket_InformationConstraints(char *&p, ED_InformationConstraints &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mConstraintCount);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDegreesOfFreedomCount);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDummy1);
}

inline void ED_FillPacket_InformationConstraints(char *p, ED_InformationConstraints &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mConstraintCount);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDegreesOfFreedomCount);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDummy1);
}

inline void ED_OpenPacket_InformationConstraints(
    char *&p,
    unsigned &aConstraintCount,
    unsigned &aDegreesOfFreedomCount,
    unsigned &aDummy1
    )
{
    BM_PacketPrimitive::Unpack(p, aConstraintCount);
    BM_PacketPrimitive::Unpack(p, aDegreesOfFreedomCount);
    BM_PacketPrimitive::Unpack(p, aDummy1);
}

inline void ED_FillPacket_InformationConstraints(
    char *p,
    const int aOuterTimeStep,
    const unsigned aConstraintCount,
    const unsigned aDegreesOfFreedomCount,
    const unsigned aDummy1
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_InformationConstraints::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Information;
    ed_hdr.mType   = ED_INFORMATION_TYPES::Constraints;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aConstraintCount);
    BM_PacketPrimitive::Pack(p, aDegreesOfFreedomCount);
    BM_PacketPrimitive::Pack(p, aDummy1);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_InformationConstraints(
    const int aOuterTimeStep,
    const unsigned aConstraintCount,
    const unsigned aDegreesOfFreedomCount,
    const unsigned aDummy1
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_InformationConstraints::PackedSize];

    ED_FillPacket_InformationConstraints(
    buff,
    aOuterTimeStep,
    aConstraintCount,
    aDegreesOfFreedomCount,
    aDummy1
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationConstraints::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationConstraints::PackedSize );
#endif
}

inline void ED_Emit_Tagged_InformationConstraints(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const unsigned aConstraintCount,
    const unsigned aDegreesOfFreedomCount,
    const unsigned aDummy1
    )
{
    char buff[ED_InformationConstraints::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_InformationConstraints(
    p,
    aOuterTimeStep,
    aConstraintCount,
    aDegreesOfFreedomCount,
    aDummy1
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationConstraints::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationConstraints::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_InformationConstraints &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mConstraintCount << " ";
    os << ed_pkt.mDegreesOfFreedomCount << " ";
    os << ed_pkt.mDummy1;
    return (os);
}

//=============================================================================
//====        4      6 InformationRTP                                      ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mCoulombMethod;
    double  mInnerTimeStepInPicoSeconds;
    int  mNumberOfInnerTimeSteps;
    double  mOuterTimeStepInPicoSeconds;
    int  mNumberOfOuterTimeSteps;
    int  mNumberOfRESPALevels;
    double  mPressureControlPistonMass;
    double  mPressureControlTarget;
    double  mVelocityResampleTargetTemperature;
    int  mVelocityResamplePeriodInOTS;
    int  mEnsemble;
    unsigned  mSnapshotPeriodInOTS;
    unsigned  mVelocityResampleRandomSeed;
    unsigned  mSourceID;
    unsigned  mExecutableID;
    unsigned  mNumberOfTwoChainThermostats;
    unsigned  mJobID;
    XYZ  mDynamicBoundingBoxMin;
    XYZ  mDynamicBoundingBoxMax;
    enum {PackedSize = 140,
          PackedTaggedSize = 140+TaggedPacket::TAGSIZE};
} ED_InformationRTP;

inline void ED_OpenPacket_InformationRTP(char *&p, ED_InformationRTP &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mCoulombMethod);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mInnerTimeStepInPicoSeconds);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mNumberOfInnerTimeSteps);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mOuterTimeStepInPicoSeconds);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mNumberOfOuterTimeSteps);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mNumberOfRESPALevels);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPressureControlPistonMass);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mPressureControlTarget);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocityResampleTargetTemperature);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocityResamplePeriodInOTS);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mEnsemble);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSnapshotPeriodInOTS);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mVelocityResampleRandomSeed);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSourceID);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mExecutableID);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mNumberOfTwoChainThermostats);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mJobID);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDynamicBoundingBoxMin);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mDynamicBoundingBoxMax);
}

inline void ED_FillPacket_InformationRTP(char *p, ED_InformationRTP &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mCoulombMethod);
    BM_PacketPrimitive::Pack(p, ed_pkt.mInnerTimeStepInPicoSeconds);
    BM_PacketPrimitive::Pack(p, ed_pkt.mNumberOfInnerTimeSteps);
    BM_PacketPrimitive::Pack(p, ed_pkt.mOuterTimeStepInPicoSeconds);
    BM_PacketPrimitive::Pack(p, ed_pkt.mNumberOfOuterTimeSteps);
    BM_PacketPrimitive::Pack(p, ed_pkt.mNumberOfRESPALevels);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPressureControlPistonMass);
    BM_PacketPrimitive::Pack(p, ed_pkt.mPressureControlTarget);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocityResampleTargetTemperature);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocityResamplePeriodInOTS);
    BM_PacketPrimitive::Pack(p, ed_pkt.mEnsemble);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSnapshotPeriodInOTS);
    BM_PacketPrimitive::Pack(p, ed_pkt.mVelocityResampleRandomSeed);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSourceID);
    BM_PacketPrimitive::Pack(p, ed_pkt.mExecutableID);
    BM_PacketPrimitive::Pack(p, ed_pkt.mNumberOfTwoChainThermostats);
    BM_PacketPrimitive::Pack(p, ed_pkt.mJobID);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDynamicBoundingBoxMin);
    BM_PacketPrimitive::Pack(p, ed_pkt.mDynamicBoundingBoxMax);
}

inline void ED_OpenPacket_InformationRTP(
    char *&p,
    int &aCoulombMethod,
    double &aInnerTimeStepInPicoSeconds,
    int &aNumberOfInnerTimeSteps,
    double &aOuterTimeStepInPicoSeconds,
    int &aNumberOfOuterTimeSteps,
    int &aNumberOfRESPALevels,
    double &aPressureControlPistonMass,
    double &aPressureControlTarget,
    double &aVelocityResampleTargetTemperature,
    int &aVelocityResamplePeriodInOTS,
    int &aEnsemble,
    unsigned &aSnapshotPeriodInOTS,
    unsigned &aVelocityResampleRandomSeed,
    unsigned &aSourceID,
    unsigned &aExecutableID,
    unsigned &aNumberOfTwoChainThermostats,
    unsigned &aJobID,
    XYZ &aDynamicBoundingBoxMin,
    XYZ &aDynamicBoundingBoxMax
    )
{
    BM_PacketPrimitive::Unpack(p, aCoulombMethod);
    BM_PacketPrimitive::Unpack(p, aInnerTimeStepInPicoSeconds);
    BM_PacketPrimitive::Unpack(p, aNumberOfInnerTimeSteps);
    BM_PacketPrimitive::Unpack(p, aOuterTimeStepInPicoSeconds);
    BM_PacketPrimitive::Unpack(p, aNumberOfOuterTimeSteps);
    BM_PacketPrimitive::Unpack(p, aNumberOfRESPALevels);
    BM_PacketPrimitive::Unpack(p, aPressureControlPistonMass);
    BM_PacketPrimitive::Unpack(p, aPressureControlTarget);
    BM_PacketPrimitive::Unpack(p, aVelocityResampleTargetTemperature);
    BM_PacketPrimitive::Unpack(p, aVelocityResamplePeriodInOTS);
    BM_PacketPrimitive::Unpack(p, aEnsemble);
    BM_PacketPrimitive::Unpack(p, aSnapshotPeriodInOTS);
    BM_PacketPrimitive::Unpack(p, aVelocityResampleRandomSeed);
    BM_PacketPrimitive::Unpack(p, aSourceID);
    BM_PacketPrimitive::Unpack(p, aExecutableID);
    BM_PacketPrimitive::Unpack(p, aNumberOfTwoChainThermostats);
    BM_PacketPrimitive::Unpack(p, aJobID);
    BM_PacketPrimitive::Unpack(p, aDynamicBoundingBoxMin);
    BM_PacketPrimitive::Unpack(p, aDynamicBoundingBoxMax);
}

inline void ED_FillPacket_InformationRTP(
    char *p,
    const int aOuterTimeStep,
    const int aCoulombMethod,
    const double aInnerTimeStepInPicoSeconds,
    const int aNumberOfInnerTimeSteps,
    const double aOuterTimeStepInPicoSeconds,
    const int aNumberOfOuterTimeSteps,
    const int aNumberOfRESPALevels,
    const double aPressureControlPistonMass,
    const double aPressureControlTarget,
    const double aVelocityResampleTargetTemperature,
    const int aVelocityResamplePeriodInOTS,
    const int aEnsemble,
    const unsigned aSnapshotPeriodInOTS,
    const unsigned aVelocityResampleRandomSeed,
    const unsigned aSourceID,
    const unsigned aExecutableID,
    const unsigned aNumberOfTwoChainThermostats,
    const unsigned aJobID,
    const XYZ aDynamicBoundingBoxMin,
    const XYZ aDynamicBoundingBoxMax
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_InformationRTP::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Information;
    ed_hdr.mType   = ED_INFORMATION_TYPES::RTP;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aCoulombMethod);
    BM_PacketPrimitive::Pack(p, aInnerTimeStepInPicoSeconds);
    BM_PacketPrimitive::Pack(p, aNumberOfInnerTimeSteps);
    BM_PacketPrimitive::Pack(p, aOuterTimeStepInPicoSeconds);
    BM_PacketPrimitive::Pack(p, aNumberOfOuterTimeSteps);
    BM_PacketPrimitive::Pack(p, aNumberOfRESPALevels);
    BM_PacketPrimitive::Pack(p, aPressureControlPistonMass);
    BM_PacketPrimitive::Pack(p, aPressureControlTarget);
    BM_PacketPrimitive::Pack(p, aVelocityResampleTargetTemperature);
    BM_PacketPrimitive::Pack(p, aVelocityResamplePeriodInOTS);
    BM_PacketPrimitive::Pack(p, aEnsemble);
    BM_PacketPrimitive::Pack(p, aSnapshotPeriodInOTS);
    BM_PacketPrimitive::Pack(p, aVelocityResampleRandomSeed);
    BM_PacketPrimitive::Pack(p, aSourceID);
    BM_PacketPrimitive::Pack(p, aExecutableID);
    BM_PacketPrimitive::Pack(p, aNumberOfTwoChainThermostats);
    BM_PacketPrimitive::Pack(p, aJobID);
    BM_PacketPrimitive::Pack(p, aDynamicBoundingBoxMin);
    BM_PacketPrimitive::Pack(p, aDynamicBoundingBoxMax);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_InformationRTP(
    const int aOuterTimeStep,
    const int aCoulombMethod,
    const double aInnerTimeStepInPicoSeconds,
    const int aNumberOfInnerTimeSteps,
    const double aOuterTimeStepInPicoSeconds,
    const int aNumberOfOuterTimeSteps,
    const int aNumberOfRESPALevels,
    const double aPressureControlPistonMass,
    const double aPressureControlTarget,
    const double aVelocityResampleTargetTemperature,
    const int aVelocityResamplePeriodInOTS,
    const int aEnsemble,
    const unsigned aSnapshotPeriodInOTS,
    const unsigned aVelocityResampleRandomSeed,
    const unsigned aSourceID,
    const unsigned aExecutableID,
    const unsigned aNumberOfTwoChainThermostats,
    const unsigned aJobID,
    const XYZ aDynamicBoundingBoxMin,
    const XYZ aDynamicBoundingBoxMax
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_InformationRTP::PackedSize];

    ED_FillPacket_InformationRTP(
    buff,
    aOuterTimeStep,
    aCoulombMethod,
    aInnerTimeStepInPicoSeconds,
    aNumberOfInnerTimeSteps,
    aOuterTimeStepInPicoSeconds,
    aNumberOfOuterTimeSteps,
    aNumberOfRESPALevels,
    aPressureControlPistonMass,
    aPressureControlTarget,
    aVelocityResampleTargetTemperature,
    aVelocityResamplePeriodInOTS,
    aEnsemble,
    aSnapshotPeriodInOTS,
    aVelocityResampleRandomSeed,
    aSourceID,
    aExecutableID,
    aNumberOfTwoChainThermostats,
    aJobID,
    aDynamicBoundingBoxMin,
    aDynamicBoundingBoxMax
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationRTP::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationRTP::PackedSize );
#endif
}

inline void ED_Emit_Tagged_InformationRTP(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aCoulombMethod,
    const double aInnerTimeStepInPicoSeconds,
    const int aNumberOfInnerTimeSteps,
    const double aOuterTimeStepInPicoSeconds,
    const int aNumberOfOuterTimeSteps,
    const int aNumberOfRESPALevels,
    const double aPressureControlPistonMass,
    const double aPressureControlTarget,
    const double aVelocityResampleTargetTemperature,
    const int aVelocityResamplePeriodInOTS,
    const int aEnsemble,
    const unsigned aSnapshotPeriodInOTS,
    const unsigned aVelocityResampleRandomSeed,
    const unsigned aSourceID,
    const unsigned aExecutableID,
    const unsigned aNumberOfTwoChainThermostats,
    const unsigned aJobID,
    const XYZ aDynamicBoundingBoxMin,
    const XYZ aDynamicBoundingBoxMax
    )
{
    char buff[ED_InformationRTP::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_InformationRTP(
    p,
    aOuterTimeStep,
    aCoulombMethod,
    aInnerTimeStepInPicoSeconds,
    aNumberOfInnerTimeSteps,
    aOuterTimeStepInPicoSeconds,
    aNumberOfOuterTimeSteps,
    aNumberOfRESPALevels,
    aPressureControlPistonMass,
    aPressureControlTarget,
    aVelocityResampleTargetTemperature,
    aVelocityResamplePeriodInOTS,
    aEnsemble,
    aSnapshotPeriodInOTS,
    aVelocityResampleRandomSeed,
    aSourceID,
    aExecutableID,
    aNumberOfTwoChainThermostats,
    aJobID,
    aDynamicBoundingBoxMin,
    aDynamicBoundingBoxMax
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationRTP::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationRTP::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_InformationRTP &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mCoulombMethod << " ";
    os << ed_pkt.mInnerTimeStepInPicoSeconds << " ";
    os << ed_pkt.mNumberOfInnerTimeSteps << " ";
    os << ed_pkt.mOuterTimeStepInPicoSeconds << " ";
    os << ed_pkt.mNumberOfOuterTimeSteps << " ";
    os << ed_pkt.mNumberOfRESPALevels << " ";
    os << ed_pkt.mPressureControlPistonMass << " ";
    os << ed_pkt.mPressureControlTarget << " ";
    os << ed_pkt.mVelocityResampleTargetTemperature << " ";
    os << ed_pkt.mVelocityResamplePeriodInOTS << " ";
    os << ed_pkt.mEnsemble << " ";
    os << ed_pkt.mSnapshotPeriodInOTS << " ";
    os << ed_pkt.mVelocityResampleRandomSeed << " ";
    os << ed_pkt.mSourceID << " ";
    os << ed_pkt.mExecutableID << " ";
    os << ed_pkt.mNumberOfTwoChainThermostats << " ";
    os << ed_pkt.mJobID << " ";
    os << ed_pkt.mDynamicBoundingBoxMin << " ";
    os << ed_pkt.mDynamicBoundingBoxMax;
    return (os);
}

//=============================================================================
//====        4      7 InformationSite                                     ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mSiteId;
    double  mMass;
    double  mCharge;
    int  mLJType;
    int  mFragmentId;
    int  mFragmentIndex;
    int  mAtomType;
    double  mdummy1;
    double  mdummy2;
    double  mdummy3;
    enum {PackedSize = 64,
          PackedTaggedSize = 64+TaggedPacket::TAGSIZE};
} ED_InformationSite;

inline void ED_OpenPacket_InformationSite(char *&p, ED_InformationSite &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSiteId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mMass);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mCharge);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mLJType);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mFragmentId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mFragmentIndex);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mAtomType);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mdummy1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mdummy2);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mdummy3);
}

inline void ED_FillPacket_InformationSite(char *p, ED_InformationSite &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSiteId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mMass);
    BM_PacketPrimitive::Pack(p, ed_pkt.mCharge);
    BM_PacketPrimitive::Pack(p, ed_pkt.mLJType);
    BM_PacketPrimitive::Pack(p, ed_pkt.mFragmentId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mFragmentIndex);
    BM_PacketPrimitive::Pack(p, ed_pkt.mAtomType);
    BM_PacketPrimitive::Pack(p, ed_pkt.mdummy1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mdummy2);
    BM_PacketPrimitive::Pack(p, ed_pkt.mdummy3);
}

inline void ED_OpenPacket_InformationSite(
    char *&p,
    int &aSiteId,
    double &aMass,
    double &aCharge,
    int &aLJType,
    int &aFragmentId,
    int &aFragmentIndex,
    int &aAtomType,
    double &adummy1,
    double &adummy2,
    double &adummy3
    )
{
    BM_PacketPrimitive::Unpack(p, aSiteId);
    BM_PacketPrimitive::Unpack(p, aMass);
    BM_PacketPrimitive::Unpack(p, aCharge);
    BM_PacketPrimitive::Unpack(p, aLJType);
    BM_PacketPrimitive::Unpack(p, aFragmentId);
    BM_PacketPrimitive::Unpack(p, aFragmentIndex);
    BM_PacketPrimitive::Unpack(p, aAtomType);
    BM_PacketPrimitive::Unpack(p, adummy1);
    BM_PacketPrimitive::Unpack(p, adummy2);
    BM_PacketPrimitive::Unpack(p, adummy3);
}

inline void ED_FillPacket_InformationSite(
    char *p,
    const int aOuterTimeStep,
    const int aSiteId,
    const double aMass,
    const double aCharge,
    const int aLJType,
    const int aFragmentId,
    const int aFragmentIndex,
    const int aAtomType,
    const double adummy1,
    const double adummy2,
    const double adummy3
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_InformationSite::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Information;
    ed_hdr.mType   = ED_INFORMATION_TYPES::Site;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aSiteId);
    BM_PacketPrimitive::Pack(p, aMass);
    BM_PacketPrimitive::Pack(p, aCharge);
    BM_PacketPrimitive::Pack(p, aLJType);
    BM_PacketPrimitive::Pack(p, aFragmentId);
    BM_PacketPrimitive::Pack(p, aFragmentIndex);
    BM_PacketPrimitive::Pack(p, aAtomType);
    BM_PacketPrimitive::Pack(p, adummy1);
    BM_PacketPrimitive::Pack(p, adummy2);
    BM_PacketPrimitive::Pack(p, adummy3);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_InformationSite(
    const int aOuterTimeStep,
    const int aSiteId,
    const double aMass,
    const double aCharge,
    const int aLJType,
    const int aFragmentId,
    const int aFragmentIndex,
    const int aAtomType,
    const double adummy1,
    const double adummy2,
    const double adummy3
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_InformationSite::PackedSize];

    ED_FillPacket_InformationSite(
    buff,
    aOuterTimeStep,
    aSiteId,
    aMass,
    aCharge,
    aLJType,
    aFragmentId,
    aFragmentIndex,
    aAtomType,
    adummy1,
    adummy2,
    adummy3
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationSite::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationSite::PackedSize );
#endif
}

inline void ED_Emit_Tagged_InformationSite(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aSiteId,
    const double aMass,
    const double aCharge,
    const int aLJType,
    const int aFragmentId,
    const int aFragmentIndex,
    const int aAtomType,
    const double adummy1,
    const double adummy2,
    const double adummy3
    )
{
    char buff[ED_InformationSite::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_InformationSite(
    p,
    aOuterTimeStep,
    aSiteId,
    aMass,
    aCharge,
    aLJType,
    aFragmentId,
    aFragmentIndex,
    aAtomType,
    adummy1,
    adummy2,
    adummy3
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationSite::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationSite::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_InformationSite &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mSiteId << " ";
    os << ed_pkt.mMass << " ";
    os << ed_pkt.mCharge << " ";
    os << ed_pkt.mLJType << " ";
    os << ed_pkt.mFragmentId << " ";
    os << ed_pkt.mFragmentIndex << " ";
    os << ed_pkt.mAtomType << " ";
    os << ed_pkt.mdummy1 << " ";
    os << ed_pkt.mdummy2 << " ";
    os << ed_pkt.mdummy3;
    return (os);
}

//=============================================================================
//====        4      8 InformationBond                                     ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    int  mBondId;
    int  mSiteA;
    int  mSiteB;
    int  mBondType;
    double  mdummy1;
    double  mdummy2;
    enum {PackedSize = 36,
          PackedTaggedSize = 36+TaggedPacket::TAGSIZE};
} ED_InformationBond;

inline void ED_OpenPacket_InformationBond(char *&p, ED_InformationBond &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mBondId);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSiteA);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mSiteB);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mBondType);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mdummy1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mdummy2);
}

inline void ED_FillPacket_InformationBond(char *p, ED_InformationBond &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mBondId);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSiteA);
    BM_PacketPrimitive::Pack(p, ed_pkt.mSiteB);
    BM_PacketPrimitive::Pack(p, ed_pkt.mBondType);
    BM_PacketPrimitive::Pack(p, ed_pkt.mdummy1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mdummy2);
}

inline void ED_OpenPacket_InformationBond(
    char *&p,
    int &aBondId,
    int &aSiteA,
    int &aSiteB,
    int &aBondType,
    double &adummy1,
    double &adummy2
    )
{
    BM_PacketPrimitive::Unpack(p, aBondId);
    BM_PacketPrimitive::Unpack(p, aSiteA);
    BM_PacketPrimitive::Unpack(p, aSiteB);
    BM_PacketPrimitive::Unpack(p, aBondType);
    BM_PacketPrimitive::Unpack(p, adummy1);
    BM_PacketPrimitive::Unpack(p, adummy2);
}

inline void ED_FillPacket_InformationBond(
    char *p,
    const int aOuterTimeStep,
    const int aBondId,
    const int aSiteA,
    const int aSiteB,
    const int aBondType,
    const double adummy1,
    const double adummy2
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_InformationBond::PackedSize;
    ed_hdr.mClass  = ED_CLASS::Information;
    ed_hdr.mType   = ED_INFORMATION_TYPES::Bond;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aBondId);
    BM_PacketPrimitive::Pack(p, aSiteA);
    BM_PacketPrimitive::Pack(p, aSiteB);
    BM_PacketPrimitive::Pack(p, aBondType);
    BM_PacketPrimitive::Pack(p, adummy1);
    BM_PacketPrimitive::Pack(p, adummy2);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_InformationBond(
    const int aOuterTimeStep,
    const int aBondId,
    const int aSiteA,
    const int aSiteB,
    const int aBondType,
    const double adummy1,
    const double adummy2
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_InformationBond::PackedSize];

    ED_FillPacket_InformationBond(
    buff,
    aOuterTimeStep,
    aBondId,
    aSiteA,
    aSiteB,
    aBondType,
    adummy1,
    adummy2
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationBond::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationBond::PackedSize );
#endif
}

inline void ED_Emit_Tagged_InformationBond(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const int aBondId,
    const int aSiteA,
    const int aSiteB,
    const int aBondType,
    const double adummy1,
    const double adummy2
    )
{
    char buff[ED_InformationBond::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_InformationBond(
    p,
    aOuterTimeStep,
    aBondId,
    aSiteA,
    aSiteB,
    aBondType,
    adummy1,
    adummy2
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_InformationBond::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_InformationBond::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_InformationBond &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mBondId << " ";
    os << ed_pkt.mSiteA << " ";
    os << ed_pkt.mSiteB << " ";
    os << ed_pkt.mBondType << " ";
    os << ed_pkt.mdummy1 << " ";
    os << ed_pkt.mdummy2;
    return (os);
}

//=============================================================================
//====        5      1 UDFd1                                               ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mCode;
    unsigned  mcount;
    double  mValue;
    enum {PackedSize = 20,
          PackedTaggedSize = 20+TaggedPacket::TAGSIZE};
} ED_UDFd1;

inline void ED_OpenPacket_UDFd1(char *&p, ED_UDFd1 &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mCode);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mcount);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mValue);
}

inline void ED_FillPacket_UDFd1(char *p, ED_UDFd1 &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mCode);
    BM_PacketPrimitive::Pack(p, ed_pkt.mcount);
    BM_PacketPrimitive::Pack(p, ed_pkt.mValue);
}

inline void ED_OpenPacket_UDFd1(
    char *&p,
    unsigned &aCode,
    unsigned &acount,
    double &aValue
    )
{
    BM_PacketPrimitive::Unpack(p, aCode);
    BM_PacketPrimitive::Unpack(p, acount);
    BM_PacketPrimitive::Unpack(p, aValue);
}

inline void ED_FillPacket_UDFd1(
    char *p,
    const int aOuterTimeStep,
    const unsigned aCode,
    const unsigned acount,
    const double aValue
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_UDFd1::PackedSize;
    ed_hdr.mClass  = ED_CLASS::UDF;
    ed_hdr.mType   = ED_UDF_TYPES::d1;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aCode);
    BM_PacketPrimitive::Pack(p, acount);
    BM_PacketPrimitive::Pack(p, aValue);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_UDFd1(
    const int aOuterTimeStep,
    const unsigned aCode,
    const unsigned acount,
    const double aValue
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_UDFd1::PackedSize];

    ED_FillPacket_UDFd1(
    buff,
    aOuterTimeStep,
    aCode,
    acount,
    aValue
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_UDFd1::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_UDFd1::PackedSize );
#endif
}

inline void ED_Emit_Tagged_UDFd1(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const unsigned aCode,
    const unsigned acount,
    const double aValue
    )
{
    char buff[ED_UDFd1::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_UDFd1(
    p,
    aOuterTimeStep,
    aCode,
    acount,
    aValue
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_UDFd1::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_UDFd1::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_UDFd1 &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mCode << " ";
    os << ed_pkt.mcount << " ";
    os << ed_pkt.mValue;
    return (os);
}

//=============================================================================
//====        5      2 UDFd2                                               ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mCode;
    unsigned  mcount;
    double  mValue1;
    double  mValue2;
    enum {PackedSize = 28,
          PackedTaggedSize = 28+TaggedPacket::TAGSIZE};
} ED_UDFd2;

inline void ED_OpenPacket_UDFd2(char *&p, ED_UDFd2 &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mCode);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mcount);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mValue1);
    BM_PacketPrimitive::Unpack(p, ed_pkt.mValue2);
}

inline void ED_FillPacket_UDFd2(char *p, ED_UDFd2 &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mCode);
    BM_PacketPrimitive::Pack(p, ed_pkt.mcount);
    BM_PacketPrimitive::Pack(p, ed_pkt.mValue1);
    BM_PacketPrimitive::Pack(p, ed_pkt.mValue2);
}

inline void ED_OpenPacket_UDFd2(
    char *&p,
    unsigned &aCode,
    unsigned &acount,
    double &aValue1,
    double &aValue2
    )
{
    BM_PacketPrimitive::Unpack(p, aCode);
    BM_PacketPrimitive::Unpack(p, acount);
    BM_PacketPrimitive::Unpack(p, aValue1);
    BM_PacketPrimitive::Unpack(p, aValue2);
}

inline void ED_FillPacket_UDFd2(
    char *p,
    const int aOuterTimeStep,
    const unsigned aCode,
    const unsigned acount,
    const double aValue1,
    const double aValue2
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_UDFd2::PackedSize;
    ed_hdr.mClass  = ED_CLASS::UDF;
    ed_hdr.mType   = ED_UDF_TYPES::d2;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aCode);
    BM_PacketPrimitive::Pack(p, acount);
    BM_PacketPrimitive::Pack(p, aValue1);
    BM_PacketPrimitive::Pack(p, aValue2);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_UDFd2(
    const int aOuterTimeStep,
    const unsigned aCode,
    const unsigned acount,
    const double aValue1,
    const double aValue2
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_UDFd2::PackedSize];

    ED_FillPacket_UDFd2(
    buff,
    aOuterTimeStep,
    aCode,
    acount,
    aValue1,
    aValue2
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_UDFd2::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_UDFd2::PackedSize );
#endif
}

inline void ED_Emit_Tagged_UDFd2(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const unsigned aCode,
    const unsigned acount,
    const double aValue1,
    const double aValue2
    )
{
    char buff[ED_UDFd2::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_UDFd2(
    p,
    aOuterTimeStep,
    aCode,
    acount,
    aValue1,
    aValue2
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_UDFd2::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_UDFd2::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_UDFd2 &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mCode << " ";
    os << ed_pkt.mcount << " ";
    os << ed_pkt.mValue1 << " ";
    os << ed_pkt.mValue2;
    return (os);
}

//=============================================================================
//====        6      1 RTPValuesVelocityResampleTargetTemperature          ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    double  mValue;
    enum {PackedSize = 12,
          PackedTaggedSize = 12+TaggedPacket::TAGSIZE};
} ED_RTPValuesVelocityResampleTargetTemperature;

inline void ED_OpenPacket_RTPValuesVelocityResampleTargetTemperature(char *&p, ED_RTPValuesVelocityResampleTargetTemperature &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mValue);
}

inline void ED_FillPacket_RTPValuesVelocityResampleTargetTemperature(char *p, ED_RTPValuesVelocityResampleTargetTemperature &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mValue);
}

inline void ED_OpenPacket_RTPValuesVelocityResampleTargetTemperature(
    char *&p,
    double &aValue
    )
{
    BM_PacketPrimitive::Unpack(p, aValue);
}

inline void ED_FillPacket_RTPValuesVelocityResampleTargetTemperature(
    char *p,
    const int aOuterTimeStep,
    const double aValue
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_RTPValuesVelocityResampleTargetTemperature::PackedSize;
    ed_hdr.mClass  = ED_CLASS::RTPValues;
    ed_hdr.mType   = ED_RTPVALUES_TYPES::VelocityResampleTargetTemperature;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aValue);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_RTPValuesVelocityResampleTargetTemperature(
    const int aOuterTimeStep,
    const double aValue
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_RTPValuesVelocityResampleTargetTemperature::PackedSize];

    ED_FillPacket_RTPValuesVelocityResampleTargetTemperature(
    buff,
    aOuterTimeStep,
    aValue
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_RTPValuesVelocityResampleTargetTemperature::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_RTPValuesVelocityResampleTargetTemperature::PackedSize );
#endif
}

inline void ED_Emit_Tagged_RTPValuesVelocityResampleTargetTemperature(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const double aValue
    )
{
    char buff[ED_RTPValuesVelocityResampleTargetTemperature::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_RTPValuesVelocityResampleTargetTemperature(
    p,
    aOuterTimeStep,
    aValue
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_RTPValuesVelocityResampleTargetTemperature::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_RTPValuesVelocityResampleTargetTemperature::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_RTPValuesVelocityResampleTargetTemperature &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mValue;
    return (os);
}

//=============================================================================
//====        6      2 RTPValuesNumberOfOuterTimeSteps                     ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mValue;
    enum {PackedSize = 8,
          PackedTaggedSize = 8+TaggedPacket::TAGSIZE};
} ED_RTPValuesNumberOfOuterTimeSteps;

inline void ED_OpenPacket_RTPValuesNumberOfOuterTimeSteps(char *&p, ED_RTPValuesNumberOfOuterTimeSteps &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mValue);
}

inline void ED_FillPacket_RTPValuesNumberOfOuterTimeSteps(char *p, ED_RTPValuesNumberOfOuterTimeSteps &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mValue);
}

inline void ED_OpenPacket_RTPValuesNumberOfOuterTimeSteps(
    char *&p,
    unsigned &aValue
    )
{
    BM_PacketPrimitive::Unpack(p, aValue);
}

inline void ED_FillPacket_RTPValuesNumberOfOuterTimeSteps(
    char *p,
    const int aOuterTimeStep,
    const unsigned aValue
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_RTPValuesNumberOfOuterTimeSteps::PackedSize;
    ed_hdr.mClass  = ED_CLASS::RTPValues;
    ed_hdr.mType   = ED_RTPVALUES_TYPES::NumberOfOuterTimeSteps;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aValue);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_RTPValuesNumberOfOuterTimeSteps(
    const int aOuterTimeStep,
    const unsigned aValue
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_RTPValuesNumberOfOuterTimeSteps::PackedSize];

    ED_FillPacket_RTPValuesNumberOfOuterTimeSteps(
    buff,
    aOuterTimeStep,
    aValue
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_RTPValuesNumberOfOuterTimeSteps::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_RTPValuesNumberOfOuterTimeSteps::PackedSize );
#endif
}

inline void ED_Emit_Tagged_RTPValuesNumberOfOuterTimeSteps(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const unsigned aValue
    )
{
    char buff[ED_RTPValuesNumberOfOuterTimeSteps::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_RTPValuesNumberOfOuterTimeSteps(
    p,
    aOuterTimeStep,
    aValue
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_RTPValuesNumberOfOuterTimeSteps::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_RTPValuesNumberOfOuterTimeSteps::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_RTPValuesNumberOfOuterTimeSteps &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mValue;
    return (os);
}

//=============================================================================
//====        6      3 RTPValuesSnapshotPeriodInOTS                        ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mValue;
    enum {PackedSize = 8,
          PackedTaggedSize = 8+TaggedPacket::TAGSIZE};
} ED_RTPValuesSnapshotPeriodInOTS;

inline void ED_OpenPacket_RTPValuesSnapshotPeriodInOTS(char *&p, ED_RTPValuesSnapshotPeriodInOTS &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mValue);
}

inline void ED_FillPacket_RTPValuesSnapshotPeriodInOTS(char *p, ED_RTPValuesSnapshotPeriodInOTS &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mValue);
}

inline void ED_OpenPacket_RTPValuesSnapshotPeriodInOTS(
    char *&p,
    unsigned &aValue
    )
{
    BM_PacketPrimitive::Unpack(p, aValue);
}

inline void ED_FillPacket_RTPValuesSnapshotPeriodInOTS(
    char *p,
    const int aOuterTimeStep,
    const unsigned aValue
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_RTPValuesSnapshotPeriodInOTS::PackedSize;
    ed_hdr.mClass  = ED_CLASS::RTPValues;
    ed_hdr.mType   = ED_RTPVALUES_TYPES::SnapshotPeriodInOTS;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aValue);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_RTPValuesSnapshotPeriodInOTS(
    const int aOuterTimeStep,
    const unsigned aValue
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_RTPValuesSnapshotPeriodInOTS::PackedSize];

    ED_FillPacket_RTPValuesSnapshotPeriodInOTS(
    buff,
    aOuterTimeStep,
    aValue
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_RTPValuesSnapshotPeriodInOTS::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_RTPValuesSnapshotPeriodInOTS::PackedSize );
#endif
}

inline void ED_Emit_Tagged_RTPValuesSnapshotPeriodInOTS(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const unsigned aValue
    )
{
    char buff[ED_RTPValuesSnapshotPeriodInOTS::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_RTPValuesSnapshotPeriodInOTS(
    p,
    aOuterTimeStep,
    aValue
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_RTPValuesSnapshotPeriodInOTS::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_RTPValuesSnapshotPeriodInOTS::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_RTPValuesSnapshotPeriodInOTS &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mValue;
    return (os);
}

//=============================================================================
//====        6      4 RTPValuesEmitEnergyTimeStepModulo                   ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mValue;
    enum {PackedSize = 8,
          PackedTaggedSize = 8+TaggedPacket::TAGSIZE};
} ED_RTPValuesEmitEnergyTimeStepModulo;

inline void ED_OpenPacket_RTPValuesEmitEnergyTimeStepModulo(char *&p, ED_RTPValuesEmitEnergyTimeStepModulo &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mValue);
}

inline void ED_FillPacket_RTPValuesEmitEnergyTimeStepModulo(char *p, ED_RTPValuesEmitEnergyTimeStepModulo &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mValue);
}

inline void ED_OpenPacket_RTPValuesEmitEnergyTimeStepModulo(
    char *&p,
    unsigned &aValue
    )
{
    BM_PacketPrimitive::Unpack(p, aValue);
}

inline void ED_FillPacket_RTPValuesEmitEnergyTimeStepModulo(
    char *p,
    const int aOuterTimeStep,
    const unsigned aValue
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_RTPValuesEmitEnergyTimeStepModulo::PackedSize;
    ed_hdr.mClass  = ED_CLASS::RTPValues;
    ed_hdr.mType   = ED_RTPVALUES_TYPES::EmitEnergyTimeStepModulo;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aValue);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_RTPValuesEmitEnergyTimeStepModulo(
    const int aOuterTimeStep,
    const unsigned aValue
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_RTPValuesEmitEnergyTimeStepModulo::PackedSize];

    ED_FillPacket_RTPValuesEmitEnergyTimeStepModulo(
    buff,
    aOuterTimeStep,
    aValue
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_RTPValuesEmitEnergyTimeStepModulo::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_RTPValuesEmitEnergyTimeStepModulo::PackedSize );
#endif
}

inline void ED_Emit_Tagged_RTPValuesEmitEnergyTimeStepModulo(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const unsigned aValue
    )
{
    char buff[ED_RTPValuesEmitEnergyTimeStepModulo::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_RTPValuesEmitEnergyTimeStepModulo(
    p,
    aOuterTimeStep,
    aValue
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_RTPValuesEmitEnergyTimeStepModulo::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_RTPValuesEmitEnergyTimeStepModulo::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_RTPValuesEmitEnergyTimeStepModulo &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mValue;
    return (os);
}

//=============================================================================
//====        6      5 RTPValuesVelocityResampleRandomSeed                 ====
//=============================================================================


typedef struct
{
    DCL_ED_HEADER;
    unsigned  mValue;
    enum {PackedSize = 8,
          PackedTaggedSize = 8+TaggedPacket::TAGSIZE};
} ED_RTPValuesVelocityResampleRandomSeed;

inline void ED_OpenPacket_RTPValuesVelocityResampleRandomSeed(char *&p, ED_RTPValuesVelocityResampleRandomSeed &ed_pkt)
{
    BM_PacketPrimitive::Unpack(p, ed_pkt.mValue);
}

inline void ED_FillPacket_RTPValuesVelocityResampleRandomSeed(char *p, ED_RTPValuesVelocityResampleRandomSeed &ed_pkt)
{
    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);
    BM_PacketPrimitive::Pack(p, ed_pkt.mValue);
}

inline void ED_OpenPacket_RTPValuesVelocityResampleRandomSeed(
    char *&p,
    unsigned &aValue
    )
{
    BM_PacketPrimitive::Unpack(p, aValue);
}

inline void ED_FillPacket_RTPValuesVelocityResampleRandomSeed(
    char *p,
    const int aOuterTimeStep,
    const unsigned aValue
    )
{
    ED_Header ed_hdr;
    ed_hdr.mLen    = ED_RTPValuesVelocityResampleRandomSeed::PackedSize;
    ed_hdr.mClass  = ED_CLASS::RTPValues;
    ed_hdr.mType   = ED_RTPVALUES_TYPES::VelocityResampleRandomSeed;
    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;

    BM_PacketPrimitive::Pack(p, ed_hdr);
    BM_PacketPrimitive::Pack(p, aValue);
}

#ifndef NO_PK_PLATFORM
inline void ED_Emit_RTPValuesVelocityResampleRandomSeed(
    const int aOuterTimeStep,
    const unsigned aValue
    )
{
#if !defined( NO_EXTERNAL_DATAGRAMS )
    char buff[ED_RTPValuesVelocityResampleRandomSeed::PackedSize];

    ED_FillPacket_RTPValuesVelocityResampleRandomSeed(
    buff,
    aOuterTimeStep,
    aValue
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_RTPValuesVelocityResampleRandomSeed::PackedSize );
    SendDatagramToMonitor( 1, buff, ED_RTPValuesVelocityResampleRandomSeed::PackedSize );
#endif
}

inline void ED_Emit_Tagged_RTPValuesVelocityResampleRandomSeed(
    const unsigned short aTag,
    const int aOuterTimeStep,
    const unsigned aValue
    )
{
    char buff[ED_RTPValuesVelocityResampleRandomSeed::PackedTaggedSize];

    char *p = buff;
    BM_PacketPrimitive::Pack(p, aTag);
    ED_FillPacket_RTPValuesVelocityResampleRandomSeed(
    p,
    aOuterTimeStep,
    aValue
    );

//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED_RTPValuesVelocityResampleRandomSeed::PackedTaggedSize );
    SendDatagramToMonitor( 1, buff, ED_RTPValuesVelocityResampleRandomSeed::PackedTaggedSize );
}
#endif

template <class streamclass>static inline streamclass& operator<<(streamclass& os, const ED_RTPValuesVelocityResampleRandomSeed &ed_pkt)
{
    os << ed_pkt.mHeader.mShortTimeStep << " ";
    os << ed_pkt.mValue;
    return (os);
}


struct Packet
{
    union
    {
        DCL_ED_HEADER;
        char mData[MAXPACKEDPACKETSIZE];
    };

    inline int getClass()
    {
        return mHeader.mClass;
    }

    inline int getType()
    {
        return mHeader.mType;
    }

    inline int getShortTimeStep()
    {
        return mHeader.mShortTimeStep;
    }

    inline static int getClass(const char *p)
    {
        ED_Header *HdrPtr = (ED_Header *)p;
        return HdrPtr->mClass;
    }

    inline static int getClass(const ED_Header &Hdr)
    {
        return Hdr.mClass;
    }

    inline static int getType(const char *p)
    {
        ED_Header *HdrPtr = (ED_Header *)p;
        return HdrPtr->mType;
    }

    inline static int getType(const ED_Header &Hdr)
    {
        return Hdr.mType;
    }

    inline static int getShortTimeStep(const char *p)
    {
        ED_Header *HdrPtr = (ED_Header *)p;
        return HdrPtr->mShortTimeStep;
    }

    inline static int getShortTimeStep(const ED_Header &Hdr)
    {
        return Hdr.mShortTimeStep;
    }

    inline static int getPayloadSize(const char *p)
    {
        ED_Header *HdrPtr = (ED_Header *)p;
        return HdrPtr->mLen - sizeof(ED_Header);
    }

    inline static int getPayloadSize(const ED_Header &Hdr)
    {
        return Hdr.mLen - sizeof(ED_Header);
    }

    // load next packed packet from file into buffer.  Unpack header,
    // but do not unpack payload.
    // Return with buff pointing to start of payload
    inline static int load(const int Fd, char *buff, ED_Header &Hdr)
    {
        int BytesRead;
        char *ptr = buff;

        assert(Fd >= 0);

        // Read the header of the next record.
        BytesRead = read( Fd, (void *) ptr, sizeof( ED_Header ) );

        // Look for the end of file.
        if( BytesRead == 0 )
            return -1;  // return nonzero on acceptable eof

        // Assert we got a headers worth of bytes.
        assert( BytesRead == sizeof( ED_Header ) );

        BM_PacketPrimitive::Unpack(ptr, Hdr);

        // Read the rest of the record at the start of the buffer, overwriting the packed header
        BytesRead = read(Fd,
            (void *) buff,
            getPayloadSize(Hdr));

        assert( BytesRead == getPayloadSize(Hdr) );

        return 0;
    }


  // load next packed packet from pointer into buffer.  Unpack header,
  // but do not unpack payload.
  // Return with buff pointing to start of payload
  inline static int load(char** aCurrentDataPtr, char *aBuffer, ED_Header &aHdr, int* aBytesRead )
    {
      int BytesRead;
      char *ptr = aBuffer;
      
      // Read the header of the next record.
      // BytesRead = read( Fd, (void *) ptr, sizeof( ED_Header ) );
      memcpy( ptr, *aCurrentDataPtr, sizeof( ED_Header ) );
      *aCurrentDataPtr += sizeof( ED_Header ) ;
      *aBytesRead += sizeof( ED_Header );
      

      // Look for the end of file.
      // if( BytesRead == 0 )
      //  return -1;  // return nonzero on acceptable eof
      
      // Assert we got a headers worth of bytes.
      // assert( BytesRead == sizeof( ED_Header ) );
      
      BM_PacketPrimitive::Unpack(ptr, aHdr);
      
      // Read the rest of the record at the start of the buffer, overwriting the packed header
      // BytesRead = read(Fd,
      //                 (void *) buff,
      //                 getPayloadSize(Hdr));
     
      memcpy( aBuffer, *aCurrentDataPtr, getPayloadSize(aHdr) );
      *aCurrentDataPtr += getPayloadSize(aHdr);
      *aBytesRead += getPayloadSize(aHdr);
      // assert( BytesRead == getPayloadSize(Hdr) );
      
      return 0;
    }

};


struct tEnergySumAccumulator
{
    unsigned mFullTimeStep;

    double mKineticEnergy;

    // Nonbonded contribution
    int    mNonBondedPartialSumCount;
    double mChargeEnergy;
    double mLennardJonesEnergy;
    double mEwaldKSpaceEnergy;
    double mWaterEnergy;

    // Other
    double mCOMKineticEnergy;
    double mPistonKineticEnergy;
    double mPressureEnergy;
    double mResampleEnergyLoss;
    double mNoseHooverEnergy;
};

struct tEnergySums
{
    double mIntraE;
    double mInterE;
    double mTotalE;
    double mTemp;
    double mConservedQuantity;
};

struct tPressureInfo
{
    double mVirial;
    double mPressure;
    double mVolume;
    double mVolumeVelocity;
    double mExternalPressure;
    double mPistonMass;
    int    mDoPressure;
};

struct tEnergyInfo
{
    tEnergySumAccumulator mEnergySumAccumulator;
    tEnergySums           mEnergySums;
    tPressureInfo         mPressureInfo;
};

struct tSiteInfo
{
    int mSiteId;
    XYZ mPosition;
    XYZ mVelocity;
};

struct tSiteData
{
    XYZ mPosition;
    XYZ mVelocity;
};

typedef unsigned short tOuterTimeStep;

#include <BlueMatter/ExternalDatagram_Helper.hpp>

#endif
