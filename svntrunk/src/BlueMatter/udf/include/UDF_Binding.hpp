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
 #ifndef UDF_BINDING_HPP
#define UDF_BINDING_HPP

//  DO NOT EDIT     DO NOT EDIT    DO NOT EDIT    DO NOT EDIT     DO NOT EDIT     DO NOT EDIT     DO NOT EDIT
//  DO NOT EDIT     DO NOT EDIT    DO NOT EDIT    DO NOT EDIT     DO NOT EDIT     DO NOT EDIT     DO NOT EDIT
//  DO NOT EDIT     DO NOT EDIT    DO NOT EDIT    DO NOT EDIT     DO NOT EDIT     DO NOT EDIT     DO NOT EDIT
//
//  This file was automatically created by the script UDFBind.pl, which constructs the UDF_Registry class
//  using the contents of the UDF registry and all its corresponding UDF .hpp files.
//
//  If changes are needed to this file, please communicate them to your local UDF control authority.
//

#include <cstring>
using namespace std ;

#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/math.hpp>
#include <BlueMatter/UDF_State.hpp>
#include <BlueMatter/UDF_HelperWater.hpp>
#include <BlueMatter/UDF_HelperLekner.hpp>
#include <BlueMatter/UDF_HelperGroup.hpp>

// extern "C" {
// unsigned SimTick;
// };

class UDF_Binding
{
    struct RegistryRecord
    {
        int    mCode;
        char   *mCategories;
    };

    enum { TOKENBUFFERSIZE = 1024 };

    static char mTokenBuffer[TOKENBUFFERSIZE];

    // Private method for internal use...
    // This could be a bsearch or something.  Order should be maintained in registry.
    static inline int GetIndexByUDFCode(const int code)
    {
        int i;
        for (i=0; i<NUMENTRIES; i++) {
            if (code == UDF_RegistryList[i].mCode)
                return i;
        }
        return CODENOTFOUND;
    }


public:

    enum {CODENOTFOUND = -1};
    enum {ARGNOTFOUND = -1};

    template <class UDFParamType> class NSQ2_Param
    {
    public:
        UDFParamType *A;
        UDFParamType *B;
    };

    template <class InputParamType, class OutputParamType> class Function_Param
    {
    public:
        InputParamType  *A;
        OutputParamType *B;
    };

    template <class InputParamType, class OutputParamType> class PairFunction_Param
    {
    public:
        InputParamType *A;
        InputParamType *B;
        OutputParamType *C;
    };

    template <class UDFParamType> class Transform_Param
    {
    public:
        UDFParamType *A;
        UDFParamType *B;
    };

    template <class UDFParamType> class PairTransform_Param
    {
    public:
        UDFParamType *A;
        UDFParamType *B;
        UDFParamType *C;
    };


    // Does the UDF Code contain the requested category?
    static inline void IsUDFCodeCategory(const int code, const char *cat, int &result)
    {
        int i = GetIndexByUDFCode(code);

        // assert on failure
        if (i == CODENOTFOUND) {
            result = 0;
	    // If we're building data only, we won't have functional logging
	    // so this would be a syntax error. However this should never
	    // be driven in the data only case ...
#if !defined(MSD_COMPILE_DATA_ONLY)
            BegLogLine(1)
                 << "IsUDFCodeCategory failed - code " << code << " not found"
                 << EndLogLine ;
#endif
            assert(0);
        }

        assert(strlen(UDF_RegistryList[i].mCategories) < TOKENBUFFERSIZE);
        strcpy(mTokenBuffer, UDF_RegistryList[i].mCategories);
        char *p = strtok(mTokenBuffer, ":");
        while (p && strcmp(p, cat))
            p = strtok(NULL, ":");

        result = (p != NULL);
    }


    enum { NUMENTRIES = 75 };

    enum { MAXSITECOUNT = 4 };

    static RegistryRecord UDF_RegistryList[];

    enum
    {
        DefaultForce_Code = 0,
        StdHarmonicBondForce_Code = 1,
        StdHarmonicAngleForce_Code = 2,
        SwopeTorsionForce_Code = 3,
        UreyBradleyForce_Code = 4,
        OPLSTorsionForce_Code = 5,
        OPLSImproperForce_Code = 6,
        ImproperDihedralForce_Code = 7,
        LennardJonesForce_Code = 8,
        StdChargeForce_Code = 9,
        TIP3PForce_Code = 10,
        KineticEnergy_Code = 11,
        WaterRigid3SiteShake_Code = 12,
        WaterRigid3SiteRattle_Code = 13,
        UpdateVolumeHalfStep_Code = 14,
        UpdateVolumePosition_Code = 15,
        UpdateVelocityFirstHalfStepNPT_Code = 16,
        UpdatePositionNPT_Code = 17,
        UpdateVelocitySecondHalfStepNPT_Code = 18,
        UpdateVirialForce_Code = 19,
        SPCForce_Code = 20,
        EwaldRealSpaceForce_Code = 21,
        EwaldStructureFactors_Code = 22,
        EwaldKSpaceForce_Code = 23,
        EwaldSelfEnergyForce_Code = 24,
        EwaldCorrectionForce_Code = 25,
        WaterSPCShake_Code = 26,
        WaterSPCRattle_Code = 27,
        WaterTIP3PShake_Code = 28,
        WaterTIP3PRattle_Code = 29,
        NSQLennardJones_Code = 30,
        NSQCoulomb_Code = 31,
        LJStdCombiner_Code = 32,
        SwitchStd5_Code = 33,
        NSQEwaldCorrection_Code = 34,
        NSQEwaldRealSpace_Code = 35,
        Coulomb14_Code = 36,
        WaterTIP3PInit_Code = 37,
        WaterSPCInit_Code = 38,
        LJOPLSCombiner_Code = 39,
        NSQCheckSitesInBox_Code = 40,
        COMKineticEnergy_Code = 41,
        ResampleEnergyLoss_Code = 42,
        UpdateVolumeSecondHalfStep_Code = 43,
        ZeroForce_Code = 44,
        UpdatePosition_Code = 45,
        UpdateVelocity_Code = 46,
        UpdateCenterOfMassKineticEnergy_Code = 47,
        UpdateCenterOfMass_Code = 48,
        LennardJones14Force_Code = 49,
        OPLSLennardJones14Force_Code = 50,
        NSQOPLSLennardJones_Code = 51,
        NSQLekner_Code = 52,
        NSQLeknerCorrection_Code = 53,
        LeknerSelfEnergy_Code = 54,
        ShakeGroup2_Code = 55,
        ShakeGroup3_Code = 56,
        ShakeGroup4_Code = 57,
        RattleGroup4_Code = 58,
        RattleGroup3_Code = 59,
        RattleGroup2_Code = 60,
        AtomToFragmentVirialCorrection_Code = 61,
        ZeroCenterOfMassForce_Code = 62,
        UpdateCenterOfMassPosition_Code = 63,
        UpdateCenterOfMassVelocity_Code = 64,
        UpdateCenterOfMassForce_Code = 65,
        NSQEwaldRespaCorrection_Code = 66,
        SwitchFunction_Code = 67,
        SwitchFunctionForce_Code = 68,
        NoseHooverInit_Code = 69,
        UpdateFragmentKEForNoseHoover_Code = 70,
        NoseHooverIntegration_Code = 71,
        NoseHooverEnergy_Code = 72,
        NoseHooverInitThermostatPosVel_Code = 73,
        COMRestraint_Code = 74
    };

    enum { UDF_CODE_COUNT = 75 };

    enum { UDF_MAX_CODE = 74 };

    struct DefaultForce_Params
    {
        int dummy;
    };

    struct StdHarmonicBondForce_Params
    {
        double k;
        double r0;
    };

    struct StdHarmonicAngleForce_Params
    {
        double k;
        double th0;
    };

    struct SwopeTorsionForce_Params
    {
        double cosDelta;
        double k;
        double sinDelta;
        int n;
    };

    struct UreyBradleyForce_Params
    {
        double k;
        double s0;
    };

    struct OPLSTorsionForce_Params
    {
        double cosDelta;
        double k;
        double sinDelta;
        int n;
    };

    struct OPLSImproperForce_Params
    {
        double cosDelta;
        double k;
        double sinDelta;
        int n;
    };

    struct ImproperDihedralForce_Params
    {
        double k;
        double psi0;
    };

    struct LennardJonesForce_Params
    {
        double epsilon;
        double sigma;
    };

    struct StdChargeForce_Params
    {
        double ChargeAxB;
    };

    struct EwaldRealSpaceForce_Params
    {
        double ChargeAxB;
        double alpha;
    };

    struct EwaldStructureFactors_Params
    {
        double charge;
        double kx;
        double ky;
        double kz;
    };

    struct EwaldCorrectionForce_Params
    {
        double ChargeAxB;
        double alpha;
    };

    struct NSQLennardJones_Params
    {
        double epsilon;
        double sigma;
    };

    struct NSQCoulomb_Params
    {
        double ChargeAxB;
    };

    struct LJStdCombiner_Params
    {
        double epsilon;
        double sigma;
    };

    struct SwitchStd5_Params
    {
        double r;
    };

    struct SwitchStd5_OutputParams
    {
        double S;
        double dSdR;
    };

    struct NSQEwaldCorrection_Params
    {
        double ChargeAxB;
    };

    struct NSQEwaldRealSpace_Params
    {
        double ChargeAxB;
    };

    struct Coulomb14_Params
    {
        double ChargeAxB;
    };

    struct LJOPLSCombiner_Params
    {
        double epsilon;
        double sigma;
    };

    struct LennardJones14Force_Params
    {
        double epsilon;
        double sigma;
    };

    struct OPLSLennardJones14Force_Params
    {
        double epsilon;
        double sigma;
    };

    struct NSQOPLSLennardJones_Params
    {
        double epsilon;
        double sigma;
    };

    struct NSQLekner_Params
    {
        double ChargeAxB;
    };

    struct NSQLeknerCorrection_Params
    {
        double ChargeAxB;
    };

    struct ShakeGroup2_Params
    {
        ConstraintGroupParams<2> cparams;
    };

    struct ShakeGroup3_Params
    {
        ConstraintGroupParams<3> cparams;
    };

    struct ShakeGroup4_Params
    {
        ConstraintGroupParams<4> cparams;
    };

    struct RattleGroup4_Params
    {
        ConstraintGroupParams<4> cparams;
    };

    struct RattleGroup3_Params
    {
        ConstraintGroupParams<3> cparams;
    };

    struct RattleGroup2_Params
    {
        ConstraintGroupParams<2> cparams;
    };

    struct NSQEwaldRespaCorrection_Params
    {
        double ChargeAxB;
    };

    struct COMRestraint_Params
    {
        double k;
        double z0;
    };

// Shouldn't be calling anything via the 'switch' nowadays
#if 0

    template <class MDVM> static inline void UDF_Execute(const int Code, MDVM &aMDVM, void *Params)
    {
        switch (Code)
        {
        case DefaultForce_Code:
            UDF_DefaultForce_Execute(aMDVM, *(DefaultForce_Params *)Params);
            break;
        case StdHarmonicBondForce_Code:
            UDF_StdHarmonicBondForce_Execute(aMDVM, *(StdHarmonicBondForce_Params *)Params);
            break;
        case StdHarmonicAngleForce_Code:
            UDF_StdHarmonicAngleForce_Execute(aMDVM, *(StdHarmonicAngleForce_Params *)Params);
            break;
        case SwopeTorsionForce_Code:
            UDF_SwopeTorsionForce_Execute(aMDVM, *(SwopeTorsionForce_Params *)Params);
            break;
        case UreyBradleyForce_Code:
            UDF_UreyBradleyForce_Execute(aMDVM, *(UreyBradleyForce_Params *)Params);
            break;
        case OPLSTorsionForce_Code:
            UDF_OPLSTorsionForce_Execute(aMDVM, *(OPLSTorsionForce_Params *)Params);
            break;
        case OPLSImproperForce_Code:
            UDF_OPLSImproperForce_Execute(aMDVM, *(OPLSImproperForce_Params *)Params);
            break;
        case ImproperDihedralForce_Code:
            UDF_ImproperDihedralForce_Execute(aMDVM, *(ImproperDihedralForce_Params *)Params);
            break;
        case LennardJonesForce_Code:
            UDF_LennardJonesForce_Execute(aMDVM, *(LennardJonesForce_Params *)Params);
            break;
        case StdChargeForce_Code:
            UDF_StdChargeForce_Execute(aMDVM, *(StdChargeForce_Params *)Params);
            break;
        case TIP3PForce_Code:
            UDF_TIP3PForce_Execute(aMDVM);
            break;
        case KineticEnergy_Code:
            UDF_KineticEnergy_Execute(aMDVM);
            break;
        case WaterRigid3SiteShake_Code:
            UDF_WaterRigid3SiteShake_Execute(aMDVM);
            break;
        case WaterRigid3SiteRattle_Code:
            UDF_WaterRigid3SiteRattle_Execute(aMDVM);
            break;
        case UpdateVolumeHalfStep_Code:
            UDF_UpdateVolumeHalfStep_Execute(aMDVM);
            break;
        case UpdateVolumePosition_Code:
            UDF_UpdateVolumePosition_Execute(aMDVM);
            break;
        case UpdateVelocityFirstHalfStepNPT_Code:
            UDF_UpdateVelocityFirstHalfStepNPT_Execute(aMDVM);
            break;
        case UpdatePositionNPT_Code:
            UDF_UpdatePositionNPT_Execute(aMDVM);
            break;
        case UpdateVelocitySecondHalfStepNPT_Code:
            UDF_UpdateVelocitySecondHalfStepNPT_Execute(aMDVM);
            break;
        case UpdateVirialForce_Code:
            UDF_UpdateVirialForce_Execute(aMDVM);
            break;
        case SPCForce_Code:
            UDF_SPCForce_Execute(aMDVM);
            break;
        case EwaldRealSpaceForce_Code:
            UDF_EwaldRealSpaceForce_Execute(aMDVM, *(EwaldRealSpaceForce_Params *)Params);
            break;
        case EwaldStructureFactors_Code:
            UDF_EwaldStructureFactors_Execute(aMDVM, *(EwaldStructureFactors_Params *)Params);
            break;
        case EwaldKSpaceForce_Code:
            UDF_EwaldKSpaceForce_Execute(aMDVM);
            break;
        case EwaldSelfEnergyForce_Code:
            UDF_EwaldSelfEnergyForce_Execute(aMDVM);
            break;
        case EwaldCorrectionForce_Code:
            UDF_EwaldCorrectionForce_Execute(aMDVM, *(EwaldCorrectionForce_Params *)Params);
            break;
        case WaterSPCShake_Code:
            UDF_WaterSPCShake_Execute(aMDVM);
            break;
        case WaterSPCRattle_Code:
            UDF_WaterSPCRattle_Execute(aMDVM);
            break;
        case WaterTIP3PShake_Code:
            UDF_WaterTIP3PShake_Execute(aMDVM);
            break;
        case WaterTIP3PRattle_Code:
            UDF_WaterTIP3PRattle_Execute(aMDVM);
            break;
        case NSQLennardJones_Code:
            UDF_NSQLennardJones_Execute(aMDVM, *(NSQLennardJones_Params *)Params);
            break;
        case NSQCoulomb_Code:
            UDF_NSQCoulomb_Execute(aMDVM, *(NSQCoulomb_Params *)Params);
            break;
        case LJStdCombiner_Code:
            UDF_LJStdCombiner_Execute(aMDVM, *(*(PairTransform_Param<LJStdCombiner_Params> *)Params).A, *(*(PairTransform_Param<LJStdCombiner_Params> *)Params).B, *(*(PairTransform_Param<LJStdCombiner_Params> *)Params).C);
            break;
        case SwitchStd5_Code:
            UDF_SwitchStd5_Execute(aMDVM, *(*(Function_Param<SwitchStd5_Params, SwitchStd5_OutputParams> *)Params).A, *(*(Function_Param<SwitchStd5_Params, SwitchStd5_OutputParams> *)Params).B);
            break;
        case NSQEwaldCorrection_Code:
            UDF_NSQEwaldCorrection_Execute(aMDVM, *(NSQEwaldCorrection_Params *)Params);
            break;
        case NSQEwaldRealSpace_Code:
            UDF_NSQEwaldRealSpace_Execute(aMDVM, *(NSQEwaldRealSpace_Params *)Params);
            break;
        case Coulomb14_Code:
            UDF_Coulomb14_Execute(aMDVM, *(Coulomb14_Params *)Params);
            break;
        case WaterTIP3PInit_Code:
            UDF_WaterTIP3PInit_Execute(aMDVM);
            break;
        case WaterSPCInit_Code:
            UDF_WaterSPCInit_Execute(aMDVM);
            break;
        case LJOPLSCombiner_Code:
            UDF_LJOPLSCombiner_Execute(aMDVM, *(*(PairTransform_Param<LJOPLSCombiner_Params> *)Params).A, *(*(PairTransform_Param<LJOPLSCombiner_Params> *)Params).B, *(*(PairTransform_Param<LJOPLSCombiner_Params> *)Params).C);
            break;
        case NSQCheckSitesInBox_Code:
            UDF_NSQCheckSitesInBox_Execute(aMDVM);
            break;
        case COMKineticEnergy_Code:
            UDF_COMKineticEnergy_Execute(aMDVM);
            break;
        case ResampleEnergyLoss_Code:
            UDF_ResampleEnergyLoss_Execute(aMDVM);
            break;
        case UpdateVolumeSecondHalfStep_Code:
            UDF_UpdateVolumeSecondHalfStep_Execute(aMDVM);
            break;
        case ZeroForce_Code:
            UDF_ZeroForce_Execute(aMDVM);
            break;
        case UpdatePosition_Code:
            UDF_UpdatePosition_Execute(aMDVM);
            break;
        case UpdateVelocity_Code:
            UDF_UpdateVelocity_Execute(aMDVM);
            break;
        case UpdateCenterOfMassKineticEnergy_Code:
            UDF_UpdateCenterOfMassKineticEnergy_Execute(aMDVM);
            break;
        case UpdateCenterOfMass_Code:
            UDF_UpdateCenterOfMass_Execute(aMDVM);
            break;
        case LennardJones14Force_Code:
            UDF_LennardJones14Force_Execute(aMDVM, *(LennardJones14Force_Params *)Params);
            break;
        case OPLSLennardJones14Force_Code:
            UDF_OPLSLennardJones14Force_Execute(aMDVM, *(OPLSLennardJones14Force_Params *)Params);
            break;
        case NSQOPLSLennardJones_Code:
            UDF_NSQOPLSLennardJones_Execute(aMDVM, *(NSQOPLSLennardJones_Params *)Params);
            break;
        case NSQLekner_Code:
            UDF_NSQLekner_Execute(aMDVM, *(NSQLekner_Params *)Params);
            break;
        case NSQLeknerCorrection_Code:
            UDF_NSQLeknerCorrection_Execute(aMDVM, *(NSQLeknerCorrection_Params *)Params);
            break;
        case LeknerSelfEnergy_Code:
            UDF_LeknerSelfEnergy_Execute(aMDVM);
            break;
        case ShakeGroup2_Code:
            UDF_ShakeGroup2_Execute(aMDVM, *(ShakeGroup2_Params *)Params);
            break;
        case ShakeGroup3_Code:
            UDF_ShakeGroup3_Execute(aMDVM, *(ShakeGroup3_Params *)Params);
            break;
        case ShakeGroup4_Code:
            UDF_ShakeGroup4_Execute(aMDVM, *(ShakeGroup4_Params *)Params);
            break;
        case RattleGroup4_Code:
            UDF_RattleGroup4_Execute(aMDVM, *(RattleGroup4_Params *)Params);
            break;
        case RattleGroup3_Code:
            UDF_RattleGroup3_Execute(aMDVM, *(RattleGroup3_Params *)Params);
            break;
        case RattleGroup2_Code:
            UDF_RattleGroup2_Execute(aMDVM, *(RattleGroup2_Params *)Params);
            break;
        case AtomToFragmentVirialCorrection_Code:
            UDF_AtomToFragmentVirialCorrection_Execute(aMDVM);
            break;
        case ZeroCenterOfMassForce_Code:
            UDF_ZeroCenterOfMassForce_Execute(aMDVM);
            break;
        case UpdateCenterOfMassPosition_Code:
            UDF_UpdateCenterOfMassPosition_Execute(aMDVM);
            break;
        case UpdateCenterOfMassVelocity_Code:
            UDF_UpdateCenterOfMassVelocity_Execute(aMDVM);
            break;
        case UpdateCenterOfMassForce_Code:
            UDF_UpdateCenterOfMassForce_Execute(aMDVM);
            break;
        case NSQEwaldRespaCorrection_Code:
            UDF_NSQEwaldRespaCorrection_Execute(aMDVM, *(NSQEwaldRespaCorrection_Params *)Params);
            break;
        case SwitchFunction_Code:
            UDF_SwitchFunction_Execute(aMDVM);
            break;
        case SwitchFunctionForce_Code:
            UDF_SwitchFunctionForce_Execute(aMDVM);
            break;
        case NoseHooverInit_Code:
            UDF_NoseHooverInit_Execute(aMDVM);
            break;
        case UpdateFragmentKEForNoseHoover_Code:
            UDF_UpdateFragmentKEForNoseHoover_Execute(aMDVM);
            break;
        case NoseHooverIntegration_Code:
            UDF_NoseHooverIntegration_Execute(aMDVM);
            break;
        case NoseHooverEnergy_Code:
            UDF_NoseHooverEnergy_Execute(aMDVM);
            break;
        case NoseHooverInitThermostatPosVel_Code:
            UDF_NoseHooverInitThermostatPosVel_Execute(aMDVM);
            break;
        case COMRestraint_Code:
            UDF_COMRestraint_Execute(aMDVM, *(COMRestraint_Params *)Params);
            break;
        default:
#if !defined(MSD_COMPILE_DATA_ONLY)
            BegLogLine(1)
                 << "UDF_Execute failed - Code " << Code << " not found"
                 << EndLogLine ;
#endif
            assert(0);
            break;
        }
    }
#endif


