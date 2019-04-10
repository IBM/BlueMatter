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
 #ifndef _MOLSYSDEF_HPP_
#define _MOLSYSDEF_HPP_
// This defines the in-memory rep of a molecular system definition
// and also defines a ".msd" file.

#include <pk/platform.hpp>
#include <pk/fxlogger.hpp>

#include <BlueMatter/math.hpp>
//#include <BlueMatter/ForceParameters.hpp>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/Old_Binding.hpp>
#include <BlueMatter/SiteSpec.hpp>
#include <BlueMatter/MDVM_Code.hpp>


// These defines are part of the agreement between the application kernel
// and the setup code.

const double mTimeConversion   = 0.0488882;
const double mChargeConversion = 18.2224;

class FragmentInstanceListItem
  {
  public:
    int FragmentTypeId;
    int InternalAbsoluteSiteStartIndex;
    int ExternalSiteTableIndex;
    int FragmentNeighborListIndex;  // Index to a list of records containing Fragment neighbors
    int FragmentNeighborCount;      // Count of number of neighbors in above record.
  };


#define  BONDTYPE UDF_Registry::StdHarmonicBondForce_Params
#define  ANGLETYPE UDF_Registry::StdHarmonicAngleForce_Params
#define  UREYTYPE UDF_Registry::UreyBradleyForce_Params
#define  TORSIONTYPE UDF_Registry::SwopeTorsionForce_Params
#define  IMPROPERTYPE UDF_Registry::ImproperDihedralForce_Params


///class xFragSpec
class xxxFragSpec
  {
  public:
    int           NumberInternalSites;         ///// const
    int           NumberCodes;                 ///// const
    int          *InternalSiteSpecTableIndex;  ///// const
    int          *ExternalSiteSpecTableIndex;  ///// const
    MDVM_Code    *CodeList;                    ///// const
    BONDTYPE     *BondTypeTable;               ///// const
    ANGLETYPE    *AngleTypeTable;              ///// const
    UREYTYPE    *UreyTypeTable;                 ///// const
    TORSIONTYPE  *TorsionTypeTable;            ///// const
    IMPROPERTYPE *ImproperTypeTable;           ///// const

    int           NumberExcludedSitePairs; //NOTE THIS IS NOT NOT NOT THE NUMBER OF PAIRS BUT OF REFS
    SiteRef      *ExcludedSitePairs;       //Pairs of refs
    int           NumberPair14SitePairs;  // NOTE THIS IS NOT A PAIR COUNT BUT A REF COUNT, 2 REFS TO A PAIR!!!!!
    SiteRef      *Pair14SitePairs;
    int           NumnberFragmentNeighbors    ;
    int          *FragmentNeighborList    ;

    void
    AdjustAddressBase( int aDelta )
      {
      BegLogLine(1)
        << "FragSpec::AdjustAddressBase "
        << " NumberInternalSites "
        << NumberInternalSites
        << " NumberCodes "
        << NumberCodes
        << EndLogLine;

      InternalSiteSpecTableIndex = (int          *) (((char *)InternalSiteSpecTableIndex)  + aDelta);
      ExternalSiteSpecTableIndex = (int          *) (((char *)ExternalSiteSpecTableIndex)  + aDelta);
      CodeList                   = (MDVM_Code    *) (((char *)CodeList                  )  + aDelta);
      BondTypeTable              = (BONDTYPE     *) (((char *)BondTypeTable             )  + aDelta);
      AngleTypeTable             = (ANGLETYPE    *) (((char *)AngleTypeTable            )  + aDelta);
      UreyTypeTable                = (UREYTYPE    *) (((char *)UreyTypeTable               )  + aDelta);
      TorsionTypeTable           = (TORSIONTYPE  *) (((char *)TorsionTypeTable          )  + aDelta);
      ImproperTypeTable          = (IMPROPERTYPE *) (((char *)ImproperTypeTable         )  + aDelta);
      ExcludedSitePairs          = (SiteRef      *) (((char *)ExcludedSitePairs         )  + aDelta);
      Pair14SitePairs            = (SiteRef      *) (((char *)Pair14SitePairs           )  + aDelta);
      FragmentNeighborList       = (int          *) (((char *)FragmentNeighborList       )  + aDelta);
      }
  };


//Magic number is writen into the data structure after pointer normalization and checked during remapping.
//NEED: be good to add check summing also!
static const int MolSysDef_MagicNumber = 0xFACEB0B2;  //increment least sig nibble on each change

