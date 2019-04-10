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
 package com.ibm.bluematter.db2probspec.parser;

import java.util.*;
import java.sql.*;
import java.io.*;
import com.ibm.bluematter.db2probspec.*;

public class RunTimeParameters {
    
    public final static int NO_LRF     = 0;
    public final static int STDCOULOMB = 1;
    public final static int FRC        = 2;
    public final static int EWALD      = 3;
    public final static int P3ME       = 4;
    public final static int LEKNER     = 5;
    
    
    public static int longRangeForceMethod;

    private static String       fileName = null;    
    private static int          numberOfRespaLevels;
    private static int          numberOfRespaForCategoryDefines;
    private static int          numberOfSwitchLowerCutoffDefines;
    private static int          numberOfSwitchDeltaDefines;
    private static boolean      doRespa;    
    private static boolean      doConstraints;
    private static boolean      doPressureControl;
    private static boolean      doTemperatureControl;
    private static boolean      doShakeRattleWater;
    private static boolean      doShakeRattleProtein;
    private static boolean      doNVT;
    private static boolean      doFreeEnergy;
    private static boolean      doNoseHoover;

    private static boolean      doFep;
    private static boolean      doOuter;
    private static boolean      doInner;


    private static Hashtable    params = null;
    private static Hashtable    enumParams = null;
    private static Hashtable    respaParams = null;
    // private static Hashtable    probspecParams = null;

    private static Statement stmt = null;
    private static int       ctpId = -1;
    
    public static void init (String fName, int ctp_id, Connection con) {

        fileName = fName;
        ctpId = ctp_id;

  params = new Hashtable();
  respaParams = new Hashtable();
  // probspecParams = new Hashtable();
  enumParams = new Hashtable();

  numberOfRespaLevels = 1;
  longRangeForceMethod = NO_LRF;
  numberOfRespaForCategoryDefines = 0;
        numberOfSwitchLowerCutoffDefines=0;
        numberOfSwitchDeltaDefines=0;

  doPressureControl = false;
  doTemperatureControl = false;
  doShakeRattleWater = false;
  doShakeRattleProtein = false;
  doNVT = false;
  doFreeEnergy = false;
  doNoseHoover = false;


  doRespa = false;
  doFep = false;
  doConstraints = false;
  doOuter = false;
  doInner = false;


        try {
            stmt = con.createStatement();
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(1);
        } 

  initDefaults();
    }
    
    public static boolean doRespa() {
  return doRespa;
    }

    public static boolean doPressureControl() {
  return doPressureControl;
    }

    public static boolean doNoseHoover() {
  return doNoseHoover;
    }

    public static boolean doTemperatureControl() {
  return doTemperatureControl;
    }
 
    public static boolean doShakeRattleWater() {
  return doShakeRattleWater;
    }

    public static boolean doShakeRattleProtein() {
  return doShakeRattleProtein;
    }

    public static String getCurrentWaterMethod() throws Exception {

        String result = getCTParameterFromDB2("WaterModel");
        //        String result = (String) probspecParams.get("WaterMethod");

        if(result!=null)
            result = result.toUpperCase();
        
        if( result==null || result.equals("NONE"))
            return null;

        if( result.equals("CHARMM_TIP3P") )
            result = "TIP3P";
        else if (result.equals("CHARMM_TIP3PFLOPPY"))
            result = "TIP3PFLOPPY";
        
  return result;
    }

    public static String getWaterModel() throws Exception {
        String result = getCTParameterFromDB2("WaterModel");

        if(result!=null)
            result = result.toUpperCase();

        if( result==null || result.startsWith("NONE"))
            return null;

        if(result.endsWith("FLOPPY")) {
            int index = result.lastIndexOf("FLOPPY");
            result = result.substring(0,index);
        }
            
        return result;
    }
    
    public static String getLongRangeMethod() {
  String lrm = (String) enumParams.get("LongRangeForceMethod");
  if(lrm == null)
      return "NO_LRF";
     
  return lrm;
    }

    private static boolean getBooleanForName(String name) {
  String test = (String) enumParams.get(name);	
  return !(test == null || test.equals("0"));

    }

    private static void initDefaults() {

  enumParams.put("LongRangeForceMethod", "STDCOULOMB");
  enumParams.put("DoShakeRattleWater", "0");     //turn shake off by default	
  enumParams.put("DoShakeRattle", "0");     //turn shake off by default	
  enumParams.put("DoShakeRattleProtein", "0");     //turn shake off by default	
  enumParams.put("DoPressureControl", "0"); // turn pressure off by default
  enumParams.put("DoNVT", "0"); 
  enumParams.put("DoFreeEnergy", "0"); 
  enumParams.put("DoVelocityResample", "0");
  enumParams.put("DoTemperatureControl", "0");
  enumParams.put("DoRespa", "0");
  enumParams.put("DoWaterInit", "0");
  enumParams.put("DoNSQCheck", "0");
  enumParams.put("NOSE_HOOVER", "0");
        enumParams.put("DoComputeVirial", "0");
        enumParams.put("ReplicaExchange", "0");
        enumParams.put("Stochastic", "0");
        
  params.put("DoOuter", "0:int");

    }
    
