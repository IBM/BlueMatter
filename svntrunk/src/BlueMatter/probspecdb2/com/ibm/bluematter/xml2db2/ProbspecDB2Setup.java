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
 package com.ibm.bluematter.xml2db2;

import java.sql.*;
import java.text.*;
import java.io.*;
import java.util.*;


class ProbspecDB2Setup {

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
      if(argc < 1) {
	usage();  
	return;
      }

      String xmlFile = argv[0];
      // Gets the name of the system. The token before the first dot
      File xmlFileObj = new File(xmlFile);
      
      String xmlFileName = xmlFileObj.getName();
      String systemName = xmlFileName.substring(0,xmlFileName.lastIndexOf(".xml"));
      System.out.println("Processing system: " + systemName );
      String potentialSysIdentName = systemName.toUpperCase();

      int i=1;
      String databaseName = new String("mdsetup");
      String creatorName = "null";
      String className = "scratch";

      
      String defSysIdentName = null;
      boolean overwrite = false;
      boolean sysIdentDefined = false;
      String dirName = null;
      while( i < argc ) {
	  if (argv[i].equals("-database")) {
	      if((i==argc-1) || argv[i+1].startsWith("-")) {
		  usage();
		  return;
	      }
	      databaseName = argv[i+1];
          }
	  else if (argv[i].equals("-sys_class")) {
	      if((i==argc-1) || argv[i+1].startsWith("-")) {
		  usage();
		  return;
	      }
	      className = argv[i+1];
              if(!className.equals("scratch") || !className.equals("testing") || !className.equals("production")) {
                  usage();
                  return;
              }
          }
          else if (argv[i].equals("-creator")) {
	      if((i==argc-1) || argv[i+1].startsWith("-")) {
		  usage();
		  return;
	      }
	      creatorName = argv[i+1];
          } else if ( argv[i].equals("-sys_desc")) {
	      if((i==argc-1) || argv[i+1].startsWith("-") || (defSysIdentName != null)) {
		  usage();
		  return;
	      }
	      overwrite = true;
	      sysIdentDefined = true;
	      defSysIdentName = argv[i+1].toUpperCase();
	  } else if ( argv[i].equals("-sysid_warn")) {
	      if((i==argc-1) || argv[i+1].startsWith("-") || (defSysIdentName != null)) {
		  usage();
		  return;
	      }
	      sysIdentDefined = true;
	      overwrite = false;
	      defSysIdentName = argv[i+1].toUpperCase();
	  }  else if ( argv[i].equals("-dir")) {
	      if((i==argc-1) || argv[i+1].startsWith("-")) {
		  usage();
		  return;
	      }

	      dirName = argv[i+1];
	  } 
	  i++;
      } 
      
      if(dirName == null) {
	  usage();
	  return;
      }

      //  URL is jdbc:db2:dbname 
      Connection con = null;	     
      System.out.println( "Database: " + databaseName );
      String url = "jdbc:db2:" + databaseName;

