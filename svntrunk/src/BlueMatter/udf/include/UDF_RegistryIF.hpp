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
 #ifndef UDF_REGISTRYIF_HPP
#define UDF_REGISTRYIF_HPP

//  DO NOT EDIT     DO NOT EDIT    DO NOT EDIT    DO NOT EDIT     DO NOT EDIT     DO NOT EDIT     DO NOT EDIT
//  DO NOT EDIT     DO NOT EDIT    DO NOT EDIT    DO NOT EDIT     DO NOT EDIT     DO NOT EDIT     DO NOT EDIT
//  DO NOT EDIT     DO NOT EDIT    DO NOT EDIT    DO NOT EDIT     DO NOT EDIT     DO NOT EDIT     DO NOT EDIT
//
//  This file was automatically created by the script UDFRegister.pl, which constructs the UDF_RegistryIF class
//  using the contents of the UDF registry.
//
//  If changes are needed to this file, please communicate them to your local UDF control authority.
//

// #include <BMStuff.hpp>
#include <assert.h>
#include <string.h>
#include <stdio.h>

class UDF_RegistryIF
{
private:

    struct RegistryRecord
    {
        int    mCode;
        char   *mName;
        char   *mDate;
        char   *mTime;
        int    mArgCheckSum;
        int    mBodyCheckSum;
        int    mSiteCount;
        int    mParamCount;
        char   *mCategories;
        char   *mVariables;
        char   *mOutputVariables;
    };

    enum { TOKENBUFFERSIZE = 1024 };
    enum { STRUCTUREBUFFERSIZE = TOKENBUFFERSIZE + 256 };

    static char mTokenBuffer[TOKENBUFFERSIZE];
    static char mStructureBuffer[STRUCTUREBUFFERSIZE];

    static RegistryRecord UDF_RegistryList[];

    enum { NUMENTRIES = 75 };

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

public:

    static inline int GetParamSizeByUDFCode(const int Code)
    {
        switch (Code)
        {
        case DefaultForce_Code:
            {
                struct DefaultForce_LocalStruct
                {
                    int dummy;
                };
                return sizeof(DefaultForce_LocalStruct);
            }
            break;
        case StdHarmonicBondForce_Code:
            {
                struct StdHarmonicBondForce_LocalStruct
                {
                    double k;
                    double r0;
                };
                return sizeof(StdHarmonicBondForce_LocalStruct);
            }
            break;
        case StdHarmonicAngleForce_Code:
            {
                struct StdHarmonicAngleForce_LocalStruct
                {
                    double k;
                    double th0;
                };
                return sizeof(StdHarmonicAngleForce_LocalStruct);
            }
            break;
        case SwopeTorsionForce_Code:
            {
                struct SwopeTorsionForce_LocalStruct
                {
                    double cosDelta;
                    double k;
                    double sinDelta;
                    int n;
                };
                return sizeof(SwopeTorsionForce_LocalStruct);
            }
            break;
        case UreyBradleyForce_Code:
            {
                struct UreyBradleyForce_LocalStruct
                {
                    double k;
                    double s0;
                };
                return sizeof(UreyBradleyForce_LocalStruct);
            }
            break;
        case OPLSTorsionForce_Code:
            {
                struct OPLSTorsionForce_LocalStruct
                {
                    double cosDelta;
                    double k;
                    double sinDelta;
                    int n;
                };
                return sizeof(OPLSTorsionForce_LocalStruct);
            }
            break;
        case OPLSImproperForce_Code:
            {
                struct OPLSImproperForce_LocalStruct
                {
                    double cosDelta;
                    double k;
                    double sinDelta;
                    int n;
                };
                return sizeof(OPLSImproperForce_LocalStruct);
            }
            break;
        case ImproperDihedralForce_Code:
            {
                struct ImproperDihedralForce_LocalStruct
                {
                    double k;
                    double psi0;
                };
                return sizeof(ImproperDihedralForce_LocalStruct);
            }
            break;
        case LennardJonesForce_Code:
            {
                struct LennardJonesForce_LocalStruct
                {
                    double epsilon;
                    double sigma;
                };
                return sizeof(LennardJonesForce_LocalStruct);
            }
            break;
        case StdChargeForce_Code:
            {
                struct StdChargeForce_LocalStruct
                {
                    double ChargeAxB;
                };
                return sizeof(StdChargeForce_LocalStruct);
            }
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
            {
                struct EwaldRealSpaceForce_LocalStruct
                {
                    double ChargeAxB;
                    double alpha;
                };
                return sizeof(EwaldRealSpaceForce_LocalStruct);
            }
            break;
        case EwaldStructureFactors_Code:
            {
                struct EwaldStructureFactors_LocalStruct
                {
                    double charge;
                    double kx;
                    double ky;
                    double kz;
                };
                return sizeof(EwaldStructureFactors_LocalStruct);
            }
            break;
        case EwaldKSpaceForce_Code:
            return 0;
            break;
        case EwaldSelfEnergyForce_Code:
            return 0;
            break;
        case EwaldCorrectionForce_Code:
            {
                struct EwaldCorrectionForce_LocalStruct
                {
                    double ChargeAxB;
                    double alpha;
                };
                return sizeof(EwaldCorrectionForce_LocalStruct);
            }
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
            {
                struct NSQLennardJones_LocalStruct
                {
                    double epsilon;
                    double sigma;
                };
                return sizeof(NSQLennardJones_LocalStruct);
            }
            break;
        case NSQCoulomb_Code:
            {
                struct NSQCoulomb_LocalStruct
                {
                    double ChargeAxB;
                };
                return sizeof(NSQCoulomb_LocalStruct);
            }
            break;
        case LJStdCombiner_Code:
            {
                struct LJStdCombiner_LocalStruct
                {
                    double epsilon;
                    double sigma;
                };
                return sizeof(LJStdCombiner_LocalStruct);
            }
            break;
        case SwitchStd5_Code:
            {
                struct SwitchStd5_LocalStruct
                {
                    double r;
                };
                return sizeof(SwitchStd5_LocalStruct);
            }
            break;
        case NSQEwaldCorrection_Code:
            {
                struct NSQEwaldCorrection_LocalStruct
                {
                    double ChargeAxB;
                };
                return sizeof(NSQEwaldCorrection_LocalStruct);
            }
            break;
        case NSQEwaldRealSpace_Code:
            {
                struct NSQEwaldRealSpace_LocalStruct
                {
                    double ChargeAxB;
                };
                return sizeof(NSQEwaldRealSpace_LocalStruct);
            }
            break;
        case Coulomb14_Code:
            {
                struct Coulomb14_LocalStruct
                {
                    double ChargeAxB;
                };
                return sizeof(Coulomb14_LocalStruct);
            }
            break;
        case WaterTIP3PInit_Code:
            return 0;
            break;
        case WaterSPCInit_Code:
            return 0;
            break;
        case LJOPLSCombiner_Code:
            {
                struct LJOPLSCombiner_LocalStruct
                {
                    double epsilon;
                    double sigma;
                };
                return sizeof(LJOPLSCombiner_LocalStruct);
            }
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
            {
                struct LennardJones14Force_LocalStruct
                {
                    double epsilon;
                    double sigma;
                };
                return sizeof(LennardJones14Force_LocalStruct);
            }
            break;
        case OPLSLennardJones14Force_Code:
            {
                struct OPLSLennardJones14Force_LocalStruct
                {
                    double epsilon;
                    double sigma;
                };
                return sizeof(OPLSLennardJones14Force_LocalStruct);
            }
            break;
        case NSQOPLSLennardJones_Code:
            {
                struct NSQOPLSLennardJones_LocalStruct
                {
                    double epsilon;
                    double sigma;
                };
                return sizeof(NSQOPLSLennardJones_LocalStruct);
            }
            break;
        case NSQLekner_Code:
            {
                struct NSQLekner_LocalStruct
                {
                    double ChargeAxB;
                };
                return sizeof(NSQLekner_LocalStruct);
            }
            break;
        case NSQLeknerCorrection_Code:
            {
                struct NSQLeknerCorrection_LocalStruct
                {
                    double ChargeAxB;
                };
                return sizeof(NSQLeknerCorrection_LocalStruct);
            }
            break;
        case LeknerSelfEnergy_Code:
            return 0;
            break;
        case ShakeGroup2_Code:
            {
                struct ShakeGroup2_LocalStruct
                {
                    ConstraintGroupParams<2> cparams;
                };
                return sizeof(ShakeGroup2_LocalStruct);
            }
            break;
        case ShakeGroup3_Code:
            {
                struct ShakeGroup3_LocalStruct
                {
                    ConstraintGroupParams<3> cparams;
                };
                return sizeof(ShakeGroup3_LocalStruct);
            }
            break;
        case ShakeGroup4_Code:
            {
                struct ShakeGroup4_LocalStruct
                {
                    ConstraintGroupParams<4> cparams;
                };
                return sizeof(ShakeGroup4_LocalStruct);
            }
            break;
        case RattleGroup4_Code:
            {
                struct RattleGroup4_LocalStruct
                {
                    ConstraintGroupParams<4> cparams;
                };
                return sizeof(RattleGroup4_LocalStruct);
            }
            break;
        case RattleGroup3_Code:
            {
                struct RattleGroup3_LocalStruct
                {
                    ConstraintGroupParams<3> cparams;
                };
                return sizeof(RattleGroup3_LocalStruct);
            }
            break;
        case RattleGroup2_Code:
            {
                struct RattleGroup2_LocalStruct
                {
                    ConstraintGroupParams<2> cparams;
                };
                return sizeof(RattleGroup2_LocalStruct);
            }
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
            {
                struct NSQEwaldRespaCorrection_LocalStruct
                {
                    double ChargeAxB;
                };
                return sizeof(NSQEwaldRespaCorrection_LocalStruct);
            }
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
            {
                struct COMRestraint_LocalStruct
                {
                    double k;
                    double z0;
                };
                return sizeof(COMRestraint_LocalStruct);
            }
            break;
        default:
            BegLogLine(1)
                 << "GetParamSizeByUDFCode failed - Code " << Code << " not found"
                 << EndLogLine ;
            assert(0);
            break;
        }
        return -1;
    }

