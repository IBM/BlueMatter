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
 #ifndef _MSDIF_HPP_
#define _MSDIF_HPP_
//BGF MSDIF should probably be in it's own header file
//BGF this is because the signleton ptr implies a link time symbol
//BGF which should go in a file called MSDIF.cpp.

// 'link-edited with program' form of Molecular System Definition interface
#if defined(MSD_LINKED)
class MSDIF_BASE
{
  public:

//    typedef xxxFragSpec FragSpec;

    const
    static
    SiteSpec * GetSiteSpecTable ()
    {
       return SiteSpecTable ;
    }

    const
    static
    int * GetExternalSiteTable ()
    {
       return ExternalSiteTable ;
    }

    int
    static
    GetNumberOfSites()
      {
      return( NumberOfSites );
      }

    static
    int
    GetVDWCombine()
      {
      return(  VDWCombine );
      }

    static
    int
    GetNumberOfFragmentInstances()
      {
      return(  NumberOfFragments );
      }

    static
    int
    GetFragmentTypeId( int aFragmentInstanceListIndex )
      {
      int rc =  FragmentInstanceList[ aFragmentInstanceListIndex ].FragmentTypeId;
      return( rc );
      }

    static
    int
    GetExternalSiteTableIndex( int aFragmentInstanceListIndex )
      {
      int rc =  FragmentInstanceList[ aFragmentInstanceListIndex ].ExternalSiteTableIndex;
      return( rc );
      }

    static
    int
    GetInternalAbsoluteSiteStartIndex( int aFragmentInstanceListIndex )
      {
      int rc =  FragmentInstanceList[ aFragmentInstanceListIndex ].InternalAbsoluteSiteStartIndex;
      return( rc );
      }

    static
    int
    GetFragmentNeighborListIndex( int aFragmentInstanceListIndex )
      {
      int rc =  FragmentInstanceList[ aFragmentInstanceListIndex ].FragmentNeighborListIndex;
      return( rc );
      }

    static
    int
    GetFragmentNeighborCount( int aFragmentInstanceListIndex )
      {
      int rc =  FragmentInstanceList[ aFragmentInstanceListIndex ].FragmentNeighborCount;
      return( rc );
      }

    static
    const int *
    GetFragmentNeighborList( int aFragmentInstanceListIndex )
      {
      const int *rc =  & FragmentNeighborList[ aFragmentInstanceListIndex ];
      return( rc );
      }

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //NOTE: This MUST be pased the FragmentInstanceListIndex that te SiteRef is relative to!!!!!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    static
    inline
    int
    GetAbsoluteSiteId( const int aFragmentInstanceListIndex, const SiteRef aSiteRef ) const
      {
      int AbsSiteId = -1;
      if( aSiteRef.mAbsoluteFlag )
        {
        AbsSiteId =
          ExternalSiteTable[ aSiteRef.mOffset + GetExternalSiteTableIndex( aFragmentInstanceListIndex ) ];
        }
      else
        {
        AbsSiteId =
            GetInternalAbsoluteSiteStartIndex( aFragmentInstanceListIndex  )
          + aSiteRef.mOffset;
        }
      return( AbsSiteId );
      }

    static
    inline
    int
    GetAbsoluteSiteId( const int aFragmentInstanceListIndex, const int aInternalSite ) const
      {
      int AbsSiteId = -1;
      AbsSiteId =
            GetInternalAbsoluteSiteStartIndex( aFragmentInstanceListIndex  )
          + aInternalSite;
      return( AbsSiteId );
      }

    static
    const
    SiteSpec &
    GetSiteSpecTableEntry( int SiteSpecTableIndex )  // The same as saying "site type id"
      {
      const SiteSpec &rc     =  SiteSpecTable[ SiteSpecTableIndex ];
      return( rc );
      }

    static
    const
    MolSysDef::FragSpec &
    AccessFragSpec( int aFragmentInstanceListIndex )
      {
      int FragSpecTableIndex = GetFragmentTypeId( aFragmentInstanceListIndex );
      const MolSysDef::FragSpec &fr =  (const MolSysDef::FragSpec &)FragSpecTable[ FragSpecTableIndex ];  // for debugging
      return( fr );
      }