    public static void dumpRTPParamsDeclaration(FileWriter out) throws Exception {
  
  out.write("static const int    SimTicksAtRespaLevel[];\n");
  out.write("static const int    RespaLevelByUDFCode[];\n");
  out.write("static const int    NumberOfRespaLevels;\n");
  out.write("static const int    NumberOfNoseHooverRespaLevels;\n");
  out.write("static const double    NoseHooverMassFactor;\n");
  out.write("static int          NumberOfSimTicks;\n");
  out.write("static const double ConvertedInnerTimeStep;\n");
  out.write("static const double IntegratorLevelConvertedTimeDelta[];\n");
        out.write( "static const double SwitchLowerCutoffForShell[];\n\n" );
        out.write( "static const double SwitchDeltaForShell[];\n\n" );
        out.write( "static const int DoNonBondedAtRespaLevel[];\n\n" );
        out.write( "static const int DoKSpaceAtRespaLevel[];\n\n" );
  out.write("static const int RespaLevelOfFirstShell;\n");

  out.write("static const int    SimpleRespaRatio;\n");
  out.write("static const double ShakeTolerance;\n");
  out.write("static const int    ShakeMaxIterations;\n");
  out.write("static const double RattleTolerance;\n");
  out.write("static const int    RattleMaxIterations;\n");
  out.write("static const double EwaldAlpha;\n");
  out.write("static const double LeknerEpsilon;\n");
  out.write("static const int    EwaldKmax;\n");

  out.write("static const int    P3MEchargeAssignment;\n");
  out.write("static const int    P3MEdiffOrder;\n");

  out.write("static const double InnerTimeStepInPicoSeconds;\n");
  out.write("static const int    NumberOfInnerTimeSteps;\n");
  
  out.write("static const int       DoOuter;\n");
  out.write("static const double OuterTimeStepInPicoSeconds;\n");
  out.write("static int    NumberOfOuterTimeSteps;\n");

  out.write("static const int    NumberOfConstraints;\n");

  out.write("static int    SnapshotPeriodInOTS;\n");
  out.write("static const int    SimTicksPerTimeStep;\n");
        out.write("static int    EmitEnergyTimeStepModulo;\n");
  
  out.write("static const double    ExternalPressure;\n");
  out.write("static double    PressureControlPistonMass;\n");
  out.write("static double    PressureControlPistonInitialVelocity;\n");
  
  out.write("static int       VelocityResamplePeriodInOTS;\n");	
  out.write("static int       VelocityResampleRandomSeed;\n");	
  out.write("static double    VelocityResampleTargetTemperature;\n");	
        out.write("static double    PressureControlTarget;\n");

        // These values are now comming in in the form of arrays
// 	out.write("static const double    SwitchLowerCutoff;\n");	
// 	out.write("static const double    SwitchDelta;\n");	

        //	out.write("static const XYZ    P3MEinitSpacing;\n");	
  out.write("static BoundingBox    BoundingBoxDef;\n");	

  out.write("static const double    Lambda;\n");	
  out.write("static int             SwapPeriodOTS;\n");	
  out.write("static const int       ReplicaExchange;\n");	
  out.write("static const int       Stochastic;\n");	
  out.write("static int             ReplicaExchangeRandomSeed;\n");	
  out.write("static const int       NumOfTemperatureAttempts;\n");	
    }
    