    enum {CODENOTFOUND = -1};
    enum {ARGNOTFOUND = -1};

private:

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

    // Go through all codes rather than assume they are sorted...
    static inline void GetMaxUDFCode(int &code)
    {
        assert(NUMENTRIES > 0);
        code = -1;
        for (int i=0; i<NUMENTRIES; i++)
            if (code < UDF_RegistryList[i].mCode)
                code = UDF_RegistryList[i].mCode;
    }

    static inline void GetUDFCodeByName(const char *name, int &code)
    {
        for (int i=0; i<NUMENTRIES; i++) {
            if (!strcmp(name, UDF_RegistryList[i].mName)) {
                code = UDF_RegistryList[i].mCode;
                return;
            }
        }
        code = CODENOTFOUND;
        printf("GetUDFCodeByName failed - CodeName %s not found\n", name);
        assert(0);
    }

    static inline void GetNSitesByUDFCode(const int code, int &num)
    {
        int i = GetIndexByUDFCode(code);

        if (i == CODENOTFOUND) {
            num = CODENOTFOUND;
            printf("GetNSitesByUDFCode failed - code %d not found\n", code);
            assert(0);
        }

        num = UDF_RegistryList[code].mSiteCount;
    }

    static inline void GetParamCountByUDFCode(const int code, int &count)
    {
        int i = GetIndexByUDFCode(code);

        if (i == CODENOTFOUND) {
            count = CODENOTFOUND;
            printf("GetParamCountByUDFCode failed - code %d not found\n", code);
            assert(0);
        }

        count = UDF_RegistryList[code].mParamCount;
    }