    static inline int GetParamSizeByUDFCode(const int Code)
    {
        switch (Code)
        {
        case DefaultForce_Code:
            return sizeof(DefaultForce_Params);
            break;
        case StdHarmonicBondForce_Code:
            return sizeof(StdHarmonicBondForce_Params);
            break;
        case StdHarmonicAngleForce_Code:
            return sizeof(StdHarmonicAngleForce_Params);
            break;
        case SwopeTorsionForce_Code:
            return sizeof(SwopeTorsionForce_Params);
            break;
        case UreyBradleyForce_Code:
            return sizeof(UreyBradleyForce_Params);
            break;
        case OPLSTorsionForce_Code:
            return sizeof(OPLSTorsionForce_Params);
            break;
        case OPLSImproperForce_Code:
            return sizeof(OPLSImproperForce_Params);
            break;
        case ImproperDihedralForce_Code:
            return sizeof(ImproperDihedralForce_Params);
            break;
        case LennardJonesForce_Code:
            return sizeof(LennardJonesForce_Params);
            break;
        case StdChargeForce_Code:
            return sizeof(StdChargeForce_Params);
            break;
        case TIP3PForce_Code:
            return 0;
            break;
        case KineticEnergy_Code:
            return 0;
            break;
        case WaterRigid3SiteShake_Code:
            return 0;
            break;
        case WaterRigid3SiteRattle_Code:
            return 0;
            break;
        case UpdateVolumeHalfStep_Code:
            return 0;
            break;
        case UpdateVolumePosition_Code:
            return 0;
            break;
        case UpdateVelocityFirstHalfStepNPT_Code:
            return 0;
            break;
        case UpdatePositionNPT_Code:
            return 0;
            break;
        case UpdateVelocitySecondHalfStepNPT_Code:
            return 0;
            break;
        case UpdateVirialForce_Code:
            return 0;
            break;
        case SPCForce_Code:
            return 0;
            break;
        case EwaldRealSpaceForce_Code:
            return sizeof(EwaldRealSpaceForce_Params);
            break;
        case EwaldStructureFactors_Code:
            return sizeof(EwaldStructureFactors_Params);
            break;
        case EwaldKSpaceForce_Code:
            return 0;
            break;
        case EwaldSelfEnergyForce_Code:
            return 0;
            break;
        case EwaldCorrectionForce_Code:
            return sizeof(EwaldCorrectionForce_Params);
            break;
        case WaterSPCShake_Code:
            return 0;
            break;
        case WaterSPCRattle_Code:
            return 0;
            break;
        case WaterTIP3PShake_Code:
            return 0;
            break;
        case WaterTIP3PRattle_Code:
            return 0;
            break;
        case NSQLennardJones_Code:
            return sizeof(NSQLennardJones_Params);
            break;
        case NSQCoulomb_Code:
            return sizeof(NSQCoulomb_Params);
            break;
        case LJStdCombiner_Code:
            return sizeof(LJStdCombiner_Params);
            break;
        case SwitchStd5_Code:
            return sizeof(SwitchStd5_Params);
            break;
        case NSQEwaldCorrection_Code:
            return sizeof(NSQEwaldCorrection_Params);
            break;
        case NSQEwaldRealSpace_Code:
            return sizeof(NSQEwaldRealSpace_Params);
            break;
        case Coulomb14_Code:
            return sizeof(Coulomb14_Params);
            break;
        case WaterTIP3PInit_Code:
            return 0;
            break;
        case WaterSPCInit_Code:
            return 0;
            break;
        case LJOPLSCombiner_Code:
            return sizeof(LJOPLSCombiner_Params);
            break;
        case NSQCheckSitesInBox_Code:
            return 0;
            break;
        case COMKineticEnergy_Code:
            return 0;
            break;
        case ResampleEnergyLoss_Code:
            return 0;
            break;
        case UpdateVolumeSecondHalfStep_Code:
            return 0;
            break;
        case ZeroForce_Code:
            return 0;
            break;
        case UpdatePosition_Code:
            return 0;
            break;
        case UpdateVelocity_Code:
            return 0;
            break;
        case UpdateCenterOfMassKineticEnergy_Code:
            return 0;
            break;
        case UpdateCenterOfMass_Code:
            return 0;
            break;
        case LennardJones14Force_Code:
            return sizeof(LennardJones14Force_Params);
            break;
        case OPLSLennardJones14Force_Code:
            return sizeof(OPLSLennardJones14Force_Params);
            break;
        case NSQOPLSLennardJones_Code:
            return sizeof(NSQOPLSLennardJones_Params);
            break;
        case NSQLekner_Code:
            return sizeof(NSQLekner_Params);
            break;
        case NSQLeknerCorrection_Code:
            return sizeof(NSQLeknerCorrection_Params);
            break;
        case LeknerSelfEnergy_Code:
            return 0;
            break;
        case ShakeGroup2_Code:
            return sizeof(ShakeGroup2_Params);
            break;
        case ShakeGroup3_Code:
            return sizeof(ShakeGroup3_Params);
            break;
        case ShakeGroup4_Code:
            return sizeof(ShakeGroup4_Params);
            break;
        case RattleGroup4_Code:
            return sizeof(RattleGroup4_Params);
            break;
        case RattleGroup3_Code:
            return sizeof(RattleGroup3_Params);
            break;
        case RattleGroup2_Code:
            return sizeof(RattleGroup2_Params);
            break;
        case AtomToFragmentVirialCorrection_Code:
            return 0;
            break;
        case ZeroCenterOfMassForce_Code:
            return 0;
            break;
        case UpdateCenterOfMassPosition_Code:
            return 0;
            break;
        case UpdateCenterOfMassVelocity_Code:
            return 0;
            break;
        case UpdateCenterOfMassForce_Code:
            return 0;
            break;
        case NSQEwaldRespaCorrection_Code:
            return sizeof(NSQEwaldRespaCorrection_Params);
            break;
        case SwitchFunction_Code:
            return 0;
            break;
        case SwitchFunctionForce_Code:
            return 0;
            break;
        case NoseHooverInit_Code:
            return 0;
            break;
        case UpdateFragmentKEForNoseHoover_Code:
            return 0;
            break;
        case NoseHooverIntegration_Code:
            return 0;
            break;
        case NoseHooverEnergy_Code:
            return 0;
            break;
        case NoseHooverInitThermostatPosVel_Code:
            return 0;
            break;
        case COMRestraint_Code:
            return sizeof(COMRestraint_Params);
            break;
        default:
#if !defined(MSD_COMPILE_DATA_ONLY)
            BegLogLine(1)
                 << "GetParamSizeByUDFCode failed - Code " << Code << " not found"
                 << EndLogLine ;
#endif
            assert(0);
            break;
        }
        return -1;
    }


    static inline int GetNumParamsByUDFCode(const int Code)
    {
        switch (Code)
        {
        case DefaultForce_Code:
            return 0;
            break;
        case StdHarmonicBondForce_Code:
            return 1;
            break;
        case StdHarmonicAngleForce_Code:
            return 1;
            break;
        case SwopeTorsionForce_Code:
            return 1;
            break;
        case UreyBradleyForce_Code:
            return 1;
            break;
        case OPLSTorsionForce_Code:
            return 1;
            break;
        case OPLSImproperForce_Code:
            return 1;
            break;
        case ImproperDihedralForce_Code:
            return 1;
            break;
        case LennardJonesForce_Code:
            return 1;
            break;
        case StdChargeForce_Code:
            return 1;
            break;
        case TIP3PForce_Code:
            return 1;
            break;
        case KineticEnergy_Code:
            return 1;
            break;
        case WaterRigid3SiteShake_Code:
            return 1;
            break;
        case WaterRigid3SiteRattle_Code:
            return 1;
            break;
        case UpdateVolumeHalfStep_Code:
            return 1;
            break;
        case UpdateVolumePosition_Code:
            return 1;
            break;
        case UpdateVelocityFirstHalfStepNPT_Code:
            return 1;
            break;
        case UpdatePositionNPT_Code:
            return 1;
            break;
        case UpdateVelocitySecondHalfStepNPT_Code:
            return 1;
            break;
        case UpdateVirialForce_Code:
            return 1;
            break;
        case SPCForce_Code:
            return 1;
            break;
        case EwaldRealSpaceForce_Code:
            return 1;
            break;
        case EwaldStructureFactors_Code:
            return 1;
            break;
        case EwaldKSpaceForce_Code:
            return 1;
            break;
        case EwaldSelfEnergyForce_Code:
            return 1;
            break;
        case EwaldCorrectionForce_Code:
            return 1;
            break;
        case WaterSPCShake_Code:
            return 1;
            break;
        case WaterSPCRattle_Code:
            return 1;
            break;
        case WaterTIP3PShake_Code:
            return 1;
            break;
        case WaterTIP3PRattle_Code:
            return 1;
            break;
        case NSQLennardJones_Code:
            return 1;
            break;
        case NSQCoulomb_Code:
            return 1;
            break;
        case LJStdCombiner_Code:
            return 1;
            break;
        case SwitchStd5_Code:
            return 1;
            break;
        case NSQEwaldCorrection_Code:
            return 1;
            break;
        case NSQEwaldRealSpace_Code:
            return 1;
            break;
        case Coulomb14_Code:
            return 1;
            break;
        case WaterTIP3PInit_Code:
            return 1;
            break;
        case WaterSPCInit_Code:
            return 1;
            break;
        case LJOPLSCombiner_Code:
            return 1;
            break;
        case NSQCheckSitesInBox_Code:
            return 1;
            break;
        case COMKineticEnergy_Code:
            return 1;
            break;
        case ResampleEnergyLoss_Code:
            return 1;
            break;
        case UpdateVolumeSecondHalfStep_Code:
            return 1;
            break;
        case ZeroForce_Code:
            return 1;
            break;
        case UpdatePosition_Code:
            return 1;
            break;
        case UpdateVelocity_Code:
            return 1;
            break;
        case UpdateCenterOfMassKineticEnergy_Code:
            return 1;
            break;
        case UpdateCenterOfMass_Code:
            return 1;
            break;
        case LennardJones14Force_Code:
            return 1;
            break;
        case OPLSLennardJones14Force_Code:
            return 1;
            break;
        case NSQOPLSLennardJones_Code:
            return 1;
            break;
        case NSQLekner_Code:
            return 1;
            break;
        case NSQLeknerCorrection_Code:
            return 1;
            break;
        case LeknerSelfEnergy_Code:
            return 0;
            break;
        case ShakeGroup2_Code:
            return 1;
            break;
        case ShakeGroup3_Code:
            return 1;
            break;
        case ShakeGroup4_Code:
            return 1;
            break;
        case RattleGroup4_Code:
            return 1;
            break;
        case RattleGroup3_Code:
            return 1;
            break;
        case RattleGroup2_Code:
            return 1;
            break;
        case AtomToFragmentVirialCorrection_Code:
            return 1;
            break;
        case ZeroCenterOfMassForce_Code:
            return 1;
            break;
        case UpdateCenterOfMassPosition_Code:
            return 1;
            break;
        case UpdateCenterOfMassVelocity_Code:
            return 1;
            break;
        case UpdateCenterOfMassForce_Code:
            return 1;
            break;
        case NSQEwaldRespaCorrection_Code:
            return 1;
            break;
        case SwitchFunction_Code:
            return 1;
            break;
        case SwitchFunctionForce_Code:
            return 1;
            break;
        case NoseHooverInit_Code:
            return 1;
            break;
        case UpdateFragmentKEForNoseHoover_Code:
            return 1;
            break;
        case NoseHooverIntegration_Code:
            return 1;
            break;
        case NoseHooverEnergy_Code:
            return 1;
            break;
        case NoseHooverInitThermostatPosVel_Code:
            return 1;
            break;
        case COMRestraint_Code:
            return 2;
            break;
        default:
#if !defined(MSD_COMPILE_DATA_ONLY)
            BegLogLine(1)
               << "GetNumParamsByUDFCode failed - Code " << Code << " not found"
               << EndLogLine ;
#endif
            assert(0);
            break;
        }
        return -1;
    }


    static inline int GetNumSitesByUDFCode(const int Code)
    {
        switch (Code)
        {
        case DefaultForce_Code:
            return 0;
            break;
        case StdHarmonicBondForce_Code:
            return 2;
            break;
        case StdHarmonicAngleForce_Code:
            return 3;
            break;
        case SwopeTorsionForce_Code:
            return 4;
            break;
        case UreyBradleyForce_Code:
            return 2;
            break;
        case OPLSTorsionForce_Code:
            return 4;
            break;
        case OPLSImproperForce_Code:
            return 4;
            break;
        case ImproperDihedralForce_Code:
            return 4;
            break;
        case LennardJonesForce_Code:
            return 2;
            break;
        case StdChargeForce_Code:
            return 2;
            break;
        case TIP3PForce_Code:
            return 3;
            break;
        case KineticEnergy_Code:
            return 0;
            break;
        case WaterRigid3SiteShake_Code:
            return 3;
            break;
        case WaterRigid3SiteRattle_Code:
            return 3;
            break;
        case UpdateVolumeHalfStep_Code:
            return 0;
            break;
        case UpdateVolumePosition_Code:
            return 0;
            break;
        case UpdateVelocityFirstHalfStepNPT_Code:
            return 1;
            break;
        case UpdatePositionNPT_Code:
            return -1;
            break;
        case UpdateVelocitySecondHalfStepNPT_Code:
            return -1;
            break;
        case UpdateVirialForce_Code:
            return 0;
            break;
        case SPCForce_Code:
            return 3;
            break;
        case EwaldRealSpaceForce_Code:
            return 2;
            break;
        case EwaldStructureFactors_Code:
            return 1;
            break;
        case EwaldKSpaceForce_Code:
            return 0;
            break;
        case EwaldSelfEnergyForce_Code:
            return 0;
            break;
        case EwaldCorrectionForce_Code:
            return 2;
            break;
        case WaterSPCShake_Code:
            return 3;
            break;
        case WaterSPCRattle_Code:
            return 3;
            break;
        case WaterTIP3PShake_Code:
            return 3;
            break;
        case WaterTIP3PRattle_Code:
            return 3;
            break;
        case NSQLennardJones_Code:
            return 2;
            break;
        case NSQCoulomb_Code:
            return 2;
            break;
        case LJStdCombiner_Code:
            return 0;
            break;
        case SwitchStd5_Code:
            return 0;
            break;
        case NSQEwaldCorrection_Code:
            return 2;
            break;
        case NSQEwaldRealSpace_Code:
            return 2;
            break;
        case Coulomb14_Code:
            return 2;
            break;
        case WaterTIP3PInit_Code:
            return 3;
            break;
        case WaterSPCInit_Code:
            return 3;
            break;
        case LJOPLSCombiner_Code:
            return 0;
            break;
        case NSQCheckSitesInBox_Code:
            return 2;
            break;
        case COMKineticEnergy_Code:
            return 0;
            break;
        case ResampleEnergyLoss_Code:
            return 0;
            break;
        case UpdateVolumeSecondHalfStep_Code:
            return 0;
            break;
        case ZeroForce_Code:
            return 0;
            break;
        case UpdatePosition_Code:
            return 0;
            break;
        case UpdateVelocity_Code:
            return 0;
            break;
        case UpdateCenterOfMassKineticEnergy_Code:
            return 0;
            break;
        case UpdateCenterOfMass_Code:
            return 0;
            break;
        case LennardJones14Force_Code:
            return 2;
            break;
        case OPLSLennardJones14Force_Code:
            return 2;
            break;
        case NSQOPLSLennardJones_Code:
            return 2;
            break;
        case NSQLekner_Code:
            return 2;
            break;
        case NSQLeknerCorrection_Code:
            return 2;
            break;
        case LeknerSelfEnergy_Code:
            return 0;
            break;
        case ShakeGroup2_Code:
            return 2;
            break;
        case ShakeGroup3_Code:
            return 2;
            break;
        case ShakeGroup4_Code:
            return 2;
            break;
        case RattleGroup4_Code:
            return 2;
            break;
        case RattleGroup3_Code:
            return 2;
            break;
        case RattleGroup2_Code:
            return 2;
            break;
        case AtomToFragmentVirialCorrection_Code:
            return 0;
            break;
        case ZeroCenterOfMassForce_Code:
            return 0;
            break;
        case UpdateCenterOfMassPosition_Code:
            return 0;
            break;
        case UpdateCenterOfMassVelocity_Code:
            return 0;
            break;
        case UpdateCenterOfMassForce_Code:
            return 0;
            break;
        case NSQEwaldRespaCorrection_Code:
            return 2;
            break;
        case SwitchFunction_Code:
            return 0;
            break;
        case SwitchFunctionForce_Code:
            return 0;
            break;
        case NoseHooverInit_Code:
            return 0;
            break;
        case UpdateFragmentKEForNoseHoover_Code:
            return 0;
            break;
        case NoseHooverIntegration_Code:
            return 0;
            break;
        case NoseHooverEnergy_Code:
            return 0;
            break;
        case NoseHooverInitThermostatPosVel_Code:
            return 0;
            break;
        case COMRestraint_Code:
            return 4;
            break;
        default:
#if !defined(MSD_COMPILE_DATA_ONLY)
            BegLogLine(1)
                 << "GetNumSitesByUDFCode failed - Code " << Code << " not found"
                 << EndLogLine ;
#endif
            assert(0);
            break;
        }
        return -1;
    }


template
<class MDVM, class ParamsType>
static
inline
void
UDF_DefaultForce_Execute(MDVM &aMDVM, ParamsType &Params)
{
	// Just a Params.dummy body as a placeholder
}


template
<class MDVM, class ParamsType>
static
inline
void
UDF_StdHarmonicBondForce_Execute(MDVM &aMDVM, ParamsType &Params)
{
#if !defined(OMIT_BOND_VECTOR_TABLE)
      double rAB = aMDVM.GetLength() ;
      double rABr = aMDVM.GetRecipLength() ;
      double DeltaFromEquilibrium  = rAB - Params.r0;

      aMDVM.ReportEnergy(  Params.k
                      * DeltaFromEquilibrium * DeltaFromEquilibrium );

      double dEdr  =
        -2.0 * ( Params.k * DeltaFromEquilibrium ) ;
      aMDVM.ReportdEdr(dEdr) ;
#else
      XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ) ;
      double rAB2 = dVecAB.LengthSquared() ;
      double rABr = 1.0 / sqrt(rAB2) ;
      double rAB = rAB2 * rABr ;
      double DeltaFromEquilibrium  = rAB - Params.r0;

      aMDVM.ReportEnergy(  Params.k
                      * DeltaFromEquilibrium * DeltaFromEquilibrium );

      double DEDR  =
        -2.0 * ( Params.k * DeltaFromEquilibrium ) * rABr;

      XYZ Force = dVecAB * DEDR;

      aMDVM.ReportForce( MDVM::SiteA, Force );
      aMDVM.ReportForce( MDVM::SiteB, Force * (-1) );
#endif      
}


template
<class MDVM, class ParamsType>
static
inline
void
UDF_StdHarmonicAngleForce_Execute(MDVM &aMDVM, ParamsType &Params)
      {
#if !defined(OMIT_BOND_VECTOR_TABLE)
      XYZ VectorBA = aMDVM.GetVectorN( 0 );
      XYZ VectorBC = aMDVM.GetVectorN( 1 );
      
      XYZ RP = VectorBC.crossProduct(VectorBA);
      double RPL2 = RP.LengthSquared() ;
      
      double recipBA =aMDVM.GetRecipLengthN(0) ; 
      double recipBC =aMDVM.GetRecipLengthN(1) ; 
      
      // Caution: if the vectors were collinear, RPL2 will be zero and RPL will be computed as NaN which will propagate
      double RPL = 1.0/sqrt(RPL2) ;
      // These lines get the distances from the MDVM.
      // double DistanceBA = aMDVM.GetDistance( MDVM::SiteB, MDVM::SiteA );
      // double DistanceBC = aMDVM.GetDistance( MDVM::SiteB, MDVM::SiteC );

      double COSTHE = ( VectorBA.DotProduct( VectorBC ) ) * (recipBA * recipBC);
      // double COSTHE = ( VectorBA.DotProduct( VectorBC ) ) / (DistanceBA * DistanceBC);

      // Caution: for colliner vectors, SINTHE will be computed as NaN
      double SINTHE = RPL2 * RPL * ( recipBA * recipBC) ;

      /////   DETERMINE THETA.  IT IS IN THE RANGE OF 0 TO PI RADIANS.

      // double THETA = acos(COSTHE);
      double THETA = Math::atrig(SINTHE, COSTHE);

      // Caution: for collinear vectors, AngleEnergy will be computed as NaN
      double AngleEnergy = Params.k * Math::sqr(THETA - Params.th0) ;


      double DEDTH = 2.0 * Params.k * (THETA - Params.th0);

      // COMPUTE AND NORMALIZE R(PERP) TO FACILITATE FORCE EVALUATION
      // R(PERP) = R32 CROSS R12


      // RPERP WILL NOT NORMALIZE IF 1-2-3 COLLINEAR; I.E., THETA=0 OR 180
      // FORCE CAN BE LEGITIMATELY SET TO ZERO IN THIS CASE.

      // suits 7/01 Using heuristic function here may have regression side-effect
      // double RPL = RP.Length();
      //     if (Heuristics::IsVectorLengthZero(RPL))
      //           RPL = 0.0;
      // else
      //           RPL = 1.0 / RPL;

      // if( RPL > 0.0000000001 )
      //  RPL = 1.0 / RPL;
      // else
      //  RPL=0.0;

      //    COMPUTE GRADIENTS OF ENERGY WITH RESPECT TO COMPONENTS OF VectorAB,RO2

      double    R12R = -1.0 *  (recipBA * recipBA);
      double    R32R =  1.0 *  (recipBC * recipBC);
      double    DTD12X = R12R*(RPL*DEDTH) * (VectorBA.mY*RP.mZ - VectorBA.mZ*RP.mY);
      double    DTD12Y = R12R*(RPL*DEDTH) * (VectorBA.mZ*RP.mX - VectorBA.mX*RP.mZ);
      double    DTD12Z = R12R*(RPL*DEDTH) * (VectorBA.mX*RP.mY - VectorBA.mY*RP.mX);
      double    DTD32X = R32R*(RPL*DEDTH) * (VectorBC.mY*RP.mZ - VectorBC.mZ*RP.mY);
      double    DTD32Y = R32R*(RPL*DEDTH) * (VectorBC.mZ*RP.mX - VectorBC.mX*RP.mZ);
      double    DTD32Z = R32R*(RPL*DEDTH) * (VectorBC.mX*RP.mY - VectorBC.mY*RP.mX);

      // COMPUTE FORCES DUE TO ANGLE ENERGY ON O, H1 AND H2

      XYZ ForceOnB;
      XYZ ForceOnA;
      XYZ ForceOnC;

      // If the 'angle' is 180 degrees, set the force to zero,but give an energy
      ForceOnA.mX = fsel(Heuristics::EpsilonVectorSquareLength-RPL2, 0.0, DTD12X        );
      ForceOnA.mY = fsel(Heuristics::EpsilonVectorSquareLength-RPL2, 0.0, DTD12Y        );
      ForceOnA.mZ = fsel(Heuristics::EpsilonVectorSquareLength-RPL2, 0.0, DTD12Z        );
      ForceOnB.mX = fsel(Heuristics::EpsilonVectorSquareLength-RPL2, 0.0,-DTD12X-DTD32X );
      ForceOnB.mY = fsel(Heuristics::EpsilonVectorSquareLength-RPL2, 0.0,-DTD12Y-DTD32Y );
      ForceOnB.mZ = fsel(Heuristics::EpsilonVectorSquareLength-RPL2, 0.0,-DTD12Z-DTD32Z );
      ForceOnC.mX = fsel(Heuristics::EpsilonVectorSquareLength-RPL2, 0.0,        DTD32X );
      ForceOnC.mY = fsel(Heuristics::EpsilonVectorSquareLength-RPL2, 0.0,        DTD32Y );
      ForceOnC.mZ = fsel(Heuristics::EpsilonVectorSquareLength-RPL2, 0.0,        DTD32Z );
      
      double FixedUpAngleEnergy = fsel(
         Heuristics::EpsilonVectorSquareLength-RPL2
        , Params.k * Math::sqr(M_PI - Params.th0 )
        , AngleEnergy
      ) ; 
      
        
      aMDVM.ReportForce( 0, ForceOnA );
      aMDVM.ReportForce( 1, ForceOnB );
      aMDVM.ReportForce( 2, ForceOnC );
      aMDVM.ReportEnergy( FixedUpAngleEnergy );
#else
      /////DETERMINE COS(THETA) = R12 DOT R32 / SQRT((R12**2)*(R32**2))

      XYZ VectorBA = aMDVM.GetVector( MDVM::SiteB, MDVM::SiteA );
      XYZ VectorBC = aMDVM.GetVector( MDVM::SiteB, MDVM::SiteC );

      // XYZ RP;
      // RP.mX = VectorAC.mY * VectorAB.mZ - VectorAC.mZ * VectorAB.mY;
      // RP.mY = VectorAC.mZ * VectorAB.mX - VectorAC.mX * VectorAB.mZ;
      // RP.mZ = VectorAC.mX * VectorAB.mY - VectorAC.mY * VectorAB.mX;

      XYZ RP = VectorBC.crossProduct(VectorBA);

      double RPL2 = RP.LengthSquared() ;

      double r2BA = VectorBA.LengthSquared() ;
      double r2BC = VectorBC.LengthSquared() ;

      double recipBA = 1.0/sqrt(r2BA) ;
      double recipBC = 1.0/sqrt(r2BC) ;
      // Caution: if the vectors were collinear, RPL2 will be zero and RPL will be computed as NaN which will propagate
      double RPL = 1.0/sqrt(RPL2) ;
      // These lines get the distances from the MDVM.
      // double DistanceBA = aMDVM.GetDistance( MDVM::SiteB, MDVM::SiteA );
      // double DistanceBC = aMDVM.GetDistance( MDVM::SiteB, MDVM::SiteC );

      double COSTHE = ( VectorBA.DotProduct( VectorBC ) ) * (recipBA * recipBC);
      // double COSTHE = ( VectorBA.DotProduct( VectorBC ) ) / (DistanceBA * DistanceBC);

      // Caution: for colliner vectors, SINTHE will be computed as NaN
      double SINTHE = RPL2 * RPL * ( recipBA * recipBC) ;

      /////   DETERMINE THETA.  IT IS IN THE RANGE OF 0 TO PI RADIANS.

      // double THETA = acos(COSTHE);
      double THETA = Math::atrig(SINTHE, COSTHE);

      // Caution: for collinear vectors, AngleEnergy will be computed as NaN
      double AngleEnergy = Params.k * Math::sqr(THETA - Params.th0) ;


      double DEDTH = 2.0 * Params.k * (THETA - Params.th0);

      // COMPUTE AND NORMALIZE R(PERP) TO FACILITATE FORCE EVALUATION
      // R(PERP) = R32 CROSS R12


      // RPERP WILL NOT NORMALIZE IF 1-2-3 COLLINEAR; I.E., THETA=0 OR 180
      // FORCE CAN BE LEGITIMATELY SET TO ZERO IN THIS CASE.

      // suits 7/01 Using heuristic function here may have regression side-effect
      // double RPL = RP.Length();
      //     if (Heuristics::IsVectorLengthZero(RPL))
      //           RPL = 0.0;
      // else
      //           RPL = 1.0 / RPL;

      // if( RPL > 0.0000000001 )
      //  RPL = 1.0 / RPL;
      // else
      //  RPL=0.0;

      //    COMPUTE GRADIENTS OF ENERGY WITH RESPECT TO COMPONENTS OF VectorAB,RO2

      double    R12R = -1.0 *  (recipBA * recipBA);
      double    R32R =  1.0 *  (recipBC * recipBC);
      double    DTD12X = R12R*(RPL*DEDTH) * (VectorBA.mY*RP.mZ - VectorBA.mZ*RP.mY);
      double    DTD12Y = R12R*(RPL*DEDTH) * (VectorBA.mZ*RP.mX - VectorBA.mX*RP.mZ);
      double    DTD12Z = R12R*(RPL*DEDTH) * (VectorBA.mX*RP.mY - VectorBA.mY*RP.mX);
      double    DTD32X = R32R*(RPL*DEDTH) * (VectorBC.mY*RP.mZ - VectorBC.mZ*RP.mY);
      double    DTD32Y = R32R*(RPL*DEDTH) * (VectorBC.mZ*RP.mX - VectorBC.mX*RP.mZ);
      double    DTD32Z = R32R*(RPL*DEDTH) * (VectorBC.mX*RP.mY - VectorBC.mY*RP.mX);

      // COMPUTE FORCES DUE TO ANGLE ENERGY ON O, H1 AND H2

      XYZ ForceOnB;
      XYZ ForceOnA;
      XYZ ForceOnC;


      ForceOnA.mX =  DTD12X        ;
      ForceOnA.mY =  DTD12Y        ;
      ForceOnA.mZ =  DTD12Z        ;
      ForceOnB.mX = -DTD12X-DTD32X ;
      ForceOnB.mY = -DTD12Y-DTD32Y ;
      ForceOnB.mZ = -DTD12Z-DTD32Z ;
      ForceOnC.mX =         DTD32X ;
      ForceOnC.mY =         DTD32Y ;
      ForceOnC.mZ =         DTD32Z ;

    if ( !  Heuristics::IsVectorSquareLengthZero(RPL2) )
      {
        // In the general case, the vectors are not collinear, and the AngleEnergy and forces are well-defined
        aMDVM.ReportEnergy( AngleEnergy );
        aMDVM.ReportForce( MDVM::SiteA, ForceOnA );
        aMDVM.ReportForce( MDVM::SiteB, ForceOnB );
        aMDVM.ReportForce( MDVM::SiteC, ForceOnC );
      }
      else
      {
        // In the special case, we computed AngleEnergy as NaN; perform the correct computation
        // (Assume the angle is 180 degrees, since 0 degrees is not physical)
        double CollinearAngleEnergy = Params.k * Math::sqr(M_PI - Params.th0 ) ;
        aMDVM.ReportEnergy( CollinearAngleEnergy );
      }

      return;
#endif      
      }