    public static void dumpRTPParamsInitiation( FileWriter out, UDF_RegistryIF udfRegistry, Hashtable globalDefines ) throws Exception {
  
  Enumeration keys = params.keys();

  while(keys.hasMoreElements() ) {
      String key = (String) keys.nextElement();

      Object val =  params.get(key);
      if (val instanceof String) {

    String value = (String) val;
    
    int colon = value.lastIndexOf(":");
                if(colon == -1)
                    throw new Exception("ERROR: Internal Error:: RunTimeParameters.java:: dumpRTPParamsInitiation():: type");

                String content = value.substring(0,colon);
                String type = value.substring(colon+1, value.length());
                
                if( key.equals("NumberOfSitesPerNoseHooverChain")) {
                    ProbspecgenDB2.globalDefines.put("NUMBER_OF_SITES_PER_THERMOSTAT", content );
                    continue;
                }
                
                // NumberOfConstraints is set in MSDGenerator based on the number of rigid bonds
                if(key.equals("NumberOfConstraints") || key.startsWith("SwitchLowerCutoff") 
                   || key.startsWith("SwitchDelta") || key.startsWith("ComputeVirial") )
                    continue;

                if(key.equals("NumberOfOuterTimeSteps") ||
                   key.equals("VelocityResampleTargetTemperature") ||
                   key.equals("EmitEnergyTimeStepModulo") ||
                   key.equals("SnapshotPeriodInOTS") ||
                   key.equals("BoundingBoxDef") ||
                   key.equals("SwapPeriodOTS") ||
                   key.equals("PressureControlTarget") ||
                   key.equals("PressureControlPistonMass") ||
                   key.equals("PressureControlPistonInitialVelocity") ||
                   key.equals("VelocityResamplePeriodInOTS") ||
                   key.equals("ReplicaExchangeRandomSeed") ||
                   key.equals("VelocityResampleRandomSeed") ) {
                    
                     if(key.equals("PressureControlPistonMass") && content.equals("-1.0")) {
                         String boundingBox = (String) params.get("BoundingBoxDef");
                         
                         boundingBox = boundingBox.replace('{',' ');
                         boundingBox = boundingBox.replace('}',' ');
                         
                         StringTokenizer boxTokenizer = new StringTokenizer(boundingBox, ",");

                         if(boxTokenizer.countTokens() != 6) {
                             throw new Exception("ERROR:: BoundingBox is not set correctly");
                         }

                         String minX = boxTokenizer.nextToken();
                         String minY = boxTokenizer.nextToken();
                         String minZ = boxTokenizer.nextToken();
                         String maxX = boxTokenizer.nextToken();
                         String maxY = boxTokenizer.nextToken();
                         String maxZ = boxTokenizer.nextToken();

                         int index = maxZ.lastIndexOf(":");
                         maxZ = maxZ.substring(0, index);

                         String volume = "((" + maxX + ")-(" + minX + "))"+
                             "*" + "((" + maxY + ")-(" + minY + "))"+
                             "*" + "((" + maxZ + ")-(" + minZ + "))";

                         String cubeRootVolume = "cbrt("+volume+")";
                         
                         content = "0.0004 * 25.0 / ( "+ cubeRootVolume +" )";
                         
                     }

                    out.write(type + " MSD::" + key + " = " + content + ";\n\n"); 

                    // out.write("#define MSD_RTP_" + key + "_NONCONST\n\n");
                    globalDefines.put("MSD_RTP_" + key + "_NONCONST", new String(" "));
                }
                else {
                    out.write("const " + type + " MSD::" + key + " = " + content + ";\n\n"); 
                }
            }
  }	    

  int numberOfUdfCodes = udfRegistry.GetNumberOfUDFCodes(); 	
        
        if(!doRespa) {
            numberOfRespaLevels=1;
        }
        else {
        }

  int simTicksAtRespaLevel[] = new int[ numberOfRespaLevels ];
        boolean isRespaLevelDefined[] = new boolean[ numberOfRespaLevels ];
        
        if( doRespa ) {            
            for( int rl=0; rl < numberOfRespaLevels; rl++ ) 
                {
                    isRespaLevelDefined[ rl ] = false;       
                }
        }

        isRespaLevelDefined[ 0 ] = true;       
            
  // respaLevel 0 always gets ONE SimTick
  simTicksAtRespaLevel[ 0 ] = 1;
  
  out.write("const int MSD::SimTicksAtRespaLevel[] = {\n");
  
        if(ProbspecgenDB2.debug)
            System.out.println("respaParams hashtable: " + respaParams);

  for( int i = 1; i < numberOfRespaLevels; i++ ) {
      String defineRespa = (String) respaParams.get("DefineRespaLevel "+i);
      
      if (defineRespa == null)
    throw new Exception("Respa level not defined in RTP file.");
      
      int colon = defineRespa.lastIndexOf(":");
      int respaLevel    = Integer.parseInt(defineRespa.substring(0,colon));
      int simTicks   = Integer.parseInt(defineRespa.substring(colon+1, defineRespa.length()));
            isRespaLevelDefined[ respaLevel ] = true;
      simTicksAtRespaLevel[ respaLevel ] = simTicks;	    
  }
  
        // NOTE: Here is where we enforce the convention of a simticks is a multiple of the previous respa level
  for( int respaLevel = 1; respaLevel < numberOfRespaLevels; respaLevel++ ) {
      simTicksAtRespaLevel[ respaLevel ] = simTicksAtRespaLevel[ respaLevel ] * simTicksAtRespaLevel[ respaLevel - 1];	    
  }
  
  for( int i = 0; i < numberOfRespaLevels; i++ ) {
      out.write(String.valueOf(simTicksAtRespaLevel[ i ]));
      if (i != numberOfRespaLevels-1) 
    out.write( ",\n" );
  }
  out.write("};\n\n");
  
  out.write("const int MSD::SimTicksPerTimeStep = " + String.valueOf(simTicksAtRespaLevel[ numberOfRespaLevels-1 ]) + ";\n\n");
    
  String categoryForRespaLevel[] = new String[ numberOfRespaLevels ];	
  
        //	int respaLevelNonbonded = 0;
  for (int i=0; (i < numberOfRespaForCategoryDefines) && doRespa; i++) {
      String defineRespa = (String) respaParams.get("DefineRespaLevelForForceCategory"+i);
      
      if (defineRespa == null)
    throw new Exception("DefineRespaLevelForForceCategory not defined in RTP file.");
      
      int colon = defineRespa.lastIndexOf(":");
      
      String category   = defineRespa.substring(0, colon);
      int respaLevel    = Integer.parseInt(defineRespa.substring(colon+1, defineRespa.length()));
      categoryForRespaLevel[ respaLevel ] = category;	    
  }
  
  if(ProbspecgenDB2.debug) {
      for(int respaLevel=0; doRespa && (respaLevel < numberOfRespaLevels); respaLevel++ ) {
    System.out.println(categoryForRespaLevel[ respaLevel ]);	    
      }
  }
  
        //	respaLevelNonbonded = 0;
        
        int doNonBondedAtRespaLevel[] = new int[ numberOfRespaLevels ];
        int doKSpaceAtRespaLevel[] = new int[ numberOfRespaLevels ];
        doNonBondedAtRespaLevel[ 0 ] = 1;
        doKSpaceAtRespaLevel[ 0 ] = 1;

        for(int respaLevel=0; doRespa && (respaLevel < numberOfRespaLevels); respaLevel++ ) 
        {
            doNonBondedAtRespaLevel[ respaLevel ] = 0;
            doKSpaceAtRespaLevel[ respaLevel ] = 0;            
        }

        int numberOfShells=1;
        int respaLevelOfFirstShell=0;        
        boolean firstEntry=true;
        int respaLevelToShellIdMap[] = new int[ numberOfRespaLevels ];
        int shellId=0;
  for(int respaLevel=0; doRespa && (respaLevel < numberOfRespaLevels); respaLevel++ ) {
      if (categoryForRespaLevel[ respaLevel ].equals("NonBonded")) 
            {
                doNonBondedAtRespaLevel[ respaLevel ] = 1;
                respaLevelToShellIdMap[ respaLevel ] = shellId;
                shellId++;

                if(firstEntry) {
                    respaLevelOfFirstShell = respaLevel;
                    firstEntry = false;
                } 
                else 
                    numberOfShells++;
      } 
            else if(categoryForRespaLevel[ respaLevel ].equals("KSpace")) 
            {
                doKSpaceAtRespaLevel[ respaLevel ] = 1;
                doNonBondedAtRespaLevel[ respaLevel ] = 1;
            }
            else 
            {
                respaLevelToShellIdMap[ respaLevel ] = 0;
            }	    
  }	

  if(ProbspecgenDB2.debug) {
            System.out.println("Number of shells: " + numberOfShells );
        }

        ProbspecgenDB2.globalDefines.put("NUMBER_OF_SHELLS", new String( numberOfShells + "") );
  out.write("const int MSD::RespaLevelOfFirstShell="+ respaLevelOfFirstShell +";\n");	    	

  int respaLevelByUDFCode[] = new int[ numberOfUdfCodes ];
  if(doRespa) {

            int ewaldCorrectionCode  = udfRegistry.GetUDFCodeByName("EwaldCorrectionForce");
            int NSQewaldCorrectionCode  = udfRegistry.GetUDFCodeByName("NSQEwaldCorrection");
            int NSQEwaldRespaCorrectionCode  = udfRegistry.GetUDFCodeByName("NSQEwaldRespaCorrection");

      for (int udfCode=0; udfCode <  numberOfUdfCodes; udfCode++) {
    
                if(( ewaldCorrectionCode == udfCode ) || 
                   (NSQewaldCorrectionCode == udfCode ) || 
                   (NSQEwaldRespaCorrectionCode == udfCode) ) {
                    // Ewald Correction code goes on the last respa level, along with kspace
                    // NOTE:: For SimpleRespa we want these UDFs on each SimTick
                    // respaLevelByUDFCode[ udfCode ] = numberOfRespaLevels-1;
                    respaLevelByUDFCode[ udfCode ] = 0;
                    continue;
                }

    for (int respaLevel=0; respaLevel < numberOfRespaLevels; respaLevel++ ) {	    
        if( udfRegistry.IsCategoryForCode(udfCode, categoryForRespaLevel[ respaLevel ]) ) {
      respaLevelByUDFCode[ udfCode ] = respaLevel;
      break;
        }  
        else {
                        respaLevelByUDFCode[ udfCode ] = 0;
        }
    }
      }
  }
  else {
       for (int udfCode=0; udfCode <  numberOfUdfCodes; udfCode++) {
     respaLevelByUDFCode[ udfCode ] = 0;
       } 	    
  }
  
  out.write("const int MSD::RespaLevelByUDFCode[] = {\n");	    
  
  for (int udfCode=0; udfCode <  numberOfUdfCodes; udfCode++) {            
      out.write(String.valueOf(respaLevelByUDFCode[ udfCode ]));
      if( udfCode != numberOfUdfCodes-1 ) {
    out.write(",\n");
      }
  }
  out.write("\n};\n");        
  
  out.write("const int MSD::NumberOfRespaLevels = " + numberOfRespaLevels + ";\n\n");
        ProbspecgenDB2.globalDefines.put("NUMBER_OF_RESPA_LEVELS", new String( numberOfRespaLevels + "") );

        out.write("const int MSD::DoNonBondedAtRespaLevel[] = {\n");

        for(int i=0; i<numberOfRespaLevels; i++) {
            out.write( String.valueOf(doNonBondedAtRespaLevel[ i ] ) );
            if( i!=numberOfRespaLevels-1)
                out.write(",\n");
        }
        out.write("\n};\n");

        out.write("const int MSD::DoKSpaceAtRespaLevel[] = {\n");

        for(int i=0; i<numberOfRespaLevels; i++) {
            out.write( String.valueOf( doKSpaceAtRespaLevel[ i ] ) );
            if( i != (numberOfRespaLevels-1) )
                out.write(",\n");
        }
        out.write("\n};\n");


        // The switchFunction parameters by respa level go here
        String switchLowerCutoffByShell[] = new String[ numberOfShells ];
        String switchDeltaByShell[] = new String[ numberOfShells ];
        
        if(!doRespa)
            numberOfSwitchLowerCutoffDefines=1;

        for(int i=0; i<numberOfSwitchLowerCutoffDefines; i++) {
      String cutoffValueAndRespaLevel = (String) respaParams.get("SwitchLowerCutoff"+i);            
      String deltaValueAndRespaLevel = (String) respaParams.get("SwitchDelta"+i);            
            
            StringTokenizer cutoffTokens = new StringTokenizer(cutoffValueAndRespaLevel);
            StringTokenizer deltaTokens = new StringTokenizer(deltaValueAndRespaLevel);

            String cutoffValue= cutoffTokens.nextToken();
            
            int respaLevelCutoff=0;
            if(cutoffTokens.hasMoreTokens()) {
                String respaLevelCutoffStr = cutoffTokens.nextToken();
                respaLevelCutoff = Integer.parseInt(respaLevelCutoffStr);
            }
            
            if(respaLevelCutoff < 0 || respaLevelCutoff >= numberOfRespaLevels)
                throw new Exception("ERROR:: Respa Level: "+respaLevelCutoff+" is out of bounds");            
            
            if( isRespaLevelDefined[ respaLevelCutoff ] )
                switchLowerCutoffByShell[ respaLevelToShellIdMap[ respaLevelCutoff ] ] = cutoffValue;
                     
            String deltaValue= deltaTokens.nextToken();           

            int respaLevelDelta=0;
            if(deltaTokens.hasMoreTokens()) {
                String respaLevelDeltaStr = deltaTokens.nextToken();
                respaLevelDelta = Integer.parseInt( respaLevelDeltaStr );
            }

            if( respaLevelDelta < 0 || respaLevelDelta >= numberOfRespaLevels )
                throw new Exception("ERROR:: Respa Level: "+respaLevelDelta+" is out of bounds");
            
            if( isRespaLevelDefined[ respaLevelDelta ] )
                switchDeltaByShell[ respaLevelToShellIdMap[ respaLevelDelta ] ] = deltaValue;                        

            if( ProbspecgenDB2.debug ) {
                
                System.out.println("isRespaLevelDefined[ " + respaLevelDelta + " ]=" + isRespaLevelDefined[ respaLevelDelta ] );

                System.out.println("switchLowerCutoffByShell[ " + respaLevelToShellIdMap[ respaLevelCutoff ] + " ] = " 
                                   + switchLowerCutoffByShell[ respaLevelToShellIdMap[ respaLevelCutoff ] ] );
                                   
                System.out.println("cutoffValue = " + cutoffValue );
                System.out.println("switchDeltaByShell[ respaLevelToShellIdMap[ respaLevelDelta ] ]=" + switchDeltaByShell[ respaLevelToShellIdMap[ respaLevelDelta ] ] );
                System.out.println("deltaValue = " + deltaValue );
            }

        }

        out.write("const double MSD::SwitchLowerCutoffForShell[] = {\n");
        for(int i=0; i<numberOfShells; i++) {
            out.write( switchLowerCutoffByShell[ i ] );
            if(i!=numberOfShells-1)
                out.write(",\n");
        }
        out.write("\n};\n");

        out.write("const double MSD::SwitchDeltaForShell[] = {\n");
        for(int i=0; i<numberOfShells; i++) {
            out.write( switchDeltaByShell[ i ] );
            if(i!=numberOfShells-1)
                out.write(",\n");
        }
        out.write("\n};\n");
        
  double InnerTS = 0.0;
  double OuterTS = 0.0;	
  
  if(doOuter) {
      
      String outerTimeStepsInPicoSecondsStr = (String) params.get("OuterTimeStepInPicoSeconds");
      int colonIndex = outerTimeStepsInPicoSecondsStr.indexOf(":");
      double outerTimeStepsInPicoSeconds = Double.parseDouble(outerTimeStepsInPicoSecondsStr.substring(0, colonIndex));
      
      OuterTS = outerTimeStepsInPicoSeconds;
      InnerTS = OuterTS / simTicksAtRespaLevel[ numberOfRespaLevels-1 ];
  }
  else {
      String innerTimeStepsInPicoSecondsStr = (String) params.get("InnerTimeStepInPicoSeconds");
      int colonIndex = innerTimeStepsInPicoSecondsStr.indexOf(":");
      double innerTimeStepsInPicoSeconds = Double.parseDouble(innerTimeStepsInPicoSecondsStr.substring(0, colonIndex));
      
      InnerTS = innerTimeStepsInPicoSeconds;
      OuterTS = InnerTS/simTicksAtRespaLevel[ numberOfRespaLevels-1 ];
  }

  out.write("const double MSD::ConvertedInnerTimeStep = " + String.valueOf(InnerTS) + "/MSD::TimeConversionFactorMsd ;\n\n");

  out.write("const double MSD::IntegratorLevelConvertedTimeDelta[] = {\n");
  for( int respaLevel=0; respaLevel < numberOfRespaLevels; respaLevel++) {
      out.write("MSD::ConvertedInnerTimeStep * " + String.valueOf(simTicksAtRespaLevel[ respaLevel ])); 
      if(respaLevel != numberOfRespaLevels-1) {
    out.write(",");
      }
      out.write("\n");
  }
  out.write("};\n");

  String numberOfSimTicksStr = "";
  int numberOfSimTicks = 0;
  if(doOuter) {
      String NumberOfOuterTimeStepsStr = (String) params.get("NumberOfOuterTimeSteps");
      int colonIndex = NumberOfOuterTimeStepsStr.indexOf(":");
      int NumberOfOuterTimeSteps = Integer.parseInt(NumberOfOuterTimeStepsStr.substring(0, colonIndex));	    
      numberOfSimTicksStr = "( NumberOfOuterTimeSteps * " + simTicksAtRespaLevel[ numberOfRespaLevels - 1] + " )";
      numberOfSimTicks = NumberOfOuterTimeSteps * simTicksAtRespaLevel[ numberOfRespaLevels - 1];
  }
  else {
      String NumberOfInnerTimeStepsStr = (String) params.get("NumberOfInnerTimeSteps");
      int colonIndex = NumberOfInnerTimeStepsStr.indexOf(":");
      int NumberOfInnerTimeSteps = Integer.parseInt(NumberOfInnerTimeStepsStr.substring(0, colonIndex));
      numberOfSimTicksStr = String.valueOf(NumberOfInnerTimeSteps);
      numberOfSimTicks = NumberOfInnerTimeSteps;
  }
  
  if( 0 != numberOfSimTicks % simTicksAtRespaLevel[ numberOfRespaLevels - 1]) {
       numberOfSimTicks += ( numberOfSimTicks % simTicksAtRespaLevel[ numberOfRespaLevels - 1] );
             numberOfSimTicksStr = numberOfSimTicksStr 
                 + " +  ( " + numberOfSimTicksStr + " % " + simTicksAtRespaLevel[ numberOfRespaLevels - 1] + " )";
  }
        
        out.write("#endif\n");
  out.write("\ninline int MSD::GetNumberOfSimTicks()\n");
  out.write("{\n");
  out.write("int simTicks = " + numberOfSimTicksStr + ";\n" );
  out.write("return simTicks;\n");
  out.write("}\n\n");

        out.write("#define MSD_GetNumberOfSimTicks_Method\n");

        out.write("#ifndef MSD_COMPILE_CODE_ONLY\n");

  //out.write("int MSD::NumberOfSimTicks = " + numberOfSimTicksStr + ";\n\n");
    }
    