    // The returned string is volatile and should be used immediatly or copied locally
    static inline void GetArgByName(const int code, const char *name, int &ord, char* &type)
    {
        int i = GetIndexByUDFCode(code);

        if (i == CODENOTFOUND) {
            ord = ARGNOTFOUND;
            type = NULL;
            printf("GetArgByName failed - code %d not found\n", code);
            assert(0);
        }

        ord = 0;
        assert(strlen(UDF_RegistryList[i].mVariables) < TOKENBUFFERSIZE);
        strcpy(mTokenBuffer, UDF_RegistryList[i].mVariables);
        char *p = strtok(mTokenBuffer, ":");
        while (p) {
            type = p;
            p = strtok(NULL, ":");
            if (p && !strcmp(p, name))
                break;
            ord++;
            p = strtok(NULL, ":");
        }
        if (!p) {
            ord = ARGNOTFOUND;
            type = NULL;
            printf("GetArgByName failed - ArgName %s not found\n", name);
            assert(0);
        }
    }

    static inline void GetArgCountByUDFCode(const int code, int &count)
    {
        int i = GetIndexByUDFCode(code);

        if (i == CODENOTFOUND) {
            count = CODENOTFOUND;
            printf("GetArgCountByCode failed - code %d not found\n", code);
            assert(0);
        }

        count = 0;
        assert(strlen(UDF_RegistryList[i].mVariables) < TOKENBUFFERSIZE);
        strcpy(mTokenBuffer, UDF_RegistryList[i].mVariables);
        char *p = strtok(mTokenBuffer, ":");
        while (p) {
            if (!strcmp(p, "EMPTY"))
                break;
            p = strtok(NULL, ":");
            count++;
            if (p)
                p = strtok(NULL, ":");
        }
    }

    // The returned strings are volatile and should be used immediatly or copied locally
    static inline void GetArgByOrd(const int code, const int ord, char* &name, char* &type)
    {
        int i = GetIndexByUDFCode(code);

        // assert on failure
        if (i == CODENOTFOUND) {
            name = NULL;
            type = NULL;
            printf("GetArgByOrd failed - Arg %d not found\n", ord);
            assert(0);
        }

        assert(strlen(UDF_RegistryList[i].mVariables) < TOKENBUFFERSIZE);
        strcpy(mTokenBuffer, UDF_RegistryList[i].mVariables);
        char *p = strtok(mTokenBuffer, ":");

        // Skip variables to the requested one
        for (i=0; p && (i<=ord); i++) {
            type = p;
            name = strtok(NULL, ":");
            if (name)
                p = strtok(NULL, ":");
        }
        // If either one is null, something's messed up
        // assert on failure
        if (!type || !name) {
            name = NULL;
            type = NULL;
            printf("Bad name or type in GetArgByOrd - Ord %d caused a problem\n", ord);
            assert(0);
        }
    }

    // Does the UDF Code contain the requested category?
    static inline void IsUDFCodeCategory(const int code, const char *cat, int &result)
    {
        int i = GetIndexByUDFCode(code);

        // assert on failure
        if (i == CODENOTFOUND) {
            result = 0;
            printf("IsUDFCodeCategory failed - code %d not found\n", code);
            assert(0);
        }

        assert(strlen(UDF_RegistryList[i].mCategories) < TOKENBUFFERSIZE);
        strcpy(mTokenBuffer, UDF_RegistryList[i].mCategories);
        char *p = strtok(mTokenBuffer, ":");
        while (p && strcmp(p, cat))
            p = strtok(NULL, ":");

        result = (p != NULL);
    }

