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
 // This file is meant to contain code that modifys how the
// UDF_Binding.hpp file information is seen by the core.
#ifndef __UDF_BINDING_WRAPPER_HPP__
#define __UDF_BINDING_WRAPPER_HPP__

#include <BlueMatter/UDF_Binding.hpp>

// There is a problem with the 2003-03-10 or so vacpp daily compiler
// which is patched around like this
#if defined(PATCH_BAD_SQRT)
#include <builtins.h>
inline double sqrtx(double x)
  {
  return __fsel(__fnabs(x),0.0,x/sqrt(x)) ;
  }
#else
#define sqrtx sqrt
#endif
class UDF_KineticEnergy
    : public UDF_Base
  {
  public:
  void Init(){}
  enum { Code = UDF_Binding::KineticEnergy_Code };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };

  static char* UdfName;

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_KineticEnergy_Execute( aMDVM );
    return;
    }
  };



class UDF_NoseHooverEnergy
    : public UDF_Base
  {
  public:
  void Init(){}
  // enum { Code = UDF_Binding::NoseHooverEnergy_Code };
  enum { Code = UDF_Binding::ResampleEnergyLoss_Code };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };

  static char* UdfName;

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_NoseHooverEnergy_Execute( aMDVM );
    return;
    }
  };



class UDF_UpdateVelocity
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_UpdateVelocity_Execute( aMDVM );
    return;
    }
  };


class UDF_SwitchFunction
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_SwitchFunction_Execute( aMDVM );
    return;
    }
  };


class UDF_SwitchFunctionForce
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_SwitchFunctionForce_Execute( aMDVM );
    return;
    }
  };



class UDF_AtomToFragmentVirialCorrection
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_AtomToFragmentVirialCorrection_Execute( aMDVM );
    return;
    }
  };


class UDF_UpdateVelocityFirstHalfStepNPT
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_UpdateVelocityFirstHalfStepNPT_Execute( aMDVM );
    return;
    }
  };


class UDF_UpdateVelocitySecondHalfStepNPT
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_UpdateVelocitySecondHalfStepNPT_Execute( aMDVM );
    return;
    }
  };


class UDF_UpdateCenterOfMass
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_UpdateCenterOfMass_Execute( aMDVM );
    return;
    }
  };


class UDF_UpdatePosition
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_UpdatePosition_Execute( aMDVM );
    return;
    }
  };



class UDF_UpdatePositionNPT
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_UpdatePositionNPT_Execute( aMDVM );
    return;
    }
  };



class UDF_UpdateCenterOfMassKineticEnergy
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  enum { CalculatesEnergy = 1 };

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_UpdateCenterOfMassKineticEnergy_Execute( aMDVM );
    return;
    }
  };


class UDF_ZeroForce
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template<class MDVM>
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_ZeroForce_Execute( aMDVM );
    return;
    }
  };


class UDF_ZeroCenterOfMassForce
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template<class MDVM>
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_ZeroCenterOfMassForce_Execute( aMDVM );
    return;
    }
  };


class UDF_UpdateCenterOfMassForce
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template<class MDVM>
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_UpdateCenterOfMassForce_Execute( aMDVM );
    return;
    }
  };


class UDF_UpdateCenterOfMassPosition
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template<class MDVM>
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_UpdateCenterOfMassPosition_Execute( aMDVM );
    return;
    }
  };


class UDF_UpdateCenterOfMassVelocity
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init() {}

  template<class MDVM>
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_UpdateCenterOfMassVelocity_Execute( aMDVM );
    return;
    }
  };


class UDF_StdHarmonicBondForce
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::StdHarmonicBondForce_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };
  enum { BondVectorOptimisationEnabled = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_StdHarmonicBondForce_Execute( aMDVM, Params);
    }
  };

class UDF_COMRestraint
    : public UDF_Base
  {
  public:
      enum { NumberOfSites = 2 }; // this is ignored
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::COMRestraint_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_COMRestraint_Execute( aMDVM, Params);
    }
  };