    public static String getRTPDefinesAsString() {
  String result = "";
  
  Enumeration keys = enumParams.keys();

  while(keys.hasMoreElements()) {
      String key = (String) keys.nextElement();
      String value = (String) enumParams.get( key );
      
      if (key.equals("LongRangeForceMethod")) {
    result += "#define MSDDEF_" + value + "\n\n";
      }
      else {
    result += "#define MSDDEF_" + key + "\t" + value + "\n\n";
      }
  }
    
  return result;
    }

    public static String getRTPEnumAsString() {
  String result = "";
  
  Enumeration keys = enumParams.keys();

  while(keys.hasMoreElements()) {
      String key = (String) keys.nextElement();
      String value = (String) enumParams.get( key );
      result += "enum { " + key + " = " + value + " };\n";
  }

// 	result += "enum { MaxRespaLevels = 3 };\n";
// 	result += "enum { STDCOULOMB, EWALD, P3ME, FTC, NO_LRF };\n";
// 	result += "enum { GRADIENT=-1, ANALYTICAL=0, FINITE_2PT=1, FINITE_4PT=2,"
// 	    + " FINITE_6PT=3, FINITE_8PT=4, FINITE_10PT=5, FINITE_12PT=6 };\n";
  
  return result;
    }
    
    public static void getCTPFromDB2() throws Exception {
        fillEnumParamsFromDB2();
        fillRespaParamsFromDB2();
        fillParamsFromDB2();
    }