    static
    int
    LoadMsdFile( char * MsdFileName ) {
       return 0 ;
    } ; // When linked, 'load from file' is dummy
//**
} ;
// 'loaded from file' form of Molecular System Definition interface
#else
class MSDIF_BASE
{
   private:
    //NEED: Possibly should use a lock var to serialize instantiation.

    static MolSysDef   FlatHeader;       // Header contains the flat portion of the file

    static MolSysDef * SingletonPtr; // A pointer to the complete file, including deep parts of the structure.

  public:

    typedef xxxFragSpec FragSpec;

    const
    static
    SiteSpec * GetSiteSpecTable ()
    {
       return FlatHeader.SiteSpecTable ;
    }

    const
    static
    int * GetExternalSiteTable ()
    {
       return FlatHeader.ExternalSiteTable ;
    }

    static
    inline
    MolSysDef &
    Access()
      {
      //BGF Should be more checking.
      assert( SingletonPtr != NULL );
      return( * SingletonPtr );
      }

    static
    int
    LoadMsdFile( char * MsdFileName )
#if 0
      {
      int fd = open( MsdFileName, O_RDONLY );
      assert( fd > 0 );  // Failed to open MolSysDef file

      int HeaderBytesRead = read( fd, (char*)(&FlatHeader) , sizeof( MolSysDef ) );
      assert( HeaderBytesRead == sizeof( MolSysDef ) );

      int TotalByteSize = FlatHeader.TotalByteSize ;

      //BGF allocate as double - this is supposed to get good alinement. Is it enough?  Does it matter?
      SingletonPtr = (MolSysDef *) new double[ ( TotalByteSize + sizeof(double) - 1 ) / 8 ];
      assert( SingletonPtr != NULL );

      lseek( fd, 0, SEEK_SET );

      int BytesRead = read( fd, SingletonPtr, TotalByteSize );
      assert( BytesRead == TotalByteSize );

      close( fd );

      BegLogLine(1)
        << "MSDIF() read file >"
        << MsdFileName
        << "< for "
        << BytesRead
        << " BytesRead "
        << EndLogLine;

      Access().ConvertOffsetsToPointers();

      // Copy the corrected values back into the static header.
      memcpy( &FlatHeader, SingletonPtr, sizeof( MolSysDef ) );

      return( 0 );
      }
#else
  ;
#endif
//**

    int
    static
    GetNumberOfSites()
      {
      return( FlatHeader.NumberOfSites );
      }

    static
    int
    GetVDWCombine()
      {
      return(  FlatHeader.VDWCombine );
      }

    static
    int
    GetNumberOfFragmentInstances()
      {
      return(  FlatHeader.NumberOfFragmentInstances );
      }


static int
GetNumberOfSiteSpecs()
  {
  return( FlatHeader.NumberOfSiteSpecs );
  }




    static
    int
    GetFragmentTypeId( int aFragmentInstanceListIndex )
      {
      int rc =  FlatHeader.FragmentInstanceList[ aFragmentInstanceListIndex ].FragmentTypeId;
      return( rc );
      }

    static
    int
    GetExternalSiteTableIndex( int aFragmentInstanceListIndex )
      {
      int rc =  FlatHeader.FragmentInstanceList[ aFragmentInstanceListIndex ].ExternalSiteTableIndex;
      return( rc );
      }

    static
    int
    GetInternalAbsoluteSiteStartIndex( int aFragmentInstanceListIndex )
      {
      int rc =  FlatHeader.FragmentInstanceList[ aFragmentInstanceListIndex ].InternalAbsoluteSiteStartIndex;
      return( rc );
      }

    static
    int
    GetFragmentNeighborListIndex( int aFragmentInstanceListIndex )
      {
      int rc =  FlatHeader.FragmentInstanceList[ aFragmentInstanceListIndex ].FragmentNeighborListIndex;
      return( rc );
      }

    static
    int
    GetFragmentNeighborCount( int aFragmentInstanceListIndex )
      {
      int rc =  FlatHeader.FragmentInstanceList[ aFragmentInstanceListIndex ].FragmentNeighborCount;
      return( rc );
      }