template< class UDF >
class UDF_VirialAdaptor
    : public UDF
  {
  public:

  void
  Init()
    {
    UDF::Init();
    }

  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF::Execute( aMDVM, Params);

    for( int s = 0; s < aMDVM.GetSiteCount(); s++ )
      {
      XYZ Force = aMDVM.GetForce( s );
      XYZ virial;
      XYZ Rcom = aMDVM.GetCenterOfMass( s );
      virial.mX = Force.mX * Rcom.mX;
      virial.mY = Force.mY * Rcom.mY;
      virial.mZ = Force.mZ * Rcom.mZ;
      aMDVM.ReportVirial( s, virial );
      }
    }
  };

typedef
UDF_VirialAdaptor< UDF_StdHarmonicBondForce >
UDF_StdHarmonicBondForceWithVirial;


class UDF_StdHarmonicAngleForce
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 3 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::StdHarmonicAngleForce_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };
  enum { BondVectorOptimisationEnabled = 1 };
  
  static char* UdfName;

  void
  Init()
    {
    }

  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_StdHarmonicAngleForce_Execute( aMDVM, Params);
    }
  };


class UDF_UreyBradleyForce
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::UreyBradleyForce_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };
  enum { BondVectorOptimisationEnabled = 1 };


  static char* UdfName;

  void
  Init()
    {
    }

  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_UreyBradleyForce_Execute( aMDVM, Params);
    }
  };



class UDF_SwopeTorsionForce
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 4 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::SwopeTorsionForce_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };
  enum { BondVectorOptimisationEnabled = 1 };
  enum { IsExtendedKind = 1 } ; 

  static char* UdfName;

  void
  Init()
    {
    }

  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_SwopeTorsionForce_Execute( aMDVM, Params);
    }
  };


class UDF_ImproperDihedralForce
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 4 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::ImproperDihedralForce_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };
  enum { BondVectorOptimisationEnabled = 1 };
  enum { IsNoseFirstKind = 1 } ; 
  

  static char* UdfName;

  void
  Init()
    {
    }

  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_ImproperDihedralForce_Execute( aMDVM, Params);
    }
  };


class UDF_OPLSImproperForce
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 4 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::OPLSImproperForce_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };
  enum { BondVectorOptimisationEnabled = 1 };
  enum { IsExtendedKind = 1 } ; 
  

  static char* UdfName;

  void
  Init()
    {
    }

  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_OPLSImproperForce_Execute( aMDVM, Params);
    }
  };


class UDF_OPLSTorsionForce
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 4 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::OPLSTorsionForce_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };
  enum { BondVectorOptimisationEnabled = 1 };
  enum { IsExtendedKind = 1 } ; 
  

  static char* UdfName;

  void
  Init()
    {
    }

  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_OPLSTorsionForce_Execute( aMDVM, Params);
    }
  };


class UDF_Coulomb14
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::Coulomb14_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };
  enum { BondVectorOptimisationEnabled = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_Coulomb14_Execute( aMDVM, Params);
    }
  };


class UDF_NSQCoulomb
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 2 };
  enum { Code = UDF_Binding::NSQCoulomb_Code };
  enum { NsqType = 2 };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template<class Param>
  static
  inline
  void
  Combine(const Param &A, const Param &B, Param &C)
    {
    C.ChargeAxB = A.ChargeAxB * B.ChargeAxB;
    }


  template <class MDVM, class ParamType>
  static
  inline
  void
  Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_NSQCoulomb_Execute( aMDVM, P);
    }
  };


// For EwaldCorrection and LeknerCorrection
class Coulomb1_2_3_4Combiner
  {
  public:
  template
  <class Param>
  static
  inline
  void
  Combine( const Param &A, const Param  &B, Param &C)
    {
    C.ChargeAxB = A.ChargeAxB * B.ChargeAxB;
    }
  };

#include <BlueMatter/Combining_Rules.hpp>