      try {
         
            //  connect with default id/password 
         con = DriverManager.getConnection(url);
         
         Statement stmt = con.createStatement();
         Statement stmt1 = con.createStatement();
	 ResultSet rs = null;
	 ResultSet rs1 = null;
	 String sysIdentName = null;	 

	 if(sysIdentDefined) {
           sysIdentName = defSysIdentName;          
	 } else {
           sysIdentName = potentialSysIdentName;
	 }
	
         Timestamp now = new Timestamp((new java.util.Date()).getTime());
         String timeString = now.toString();

	 System.out.println("SYSTEM_DESC = " + sysIdentName);	 
         System.out.println("Creator = " + creatorName);	 
         System.out.println("Class = " + className);	 
         System.out.println("Created = " + timeString);	 

	 stmt.execute("INSERT INTO MDSYSTEM.SYSTEM (sys_desc, creator, created, sys_class )"
                      + " VALUES ('" + sysIdentName + "', '"+ creatorName+"','"+timeString+"', '"+className+"' )");

	 rs = stmt.executeQuery("SELECT SYS_ID, SYS_DESC FROM MDSYSTEM.SYSTEM "
                                +"WHERE SYS_DESC='" + sysIdentName 
                                + "' AND CREATOR='" + creatorName
                                + "' AND CREATED='" + timeString
                                + "' AND SYS_CLASS='" + className +"'");
	 rs.next();
	 int systemId = rs.getInt("SYS_ID");
	 
	 System.out.println("SYSTEM_ID = " + systemId);

         DB2TableNames tableNames = new DB2TableNames();

	 dumpImportScript("import.db2", dirName, databaseName, tableNames );

         stmt.close();
	 stmt1.close();
         con.close();
      } catch( Exception e ) {
	  e.printStackTrace();
      }
   }
    
    private static void usage() {
	System.out.println("Input: <xml_file> -dir <del files directory> [optional flags]");
	System.out.println("flags: -database <database_name>         \"mdsetup\" by default");
	System.out.println("flags: -sysid_overwrite <sysid_name>   overwrites the molecular dynamic system if it exists. ");
	System.out.println("flags: -sysid_warn <sysid_name>        if the MD system  exists it warns the user, does not overwrite");
	System.out.println("flags: -creator <creator_name>        Creator of the system");
	System.out.println("flags: -sys_class <class_name>        Purpose of the system (scratch, testing, production");
	return;
    }


    private static void dumpImportScript( String fileName, String dirName, String databaseName, DB2TableNames tableNames) {
	
	try {
	    //	    String systemPath = System.getProperty("user.dir")+ "/" + systemName;
	    String systemPath = dirName;
	    FileWriter out = new FileWriter(fileName);
	    out.write("connect to " + databaseName +";\n\n");

            for ( int i=0; i<tableNames.size(); i++) {
                String tableName = tableNames.getNthName(i);
                
                out.write("IMPORT FROM " + systemPath + "/"
                          +tableName+".DEL OF DEL COMMITCOUNT 1000000 INSERT INTO MDSYSTEM."+tableName+";\n");
                // out.write("set integrity for MDSYSTEM."+tableName+" ALL immediate unchecked;\n\n");
            }

	    out.write("disconnect " + databaseName + ";\n");
            
	    out.close();
	    
	} catch( Exception e ) {
	    e.printStackTrace();
	}
    }
    
// 	    out.write("IMPORT FROM " + systemPath + "/CHEM_COMP_INSTANCE.DEL OF DEL COMMITCOUNT 1000000 INSERT INTO MDSYSTEM.CHEM_COMP_INSTANCE;\n");
// 	    out.write("set integrity for MDSYSTEM.CHEM_COMP_INSTANCE ALL immediate unchecked;\n\n");

// 	    out.write("IMPORT FROM " + systemPath + "/MONOMER_INSTANCE.DEL OF DEL COMMITCOUNT 1000000 INSERT INTO MDSYSTEM.MONOMER_INSTANCE;\n");
// 	    out.write("set integrity for MDSYSTEM.MONOMER_INSTANCE ALL immediate unchecked;\n\n");

// 	    out.write("IMPORT FROM " + systemPath + "/SPECIFIC_MOLECULE.DEL OF DEL COMMITCOUNT 1000000 INSERT INTO MDSYSTEM.SPECIFIC_MOLECULE;\n");
// 	    out.write("set integrity for MDSYSTEM.SPECIFIC_MOLECULE ALL immediate unchecked;\n\n");

// 	    out.write("IMPORT FROM " + systemPath + "/MOLECULE_INSTANCE.DEL OF DEL COMMITCOUNT 1000000 INSERT INTO MDSYSTEM.MOLECULE_INSTANCE;\n");
// 	    out.write("set integrity for MDSYSTEM.MOLECULE_INSTANCE ALL immediate unchecked;\n\n");

// 	    out.write("IMPORT FROM " + systemPath + "/SITE.DEL OF DEL COMMITCOUNT 1000000 INSERT INTO MDSYSTEM.SITE;\n");
// 	    out.write("set integrity for MDSYSTEM.SITE ALL immediate unchecked;\n\n");
	    