template
<class MDVM, class ParamsType>
static
inline
void
UDF_SwopeTorsionForce_Execute(MDVM &aMDVM, ParamsType &Params)
      {
#if !defined(OMIT_BOND_VECTOR_TABLE)
      XYZ dVec43 = aMDVM.GetVectorN( 2 ) ; 
      XYZ dVec23 = aMDVM.GetVectorN( 1 ) ; 
      double dVec23MagR = aMDVM.GetRecipLengthN ( 1 ) ;
      XYZ dVec12 = aMDVM.GetVectorN( 0 ) ; 
      
#else
      // For now, just create the symbols used by the regressed code
      // and connect them with what is in the MDVM.
      // IF these access don't compile out, change references in code below.
      XYZ aSitePosition1 = aMDVM.GetPosition( MDVM::SiteA );
      XYZ aSitePosition2 = aMDVM.GetPosition( MDVM::SiteB );
      XYZ aSitePosition3 = aMDVM.GetPosition( MDVM::SiteC );
      XYZ aSitePosition4 = aMDVM.GetPosition( MDVM::SiteD );


      XYZ dVec43 = aSitePosition4 - aSitePosition3,
        dVec23 = aSitePosition2 - aSitePosition3,
        dVec12 = aSitePosition1 - aSitePosition2;
#endif      
        
      XYZ dVecT = dVec43.crossProduct(dVec23),
        dVecU = dVec12.crossProduct(dVec23);
      double dVecTMag2 = sqr(dVecT),
        dVecUMag2 = sqr(dVecU);
      double dVec23Mag2 = sqr(dVec23) ;
      double dVecTMagR = 1.0 / sqrt(dVecTMag2) ;
      double dVecUMagR = 1.0 / sqrt(dVecUMag2) ;
#if defined(OMIT_BOND_VECTOR_TABLE)      
      double dVec23MagR = 1.0 / sqrt(dVec23Mag2) ;
#endif      

      // collinear situation
#if defined(UDF_ALLOW_BRANCHES) 
      // Handle 'collinearity' by skipping out
      if (Heuristics::IsVectorSquareLengthZero(dVecTMag2)
        || Heuristics::IsVectorSquareLengthZero(dVecUMag2))
        return;
#else
      // Handle 'collinearity' by making the forces come out zero, without a branch
      dVecTMagR = fsel(Heuristics::EpsilonVectorSquareLength-dVecTMag2,
                       0.0,
                       dVecTMagR
                      ) ;
      dVecUMagR = fsel(
                       Heuristics::EpsilonVectorSquareLength-dVecUMag2,
                       0.0,
                       dVecUMagR
                      ) ;       
#endif    

    //    if(Params.n == 6 || (is_almost_zero(Params.k)))
    //      {
    //        aForceOnSite1.Zero();
    //        aForceOnSite2.Zero();
    //        aForceOnSite3.Zero();
    //        aForceOnSite4.Zero();
    //        return;
    //      }

//       XYZ dVec23Unit = dVec23 /
// #if defined(TORSION_USE_MDVM)
//         aMDVM::GetDistance(MDVM::SiteB, MDVM::SiteC)
// #else
//         sqrt(sqr(dVec23))
// #endif
//         ,
//         dVecTUnit = dVecT/dVecTMag,
//         dVecUUnit = dVecU/dVecUMag;
//       XYZ gradientPhiOverdVecT =
//         1.0/sqr(dVecT)*(dVecT.crossProduct(dVec23Unit)),
//         gradientPhiOverdVecU =
//         -1.0/sqr(dVecU)*(dVecU.crossProduct(dVec23Unit));

       XYZ dVec23Unit = dVec23 * dVec23MagR ,
           dVecTUnit = dVecT*dVecTMagR,
           dVecUUnit = dVecU*dVecUMagR;
       XYZ gradientPhiOverdVecT =
         sqr(dVecTMagR)*(dVecT.crossProduct(dVec23Unit)),
         gradientPhiOverdVecU =
         -1.0*sqr(dVecUMagR)*(dVecU.crossProduct(dVec23Unit));

      XYZ gradientPhiOverAP4 =
        -gradientPhiOverdVecT.crossProduct(dVec23),

        gradientPhiOverAP3 =
        gradientPhiOverdVecT.crossProduct(dVec23 - dVec43) -
        gradientPhiOverdVecU.crossProduct(dVec12),

        gradientPhiOverAP2 =
        gradientPhiOverdVecT.crossProduct(dVec43) +
        gradientPhiOverdVecU.crossProduct(dVec23 + dVec12),

        gradientPhiOverAP1 = -(gradientPhiOverAP4 + gradientPhiOverAP3
                               + gradientPhiOverAP2);

        double cosPhi = dVecTUnit*dVecUUnit,
          sinPhi = -dVec23Unit*(dVecUUnit.crossProduct(dVecTUnit));
        double DEnergyOverDPhi;
        // split the following part into different functions. get rid of the switch block...
        double aTorsionEnergy = 0.0;
#if defined(UDF_ALLOW_BRANCHES)
        switch (Params.n)
          {
          case 1:
            aTorsionEnergy = Params.k*(1 + Params.cosDelta*cosPhi + Params.sinDelta*sinPhi);
            DEnergyOverDPhi = Params.k*(-Params.cosDelta*sinPhi + Params.sinDelta*cosPhi);
            break;
          case 2:
            aTorsionEnergy = Params.k*(1 + Params.cosDelta*(2.0*sqr(cosPhi)-1) + 2.0*Params.sinDelta*sinPhi*cosPhi);
            DEnergyOverDPhi = 2.0*Params.k*(-2.0*Params.cosDelta*cosPhi*sinPhi + Params.sinDelta*(2.0*sqr(cosPhi)-1));
            break;
          case 3:
            aTorsionEnergy = Params.k*(1 + Params.cosDelta*(4.0*cube(cosPhi) - 3.0*cosPhi) + Params.sinDelta*(4.0*sqr(cosPhi)-1)*sinPhi);
            DEnergyOverDPhi = 3.0*Params.k*(-Params.cosDelta*(4.0*sqr(cosPhi)-1)*sinPhi + 4.0*Params.sinDelta*cosPhi*(cosPhi-1));
            break;
          case 4:
            aTorsionEnergy = Params.k*(1 + Params.cosDelta*(8.0*sqr(sqr(cosPhi))-8.0*sqr(cosPhi)+1) + Params.sinDelta*4.0*(2.0*cube(cosPhi)-cosPhi)*sinPhi);
            DEnergyOverDPhi = 4.0*Params.k*(-Params.cosDelta*(8.0*cube(cosPhi)-4.0*cosPhi)*sinPhi - Params.sinDelta*(8.0*sqr(cosPhi)-8.0*sqr(sqr(cosPhi))-1));
            break;
          case 5:
             aTorsionEnergy = Params.k*(1 + Params.cosDelta*cosPhi*(5.0*sqr(sqr(sinPhi)) - 10.0*sqr(cosPhi*sinPhi) + sqr(sqr(cosPhi))) + Params.sinDelta*sinPhi*(5.0*sqr(sqr(cosPhi))- 10.0*sqr(cosPhi*sinPhi) + sqr(sqr(sinPhi))));
             DEnergyOverDPhi = 5.0*Params.k*( -Params.cosDelta*sinPhi*(sqr(sqr(sinPhi)) - 10.0*sqr(sinPhi*cosPhi) + 5.0*sqr(sqr(cosPhi))) + Params.sinDelta*cosPhi*(sqr(sqr(cosPhi))- 10.0*sqr(sinPhi*cosPhi) + 5.0*sqr(sqr(sinPhi))));
             break;

          case 6:
            aTorsionEnergy = Params.k*(1+Params.cosDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) -1) + Params.sinDelta*(32*sinPhi*sqr(cosPhi)*cube(cosPhi) - 32*cube(cosPhi)*sinPhi + 6*cosPhi*sinPhi));
            DEnergyOverDPhi = 6.0*Params.k*(Params.cosDelta*(-32*sqr(cosPhi)*cube(cosPhi)*sinPhi + 32*cube(cosPhi)*sinPhi - 6*cosPhi*sinPhi) + Params.sinDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) - 1));
            break;

          default:
            assert(0);
            break;
          }
#else        
        assert(Params.n <= 6 ) ; 
        double E[7] ;
        double dEdPhi[7] ;
        E[0] = 0.0 ; 
        E[1] = 1 + Params.cosDelta*cosPhi + Params.sinDelta*sinPhi ;
        E[2] = 1 + Params.cosDelta*(2.0*sqr(cosPhi)-1) + 2.0*Params.sinDelta*sinPhi*cosPhi ;
        E[3] = 1 + Params.cosDelta*(4.0*cube(cosPhi) - 3.0*cosPhi) + Params.sinDelta*(4.0*sqr(cosPhi)-1)*sinPhi ;
        E[4] = 1 + Params.cosDelta*(8.0*sqr(sqr(cosPhi))-8.0*sqr(cosPhi)+1) + Params.sinDelta*4.0*(2.0*cube(cosPhi)-cosPhi)*sinPhi;
        E[5] = 1 + Params.cosDelta*cosPhi*(5.0*sqr(sqr(sinPhi)) - 10.0*sqr(cosPhi*sinPhi) + sqr(sqr(cosPhi))) + Params.sinDelta*sinPhi*(5.0*sqr(sqr(cosPhi))- 10.0*sqr(cosPhi*sinPhi) + sqr(sqr(sinPhi)));
        E[6] = 1 + Params.cosDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) -1) + Params.sinDelta*(32*sinPhi*sqr(cosPhi)*cube(cosPhi) - 32*cube(cosPhi)*sinPhi + 6*cosPhi*sinPhi);
        
        dEdPhi[0] = 0.0 ;
        dEdPhi[1] = -Params.cosDelta*sinPhi + Params.sinDelta*cosPhi ;
        dEdPhi[2] = 2.0*(-2.0*Params.cosDelta*cosPhi*sinPhi + Params.sinDelta*(2.0*sqr(cosPhi)-1)) ;
        dEdPhi[3] = 3.0*(-Params.cosDelta*(4.0*sqr(cosPhi)-1)*sinPhi + 4.0*Params.sinDelta*cosPhi*(cosPhi-1));
        dEdPhi[4] = 4.0*(-Params.cosDelta*(8.0*cube(cosPhi)-4.0*cosPhi)*sinPhi - Params.sinDelta*(8.0*sqr(cosPhi)-8.0*sqr(sqr(cosPhi))-1));
        dEdPhi[5] = 5.0*( -Params.cosDelta*sinPhi*(sqr(sqr(sinPhi)) - 10.0*sqr(sinPhi*cosPhi) + 5.0*sqr(sqr(cosPhi))) + Params.sinDelta*cosPhi*(sqr(sqr(cosPhi))- 10.0*sqr(sinPhi*cosPhi) + 5.0*sqr(sqr(sinPhi))));
        dEdPhi[6] = 6.0*(Params.cosDelta*(-32*sqr(cosPhi)*cube(cosPhi)*sinPhi + 32*cube(cosPhi)*sinPhi - 6*cosPhi*sinPhi) + Params.sinDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) - 1));
        
        aTorsionEnergy=Params.k*E[Params.n] ;
        DEnergyOverDPhi=Params.k*dEdPhi[Params.n] ;
#endif          

      aMDVM.ReportEnergy( aTorsionEnergy );

      aMDVM.ReportForce( MDVM::SiteA ,  -DEnergyOverDPhi*gradientPhiOverAP1 ) ;
      aMDVM.ReportForce( MDVM::SiteB ,  -DEnergyOverDPhi*gradientPhiOverAP2 ) ;
      aMDVM.ReportForce( MDVM::SiteC ,  -DEnergyOverDPhi*gradientPhiOverAP3 ) ;
      aMDVM.ReportForce( MDVM::SiteD ,  -DEnergyOverDPhi*gradientPhiOverAP4 ) ;

      return;
      }

template
<class MDVM, class ParamsType>
static
inline
void
UDF_UreyBradleyForce_Execute(MDVM &aMDVM, ParamsType &Params)
      {
#if !defined(OMIT_BOND_VECTOR_TABLE)
      double DistanceAC = aMDVM.GetLength() ;
      double DistanceACR= aMDVM.GetRecipLength() ;
      double DeltaUB  = DistanceAC - Params.s0;

      double Energy = Params.k * Math::sqr(DeltaUB);

      aMDVM.ReportEnergy( Energy );

      double dEdr           = -2.0 * (Params.k * DeltaUB) ;
      aMDVM.ReportdEdr(dEdr) ;
#else      
      XYZ VectorAC = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB );
      double DistanceAC2 = VectorAC.LengthSquared() ;
      double DistanceACR = 1.0 / sqrt(DistanceAC2) ;
      double DistanceAC = DistanceAC2 * DistanceACR ;
      // These lines get the distances from the MDVM.
      // double DistanceAC = aMDVM.GetDistance( MDVM::SiteA, MDVM::SiteC );

      // These vectors will have been computed in the MDVM as a
      // byproduct of the above distances.

      double DeltaUB  = DistanceAC - Params.s0;

      double Energy = Params.k * Math::sqr(DeltaUB);

      aMDVM.ReportEnergy( Energy );

//      double DEDR           = -2.0 * (Params.k * DeltaUB) / DistanceAC;
      double DEDR           = -2.0 * (Params.k * DeltaUB) * DistanceACR;

      // XYZ ForceOnSiteB;
      XYZ ForceOnSiteA;
      XYZ ForceOnSiteC;

      // ForceOnSiteB.mX = 0.0;
      // ForceOnSiteB.mY = 0.0;
      // ForceOnSiteB.mZ = 0.0;
      ForceOnSiteA          =    VectorAC * DEDR;
      ForceOnSiteC          =   (VectorAC * DEDR) * (-1);

      aMDVM.ReportForce( MDVM::SiteA, ForceOnSiteA );
      // aMDVM.ReportForce( MDVM::SiteB, ForceOnSiteB );
      aMDVM.ReportForce( MDVM::SiteB, ForceOnSiteC );

      return;
#endif      
      }


template
<class MDVM, class ParamsType>
static
inline
void
UDF_OPLSTorsionForce_Execute(MDVM &aMDVM, ParamsType &Params)
  {
#if !defined(OMIT_BOND_VECTOR_TABLE)
      XYZ dVec43 = aMDVM.GetVectorN( 2 ) ; 
      XYZ dVec23 = aMDVM.GetVectorN( 1 ) ; 
      XYZ dVec12 = aMDVM.GetVectorN( 0 ) ; 
      
#else
  XYZ aSitePosition1 = aMDVM.GetPosition( MDVM::SiteA );
  XYZ aSitePosition2 = aMDVM.GetPosition( MDVM::SiteB );
  XYZ aSitePosition3 = aMDVM.GetPosition( MDVM::SiteC );
  XYZ aSitePosition4 = aMDVM.GetPosition( MDVM::SiteD );


  XYZ dVec43 = aSitePosition4 - aSitePosition3,
    dVec23 = aSitePosition2 - aSitePosition3,
    dVec12 = aSitePosition1 - aSitePosition2;
#endif
    
  XYZ dVecT = dVec43.crossProduct(dVec23),
    dVecU = dVec12.crossProduct(dVec23);
    
//  double dVecTMag = dVecT.Length(),
//    dVecUMag = dVecU.Length();
    double dVecTMag2 = dVecT.LengthSquared() ;
    double dVecUMag2 = dVecU.LengthSquared() ;
    double dVec23Mag2 = dVec23.LengthSquared() ;
    double dVecTMagR = 1.0 / sqrt(dVecTMag2) ;
    double dVecUMagR = 1.0 / sqrt(dVecUMag2) ;
    double dVec23MagR = 1.0 / sqrt(dVec23Mag2) ;

  // collinear situation
  // if (Heuristics::IsVectorLengthZero(dVecTMag) || Heuristics::IsVectorLengthZero(dVecUMag))
  //  return;
  if (Heuristics::IsVectorSquareLengthZero(dVecTMag2)
    || Heuristics::IsVectorSquareLengthZero(dVecUMag2))
    return;

// XYZ dVec23Unit = dVec23/sqrt(sqr(dVec23)),
//   dVecTUnit = dVecT/dVecTMag,
//   dVecUUnit = dVecU/dVecUMag;
// XYZ gradientPhiOverdVecT =
//   1.0/sqr(dVecT)*(dVecT.crossProduct(dVec23Unit)),
//   gradientPhiOverdVecU =
//   -1.0/sqr(dVecU)*(dVecU.crossProduct(dVec23Unit));

  XYZ dVec23Unit = dVec23*dVec23MagR,
    dVecTUnit = dVecT*dVecTMagR,
    dVecUUnit = dVecU*dVecUMagR;
  XYZ gradientPhiOverdVecT =
    dVecTMagR*dVecTMagR*(dVecT.crossProduct(dVec23Unit)),
    gradientPhiOverdVecU =
    -1.0*dVecUMagR*dVecUMagR*(dVecU.crossProduct(dVec23Unit));

  XYZ gradientPhiOverAP4 =
    -gradientPhiOverdVecT.crossProduct(dVec23),

    gradientPhiOverAP3 =
    gradientPhiOverdVecT.crossProduct(dVec23 - dVec43) -
    gradientPhiOverdVecU.crossProduct(dVec12),

    gradientPhiOverAP2 =
    gradientPhiOverdVecT.crossProduct(dVec43) +
    gradientPhiOverdVecU.crossProduct(dVec23 + dVec12),

    gradientPhiOverAP1 = -(gradientPhiOverAP4 + gradientPhiOverAP3
                           + gradientPhiOverAP2);

    double cosPhi = dVecTUnit*dVecUUnit,
      sinPhi = -dVec23Unit*(dVecUUnit.crossProduct(dVecTUnit));
    double DEnergyOverDPhi;
        double aTorsionEnergy = 0.0;
    // split the following part into different functions. get rid of the switch block...
    switch (Params.n)
      {
      case 0:
        aTorsionEnergy = Params.k;
        DEnergyOverDPhi = 0.0;
                break;
      case 1:
        aTorsionEnergy = (0.50*Params.k)*(1 + Params.cosDelta*cosPhi + Params.sinDelta*sinPhi);
        DEnergyOverDPhi = (0.50*Params.k)*(-Params.cosDelta*sinPhi + Params.sinDelta*cosPhi);
        break;
      case 2:
        aTorsionEnergy = (0.50*Params.k)*(1 + Params.cosDelta*(2.0*sqr(cosPhi)-1) + 2.0*Params.sinDelta*sinPhi*cosPhi);
        DEnergyOverDPhi = 2.0*(0.50*Params.k)*(-2.0*Params.cosDelta*cosPhi*sinPhi + Params.sinDelta*(2.0*sqr(cosPhi)-1));
        break;
      case 3:
        aTorsionEnergy = (0.50*Params.k)*(1 + Params.cosDelta*(4.0*cube(cosPhi) - 3.0*cosPhi) + Params.sinDelta*(4.0*sqr(cosPhi)-1)*sinPhi);
        DEnergyOverDPhi = 3.0*(0.50*Params.k)*(-Params.cosDelta*(4.0*sqr(cosPhi)-1)*sinPhi + 4.0*Params.sinDelta*cosPhi*(cosPhi-1));
        break;
      case 4:
        aTorsionEnergy = (0.50*Params.k)*(1 + Params.cosDelta*(8.0*sqr(sqr(cosPhi))-8.0*sqr(cosPhi)+1) + Params.sinDelta*4.0*(2.0*cube(cosPhi)-cosPhi)*sinPhi);
        DEnergyOverDPhi = 4.0*(0.50*Params.k)*(-Params.cosDelta*(8.0*cube(cosPhi)-4.0*cosPhi)*sinPhi - Params.sinDelta*(8.0*sqr(cosPhi)-8.0*sqr(sqr(cosPhi))-1));
        break;
      case 6:
        aTorsionEnergy = (0.50*Params.k)*(1+Params.cosDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) -1) + Params.sinDelta*(32*sinPhi*sqr(cosPhi)*cube(cosPhi) - 32*cube(cosPhi)*sinPhi + 6*cosPhi*sinPhi));
        DEnergyOverDPhi = 6.0*(0.50*Params.k)*(Params.cosDelta*(-32*sqr(cosPhi)*cube(cosPhi)*sinPhi + 32*cube(cosPhi)*sinPhi - 6*cosPhi*sinPhi) + Params.sinDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) - 1));
        break;

      default:
        assert(0);
        break;
      }

      aMDVM.ReportEnergy( aTorsionEnergy );

      aMDVM.ReportForce( MDVM::SiteA ,  -DEnergyOverDPhi*gradientPhiOverAP1 ) ;
      aMDVM.ReportForce( MDVM::SiteB ,  -DEnergyOverDPhi*gradientPhiOverAP2 ) ;
      aMDVM.ReportForce( MDVM::SiteC ,  -DEnergyOverDPhi*gradientPhiOverAP3 ) ;
      aMDVM.ReportForce( MDVM::SiteD ,  -DEnergyOverDPhi*gradientPhiOverAP4 ) ;
}

