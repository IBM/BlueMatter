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
 #ifndef __IFP_BINDER_HPP__
#define __IFP_BINDER_HPP__

template< class T_UDF, class ParamTypeAccessor>
class IFP_Binder
  {
  public:
    typedef T_UDF UDF;

    typedef ParamTypeAccessor ParamAccessor;
    typedef typename ParamTypeAccessor::ParamType ParamType;

    static const 
    typename ParamTypeAccessor::ParamType&
    GetParam( int aTupleIndex )
      {
//      ParamTypeAccessor::ParamType Param;
//      Param = ParamTypeAccessor::GetParam( aTupleIndex );
//      return( Param );
        return ParamTypeAccessor::GetParam( aTupleIndex );
      } 
  };

class NULL_IFP_ParamAccessor
{
public:
  class ParamType
    {
    };

  static const 
  ParamType&
  GetParam( int i )
    {
    static ParamType p;
    assert( ! "NULL_UDF_ParamAccessor::GetParam() called - framework error." );
    return( p );
    }
};

class NULL_IFP_UDF
    : public UDF_Base
  {
  public:
    enum { Code = -1 };
    enum { NsqType = 0 };
    enum { TakesParameter = 0 };

    void
    Init()
      {
      assert( ! "NULL_UDF::Init() called - framework error." );
      }

    template<class MDVM, class Param>
    static
    void
    Execute(MDVM &m, Param & p)
      {
      assert( ! "NULL_UDF::Execute() called - framework error." );
      }

    template<class Param>
    static
    inline
    void
    Combine(const Param &A, const Param &B, Param &C)
      {
      assert( ! "NULL_UDF::Combine() called - framework error." );
      }

  };

typedef IFP_Binder< NULL_IFP_UDF, NULL_IFP_ParamAccessor > NULL_IFP_Binder;


#endif