// 	    out.write("IMPORT FROM " + systemPath + "/PARAM_TUPLE_LIST.DEL OF DEL COMMITCOUNT 1000000 INSERT  INTO "
// 		      + "MDSYSTEM.PARAMTUPLELIST;\n");
// 	    out.write("set integrity for MDSYSTEM.PARAMTUPLELIST ALL immediate unchecked;\n\n");

// 	    out.write("IMPORT FROM " + systemPath + "/PARAM_TUPLE.DEL OF DEL COMMITCOUNT 1000000 INSERT  INTO " 
// 		     + "MDSYSTEM.PARAMTUPLE;\n");
// 	    out.write("set integrity for MDSYSTEM.PARAMTUPLE ALL immediate unchecked;\n\n");

// 	    out.write("IMPORT FROM " + systemPath + "/PARAM_TUPLE_SPEC.DEL OF DEL COMMITCOUNT 1000000 INSERT  INTO " 
// 		      + "MDSYSTEM.PARAMTUPLESPEC;\n");
// 	    out.write("set integrity for MDSYSTEM.PARAMTUPLESPEC ALL immediate unchecked;\n\n");

// 	    out.write("IMPORT FROM " + systemPath + "/PARAM_TUPLE_DATA.DEL OF DEL COMMITCOUNT 1000000 INSERT  INTO " 
// 		      + "MDSYSTEM.PARAMTUPLEDATA;\n");
// 	    out.write("set integrity for MDSYSTEM.PARAMTUPLEDATA ALL immediate unchecked;\n\n");

//  	    out.write("IMPORT FROM " + systemPath + "/SITE_TUPLE_LIST.DEL OF DEL COMMITCOUNT 1000000 INSERT  INTO " 
//  		      + "MDSYSTEM.SITETUPLELIST;\n");
//  	    out.write("set integrity for MDSYSTEM.SITETUPLELIST ALL immediate unchecked;\n\n");

//  	    out.write("IMPORT FROM " + systemPath + "/SITE_TUPLE.DEL OF DEL COMMITCOUNT 1000000 INSERT  INTO " 
//  		      + "MDSYSTEM.SITETUPLE;\n");
//  	    out.write("set integrity for MDSYSTEM.SITETUPLE ALL immediate unchecked;\n\n");

//  	    out.write("IMPORT FROM " + systemPath + "/SITE_TUPLE_DATA.DEL OF DEL COMMITCOUNT 1000000 INSERT INTO " 
//  		      + "MDSYSTEM.SITETUPLEDATA;\n");
//  	    out.write("set integrity for MDSYSTEM.SITETUPLEDATA ALL immediate unchecked;\n\n");


//  	    out.write("IMPORT FROM " + systemPath + "/BOND.DEL OF DEL COMMITCOUNT 1000000 INSERT  INTO MDSYSTEM.BOND;\n");
// 	    out.write("set integrity for MDSYSTEM.BOND ALL immediate unchecked;\n\n");

//  	    out.write("IMPORT FROM " + systemPath 
// 		      + "/UDF_INVOCATION_TABLE.DEL OF DEL COMMITCOUNT 1000000 INSERT  INTO MDSYSTEM.UDF_INVOCATION_TABLE;\n");
// 	    out.write("set integrity for MDSYSTEM.UDF_INVOCATION_TABLE ALL immediate unchecked;\n\n");

// 	    out.write("IMPORT FROM " + systemPath 
//  		      + "/GLOBAL_PARAMETER_TYPE.DEL OF DEL COMMITCOUNT 1000000 INSERT  INTO MDSYSTEM.GLOBAL_PARAMETER_TYPE;\n");
// 	    out.write("set integrity for MDSYSTEM.GLOBAL_PARAMETER_TYPE ALL immediate unchecked;\n\n");
	    
//  	    out.write("IMPORT FROM " + systemPath 
//  		      + "/GLOBAL_PARAMETER.DEL OF DEL COMMITCOUNT 1000000 INSERT  INTO " 
//  		      + "MDSYSTEM.GLOBAL_PARAMETER;\n");
//  	    out.write("set integrity for MDSYSTEM.GLOBAL_PARAMETER ALL immediate unchecked;\n\n");
}