template
<class MDVM, class ParamsType>
static
inline
void
UDF_OPLSImproperForce_Execute(MDVM &aMDVM, ParamsType &Params)
  {
#if !defined(OMIT_BOND_VECTOR_TABLE)
      XYZ dVec43 = aMDVM.GetVectorN( 2 ) ; 
      XYZ dVec23 = aMDVM.GetVectorN( 1 ) ; 
      XYZ dVec12 = aMDVM.GetVectorN( 0 ) ; 
          
#else
  
  XYZ dVec43 = aMDVM.GetVector( MDVM::SiteD , MDVM::SiteC ),
          dVec23 = aMDVM.GetVector( MDVM::SiteB , MDVM::SiteC ),
          dVec12 = aMDVM.GetVector( MDVM::SiteA , MDVM::SiteB );
#endif
  XYZ dVecT = dVec43.crossProduct(dVec23),
          dVecU = dVec12.crossProduct(dVec23);

  double dVecTMag2 = dVecT.LengthSquared(),
                 dVecUMag2 = dVecU.LengthSquared();
  double dVec23Mag2 = dVec23.LengthSquared() ;

  double dVecTMagR = 1.0 / sqrt(dVecTMag2) ;
  double dVecUMagR = 1.0 / sqrt(dVecUMag2) ;
  double dVec23MagR = 1.0 / sqrt(dVec23Mag2) ;

// XYZ dVec23Unit = dVec23 / aMDVM.GetDistance(MDVM::SiteB, MDVM::SiteC),
//     dVecTUnit  = dVecT  / dVecTMag,
//     dVecUUnit  = dVecU  / dVecUMag;
//
// XYZ gradientPhiOverdVecT =
//   1.0/sqr(dVecT)*(dVecT.crossProduct(dVec23Unit)),
//   gradientPhiOverdVecU =
//   -1.0/sqr(dVecU)*(dVecU.crossProduct(dVec23Unit));

  XYZ dVec23Unit = dVec23 * dVec23MagR,
      dVecTUnit  = dVecT  * dVecTMagR,
      dVecUUnit  = dVecU  * dVecUMagR;

// XYZ gradientPhiOverdVecT =
//   1.0/sqr(dVecT)*(dVecT.crossProduct(dVec23Unit)),
//   gradientPhiOverdVecU =
//   -1.0/sqr(dVecU)*(dVecU.crossProduct(dVec23Unit));

  XYZ gradientPhiOverdVecT =
    1.0*sqr(dVecTMagR)*(dVecT.crossProduct(dVec23Unit)),
    gradientPhiOverdVecU =
    -1.0*sqr(dVecUMagR)*(dVecU.crossProduct(dVec23Unit));

  XYZ gradientPhiOverAP4 =
    -gradientPhiOverdVecT.crossProduct(dVec23),

    gradientPhiOverAP3 =
    gradientPhiOverdVecT.crossProduct(dVec23 - dVec43) -
    gradientPhiOverdVecU.crossProduct(dVec12),

    gradientPhiOverAP2 =
    gradientPhiOverdVecT.crossProduct(dVec43) +
    gradientPhiOverdVecU.crossProduct(dVec23 + dVec12),

    gradientPhiOverAP1 = -(gradientPhiOverAP4 + gradientPhiOverAP3
                           + gradientPhiOverAP2);

    double cosPhi = dVecTUnit*dVecUUnit,
      sinPhi = -dVec23Unit*(dVecUUnit.crossProduct(dVecTUnit));
    double DEnergyOverDPhi;
        double aTorsionEnergy = 0.0;
    // split the following part into different functions. get rid of the switch block...
    switch (Params.n)
      {
      case 0:
        aTorsionEnergy = Params.k;
        DEnergyOverDPhi = 0.0;
                break;
      case 1:
        aTorsionEnergy = (0.50*Params.k)*(1 + Params.cosDelta*cosPhi + Params.sinDelta*sinPhi);
        DEnergyOverDPhi = (0.50*Params.k)*(-Params.cosDelta*sinPhi + Params.sinDelta*cosPhi);
        break;
      case 2:
        aTorsionEnergy = (0.50*Params.k)*(1 + Params.cosDelta*(2.0*sqr(cosPhi)-1) + 2.0*Params.sinDelta*sinPhi*cosPhi);
        DEnergyOverDPhi = 2.0*(0.50*Params.k)*(-2.0*Params.cosDelta*cosPhi*sinPhi + Params.sinDelta*(2.0*sqr(cosPhi)-1));
        break;
      case 3:
        aTorsionEnergy = (0.50*Params.k)*(1 + Params.cosDelta*(4.0*cube(cosPhi) - 3.0*cosPhi) + Params.sinDelta*(4.0*sqr(cosPhi)-1)*sinPhi);
        DEnergyOverDPhi = 3.0*(0.50*Params.k)*(-Params.cosDelta*(4.0*sqr(cosPhi)-1)*sinPhi + 4.0*Params.sinDelta*cosPhi*(cosPhi-1));
        break;
      case 4:
        aTorsionEnergy = (0.50*Params.k)*(1 + Params.cosDelta*(8.0*sqr(sqr(cosPhi))-8.0*sqr(cosPhi)+1) + Params.sinDelta*4.0*(2.0*cube(cosPhi)-cosPhi)*sinPhi);
        DEnergyOverDPhi = 4.0*(0.50*Params.k)*(-Params.cosDelta*(8.0*cube(cosPhi)-4.0*cosPhi)*sinPhi - Params.sinDelta*(8.0*sqr(cosPhi)-8.0*sqr(sqr(cosPhi))-1));
        break;
      case 6:
        aTorsionEnergy = (0.50*Params.k)*(1+Params.cosDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) -1) + Params.sinDelta*(32*sinPhi*sqr(cosPhi)*cube(cosPhi) - 32*cube(cosPhi)*sinPhi + 6*cosPhi*sinPhi));
        DEnergyOverDPhi = 6.0*(0.50*Params.k)*(Params.cosDelta*(-32*sqr(cosPhi)*cube(cosPhi)*sinPhi + 32*cube(cosPhi)*sinPhi - 6*cosPhi*sinPhi) + Params.sinDelta*(32*sqr(cube(cosPhi)) - 48*sqr(sqr(cosPhi)) + 18*sqr(cosPhi) - 1));
        break;

      default:
        assert(0);
        break;
      }

  // collinear situation
  if (Heuristics::IsVectorSquareLengthZero(dVecTMag2) || Heuristics::IsVectorSquareLengthZero(dVecUMag2))
  {
    return;
  }

      aMDVM.ReportEnergy( aTorsionEnergy );

      aMDVM.ReportForce( MDVM::SiteA ,  -DEnergyOverDPhi*gradientPhiOverAP1 ) ;
      aMDVM.ReportForce( MDVM::SiteB ,  -DEnergyOverDPhi*gradientPhiOverAP2 ) ;
      aMDVM.ReportForce( MDVM::SiteC ,  -DEnergyOverDPhi*gradientPhiOverAP3 ) ;
      aMDVM.ReportForce( MDVM::SiteD ,  -DEnergyOverDPhi*gradientPhiOverAP4 ) ;
}

template
<class MDVM, class ParamsType>
static
inline
void
UDF_ImproperDihedralForce_Execute(MDVM &aMDVM, ParamsType &Params)
{
      // the improper dihedral angle is defined to be between the plane
      // 1-2-3 and 3-2-4

	// This code is not expressed elegantly. Cautions are ...
	// 1) Site C is the common atom in the 3 bonds.
	// 2) Site B is the 'fuselage' of the dihedral, sites A and D are the 'wings'
	// 3) dVecAB and dVecDB are not bond vectors
	// 4) 'GetVector (A, B)' returns the vector from B to A
	// It could be expressed more clearly, and with the same computational result, if it was in terms of bond vectors.
#if !defined(OMIT_BOND_VECTOR_TABLE)
          XYZ dVecAB = aMDVM.GetVectorN( 0 ) ; 
          XYZ dVecCB = aMDVM.GetVectorN( 1 ) ; 
          XYZ dVecDB = aMDVM.GetVectorN( 2 ) ; 
          double dVecCBMag2 = dVecCB.LengthSquared() ;
          double dVecCBMagR = aMDVM.GetRecipLengthN( 1 ) ;
          
#else
          XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ),
                  dVecCB = aMDVM.GetVector( MDVM::SiteC, MDVM::SiteB ),
                  dVecDB = aMDVM.GetVector( MDVM::SiteD, MDVM::SiteB );
          double dVecCBMag2 = dVecCB.LengthSquared() ;
          double dVecCBMagR = 1.0 / sqrt(dVecCBMag2) ;

#endif
          XYZ dVecT = dVecAB.crossProduct( dVecCB );
          XYZ dVecU = dVecCB.crossProduct( dVecDB );

          double dVecTMag2 = dVecT.LengthSquared();
          double dVecUMag2 = dVecU.LengthSquared();

          double dVecTMagR = 1.0 / sqrt(dVecTMag2) ;
          double dVecUMagR = 1.0 / sqrt(dVecUMag2) ;

      // collinear situation
#if defined(UDF_ALLOW_BRANCHES) 
      // Handle 'collinearity' by skipping out
      if (Heuristics::IsVectorSquareLengthZero(dVecTMag2)
        || Heuristics::IsVectorSquareLengthZero(dVecUMag2))
        return;
#else
      // Handle 'collinearity' by making the forces come out zero, without a branch
      dVecTMagR = fsel(Heuristics::EpsilonVectorSquareLength-dVecTMag2,
                       0.0,
                       dVecTMagR
                      ) ;
      dVecUMagR = fsel(
                       Heuristics::EpsilonVectorSquareLength-dVecUMag2,
                       0.0,
                       dVecUMagR
                      ) ;       
#endif    

      XYZ dVecCBUnit = dVecCB * dVecCBMagR,
        dVecTUnit = dVecT * dVecTMagR,
        dVecUUnit = dVecU * dVecUMagR;

      double cosPsi = dVecTUnit * dVecUUnit ;
      double sinPsi = dVecCBUnit*(dVecTUnit.crossProduct(dVecUUnit)) ;

      // This is a 4-quadrant angle finder, so it works even for dihedrals which are more than 90 degrees curled over;
      // but you have to be careful about the sign of the 'cos'
//    double psi = Math::atrig(sinPsi, cosPsi) ;
      double psi = Math::atrig(sinPsi, -cosPsi) ; 
//        psi = asin(sinPsi);


//    XYZ gradientPsiOverdVecT =
//      -1.0/sqr(dVecT)*(dVecT.crossProduct(dVecCBUnit)),
//      gradientPsiOverdVecU =
//      1.0/sqr(dVecU)*(dVecU.crossProduct(dVecCBUnit));

      XYZ gradientPsiOverdVecT =
        -1.0*dVecTMagR*dVecTMagR*(dVecT.crossProduct(dVecCBUnit)),
        gradientPsiOverdVecU =
        1.0*dVecUMagR*dVecUMagR*(dVecU.crossProduct(dVecCBUnit));

      XYZ gradientPsiOverAP2 =
        gradientPsiOverdVecT.crossProduct(dVecCB - dVecAB) +
        gradientPsiOverdVecU.crossProduct(dVecDB - dVecCB),

        gradientPsiOverAP1 =
        -gradientPsiOverdVecT.crossProduct(dVecCB),

        gradientPsiOverAP3 =
        gradientPsiOverdVecT.crossProduct(dVecAB) -
        gradientPsiOverdVecU.crossProduct(dVecDB),

        gradientPsiOverAP4 =
        gradientPsiOverdVecU.crossProduct(dVecCB);


// If collinear, and not branching, then set as if in equilibrium
      double AngleFromEquilibrium = 
#if defined(UDF_ALLOW_BRANCHES)      
      psi - Params.psi0 
#else
      fsel(Heuristics::EpsilonVectorSquareLength-dVecTMag2,
           0.0,
           psi - Params.psi0 
      ) 
#endif      
      ;
      
      aMDVM.ReportEnergy( Params.k*sqr(AngleFromEquilibrium) );

      double DenergyOverDpsi = 2.0*Params.k*AngleFromEquilibrium;

      aMDVM.ReportForce( MDVM::SiteA , -DenergyOverDpsi*gradientPsiOverAP1 );
      aMDVM.ReportForce( MDVM::SiteB , -DenergyOverDpsi*gradientPsiOverAP2 );
      aMDVM.ReportForce( MDVM::SiteC , -DenergyOverDpsi*gradientPsiOverAP3 );
      aMDVM.ReportForce( MDVM::SiteD , -DenergyOverDpsi*gradientPsiOverAP4 );

      BegLogLine(0)
        << "UDF::ImproperDihedralForce()"
        << " SiteA=" << aMDVM.GetPosition( MDVM::SiteA )
        << " SiteB=" << aMDVM.GetPosition( MDVM::SiteB )
        << " SiteC=" << aMDVM.GetPosition( MDVM::SiteC )
        << " SiteD=" << aMDVM.GetPosition( MDVM::SiteD )
        << EndLogLine;

      BegLogLine(0)
        << " dVecAB=" << dVecAB
        << " dVecCB=" << dVecCB
        << " dVecDB=" << dVecDB
        << EndLogLine;

      BegLogLine(0)
        << " dVecT=" << dVecT
        << " dVecU=" << dVecU
        << EndLogLine;

      BegLogLine(0)
        << " sinPsi=" << sinPsi
        << " cosPsi=" << cosPsi
        << " psi=" << psi
        << " Params.psi0=" << Params.psi0
        << EndLogLine;
      return;
}


template
<class MDVM, class ParamsType>
static
inline
void
UDF_LennardJonesForce_Execute(MDVM &aMDVM, ParamsType &Params)
  {
  PLATFORM_ABORT("LennardJonesForce udf is deprecated.  Use correct replacement.");
  XYZ    dVecAB        = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB) ;
  double dVecABMag2    = dVecAB.LengthSquared() ;

  double dVecABMagR    = 1.0/sqrt(dVecABMag2) ;
  double dVecABMag2R   = dVecABMagR*dVecABMagR ;
  double dVecABMag    = dVecABMag2 * dVecABMagR ;

//  double dVecABMag2R   = 1.0/dVecABMag2 ;
//  double dVecABMagR    = sqrt(dVecABMag2R) ;
//  double dVecABMag     = sqrt(dVecABMag2) ;

  double tmp2          = Params.sigma*Params.sigma*dVecABMag2R ;
  double tmp6          = tmp2*tmp2*tmp2 ;
  double tmp12         = tmp6 * tmp6;

///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  UDF_Binding::SwitchStd5_Params sp;
///////////////  UDF_Binding::SwitchStd5_OutputParams op;
///////////////  sp.r = sqrt(dVecABMag2);
//  UDF_Binding::Function_Param<UDF_Binding::SwitchStd5_Params, UDF_Binding::SwitchStd5_OutputParams> pin;
//  pin.A = &sp;
//  pin.B = &op;
//  UDF_Binding::UDF_Execute(UDF_Binding::SwitchStd5_Code, aMDVM, (void *)(&pin));
///////////////  UDF_Binding::UDF_SwitchStd5_Execute(aMDVM, sp, op);
///////////////  double S = op.S ;
///////////////  double dSdR = op.dSdR ;
///////////////#else
  double S ;
  double pdSdR ;
  SwitchFunctionRadii sfr;
  aMDVM.GetSwitchFunctionRadii(sfr);
//////////  SwitchFunction sf(RTG.mSwitchFunctionRadii) ;
  SwitchFunction sf(sfr) ;
  sf.Evaluate(dVecABMag,S,pdSdR) ;
///////////////#endif


  double lje           = Params.epsilon * (tmp12 - 2.0 * tmp6) ;



  aMDVM.ReportEnergy( lje * S );

  double DEDR =
                (
                  (12 * Params.epsilon) *
                  (
                      (  tmp12)
                    - (  tmp6)
                  )
                ) * dVecABMagR ;



///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  XYZ    fab     = dVecAB * dVecABMagR * ( S * DEDR + dSdR * lje );
///////////////#else
  XYZ    fab     = dVecAB * dVecABMagR * ( S * DEDR - pdSdR * lje );
///////////////#endif

  aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
  aMDVM.ReportForce( MDVM::SiteB , - fab ) ;

  return;
  }

template
<class MDVM, class ParamsType>
static
inline
void
UDF_StdChargeForce_Execute(MDVM &aMDVM, ParamsType &Params)
  {
  PLATFORM_ABORT("StdChargeForce udf is deprecated.  Use correct replacement.");
#if 0
  // NOTE: This method assumes that distance based cutoffs are handled outside.

  XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ) ;
  double dVecABMag2 = dVecAB.LengthSquared() ;
  double dVecABMagR = 1.0 / sqrt(dVecABMag2) ;
  // double DistanceAB = aMDVM.GetDistance( MDVM::SiteA, MDVM::SiteB );

  // double ca         = Params.ChargeAxB / DistanceAB;
  double ca         = Params.ChargeAxB * dVecABMagR ;

  // double DEDR       = Params.ChargeAxB / ( DistanceAB * DistanceAB * DistanceAB );
  double DEDR       = ca * dVecABMagR * dVecABMagR ;

  // XYZ fab   = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ) * DEDR;
  XYZ fab   = dVecAB * DEDR;
#endif

  XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ) ;
  double dVecABMag2 = dVecAB.LengthSquared() ;
  double dVecABMagR = 1.0 / sqrt(dVecABMag2) ;
  double dVecABMag = dVecABMag2 * dVecABMagR ;

  double ca         = Params.ChargeAxB * dVecABMagR ;

  double DEDR       = ca * dVecABMagR ;

///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  UDF_Binding::SwitchStd5_Params sp;
///////////////  UDF_Binding::SwitchStd5_OutputParams op;
///////////////  sp.r = sqrt(dVecABMag2);
///////////////  UDF_Binding::Function_Param<UDF_Binding::SwitchStd5_Params, UDF_Binding::SwitchStd5_OutputParams> pin;
///////////////  pin.A = &sp;
///////////////  pin.B = &op;
/////////////////  UDF_Binding::UDF_Execute(UDF_Binding::SwitchStd5_Code, aMDVM, (void *)(&pin));
///////////////  UDF_Binding::UDF_SwitchStd5_Execute(aMDVM, sp, op);
///////////////  double S = op.S ;
///////////////  double dSdR = op.dSdR ;
///////////////#else
  double S ;
  double pdSdR ;
  SwitchFunctionRadii sfr;
  aMDVM.GetSwitchFunctionRadii(sfr);
  SwitchFunction sf(sfr) ;  

///////  SwitchFunction sf(RTG.mSwitchFunctionRadii) ;
  sf.Evaluate(dVecABMag,S,pdSdR) ;
///////////////#endif

///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  XYZ fab = dVecAB * dVecABMagR * ( S * DEDR + dSdR * ca );
///////////////#else
  XYZ fab = dVecAB * dVecABMagR * ( S * DEDR - pdSdR * ca );
///////////////#endif

  aMDVM.ReportEnergy( ca * S );
  aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
  aMDVM.ReportForce( MDVM::SiteB , - fab ) ;

  }


template
<class MDVM>
static
inline
void
UDF_TIP3PForce_Execute(MDVM &aMDVM)
{
    UDF_HelperWater::FlexibleForce<MDVM, UDF_HelperWater::StdFlexible>( aMDVM );
    return;
}


template
<class MDVM>
static
inline
void
UDF_KineticEnergy_Execute(MDVM &aMDVM)
{
     double FragmentKineticEnergy = 0.0;
     for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
       {
         FragmentKineticEnergy +=    0.5
              * aMDVM.GetMass(SiteIndex)
           * aMDVM.GetVelocity(SiteIndex)
           * aMDVM.GetVelocity(SiteIndex);
       }

     aMDVM.ReportEnergy( FragmentKineticEnergy );

    return;
}


template
<class MDVM>
static
inline
void
UDF_WaterRigid3SiteShake_Execute(MDVM &aMDVM)
{
    UDF_HelperWater::Rigid3SiteShake<MDVM, UDF_HelperWater::TIP3P>( aMDVM );
    return;
}

template
<class MDVM>
static
inline
void
UDF_WaterRigid3SiteRattle_Execute(MDVM &aMDVM)
{
    UDF_HelperWater::Rigid3SiteRattle<MDVM, UDF_HelperWater::TIP3P>( aMDVM );
    return;
}


template
<class MDVM>
static
inline
void
UDF_UpdateVolumeHalfStep_Execute(MDVM &aMDVM)
{
    // UDF: 1 MDVM, 0 sites, Harmonic:UpdateVirial

    // does a half-step update on the volume velocity to T+dT/2
    //
    // assumes that the volume_force has been set correctly

    XYZ    volume_velocity        = aMDVM.GetVolumeVelocity();
///////    XYZ    volume_force           = aMDVM.GetVolumeForce();
    XYZ    volume_impulse           = aMDVM.GetVolumeImpulse();
    // double volume_mass         = aMDVM.GetVolumeMass();
    double volume_halfinversemass = aMDVM.GetVolumeHalfInverseMass();
    // double dt                     = aMDVM.GetDeltaT();

    // aMDVM.ReportVolumeVelocity( volume_velocity + volume_force / volume_mass * dt / 2.0 );
//////////    aMDVM.ReportVolumeVelocity( volume_velocity + volume_force * volume_halfinversemass * dt );
    aMDVM.ReportVolumeVelocity( volume_velocity + volume_impulse * volume_halfinversemass );
    aMDVM.ReportOldVolumeVelocity( volume_velocity );
    return;
}

