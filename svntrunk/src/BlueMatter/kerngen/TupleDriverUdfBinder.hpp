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
 #ifndef __TUPLEBINDER_HPP__
#define __TUPLEBINDER_HPP__


template< class T_UDF, int TupleCount, const SiteId SiteTupleList[], class ParamTypeAccessor>
class TupleDriverUdfBinder
  {
  public:
    typedef T_UDF UDF;

    typedef SiteId TupleType[ UDF::NumberOfSites ];
    typedef typename ParamTypeAccessor::ParamType ParamType;
    
// Should really be 'UDF::NumberOfSites - 1', but there are some 1-site UDFs around and the compiler complained
    typedef unsigned int BondIndexType[ UDF::NumberOfSites  ];
    typedef unsigned char BondSenseType [ UDF::NumberOfSites ];

    static 
    inline
    const int
    GetTupleCount()
      {
      int rc = TupleCount;
      return(rc);
      }

    static
    inline
    TupleType &
    GetTuple( int aTupleIndex )
      {
      int ListIndex = aTupleIndex * UDF::NumberOfSites ;
      TupleType & Tuple = *((TupleType *) &SiteTupleList[ ListIndex ]);
      return( Tuple );
      }

    static 
    inline
    void
    Init()
      {
      ParamTypeAccessor::Init();
      }

    static 
    inline
    typename ParamTypeAccessor::ParamType
    GetParam( int aTupleIndex )
      {
      A98_TYPENAME ParamTypeAccessor::ParamType Param;
      Param = ParamTypeAccessor::GetParam( aTupleIndex );
      return( Param );
      }

  };


class NULL_ParamAccessor
{
public:
  class ParamType
    {
    };

  static
  void
  Init(){};

  static
  ParamType
  GetParam( int i )
    {
    ParamType p;
    // printf("NULLParamAccessor::GetParam()\n");
    return( p );
    }
};

extern SiteId NULL_SiteTupleList[1];
#if !defined(LEAVE_SYMS_EXTERN)
SiteId NULL_SiteTupleList[1];
#endif

class NULL_UDF
  : public UDF_Base
  {
  public:
    enum {NumberOfSites = 1 };
    enum { Code = -1 }; //NEED: should get this out of the registry

    template<class MDVM, class Param>
    static
    void
    Execute(MDVM &m, Param & p)
      {
      PLATFORM_ABORT(" NULL_UDF CALLED " );
      }
  };

typedef TupleDriverUdfBinder< NULL_UDF, 0, NULL_SiteTupleList, NULL_ParamAccessor > NULL_TupleDriverUdfBinder;




template<///////class DynVarMgr,
         class MDVM,
         class UDF1_Bound = NULL_TupleDriverUdfBinder,
         class UDF2_Bound = NULL_TupleDriverUdfBinder,
         class UDF3_Bound = NULL_TupleDriverUdfBinder,
         class UDF4_Bound = NULL_TupleDriverUdfBinder,
         class UDF5_Bound = NULL_TupleDriverUdfBinder,
         class UDF6_Bound = NULL_TupleDriverUdfBinder,
         class UDF7_Bound = NULL_TupleDriverUdfBinder,
         class UDF8_Bound = NULL_TupleDriverUdfBinder,
         class UDF9_Bound = NULL_TupleDriverUdfBinder
         >
class SiteTupleDriver;

template<///////class DynVarMgr,
         class MDVM,
         class UDF1_Bound = NULL_TupleDriverUdfBinder,
         class UDF2_Bound = NULL_TupleDriverUdfBinder,
         class UDF3_Bound = NULL_TupleDriverUdfBinder,
         class UDF4_Bound = NULL_TupleDriverUdfBinder,
         class UDF5_Bound = NULL_TupleDriverUdfBinder,
         class UDF6_Bound = NULL_TupleDriverUdfBinder,
         class UDF7_Bound = NULL_TupleDriverUdfBinder,
         class UDF8_Bound = NULL_TupleDriverUdfBinder,
         class UDF9_Bound = NULL_TupleDriverUdfBinder
         >
class MolTupleDriver;

#endif