class LennardJonesOPLSAACombiner
  {
  public:
  template
  <class Param>
  static
  inline
  void
  Combine( const Param &A, const Param  &B, Param &C)
    {
    C.epsilon = sqrtx(A.epsilon * B.epsilon);
    C.sigma   = sqrtx(A.sigma   * B.sigma  );
    }
  };

class UDF_LennardJones14Force
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 1 };
  enum { NsqType = 1 };
  enum { Code = UDF_Binding::LennardJones14Force_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };
  enum { BondVectorOptimisationEnabled = 1 };
  
  static char* UdfName;

  void
  Init()
    {
    }

  template
  <class Param>
  static
  inline
  void
  Combine( const Param &A, const Param  &B, Param &C)
    {
    C.epsilon = sqrtx(A.epsilon * B.epsilon);
    C.sigma   =    (A.sigma   + B.sigma  );
    assert(0); // who is calling this???????
    }


  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_LennardJones14Force_Execute( aMDVM, Params);
    }
  };


// I DON'T UNDERSTAND WHY WE NEED A SEPERATE LJ FOR NSQ vs TUPLE DRIVEN.

class UDF_NSQCHARMMLennardJonesForce
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 1 };
  enum { NsqType = 1 };  // NOTE 1 means precombine
  enum { Code = UDF_Binding::NSQLennardJones_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template
  <class Param>
  static
  inline
  void
  Combine( const Param &A, const Param  &B, Param &C)
    {
    C.epsilon = sqrtx(A.epsilon * B.epsilon);
    //  Correct combining rule. NOTE must correspond with change in filter
    C.sigma   =    0.5 * (A.sigma   + B.sigma  );
    }


  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_NSQLennardJones_Execute( aMDVM, Params);
    }
  };


class UDF_NSQAMBERLennardJonesForce
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 1 };
  enum { NsqType = 1 };  // NOTE 1 means precombine
  enum { Code = UDF_Binding::NSQLennardJones_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template
  <class Param>
  static
  inline
  void
  Combine( const Param &A, const Param  &B, Param &C)
    {
    C.epsilon = sqrtx(A.epsilon * B.epsilon);
    C.sigma   =     (A.sigma   + B.sigma  ) * 0.5;
    }


  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_NSQLennardJones_Execute( aMDVM, Params);
    }
  };


class UDF_NSQOPLSLennardJonesForce
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 1 };
  enum { NsqType = 1 };
  enum { Code = UDF_Binding::NSQLennardJones_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template
  <class Param>
  static
  inline
  void
  Combine( const Param &A, const Param  &B, Param &C)
    {
    C.epsilon = sqrtx(A.epsilon * B.epsilon);
    C.sigma   = sqrtx(A.sigma   * B.sigma  );
    }


  template<class MDVM, class ParamType >
  static
  inline
  void
  Execute(MDVM &aMDVM, ParamType &Params)
    {
    UDF_Binding::UDF_NSQLennardJones_Execute( aMDVM, Params);
    }
  };


class UDF_EwaldCorrection
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::NSQEwaldCorrection_Code };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { ReportsVirial = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template <class MDVM, class ParamType>
  static
  inline
  void
  Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_NSQEwaldCorrection_Execute( aMDVM, P);
    }
  };


class UDF_NSQEwaldCorrection
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 2 };
  enum { Code = UDF_Binding::NSQEwaldCorrection_Code };
  enum { NsqType = 2 };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { ReportsVirial = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template<class Param>
  static
  inline
  void
  Combine(const Param &A, const Param &B, Param &C)
    {
    C.ChargeAxB = A.ChargeAxB * B.ChargeAxB;
    }


  template <class MDVM, class ParamType>
  static
  inline
  void
  Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_NSQEwaldCorrection_Execute( aMDVM, P);
    }
  };


class UDF_NSQEwaldRealSpace
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 2 };
  enum { Code = UDF_Binding::NSQEwaldRealSpace_Code };
  enum { NsqType = 2 };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { ReportsVirial = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }


  template<class Param>
  static
  inline
  void
  Combine(const Param &A, const Param &B, Param &C)
  {
  C.ChargeAxB = A.ChargeAxB * B.ChargeAxB;
  }