template
<class MDVM>
static
inline
void
UDF_UpdateVolumePosition_Execute(MDVM &aMDVM)
{
    //
    // updates the volume position to T+dT [or if TIME_REVERSIBLE to T+dt/2]
    //
    const double dt                = aMDVM.GetDeltaT();
    const XYZ    old_inversevolume = aMDVM.GetVolumeInversePosition();
    const XYZ    old_volume        = aMDVM.GetVolumePosition();
#ifndef NOT_TIME_REVERSIBLE
    const XYZ    new_volume        = old_volume + 0.5 * dt * aMDVM.GetVolumeVelocity();
#else
    const XYZ    new_volume        = old_volume + dt * aMDVM.GetVolumeVelocity();
#endif

   // aMDVM.ReportVolumePosition( new_volume );

    // aMDVM.ReportVolumeRatio( pow( ( new_volume * old_inversevolume), (1.0 / 3.0) ));
   
    const double cuberoot = 1.0 / 3.0;

    XYZ ratio;

    // Report all components to same value
    ratio.mX = ratio.mY = ratio.mZ = 
        pow( new_volume.mX * old_inversevolume.mX, cuberoot );

    // ratio.mY = pow( new_volume.mY * old_inversevolume.mY, cuberoot );
    // ratio.mZ = pow( new_volume.mZ * old_inversevolume.mZ, cuberoot );

    // 3-Vector calculation
    // ratio.mX = new_volume.mX * old_inversevolume.mX;
    // ratio.mY = new_volume.mY * old_inversevolume.mY;
    // ratio.mZ = new_volume.mZ * old_inversevolume.mZ;

    aMDVM.ReportVolumeRatio( ratio );

    XYZ DynamicBoxDimensionVector = aMDVM.GetDynamicBoxDimensionVector();
	
    DynamicBoxDimensionVector.mX *= ratio.mX;
    DynamicBoxDimensionVector.mY *= ratio.mY;     
    DynamicBoxDimensionVector.mZ *= ratio.mZ;

///    XYZ boxSizeAbsoluteRatio;
    // for now just compute it here
///    double initvolume = boundingBoxDimensionVector.mX 
///                      * boundingBoxDimensionVector.mY 
///                      * boundingBoxDimensionVector.mZ;
///    boxSizeAbsoluteRatio.mX = pow( initvolume / new_volume.mX, cuberoot );
///    boxSizeAbsoluteRatio.mY = pow( initvolume / new_volume.mY, cuberoot );
///    boxSizeAbsoluteRatio.mZ = pow( initvolume / new_volume.mZ, cuberoot );

    aMDVM.ReportDynamicBoxDimensionVector( DynamicBoxDimensionVector );


    XYZ real_volume;
    real_volume.mX = real_volume.mY = real_volume.mZ = 
        DynamicBoxDimensionVector.mX * DynamicBoxDimensionVector.mY * DynamicBoxDimensionVector.mZ;

    aMDVM.ReportVolumePosition( real_volume );

    XYZ DynamicBoxInverseDimensionVector;             
    DynamicBoxInverseDimensionVector.mX = 1.0 / DynamicBoxDimensionVector.mX;
    DynamicBoxInverseDimensionVector.mY = 1.0 / DynamicBoxDimensionVector.mY;
    DynamicBoxInverseDimensionVector.mZ = 1.0 / DynamicBoxDimensionVector.mZ;

    aMDVM.ReportDynamicBoxInverseDimensionVector(  DynamicBoxInverseDimensionVector );        


    XYZ BirthBoxDimensionVector         = aMDVM.GetBirthBoxDimensionVector();
    XYZ BirthBoxAbsoluteRatio;
        
    BirthBoxAbsoluteRatio.mX = DynamicBoxInverseDimensionVector.mX * BirthBoxDimensionVector.mX;
    BirthBoxAbsoluteRatio.mY = DynamicBoxInverseDimensionVector.mY * BirthBoxDimensionVector.mY;
    BirthBoxAbsoluteRatio.mZ = DynamicBoxInverseDimensionVector.mZ * BirthBoxDimensionVector.mZ;
    
    aMDVM.ReportBirthBoxAbsoluteRatio( BirthBoxAbsoluteRatio );

    return;
}

template
<class MDVM>
static
inline
void
UDF_UpdateVelocityFirstHalfStepNPT_Execute(MDVM &aMDVM)
{
    //
    // updates particle velocities to T+dT/2
    //
    // NOTE: this routine returns with the site velocities holding the
    //       velocity of the site _relative_ to the fragment center of
    //       mass -- we clean things back up to real velocities at the
    //       second half step
    //
    // I am assuming this routine runs over a single irreducible fragment
    // called "thisFragment" or something
    //
    const double dt                  = aMDVM.GetDeltaT();
    const XYZ    vCOM                = aMDVM.GetCenterOfMassVelocity();
    const double mCOMHalfInverseMass = aMDVM.GetCenterOfMassHalfInverseMass();
///////////    const XYZ    fCOM                = aMDVM.GetCenterOfMassForce();
    const XYZ    impulseCOM                = aMDVM.GetCenterOfMassImpulse();

    const XYZ length_ratio           = aMDVM.GetVolumeRatio();

#ifndef NOT_TIME_REVERSIBLE
    const double expvelrat           = aMDVM.GetExpVolumeVelocityRatio();
#else
    const XYZ velocity_ratio         = aMDVM.GetVolumeVelocityRatio();
#endif

    XYZ new_vCOM;

    // new_vCOM.mX = length_ratio.mX * vCOM.mX * (1.0 - dt * (1.0 / 3.0) * velocity_ratio.mX)
    //                          + dt * fCOM.mX * mCOMHalfInverseMass;
    // new_vCOM.mY = length_ratio.mY * vCOM.mX * (1.0 - dt * (1.0 / 3.0) * velocity_ratio.mY)
    //                          + dt * fCOM.mY * mCOMHalfInverseMass;
    // new_vCOM.mZ = length_ratio.mZ * vCOM.mZ * (1.0 - dt * (1.0 / 3.0) * velocity_ratio.mZ)
    //                          + dt * fCOM.mZ * mCOMHalfInverseMass;

    // for now use x-dimension ratios
///    new_vCOM = length_ratio.mX * ( vCOM * ( 1.0 - dt / 3.0 * velocity_ratio.mX ) +
///                                   dt * fCOM * mCOMHalfInverseMass );

#ifndef NOT_TIME_REVERSIBLE
    new_vCOM = expvelrat * ( vCOM + impulseCOM * mCOMHalfInverseMass );
#else
    new_vCOM = length_ratio.mX * ( vCOM * ( 1.0 - dt / 3.0 * velocity_ratio.mX ) +
                                   impulseCOM * mCOMHalfInverseMass );
#endif

    aMDVM.ReportCenterOfMassVelocity( new_vCOM );

    //
    // loop over sites in this irreducible fragment
    //

    for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
        {
            double mSiteHalfInverseMass = aMDVM.GetHalfInverseMass( SiteIndex );
            XYZ    vSite                = aMDVM.GetVelocity( SiteIndex );
////////            XYZ    fSite                = aMDVM.GetForce( SiteIndex );
            XYZ    impulseSite                = aMDVM.GetImpulse( SiteIndex );

///            XYZ new_vSite = (vSite - vCOM) +  dt * 
///                            ( fSite * mSiteHalfInverseMass - fCOM * mCOMHalfInverseMass );
            XYZ new_vSite = (vSite - vCOM) + ( impulseSite * mSiteHalfInverseMass - impulseCOM * mCOMHalfInverseMass );

            aMDVM.ReportVelocity( SiteIndex, new_vSite );
        }

    return;
}

template
<class MDVM>
static
inline
void
UDF_UpdatePositionNPT_Execute(MDVM &aMDVM)
{
    //
    // again, operates on all sites of a fragment thisFragment
    // updates particle positions to T+dT
    //
    // first update COM position of the fragment

    const double dt   = aMDVM.GetDeltaT();
    const XYZ    vCOM = aMDVM.GetCenterOfMassVelocity();
    const XYZ    pCOM = aMDVM.GetCenterOfMassPosition();

#ifndef NOT_TIME_REVERSIBLE

    const double expfactor = aMDVM.GetExpVolumeVelocityRatio();

    XYZ new_pCOM;

    new_pCOM = expfactor * pCOM + 0.5 * dt * ( expfactor + 1.0 ) * vCOM;

#else

    const XYZ length_ratio   = aMDVM.GetVolumeRatio();

    XYZ new_pCOM;
    // new_pCOM.mX = length_ratio.mX * pCOM.mX + dt * vCOM.mX;
    // new_pCOM.mY = length_ratio.mY * pCOM.mY + dt * vCOM.mY;
    // new_pCOM.mZ = length_ratio.mZ * pCOM.mZ + dt * vCOM.mZ;

    // for now scale equally by x-dimension ratio
    new_pCOM = length_ratio.mX * pCOM + dt * vCOM;

#endif

    aMDVM.ReportCenterOfMassPosition( new_pCOM );

    //
    // loop over sites in this irreducible fragment
    //

    for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
    {
        XYZ vSite  = aMDVM.GetVelocity( SiteIndex );
        XYZ pSite  = aMDVM.GetPosition( SiteIndex );

        XYZ new_pSite = (pSite - pCOM) + new_pCOM + dt * vSite;

        aMDVM.ReportPosition( SiteIndex, new_pSite );
    }

    return;
}

template
<class MDVM>
static
inline
void
UDF_UpdateVelocitySecondHalfStepNPT_Execute(MDVM &aMDVM)
{
    const double dt                  = aMDVM.GetDeltaT();
    const XYZ    vCOM                = aMDVM.GetCenterOfMassVelocity();
    const double mCOMHalfInverseMass = aMDVM.GetCenterOfMassHalfInverseMass();
////////    const XYZ    fCOM                = aMDVM.GetCenterOfMassForce();
    const XYZ    impulseCOM                = aMDVM.GetCenterOfMassImpulse();

#ifndef NOT_TIME_REVERSIBLE
    const double expvelrat           = aMDVM.GetExpVolumeVelocityRatio();
#else
    const XYZ velocity_ratio         = aMDVM.GetVolumeVelocityRatio();
#endif

    XYZ new_vCOM;

    // here using only X component of velocity ratio
///    new_vCOM = ( vCOM + dt * fCOM * mCOMHalfInverseMass ) /
///               ( 1.0 + dt / 3.0 * velocity_ratio.mX );

#ifndef NOT_TIME_REVERSIBLE
    new_vCOM = expvelrat * vCOM + impulseCOM * mCOMHalfInverseMass;
#else
    new_vCOM = ( vCOM + impulseCOM * mCOMHalfInverseMass ) /
               ( 1.0 + dt / 3.0 * velocity_ratio.mX );
#endif

    aMDVM.ReportCenterOfMassVelocity( new_vCOM );

    for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
    {
        double mSiteHalfInverseMass = aMDVM.GetHalfInverseMass( SiteIndex );
        XYZ    vSite                = aMDVM.GetVelocity( SiteIndex );
////        XYZ    fSite                = aMDVM.GetForce( SiteIndex );
        XYZ    impulseSite                = aMDVM.GetImpulse( SiteIndex );

///        XYZ new_vSite =  new_vCOM + vSite
///            + dt * (fSite * mSiteHalfInverseMass - fCOM * mCOMHalfInverseMass);
        XYZ new_vSite =  new_vCOM + vSite + (impulseSite * mSiteHalfInverseMass - impulseCOM * mCOMHalfInverseMass);

        aMDVM.ReportVelocity( SiteIndex , new_vSite );
    }

    return;
}

template
<class MDVM>
static
inline
void
UDF_UpdateVirialForce_Execute(MDVM &aMDVM)
{
    //
    // routine to calculate the internal pressure of the system and thus the
    // force on the simulation box
    //2
    // big reductions would happen in GetMolecularVirial and GetCOMKineticEnergy
    //
/////////    const  XYZ virial                      = aMDVM.GetMolecularVirial();
    const double dt                        = aMDVM.GetDeltaT();
    const  double ekecom                   = aMDVM.GetCenterOfMassKineticEnergy(); /// desision to make: whether want to XYZ comp OR alternatively replace with 1/2 kT
    const  XYZ volume_inverseposition      = aMDVM.GetVolumeInversePosition();
    const  double external_pressurescalar  = aMDVM.GetPressureRTP();
    XYZ    external_pressure;

    external_pressure.mX = external_pressure.mY = external_pressure.mZ =
        external_pressurescalar;

    //
    // note that there can be a sign flip based on the accumulation of the
    // virial. . . we assume virial has been accumulated as - dU/dr dot r
    //

 
    XYZ internal_pressure;
    XYZ VolumeImpulse;
    // here in principle KE_mu should be used, for now use KE/3.0 instead
    // this actually is better from the analysis point of view, at least in the
    // case when the scaling is isotropic - this reduces fluctuations between the components
    // of pressure coming from fluctuations in kinetic energy components that should average to the same value

    internal_pressure.mX = internal_pressure.mY = internal_pressure.mZ = 2.0 * (ekecom/3.0) * volume_inverseposition.mX;
    VolumeImpulse.mX = VolumeImpulse.mY = VolumeImpulse.mZ = 2.0 * (ekecom/3.0) * dt * volume_inverseposition.mX;

    XYZ virialSum     = aMDVM.GetVirialSum();    
    XYZ virialImpulse = aMDVM.GetVirialImpulse();        

/////    for( int il = 0; il <= mIntegratorLevel; il++ )
/////    {
/////       internal_pressure += virial[rl];
/////       VolumeImpulse += virial[rl] * TimeStepPerRespaLevel[il];
/////    }

    internal_pressure += virialSum * volume_inverseposition.mX ;
    VolumeImpulse += virialImpulse * volume_inverseposition.mX;

    // report components separately...
    ////// internal_pressure.mX = internal_pressure.mY = internal_pressure.mZ = 1/3.0*(internal_pressure.mX + internal_pressure.mY + internal_pressure.mZ);

    // ... but keep the dynamics isotropic
    VolumeImpulse.mX = VolumeImpulse.mY = VolumeImpulse.mZ = 1/3.0*( VolumeImpulse.mX + VolumeImpulse.mY + VolumeImpulse.mZ );

    /////////////// suits - for now, just use x-component
    ///////////////internal_pressure.mX = internal_pressure.mY = internal_pressure.mZ =
    ///////////////    volume_inverseposition.mX / 3.0 * (2.0 * ekecom + (virial.mX+virial.mY+virial.mZ) );

    aMDVM.ReportInternalPressure( internal_pressure );

    VolumeImpulse.mX -= external_pressure.mX * dt;
    VolumeImpulse.mY -= external_pressure.mY * dt;
    VolumeImpulse.mZ -= external_pressure.mZ * dt;
    aMDVM.ReportVolumeImpulse( VolumeImpulse );

    return;
}


template
<class MDVM>
static
inline
void
UDF_SPCForce_Execute(MDVM &aMDVM)
{
//  duplicated from TIP3PForce.udf, RZ 10/2001
//  We need separate udf's for various water models now,
//  even though TIP3P and SPC, SPC/E only differs in parameters
    const double kr  =  450.0;
    const double r0  =    1.000;
    const double kth =   55.0;
    const double th0 =    1.910611932;  // 109.47 * Math::Deg2Rad;
//  This code previously assumed OHH order and now assumes HOH
//  changes were made only at the MDVM interface - not variable names

    XYZ    vAB = aMDVM.GetVector(  MDVM::SiteA, MDVM::SiteB);
    XYZ    vAC = aMDVM.GetVector(  MDVM::SiteC, MDVM::SiteB);
    XYZ RP = vAC.crossProduct(vAB);

    double RPL2 = RP.LengthSquared() ;

    double r2AB = vAB.LengthSquared() ;
    double r2AC = vAC.LengthSquared() ;

    double recipAB = 1.0/sqrt(r2AB) ;
    double recipAC = 1.0/sqrt(r2AC) ;
    // Caution: If the bond angle was 180 degrees, RPL2 will be zero so RPL will be Inf or NaN
    // This will propagate through SINTHE, atrig, AngleEnergy, DTD* but will all get
    // resolved in the IsVectorSquareLengthZero conditional
    double RPL = 1.0/sqrt(RPL2) ;

    double SINTHE = RPL2 * RPL * ( recipAB * recipAC) ;
    double rAB = recipAB * r2AB ;
    double rAC = recipAC * r2AC ;
    //double rAB = vAB.Length();
    //double rAC = vAC.Length();
    //double rAB = aMDVM.GetDistance( MDVM::SiteB, MDVM::SiteA );
    //double rAC = aMDVM.GetDistance( MDVM::SiteC, MDVM::SiteA );

//===========================================================

    double DeltaFromEquilibrium  = rAB - r0;

    double Energy = ( kr * DeltaFromEquilibrium ) * DeltaFromEquilibrium;

    // double  DEDR  = -2.0 * ( kr * DeltaFromEquilibrium ) / rAB;
    double  DEDR  = -2.0 * ( kr * DeltaFromEquilibrium ) * recipAB;

    XYZ ForceB = vAB * DEDR;
    XYZ ForceA = -ForceB;

//===========================================================

    DeltaFromEquilibrium  = rAC - r0;

    Energy += ( kr * DeltaFromEquilibrium ) * DeltaFromEquilibrium;

    //DEDR    = -2.0 * ( kr * DeltaFromEquilibrium ) / rAC;
    DEDR    = -2.0 * ( kr * DeltaFromEquilibrium ) * recipAC;

    XYZ ForceC  = vAC * DEDR;
        ForceA -= ForceC;

#ifdef DEBUG_UDF
    aMDVM.ReportUdfEnergy(UDF_Binding::StdHarmonicBondForce_Code ,Energy);
#endif
//===========================================================


    double COSTHE = ( vAB.DotProduct( vAC ) ) * (recipAB * recipAC);

    // double sindelta = SINTHE * costh0 - COSTHE * sinth0 ;
    // double delta = asin_small(sindelta) ;

    double THETA = Math::atrig(SINTHE, COSTHE) ;
    double delta = THETA - th0 ;

    // double THETA = acos(COSTHE);

    // Energy += kth * Math::sqr(THETA - th0);

    // Beware, the energy will be NaN if the angle was 180 degrees
    double AngleEnergy = kth * Math::sqr(delta);


    // double DEDTH = 2.0 * kth * (THETA - th0);
    double DEDTH = 2.0 * kth * (delta);

    // double RPL = RP.Length();

    // if (Heuristics::IsVectorLengthZero(RPL))
    //     RPL = 0.0;
    // else
    //    RPL = 1.0 / RPL;

    // double    R12R = -1.0 /  (rAB * rAB);
    // double    R32R =  1.0 /  (rAC * rAC);
    double    R12R = -1.0 *  (recipAB * recipAB);
    double    R32R =  1.0 *  (recipAC * recipAC);
    double    DTD12X = R12R*(RPL*DEDTH) * (vAB.mY*RP.mZ - vAB.mZ*RP.mY);
    double    DTD12Y = R12R*(RPL*DEDTH) * (vAB.mZ*RP.mX - vAB.mX*RP.mZ);
    double    DTD12Z = R12R*(RPL*DEDTH) * (vAB.mX*RP.mY - vAB.mY*RP.mX);
    double    DTD32X = R32R*(RPL*DEDTH) * (vAC.mY*RP.mZ - vAC.mZ*RP.mY);
    double    DTD32Y = R32R*(RPL*DEDTH) * (vAC.mZ*RP.mX - vAC.mX*RP.mZ);
    double    DTD32Z = R32R*(RPL*DEDTH) * (vAC.mX*RP.mY - vAC.mY*RP.mX);

    if ( !  Heuristics::IsVectorSquareLengthZero(RPL2) )
      {

        ForceB.mX -=  DTD12X        ;
        ForceB.mY -=  DTD12Y        ;
        ForceB.mZ -=  DTD12Z        ;
        ForceA.mX -= -DTD12X-DTD32X ;
        ForceA.mY -= -DTD12Y-DTD32Y ;
        ForceA.mZ -= -DTD12Z-DTD32Z ;
        ForceC.mX -=         DTD32X ;
        ForceC.mY -=         DTD32Y ;
        ForceC.mZ -=         DTD32Z ;
#ifdef DEBUG_UDF
        aMDVM.ReportUdfEnergy(UDF_Binding::StdHarmonicAngleForce_Code,AngleEnergy );
#endif

        Energy += AngleEnergy ;
        aMDVM.ReportEnergy(Energy);

        aMDVM.ReportForce( MDVM::SiteA, ForceB );
        aMDVM.ReportForce( MDVM::SiteB, ForceA );
        aMDVM.ReportForce( MDVM::SiteC, ForceC );
      }
      else
      {
        // In the special case, we computed AngleEnergy as NaN; perform the correct computation
        // (Assume the angle is 180 degrees, since 0 degrees is not physical)
        // Also, do not add the angle force in; its direction is undefined.
        double CollinearAngleEnergy = kth * Math::sqr(M_PI - th0 ) ;
#ifdef DEBUG_UDF
        aMDVM.ReportUdfEnergy(UDF_Binding::StdHarmonicAngleForce_Code,CollinearAngleEnergy );
#endif
        Energy += CollinearAngleEnergy ;
        aMDVM.ReportEnergy( Energy );

        aMDVM.ReportForce( MDVM::SiteA, ForceB );
        aMDVM.ReportForce( MDVM::SiteB, ForceA );
        aMDVM.ReportForce( MDVM::SiteC, ForceC );
      }

    return;
}


template
<class MDVM, class ParamsType>
static
inline
void
UDF_EwaldRealSpaceForce_Execute(MDVM &aMDVM, ParamsType &Params)
{
  PLATFORM_ABORT("EwaldRealSpaceForce udf is deprecated.  Use correct replacement.");
  // NOTE: This method assumes that distance based cutoffs are handled outside.

  XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ) ;
  double dVecABMag2 = dVecAB.LengthSquared() ;
  double dVecABMagR = 1.0 / sqrt(dVecABMag2);
  double dVecABMag = dVecABMag2 * dVecABMagR;

//  double erfc = Math::erfc( Params.alpha * dVecABMag );
//  double derfc = Math::derfc( Params.alpha * dVecABMag );
  double erfc;
  double derfc;
  Math::erfccd( Params.alpha * dVecABMag, erfc, derfc );
  double qaqb_r = Params.ChargeAxB * dVecABMagR;

///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  UDF_Binding::SwitchStd5_Params sp;
///////////////  UDF_Binding::SwitchStd5_OutputParams op;
///////////////  sp.r = dVecABMag;
///////////////  UDF_Binding::Function_Param<UDF_Binding::SwitchStd5_Params, UDF_Binding::SwitchStd5_OutputParams> pin;
///////////////  pin.A = &sp;
///////////////  pin.B = &op;
/////////////////  UDF_Binding::UDF_Execute(UDF_Binding::SwitchStd5_Code, aMDVM, (void *)(&pin));
///////////////  UDF_Binding::UDF_SwitchStd5_Execute(aMDVM, sp, op);
///////////////  double S = op.S ;
///////////////  double dSdR = op.dSdR ;
///////////////#else
  double S ;
  double pdSdR ;
  SwitchFunctionRadii sfr;
  aMDVM.GetSwitchFunctionRadii(sfr);
  SwitchFunction sf(sfr) ;  
//////////  SwitchFunction sf(RTG.mSwitchFunctionRadii) ;
  sf.Evaluate(dVecABMag,S,pdSdR) ;
///////////////#endif

  double energy = qaqb_r * erfc;
  aMDVM.ReportEnergy( energy * S );

  XYZ unitVecAB = dVecAB * dVecABMagR;
  double dEdR   = qaqb_r * ( erfc*dVecABMagR - derfc * Params.alpha);

///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  XYZ fab = ( S * dEdR + dSdR * energy ) * unitVecAB;
///////////////#else
  XYZ fab = ( S * dEdR - pdSdR * energy ) * unitVecAB;
///////////////#endif

  aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
  aMDVM.ReportForce( MDVM::SiteB , - fab ) ;
}




template
<class MDVM, class ParamsType>
static
inline
void
UDF_EwaldStructureFactors_Execute(MDVM &aMDVM, ParamsType &Params)
{
  PLATFORM_ABORT("EwaldStructureFactors udf is deprecated.  Use correct replacement.");
//  XYZ Kvector;
//  Kvector.mX = Params.kx;
//  Kvector.mY = Params.ky;
//  Kvector.mZ = Params.kz;
//
//  double kr = Kvector * aMDVM.GetPosition( MDVM::SiteA );
//  Complex ZEikr = Complex( cos(kr), -sin(kr) );
//  ZEikr *= Params.charge;
//
//  aMDVM.ReportComplexRegister( MDVM::SiteA, ZEikr );
}