    static
    int *
    GetFragmentNeighborList( int aFragmentInstanceListIndex )
      {
      int *rc =  & FlatHeader.FragmentNeighborList[ aFragmentInstanceListIndex ];
      return( rc );
      }

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //NOTE: This MUST be pased the FragmentInstanceListIndex that te SiteRef is relative to!!!!!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    static
    inline
    int
    GetAbsoluteSiteId( int aFragmentInstanceListIndex, SiteRef aSiteRef )
      {
      int AbsSiteId = -1;
      if( aSiteRef.mAbsoluteFlag )
        {
        AbsSiteId =
          FlatHeader.ExternalSiteTable[ aSiteRef.mOffset + GetExternalSiteTableIndex( aFragmentInstanceListIndex ) ];
        }
      else
        {
        AbsSiteId =
            GetInternalAbsoluteSiteStartIndex( aFragmentInstanceListIndex  )
          + aSiteRef.mOffset;
        }
      return( AbsSiteId );
      }

    static
    inline
    int
    GetAbsoluteSiteId( int aFragmentInstanceListIndex, int aInternalSite )
      {
      int AbsSiteId = -1;
      AbsSiteId =
            GetInternalAbsoluteSiteStartIndex( aFragmentInstanceListIndex  )
          + aInternalSite;
      return( AbsSiteId );
      }


    static
    const
    SiteSpec &
    GetSiteSpecTableEntry( int SiteSpecTableIndex )  // The same as saying "site type id"
      {
      const SiteSpec &rc     =  FlatHeader.SiteSpecTable[ SiteSpecTableIndex ];
      return( rc );
      }

    static
    const
    LennardJonesParameters &
    GetCombinedLennardJonesParameter( int SiteSpecIdA,
                                      int SiteSpecIdB )
      {
      if( SiteSpecIdA < SiteSpecIdB ) // McSwap, dude.
        {
        SiteSpecIdA ^= SiteSpecIdB;
        SiteSpecIdB ^= SiteSpecIdA;
        SiteSpecIdA ^= SiteSpecIdB;
        }

      int Index = (SiteSpecIdA * ( SiteSpecIdA + 1 )) / 2 + SiteSpecIdB;

      const LennardJonesParameters &LJP   =
           FlatHeader.mCombinedLennardJonesParameters[ Index ];

      BegLogLine(1)
        << " MSDIF::GetCombinedLennardJones Parameter(): "
        << " SiteIdA "
        << SiteSpecIdA
        << " SiteIdB "
        << SiteSpecIdB
        << " LJ Index "
        << Index
        << " LJ Parm "
        << LJP.mEpsilon
        << " "
        << LJP.mSigma
        << EndLogLine;

      return( LJP );
      }

    static
    const
    LennardJonesParameters &
    GetCombinedLennardJones14Parameter( int SiteSpecIdA,
                                        int SiteSpecIdB )
      {
      if( SiteSpecIdA < SiteSpecIdB ) // McSwap, dude.
        {
        SiteSpecIdA ^= SiteSpecIdB;
        SiteSpecIdB ^= SiteSpecIdA;
        SiteSpecIdA ^= SiteSpecIdB;
        }

      int Index = (SiteSpecIdA * ( SiteSpecIdA + 1 )) / 2 + SiteSpecIdB;

      const LennardJonesParameters &LJP   =
           FlatHeader.mCombinedLennardJones14Parameters[ Index ];


      BegLogLine(1)
        << " MSDIF::GetCombinedLennardJones 14 Parameter(): "
        << " SiteIdA "
        << SiteSpecIdA
        << " SiteIdB "
        << SiteSpecIdB
        << " LJ Index "
        << Index
        << " LJ Parm "
        << LJP.mEpsilon
        << " "
        << LJP.mSigma
        << EndLogLine;


      return( LJP );
      }

#if 0 // this is probably not a good fx to have
    static
    const
    SiteSpec &
    GetSiteSpecTableEntry( int aFragmentInstanceListIndex,
                           int aOffset )
      {
      //NEED: with additional info in MSD this could be MUCH less expensive in terms of pointer chasing.
      //      Should have a per-site map to site type.
      int SiteSpecTableIndex = AccessFragSpec( aFragmentInstanceListIndex ).InternalSiteSpecTableIndex[ aOffset ];
      const SiteSpec &rc     =  FlatHeader.SiteSpecTable[ SiteSpecTableIndex ];
      return( rc );
      }
#endif