class MolSysDef
  {
  public:
    typedef xxxFragSpec FragSpec;
    int                       TotalByteSize;          // total byte size of the structure as writen in a flat file.
    int                       MagicNumber;            // set this up to check that loaded file has a chance of being correct.
    int                       NumberOfSites            ;  // This value could be developed by scanning the FragSpecs.
    int                       NumberOfFragmentTypes;
    int                       NumberOfFragmentInstances;
    int                       VDWCombine             ;
    int                      *ExternalSiteTable        ;
    int                       NumberFragmentNeighbors    ;
    int                      *FragmentNeighborList    ;
    SiteSpec                 *SiteSpecTable            ;
    FragmentInstanceListItem *FragmentInstanceList     ;
    FragSpec                 *FragSpecTable            ;
    double                    ChargeScale14;
    double                    ChargeScaleStandard;
    double                    LJScale14;
    double                    LJScaleStandard;
#if 1
    // These are to be 1D representations of the uper triangle
    // of a 2D array of the dimension NumberOfSiteSpecs ** 2 * .5
    int NumberOfSiteSpecs;
    LennardJonesParameters   *mCombinedLennardJonesParameters;
    LennardJonesParameters   *mCombinedLennardJones14Parameters;
#endif

    void
    ConvertPointersToOffsets()
      {


      int aDelta = 0 - ((int)this) ;
      for( int i = 0; i < NumberOfFragmentTypes; i++ )
        FragSpecTable[ i ].AdjustAddressBase( aDelta );


      BegLogLine(1)
        << "ConvertOffsetsToPointers() "
        << " @this "
        << this
        << " Before "
        << FragmentInstanceList
        << " "
        << FragmentNeighborList
        << " "
        << FragSpecTable
        << " "
        << ExternalSiteTable
        << " "
        << SiteSpecTable
        << EndLogLine;


      FragmentInstanceList      = (FragmentInstanceListItem*) (((char *)FragmentInstanceList     )  + aDelta);
      FragmentNeighborList      = (int                     *) (((char *)FragmentNeighborList     )  + aDelta);
      FragSpecTable             = (FragSpec                *) (((char *)FragSpecTable            )  + aDelta);
      ExternalSiteTable         = (int                     *) (((char *)ExternalSiteTable        )  + aDelta);
      SiteSpecTable             = (SiteSpec                *) (((char *)SiteSpecTable            )  + aDelta);

#if 1
      mCombinedLennardJonesParameters  =
         (LennardJonesParameters *) (((char *)mCombinedLennardJonesParameters   )  + aDelta);
      mCombinedLennardJones14Parameters  =
         (LennardJonesParameters *) (((char *)mCombinedLennardJones14Parameters   )  + aDelta);
#endif


      // Put in the magic number
      MagicNumber               = MolSysDef_MagicNumber;

      BegLogLine(1)
        << "ConvertOffsetsToPointers() "
        << " @this "
        << this
        << " After "
        << FragmentInstanceList
        << " "
        << FragmentNeighborList
        << " "
        << FragSpecTable
        << " "
        << ExternalSiteTable
        << " "
        << SiteSpecTable
        << " Magic "
        << (void *) MagicNumber
        << EndLogLine;

      }

    void
    ConvertOffsetsToPointers()
      {
      BegLogLine(1)
        << "ConvertOffsetsToPointers() "
        << " @this "
        << " Magic is "
        << (void *) MagicNumber
        << " Expecting "
        << (void *) MolSysDef_MagicNumber
        << EndLogLine;

      assert( MagicNumber == MolSysDef_MagicNumber );  //check that this msd is valid

      BegLogLine(1)
        << "ConvertOffsetsToPointers() "
        << " @this "
        << this
        << " NumberOfSites "
        << NumberOfSites
        << " NumberOfFragmentInstances "
        << NumberOfFragmentInstances
        << EndLogLine;

      // should assert MagicNumber is correct here.
      int aDelta = ((int)this) ;

      BegLogLine(1)
        << "ConvertOffsetsToPointers() "
        << " @this "
        << this
        << " Delta "
        << aDelta
        << EndLogLine;

      BegLogLine(1)
        << "ConvertOffsetsToPointers() "
        << " @this "
        << this
        << " Before "
        << FragmentInstanceList
        << " "
        << FragSpecTable
        << " "
        << ExternalSiteTable
        << " "
        << SiteSpecTable
        << EndLogLine;

      MagicNumber               = MolSysDef_MagicNumber;
      FragmentInstanceList      = (FragmentInstanceListItem*) (((char *)FragmentInstanceList     )  + aDelta);
      FragmentNeighborList      = (int                     *) (((char *)FragmentNeighborList     )  + aDelta);
      FragSpecTable             = (FragSpec                *) (((char *)FragSpecTable            )  + aDelta);
      ExternalSiteTable         = (int                     *) (((char *)ExternalSiteTable        )  + aDelta);
      SiteSpecTable             = (SiteSpec                *) (((char *)SiteSpecTable            )  + aDelta);
#if 1
      mCombinedLennardJonesParameters  =
         (LennardJonesParameters *) (((char *)mCombinedLennardJonesParameters   )  + aDelta);
      mCombinedLennardJones14Parameters  =
         (LennardJonesParameters *) (((char *)mCombinedLennardJones14Parameters   )  + aDelta);
#endif

      BegLogLine(1)
        << "ConvertOffsetsToPointers() "
        << " @this "
        << this
        << " After "
        << FragmentInstanceList
        << " "
        << FragmentNeighborList
        << " "
        << FragSpecTable
        << " "
        << ExternalSiteTable
        << " "
        << SiteSpecTable
        << EndLogLine;


      BegLogLine(1)
        << "ConvertOffsetsToPointers() "
        << " @this "
        << this
        << " Root struct pointers reset "
        << EndLogLine;


      for( int i = 0; i < NumberOfFragmentTypes; i++ )
        {
        BegLogLine(1)
          << "ConvertOffsetsToPointers() "
          << " @this "
          << this
          << " Converting FragSpec "
          << i
          << EndLogLine;

        FragSpecTable[ i ].AdjustAddressBase( aDelta );
        }

      }

  };


// THESE FLAGS ARE TO BE USED INTERNAL TO THE FUNCTIONAL FORMS

class NonbondedOperationBitEnum
  {
  public:
    enum { Excluded              = 0x00000000, // Pair interaction is completly excluded
           LJBasic               = 0x00000001, // Do the basic LJ form.
           ChargeBasic           = 0x00000002, // Do the basic Charge-charge form.
           Virial                = 0x00000004, // Presure control is on - produce virial
           FreeEnergy            = 0x00000008, // FreeEnergy is on - do lamda stuff
           OneFour               = 0x00000010, // This is a 1:4 interaction
           Smoothing             = 0x00000020, // Smooth cutoff boundries.
           Ewald                 = 0x00000040  // Ewald charge methods in use
         };
  };

// MSD Interface class separated into its own header, but included by all users of molsysdef for convenience
// #include <BlueMatter/MSDIF.hpp>
#endif