template
<class MDVM>
static
inline
void
UDF_EwaldKSpaceForce_Execute(MDVM &aMDVM)
{
	// for now this is just a place holder
	PLATFORM_ABORT("EwaldKSpaceForce udf is deprecated.  Use correct replacement");
}

template
<class MDVM>
static
inline
void
UDF_EwaldSelfEnergyForce_Execute(MDVM &aMDVM)
{
	// for now this is just a place holder
	PLATFORM_ABORT("EwaldSelfEnergyForce udf is deprecated.  Use correct replacement.");
}

template
<class MDVM, class ParamsType>
static
inline
void
UDF_EwaldCorrectionForce_Execute(MDVM &aMDVM, ParamsType &Params)
{
  PLATFORM_ABORT("EwaldCorrectionForce udf is deprecated.  Use correct replacement.");
  // NOTE: This method assumes that distance based cutoffs are handled outside.

  XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ) ;
  double dVecABMag2 = dVecAB.LengthSquared() ;
  double dVecABMagR = 1.0 / sqrt(dVecABMag2);
  double dVecABMag = dVecABMag2 * dVecABMagR;

//  double erf = 1.0 - Math::erfc( Params.alpha * dVecABMag );
//  double derfc = Math::derfc( Params.alpha * dVecABMag );
  double erfc;
  double derfc;
  Math::erfccd( Params.alpha * dVecABMag, erfc, derfc );
  double erf = 1.0 - erfc;
  double qaqb_r = Params.ChargeAxB * dVecABMagR;


////////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  UDF_Binding::SwitchStd5_Params sp;
///////////////  UDF_Binding::SwitchStd5_OutputParams op;
///////////////  sp.r = dVecABMag;
///////////////  UDF_Binding::Function_Param<UDF_Binding::SwitchStd5_Params, UDF_Binding::SwitchStd5_OutputParams> pin;
///////////////  pin.A = &sp;
//////////////  pin.B = &op;
///////////////  UDF_Binding::UDF_Execute(UDF_Binding::SwitchStd5_Code, aMDVM, (void *)(&pin));
//////////////  UDF_Binding::UDF_SwitchStd5_Execute( aMDVM, sp, op );
/////////////  double S = op.S ;
//////////////  double dSdR = op.dSdR ;
////////////// #else
//////////////  double S ;
/////////////  double pdSdR ;
/////////////  SwitchFunction sf(RTG.mSwitchFunctionRadii) ;
/////////////  sf.Evaluate(dVecABMag,S,pdSdR) ;
///////////// #endif

  double energy = - qaqb_r * erf;
  aMDVM.ReportEnergy( energy );

  XYZ unitVecAB     = dVecAB * dVecABMagR;
  double dEdR   = - qaqb_r * ( erf*dVecABMagR + derfc * Params.alpha );

//////////////////////#if !defined(PERFORMANCE_SWITCH)
//////////////////////  XYZ fab = ( S * dEdR + dSdR * energy ) * unitVecAB;
/////////////////////#else
  XYZ fab = dEdR * unitVecAB;
////////////////#endif

  aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
  aMDVM.ReportForce( MDVM::SiteB , - fab ) ;
}




template
<class MDVM>
static
inline
void
UDF_WaterSPCShake_Execute(MDVM &aMDVM)
{
    UDF_HelperWater::Rigid3SiteShake<MDVM, UDF_HelperWater::SPC>( aMDVM );
    return;
}

template
<class MDVM>
static
inline
void
UDF_WaterSPCRattle_Execute(MDVM &aMDVM)
{
    UDF_HelperWater::Rigid3SiteRattle<MDVM, UDF_HelperWater::SPC>( aMDVM );
    return;
}


template
<class MDVM>
static
inline
void
UDF_WaterTIP3PShake_Execute(MDVM &aMDVM)
{
    UDF_HelperWater::Rigid3SiteShake<MDVM, UDF_HelperWater::TIP3P>( aMDVM );
    return;
}

template
<class MDVM>
static
inline
void
UDF_WaterTIP3PRattle_Execute(MDVM &aMDVM)
{
    UDF_HelperWater::Rigid3SiteRattle<MDVM, UDF_HelperWater::TIP3P>( aMDVM );
    return;
}


template
<class MDVM>
static
inline
void
UDF_NSQLennardJones_Execute( MDVM &aMDVM, const NSQLennardJones_Params &A)
  {
  const double epsilon = A.epsilon;
  const double sigma   = A.sigma;

  double dVecABMag2 = aMDVM.GetDistanceSquared( MDVM::SiteA, MDVM::SiteB ) ;
//  double dVecABMagR = 1.0 / sqrt(dVecABMag2) ;
  double dVecABMagR = aMDVM.GetReciprocalDistance( MDVM::SiteA, MDVM::SiteB) ;
  double dVecABMag = dVecABMag2 * dVecABMagR ;
  double dVecABMag2R = dVecABMagR * dVecABMagR ;
  double tmp2 = sigma*sigma*dVecABMag2R ;
  double tmp6 = tmp2*tmp2*tmp2 ;
  double tmp12         = tmp6 * tmp6;

  double lje           = epsilon * (tmp12 - 2.0 * tmp6);

  double DEDR =
                (
                  (12 * epsilon) *
                  (
                      (  tmp12)
                    - (  tmp6 )
                  )
                ) * dVecABMagR ;

///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  UDF_Binding::SwitchStd5_Params sp;
///////////////  UDF_Binding::SwitchStd5_OutputParams op;
///////////////  sp.r = sqrt(dVecABMag2);
///////////////  UDF_Binding::Function_Param<UDF_Binding::SwitchStd5_Params, UDF_Binding::SwitchStd5_OutputParams> pin;
///////////////  pin.A = &sp;
///////////////  pin.B = &op;
/////////////////  UDF_Binding::UDF_Execute(UDF_Binding::SwitchStd5_Code, aMDVM, (void *)(&pin));
///////////////  UDF_Binding::UDF_SwitchStd5_Execute(aMDVM, sp, op);
///////////////  double S = op.S ;
///////////////  double dSdR = op.dSdR ;
///////////////#else
///////////////  double S ;
///////////////  double pdSdR ;
///////////////  SwitchFunctionRadii sfr;
///////////////  aMDVM.GetSwitchFunctionRadii(sfr);
///////////////  SwitchFunction sf(sfr) ;
/////////  SwitchFunction sf(RTG.mSwitchFunctionRadii) ;
///////////////#if defined(PERFORMANCE_EXPLOIT_FULLY_ON)
///////////////  sf.Evaluate<aMDVM.kForceFullyOn>(dVecABMag,S,pdSdR) ;
///////////////#else
///////////////  sf.Evaluate(dVecABMag,S,pdSdR) ;
///////////////#endif
///////////////#endif
#if 1
  if( lje != 0.0 )
      // ( ( aMDVM.GetSiteId( 0 ) == 1056 ) || ( aMDVM.GetSiteId( 1 ) == 1056 ) ) )
    {
    
    int SiteA = aMDVM.GetSiteId( 0 );
    int SiteB = aMDVM.GetSiteId( 1 );
    
    if( SiteA > SiteB )
      {
      int tmp = SiteA;
      SiteA = SiteB;
      SiteB = tmp;
      }
    
    //BegLogLine( SimTick == 11 )
    BegLogLine( 0 )
      << "LJE: " << SiteA << " " << SiteB << " " << lje
      << EndLogLine;
    }
#endif

  aMDVM.ReportEnergy( lje );
  aMDVM.ReportdEdr ( -DEDR ) ;
///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  XYZ    fab     = dVecAB *dVecABMagR * ( S * DEDR + dSdR * lje );
///////////////#else
///////////////  XYZ    fab     = dVecAB *dVecABMagR * ( S * DEDR - pdSdR * lje );
///////////////#endif

  if ( 0 == aMDVM.kSuppressVectorReporting )
  {
// Force reporting now done by 'ReportdEdr'
  XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB) ;
  XYZ    fab     = dVecAB *dVecABMagR * DEDR;

  aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
  aMDVM.ReportForce( MDVM::SiteB , - fab ) ;
  }
  return;
  }

template
<class MDVM>
static
inline
void
UDF_NSQCoulomb_Execute( MDVM &aMDVM, const NSQCoulomb_Params &A)
  {
  // const double ChargeAxB = A.Charge * B.Charge;
  const double ChargeAxB = A.ChargeAxB;

  double dVecABMag2 = aMDVM.GetDistanceSquared( MDVM::SiteA, MDVM::SiteB ) ;
//  double dVecABMagR = 1.0 / sqrt(dVecABMag2) ;
  double dVecABMagR = aMDVM.GetReciprocalDistance( MDVM::SiteA, MDVM::SiteB ) ;
  double dVecABMag = dVecABMag2 * dVecABMagR ;

  double ca         = ChargeAxB * dVecABMagR ;

// Note: What we call 'DEDR' is really -d/d|r| (Energy assuming no switch)
//       i.e. the opposite sign from what you probably expected
  double DEDR       = ca * dVecABMagR ;

///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  UDF_Binding::SwitchStd5_Params sp;
///////////////  UDF_Binding::SwitchStd5_OutputParams op;
///////////////  sp.r = sqrt(dVecABMag2);
///////////////  UDF_Binding::Function_Param<UDF_Binding::SwitchStd5_Params, UDF_Binding::SwitchStd5_OutputParams> pin;
///////////////  pin.A = &sp;
///////////////  pin.B = &op;
/////////////////  UDF_Binding::UDF_Execute(UDF_Binding::SwitchStd5_Code, aMDVM, (void *)(&pin));
///////////////  UDF_Binding::UDF_SwitchStd5_Execute(aMDVM, sp, op);
///////////////  double S = op.S ;
///////////////  double dSdR = op.dSdR ;
///////////////#else
////////////////  double S ;
///////////////  double pdSdR ;
///////////////  SwitchFunctionRadii sfr;
//////////////  aMDVM.GetSwitchFunctionRadii(sfr);
///////////////  SwitchFunction sf(sfr) ;

//////  SwitchFunction sf(RTG.mSwitchFunctionRadii) ;
///////////////#if defined(PERFORMANCE_EXPLOIT_FULLY_ON)
//////////////  sf.Evaluate<aMDVM.kForceFullyOn>(dVecABMag,S,pdSdR) ;
//////////////#else
///////////////  sf.Evaluate(dVecABMag,S,pdSdR) ;
//////////////////#endif
///////////////#endif

  // XYZ fab   = dVecAB * DEDR;
///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  XYZ fab = dVecAB * dVecABMagR * ( S * DEDR + dSdR * ca );
///////////////#else

// Force = -d/d(position) of Energy
// Energy = Energy0 * Switch
// so apply the rule for differentiation of products, (uv)' = u'v + uv'
// 'DEDR' is negated from what you might expect, so we have to negate pdSdR as well
// and be careful as to which atom we apply it to

/////////////////  XYZ fab = dVecAB * dVecABMagR * ( S * DEDR - pdSdR * ca );
///////////////#endif

  aMDVM.ReportEnergy( ca );
  aMDVM.ReportdEdr( -DEDR ) ;

  if ( 0 == aMDVM.kSuppressVectorReporting )
  {
    XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ) ;
    XYZ fab = dVecAB * dVecABMagR * DEDR;

    aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
    aMDVM.ReportForce( MDVM::SiteB , - fab ) ;
   }
  }


template
<class MDVM>
static
inline
void
UDF_LJStdCombiner_Execute( MDVM &aMDVM, const LJStdCombiner_Params &A, const LJStdCombiner_Params &B, LJStdCombiner_Params &C)
  {
      C.epsilon = sqrt(A.epsilon * B.epsilon);
//      C.sigma   =     (A.sigma   + B.sigma  ) / 2.0;
      C.sigma   =     (A.sigma   + B.sigma  );
  }


template
<class MDVM>
static
inline
void
UDF_SwitchStd5_Execute( MDVM &aMDVM, const SwitchStd5_Params &A, SwitchStd5_OutputParams &B)
  {
      const double TreatSwitchAsOff = 1.0E99;

      if (A.r <= UDF_State::SwitchStd5.Rl || UDF_State::SwitchStd5.Rl > TreatSwitchAsOff ) {
	  B.S = 1;
	  B.dSdR = 0;
	  return;
      } else if (A.r >= UDF_State::SwitchStd5.Ru) {
	  B.S = 0;
	  B.dSdR = 0;
	  return;
      }

      const double dr  = A.r - UDF_State::SwitchStd5.Rl; 
      const double dr2 = dr  * dr;
      const double dr3 = dr2 * dr;
      const double dr4 = dr2 * dr2;

      B.S = UDF_State::SwitchStd5.A * dr2 * dr3 +
	    UDF_State::SwitchStd5.B * dr4       +
	    UDF_State::SwitchStd5.C * dr3       +
	    1.0;

      // The derivative was reversed below to make energy conserve
      // due to minus signs in the udf force assignments
      B.dSdR = -UDF_State::SwitchStd5.AA * dr4 -
	        UDF_State::SwitchStd5.BB * dr3 -
	        UDF_State::SwitchStd5.CC * dr2;
  }

template
<class MDVM, class ParamsType>
static
inline
void
UDF_NSQEwaldCorrection_Execute(MDVM &aMDVM, ParamsType &Params)
{
  // NOTE: This method assumes that distance based cutoffs are handled outside.

  // double q1 = A.Charge;
  // double q2 = B.Charge;

  // double Params.ChargeAxB = q1 * q2;
  // double Params.ChargeAxB = A.ChargeAxB;
  double alpha = aMDVM.GetEwaldAlpha();

  double dVecABMag2 = aMDVM.GetDistanceSquared( MDVM::SiteA, MDVM::SiteB ) ;
  double dVecABMagR = 1.0 / sqrt(dVecABMag2);
//  double dVecABMagR = aMDVM.GetReciprocalDistance( MDVM::SiteA, MDVM::SiteB ) ;
  double dVecABMag = dVecABMag2 * dVecABMagR;

//  double erf = 1.0 - Math::erfc( alpha * dVecABMag );
//  double derfc = Math::derfc( alpha * dVecABMag );
  double erfc;
  double derfc;
  Math::erfccd( alpha * dVecABMag, erfc, derfc );
  double erf = 1.0 - erfc;
  double qaqb_r = Params.ChargeAxB * dVecABMagR;

////////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  UDF_Binding::SwitchStd5_Params sp;
///////////////  UDF_Binding::SwitchStd5_OutputParams op;
///////////////  sp.r = dVecABMag;
///////////////  UDF_Binding::Function_Param<UDF_Binding::SwitchStd5_Params, UDF_Binding::SwitchStd5_OutputParams> pin;
///////////////  pin.A = &sp;
//////////////  pin.B = &op;
///////////////  UDF_Binding::UDF_Execute(UDF_Binding::SwitchStd5_Code, aMDVM, (void *)(&pin));
//////////////  UDF_Binding::UDF_SwitchStd5_Execute( aMDVM, sp, op );
/////////////  double S = op.S ;
//////////////  double dSdR = op.dSdR ;
////////////// #else
//////////////  double S ;
/////////////  double pdSdR ;
/////////////  SwitchFunction sf(RTG.mSwitchFunctionRadii) ;
/////////////  sf.Evaluate(dVecABMag,S,pdSdR) ;
///////////// #endif

  double energy = - qaqb_r * erf;
  aMDVM.ReportEnergy( energy );

  double dEdR   = - qaqb_r * ( erf*dVecABMagR + derfc * alpha );

  aMDVM.ReportdEdr(-dEdR) ;

  if ( 0 == aMDVM.kSuppressVectorReporting )
  {
    
// Only any point reporting virials if we are doing pressure control  
 // the (partial) virial here is 3Q( d/d alpha (energy) * d alpha/dQ ), d alpha/dQ = -alpha/3Q
 // Note ... this is not (in general) along the vector between the atoms !
  XYZ virial;
  virial.mX = virial.mY = virial.mZ = (Params.ChargeAxB * (-derfc) * alpha) / 3.0;

  aMDVM.ReportVirial( - virial );

  XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ) ;
  XYZ unitVecAB     = dVecAB * dVecABMagR;
//////////////////#if !defined(PERFORMANCE_SWITCH)
  XYZ fab = dEdR * unitVecAB;
////////////////#else
/////////////////  XYZ fab = ( S * dEdR - pdSdR * energy ) * unitVecAB;
////////////////#endif

// Force reporting done by 'reportdEdr', but not virial reporting
  aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
  aMDVM.ReportForce( MDVM::SiteB , - fab ) ;
  } else {
// Only any point reporting virials if we are doing pressure control  
    aMDVM.ReportAdditionalScalarVirial(-Params.ChargeAxB*derfc*alpha) ;
  }
 }




template
<class MDVM, class ParamsType>
static
inline
void
UDF_NSQEwaldRealSpace_Execute(MDVM &aMDVM, ParamsType &Params)
{
  // NOTE: This method assumes that distance based cutoffs are handled outside.

  // double q1 = A.Charge;
  // double q2 = B.Charge;

  // double Params.ChargeAxB = q1 * q2;
  // double Params.ChargeAxB = A.ChargeAxB;
  double alpha = aMDVM.GetEwaldAlpha();

  double dVecABMag2 = aMDVM.GetDistanceSquared( MDVM::SiteA, MDVM::SiteB ) ;
//  double dVecABMagR = 1.0 / sqrt(dVecABMag2);
  double dVecABMagR = aMDVM.GetReciprocalDistance( MDVM::SiteA, MDVM::SiteB ) ;
  double dVecABMag = dVecABMag2 * dVecABMagR;

//  double erfc = Math::erfc( alpha * dVecABMag );
//  double derfc = Math::derfc( alpha * dVecABMag );
  double erfc;
  double derfc;
  Math::erfccd( alpha * dVecABMag, erfc, derfc );
  double qaqb_r = Params.ChargeAxB * dVecABMagR;

///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  UDF_Binding::SwitchStd5_Params sp;
///////////////  UDF_Binding::SwitchStd5_OutputParams op;
///////////////  sp.r = dVecABMag;
/////////////// UDF_Binding::Function_Param<UDF_Binding::SwitchStd5_Params, UDF_Binding::SwitchStd5_OutputParams> pin;
///////////////  pin.A = &sp;
///////////////  pin.B = &op;
/////////////////  UDF_Binding::UDF_Execute(UDF_Binding::SwitchStd5_Code, aMDVM, (void *)(&pin));
///////////////  UDF_Binding::UDF_SwitchStd5_Execute(aMDVM, sp, op);
///////////////  double S = op.S ;
///////////////  double dSdR = op.dSdR ;
///////////////#else
///////////////  double S ;
///////////////  double pdSdR ;
///////////////  SwitchFunctionRadii sfr;
///////////////  aMDVM.GetSwitchFunctionRadii(sfr);
///////////////  SwitchFunction sf(sfr) ;
////  SwitchFunction sf(RTG.mSwitchFunctionRadii) ;
///////////////#if defined(PERFORMANCE_EXPLOIT_FULLY_ON)
///////////////  sf.Evaluate<aMDVM.kForceFullyOn>(dVecABMag,S,pdSdR) ;
///////////////#else
///////////////  sf.Evaluate(dVecABMag,S,pdSdR) ;
///////////////#endif
///////////////#endif

  double energy = qaqb_r * erfc;
  aMDVM.ReportEnergy( energy );

  double dEdR   = qaqb_r * ( erfc*dVecABMagR - derfc * alpha);

  aMDVM.ReportdEdr ( -dEdR ) ;
///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  XYZ fab = ( S * dEdR + dSdR * energy ) * unitVecAB;
///////////////#else
///////////////  XYZ fab = ( S * dEdR - pdSdR * energy ) * unitVecAB;
///////////////#endif

// See if we can steer clear of vector-ness
  if ( 0 == aMDVM.kSuppressVectorReporting )
  {
    XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ) ;
    XYZ unitVecAB = dVecAB * dVecABMagR;
   XYZ fab = unitVecAB * dEdR;
    aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
    aMDVM.ReportForce( MDVM::SiteB , - fab ) ;
    // the (partial) virial here is 3Q( d/d alpha (energy) * d alpha/dQ ), d alpha/dQ = -alpha/3Q
    XYZ virial;
    virial.mX = virial.mY = virial.mZ = Params.ChargeAxB * derfc * alpha * Math::OneThird;

    aMDVM.ReportVirial( virial );
    aMDVM.ReportVVirial( PairwiseProduct(fab,dVecAB) ) ;
   }
   else
   {
       // Report the Ewald virial as a scalar. Code elsewhere will 
	// partition it between dimensions if we are doing vector virials
	aMDVM.ReportAdditionalScalarVirial(
            Params.ChargeAxB * derfc * alpha 
	) ;
   }

}




template
<class MDVM, class ParamsType>
static
inline
void
UDF_Coulomb14_Execute(MDVM &aMDVM, ParamsType &Params)
  {
#if !defined(OMIT_BOND_VECTOR_TABLE)
  double dVecABMagR = aMDVM.GetRecipLength() ;
  double ca         = Params.ChargeAxB * dVecABMagR ;

  double dEdr       = ca * dVecABMagR ;
  aMDVM.ReportEnergy( ca );
  aMDVM.ReportdEdr(dEdr) ;

#else 
  XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ) ;
  double dVecABMag2 = dVecAB.LengthSquared() ;
  double dVecABMagR = 1.0 / sqrt(dVecABMag2) ;
  double dVecABMag = dVecABMag2 * dVecABMagR ;

  double ca         = Params.ChargeAxB * dVecABMagR ;

  double DEDR       = ca * dVecABMagR ;

//////////////////#if !defined(PERFORMANCE_SWITCH)
//////////////////  UDF_Binding::SwitchStd5_Params sp;
//////////////////  UDF_Binding::SwitchStd5_OutputParams op;
//////////////////  sp.r = sqrt(dVecABMag2);
//////////////////  UDF_Binding::Function_Param<UDF_Binding::SwitchStd5_Params, UDF_Binding::SwitchStd5_OutputParams> pin;
//////////////////  pin.A = &sp;
//////////////////  pin.B = &op;
////////////////////  UDF_Binding::UDF_Execute(UDF_Binding::SwitchStd5_Code, aMDVM, (void *)(&pin));
//////////////////  UDF_Binding::UDF_SwitchStd5_Execute(aMDVM, sp, op);
//////////////////  double S = op.S ;
//////////////////  double dSdR = op.dSdR ;
//////////////////#else
//////////////////  double S ;
//////////////////  double pdSdR ;
//////////////////  SwitchFunction sf(RTG.mSwitchFunctionRadii) ;
//////////////////  sf.Evaluate(dVecABMag,S,pdSdR) ;
//////////////////#endif

//////////////////#if !defined(PERFORMANCE_SWITCH)
  XYZ fab = dVecAB * dVecABMagR * DEDR ;
///////////////#else
///////////////  XYZ fab = dVecAB * dVecABMagR * ( S * DEDR - pdSdR * ca );
///////////////#endif

  aMDVM.ReportEnergy( ca );
  aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
  aMDVM.ReportForce( MDVM::SiteB , - fab ) ;
  
  
#endif
  }


template
<class MDVM>
static
inline
void
UDF_WaterTIP3PInit_Execute(MDVM &aMDVM)
{
    UDF_HelperWater::Rigid3SiteInit<MDVM, UDF_HelperWater::TIP3P>( aMDVM );
    return;
}


template
<class MDVM>
static
inline
void
UDF_WaterSPCInit_Execute(MDVM &aMDVM)
{
    UDF_HelperWater::Rigid3SiteInit<MDVM, UDF_HelperWater::SPC>( aMDVM );
    return;
}


template
<class MDVM>
static
inline
void
UDF_LJOPLSCombiner_Execute( MDVM &aMDVM, const LJOPLSCombiner_Params &A, const LJOPLSCombiner_Params &B, LJOPLSCombiner_Params &C)
  {
      C.epsilon = sqrt(A.epsilon * B.epsilon);
      C.sigma   = sqrt(A.sigma   * B.sigma  );
  }