template <class MDVM, class ParamType>
static
inline
void
Execute( MDVM &aMDVM, const ParamType &P )
  {
  UDF_Binding::UDF_NSQEwaldRealSpace_Execute( aMDVM, P);
  }
};


class UDF_NSQEwaldRespaCorrection
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 2 };
enum { Code = UDF_Binding::NSQEwaldRespaCorrection_Code };
enum { NsqType = 2 };
enum { ReportsForce = 1 };
enum { CalculatesEnergy = 1 };
enum { ReportsEnergy = 1 };
enum { ReportsVirial = 1 };
enum { UsesPositions = 1 };
enum { RespaShellsEnabled = 0 };

static char* UdfName;

void
Init()
  {
  }


template<class Param>
static
inline
void
Combine(const Param &A, const Param &B, Param &C)
  {
  ////BGF C.Charge = A.Charge * B.Charge;
  C.ChargeAxB = A.ChargeAxB * B.ChargeAxB;
  }


template <class MDVM, class ParamType>
static
inline
void
Execute( MDVM &aMDVM, const ParamType &P )
  {
  UDF_Binding::UDF_NSQEwaldRespaCorrection_Execute( aMDVM, P);
  }
};



class UDF_LeknerCorrection
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 1 };
enum { Code = UDF_Binding::NSQLeknerCorrection_Code };
enum { ReportsForce = 1 };
enum { CalculatesEnergy = 1 };
enum { ReportsEnergy = 1 };
enum { UsesPositions = 1 };

static char* UdfName;

void
Init()
  {
  }

template <class MDVM, class ParamType>
static
inline
void
Execute( MDVM &aMDVM, const ParamType &P )
  {
  UDF_Binding::UDF_NSQLeknerCorrection_Execute( aMDVM, P);
  }
};



class UDF_NSQLeknerCorrection
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 2 };
enum { Code = UDF_Binding::NSQLeknerCorrection_Code };
enum { NsqType = 2 };
enum { ReportsForce = 1 };
enum { CalculatesEnergy = 1 };
enum { ReportsEnergy = 1 };
enum { UsesPositions = 1 };

static char* UdfName;

void
Init()
  {
  }

template<class Param>
static
inline
void
Combine(const Param &A, const Param &B, Param &C)
  {
  ////BGF C.Charge = A.Charge * B.Charge;
  C.ChargeAxB = A.ChargeAxB * B.ChargeAxB;
  }


template <class MDVM, class ParamType>
static
inline
void
Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_NSQLeknerCorrection_Execute( aMDVM, P);
    }
  };


class UDF_NSQLekner
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 2 };
  enum { Code = UDF_Binding::NSQLekner_Code };
  enum { NsqType = 2 };
  enum { ReportsForce = 1 };
  enum { CalculatesEnergy = 1 };
  enum { ReportsEnergy = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template<class Param>
  static
  inline
  void
  Combine(const Param &A, const Param &B, Param &C)
    {
    ////BGF C.Charge = A.Charge * B.Charge;
    C.ChargeAxB = A.ChargeAxB * B.ChargeAxB;
    }


  template <class MDVM, class ParamType>
  static
  inline
  void
  Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_NSQLekner_Execute( aMDVM, P);
    }
  };


class UDF_WaterTIP3PShake
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 3 };
  enum { TakesParameter = 0 };
  enum { Code = UDF_Binding::WaterTIP3PShake_Code };
  enum { UsesVelocity = 1 };
  enum { ReportsVelocity = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template <class MDVM, class ParamType>
  static
  inline
  void
  Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_WaterTIP3PShake_Execute(aMDVM);
    }

  };


class UDF_WaterTIP3PRattle
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 3 };
  enum { TakesParameter = 0 };
  enum { Code = UDF_Binding::WaterTIP3PRattle_Code };
  enum { UsesVelocity = 1 };
  enum { ReportsVelocity = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template <class MDVM, class ParamType>
  static
  inline
  void
  Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_WaterTIP3PRattle_Execute(aMDVM);
    }
  };