    static
    const
    FragSpec &
    AccessFragSpec( int aFragmentInstanceListIndex )
      {
      int FragSpecTableIndex = GetFragmentTypeId( aFragmentInstanceListIndex );
      const FragSpec &fr =  FlatHeader.FragSpecTable[ FragSpecTableIndex ];  // for debugging
      return( fr );
      }

//**
} ;
#endif

// Molecular System Definition interface.
class MSDIF: public MSDIF_BASE
  {
  public:

    static
    void
    GetUdfParameterAddress( int aUdfCode, int aParmIndex, void *& aAddr )
      {

      // USE IMPLICIT REGISTRY - a la MIKE - UNTIL FRANKS REGISTRY IS READY

      // This only works with current msd files if there is only one fragment
      // so...

      assert( GetNumberOfFragmentInstances() == 1 );

      const MolSysDef::FragSpec & FragSpec = AccessFragSpec( 0 );

      switch( aUdfCode )
        {
        case  0   :         //#define TERMINATECODE
            assert(0);
          break;
        case  1   :         //#define BONDCODE
            aAddr = (void *) &  FragSpec.BondTypeTable[ aParmIndex ];
          break;
        case  2   :         //#define ANGLECODE
            aAddr = (void *) &  FragSpec.AngleTypeTable[ aParmIndex ];
          break;
        case  3   :         //#define TORSIONCODE
            aAddr = (void *) &  FragSpec.TorsionTypeTable[ aParmIndex ];
          break;
        case  4   :         //#define IMPROPERCODE
            aAddr = (void *) &  FragSpec.ImproperTypeTable[ aParmIndex ];
          break;
        case  5   :         //#define EXCLUDEDCODE
            assert( 0 );
          break;
        case  6   :         //#define UBCODE
            aAddr = (void *) &  FragSpec.UreyTypeTable[ aParmIndex ];
          break;
        case  7   :         //#define PAIR14CODE
            assert( 0 );
          break;
        case  8   :         //#define OPLSTORSIONCODE
            aAddr = (void *) &  FragSpec.TorsionTypeTable[ aParmIndex ];
          break;
        case  9   :         //#define OPLSIMPROPERCODE
            aAddr = (void *) &  FragSpec.TorsionTypeTable[ aParmIndex ];
          break;
        default:
          assert( 0 );
        }

      return;
      }


    // Check if an Site in A is excluded from non-bond calculations a site in B
    // Pick the appropriate functional form for the non-bond interaction
    static
    unsigned
    GetInteractionCodeMask( unsigned aAbsoluteSiteIdA,
                            unsigned aAbsoluteSiteIdB )
      {

      assert( GetNumberOfFragmentInstances() == 1 );

      unsigned rc = GetInteractionCodeMask( 0,
                                            aAbsoluteSiteIdA,
                                            0,
                                            aAbsoluteSiteIdB ) ;
      return( rc );
      }


    // Check if an Site in A is excluded from non-bond calculations a site in B
    // Pick the appropriate functional form for the non-bond interaction
    static
    unsigned
    GetInteractionCodeMask( unsigned aFragInstIdA,
                            unsigned aOffsetA,
                            unsigned aFragInstIdB,
                            unsigned aOffsetB )
      {

      if( aFragInstIdA == aFragInstIdB )
        {
        if( aOffsetA == aOffsetB  )
          {
          // Same FragId, Same Offset
          BegLogLine(1)
            << "GetInteractionCodeMask "
            << " NB:EXCLUDED "
            << " AbsIdA " << GetInternalAbsoluteSiteStartIndex( aFragInstIdA ) + aOffsetA
            << " AbsIdA " << GetInternalAbsoluteSiteStartIndex( aFragInstIdB ) + aOffsetB
            << " IDENTITY "
            << EndLogLine;
          return( NonbondedOperationBitEnum::Excluded ); // yes, excluded
          }
        else
          {
          // Same FragId, Different Offset
          // Make sure that if this is an internal exclusion that
          // we order the offsets low/high because high/low isn't
          // in the Exclusion lists that come out of probspec.
          if( aOffsetA > aOffsetB )
            {
              BegLogLine(0)
                << "GetInteractionCodeMask "
                << " FragId A == B BUT Offset A > B "
                << " AbsIdA " << GetInternalAbsoluteSiteStartIndex( aFragInstIdA ) + aOffsetA
                << " AbsIdB " << GetInternalAbsoluteSiteStartIndex( aFragInstIdB ) + aOffsetB
                << " Swap Offset A for B. "
                << EndLogLine;

            // McSwap, dude.   Look FRANK, no hands!!!!
            aOffsetA ^= aOffsetB;
            aOffsetB ^= aOffsetA;
            aOffsetA ^= aOffsetB;
            }
          }
        }
      else // else FragIds are different.
        {
        // If FragIds are different, it is important to order them
        // before looking at the exclusion or 1:4 tables in the MSD
        // because they only have have the lower triangle of pairs
        // represented.
        if( aFragInstIdA > aFragInstIdB )
          {
          BegLogLine(0)
            << "GetInteractionCodeMask "
            << " FragId A > B  "
            << " AbsIdA " << GetInternalAbsoluteSiteStartIndex( aFragInstIdA ) + aOffsetA
            << " AbsIdB " << GetInternalAbsoluteSiteStartIndex( aFragInstIdB ) + aOffsetB
            << " Swapping FragIds and Offsets "
            << EndLogLine;

          // Do a double McSwap, dude.   Look FRANK, no hands OR feet!!!!
          aFragInstIdA ^= aFragInstIdB;
          aFragInstIdB ^= aFragInstIdA;
          aFragInstIdA ^= aFragInstIdB;

          aOffsetA ^= aOffsetB;
          aOffsetB ^= aOffsetA;
          aOffsetA ^= aOffsetB;

          }
        }


      // This block checks neighbor lists to see it can be determined
      // that sites are NOT IN NEIGHBORING FRAGMENT INSTANCES.
      // NOTE: Neighborhoods are defined by shared exclusions.
      // This is NOT THE SAME AS CHECKING THE SITE BY SITE EXCLUSION LISTS
      // which is handled below.
      if( aFragInstIdA != aFragInstIdB )
        {
        // Assume the common case - that FragB will NOT be a neighbor.
        int FoundFlag = 0;

        // Check out the neighbor list to see if there could still be an exclusion.
        int FragmentNeighborCount =
            GetFragmentNeighborCount( aFragInstIdA );

        if( FragmentNeighborCount != 0 )
          {

          //Got neigbors, scan list
          int FragmentNeighborListIndex =
              GetFragmentNeighborListIndex( aFragInstIdA );

          const int *FragmentNeighborList =
               GetFragmentNeighborList( FragmentNeighborListIndex );

          // Search FragmentNeighborList for FragInstIdB
          for( int i = 0;
               i < FragmentNeighborCount;
               i++ )
            {
            if( FragmentNeighborList[ i ] == aFragInstIdB )
              {
              FoundFlag = 1;
              break;
              }
            }
          }

        // If NOT found, they're not in neighboring fragments so they
        // are included.
        // NOTE: No neighbor list, then nothing is found - which means do the basic NBs.
        if( ! FoundFlag )
          {
          BegLogLine(0)
            << "GetInteractionCodeMask "
            << " NB:BASIC "
            << " AbsIdA " << GetInternalAbsoluteSiteStartIndex( aFragInstIdA ) + aOffsetA
            << " AbsIdA " << GetInternalAbsoluteSiteStartIndex( aFragInstIdB ) + aOffsetB
            << " FragInstB IS NOT A NEIGHBOR AND NOT EQUAL TO FragInstA "
            << " FragmentNeighborCount " << FragmentNeighborCount
            << EndLogLine;
          return(    NonbondedOperationBitEnum::LJBasic
                   | NonbondedOperationBitEnum::Coulomb    ); // not excluded
          }
        }

      // Important:
      // By the time you get here, either FragInstIdA and FragInstIdB are
      // either the same or neighbors, so must look in fragment exclusion list.

      int FragInstB_StartIndex = GetInternalAbsoluteSiteStartIndex( aFragInstIdB );
      int QueryAbsIdB          = FragInstB_StartIndex + aOffsetB ;

      const MolSysDef::FragSpec & FragSpecA = AccessFragSpec( aFragInstIdA );


///NEED: Move 1:4s after the Exclusion since there ought to be less of them.


      const SiteRef *Pair14SitePairs = FragSpecA.Pair14SitePairs;

      //NEED: Change the C++ type of ExcludedPair14SitePairs table or make it a table of actual Pair structs.
      // NEED NEED NEED: clean up the naming of the pair lists!!!!!!!!!!!!!!!!!!!!!!!!!!
      for( int e14p = 0; e14p < FragSpecA.NumberPair14SitePairs; e14p += 2 )
        {
        // Yeah yeah, we know this assumes the internals of a SiteRef, FRANK!!!!

        // this nasty little break out assumes the order of the list as well!!!
//////  if( aOffsetA < FragSpecA.Pair14SitePairs[ e14p ].mOffset )
//////    break;

        if( aOffsetA == FragSpecA.Pair14SitePairs[ e14p ].mOffset )
          {
          // Method to get abs id is on FragA which owns the list.
          int TableAbsIdB = GetAbsoluteSiteId( aFragInstIdA, Pair14SitePairs[ e14p + 1 ] );
          if( QueryAbsIdB == TableAbsIdB )
            {
            BegLogLine(1)
              << "GetInteractionCodeMask "
              << " ONEFOUR "
              << " AbsIdA " << GetInternalAbsoluteSiteStartIndex( aFragInstIdA ) + aOffsetA
              << " AbsIdB " << TableAbsIdB
              << EndLogLine;
            return(    NonbondedOperationBitEnum::OneFour
                     | NonbondedOperationBitEnum::LJBasic
                     | NonbondedOperationBitEnum::Coulomb  );
            }
          }
        }


      const SiteRef *ExcludedSitePairs = FragSpecA.ExcludedSitePairs;

      //NEED: Change the C++ type of ExcludedSitePairs table or make it a table of actual Pair structs.
      // NEED NEED NEED: clean up the naming of the pair lists!!!!!!!!!!!!!!!!!!!!!!!!!!

      for( int esp = 0; esp < FragSpecA.NumberExcludedSitePairs; esp += 2 )
        {
        // Yeah yeah, we know this assumes the internals of a SiteRef, FRANK!!!!

        // this nasty little break out assumes the order of the list as well!!!
//////  if( aOffsetA < FragSpecA.ExcludedSitePairs[ esp ].mOffset )
//////    break;

        if( aOffsetA == FragSpecA.ExcludedSitePairs[ esp ].mOffset )
          {
          // Method to get abs id is on FragA which owns the list.
          int TableAbsIdB = GetAbsoluteSiteId( aFragInstIdA, ExcludedSitePairs[ esp + 1 ] );

          if( QueryAbsIdB == TableAbsIdB )
            {
            BegLogLine(1)
              << "GetInteractionCodeMask "
              << " NB:EXCLUDED "
              << " AbsIdA " << GetInternalAbsoluteSiteStartIndex( aFragInstIdA ) + aOffsetA
              << " AbsIdB " << TableAbsIdB
              << " On excluded list. "
              << EndLogLine;
            return(  NonbondedOperationBitEnum::Excluded  ); // yes, Excluded
            }
          }
        }

      // Here, means that Frags A & B are neighbors but the interaction is
      // neither excluded nor a 14, so it is a basic.
      BegLogLine(0)
        << "GetInteractionCodeMask "
        << " NB:BASIC "
        << " AbsIdA " << GetInternalAbsoluteSiteStartIndex( aFragInstIdA ) + aOffsetA
        << " AbsIdB " << GetInternalAbsoluteSiteStartIndex( aFragInstIdB ) + aOffsetB
        << " Neighbor frags but not Ex or 14 "
        << EndLogLine;

      return(    NonbondedOperationBitEnum::LJBasic
               | NonbondedOperationBitEnum::Coulomb     );  // not excluded.
      }



  };

#if 0
// This is a depreciated interface.
// Access MSD from MSDIF
  MolSysDef &
  AccessMolSysDef()
    {
    return( MSDIF().Access() );
    }
#endif

#endif