template
<class MDVM>
static
inline
void
UDF_NSQCheckSitesInBox_Execute(MDVM &aMDVM)
{
    // This will be in the rtp somehow
    const double MinSeparation = 0.8;

    double SeparationSquared = aMDVM.GetImageDistanceSquared( MDVM::SiteA, MDVM::SiteB );

    if ( SeparationSquared < MinSeparation*MinSeparation )
    {
        XYZ pA = aMDVM.GetPosition( MDVM::SiteA );
        XYZ pB = aMDVM.GetPosition( MDVM::SiteB );
        BegLogLine(1)
             << "Pair " << pA << " " << pB << " are too close, with separation " << sqrt( SeparationSquared )
             << "Check that the box size agrees with the dvs file."
             << EndLogLine ;
        assert(0);
    }
}

template
<class MDVM>
static
inline
void
UDF_COMKineticEnergy_Execute(MDVM &aMDVM)
{
    // for now this is just a place holder for the COMKE "semi-udf"
    return;
}


template
<class MDVM>
static
inline
void
UDF_ResampleEnergyLoss_Execute(MDVM &aMDVM)
{
    // for now this is just a place holder for the ResampleEnergyLoss "semi-udf"
    return;
}


template
<class MDVM>
static
inline
void
UDF_UpdateVolumeSecondHalfStep_Execute(MDVM &aMDVM)
{
///  const double dt                      = aMDVM.GetDeltaT();
  const XYZ    volume_vold             = aMDVM.GetOldVolumeVelocity();
////  const XYZ    volume_force            = aMDVM.GetVolumeForce();
  const XYZ    volume_impulse            = aMDVM.GetVolumeImpulse();
  const double volume_halfinversemass  = aMDVM.GetVolumeHalfInverseMass();
  const XYZ    volume_inverseposition  = aMDVM.GetVolumeInversePosition();

  XYZ volume_vest;

////  volume_vest.mX = volume_vest.mY = volume_vest.mZ =
////    volume_vold.mX + dt * volume_force.mX * 2 * volume_halfinversemass;

  volume_vest.mX = volume_vest.mY = volume_vest.mZ =
    volume_vold.mX + volume_impulse.mX * 2 * volume_halfinversemass;

  XYZ velocity_ratio;

  // set all components to the same value
  velocity_ratio.mX = velocity_ratio.mY = velocity_ratio.mZ =
      volume_vest.mX * volume_inverseposition.mX;
  aMDVM.ReportVolumeVelocityRatio( velocity_ratio );

  return;
}

template
<class MDVM>
static
inline
void
UDF_ZeroForce_Execute(MDVM &aMDVM)
{
///  aMDVM.SetCenterOfMassForce( XYZ::ZERO_VALUE() );
  for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
    {
    aMDVM.ZeroForce( SiteIndex );
    }
  return;
}

template
<class MDVM>
static
inline
void
UDF_UpdatePosition_Execute(MDVM &aMDVM)
{
  for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
    {
    XYZ NewPosition = aMDVM.GetPosition( SiteIndex ) + aMDVM.GetDisplacement( SiteIndex );
    aMDVM.ReportPosition(  SiteIndex, NewPosition );
    }

  return;
}

template
<class MDVM>
static
inline
void
UDF_UpdateVelocity_Execute(MDVM &aMDVM)
{
  for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
    {
    XYZ NewVelocity = aMDVM.GetVelocity( SiteIndex )
                      + (   aMDVM.GetImpulse( SiteIndex )
                          * aMDVM.GetHalfInverseMass( SiteIndex ) );
    aMDVM.ReportVelocity(  SiteIndex, NewVelocity );
    }

  return;
}

template
<class MDVM>
static
inline
void
UDF_UpdateCenterOfMassKineticEnergy_Execute(MDVM &aMDVM)
{
  // This assumes the COM velocity is up to date
  // Should call UpdateCenterOfMass on each fragment prior
  XYZ    COMVelocity = aMDVM.GetCenterOfMassVelocity();
  double COMMass     = aMDVM.GetCenterOfMassMass();
  double COMKE = 0.5 * COMMass * COMVelocity * COMVelocity;

  aMDVM.ReportCenterOfMassKineticEnergy( COMKE );

  return;
}

template
<class MDVM>
static
inline
void
UDF_UpdateCenterOfMass_Execute(MDVM &aMDVM)
{
  double InverseMass = 2 * aMDVM.GetCenterOfMassHalfInverseMass();
  XYZ    SumPositionTimesMass;
  XYZ    SumVelocityTimesMass;
  SumPositionTimesMass.Zero();
  SumVelocityTimesMass.Zero();

  for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
    {
    SumPositionTimesMass  += aMDVM.GetPosition( SiteIndex ) * aMDVM.GetMass( SiteIndex );
    SumVelocityTimesMass  += aMDVM.GetVelocity( SiteIndex ) * aMDVM.GetMass( SiteIndex );

    // Adds the force of the site to the center of mass force on per respa level     
    aMDVM.AddForceToCenterOfMassForce( SiteIndex );    
    }
  XYZ CenterOfMassPosition = SumPositionTimesMass * InverseMass;
  XYZ CenterOfMassVelocity = SumVelocityTimesMass * InverseMass;
  aMDVM.ReportCenterOfMassPosition( CenterOfMassPosition );
  aMDVM.ReportCenterOfMassVelocity( CenterOfMassVelocity );

  return;
}

template
<class MDVM, class ParamsType>
static
inline
void
UDF_LennardJones14Force_Execute(MDVM &aMDVM, ParamsType &Params)
  {
#if !defined(OMIT_BOND_VECTOR_TABLE)
  double dVecABMag = aMDVM.GetLength() ;
  double dVecABMagR = aMDVM.GetRecipLength() ;
  double dVecABMag2R   = dVecABMagR*dVecABMagR ;
  double tmp2          = Params.sigma*Params.sigma*dVecABMag2R ;
  double tmp6          = tmp2*tmp2*tmp2 ;
  double tmp12         = tmp6 * tmp6;

  double lje           = Params.epsilon * (tmp12 - 2.0 * tmp6) ;
  double dEdr =
                (
                  (12 * Params.epsilon) *
                  (
                      (  tmp12)
                    - (  tmp6)
                  )
                )  * dVecABMagR ;

  aMDVM.ReportEnergy( lje );
  aMDVM.ReportdEdr(dEdr) ;

#else
  XYZ    dVecAB        = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB) ;
  double dVecABMag2    = dVecAB.LengthSquared() ;

  double dVecABMagR    = 1.0/sqrt(dVecABMag2) ;
  double dVecABMag2R   = dVecABMagR*dVecABMagR ;
  double dVecABMag     = dVecABMag2 * dVecABMagR ;

  double tmp2          = Params.sigma*Params.sigma*dVecABMag2R ;
  double tmp6          = tmp2*tmp2*tmp2 ;
  double tmp12         = tmp6 * tmp6;

  double lje           = Params.epsilon * (tmp12 - 2.0 * tmp6) ;

  aMDVM.ReportEnergy( lje );

  double DEDR =
                (
                  (12 * Params.epsilon) *
                  (
                      (  tmp12)
                    - (  tmp6)
                  )
                ) * dVecABMagR ;

  XYZ    fab     = dVecAB * dVecABMagR * DEDR;

  aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
  aMDVM.ReportForce( MDVM::SiteB , - fab ) ;

  return;
#endif  
  }

template
<class MDVM, class ParamsType>
static
inline
void
UDF_OPLSLennardJones14Force_Execute(MDVM &aMDVM, ParamsType &Params)
  {

  XYZ    dVecAB        = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB) ;
  double dVecABMag2    = dVecAB.LengthSquared() ;

  double dVecABMagR    = 1.0/sqrt(dVecABMag2) ;
  double dVecABMag2R   = dVecABMagR*dVecABMagR ;
  double dVecABMag     = dVecABMag2 * dVecABMagR ;

  double tmp2          = Params.sigma*Params.sigma*dVecABMag2R ;
  double tmp6          = tmp2*tmp2*tmp2 ;
  double tmp12         = tmp6 * tmp6;

  double lje           = Params.epsilon * (tmp12 - 2.0 * tmp6) ;

  aMDVM.ReportEnergy( lje );

  double DEDR =
                (
                  (12 * Params.epsilon) *
                  (
                      (  tmp12)
                    - (  tmp6)
                  )
                ) * dVecABMagR ;

  XYZ    fab     = dVecAB * dVecABMagR * DEDR;

  aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
  aMDVM.ReportForce( MDVM::SiteB , - fab ) ;

  return;
  }

template
<class MDVM>
static
inline
void
UDF_NSQOPLSLennardJones_Execute( MDVM &aMDVM, const NSQOPLSLennardJones_Params &A)
  {
  const double epsilon = A.epsilon;
  const double sigma   = A.sigma;

  double dVecABMag2 = aMDVM.GetDistanceSquared( MDVM::SiteA, MDVM::SiteB ) ;
//  double dVecABMagR = 1.0 / sqrt(dVecABMag2) ;
  double dVecABMagR = aMDVM.GetReciprocalDistance( MDVM::SiteA, MDVM::SiteB) ;
  double dVecABMag = dVecABMag2 * dVecABMagR ;
  double dVecABMag2R = dVecABMagR * dVecABMagR ;
  double tmp2 = sigma*sigma*dVecABMag2R ;
  double tmp6 = tmp2*tmp2*tmp2 ;
  double tmp12         = tmp6 * tmp6;

  double lje           = epsilon * (tmp12 - 2.0 * tmp6);

  double DEDR =
                (
                  (12 * epsilon) *
                  (
                      (  tmp12)
                    - (  tmp6 )
                  )
                ) * dVecABMagR ;

///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  UDF_Binding::SwitchStd5_Params sp;
///////////////  UDF_Binding::SwitchStd5_OutputParams op;
///////////////  sp.r = sqrt(dVecABMag2);
///////////////  UDF_Binding::Function_Param<UDF_Binding::SwitchStd5_Params, UDF_Binding::SwitchStd5_OutputParams> pin;
///////////////  pin.A = &sp;
///////////////  pin.B = &op;
/////////////////  UDF_Binding::UDF_Execute(UDF_Binding::SwitchStd5_Code, aMDVM, (void *)(&pin));
///////////////  UDF_Binding::UDF_SwitchStd5_Execute(aMDVM, sp, op);
///////////////  double S = op.S ;
///////////////  double dSdR = op.dSdR ;
///////////////#else
  double S ;
  double pdSdR ;
  SwitchFunctionRadii sfr;
  aMDVM.GetSwitchFunctionRadii(sfr);
  SwitchFunction sf(sfr) ;
///////  SwitchFunction sf(RTG.mSwitchFunctionRadii) ;
  sf.Evaluate(dVecABMag,S,pdSdR) ;
///////////////#endif

  aMDVM.ReportEnergy( lje * S );
// !!! Caution -- this is reporting 'switched' energies and forces, not in line with what IFP wants nowadays
// Bring this to someone's attention if it is used, because it does not look consistent
// tjcw 20030827

  assert(0)  ;

// This is what 'dEdr' reporting should look like, i.e. no switch in the UDF
  aMDVM.ReportdEdr ( -DEDR ) ;

  if ( 0 == aMDVM.kSuppressVectorReporting )
  {

///////////////#if !defined(PERFORMANCE_SWITCH)
///////////////  XYZ    fab     = dVecAB *dVecABMagR * ( S * DEDR + dSdR * lje );
///////////////#else
  XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB) ;
  XYZ    fab     = dVecAB *dVecABMagR * ( S * DEDR - pdSdR * lje );
///////////////#endif

  aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
  aMDVM.ReportForce( MDVM::SiteB , - fab ) ;
}
  return;
  }

template
<class MDVM, class ParamsType, class DynVarMgrIF, class RTG>
static
inline
void
UDF_NSQLekner_Execute(MDVM &aMDVM, ParamsType &Params)
{
        UDF_HelperLekner<MDVM,DynVarMgrIF,RTG>::template pairwiseRotated<LeknerIncludeInvR>( aMDVM, Params.ChargeAxB );
        return;
}

template
<class MDVM, class ParamsType, class DynVarMgrIF, class RTG>
static
inline
void
UDF_NSQLeknerCorrection_Execute(MDVM &aMDVM, ParamsType &Params)
{
        UDF_HelperLekner<MDVM, DynVarMgrIF,  RTG>::template pairwiseRotated<LeknerExcludeInvR>( aMDVM, Params.ChargeAxB );
        return;
}

template
<class MDVM, class DynVarMgrIF, class RTG>
static
inline
void
UDF_LeknerSelfEnergy_Execute(MDVM &aMDVM)
{
	// for now this is just a place holder
        assert(0); 
}

template
<class MDVM, class ParamsType>
static
inline
void
UDF_ShakeGroup2_Execute(MDVM &aMDVM, ParamsType &Params)
{
    UDF_HelperGroup::ShakeRGroup<MDVM, 2>( aMDVM, Params.cparams );
    return;
}

template
<class MDVM, class ParamsType>
static
inline
void
UDF_ShakeGroup3_Execute(MDVM &aMDVM, ParamsType &Params)
{
    UDF_HelperGroup::ShakeRGroup<MDVM, 3>( aMDVM, Params.cparams );
    return;
}

template
<class MDVM, class ParamsType>
static
inline
void
UDF_ShakeGroup4_Execute(MDVM &aMDVM, ParamsType &Params)
{
    UDF_HelperGroup::ShakeRGroup<MDVM, 4>( aMDVM, Params.cparams );
    return;
}

template
<class MDVM, class ParamsType>
static
inline
void
UDF_RattleGroup4_Execute(MDVM &aMDVM, ParamsType &Params)
{
    UDF_HelperGroup::RattleRGroup<MDVM,4>( aMDVM, Params.cparams );
    return;
}

template
<class MDVM, class ParamsType>
static
inline
void
UDF_RattleGroup3_Execute(MDVM &aMDVM, ParamsType &Params)
{
    UDF_HelperGroup::RattleRGroup<MDVM, 3>( aMDVM, Params.cparams );
    return;
}

template
<class MDVM, class ParamsType>
static
inline
void
UDF_RattleGroup2_Execute(MDVM &aMDVM, ParamsType &Params)
{
    UDF_HelperGroup::RattleRGroup<MDVM, 2>( aMDVM, Params.cparams );
    return;
}

template
<class MDVM>
static
inline
void
UDF_AtomToFragmentVirialCorrection_Execute(MDVM &aMDVM)
{
  XYZ fragmentVirial;
  fragmentVirial.Zero();      

  for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
    {
///        XYZ force            = aMDVM.GetImpulse( SiteIndex );
        XYZ force            = aMDVM.GetForce( SiteIndex );
        XYZ siteDisplacement = aMDVM.GetPosition( SiteIndex ) - aMDVM.GetCenterOfMassPosition(); 
        fragmentVirial.mX  += force.mX * siteDisplacement.mX;
        fragmentVirial.mY  += force.mY * siteDisplacement.mY;
        fragmentVirial.mZ  += force.mZ * siteDisplacement.mZ;
    }
    aMDVM.ReportVirial( -fragmentVirial );
///    aMDVM.ReportVirialImpulse( -fragmentVirial );

  return;
}

template
<class MDVM>
static
inline
void
UDF_ZeroCenterOfMassForce_Execute(MDVM &aMDVM)
{
  aMDVM.ZeroCenterOfMassForce();
  return;
}

template
<class MDVM>
static
inline
void
UDF_UpdateCenterOfMassPosition_Execute(MDVM &aMDVM)
{
  double InverseMass = 2 * aMDVM.GetCenterOfMassHalfInverseMass();
  XYZ    SumPositionTimesMass;
  SumPositionTimesMass.Zero();

  for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
    {
    SumPositionTimesMass  += aMDVM.GetPosition( SiteIndex ) * aMDVM.GetMass( SiteIndex );
    }
  XYZ CenterOfMassPosition = SumPositionTimesMass * InverseMass;
  aMDVM.ReportCenterOfMassPosition( CenterOfMassPosition );

  return;
}

template
<class MDVM>
static
inline
void
UDF_UpdateCenterOfMassVelocity_Execute(MDVM &aMDVM)
{
  double InverseMass = 2 * aMDVM.GetCenterOfMassHalfInverseMass();
  XYZ    SumVelocityTimesMass;
  SumVelocityTimesMass.Zero();

  for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
    {
    SumVelocityTimesMass  += aMDVM.GetVelocity( SiteIndex ) * aMDVM.GetMass( SiteIndex );
    }
  XYZ CenterOfMassVelocity = SumVelocityTimesMass * InverseMass;
  aMDVM.ReportCenterOfMassVelocity( CenterOfMassVelocity );

  return;
}

template
<class MDVM>
static
inline
void
UDF_UpdateCenterOfMassForce_Execute(MDVM &aMDVM)
{
  aMDVM.ZeroCenterOfMassForce();

  for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )
    {
      // Adds the force of the site to the center of mass force on per respa level     
      aMDVM.AddForceToCenterOfMassForce( SiteIndex );    
    }
  return;
}

template
<class MDVM, class ParamsType>
static
inline
void
UDF_NSQEwaldRespaCorrection_Execute(MDVM &aMDVM, ParamsType &Params)
{
  // NOTE: This method assumes that distance based cutoffs are handled outside.

  // double q1 = A.Charge;
  // double q2 = B.Charge;

  // double Params.ChargeAxB = q1 * q2;
  // double Params.ChargeAxB = A.ChargeAxB;
  double alpha = aMDVM.GetEwaldAlpha();

  double dVecABMag2 = aMDVM.GetDistanceSquared( MDVM::SiteA, MDVM::SiteB ) ;
//  double dVecABMagR = 1.0 / sqrt(dVecABMag2);
  double dVecABMagR = aMDVM.GetReciprocalDistance( MDVM::SiteA, MDVM::SiteB ) ;
  double dVecABMag = dVecABMag2 * dVecABMagR;

//  double erfc = Math::erfc( alpha * dVecABMag );
//  double derfc = Math::derfc( alpha * dVecABMag );
  double erf;
  double derf;
  Math::erfccd( alpha * dVecABMag, erf, derf );
  erf = 1.0 - erf;
  derf = - derf;
  double qaqb_r = Params.ChargeAxB * dVecABMagR;

////////////////#if !defined(PERFORMANCE_SWITCH)
////////////////  UDF_Binding::SwitchStd5_Params sp;
////////////////  UDF_Binding::SwitchStd5_OutputParams op;
////////////////  sp.r = dVecABMag;
////////////////  UDF_Binding::Function_Param<UDF_Binding::SwitchStd5_Params, UDF_Binding::SwitchStd5_OutputParams> pin;
////////////////  pin.A = &sp;
////////////////  pin.B = &op;
//////////////////  UDF_Binding::UDF_Execute(UDF_Binding::SwitchStd5_Code, aMDVM, (void *)(&pin));
////////////////  UDF_Binding::UDF_SwitchStd5_Execute(aMDVM, sp, op);
////////////////  double S = op.S ;
////////////////  double dSdR = op.dSdR ;
////////////////#else
////////////////  double S ;
////////////////  double pdSdR ;
////////////////  SwitchFunction sf(RTG.mSwitchFunctionRadii) ;
////////////////#if defined(PERFORMANCE_EXPLOIT_FULLY_ON)
////////////////  sf.Evaluate<aMDVM.kForceFullyOn>(dVecABMag,S,pdSdR) ;
////////////////#else
////////////////  sf.Evaluate(dVecABMag,S,pdSdR) ;
////////////////#endif
////////////////#endif

  double energy = - qaqb_r * erf;
  aMDVM.ReportEnergy( energy );

  double dEdR   = - qaqb_r * ( erf*dVecABMagR - derf * alpha);

////#if !defined(PERFORMANCE_SWITCH)
///  XYZ fab = ( S * dEdR + dSdR * energy ) * unitVecAB;
///#else
//////////  XYZ fab = ( S * dEdR - pdSdR * energy ) * unitVecAB;
///#endif

  aMDVM.ReportdEdr ( -dEdR ) ;

// See if we can steer clear of vector-ness
  if ( 0 == aMDVM.kSuppressVectorReporting )
  {
    XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB ) ;
    XYZ unitVecAB = dVecAB * dVecABMagR;
    XYZ fab = unitVecAB * dEdR;
// Force reporting done under 'ReportdEdr', but virial reporting not done automatically
    aMDVM.ReportForce( MDVM::SiteA ,   fab ) ;
    aMDVM.ReportForce( MDVM::SiteB , - fab ) ;
// Only any point reporting virials if we are doing pressure control  
   // the (partial) virial here is 3Q( d/d alpha (energy) * d alpha/dQ ), d alpha/dQ = -alpha/3Q
    XYZ virial;
    virial.mX = virial.mY = virial.mZ = Params.ChargeAxB * derf * alpha * Math::OneThird;

    aMDVM.ReportVirial( - virial );
    aMDVM.ReportVVirial( PairwiseProduct(fab,dVecAB) ) ;
  }
  else
  {
// Report additional Ewald virial as a scalar
	aMDVM.ReportAdditionalScalarVirial(-Params.ChargeAxB * derf * alpha) ;
  }
}




template
<class MDVM>
static
inline
void
UDF_SwitchFunction_Execute(MDVM &aMDVM)
{
   double distance = aMDVM.GetDistance( MDVM::SiteA, MDVM::SiteB );
    double S ;
    double pdSdR ;
//   SwitchFunctionRadii sfr;
//   aMDVM.GetSwitchFunctionRadii( sfr );
//   SwitchFunction sf( sfr ) ;  
//   sf.Evaluate(distance, S, pdSdR) ;
    aMDVM.EvaluateSwitch(distance, S, pdSdR) ;
 
   aMDVM.ReportSwitch(S,pdSdR) ;   
   
   aMDVM.ApplySwitch() ;
}

template
<class MDVM>
static
inline
void
UDF_SwitchFunctionForce_Execute(MDVM &aMDVM)
{
   XYZ force = aMDVM.GetForce( MDVM::SiteA );     
   XYZ virial = aMDVM.GetVirial();     
   double energy = aMDVM.GetEnergy();                      
   double distance = aMDVM.GetDistance( MDVM::SiteA, MDVM::SiteB );
                     
   double S ;
   double pdSdR ;
//   SwitchFunctionRadii sfr;
//   aMDVM.GetSwitchFunctionRadii( sfr );
//   SwitchFunction sf( sfr ) ;  
//   sf.Evaluate(distance, S, pdSdR) ;
   aMDVM.EvaluateSwitch(distance, S, pdSdR) ;

//   XYZ dVecAB = aMDVM.GetVector( MDVM::SiteA, MDVM::SiteB );
//   double dVecABMagR = aMDVM.GetReciprocalDistance( MDVM::SiteA, MDVM::SiteB );
//   XYZ unitVector = dVecAB*dVecABMagR;

   //    XYZ ForceSwitched = force*S - unitVector*energy*pdSdR;
   XYZ ForceSwitched = force*S;
//   double EnergySwitched = energy * S;
   XYZ    VirialSwitched = virial * S;

   aMDVM.ReportEnergy( energy );
   aMDVM.ReportVirial( virial );
                      
   aMDVM.ReportForce( MDVM::SiteA,   ForceSwitched );
   aMDVM.ReportForce( MDVM::SiteB, - ForceSwitched );
// !!! tjcw caution; if we ever suport switch function force with Ewald scalar 
// virials, we will need some code here. But I think we would do it a different awy
}

