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
 package com.ibm.bluematter.db2probspec;

import java.sql.*;
import java.io.*;
import java.util.*;

import com.ibm.bluematter.db2probspec.parser.*;

public class MSD_WrapperGenerator {


    public static void dump( FileWriter out, Vector udfInvocationTable, UDF_RegistryIF udfRegistry,
                             boolean doLJ14, String ffFamily, Vector pair1234,
                             Vector[] rigidSites, int numberOfWaters ) throws Exception {

        Vector activeUdfNames = new Vector();

        out.write("// This file is meant to contain code that modifys how the\n");
        out.write("// msd.hpp file information is seen by the core.\n");
        out.write("\n");
        out.write("#include <BlueMatter/TupleDriverUdfBinder.hpp>\n");
        out.write("\n");

        out.write("\n");
        out.write("\n");

        out.write("class ParamAccessor_Base\n");
        out.write("  {\n");
        out.write("  public:\n");
        out.write("    static void Init(){}\n");
        out.write("  };\n");
        out.write("\n");
        out.write("template< int UdfInvocationInstance >\n");
        out.write("class ParamAccessor{};\n");
        out.write("\n");


        //out.write("#ifdef UDF_StdHarmonicBondForce_Enabled\n");

        for (int udfIndex = 0; udfIndex < udfInvocationTable.size(); udfIndex++) {
            //out.write("template<>\n");
            UDFInvocation curInvocation = (UDFInvocation) udfInvocationTable.elementAt( udfIndex );
            String udfName = curInvocation.getUdfName();
            String siteTupleListName = curInvocation.getSiteTupleListName();
            String paramTupleListName = curInvocation.getParamTupleListName();
            int siteTupleCount  = curInvocation.getSiteTupleCount();

            out.write("class "+ udfName +"Accessor : public ParamAccessor_Base\n");
            out.write("  {\n");
            out.write("  public:\n");
            out.write("\n");
            out.write("    typedef UDF_Binding::" + udfName + "_Params ParamType;\n");
            out.write("\n");
            out.write("    static\n");
            out.write("    const ParamType&\n");
            out.write("    GetParam( int i )\n");
            out.write("      {\n");
            out.write("      return( MSD::"+paramTupleListName+"[ i ] );\n");
            out.write("      }\n");
            out.write("  };\n");
            out.write("\n");
            out.write("typedef TupleDriverUdfBinder< UDF_" + udfName + ",\n");
            //out.write("                sizeof(  MSD::"+paramTupleListName+" ) / sizeof( MSD::"+paramTupleListName+"[0]),\n");
            out.write("                " + siteTupleCount + ",\n");
            out.write("                MSD::"+siteTupleListName+",\n");
            out.write("                "+udfName+"Accessor > "+udfName+"UDF_Bound;\n");
            out.write("\n");
            activeUdfNames.add(udfName);
        }

        // out.write("#ifdef UDF_LennardJones14Force_Enabled\n");

        if(doLJ14) {
            out.write("template<class Combiner>\n");
            out.write("class LennardJones14ParamAccessor   : public ParamAccessor_Base\n");
            out.write("  {\n");
            out.write("  public:\n");
            out.write("\n");
            out.write("    typedef UDF_Binding::LennardJonesForce_Params ParamType;\n");
            out.write("\n");
            out.write("//    static ParamType* combinedParameterArray;\n");
            out.write("\n");
            out.write("    static\n");
            out.write("    void\n");
            out.write("    Init()\n");
            out.write("    {\n");
            out.write("      int numberOfSiteTuples =  PAIR14_SITE_LIST_SIZE;\n");
            out.write("      int paramSize          = sizeof(ParamType);\n");
            out.write("\n");
            out.write("      ParamType* combinedParameterArray = NULL;\n");
            out.write("      GetParam( -1, &combinedParameterArray );\n");
            out.write("\n");
            out.write("      for (int i = 0; i < numberOfSiteTuples; i++)\n");
            out.write("        {\n");
            out.write("\n");
            out.write("          int site1 = MSD::Pair14SiteList[ 2*i ];\n");
            out.write("          int site2 = MSD::Pair14SiteList[ 2*i + 1 ];\n");
            out.write("\n");
            out.write("          int siteType1 = MSD::SiteIDtoSiteTypeMap[ site1 ];\n");
            out.write("          int siteType2 = MSD::SiteIDtoSiteTypeMap[ site2 ];\n");
            out.write("          Combiner::Combine<ParamType>( MSD::LJ14PairTable[ siteType1 ],  MSD::LJ14PairTable[ siteType2 ], combinedParameterArray[ i ] );\n");
            out.write("        }\n");
            out.write("    }\n");
            out.write("\n");
            out.write("    static\n");
            out.write("    const ParamType&\n");
            out.write("    GetParam( int i , ParamType **SecretParameterArrayReturnField = (ParamType **) 0xFFFFFFFF )\n");
            out.write("      {\n");
            out.write("      static ParamType CombinedParameterArray[ PAIR14_SITE_LIST_SIZE ];\n");
            out.write("      if( SecretParameterArrayReturnField != (ParamType **)0xFFFFFFFF )\n");
            out.write("        {\n");
            out.write("        *SecretParameterArrayReturnField = CombinedParameterArray;\n");
            out.write("        return( CombinedParameterArray[ 0 ] );\n");
            out.write("        }\n");
            out.write("      return( CombinedParameterArray[ i ] );\n");
            out.write("      }\n");
            out.write("  };\n");
            out.write("\n");
            out.write("typedef TupleDriverUdfBinder< UDF_LennardJones14Force,\n");
            out.write("                PAIR14_SITE_LIST_SIZE,\n");
            out.write("                MSD::Pair14SiteList,\n");
            activeUdfNames.add("LennardJones14Force");

            // CLEAN THIS UP when things settle
            if(ffFamily.equals("CHARMM"))
                out.write("                LennardJones14ParamAccessor<LennardJones14CHARMMCombiner > > LennardJones14UDF_Bound;\n");
            else if(ffFamily.equals("OPLSAA"))
                out.write("                LennardJones14ParamAccessor<LennardJones14OPLSAACombiner > > LennardJones14UDF_Bound;\n");
            else if(ffFamily.equals("AMBER"))
                out.write("                LennardJones14ParamAccessor<LennardJones14AMBERCombiner > > LennardJones14UDF_Bound;\n");

            out.write("\n");
            out.write("\n");
            out.write("\n");
            out.write("\n");
            out.write("///----------------------------------------------------------------\n");
            out.write("\n");
        }

        if (pair1234.size() != 0 ) {
            out.write("// This is needed for EwaldCorrection and LeknerCorrection\n");
            out.write("template<class Combiner>\n");
            out.write("class Coulomb1_2_3_4ParamAccessor   : public ParamAccessor_Base\n");
            out.write("  {\n");
            out.write("  public:\n");
            out.write("\n");
            out.write("    typedef UDF_Binding::NSQCoulomb_Params ParamType;\n");
            out.write("\n");
            out.write("//    static ParamType* combinedParameterArray;\n");
            out.write("\n");
            out.write("    static\n");
            out.write("    void\n");
            out.write("    Init()\n");
            out.write("    {\n");
            out.write("      int numberOfSiteTuples =  EXCLUSION_LIST1_2_3_4;\n");
            out.write("      int paramSize          = sizeof(ParamType);\n");
            out.write("\n");
            out.write("      ParamType* combinedParameterArray = NULL;\n");
            out.write("      GetParam( -1, &combinedParameterArray );\n");
            out.write("\n");
            out.write("      for (int i = 0; i < numberOfSiteTuples; i++)\n");
            out.write("        {\n");
            out.write("          int site1 = MSD::ExclusionList1_2_3_4[ 2*i ];\n");
            out.write("          int site2 = MSD::ExclusionList1_2_3_4[ 2*i + 1 ];\n");
            out.write("\n");
            out.write("          int siteType1 = MSD::SiteIDtoSiteTypeMap[ site1 ];\n");
            out.write("          int siteType2 = MSD::SiteIDtoSiteTypeMap[ site2 ];\n");
            out.write("          \n");
            out.write("          ParamType charge1;\n");
            out.write("          ParamType charge2;\n");
            out.write("          charge1.ChargeAxB = MSD::SiteInformation[ siteType1 ].charge;\n");
            out.write("          charge2.ChargeAxB = MSD::SiteInformation[ siteType2 ].charge;\n");
            out.write("          \n");
            out.write("          Combiner::Combine<ParamType>( charge1 ,  \n");
            out.write("                                        charge2, \n");
            out.write("                                        combinedParameterArray[ i ] );\n");
            out.write("        }\n");
            out.write("    }\n");
            out.write("\n");
            out.write("    static\n");
            out.write("    const ParamType&\n");
            out.write("    GetParam( int i , ParamType **SecretParameterArrayReturnField = (ParamType **) 0xFFFFFFFF )\n");
            out.write("      {\n");
            out.write("      static ParamType CombinedParameterArray[ EXCLUSION_LIST1_2_3_4 ];\n");
            out.write("      if( SecretParameterArrayReturnField != (ParamType **)0xFFFFFFFF )\n");
            out.write("        {\n");
            out.write("        *SecretParameterArrayReturnField = CombinedParameterArray;\n");
            out.write("        return( CombinedParameterArray[ 0 ] );\n");
            out.write("        }\n");
            out.write("      return( CombinedParameterArray[ i ] );\n");
            out.write("      }\n");
            out.write("  };\n");
            out.write("\n");


            if(RunTimeParameters.getLongRangeMethod().equals("EWALD") ||
               RunTimeParameters.getLongRangeMethod().equals("P3ME")) {
                out.write("typedef TupleDriverUdfBinder< UDF_EwaldCorrection,\n");
                out.write("                EXCLUSION_LIST1_2_3_4,\n");
                out.write("                MSD::ExclusionList1_2_3_4,\n");
                out.write("                Coulomb1_2_3_4ParamAccessor<Coulomb1_2_3_4Combiner > > EwaldCorrectionUDF_Bound;\n");
                activeUdfNames.add("NSQEwaldCorrection");
            }
            else if(RunTimeParameters.getLongRangeMethod().equals("LEKNER")) {
                out.write("typedef TupleDriverUdfBinder< UDF_LeknerCorrection,\n");
                out.write("                EXCLUSION_LIST1_2_3_4,\n");
                out.write("                MSD::ExclusionList1_2_3_4,\n");
                out.write("                Coulomb1_2_3_4ParamAccessor<Coulomb1_2_3_4Combiner > > LeknerCorrectionUDF_Bound;\n");
                activeUdfNames.add("NSQLeknerCorrection");
            }
        }

        out.write("\n");
        out.write("\n");
        out.write("\n");

        out.write("class  NSQCoulombParamsAccessor  : public ParamAccessor_Base\n");
        out.write("  {\n");
        out.write("  public:\n");
        out.write("\n");
        out.write("    typedef UDF_Binding::NSQCoulomb_Params ParamType;\n");
        out.write("\n");
        out.write("    static\n");
        out.write("    const ParamType&\n");
        out.write("    GetParam( int aAbsSiteId )\n");
        out.write("      {\n");
        out.write("      // contain the map to compressed parameter table in this method\n");
        out.write("      int ParamSiteType =  MSD::NsqSiteIDtoSiteTypeMap[ aAbsSiteId ];\n");
        out.write("      // could assert array bounds here - even compile time assert!\n");
        out.write("      return( MSD::ChargeNsqParams[ ParamSiteType ] );\n");
        out.write("      }\n");
        out.write("  };\n");
        out.write("\n");
        out.write("\n");
        out.write("\n");
        out.write("///----------------------------------------------------------------\n");
        out.write("\n");


        out.write("class NSQLennardJonesParamAccessor  : public ParamAccessor_Base\n");
        out.write("  {\n");
        out.write("  public:\n");
        out.write("\n");
        out.write("    typedef  UDF_Binding::NSQLennardJones_Params ParamType;\n");
        out.write("\n");
        out.write("    static\n");
        out.write("    const ParamType&\n");
        out.write("    GetParamByIndex( int aIndex )\n");
        out.write("    {\n");
        out.write("    return(  MSD::LJNsqParams[ aIndex ] );\n");
        out.write("    }\n");
        out.write("\n\n");
        out.write("    static\n");
        out.write("    const ParamType&\n");
        out.write("    GetParam( int aAbsSiteId )\n");
        out.write("      {\n");
        out.write("      int ParamSiteType =  MSD::NsqSiteIDtoSiteTypeMap[ aAbsSiteId ];\n");
        out.write("      return(  MSD::LJNsqParams[ ParamSiteType ] );\n");
        out.write("      }\n");
        out.write("  };\n");
        out.write("\n");
        out.write("\n");

        if(RunTimeParameters.getLongRangeMethod().equals("EWALD")
           || RunTimeParameters.getLongRangeMethod().equals("P3ME")
           || RunTimeParameters.getLongRangeMethod().equals("LEKNER")) {

            out.write("class NSQChargeParamAccessor : public ParamAccessor_Base \n");
            out.write("  {\n");
            out.write("  public:\n");
            out.write("\n");
            out.write("    // Ewald, Lekner and charge params are the same\n");
            out.write("    typedef UDF_Binding::NSQCoulomb_Params ParamType;\n");
            out.write("  \n");
            out.write("    static\n");
            out.write("    const ParamType&\n");
            out.write("    GetParam( int aAbsSiteId )\n");
            out.write("      {\n");
            out.write("      int ParamSiteType =  MSD::NsqSiteIDtoSiteTypeMap[ aAbsSiteId ];\n");
            out.write("      return( MSD::ChargeNsqParams[ ParamSiteType ] );\n");
            out.write("      }\n");
            out.write("  };\n\n\n");
        }
        
      out.write("#include <BlueMatter/LocalTupleListDriver.hpp>\n\n");
        out.write("#include <BlueMatter/IFP_Binder.hpp>\n");

        if(ffFamily.equals("CHARMM") )
            out.write("typedef IFP_Binder<UDF_NSQCHARMMLennardJonesForce,  NSQLennardJonesParamAccessor> UDF_LennardJonesForce_Bound;\n");
        else if(ffFamily.equals("OPLSAA"))
            out.write("typedef IFP_Binder<UDF_NSQOPLSLennardJonesForce,  NSQLennardJonesParamAccessor> UDF_LennardJonesForce_Bound;\n");
        else if(ffFamily.equals("AMBER"))
            out.write("typedef IFP_Binder<UDF_NSQAMBERLennardJonesForce,  NSQLennardJonesParamAccessor> UDF_LennardJonesForce_Bound;\n");


        out.write("\n");

//      out.write("template<class UDF0_Bound, class UDF1_Bound = NULL_IFP_Binder, class UDF2_Bound = NULL_IFP_Binder>\n");
//      out.write("class IrreducibleFragmentPair;\n");

        if(RunTimeParameters.getLongRangeMethod().equals("STDCOULOMB") ||
           RunTimeParameters.getLongRangeMethod().equals("FINITERANGECUTOFFS")) {
            out.write("typedef IFP_Binder<UDF_NSQCoulomb,  NSQCoulombParamsAccessor> UDF_NSQCoulomb_Bound;\n");
            out.write("\n");
            out.write("typedef UdfGroup\n");
            out.write("        < UDF_NSQCoulomb_Bound,\n");
            out.write("          UDF_LennardJonesForce_Bound\n");
            out.write("        > NsqUdfGroup;\n");

            if( RunTimeParameters.doRespa()) {
                out.write("typedef UdfGroup\n");
                out.write("        < UDF_NSQCoulomb_Bound,\n");
                out.write("          UDF_LennardJonesForce_Bound\n");
                out.write("        > NsqUdfGroup1;\n");
                out.write("\n");
            }
            activeUdfNames.add("NSQLennardJones");
            activeUdfNames.add("NSQCoulomb");
        }
        else if(RunTimeParameters.getLongRangeMethod().equals("EWALD") ||
                RunTimeParameters.getLongRangeMethod().equals("P3ME")) {

            if( RunTimeParameters.doRespa()) {
                out.write("typedef IFP_Binder<UDF_NSQCoulomb,  NSQChargeParamAccessor> UDF_NSQCoulomb_Bound;\n");
                out.write("typedef IFP_Binder<UDF_NSQEwaldRespaCorrection,  NSQChargeParamAccessor> UDF_NSQEwaldRespaCorrection_Bound;\n");
                out.write("\n");

                // WE ARE DOING SIMPLE RESPA

                out.write("typedef UdfGroup\n");
                out.write(" <        UDF_NSQCoulomb_Bound,\n");
                out.write("          UDF_LennardJonesForce_Bound,\n");
                out.write("          UDF_NSQEwaldRespaCorrection_Bound\n");
                out.write("     > NsqUdfGroup;\n");

                out.write("typedef UdfGroup\n");
                out.write(" <        UDF_NSQCoulomb_Bound,\n");
                out.write("          UDF_LennardJonesForce_Bound,\n");
                out.write("          UDF_NSQEwaldRespaCorrection_Bound\n");
                out.write("     > NsqUdfGroup1;\n");


                activeUdfNames.add("NSQLennardJones");
                activeUdfNames.add("NSQCoulomb");
                activeUdfNames.add("NSQEwaldRespaCorrection");
            }
            else {
                out.write("typedef IFP_Binder<UDF_NSQEwaldRealSpace,  NSQChargeParamAccessor> UDF_NSQEwaldRealSpace_Bound;\n");
                out.write("\n");
                out.write("typedef UdfGroup\n");
                out.write(" <        UDF_NSQEwaldRealSpace_Bound,\n");
                out.write("          UDF_LennardJonesForce_Bound\n");
                out.write("     > NsqUdfGroup;\n");
                activeUdfNames.add("NSQLennardJones");
                activeUdfNames.add("NSQEwaldRealSpace");
            }
        }
        else if( RunTimeParameters.getLongRangeMethod().equals("LEKNER") ) {
            out.write("typedef IFP_Binder<UDF_NSQLekner,  NSQChargeParamAccessor> UDF_NSQLekner_Bound;\n");

            out.write("\n");
            out.write("typedef UdfGroup\n");
            out.write(" <UDF_NSQLekner_Bound,\n");
            out.write("  UDF_LennardJonesForce_Bound\n");
            out.write(" > NsqUdfGroup;\n");
            activeUdfNames.add("NSQLennardJones");
            activeUdfNames.add("NSQLekner");

            if( RunTimeParameters.doRespa()) {
                throw new Exception("ERROR:: probspecdb2 is not trained to do lekner and respa");
            }
        }
        else {
            if(ProbspecgenDB2.debug)
                System.out.println("Long Range force method: " +  RunTimeParameters.getLongRangeMethod() );
            out.write("     typedef UdfGroup\n");
            out.write("         <NULL_IFP_Binder, NULL_IFP_Binder, NULL_IFP_Binder> NsqUdfGroup;\n");
        }

        boolean doPressureControl = RunTimeParameters.doPressureControl();
        boolean doTemperatureControl = RunTimeParameters.doTemperatureControl();
        boolean doNoseHoover = RunTimeParameters.doNoseHoover();

        //out.write("#ifndef DoPressureControl\n");
        if(!doPressureControl) {
            out.write("  typedef LocalFragmentDirectDriver\n");
            out.write("          < UDF_UpdateVelocity\n");
            //            out.write("            UDF_ZeroForce\n");
            out.write("          > MSD_TYPED_LFD_UpdateVelocityFirstHalfStepDriver; // This name is expected by code in main loop\n");
            //out.write("#else\n");
        }
        else {
            out.write("  typedef LocalFragmentDirectDriver\n");
            out.write("          < UDF_UpdateCenterOfMassVelocity,\n");
            out.write("            UDF_UpdateVelocityFirstHalfStepNPT\n");
            ////            out.write("            UDF_ZeroCenterOfMassForce,\n");
            // out.write("            UDF_ZeroForce\n");
            out.write("          > MSD_TYPED_LFD_UpdateVelocityFirstHalfStepDriver; // This name is expected by code in main loop\n");
            out.write("\n");
        }

        //out.write("#endif\n");
        //out.write("\n");


        // out.write("#ifndef DoPressureControl\n");
        if(!doPressureControl) {
            out.write("  typedef LocalFragmentDirectDriver\n");
            out.write("          < UDF_UpdateVelocity\n");
            out.write("          > MSD_TYPED_LFD_UpdateVelocitySecondHalfStepDriver; // This name is expected by code in main loop\n");
        }
        else {
            // out.write("#else\n");
            out.write("  typedef LocalFragmentDirectDriver\n");
            out.write("          < UDF_UpdateVelocitySecondHalfStepNPT\n");
            out.write("          > MSD_TYPED_LFD_UpdateVelocitySecondHalfStepDriver; // This name is expected by code in main loop\n");
            //out.write("#endif\n");

            out.write("  typedef LocalFragmentDirectDriver\n");
            out.write("          < UDF_AtomToFragmentVirialCorrection\n");
            out.write("          > MSD_TYPED_LFD_AtomToFragmentVirialCorrectionDriver; // This name is expected by code in main loop\n");
        }
        out.write("\n");


        if( doNoseHoover ) {
            out.write("typedef LocalFragmentDirectDriver\n");
            out.write("        < UDF_KineticEnergy,\n");
            out.write("          UDF_NoseHooverEnergy\n");
            out.write("        > MSD_TYPED_LFD_UpdateKineticEnergyDriver;  // This name is expected in the main loop\n");
            activeUdfNames.add("KineticEnergy");
            activeUdfNames.add("NoseHooverEnergy");
        }
        else {
            out.write("typedef LocalFragmentDirectDriver\n");
            out.write("        < UDF_KineticEnergy\n");
            out.write("        > MSD_TYPED_LFD_UpdateKineticEnergyDriver;  // This name is expected in the main loop\n");
            activeUdfNames.add("KineticEnergy");
        }

        if( !doPressureControl ) {
            out.write("  typedef LocalFragmentDirectDriver\n");
            out.write("          < UDF_UpdatePosition\n");
            out.write("          > MSD_TYPED_LFD_UpdatePositionDriver;  // This name is expected in the main loop\n");
        }
        else {

//             out.write("\n");
//             out.write("typedef LocalFragmentDirectDriver\n");
//             out.write("        <  UDF_UpdateCenterOfMassPosition,\n");
//             out.write("          UDF_UpdateCenterOfMassVelocity,\n");
//             out.write("          UDF_UpdateCenterOfMassForce\n");
//             out.write("        > MSD_TYPED_LFD_UpdateCenterOfMassDriver;  // This name is expected in the main loop\n");

            out.write("typedef LocalFragmentDirectDriver\n");
            out.write("        <  UDF_UpdateCenterOfMassPosition,\n");
            out.write("           UDF_UpdateCenterOfMassVelocity\n");
            out.write("        > MSD_TYPED_LFD_UpdateCenterOfMassPositionVelocityDriver;  // This name is expected in the main loop\n");
            out.write("\n");
            out.write("typedef LocalFragmentDirectDriver\n");
            out.write("        < UDF_UpdateCenterOfMassVelocity,\n");
            out.write("          UDF_UpdateCenterOfMassKineticEnergy \n");
            out.write("        > MSD_TYPED_LFD_UpdateCenterOfMassVelocityAndFragmentKineticEnergyDriver;  // This name is expected in the main loop\n");
            out.write("  typedef LocalFragmentDirectDriver\n");
            out.write("          < UDF_UpdatePositionNPT,\n");
            out.write("            UDF_UpdateCenterOfMassPosition");
            out.write("          > MSD_TYPED_LFD_UpdatePositionDriver;  // This name is expected in the main loop\n");

            out.write("\n");
            out.write("typedef LocalFragmentDirectDriver\n");
            out.write("        < UDF_UpdateCenterOfMassForce\n");
            out.write("        > MSD_TYPED_LFD_UpdateCenterOfMassForceDriver;  // This name is expected in the main loop\n");
            out.write("\n");

            out.write("\n");
            out.write("typedef LocalFragmentDirectDriver\n");
            out.write("        < UDF_UpdateCenterOfMassKineticEnergy\n");
            out.write("        > MSD_TYPED_LFD_UpdateFragmentKineticEnergyDriver;  // This name is expected in the main loop\n");
            out.write("\n");

            out.write("\n");
            out.write("typedef LocalFragmentDirectDriver\n");
            out.write("        < UDF_UpdateCenterOfMassVelocity\n");
            out.write("        > MSD_TYPED_LFD_UpdateCenterOfMassVelocityDriver;  // This name is expected in the main loop\n");
            out.write("\n");
        }

        out.write("\n");

        if( doNoseHoover ) {
            out.write("\n");
            out.write("typedef LocalFragmentDirectDriver\n");
            out.write("        < UDF_NoseHooverInit\n");
            out.write("        > MSD_TYPED_LFD_NoseHooverInitDriver;  // This name is expected in the main loop\n");
            out.write("\n");
            out.write("typedef LocalFragmentDirectDriver\n");
            out.write("        < UDF_NoseHooverInitThermostatPosVel\n");
            out.write("        > MSD_TYPED_LFD_NoseHooverInitThermostatPosVelDriver;  // This name is expected in the main loop\n");
            out.write("\n");
            out.write("typedef LocalFragmentDirectDriver\n");
            out.write("        < UDF_UpdateCenterOfMassVelocity,\n");
            out.write("          UDF_UpdateFragmentKEForNoseHoover,\n");
            out.write("          UDF_NoseHooverIntegration\n");
            out.write("        > MSD_TYPED_LFD_NoseHooverIntegrationDriver;  // This name is expected in the main loop\n");
            out.write("\n");

        }

        boolean doFloppy = false;
        String currentWaterModel = RunTimeParameters.getCurrentWaterMethod();
        if(currentWaterModel!=null) {
            if(currentWaterModel.endsWith("FLOPPY")) {
                doFloppy = true;

                String waterModelName = currentWaterModel.substring(0, currentWaterModel.lastIndexOf("FLOPPY"));

                // SPCE and SPC use the same udf functions
                if(waterModelName.equals("SPCE"))
                    waterModelName = "SPC";

                activeUdfNames.add(waterModelName + "Force");
                out.write("typedef TupleDriverUdfBinder <UDF_"+waterModelName+"Force,\n");
                //out.write("                              sizeof(MSD::Water3sites) / ( 3*sizeof(int) ),\n");
                out.write("                              "+numberOfWaters+",\n");
                out.write("                              MSD::Water3sites,\n");
                out.write("                              NULL_ParamAccessor > WaterFloppy_Bound;\n\n");
            }
        }

        out.write("typedef SiteTupleDriver\n");
        out.write("        <  MDVM ");

        for(int i=0; i < udfInvocationTable.size(); i++) {

            out.write(",\n");
            String udfName = ((UDFInvocation) udfInvocationTable.elementAt(i)).getUdfName();
            out.write("           "+udfName+"UDF_Bound");
        }

        if (doLJ14)
            out.write(",\n           LennardJones14UDF_Bound\n");


        if(doFloppy)
            out.write(",\n             WaterFloppy_Bound\n");

        if (pair1234.size() != 0 ) {
            if(RunTimeParameters.getLongRangeMethod().equals("EWALD") ||
               RunTimeParameters.getLongRangeMethod().equals("P3ME")) {
                out.write(",\n             EwaldCorrectionUDF_Bound\n");
            }
            else if(RunTimeParameters.getLongRangeMethod().equals("LEKNER")) {
                out.write(",\n             LeknerCorrectionUDF_Bound\n");
            }
        }
        out.write("        > T_MSD_TYPED_STD_BondedForceDriver;  // This name is expected by code in main loop\n");
        out.write("T_MSD_TYPED_STD_BondedForceDriver MSD_TYPED_STD_BondedForceDriver;\n");


        Vector initBoundsList = new Vector();
        Vector shakeBoundsList = new Vector();
        Vector rattleBoundsList = new Vector();
        if(RunTimeParameters.doShakeRattleWater()) {

            // String currentWaterModel = RunTimeParameters.getCurrentWaterMethod();

            // SPCE and SPC use the same udf functions
            if(currentWaterModel.equals("SPCE"))
                currentWaterModel = "SPC";

            String waterInitUdfName = "UDF_Water" + currentWaterModel + "Init";
            String waterShakeUdfName = "UDF_Water" + currentWaterModel + "Shake";
            String waterRattleUdfName = "UDF_Water" + currentWaterModel + "Rattle";

            out.write("typedef TupleDriverUdfBinder <"+waterInitUdfName+",\n");
            out.write("                              "+numberOfWaters+",\n");
            //      out.write("                              sizeof(MSD::Water3sites) / ( 3*sizeof(int) ),\n");
            out.write("                              MSD::Water3sites,\n");
            out.write("                              NULL_ParamAccessor > WaterInitUDF_Bound;\n\n");

            initBoundsList.add("WaterInitUDF_Bound");

            out.write("typedef TupleDriverUdfBinder <"+waterShakeUdfName+",\n");
            //out.write("                              sizeof(MSD::Water3sites) / ( 3*sizeof(int) ),\n");
            out.write("                              "+numberOfWaters+",\n");
            out.write("                              MSD::Water3sites,\n");
            out.write("                              NULL_ParamAccessor > WaterShakeUDF_Bound;\n\n");

            shakeBoundsList.add("WaterShakeUDF_Bound");

            out.write("typedef TupleDriverUdfBinder <"+waterRattleUdfName+",\n");
            // out.write("                              sizeof(MSD::Water3sites) / ( 3*sizeof(int) ),\n");
            out.write("                              "+numberOfWaters+",\n");
            out.write("                              MSD::Water3sites,\n");
            out.write("                              NULL_ParamAccessor > WaterRattleUDF_Bound;\n\n");

            rattleBoundsList.add("WaterRattleUDF_Bound");
        }

        if(RunTimeParameters.doShakeRattleProtein()) {

            String groupTypes[] = {"Shake","Rattle"};
            for(int j=0; j < groupTypes.length; j++)
                for(int i=2; i <= 4; i++) {


                    if( rigidSites[i-2].size() == 0 ) {
                        continue;
                    }

                    int numberOfTuples = rigidSites[i-2].size() / i;

                    String accessorName = "Rigid" + groupTypes[j] + i +"Accessor";
                    String boundName = "Rigid" + groupTypes[j] + i +"UDF_Bound";

                    out.write("\nclass "+accessorName+" : public ParamAccessor_Base\n");
                    out.write("  {\n");
                    out.write("  public:\n");
                    out.write("\n");
                    out.write("    typedef UDF_Binding::ShakeGroup" + i +"_Params ParamType;\n");
                    out.write("\n");
                    out.write("    static\n");
                    out.write("    const ParamType&\n");
                    out.write("    GetParam( int i )\n");
                    out.write("      {\n");
                    out.write("      return( MSD::Rigid" + i + "Params[ i ] );\n");
                    out.write("      }\n");
                    out.write("  };\n");
                    out.write("\n");
                    out.write("typedef TupleDriverUdfBinder< UDF_" + groupTypes[j] + "Group" + i + ",\n");
                    out.write("                              " + numberOfTuples + ",\n");
                    out.write("                              MSD::Rigid"+i+"Sites,\n");
                    out.write("                              " + accessorName + " > " + boundName + ";\n");
                    out.write("\n");

                    if(j == 0)
                        shakeBoundsList.add(boundName);
                    else
                        rattleBoundsList.add(boundName);
                }
        }

        if(RunTimeParameters.doShakeRattleWater() || RunTimeParameters.doShakeRattleProtein()) {

            out.write("#include <BlueMatter/MDVM_UVP_IF.hpp>\n\n");
            out.write("typedef SiteTupleDriver\n");
            out.write("        <  MDVM_UVP\n");
            for(int i=0; i<initBoundsList.size(); i++) {
                out.write(",        " + (String) initBoundsList.elementAt(i) + "\n");
            }
            out.write("        > T_MSD_TYPED_STD_GroupInitDriver;  // This name is expected by code in main loop\n\n");
            out.write("T_MSD_TYPED_STD_GroupInitDriver MSD_TYPED_STD_GroupInitDriver;\n");

            out.write("typedef SiteTupleDriver\n");
            out.write("        <  MDVM_UVP\n");
            for(int i=0; i<shakeBoundsList.size(); i++) {
                out.write(",        " + (String) shakeBoundsList.elementAt(i) + "\n");
            }
            out.write("        > T_MSD_TYPED_STD_GroupShakeDriver;  // This name is expected by code in main loop\n\n");
            out.write("T_MSD_TYPED_STD_GroupShakeDriver MSD_TYPED_STD_GroupShakeDriver;\n");

            out.write("typedef SiteTupleDriver\n");
            out.write("        <  MDVM_UVP\n");
            for(int i=0; i<rattleBoundsList.size(); i++) {
                out.write(",        " + (String) rattleBoundsList.elementAt(i) + "\n");
            }
            out.write("        > T_MSD_TYPED_STD_GroupRattleDriver;  // This name is expected by code in main loop\n\n");
            out.write("T_MSD_TYPED_STD_GroupRattleDriver MSD_TYPED_STD_GroupRattleDriver;\n");
        }

        int udfCount=0; // We always seem to have the kinetic energy there
        for(int i=0; i < activeUdfNames.size(); i++) {
            String udfName = (String) activeUdfNames.elementAt(i);
            if(udfRegistry.ReportsEnergy( udfName )) {
                if( ProbspecgenDB2.debug )
                    System.out.println("Energy Emitting udf_name: "+udfName+" udfCount: "+(udfCount+1));
                udfCount++;
            }
        }

        if(RunTimeParameters.getLongRangeMethod().equals("EWALD") ||
           RunTimeParameters.getLongRangeMethod().equals("P3ME") ||
           RunTimeParameters.getLongRangeMethod().equals("LEKNER") ) {

            udfCount = udfCount + 2;

            if( ProbspecgenDB2.debug )
                System.out.println("Ewald, P3ME, LEKNER udfCount: " + udfCount);

        }

        if(doTemperatureControl) {
            udfCount++;
            if( ProbspecgenDB2.debug )
                System.out.println("DoTemperatureControl udfCount: " + udfCount);
        }

        out.write("#endif\n\n");

        out.write("#ifndef MSD_COMPILE_CODE_ONLY\n\n");

        if( doPressureControl && doNoseHoover )
            udfCount++; // Extra count due to the separate piston and fragments NoseHoover energies

        out.write("const int MSD::NumberOfEnergyReportingUDFs = " + udfCount + ";\n\n");
        // ProbspecgenDB2.globalDefines.put("NUMBER_OF_ENERGY_REPORTING_UDFS", new String( udfCount + ""));
        // out.write("#define NUMBER_OF_ENERGY_REPORTING_UDFS " + udfCount + "\n");

        out.write("#endif\n\n");

        out.write("#ifndef MSD_COMPILE_DATA_ONLY\n\n");
        out.flush();
    }
}