class UDF_WaterTIP3PInit
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 3 };
  enum { TakesParameter = 0 };
  enum { Code = UDF_Binding::WaterTIP3PInit_Code };
  enum { UsesVelocity = 1 };
  enum { ReportsVelocity = 1 };
  enum { ReportsPositions = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
  {
  }

template <class MDVM, class ParamType>
static
inline
void
Execute( MDVM &aMDVM, const ParamType &P )
  {
  UDF_Binding::UDF_WaterTIP3PInit_Execute(aMDVM);
  }
};


class UDF_WaterSPCShake
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 3 };
  enum { TakesParameter = 0 };
enum { Code = UDF_Binding::WaterSPCShake_Code };
enum { UsesVelocity = 1 };
enum { ReportsVelocity = 1 };
enum { UsesPositions = 1 };

static char* UdfName;

void
Init()
  {
  }

template <class MDVM, class ParamType>
static
inline
void
Execute( MDVM &aMDVM, const ParamType &P )
  {
  UDF_Binding::UDF_WaterSPCShake_Execute(aMDVM);
  }

};


class UDF_WaterSPCRattle
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 3 };
  enum { TakesParameter = 0 };
enum { Code = UDF_Binding::WaterSPCRattle_Code };
enum { UsesVelocity = 1 };
enum { ReportsVelocity = 1 };
enum { UsesPositions = 1 };

static char* UdfName;

void
Init()
  {
  }

template <class MDVM, class ParamType>
static
inline
void
Execute( MDVM &aMDVM, const ParamType &P )
  {
  UDF_Binding::UDF_WaterSPCRattle_Execute(aMDVM);
  }
};


class UDF_WaterSPCInit
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 3 };
  enum { TakesParameter = 0 };
enum { Code = UDF_Binding::WaterSPCInit_Code };
enum { UsesVelocity = 1 };
enum { ReportsVelocity = 1 };
enum { ReportsPositions = 1 };
enum { UsesPositions = 1 };

static char* UdfName;

void
Init()
  {
  }

template <class MDVM, class ParamType>
static
inline
void
Execute( MDVM &aMDVM, const ParamType &P )
  {
  UDF_Binding::UDF_WaterSPCInit_Execute(aMDVM);
  }
};


class UDF_TIP3PForce
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 3 };
  enum { TakesParameter = 0 };
enum { Code = UDF_Binding::TIP3PForce_Code };
enum { UsesPositions = 1 };
enum { CalculatesEnergy = 1 };
enum { ReportsEnergy = 1};
enum { ReportsForce = 1};

static char* UdfName;

void
Init()
  {
  }

template <class MDVM, class ParamType>
static
inline
void
Execute( MDVM &aMDVM, const ParamType &P )
  {
  UDF_Binding::UDF_TIP3PForce_Execute(aMDVM);
  }
};


class UDF_SPCForce
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 3 };
  enum { TakesParameter = 0 };
enum { Code = UDF_Binding::SPCForce_Code };
enum { UsesPositions = 1 };
enum { CalculatesEnergy = 1 };
enum { ReportsEnergy = 1};
enum { ReportsForce = 1};

static char* UdfName;

void
Init()
  {
  }

template <class MDVM, class ParamType>
static
inline
void
Execute( MDVM &aMDVM, const ParamType &P )
  {
  UDF_Binding::UDF_SPCForce_Execute(aMDVM);
  }
  };


class UDF_ShakeGroup2
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::ShakeGroup2_Code };
  enum { UsesVelocity = 1 };
  enum { ReportsVelocity = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template <class MDVM, class ParamType>
  static
  inline
  void
  Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_ShakeGroup2_Execute( aMDVM, P);
    }
  };


class UDF_ShakeGroup3
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 3 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::ShakeGroup3_Code };
  enum { UsesVelocity = 1 };
  enum { ReportsVelocity = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template <class MDVM, class ParamType>
  static
  inline
  void
  Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_ShakeGroup3_Execute( aMDVM, P);
    }
  };


