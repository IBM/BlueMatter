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
 package com.ibm.bluematter.utils;

import java.sql.*;
import java.text.*;
import java.io.*;
import java.util.*;

public class Rtp2db2 {

    private static Statement stmt = null;
    private static Statement stmt1 = null;
    private static boolean debug = false;

    static {
      try {
         //  register the driver with DriverManager 
         //  The newInstance() call is needed for the sample to work with 
         //  JDK 1.1.1 on OS/2, where the Class.forName() method does not 
         //  run the static initializer. For other JDKs, the newInstance 
         //  call can be omitted. 
         Class.forName("COM.ibm.db2.jdbc.app.DB2Driver").newInstance();
      } catch (Exception e) {
         e.printStackTrace();
      }
   }    

/**
*   The main program sets up all the neccesary tables for  probspecgen
*/
   public static void main(String argv[]) {

      int argc = argv.length;
      if(argc < 1 || argv[0].equals("-usage") ) {
    usage();
    return;
      }

      String rtpFile = argv[0];

      if(!rtpFile.endsWith(".rtp") || rtpFile.equals("")) {
          System.out.println("The first argument does not end in .rtp ");
          System.exit(1);
      }
      
      String databaseName = new String("mdsetup");
      String rtpDesc      = null;

      int i=1;
      while (i < argc) {
    if( argv[i].equals("-database")) {
        if((i != argc-1) || argv[i+1].startsWith("-")) {
      databaseName = argv[i+1];
        }
        else {
      usage();
      return;
        }
    }
          else if( argv[i].equals("-desc")) {
        if((i != argc-1) || argv[i+1].startsWith("-")) {
      rtpDesc = argv[i+1];
        }
        else {
      usage();
      return;
        }
          }
          else if( argv[i].equals("-debug")) {
              debug=true;
          }
    i++;
      }      

      try {
          SimpleCtpParser parser = new SimpleCtpParser( rtpFile );
          Hashtable ctp = parser.parse();

          // If parsing of the file went well, then create a new ctp_id
          String ctpFilename = rtpFile.substring(rtpFile.lastIndexOf("/")+1, rtpFile.lastIndexOf(".rtp"));
          
          //  connect with default id/password 
          Connection con = DriverManager.getConnection( "jdbc:db2:" + databaseName );
          stmt = con.createStatement();
          stmt1 = con.createStatement();
          
          Runtime runTime = Runtime.getRuntime();
          Process p1 = runTime.exec("whoami");
          BufferedReader in = new BufferedReader(new InputStreamReader(p1.getInputStream()));
          
          String creator = (in.readLine()).trim();
          in.close();
          
          if(creator == null) {
              System.out.println("ERROR: Program is not able to establish creator");
              System.exit(1);
          }

          Timestamp now = new Timestamp((new java.util.Date()).getTime());
          String timeString = now.toString();

          if(rtpDesc != null)
              stmt.executeUpdate("INSERT INTO EXPERIMENT.ctp_table ( filename,desc,creator,created ) "
                                 +" VALUES ('" + ctpFilename + "','"+rtpDesc+"','"+creator+"','"+timeString+"')");
          else
              stmt.executeUpdate("INSERT INTO EXPERIMENT.ctp_table ( filename,creator,created ) "
                                 +" VALUES ('" + ctpFilename + "','"+creator+"','"+timeString+"')");
          
          
          ResultSet rs = stmt.executeQuery("SELECT ctp_id FROM EXPERIMENT.ctp_table "
                                           + "WHERE filename='"+ctpFilename+"'"
                                           + " AND creator='"+creator+"'"
                                           + " AND created='"+timeString+"'" );

          if(!rs.next())
              throw new Exception ("ERROR: Problem occured while retrieving the compiler time parameters from db2");
          
          int ctp_id = rs.getInt(1);

          // Init with defaults in db2 for ctp_id
          initDefaults( ctp_id );
          
          // ctp hashtable contains the mapping of name --> value to be put into db2
          boolean switchDeltaSeen=false;
          boolean switchDeltaSeen0=false;
          boolean switchLowerCutoffSeen=false;
          boolean switchLowerCutoffSeen0=false;
          Enumeration keys = ctp.keys();          
          while(keys.hasMoreElements()) {
              String key = (String) keys.nextElement();
              String value = (String) ctp.get(key);
              String type = "";
 
              int colonIndex = value.indexOf(":");
              if(colonIndex != -1)
                  type = (String) value.substring(colonIndex+1, value.length());

              if(key.startsWith("SwitchLowerCutoff")) {
                  switchLowerCutoffSeen=true;
                  if(key.startsWith("SwitchLowerCutoff 0")) {
                      switchLowerCutoffSeen0=true;
                  }                  
              }

              if(key.startsWith("SwitchDelta")) {
                  switchDeltaSeen=true;
                  if(key.startsWith("SwitchDelta 0")) {
                      switchDeltaSeen0=true;
                  }                  
              }
//               System.out.println("Key: " + key);
//               System.out.println("Value: " + value);

              if(key.startsWith("DefineRespaLevelForForceCategory") || key.startsWith("DefineRespaLevel") || 
                 key.startsWith("SwitchLowerCutoff") || key.startsWith("SwitchDelta")) {
                  
                  stmt1.executeUpdate("INSERT INTO EXPERIMENT.ct_params (ctp_id,name,value,type) "
                                      + " VALUES ("+ctp_id+",'"+key+"','"+value+"','method')");                  
              } 
              else {                                    
                  stmt.executeUpdate("UPDATE EXPERIMENT.ct_params SET value='"+value+"' "
                                     +"WHERE ctp_id=" + ctp_id + " AND name='"+key+"'");
              }
          }

          // If certain ctp parameters were not set add them in as defaults
          // 1. SwitchLowerCutoff Default: SwitchLowerCutoff0 1E100
          // 2. SwitchDelta Default: SwitchDelta0 1.0

          if(!switchLowerCutoffSeen0) {
                  stmt1.executeUpdate("INSERT INTO EXPERIMENT.ct_params (ctp_id,name,value,type) "
                                      + " VALUES ("+ctp_id+",'SwitchLowerCutoff 0','1.0E100 0','method')");                                
          }

          if(!switchDeltaSeen0) {
                  stmt1.executeUpdate("INSERT INTO EXPERIMENT.ct_params (ctp_id,name,value,type) "
                                      + " VALUES ("+ctp_id+",'SwitchDelta 0','1.0 0','method')");                                
          }

          System.out.println("CTP_ID: " + ctp_id);
      }
      catch (Exception e) {
          System.out.println(e.getMessage());
          e.printStackTrace();
      }      
   }
    