    public static boolean DoAndersonTempControl() throws Exception 
    {
        String tempControl = getCTParameterFromDB2("TempControl");
        return ( tempControl != null && tempControl.equals("Anderson") );
    }

    private static void fillEnumParamsFromDB2()  throws Exception {
        
        
        String tempControl = getCTParameterFromDB2("TempControl");
        if( tempControl != null && tempControl.equals("NoseHoover")) {
            enumParams.put("NOSE_HOOVER", "1");    
            doNoseHoover=true;
        }

        String replicaExchange = getCTParameterFromDB2( "ReplicaExchange" );
        if( replicaExchange != null && replicaExchange.equals( "1" ) )
          {
          enumParams.put("ReplicaExchange", "1");          
          }        
 
  String Stochastic = getCTParameterFromDB2( "Stochastic" );
        if( Stochastic != null && Stochastic.equals( "1" ) )
          {
          enumParams.put("Stochastic", "1");          
          }

        String integrator = getCTParameterFromDB2("Integrator");
        if(integrator.equals("Respa")) {
            enumParams.put("DoRespa", "1");    
        }

        String lrfMethod = getCTParameterFromDB2("LRFMethod");
        if(lrfMethod == null)
            throw new Exception("ERROR:: LRFMethod is not set!");

  enumParams.put("LongRangeForceMethod", lrfMethod.toUpperCase());

        String waterMethod = getCTParameterFromDB2("WaterModel"); 

        if(waterMethod!=null) {
            
            waterMethod = waterMethod.toUpperCase();
            if(!waterMethod.endsWith("FLOPPY") && !waterMethod.equals("NONE")) {
                enumParams.put("DoShakeRattleWater", "1");     //turn shake off by default	
                enumParams.put("DoShakeRattle", "1");     //turn shake off by default	
            }
        }
        

         String proteinShakeRattle = getCTParameterFromDB2("RigidProtein");

         if(proteinShakeRattle!=null) {

             proteinShakeRattle = proteinShakeRattle.toUpperCase();
             
             if(proteinShakeRattle.equals("ON")) {
                 enumParams.put("DoShakeRattleProtein", "1");
                 enumParams.put("DoShakeRattle", "1");     //turn shake off by default	
             }
         }

        String ComputeVirialFlag = getCTParameterFromDB2("ComputeVirial");
        
        if( ComputeVirialFlag == null )
            ComputeVirialFlag = "0";

        enumParams.put("DoComputeVirial", ComputeVirialFlag );
         
        String ensemble = getCTParameterFromDB2("Ensemble");

        if(ensemble != null)
            ensemble= ensemble.toUpperCase();
            
            if(ensemble.equals("NPT")) {
                if(doNoseHoover) {
                    enumParams.put("DoVelocityResample", "0");
    } else {
                    enumParams.put("DoVelocityResample", "1");
                    enumParams.put("NOSE_HOOVER", "0");    
    }
                enumParams.put("DoPressureControl", "1");
                enumParams.put("DoTemperatureControl", "1");
                enumParams.put("DoComputeVirial", "1");
            }
            else if(ensemble.equals("NVT")) {
                if(doNoseHoover) {
                    enumParams.put("DoVelocityResample", "0");
    } else {
                    enumParams.put("DoVelocityResample", "1");
                    enumParams.put("NOSE_HOOVER", "0");    
    }
                enumParams.put("DoNVT", "1");
                enumParams.put("DoTemperatureControl", "1");
            }
            else if(ensemble.equals("NPH")) {
                enumParams.put("DoPressureControl", "1");
                enumParams.put("DoComputeVirial", "1");
                enumParams.put("DoVelocityResample", "0");
                enumParams.put("DoTemperatureControl", "0"); 
                enumParams.put("NOSE_HOOVER", "0");    
            }
            else if(ensemble.equals("NVE")) {
                enumParams.put("DoPressureControl", "0");
                enumParams.put("DoVelocityResample", "0");
                enumParams.put("DoTemperatureControl", "0");
                enumParams.put("NOSE_HOOVER", "0");    
            }

        String fep = getCTParameterFromDB2("FEP");      

        if(fep!=null) {
            
            fep = fep.toUpperCase();
            
            if(fep.equals("ON"))
                enumParams.put("DoFreeEnergy", "1"); 
        }

        try {
            String waterInit = getCTParameterFromDB2("WaterInit");
            if(waterInit!=null) {
                
                waterInit = waterInit.toUpperCase();
                if(waterInit.equals("ON"))
                    enumParams.put("DoWaterInit", "1"); 
            }
        } catch (Exception e) {}

        try {
            String nsqCheck = getCTParameterFromDB2("CheckSitesInBox");
         
            if(nsqCheck!=null) {
                nsqCheck = nsqCheck.toUpperCase();
                
                if(nsqCheck.equals("ON"))
                    enumParams.put("DoNSQCheck", "1"); 
            }
        } catch (Exception e) {}

    }