    // The returned string is volatile and should be used immediatly or copied locally
    static inline void GetStructureNameByUDFCode(const int code, char* &name)
    {
        int i = GetIndexByUDFCode(code);

        // assert on failure
        if (i == CODENOTFOUND) {
            name = NULL;
            printf("GetStructureNameByUDFCode failed - Code %d not found\n", code);
            assert(0);
        }
        sprintf(mTokenBuffer, "%s_Params", UDF_RegistryList[i].mName);
        name = mTokenBuffer;
    }

    // The returned string is volatile and should be used immediatly or copied locally
    static inline void GetStructureStringByUDFCode(const int code, char* &struc)
    {
        int i = GetIndexByUDFCode(code);

        // assert on failure
        if (i == CODENOTFOUND) {
            struc = NULL;
            printf("GetStructureStringByUDFCode failed - Code %d not found\n", code);
            assert(0);
        }

        sprintf(mStructureBuffer, "struct %s_Params\n{\n", UDF_RegistryList[i].mName);
        assert(strlen(UDF_RegistryList[code].mVariables) < TOKENBUFFERSIZE);
        strcpy(mTokenBuffer, UDF_RegistryList[code].mVariables);
        char *p = strtok(mTokenBuffer, ":");
        int first = 1;
        while (p) {
            if (!strcmp(p, "EMPTY"))
                break;
            char *type = p;
            p = strtok(NULL, ":");
            char *name = p;
            if (first)
                first = 0;
            else
                strcat(mStructureBuffer, ";\n");
            strcat(mStructureBuffer, "	");
            strcat(mStructureBuffer, type);
            strcat(mStructureBuffer, " ");
            strcat(mStructureBuffer, name);
            p = strtok(NULL, ":");
        }
        strcat(mStructureBuffer, ";\n};\n");
        struc = mStructureBuffer;
    }

    // The returned string is volatile and should be used immediatly or copied locally
    static inline void GetStructureContentsStringByUDFCode(const int code, char* &struc)
    {
        int i = GetIndexByUDFCode(code);

        // assert on failure
        if (i == CODENOTFOUND) {
            struc = NULL;
            printf("GetStructureContentsStringByUDFCode failed - Code %d not found\n", code);
            assert(0);
        }

        struc = UDF_RegistryList[code].mVariables;
    }
};

char UDF_RegistryIF::mTokenBuffer[UDF_RegistryIF::TOKENBUFFERSIZE];

char UDF_RegistryIF::mStructureBuffer[UDF_RegistryIF::STRUCTUREBUFFERSIZE];

