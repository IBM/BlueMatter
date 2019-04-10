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
 #ifndef _MSD_PREFIX_HPP_
#define _MSD_PREFIX_HPP_

extern unsigned SimTick ;

class PreMSD
{
    public:
    enum { STDCOULOMB, EWALD, P3ME, FRC, NO_LRF };
    enum { GRADIENT=-1, ANALYTICAL=0, FINITE_2PT=1, FINITE_4PT=2, FINITE_6PT=3, FINITE_8PT=4, FINITE_10PT=5, FINITE_12PT=6 };
};

struct UdfInvocation
{
  int   udfCode;
  int   size;
  SiteId*  siteTupleList;
  void* paramTupleList;

  int   combineFlag;
  int   udfCombinationCode;

  inline
  SiteId*
  GetNthTuple(int SitesPerTuple, int tupleNumber)
    {
      int offset = SitesPerTuple*tupleNumber;
      assert((tupleNumber < size) && (offset >= 0 ));
      return siteTupleList + offset;
    }
};

struct SiteTuplePair
{
public:
  SiteId site1;
  SiteId site2;

  int operator==(const SiteTuplePair& ep) const
    {
      if( (site1 == ep.site1) &&
          (site2 == ep.site2) )
        {
          return 1;
        }

      return 0;
    }

  static int compare(const void* si1, const void* si2)
    {
      SiteTuplePair* ep1 = (SiteTuplePair *) si1;
      SiteTuplePair* ep2 = (SiteTuplePair *) si2;

      if (ep1->site1 < ep2->site1)
          return -1;
      else if ( ep1->site1 == ep2->site1 )
          {
            if ( ep1->site2 < ep2->site2 )
              return -1;
            else if (ep1->site2 == ep2->site2)
              return 0;
            else
              return 1;
          }
      else
        return 1;
    }
};

struct NsqUdfInvocation
{
    int udfCode;
    int combinationCode;
    int nsqType;
    int size;
    int incExcType;
    SiteTuplePair* incExcList;
    void* paramTupleList;
};


struct IrreduciblePartitionType
{
  int numSites;
  int moleculeType;
  int tagAtomIndex;
  int numConstraints;
  double extent;
};

struct IrreduciblePartitionMap
{
    int startSiteId;
    int fragmentType;
    int moleculeId;
    int fragmentIndexInMol;
};

struct SiteInfo
{
  double mass;
  double halfInverseMass;
  double charge;
};

// Stuff to do with UDF binding to framework



class UDF_Base
  {
  public:
    enum { Code        = 0 };
    enum { UsesVelocity = 0 };
    enum { UsesPositions = 0 };
    //// BGF I think these names should be EXACLY the method names after some prefix
    enum { CalculatesEnergy = 0 };
    enum { ReportsVelocity = 0 };
    enum { ReportsPositions = 0 };
    enum { ReportsForce = 0 };
    enum { ReportsEnergy = 0 };
    enum { ReportsVirial = 0 };
    enum { RespaShellsEnabled = 1 };
    enum { BondVectorOptimisationEnabled = 0 };
    // 4-site bonded UDFs will want either 'extended' (like acetylene) or 
    // 'nose-first' (like ammonia) presentation of vectors
    enum { IsExtendedKind = 0 } ;
    enum { IsNoseFirstKind = 0 } ; 

    static char* UdfName;
  };

#if !defined(MSD_COMPILE_CODE_ONLY)
char* UDF_Base::UdfName = "UDF_Base";
#endif

#include <BlueMatter/IFP_Binder.hpp>

template<class tUDF0_Bound, class tUDF1_Bound = NULL_IFP_Binder, class tUDF2_Bound = NULL_IFP_Binder>
class UdfGroup
  {
  public:
    typedef tUDF0_Bound UDF0_Bound;
    typedef tUDF1_Bound UDF1_Bound;
    typedef tUDF2_Bound UDF2_Bound;

    enum { ActiveUDFCount =
           ( ( UDF0_Bound::UDF::Code == -1 ? 0 : 1 ) +
             ( UDF1_Bound::UDF::Code == -1 ? 0 : 1 ) +
             ( UDF2_Bound::UDF::Code == -1 ? 0 : 1 ) )
         };
  };


#endif