    private static String getCTParameterFromDB2(String name) throws Exception {
        ResultSet rs = stmt.executeQuery("SELECT value FROM EXPERIMENT.ct_params "
                                         + "WHERE ctp_id="+ctpId
                                         + " AND name='"+name+"'");
        if(!rs.next()) {
            System.out.println("WARNING:: Name: " + name + " does not exist for ctp_id: " + ctpId);
            return null;
            //            throw new Exception("ERROR: Name: " + name + " does not exist for ctp_id: " + ctpId );            
        }
        return rs.getString("value").trim();
        
    }

    private static void fillRespaParamsFromDB2() throws Exception {
        ResultSet rs = stmt.executeQuery("SELECT * FROM EXPERIMENT.ct_params "
                                         + "WHERE ctp_id="+ctpId
                                         + " AND ((name LIKE 'DefineRespaLevel___') OR (name LIKE 'DefineRespaLevel__')) order by name");
        
        while(rs.next()) {
            String name = rs.getString("name"); //DefineRespaLevel+RespaLevel
            String value = rs.getString("value"); // SimTicks
            
            incRespaLevels();
            ///            doRespa = true;
            
//             String respaLevel = value.substring(0,value.indexOf(":"));
//             String simTicks = value.substring(value.indexOf(":")+1, value.length());
            //name.replace(' ',':');
            respaParams.put(name, value);
        }

        rs = stmt.executeQuery("SELECT * FROM EXPERIMENT.ct_params "
                                         + "WHERE ctp_id="+ctpId
                                         + " AND name LIKE 'DefineRespaLevelForForceCategory%' order by name");
        
        int i=0;
        while( rs.next() ) {
            String name = rs.getString("name").trim();
            String value = rs.getString("value").trim();
            
            // space ' ' <-- is the delimiting character
            String categoryAndRespaLevel = name.substring(name.indexOf(" ")+1, name.length());
            name = name.substring(0,name.indexOf(" "));
            
            String category = categoryAndRespaLevel.substring( 0, categoryAndRespaLevel.indexOf(" ") );

            incRespaForCategoryCount();
            respaParams.put(name + i, category+":"+value);
            i++;
        }

        rs = stmt.executeQuery("SELECT * FROM EXPERIMENT.ct_params "
                               + "WHERE ctp_id="+ctpId
                               + " AND name LIKE 'SwitchLowerCutoff%' order by name");        
        i=0;
        while( rs.next() ) {           
            String value = rs.getString("value").trim();
            // Value == "cutoff respa_level"            
            respaParams.put("SwitchLowerCutoff"+i, value);                
            i++;            
            numberOfSwitchLowerCutoffDefines++;
        }

        rs = stmt.executeQuery("SELECT * FROM EXPERIMENT.ct_params "
                               + "WHERE ctp_id="+ctpId
                               + " AND name LIKE 'SwitchDelta%' order by name");        
        i=0;
        while( rs.next() ) {           
            String value = rs.getString("value").trim();
            // Value == "delta respa_level"            
            respaParams.put("SwitchDelta"+i, value);
            i++;
            numberOfSwitchDeltaDefines++;
        }

        if( numberOfSwitchLowerCutoffDefines != numberOfSwitchDeltaDefines ) {
            throw new Exception("ERROR: The number of SwitchLowerCutoff defines does not match the number of SwitchDelta defines");
        }
    }