UDF_RegistryIF::RegistryRecord UDF_RegistryIF::UDF_RegistryList[] =
{
{0, "DefaultForce", "2006/01/13", "12:53:51", 5973, 52940, 0, 0, "Default", "int:dummy", "EMPTY:EMPTY"},
{1, "StdHarmonicBondForce", "2006/01/13", "12:53:51", 61670, 11297, 2, 1, "ReportsEnergy:Harmonic:UpdateForce", "double:k:double:r0", "EMPTY:EMPTY"},
{2, "StdHarmonicAngleForce", "2006/01/13", "12:53:52", 53494, 63087, 3, 1, "ReportsEnergy:Harmonic:UpdateForce", "double:k:double:th0", "EMPTY:EMPTY"},
{3, "SwopeTorsionForce", "2006/01/13", "12:53:52", 16422, 53659, 4, 1, "ReportsEnergy:Torsion:UpdateForce", "double:cosDelta:double:k:double:sinDelta:int:n", "EMPTY:EMPTY"},
{4, "UreyBradleyForce", "2006/01/13", "12:53:52", 61674, 14292, 2, 1, "ReportsEnergy:Harmonic:UpdateForce", "double:k:double:s0", "EMPTY:EMPTY"},
{5, "OPLSTorsionForce", "2006/01/13", "12:53:52", 16422, 55038, 4, 1, "ReportsEnergy:Torsion:UpdateForce", "double:cosDelta:double:k:double:sinDelta:int:n", "EMPTY:EMPTY"},
{6, "OPLSImproperForce", "2006/01/13", "12:53:52", 16422, 63775, 4, 1, "ReportsEnergy:Torsion:UpdateForce", "double:cosDelta:double:k:double:sinDelta:int:n", "EMPTY:EMPTY"},
{7, "ImproperDihedralForce", "2006/01/13", "12:53:53", 48556, 29245, 4, 1, "ReportsEnergy:Torsion:UpdateForce", "double:k:double:psi0", "EMPTY:EMPTY"},
{8, "LennardJonesForce", "2006/01/13", "12:53:53", 4233, 4828, 2, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:epsilon:double:sigma", "EMPTY:EMPTY"},
{9, "StdChargeForce", "2006/01/13", "12:53:53", 19870, 44763, 2, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:ChargeAxB", "EMPTY:EMPTY"},
{10, "TIP3PForce", "2006/01/13", "12:53:53", 21090, 6298, 3, 1, "ReportsEnergy:Harmonic:UpdateForce", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{11, "KineticEnergy", "2006/01/13", "12:53:53", 7930, 50317, 0, 1, "ReportsEnergy:Harmonic:UpdateEnergy", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{12, "WaterRigid3SiteShake", "2006/01/13", "12:53:54", 57716, 33448, 3, 1, "Harmonic:UpdateVelocityFirstHalfStep", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{13, "WaterRigid3SiteRattle", "2006/01/13", "12:53:54", 33307, 7817, 3, 1, "Harmonic:UpdateVelocitySecondHalfStep", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{14, "UpdateVolumeHalfStep", "2006/01/13", "12:53:54", 44441, 2638, 0, 1, "Harmonic:UpdateVirial", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{15, "UpdateVolumePosition", "2006/01/13", "12:53:54", 44441, 62080, 0, 1, "Harmonic:UpdateVirial", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{16, "UpdateVelocityFirstHalfStepNPT", "2006/01/13", "12:53:54", 57716, 63845, 1, 1, "Harmonic:UpdateVelocityFirstHalfStep", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{17, "UpdatePositionNPT", "2006/01/13", "12:53:55", 53927, 13785, -1, 1, "Harmonic:UpdatePosition", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{18, "UpdateVelocitySecondHalfStepNPT", "2006/01/13", "12:53:55", 33307, 12799, -1, 1, "Harmonic:UpdateVelocitySecondHalfStep", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{19, "UpdateVirialForce", "2006/01/13", "12:53:55", 44441, 27038, 0, 1, "Harmonic:UpdateVirial", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{20, "SPCForce", "2006/01/13", "12:53:55", 21090, 8640, 3, 1, "ReportsEnergy:Harmonic:UpdateForce", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{21, "EwaldRealSpaceForce", "2006/01/13", "12:53:55", 24712, 7800, 2, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:ChargeAxB:double:alpha", "EMPTY:EMPTY"},
{22, "EwaldStructureFactors", "2006/01/13", "12:53:56", 59393, 16630, 1, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:charge:double:kx:double:ky:double:kz", "EMPTY:EMPTY"},
{23, "EwaldKSpaceForce", "2006/01/13", "12:53:56", 51125, 7949, 0, 1, "ReportsEnergy:NonBonded:UpdateForce", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{24, "EwaldSelfEnergyForce", "2006/01/13", "12:53:56", 51125, 32264, 0, 1, "ReportsEnergy:NonBonded:UpdateForce", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{25, "EwaldCorrectionForce", "2006/01/13", "12:53:56", 24712, 33655, 2, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:ChargeAxB:double:alpha", "EMPTY:EMPTY"},
{26, "WaterSPCShake", "2006/01/13", "12:53:56", 57716, 4709, 3, 1, "Harmonic:UpdateVelocityFirstHalfStep", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{27, "WaterSPCRattle", "2006/01/13", "12:53:56", 33307, 58625, 3, 1, "Harmonic:UpdateVelocitySecondHalfStep", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{28, "WaterTIP3PShake", "2006/01/13", "12:53:57", 57716, 22029, 3, 1, "Harmonic:UpdateVelocityFirstHalfStep", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{29, "WaterTIP3PRattle", "2006/01/13", "12:53:57", 33307, 60993, 3, 1, "Harmonic:UpdateVelocitySecondHalfStep", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{30, "NSQLennardJones", "2006/01/13", "12:53:57", 53452, 18819, 2, 1, "ReportsEnergy:NonBonded:NSQ1:UpdateForce", "double:epsilon:double:sigma", "EMPTY:EMPTY"},
{31, "NSQCoulomb", "2006/01/13", "12:53:57", 27552, 33430, 2, 1, "ReportsEnergy:NonBonded:NSQ1:UpdateForce", "double:ChargeAxB", "EMPTY:EMPTY"},
{32, "LJStdCombiner", "2006/01/13", "12:53:57", 22141, 54778, 0, 1, "PairTransform", "double:epsilon:double:sigma", "EMPTY:EMPTY"},
{33, "SwitchStd5", "2006/01/13", "12:53:58", 37954, 26364, 0, 1, "Function", "double:r", "double:S:double:dSdR"},
{34, "NSQEwaldCorrection", "2006/01/13", "12:53:58", 19870, 27010, 2, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:ChargeAxB", "EMPTY:EMPTY"},
{35, "NSQEwaldRealSpace", "2006/01/13", "12:53:58", 19870, 16307, 2, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:ChargeAxB", "EMPTY:EMPTY"},
{36, "Coulomb14", "2006/01/13", "12:53:58", 19870, 59136, 2, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:ChargeAxB", "EMPTY:EMPTY"},
{37, "WaterTIP3PInit", "2006/01/13", "12:53:58", 23730, 11581, 3, 1, "WaterInit", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{38, "WaterSPCInit", "2006/01/13", "12:53:59", 23730, 6117, 3, 1, "WaterInit", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{39, "LJOPLSCombiner", "2006/01/13", "12:53:59", 22141, 18946, 0, 1, "PairTransform", "double:epsilon:double:sigma", "EMPTY:EMPTY"},
{40, "NSQCheckSitesInBox", "2006/01/13", "12:53:59", 54809, 43412, 2, 1, "Init", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{41, "COMKineticEnergy", "2006/01/13", "12:53:59", 25433, 54504, 0, 1, "ReportsEnergy:Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{42, "ResampleEnergyLoss", "2006/01/13", "12:53:59", 25433, 59329, 0, 1, "ReportsEnergy:Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{43, "UpdateVolumeSecondHalfStep", "2006/01/13", "12:54:00", 55881, 13700, 0, 1, "Harmonic:UpdateVolumeSecondHalfStep", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{44, "ZeroForce", "2006/01/13", "12:54:00", 54835, 26527, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{45, "UpdatePosition", "2006/01/13", "12:54:00", 54835, 28539, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{46, "UpdateVelocity", "2006/01/13", "12:54:00", 54835, 55061, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{47, "UpdateCenterOfMassKineticEnergy", "2006/01/13", "12:54:00", 25433, 48983, 0, 1, "ReportsEnergy:Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{48, "UpdateCenterOfMass", "2006/01/13", "12:54:01", 54835, 36283, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{49, "LennardJones14Force", "2006/01/13", "12:54:01", 4233, 33040, 2, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:epsilon:double:sigma", "EMPTY:EMPTY"},
{50, "OPLSLennardJones14Force", "2006/01/13", "12:54:01", 4233, 55870, 2, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:epsilon:double:sigma", "EMPTY:EMPTY"},
{51, "NSQOPLSLennardJones", "2006/01/13", "12:54:01", 53452, 10028, 2, 1, "ReportsEnergy:NonBonded:NSQ1:UpdateForce", "double:epsilon:double:sigma", "EMPTY:EMPTY"},
{52, "NSQLekner", "2006/01/13", "12:54:01", 19870, 4319, 2, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:ChargeAxB", "EMPTY:EMPTY"},
{53, "NSQLeknerCorrection", "2006/01/13", "12:54:02", 19870, 12731, 2, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:ChargeAxB", "EMPTY:EMPTY"},
{54, "LeknerSelfEnergy", "2006/01/13", "12:54:02", 56965, 19765, 0, 0, "Default", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{55, "ShakeGroup2", "2006/01/13", "12:54:02", 18126, 59162, 2, 1, "Harmonic:UpdateVelocityFirstHalfStep", "ConstraintGroupParams<2>:cparams", "EMPTY:EMPTY"},
{56, "ShakeGroup3", "2006/01/13", "12:54:02", 19150, 61282, 2, 1, "Harmonic:UpdateVelocityFirstHalfStep", "ConstraintGroupParams<3>:cparams", "EMPTY:EMPTY"},
{57, "ShakeGroup4", "2006/01/13", "12:54:02", 20174, 63402, 2, 1, "Harmonic:UpdateVelocityFirstHalfStep", "ConstraintGroupParams<4>:cparams", "EMPTY:EMPTY"},
{58, "RattleGroup4", "2006/01/13", "12:54:03", 33747, 38436, 2, 1, "Harmonic:UpdateVelocitySecondHalfStep", "ConstraintGroupParams<4>:cparams", "EMPTY:EMPTY"},
{59, "RattleGroup3", "2006/01/13", "12:54:03", 32723, 53831, 2, 1, "Harmonic:UpdateVelocitySecondHalfStep", "ConstraintGroupParams<3>:cparams", "EMPTY:EMPTY"},
{60, "RattleGroup2", "2006/01/13", "12:54:03", 31699, 52741, 2, 1, "Harmonic:UpdateVelocitySecondHalfStep", "ConstraintGroupParams<2>:cparams", "EMPTY:EMPTY"},
{61, "AtomToFragmentVirialCorrection", "2006/01/13", "12:54:03", 54835, 43648, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{62, "ZeroCenterOfMassForce", "2006/01/13", "12:54:03", 54835, 19911, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{63, "UpdateCenterOfMassPosition", "2006/01/13", "12:54:04", 54835, 64657, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{64, "UpdateCenterOfMassVelocity", "2006/01/13", "12:54:04", 54835, 54627, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{65, "UpdateCenterOfMassForce", "2006/01/13", "12:54:04", 54835, 28353, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{66, "NSQEwaldRespaCorrection", "2006/01/13", "12:54:04", 19870, 32408, 2, 1, "ReportsEnergy:NonBonded:UpdateForce", "double:ChargeAxB", "EMPTY:EMPTY"},
{67, "SwitchFunction", "2006/01/13", "12:54:04", 54299, 52007, 0, 1, "NonBonded", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{68, "SwitchFunctionForce", "2006/01/13", "12:54:05", 54299, 11722, 0, 1, "NonBonded", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{69, "NoseHooverInit", "2006/01/13", "12:54:05", 54835, 1136, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{70, "UpdateFragmentKEForNoseHoover", "2006/01/13", "12:54:05", 54835, 49840, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{71, "NoseHooverIntegration", "2006/01/13", "12:54:05", 54835, 57423, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{72, "NoseHooverEnergy", "2006/01/13", "12:54:05", 54835, 13006, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{73, "NoseHooverInitThermostatPosVel", "2006/01/13", "12:54:06", 54835, 35439, 0, 1, "Harmonic", "EMPTY:EMPTY", "EMPTY:EMPTY"},
{74, "COMRestraint", "2006/01/13", "12:54:06", 37902, 57753, 4, 2, "Harmonic", "double:k:double:z0", "EMPTY:EMPTY"}
};

#endif