class UDF_ShakeGroup4
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 4 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::ShakeGroup4_Code };
  enum { UsesVelocity = 1 };
  enum { ReportsVelocity = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template <class MDVM, class ParamType>
  static
  inline
  void
  Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_ShakeGroup4_Execute( aMDVM, P);
    }
  };


class UDF_RattleGroup2
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 2 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::RattleGroup2_Code };
  enum { UsesVelocity = 1 };
  enum { ReportsVelocity = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template <class MDVM, class ParamType>
  static
  inline
  void
  Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_RattleGroup2_Execute( aMDVM, P);
    }
  };


class UDF_RattleGroup3
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 3 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::RattleGroup3_Code };
  enum { UsesVelocity = 1 };
  enum { ReportsVelocity = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template <class MDVM, class ParamType>
  static
  inline
  void
  Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_RattleGroup3_Execute( aMDVM, P);
    }
  };


class UDF_RattleGroup4
    : public UDF_Base
  {
  public:
  enum { NumberOfSites = 4 };
  enum { TakesParameter = 1 };
  enum { Code = UDF_Binding::RattleGroup4_Code };
  enum { UsesVelocity = 1 };
  enum { ReportsVelocity = 1 };
  enum { UsesPositions = 1 };

  static char* UdfName;

  void
  Init()
    {
    }

  template <class MDVM, class ParamType>
  static
  inline
  void
  Execute( MDVM &aMDVM, const ParamType &P )
    {
    UDF_Binding::UDF_RattleGroup4_Execute( aMDVM, P);
    }
  };


class UDF_NoseHooverInit
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init()
    {}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_NoseHooverInit_Execute( aMDVM );
    return;
    }
  };


class UDF_NoseHooverInitThermostatPosVel
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init()
    {}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_NoseHooverInitThermostatPosVel_Execute( aMDVM );
    return;
    }
  };


class UDF_UpdateFragmentKEForNoseHoover
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_UpdateFragmentKEForNoseHoover_Execute( aMDVM );
    return;
    }
  };


class UDF_NoseHooverIntegration
    : public UDF_Base
  {
  public:
  static char* UdfName;

  void Init(){}

  template< class MDVM >
  static
  inline
  void
  Execute(MDVM &aMDVM)
    {
    UDF_Binding::UDF_NoseHooverIntegration_Execute( aMDVM );
    return;
    }
  };

#if !defined(LEAVE_SYMS_EXTERN)
char* UDF_KineticEnergy::UdfName = "UDF_KineticEnergy";
char* UDF_NoseHooverEnergy::UdfName = "UDF_NoseHooverEnergy";
char* UDF_UpdateVelocity::UdfName = "UDF_UpdateVelocity";
char* UDF_SwitchFunction::UdfName = "UDF_SwitchFunction";
char* UDF_SwitchFunctionForce::UdfName = "UDF_SwitchFunctionForce";
char* UDF_AtomToFragmentVirialCorrection::UdfName = "UDF_AtomToFragmentVirialCorrection";
char* UDF_UpdateVelocityFirstHalfStepNPT::UdfName = "UDF_UpdateVelocityFirstHalfStepNPT";
char* UDF_UpdateVelocitySecondHalfStepNPT::UdfName = "UDF_UpdateVelocitySecondHalfStepNPT";
char* UDF_UpdateCenterOfMass::UdfName = "UDF_UpdateCenterOfMass";
char* UDF_UpdatePosition::UdfName = "UDF_UpdatePosition";
char* UDF_UpdatePositionNPT::UdfName = "UDF_UpdatePositionNPT";
char* UDF_UpdateCenterOfMassKineticEnergy::UdfName = "UDF_UpdateCenterOfMassKineticEnergy";
char* UDF_ZeroForce::UdfName = "UDF_ZeroForce";
char* UDF_ZeroCenterOfMassForce::UdfName = "UDF_ZeroCenterOfMassForce";
char* UDF_UpdateCenterOfMassForce::UdfName = "UDF_UpdateCenterOfMassForce";
char* UDF_UpdateCenterOfMassPosition::UdfName = "UDF_UpdateCenterOfMassPosition";
char* UDF_UpdateCenterOfMassVelocity::UdfName = "UDF_UpdateCenterOfMassVelocity";