    private static void fillParamsFromDB2() throws Exception {
    
        ResultSet rs = stmt.executeQuery("SELECT * FROM EXPERIMENT.ct_params "
                               + "WHERE ctp_id="+ctpId);

        while(rs.next()) {
            String name = rs.getString("name");
            String value = rs.getString("value");
            String type = rs.getString("type");

            if(type.equals("method"))
                continue;

            if(name.equals("OuterTimeStepInPicoSeconds") && !value.equals("-1")) {
                doOuter = true;
                params.put("DoOuter","1:int");
            }
            
            params.put(name,value+":"+type);
        }
    }


    public static void processRunTimeParameters( UDF_RegistryIF udfRegistry ) throws Exception {
  
  String methodName = (String) enumParams.get("LongRangeForceMethod");
  
  // int start = lrfFlag.lastIndexOf("::"); 
  // int end = lrfFlag.lastIndexOf(":");
  // String methodName = lrfFlag.substring(start+2, lrfFlag.length());
  
  if ( methodName.equals("EWALD" ) ) {
      ProbspecgenDB2.doEwald = true;
      longRangeForceMethod = EWALD;
  }
  else if ( methodName.equals("P3ME") )  {
      ProbspecgenDB2.doEwald = true;
      longRangeForceMethod = P3ME;
  }
  else if( methodName.equals("FRC" )) {
      longRangeForceMethod = FRC;
  }
  else if( methodName.equals("STDCOULOMB" )) {
      longRangeForceMethod = STDCOULOMB;
  }
  else if( methodName.equals("LEKNER" )) {
      longRangeForceMethod = LEKNER;
  }
  else {
      ProbspecgenDB2.no14 = true;
      longRangeForceMethod = NO_LRF;
  }
  
  doPressureControl = getBooleanForName("DoPressureControl");
  doTemperatureControl = getBooleanForName("DoTemperatureControl");
  doShakeRattleWater = getBooleanForName("DoShakeRattleWater");
  doShakeRattleProtein = getBooleanForName("DoShakeRattleProtein");
  doNVT = getBooleanForName("DoNVT");
  doFreeEnergy = getBooleanForName("DoFreeEnergy");
        doNoseHoover = getBooleanForName("NOSE_HOOVER");
        


  String fragmentation = getCTParameterFromDB2("Fragmentation");
  
  if(fragmentation == null) {
            ProbspecgenDB2.fragment_mode = ProbspecgenDB2.RESIDUE_FRAGMENTATION;
  }	
        if (fragmentation.equals("nmnf")) {
            // ProbspecgenDB2.fragment_mode = ProbspecgenDB2.RESIDUE_FRAGMENTATION;            
            ProbspecgenDB2.fragment_mode = ProbspecgenDB2.ALL_MOLECULES_IN_THEIR_OWN_FRAGMENT;
            //System.out.println("WARNING: testing RIGID_GROUP_FRAGMENTATION");
            //ProbspecgenDB2.fragment_mode = ProbspecgenDB2.RIGID_GROUP_FRAGMENTATION;
        }
  else if(fragmentation.equals("amof")) {
      ProbspecgenDB2.fragment_mode = ProbspecgenDB2.ALL_MOLECULES_IN_ONE_FRAGMENT;
  }
  else if(fragmentation.equals("nsnf")) {
      ProbspecgenDB2.fragment_mode = ProbspecgenDB2.ALL_SITES_IN_THEIR_OWN_FRAGMENT;
  }
  else if(fragmentation.equals("asof")) {
      ProbspecgenDB2.fragment_mode = ProbspecgenDB2.ALL_SITES_IN_ONE_FRAGMENT;
  }
  else if(fragmentation.equals("residue")) {
      ProbspecgenDB2.fragment_mode = ProbspecgenDB2.RESIDUE_FRAGMENTATION;
  }
        else if(fragmentation.equals("rigid_group")) {
            ProbspecgenDB2.fragment_mode = ProbspecgenDB2.RIGID_GROUP_FRAGMENTATION;
        }
  else {
      throw new Exception ("Fragmentation option: "+fragmentation+" is not recognized");
  }
  
  
        String waterMethod = getCTParameterFromDB2("WaterModel");
  // String waterMethod = (String) probspecParams.get("WaterMethod");
        if(waterMethod !=null)
            waterMethod = waterMethod.toUpperCase();

  if ( waterMethod != null && !waterMethod.equals("NONE")) {
      
            
      if( waterMethod.equals("TIP3P") ) {
    ProbspecgenDB2.doTIP3P = true;
      }
      else if( waterMethod.equals("TIP3PFLOPPY") ) {
    ProbspecgenDB2.doTIP3PFloppy = true;
      }
      else if( waterMethod.equals("SPC") || waterMethod.equals("SPCE"))  {
    ProbspecgenDB2.doSPC = true;
      }
      else if( waterMethod.equals("SPCFLOPPY") || waterMethod.equals("SPCEFLOPPY")) {
    ProbspecgenDB2.doSPCFloppy = true;
      }
  }

        String integrator = getCTParameterFromDB2("Integrator");
        doRespa = integrator.equals("Respa");

  return;
    }
  