template
<class MDVM>
static
inline
void
UDF_NoseHooverInit_Execute(MDVM &aMDVM)
{
  
 int NumberOfThermostats = aMDVM.GetNumberOfThermostats();
 int NumberOfSitesPerThermostat = aMDVM.GetNumberOfSitesPerThermostat();     

 // Nose-Hoover add-ons
 double kt1 = aMDVM.GetVelocityResampleTargetTemperature() * SciConst::KBoltzmann_IU;
 // double tau = 100 * aMDVM.GetDeltaT() * aMDVM.GetNoseHooverMassFactor();   
 double tau = aMDVM.GetProperTimeConstant() * aMDVM.GetNoseHooverMassFactor();
        
      
 int dim = aMDVM.GetFragmentDimension();        

 aMDVM.ReportDkT0( 0, dim * kt1 ); 

 int degOfFreedom = 3 * aMDVM.GetFragmentSiteCount() - 3 - aMDVM.GetFragmentConstraintCount();

// if( degOfFreedom == 0 )
//   degOfFreedom = 3;

 aMDVM.ReportDkT1( 0, degOfFreedom * kt1 );
          
  for(int j = 1; j < NumberOfSitesPerThermostat; j++)
    {
      aMDVM.ReportDkT0( j, kt1 ); 
      aMDVM.ReportDkT1( j, kt1 ); 
    }
   
  /****************************************************************
   *       Initialize Thermostat Masses Positions Velocities
   ****************************************************************/
   for(int thermostat=0; thermostat  < NumberOfThermostats ; thermostat++)
    {   
      for(int inhc=0; inhc < NumberOfSitesPerThermostat ; inhc++)
	{
          // double kt = _dkt[thermostat][inhc];
          double kt = aMDVM.GetDkt( thermostat, inhc );
          // double tau = aMDVM.GetTAU();

          // _thermostatMasses[thermostat][inhc] = kt * tau  * tau;
          double thermostatMass = kt * tau  * tau;
          aMDVM.ReportThermostatMass( thermostat, inhc, thermostatMass );
	}
    }        
   /****************************************************************/
}

template
<class MDVM>
static
inline
void
UDF_UpdateFragmentKEForNoseHoover_Execute(MDVM &aMDVM)
{

//    double fragKE = 0.0;    
//     
//    for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ )      
//    {
//        XYZ vel = aMDVM.GetVelocity( SiteIndex );
//        fragKE += 0.5 * aMDVM.GetMass( SiteIndex ) * vel * vel;
//    }
//
//    XYZ    COMVelocity = aMDVM.GetCenterOfMassVelocity();
//    double COMMass     = aMDVM.GetCenterOfMassMass();
//    double COMKE = 0.5 * COMMass * COMVelocity * COMVelocity;

    double fragKE = aMDVM.EvaluateFragmentKineticEnergy();
    double COMKE = aMDVM.EvaluateFragmentCOMKineticEnergy();

    aMDVM.ReportFragmentKineticEnergy ( fragKE );      
    aMDVM.ReportFragmentCOMKineticEnergy ( COMKE );      

    return;
}

template
<class MDVM>
static
inline
void
UDF_NoseHooverIntegration_Execute(MDVM &aMDVM)
{
  
   int NumberOfThermostats        = aMDVM.GetNumberOfThermostats();     
   int NumberOfSitesPerThermostat = aMDVM.GetNumberOfSitesPerThermostat();     
   int NumberOfNHRespaLevels      = aMDVM.GetNumberOfNHRespaLevels();
   int NumberOfYoshParams         = NHC::NYOSH;     

   double kineticEnergy[ NHC::NUMBER_OF_THERMOSTATS ];     
   kineticEnergy[ 0 ] = aMDVM.GetFragmentCOMKineticEnergy();
   kineticEnergy[ 1 ] = aMDVM.GetFragmentKineticEnergy() - kineticEnergy[ 0 ];

   double scale[ NHC::NUMBER_OF_THERMOSTATS ];
   scale[ 0 ] = 1.0;  
   scale[ 1 ] = 1.0;  
   
 /****************************************************************
  *      Update Thermostat Forces
  ****************************************************************/       
  for(int thermostat = 0; thermostat < NumberOfThermostats; thermostat++)
    {
        
      // _thermostatForces[thermostat][0] =
      // (2*_particleKineticEnergy[thermostat]-_dkt[thermostat][0])
      //                  /_thermostatMasses[thermostat][0];

      double thermostatForce = ( 2 * kineticEnergy[ thermostat ] -  aMDVM.GetDkt( thermostat, 0 ) )
                                  / aMDVM.GetThermostatMass( thermostat , 0 );
        
      aMDVM.ReportThermostatForce( thermostat, 0, thermostatForce );

      for(int inhc=1; inhc < NumberOfSitesPerThermostat; inhc++)
	{
	  // _thermostatForces[thermostat][inhc] =
	  //   (_thermostatMasses[thermostat][inhc-1]
	  //   *_thermostatVelocities[thermostat][inhc-1]
	  //   *_thermostatVelocities[thermostat][inhc-1]
	  //   - _dkt[thermostat][inhc])/_thermostatMasses[thermostat][inhc];

          double tVel = aMDVM.GetThermostatVelocity( thermostat, inhc-1 );
          double tMass_1 = aMDVM.GetThermostatMass( thermostat, inhc-1 );
          double tMass = aMDVM.GetThermostatMass( thermostat, inhc );
          double dtk  =  aMDVM.GetDkt( thermostat, inhc );
          double tForce =  ( tMass_1 * tVel * tVel - dtk ) / tMass ;

          aMDVM.ReportThermostatForce( thermostat, inhc, tForce );
	}
    }
   /****************************************************************/

  
   /****************************************************************
    *  Propagate Nose-Hoover chain dynamics 
    ****************************************************************/
    for(int thermostat = 0; thermostat < NumberOfThermostats; thermostat++)
      {
        // aMDVM.ReportNHScaling( thermostat,  1.0 ); 
        // _scale[thermostat]=1.0;
        // scale[ thermostat ] = 1.0;  
 

        for(int respStep=0; respStep < NumberOfNHRespaLevels ; respStep++)
          {
            for(int yosh=0; yosh < NumberOfYoshParams; yosh++)
              {
   	        // loop over the thermostates 
                /********************************************************
	        * updateFirstHalfThermostatVelocities(yosh,thermostat);
                *********************************************************/
                //  _thermostatVelocities[thermostat][_INHC-1] += 
                //    _thermostatForces[thermostat][_INHC-1]*_wdti4[yosh];
          
                // for(int inhc=_INHC-2; inhc>=0;  inhc--)
                //    {
                //      double aa =exp(-_wdti8[yosh]*_thermostatVelocities[thermostat][inhc+1]);
                //      _thermostatVelocities[thermostat][inhc] = 
                // 	   _thermostatVelocities[thermostat][inhc]*aa*aa
                //          +_wdti4[yosh]*_thermostatForces[thermostat][inhc]*aa;
                //     } 
                double tVel_last = aMDVM.GetThermostatVelocity( thermostat, NumberOfSitesPerThermostat-1 );
                double tForce_last = aMDVM.GetThermostatForce( thermostat, NumberOfSitesPerThermostat-1 );  
                double tVel_last_new = tVel_last + tForce_last * NHC::wdti4[ yosh ];        

                aMDVM.ReportThermostatVelocity( thermostat, NumberOfSitesPerThermostat-1, tVel_last_new );

                for(int inhc=NumberOfSitesPerThermostat-2; inhc>=0;  inhc--)
                  {
                    double tVel_p1 = aMDVM.GetThermostatVelocity( thermostat, inhc+1 );
                    double aa = exp( -NHC::wdti8[ yosh ] * tVel_p1 );
                    double tVel = aMDVM.GetThermostatVelocity( thermostat, inhc );    
                    double tForce = aMDVM.GetThermostatForce( thermostat, inhc );    
                    double tVel_new = tVel * aa * aa + NHC::wdti4[yosh] * tForce * aa;

                    aMDVM.ReportThermostatVelocity(thermostat, inhc, tVel_new );
                  }
                /*****************************************************************/

                /****************************************************************
	        * calcRespParticleVelocitiesScale(yosh,thermostat);
                *****************************************************************/
                // double aa = exp(-_wdti2[yosh]*_thermostatVelocities[thermostat][0]);
                // _scale[thermostat] *= aa;
                double tVel_first =  aMDVM.GetThermostatVelocity( thermostat, 0 );
                double aa = exp(-NHC::wdti2[yosh] * tVel_first);
                scale[ thermostat ] *= aa;
	       /*****************************************************************/
	      	       
                /****************************************************************
	        *  updateThermostatPositions(yosh,thermostat);
                *****************************************************************/
                //for(int inhc=0; inhc<_INHC; inhc++)
                //  {
                //    _thermostatPositions[thermostat][inhc] += 
           	//    _thermostatVelocities[thermostat][inhc]*_wdti2[yosh];
                //  }
                for(int inhc=0; inhc < NumberOfSitesPerThermostat; inhc++)
                  {
                    double tPos = aMDVM.GetThermostatPosition( thermostat, inhc );
                    double tVel = aMDVM.GetThermostatVelocity( thermostat, inhc );
                    double tPos_new = tPos + tVel * NHC::wdti2[ yosh ];
                    aMDVM.ReportThermostatPosition(thermostat, inhc, tPos_new);
                  }
                /*****************************************************************/

                /****************************************************************
	        *  update thermostat forces
                *****************************************************************/
	        //_thermostatForces[thermostat][0] =
		//    (_scale[thermostat]*_scale[thermostat]
  		//     *2.*_particleKineticEnergy[thermostat]
		//     -_dkt[thermostat][0])/_thermostatMasses[thermostat][0];
                double tForce = ( 2 * scale[ thermostat ] * scale[ thermostat ] * kineticEnergy[ thermostat ] 
                                -  aMDVM.GetDkt( thermostat, 0 ) )
                                / aMDVM.GetThermostatMass( thermostat , 0 );
                aMDVM.ReportThermostatForce( thermostat, 0, tForce );                                       
                /*****************************************************************/

                /****************************************************************
	        * updateSecondHalfThermostatVelocities(yosh,thermostat);
                *****************************************************************/
                //for(int inhc=0; inhc<_INHC-1; inhc++)
                //  {
                //    double aa = exp(-_wdti8[yosh]*_thermostatVelocities[thermostat][inhc+1]);
      
                //   _thermostatVelocities[thermostat][inhc] =
              	//           _thermostatVelocities[thermostat][inhc]*aa*aa
	        //         + _wdti4[yosh]*_thermostatForces[thermostat][inhc]*aa;
      
                //  _thermostatForces[thermostat][inhc+1] =
              	//          (_thermostatMasses[thermostat][inhc]
                //      	  *_thermostatVelocities[thermostat][inhc]
                //          * _thermostatVelocities[thermostat][inhc]
 	        //          -_dkt[thermostat][inhc+1])/_thermostatMasses[thermostat][inhc+1];
                //  }

                // _thermostatVelocities[thermostat][_INHC-1] += 
                // _thermostatForces[thermostat][_INHC-1]*_wdti4[yosh]; 
                for(int inhc=0; inhc < NumberOfSitesPerThermostat-1; inhc++)
                  {
                    double tVel_p1 = aMDVM.GetThermostatVelocity( thermostat, inhc+1 );
                    double aa1 = exp( -NHC::wdti8[ yosh ] * tVel_p1 );
                    double tVel = aMDVM.GetThermostatVelocity( thermostat, inhc );    
                    double tForce = aMDVM.GetThermostatForce( thermostat, inhc );    
                    double tVel_new = tVel * aa1 * aa1 + NHC::wdti4[yosh] * tForce * aa1;

                    aMDVM.ReportThermostatVelocity(thermostat, inhc, tVel_new );

                    double tMass     = aMDVM.GetThermostatMass( thermostat, inhc );     
                    double tMass_p1  = aMDVM.GetThermostatMass( thermostat, inhc+1 );     
                    double dkt_p1    = aMDVM.GetDkt( thermostat, inhc+1 ); 
                    double tForce_p1 = ( tMass * tVel_new * tVel_new - dkt_p1 ) / tMass_p1;
                    aMDVM.ReportThermostatForce(thermostat, inhc+1, tForce_p1 );  
                  }
                  
                double tVel_last1     = aMDVM.GetThermostatVelocity( thermostat, NumberOfSitesPerThermostat-1 );
                double tForce_last1   = aMDVM.GetThermostatForce( thermostat, NumberOfSitesPerThermostat-1 );
                double tVel_last_new1 = tVel_last1 + tForce_last1 * NHC::wdti4[yosh];
                aMDVM.ReportThermostatVelocity(thermostat, NumberOfSitesPerThermostat-1, tVel_last_new1 );  
                /*****************************************************************/
              }
           }
        }
      /****************************************************************/


  /****************************************************************
  * Update Particle Velocity
  ****************************************************************/       
  //TYPE particleCMVelocity;
  //(*_ms).getFragmentCMVelocity(particleCMVelocity);
  // getNumOfParticles(numOfParticles);

  //for(int particle=0; particle<numOfParticles; particle++)
  //  {
  //    double particleVelocity;
  //    (*_ms).getParticleVelocity(particleVelocity, particle);
  //   
  //    particleVelocity =(particleVelocity-particleCMVelocity)
  //	*_scale[1] + particleCMVelocity*_scale[0];
  //      (*_ms).putParticleVelocity(particleVelocity, particle);
  //  }
  
 const XYZ    vCOM = aMDVM.GetCenterOfMassVelocity(); 

 if( aMDVM.GetFragmentSiteCount() == 1 )
   {
     XYZ vSite = vCOM * scale[ 0 ];
     aMDVM.ReportVelocity( 0, vSite );        
   }  
 else
   {
     for( int SiteIndex = 0; SiteIndex < aMDVM.GetFragmentSiteCount(); SiteIndex++ ) 
       {
       XYZ vSite  = aMDVM.GetVelocity( SiteIndex );
       vSite = ( vSite - vCOM ) * scale[ 1 ] + vCOM * scale[ 0 ];
       aMDVM.ReportVelocity( SiteIndex, vSite );
       }
   }
  /****************************************************************/
}

template
<class MDVM>
static
inline
void
UDF_NoseHooverEnergy_Execute(MDVM &aMDVM)
{
  double NoseHooverEnergy = 0.0;

  int NumberOfThermostats        = aMDVM.GetNumberOfThermostats();
  int NumberOfSitesPerThermostat = aMDVM.GetNumberOfSitesPerThermostat();     

  for(int thermostat = 0; thermostat < NumberOfThermostats; thermostat++)
    {
      for(int inhc=0; inhc < NumberOfSitesPerThermostat; inhc++)
	{
           double mass = aMDVM.GetThermostatMass( thermostat, inhc );
           double vel = aMDVM.GetThermostatVelocity( thermostat, inhc );
           double pos = aMDVM.GetThermostatPosition( thermostat, inhc );
           double dkt = aMDVM.GetDkt( thermostat, inhc );
              
           double potNH = pos * dkt;           
       
           NoseHooverEnergy += ( 0.5 * mass * vel * vel + potNH );
        }
    }

   aMDVM.ReportEnergy( NoseHooverEnergy );

   return;
}


template
<class MDVM>
static
inline
void
UDF_NoseHooverInitThermostatPosVel_Execute(MDVM &aMDVM)
{
  
 int NumberOfThermostats = aMDVM.GetNumberOfThermostats();
 int NumberOfSitesPerThermostat = aMDVM.GetNumberOfSitesPerThermostat();     

 // Nose-Hoover add-ons
 double kt1 = aMDVM.GetVelocityResampleTargetTemperature() * SciConst::KBoltzmann_IU;
   
  /****************************************************************
   *       Initialize Thermostat Masses Positions Velocities
   ****************************************************************/
   for(int thermostat=0; thermostat  < NumberOfThermostats ; thermostat++)
    {   
      for(int inhc=0; inhc < NumberOfSitesPerThermostat ; inhc++)
	{
          double mass = aMDVM.GetThermostatMass(  thermostat, inhc );

          double thermostatPosition = 0.0;
          aMDVM.ReportThermostatPosition( thermostat, inhc, thermostatPosition );
          // _thermostatPositions[thermostat][inhc] = 0.;

          // Sample from Maxwell Boltzman: multiply by gaussian random number
          //_thermostatVelocities[thermostat][inhc] = 1.;        

          // double thermostatVelocity = 1.0;
          double thermostatVelocity = sqrt( kt1 / mass ) *  aMDVM.GetGaussRandomNumber();
          aMDVM.ReportThermostatVelocity( thermostat, inhc, thermostatVelocity );
	}
    }        
   /****************************************************************/
}

template
<class MDVM, class ParamsType>
static
inline
void
UDF_COMRestraint_Execute(MDVM &aMDVM, ParamsType &Params)
{
  XYZ SumPositionTimesMass;

  XYZ CenterOfMassPosition[ 2 ];
  double TotalMass[2];
      
  for( int MolOrd = 0; MolOrd < 2; MolOrd++ )
    {
    SumPositionTimesMass.Zero();
    TotalMass[ MolOrd ] = 0.0;
    for( int SiteIndex = 0; SiteIndex < aMDVM.GetMoleculeSiteCount( MolOrd ); SiteIndex++ )
      {
      double SiteMass = aMDVM.GetMass( MolOrd, SiteIndex );
      
      XYZ Pos = aMDVM.GetPosition( MolOrd, SiteIndex );
      
      SumPositionTimesMass  +=  Pos * SiteMass;
      TotalMass[ MolOrd ] += SiteMass;
      }
    
    CenterOfMassPosition[ MolOrd ] = SumPositionTimesMass / TotalMass[ MolOrd ];
    }

  XYZ DimensionVector = aMDVM.GetDimensionVector();

  XYZ DiffCenterOfMassPosition = CenterOfMassPosition[ 0 ] - CenterOfMassPosition[ 1 ];

  // Image X
  while( DiffCenterOfMassPosition.mX > 0.5 * DimensionVector.mX )
    {
    DiffCenterOfMassPosition.mX -= DimensionVector.mX;
    }

  while( DiffCenterOfMassPosition.mX < -0.5 * DimensionVector.mX )
    {
    DiffCenterOfMassPosition.mX += DimensionVector.mX;
    }

  // Image Y
  while( DiffCenterOfMassPosition.mY > 0.5 * DimensionVector.mY )
    {
    DiffCenterOfMassPosition.mY -= DimensionVector.mY;
    }

  while( DiffCenterOfMassPosition.mY < -0.5 * DimensionVector.mY )
    {
    DiffCenterOfMassPosition.mY += DimensionVector.mY;
    }

  // Image Z
  while( DiffCenterOfMassPosition.mZ > 0.5 * DimensionVector.mZ )
    {
    DiffCenterOfMassPosition.mZ -= DimensionVector.mZ;
    }

  while( DiffCenterOfMassPosition.mZ < -0.5 * DimensionVector.mZ )
    {
    DiffCenterOfMassPosition.mZ += DimensionVector.mZ;
    }
  
  XYZ fz = -Params.k * DiffCenterOfMassPosition;

  XYZ force;
  force.Zero();

  for( int SiteIndex = 0; SiteIndex < aMDVM.GetMoleculeSiteCount( 0 ); SiteIndex++ )
    {
    force = fz * aMDVM.GetMass( 0, SiteIndex ) / TotalMass[ 0 ];
    aMDVM.ReportForce( 0, SiteIndex, force );  // This should add to the net force on the site
    }

  for( int SiteIndex = 0; SiteIndex < aMDVM.GetMoleculeSiteCount( 1 ); SiteIndex++ )
    {
    force = -1.0 * fz * aMDVM.GetMass( 1, SiteIndex ) / TotalMass[ 1 ];
    aMDVM.ReportForce( 1, SiteIndex, force );  // This should add to the net force on the site
    }
  
  double DistanceSqr = DiffCenterOfMassPosition.LengthSquared();

  double E = 0.5 * Params.k * DistanceSqr;
  
  aMDVM.ReportEnergy( E );

  return;
}


};

#ifndef MSD_COMPILE_CODE_ONLY
char UDF_Binding::mTokenBuffer[UDF_Binding::TOKENBUFFERSIZE];

UDF_Binding::RegistryRecord UDF_Binding::UDF_RegistryList[] =
{
    {0, "Default"},
    {1, "ReportsEnergy:Harmonic:UpdateForce"},
    {2, "ReportsEnergy:Harmonic:UpdateForce"},
    {3, "ReportsEnergy:Torsion:UpdateForce"},
    {4, "ReportsEnergy:Harmonic:UpdateForce"},
    {5, "ReportsEnergy:Torsion:UpdateForce"},
    {6, "ReportsEnergy:Torsion:UpdateForce"},
    {7, "ReportsEnergy:Torsion:UpdateForce"},
    {8, "ReportsEnergy:NonBonded:UpdateForce"},
    {9, "ReportsEnergy:NonBonded:UpdateForce"},
    {10, "ReportsEnergy:Harmonic:UpdateForce"},
    {11, "ReportsEnergy:Harmonic:UpdateEnergy"},
    {12, "Harmonic:UpdateVelocityFirstHalfStep"},
    {13, "Harmonic:UpdateVelocitySecondHalfStep"},
    {14, "Harmonic:UpdateVirial"},
    {15, "Harmonic:UpdateVirial"},
    {16, "Harmonic:UpdateVelocityFirstHalfStep"},
    {17, "Harmonic:UpdatePosition"},
    {18, "Harmonic:UpdateVelocitySecondHalfStep"},
    {19, "Harmonic:UpdateVirial"},
    {20, "ReportsEnergy:Harmonic:UpdateForce"},
    {21, "ReportsEnergy:NonBonded:UpdateForce"},
    {22, "ReportsEnergy:NonBonded:UpdateForce"},
    {23, "ReportsEnergy:NonBonded:UpdateForce"},
    {24, "ReportsEnergy:NonBonded:UpdateForce"},
    {25, "ReportsEnergy:NonBonded:UpdateForce"},
    {26, "Harmonic:UpdateVelocityFirstHalfStep"},
    {27, "Harmonic:UpdateVelocitySecondHalfStep"},
    {28, "Harmonic:UpdateVelocityFirstHalfStep"},
    {29, "Harmonic:UpdateVelocitySecondHalfStep"},
    {30, "ReportsEnergy:NonBonded:NSQ1:UpdateForce"},
    {31, "ReportsEnergy:NonBonded:NSQ1:UpdateForce"},
    {32, "PairTransform"},
    {33, "Function"},
    {34, "ReportsEnergy:NonBonded:UpdateForce"},
    {35, "ReportsEnergy:NonBonded:UpdateForce"},
    {36, "ReportsEnergy:NonBonded:UpdateForce"},
    {37, "WaterInit"},
    {38, "WaterInit"},
    {39, "PairTransform"},
    {40, "Init"},
    {41, "ReportsEnergy:Harmonic"},
    {42, "ReportsEnergy:Harmonic"},
    {43, "Harmonic:UpdateVolumeSecondHalfStep"},
    {44, "Harmonic"},
    {45, "Harmonic"},
    {46, "Harmonic"},
    {47, "ReportsEnergy:Harmonic"},
    {48, "Harmonic"},
    {49, "ReportsEnergy:NonBonded:UpdateForce"},
    {50, "ReportsEnergy:NonBonded:UpdateForce"},
    {51, "ReportsEnergy:NonBonded:NSQ1:UpdateForce"},
    {52, "ReportsEnergy:NonBonded:UpdateForce"},
    {53, "ReportsEnergy:NonBonded:UpdateForce"},
    {54, "Default"},
    {55, "Harmonic:UpdateVelocityFirstHalfStep"},
    {56, "Harmonic:UpdateVelocityFirstHalfStep"},
    {57, "Harmonic:UpdateVelocityFirstHalfStep"},
    {58, "Harmonic:UpdateVelocitySecondHalfStep"},
    {59, "Harmonic:UpdateVelocitySecondHalfStep"},
    {60, "Harmonic:UpdateVelocitySecondHalfStep"},
    {61, "Harmonic"},
    {62, "Harmonic"},
    {63, "Harmonic"},
    {64, "Harmonic"},
    {65, "Harmonic"},
    {66, "ReportsEnergy:NonBonded:UpdateForce"},
    {67, "NonBonded"},
    {68, "NonBonded"},
    {69, "Harmonic"},
    {70, "Harmonic"},
    {71, "Harmonic"},
    {72, "Harmonic"},
    {73, "Harmonic"},
    {74, "Harmonic"}
};
#endif

#endif