char* UDF_COMRestraint::UdfName = "UDF_COMRestraint";

char* UDF_StdHarmonicBondForce::UdfName = "UDF_StdHarmonicBondForce";
char* UDF_StdHarmonicAngleForce::UdfName = "UDF_StdHarmonicAngleForce";
char* UDF_UreyBradleyForce::UdfName = "UDF_UreyBradleyForce";
char* UDF_SwopeTorsionForce::UdfName = "UDF_SwopeTorsionForce";
char* UDF_ImproperDihedralForce::UdfName = "UDF_ImproperDihedralForce";
char* UDF_OPLSImproperForce::UdfName = "UDF_OPLSImproperForce";
char* UDF_OPLSTorsionForce::UdfName = "UDF_OPLSTorsionForce";
char* UDF_Coulomb14::UdfName = "UDF_Coulomb14";
char* UDF_NSQCoulomb::UdfName = "UDF_NSQCoulomb";
char* UDF_LennardJones14Force::UdfName = "UDF_LennardJones14Force";
char* UDF_NSQCHARMMLennardJonesForce::UdfName = "UDF_NSQCHARMMLennardJonesForce";
char* UDF_NSQAMBERLennardJonesForce::UdfName = "UDF_NSQAMBERLennardJonesForce";
char* UDF_NSQOPLSLennardJonesForce::UdfName = "UDF_NSQOPLSLennardJonesForce";
char* UDF_EwaldCorrection::UdfName = "UDF_EwaldCorrection";
char*  UDF_NSQEwaldCorrection::UdfName = "UDF_NSQEwaldCorrection";
char* UDF_NSQEwaldRealSpace::UdfName = "UDF_NSQEwaldRealSpace";
char* UDF_NSQEwaldRespaCorrection::UdfName = "UDF_NSQEwaldRespaCorrection";
char* UDF_LeknerCorrection::UdfName = "UDF_LeknerCorrection";
char* UDF_NSQLeknerCorrection::UdfName = "UDF_NSQLeknerCorrection";
char* UDF_NSQLekner::UdfName = "UDF_NSQLekner";
char*  UDF_WaterTIP3PShake::UdfName = "UDF_WaterTIP3PShake";
char* UDF_WaterTIP3PRattle::UdfName = "UDF_WaterTIP3PRattle";
char* UDF_WaterTIP3PInit::UdfName = "UDF_WaterTIP3PInit";
char* UDF_WaterSPCShake::UdfName = "UDF_WaterSPCShake";
char* UDF_WaterSPCRattle::UdfName = "UDF_WaterSPCRattle";
char* UDF_WaterSPCInit::UdfName = "UDF_WaterSPCInit";
char* UDF_TIP3PForce::UdfName = "UDF_TIP3PForce";
char* UDF_SPCForce::UdfName = "UDF_SPCForce";
char* UDF_ShakeGroup2::UdfName = "UDF_ShakeGroup2";
char* UDF_ShakeGroup3::UdfName = "UDF_ShakeGroup3";
char* UDF_ShakeGroup4::UdfName = "UDF_ShakeGroup4";
char* UDF_RattleGroup2::UdfName = "UDF_RattleGroup2";
char* UDF_RattleGroup3::UdfName = "UDF_RattleGroup3";
char* UDF_RattleGroup4::UdfName = "UDF_RattleGroup4";
char* UDF_NoseHooverInit::UdfName = "UDF_NoseHooverInit";
char* UDF_NoseHooverInitThermostatPosVel::UdfName = "UDF_NoseHooverInitThermostatPosVel";
char* UDF_UpdateFragmentKEForNoseHoover::UdfName = "UDF_UpdateFragmentKEForNoseHoover";
char* UDF_NoseHooverIntegration::UdfName = "UDF_NoseHooverIntegration";
#endif
#endif