    public static void setParam (String paramName, Object paramValue) {
  params.put(paramName, paramValue);
    }

    public static void setEnumParam (String paramName, Object paramValue) {
  enumParams.put(paramName, paramValue);
    }
    
    public static void setRespaParam (String paramName, Object paramValue) {
  respaParams.put(paramName, paramValue);
    }

    /**
     * Debugging tool
     */
    public static void dumpParams( int systemId ) throws IOException {
     
        // int index = fileName.lastIndexOf(".rtp");

        FileWriter fOut = new FileWriter(fileName + "_" + systemId+".rtp");

  fOut.write("#RTP File name: " + fileName+"\n");
  fOut.write("#System Id: " + systemId +"\n\n");

  fOut.write("#Runtime parameters:\n\n");
        dumpHashtable(params, fOut);
        dumpHashtable(enumParams, fOut);
        dumpHashtable(respaParams, fOut);
        // dumpHashtable(probspecParams, fOut);
        fOut.write("#******************************************************************************");
        fOut.flush();
        fOut.close();
    }

    private static void dumpHashtable(Hashtable hash, FileWriter out) throws IOException {
        Enumeration keys = hash.keys();
        while(keys.hasMoreElements()) {
            String key = (String) keys.nextElement();
            String val =  (String) hash.get(key);
            
            out.write(key+": " + val+"\n");
        }
    }

    public static void incRespaLevels() {
  numberOfRespaLevels++;
    }

    public static int getNumberOfRespaLevels() {
  return numberOfRespaLevels;
    }

    public static void incRespaForCategoryCount() {
  numberOfRespaForCategoryDefines++;
    }

    public static int getRespaForCategoryCount() {
  return numberOfRespaForCategoryDefines;
    }

    public static void doRespaFlag() {
  doRespa = true;
    }
    
    public static void constraintsOn() {
  doConstraints = true;
    } 

    public static void fepOn() {
  doFep = true;
    } 

    public static void doInner() {
  doInner = true;
    } 

    public static void dumpFFTPlan( FileWriter out ) throws Exception {

        String fftSizes = getCTParameterFromDB2("FFTSize");
        if(fftSizes == null)
            throw new Exception("");

        StringTokenizer fftSizesTokens = new StringTokenizer( fftSizes );
        if( fftSizesTokens.countTokens() != 3 ) 
            throw new Exception("ERROR:: Corrupt fftSizes.");

        String xSize = fftSizesTokens.nextToken();
        String ySize = fftSizesTokens.nextToken();
        String zSize = fftSizesTokens.nextToken();

        out.write("struct FFT_PLAN\n");
        out.write("{\n");
        out.write("enum {\n");
        out.write("       GLOBAL_SIZE_X = " + xSize + ",\n");
        out.write("       GLOBAL_SIZE_Y = " + ySize + ",\n");
        out.write("       GLOBAL_SIZE_Z = " + zSize + "\n");        
        out.write("     };\n\n");
        out.write(ImplManager.getFFTProcessorMeshSize() + "\n");
        out.write("};\n\n\n");
    }
}
