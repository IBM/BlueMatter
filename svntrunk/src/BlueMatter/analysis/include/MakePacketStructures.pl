#!/usr/bin/perl

use strict;

#some globals
my %OutputFiles = ();
my %PrimitiveSizes = ();
my %Classes = ();
my @PacketTypes = ();

Main();
exit;

sub Main {
    ParseInputFile();

    PrintHead();

    PrintClassesTypes();

    DumpPacketStats();

    DumpPackets();

    PrintTail();
}

sub ParseInputFile {
    # Skip to list of file names
    LINE: while (<>) {
          last LINE if /GENERATEDFILENAMES:/;
    }

    # Get file names
    LINE: while (<>) {
          last LINE if /PRIMITIVESIZES:/;
          $_ = CleanLine($_);
          my @Assignment = split(/=/);
          if (@Assignment == 2) {
              $OutputFiles{$Assignment[0]} = $Assignment[1];
          }
    }

    # Get primitive sizes
    LINE: while (<>) {
          last LINE if /CLASSES:/;
          $_ = CleanLine($_);
          my @Assignment = split(/=/);
          if (@Assignment == 2) {
              $PrimitiveSizes{$Assignment[0]} = $Assignment[1];
          }
    }

    # Get classes
    LINE: while (<>) {
          last LINE if /DATAGRAMSPECS:/;
          $_ = CleanLine($_);
          my @Assignment = split(/=/);
          if (@Assignment == 2) {
              $Classes{$Assignment[0]} = $Assignment[1];
          }
    }

    my $PacketSize;
    # Get datagram specs
    while (<>) {
        my $ClassName;
        my $TypeName;
        my $TypeNum;
        my $Explicit;
        if (/^END:/) {
            last;
        }
        $_ = CleanLine($_);
        # find new packet type
        if (/:$/) {
            s/:$//;
            my @Assignment = split(/=/);
            ($ClassName, $TypeName) = split(/\//, $Assignment[0]);
            $TypeNum = $Assignment[1];
            $PacketSize = 4;
            push(@PacketTypes, {ClassName=>$ClassName, ClassNum=>$Classes{$ClassName}, TypeName=>$TypeName, TypeNum=>$TypeNum, Size=>$PacketSize, ExplicitTimeStep=>0, MemberVariables=>()});
        } else {
            if (/\w/) {
                /(\w+) +(.*)/;
                my $VarName = $1;
                my $VarType = $2;
                $PacketSize += $PrimitiveSizes{$VarType};
                $PacketTypes[-1]{Size} = $PacketSize;
                if ($VarName eq "FullOuterTimeStep") {
                    $PacketTypes[-1]{ExplicitTimeStep} = 1;
                }
                push(@{$PacketTypes[-1]{MemberVariables}}, {Name=>$VarName, Type=>$VarType, Size=>$PrimitiveSizes{$VarType}});
            }
        }
    }
}


sub PrintClassesTypes {
    print "struct ED_CLASS {\n";
    print "enum {";
    my $key;
    my $first = 1;
    my @SortedKeys = sort { $Classes{$a} <=> $Classes{$b} } keys(%Classes);
    foreach (@SortedKeys)  {
        if ($first) {
            $first = 0;
        } else {
            print ",";
        }
        print "\n";
        print "    $_ = $Classes{$_}";
    }
    print "\n";
    print "    };\n";
    print "};\n";
    print "\n";

    foreach (@SortedKeys) {
        my $CNum = $Classes{$_};
        tr/a-z/A-Z/;
        print "struct ED\_$_\_TYPES {\n";
        print "enum {\n";
        print "    Unknown = 0";
        my $i;
        foreach $i (0 .. $#PacketTypes) {
            if ($PacketTypes[$i]{ClassNum} == $CNum) {
                print ",\n";
                print "    $PacketTypes[$i]{TypeName} = $PacketTypes[$i]{TypeNum}";
            }
        }
        print "\n";
        print "    };\n";
        print "};\n";
        print "\n";
    }
}

sub DumpPacketStats {
    my $i=0;
    my $PacketSum = 0;
    my $PacketMin = $PacketTypes[0]{Size};
    my $PacketMax = $PacketMin;
    my $PacketMinName = $PacketTypes[0]{ClassName}.$PacketTypes[0]{TypeName};
    my $PacketMaxName = $PacketMinName;
    for $i (0 .. $#PacketTypes) {
        my $s = $PacketTypes[$i]{Size};
        $PacketSum += $s;
        if ($PacketMin > $s) {
            $PacketMin = $s;
            $PacketMinName = $PacketTypes[$i]{ClassName}.$PacketTypes[$i]{TypeName};
        }
        if ($PacketMax < $s) {
            $PacketMax = $s;
            $PacketMaxName = $PacketTypes[$i]{ClassName}.$PacketTypes[$i]{TypeName};
        }
    }

    my $TotalNumPackets = @PacketTypes;
    print "#define TOTALNUMPACKETS     $TotalNumPackets\n";
    print "#define MAXPACKEDPACKETSIZE $PacketMax\n";
    print "#define MAXPACKEDPACKETNAME $PacketMaxName\n";
    print "#define MINPACKEDPACKETSIZE $PacketMin\n";
    print "#define MINPACKEDPACKETNAME $PacketMinName\n";
    print "// Mean Packet Size = ", $PacketSum/$#PacketTypes, "\n";
    print "\n\n";
}

sub DumpPackets {
    my $i;
    for $i (0 .. $#PacketTypes) {
        DumpPacketStructure($PacketTypes[$i]);
    }
}

sub DumpPacketStructure {
    my %Packet = %{$_[0]};
    my $Class = $Packet{ClassName};
    my $Type = $Packet{TypeName};
    my $TypeNum = $Packet{TypeNum};
    my $PacketName = $Class.$Type;
    my @Vars = @{$Packet{MemberVariables}};
    my $i = 0;


    format STDOUT =
//=============================================================================
//====   @>>>>> @>>>>> @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<====
$Packet{ClassNum}, $Packet{TypeNum}, $PacketName
//=============================================================================
.
    write;

    print <<HERE;


typedef struct
{
    DCL_ED_HEADER;
HERE

    for $i (0 .. $#Vars) {
        print "    $Vars[$i]{Type}  m$Vars[$i]{Name};\n";
    }
    print "    enum {PackedSize = $Packet{Size},\n";
    print "          PackedTaggedSize = $Packet{Size}+TaggedPacket::TAGSIZE};\n";
    print "} ED\_$PacketName;\n";
    print "\n";

    print "inline void ED_OpenPacket\_$PacketName(char *&p, ED\_$PacketName \&ed_pkt)\n";
    print "{\n";
    for $i (0 .. $#Vars) {
        print "    BM_PacketPrimitive::Unpack(p, ed_pkt.m$Vars[$i]{Name});\n";
    }
    print "}\n\n";

    print "inline void ED_FillPacket\_$PacketName(char *p, ED\_$PacketName \&ed_pkt)\n";
    print "{\n";
    print "    BM_PacketPrimitive::Pack(p, ed_pkt.mHeader);\n";

    for $i (0 .. $#Vars) {
        print "    BM_PacketPrimitive::Pack(p, ed_pkt.m$Vars[$i]{Name});\n";
    }
    print "}\n\n";

    print "inline void ED_OpenPacket\_$PacketName(\n";
    print "    char *&p";
    for $i (0 .. $#Vars) {
        print ",\n    $Vars[$i]{Type} &a$Vars[$i]{Name}";
    }
    print "\n    )\n{\n";

    for $i (0 .. $#Vars) {
        print "    BM_PacketPrimitive::Unpack(p, a$Vars[$i]{Name});\n";
    }

    print "}\n\n";

    print "inline void ED_FillPacket\_$PacketName(\n";
    print "    char *p";
    if (!$Packet{ExplicitTimeStep}) {
        print ",\n    const int aOuterTimeStep";
    }

    for $i (0 ..  $#Vars) {
        print ",\n    const $Vars[$i]{Type} a$Vars[$i]{Name}";
    }
    print "\n    )\n{\n";

    print "    ED_Header ed_hdr;\n";
    print "    ed_hdr.mLen    = ED\_$PacketName\::PackedSize;\n";
    print "    ed_hdr.mClass  = ED_CLASS::$Packet{ClassName};\n";
    my $UpperClass = $Packet{ClassName};
    $UpperClass =~ tr/a-z/A-Z/;
    print "    ed_hdr.mType   = ED\_$UpperClass\_TYPES::$Packet{TypeName};\n";
    if ($Packet{ExplicitTimeStep}) {
        print "    ed_hdr.mShortTimeStep   = aFullOuterTimeStep % 256;\n";
    } else {
        print "    ed_hdr.mShortTimeStep   = aOuterTimeStep % 256;\n";
    }
    print "\n";
    print "    BM_PacketPrimitive::Pack(p, ed_hdr);\n";

    for $i (0 .. $#Vars) {
        print "    BM_PacketPrimitive::Pack(p, a$Vars[$i]{Name});\n";
    }
    print "}\n\n";

    print "#ifndef NO_PK_PLATFORM\n";
    print "inline void ED_Emit\_$PacketName(\n";
    if (!$Packet{ExplicitTimeStep}) {
        print "    const int aOuterTimeStep,\n";
    }
    print "    const $Vars[0]{Type} a$Vars[0]{Name}";
    for $i (1 .. $#Vars) {
        print ",\n    const $Vars[$i]{Type} a$Vars[$i]{Name}";
    }
    print "\n    )\n{\n";
    printf "#if !defined( NO_EXTERNAL_DATAGRAMS )\n";
    print "    char buff[ED\_$PacketName\::PackedSize];\n";
    print "\n";
    print "    ED_FillPacket\_$PacketName(\n";
    print "    buff";
    if (!$Packet{ExplicitTimeStep}) {
        print ",\n    aOuterTimeStep";
    }
    for $i (0 .. $#Vars) {
        print ",\n    a$Vars[$i]{Name}";
    }
    print "\n    );\n\n";
    print "//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED\_$PacketName\::PackedSize );\n";
    print "    SendDatagramToMonitor( 1, buff, ED\_$PacketName\::PackedSize );\n";
    printf "#endif\n";
    print "}\n";

    print "\n";
    print "inline void ED_Emit_Tagged\_$PacketName(\n";
    print "    const unsigned short aTag,\n";
    if (!$Packet{ExplicitTimeStep}) {
        print "    const int aOuterTimeStep,\n";
    }
    print "    const $Vars[0]{Type} a$Vars[0]{Name}";
    for $i (1 .. $#Vars) {
        print ",\n    const $Vars[$i]{Type} a$Vars[$i]{Name}";
    }
    print "\n    )\n{\n";

    print "    char buff[ED\_$PacketName\::PackedTaggedSize];\n";
    print "\n";
    print "    char *p = buff;\n";
    print "    BM_PacketPrimitive::Pack(p, aTag);\n";
    print "    ED_FillPacket\_$PacketName(\n";
    print "    p";
    if (!$Packet{ExplicitTimeStep}) {
        print ",\n    aOuterTimeStep";
    }
    for $i (0 .. $#Vars) {
        print ",\n    a$Vars[$i]{Name}";
    }
    print "\n    );\n\n";
    print "//    Platform::ExternalDatagramInterface::SendTo( 1, buff, ED\_$PacketName\::PackedTaggedSize );\n";
    print "    SendDatagramToMonitor( 1, buff, ED\_$PacketName\::PackedTaggedSize );\n";

    print "}\n";

    print "#endif\n\n";

    print "template <class streamclass>";
    print "static inline streamclass& operator<<(streamclass& os, const ED\_$PacketName &ed_pkt)\n";
    print "{\n";
    print "    os << ed_pkt.mHeader.mShortTimeStep";
    for $i (0 .. $#Vars) {
        print " << \" \";\n    os << ed_pkt.m$Vars[$i]{Name}";
    }
    print ";\n";
    print "    return (os);\n";
    print "}\n\n";
}


# Rip out the // comment section
sub TrimComments {
    my @fields = split(/\/\//, $_[0]);
        return $fields[0];
}

# Strip comment and clean up spaces, including around =
sub CleanLine {
    $_ = TrimComments($_[0]);
    $_ = KillSpaces($_);
    s/\s*=\s*/=/;
    return $_;
}

# Clean up spaces (trim front and back, make single)
sub KillSpaces {
    $_ = $_[0];
    s/^\s+//;
    s/\s+$//;
    s/\s+/ /g;
    return $_;
}

# Clean up spaces and replace with _
sub MakeVariableName {
    $_ = KillSpaces($_[0]);
    s/\s/_/g;
    return $_;
}

sub PrintTail {
    print <<TAIL;

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
TAIL
}

sub PrintHead {
    print <<HEADER;
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



HEADER

}