    private static void initDefaults( int ctp_id ) throws Exception {

        ResultSet rs = stmt.executeQuery("SELECT name FROM EXPERIMENT.vocab");
        
        String value = null;
        String type = null;

        while(rs.next()) {
            String name = rs.getString("name");
                        
            if(name.equals("ShakeTolerance") || name.equals("RattleTolerance")) {
                value = "1e-8"; type = "double";
            }
            else if(name.equals("ShakeMaxIterations") || name.equals("RattleMaxIterations")) {
                value = "1000"; type = "int";
            }
            else if(name.equals("VelocityResamplePeriodInOTS")) {
                value = "0"; type = "int";
            }
            else if(name.equals("SimpleRespaRatio")) {
                value = "1"; type = "int";
            }
            else if(name.equals("VelocityResampleTargetTemperature")) {
                value = "0.0"; type = "double";
            }
            else if(name.equals("EwaldAlpha")) {
                value = "0.0"; type = "double";
            }
            else if(name.equals("LeknerEpsilon")) {
                value = "0.0"; type = "double";
            }
            else if(name.equals("EwaldKmax")) {
                value = "0"; type = "int";
            }
            else if(name.equals("P3MEchargeAssignment")) {
                value = "4"; type = "int";
            }
            else if(name.equals("P3MEdiffOrder")) {
                value = "2"; type = "int";
            }
            else if(name.equals("InnerTimeStepInPicoSeconds")) {
                value = "-1.0"; type = "double";
            }
            else if(name.equals("NumberOfInnerTimeSteps")) {
                value = "-1"; type = "int";
            }
            else if(name.equals("OuterTimeStepInPicoSeconds")) {
                value = "-1"; type = "double";
            }
            else if(name.equals("NumberOfOuterTimeSteps")) {
                value = "-1"; type = "int";
            }
            else if(name.equals("NumberOfConstraints")) {
                value = "3"; type = "int";
            }
            else if(name.equals("PressureControlPistonMass")) {
                value = "-1.0"; type = "double";
            }
            else if(name.equals("PressureControlTarget")) {
                value = "1.0"; type = "double";
            }
            else if(name.equals("PressureControlPistonInitialVelocity")) {
                value = "0.0"; type = "double";
            }
            else if(name.equals("ExternalPressure")) {
                value = "1.0"; type = "double";
            }
//             else if(name.equals("SwitchLowerCutoff")) {
//                 value = "1.0E100 0"; type = "double";
//             }
//             else if(name.equals("SwitchDelta")) {
//                 value = "1.0 0"; type = "double";
//             }
//             else if(name.equals("P3MEinitSpacing")) {
//                 value = "{.5,.5,.5}"; type = "XYZ";
//             }
            else if(name.equals("FFTSize")) {
                value = "63 63 63"; type = "method";
            }            
            else if(name.equals("BoundingBoxDef")) {
                value = "{0,0,0,0,0,0}"; type = "BoundingBox";
            }
            else if(name.equals("VelocityResampleTargetTemperature")) {
                value = "0.0"; type = "double";
            }
            else if(name.equals("EmitEnergyTimeStepModulo")) {
                value = "1"; type = "int";
            }
            else if(name.equals("SnapshotPeriodInOTS")) {
                value = "-1"; type = "int";
            }
            else if(name.equals("VelocityResampleRandomSeed")) {
                value = "51579"; type = "int";
            }            
            else if( name.equals("ReplicaExchangeRandomSeed") ) {
                value = "51579"; type = "int";
            }            
            else if(name.equals("Lambda")) {
                value = "0.0"; type = "double";
            }            
            else if(name.equals("WaterModel") ){
                value = "none"; type = "method";
            }          
            else if( name.equals("Integrator") ) {
                value = "Verlet"; type = "method";
            }           
            else if( name.equals("ReplicaExchange") ) {
                value = "0"; type = "int";
            }           
            else if( name.equals("Stochastic") ) {
                value = "0"; type = "int";
            }           
            else if( name.equals("SwapPeriodOTS") ) {
                value = "99999999"; type = "int";
            }           
            else if( name.equals("NumOfTemperatureAttempts") ) {
                value = "1"; type = "int";
            }           
            else if( name.equals("TempControl") ) {
                value = "Anderson"; type = "method";
            }           
            else if( name.equals("NumberOfSitesPerNoseHooverChain") ) {
                value = "3"; type = "int";
            }           
            else if( name.equals("NoseHooverMassFactor") ) {
                value = "1.0"; type = "double";
            }           
            else if( name.equals("NumberOfNoseHooverRespaLevels") ) {
                value = "2"; type = "int";
            }           
            else if( name.equals("Constraints") ) {
                value = "off"; type = "method";
            }                       
            else if( name.equals("FEP") ) {
                value = "off"; type = "method";
            }                       
            else if( name.equals("RigidProtein") ) {
                value = "off"; type = "method";
            }                       
            else if( name.equals("CheckSitesInBox") ) {
                value = "off"; type = "method";
            }                       
            else if( name.equals("WaterInit") ) {
                value = "off"; type = "method";
            }                       
            else if( name.equals("Fragmentation") ) {
                value = "nmnf"; type = "method";
            }                       
            else if( name.equals("Ensemble") ) {
                value = "NVE"; type = "method";
            }                       
            else if( name.equals("LRFMethod") ) {
                value = "StdCoulomb"; type = "method";
            }                       
            else if( name.equals("ComputeVirial") ) {
                value = "0"; type = "int";
            }                       
            else if( name.startsWith("DefineRespaLevel") ||  name.startsWith("DefineRespaLevelForForceCategory") ) {
                continue;
            }                       
            else {
                if(debug)
                    System.out.println("WARNING: Name not matched: " + name);
                continue;
            }

            stmt1.executeUpdate("INSERT INTO EXPERIMENT.ct_params (ctp_id,name,value,type) "
                               + " VALUES ("+ctp_id+",'"+name+"','"+value+"','"+type+"')");
        }
        
    }


    private static void usage() {
  System.out.println("Input: <rtp_filename> [flags]");
  System.out.println("flags: -database <database_name> --> \"mdsetup\" by default");
  return;
    }
}
