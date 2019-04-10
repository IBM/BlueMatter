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
 // ************************************************************************
// File: ProbspecgenDB2.java
// Author: Alex Rayshubskiy
// Date: Sep 9, 2001
// Class:
// Description: This class uses db2  to generate
// the file which contains the specification of the MD problem
// Modification:
// 6/22/01     -  Birth of probspecgen
// 8/8/01      -  Using a different interface to the XML
// 8/13/01     -  Changing the workorders to be |udf code|siteTupleListPtr|ParamTuplePtr|
// 9/08/01     -  Porting probspecgen to Java
// ************************************************************************
package com.ibm.bluematter.db2probspec;

import java.sql.*;
import java.util.*;
import java.io.*;

import com.ibm.bluematter.db2probspec.parser.*;

public class ProbspecgenDB2 {

    public static final int  ALL_SITES_IN_ONE_FRAGMENT = 0,
        ALL_MOLECULES_IN_ONE_FRAGMENT = 1,
        ALL_SITES_IN_THEIR_OWN_FRAGMENT = 2,
        ALL_MOLECULES_IN_THEIR_OWN_FRAGMENT = 3,
        RESIDUE_FRAGMENTATION = 4,
        RIGID_GROUP_FRAGMENTATION = 5;

    public static boolean verbose         = false;
    public static boolean doEwald         = false;
    public static boolean ljOff           = false;
    public static int     fragment_mode   = ALL_MOLECULES_IN_THEIR_OWN_FRAGMENT;
    public static boolean dumpRTP         = false;
    public static boolean debug           = false;
    public static boolean no14            = false;
    public static boolean regen           = false;

    public static boolean doTIP3P            = false;
    public static boolean doTIP3PFloppy      = false;
    public static boolean doSPC              = false;
    public static boolean doSPCFloppy        = false;
    public static boolean doSPCE             = false;
    public static boolean doSPCEFloppy       = false;

    public static boolean charmm             = false;
    public static boolean oplsaa             = false;
    public static boolean amber              = false;
    public static Hashtable        globalDefines          = new Hashtable();

    public static boolean checkSystemIntegrity              = false;

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

       Runtime rt = Runtime.getRuntime();

       if(argc < 2) {
           usage();
           return;
       }

       String databaseName = new String("mdsetup");

       String regenMSDFileName = "";

       int i = 0;
       while( i < argc ) {

           if (argv[i].equals("-v"))
               verbose = true;
           else  if( argv[i].equals("-database")) {
               if((i != argc-1) || argv[i+1].startsWith("-")) {
                   databaseName = argv[i+1];
               }
               else {
                   usage();
                   return;
               }
           }
           else if ( argv[i].equals("-dumpRTP")) {
               dumpRTP = true;
           }
           else if ( argv[i].equals("-debug")) {
               debug = true;
           }
           else if ( argv[i].equals("-checkIntegrity")) {
               checkSystemIntegrity = true;
           }
           else if ( argv[i].equals("-regen") ) {
               if((i != argc-1) || argv[i+1].startsWith("-")) {
                   regenMSDFileName = argv[i+1];
                   regen = true;
               }
               else {
                   usage();
                   return;
               }
           }
           i++;
       }

       int sourceId = -1;
       int systemId = -1;
       int ctpId = -1;
       int implId = -1;
       int pltId = -1;
       boolean useSourceId = false;

       if( regen ) {
           try {
               sourceId = getAttrFromFile( regenMSDFileName, "source_id" );
               implId = getAttrFromFile( regenMSDFileName, "impl_id" );
               pltId = getAttrFromFile( regenMSDFileName, "plt_id" );
               useSourceId = true;
           } catch (Exception e) {
               System.out.println(e.getMessage());
               System.exit(1);
           }
       }
       else if( argv[ 0 ].equals("-source_id") ) {
           useSourceId = true;
           try {
               sourceId = Integer.parseInt(argv[ 1 ]);
           }
           catch (Exception e) {
               System.out.println("Invalid sourceId: " + argv[ 1 ]);
               System.exit(1);
           }
       }

       if(!regen) {
           try {
               implId = Integer.parseInt(argv[ 2 ]);
           }
           catch (Exception e) {
               System.out.println("Invalid implId: " + argv[ 2 ]);
               System.exit(1);
           }

           try {
               pltId = Integer.parseInt(argv[ 3 ]);
           }
           catch (Exception e) {
               System.out.println("Invalid pltId: " + argv[ 3 ]);
               System.exit(1);
           }
       }

       Connection con = null;

       String url = "jdbc:db2:" + databaseName;

       Statement stmt = null;
       try {
           con = DriverManager.getConnection(url);
           stmt = con.createStatement();

           if(!useSourceId) {

               String systemIdString  = argv[ 0 ].toUpperCase();
               try {
                   systemId = Integer.parseInt(systemIdString);
               }
               catch (Exception e) {
                       System.out.println("Invalid systemId: " + systemIdString);
                       System.exit(1);
               }

               String ctpIdString = argv[ 1 ];
               try {
                   ctpId = Integer.parseInt(ctpIdString);
               }
               catch (Exception e) {
                   System.out.println("Invalid compiler time parameter Id: " + ctpIdString);
                   System.exit(1);
               }

               ResultSet rs = stmt.executeQuery("SELECT source_id FROM EXPERIMENT.system_source "
                                                + " WHERE ctp_id="+ctpId + " AND sys_id="+systemId);

               if(!rs.next()) {

                   // The { sys_id, ctp_id } pair doesn't exist in db2 so insert

                   System.out.println("SystemId: " + systemId);
                   System.out.println("CtpId: " + ctpId);

                   stmt.executeUpdate("INSERT INTO EXPERIMENT.system_source ( sys_id, ctp_id ) "+
                                      " VALUES ("+systemId+","+ctpId+")");

                   ResultSet rs1 = stmt.executeQuery("SELECT source_id FROM EXPERIMENT.system_source "
                                                     +" WHERE ctp_id="+ ctpId + " AND sys_id="+systemId);
                   if(!rs1.next())
                       throw new Exception("ERROR: Problem occured while retrieving source id for ctp id: " + ctpId
                                           + " and system id: " + systemId );

                   sourceId = rs1.getInt("source_id");
               }
               else {
                   sourceId = rs.getInt("source_id");
               }
           }
           else {
               if(sourceId==-1)
                   throw new Exception("ERROR: SourceId in use, but not set.");

               ResultSet rs = stmt.executeQuery("SELECT sys_id, ctp_id FROM EXPERIMENT.system_source "
                                                + " WHERE source_id="+sourceId);

               if(!rs.next())
                   throw new Exception("ERROR: No information available for source id: " + sourceId);

               systemId = rs.getInt("sys_id");
               ctpId = rs.getInt("ctp_id");
           }

           String cppFile = null;

           if( regen )
               cppFile = regenMSDFileName;
           else
               cppFile = argv[ 4 ]+".msd";

           ResultSet rs = stmt.executeQuery("SELECT * FROM MDSYSTEM.SYSTEM WHERE SYS_ID=" + systemId);

           if(!rs.next())
               throw new Exception("The systemId: " + systemId +" does not exist in the probspec database");

           String sys_desc = rs.getString("sys_desc");
           String sys_creator = rs.getString("creator");
           Timestamp sys_created = rs.getTimestamp("created");

           rs = stmt.executeQuery("SELECT * FROM EXPERIMENT.ctp_table WHERE CTP_ID=" + ctpId);

           if(!rs.next())
               throw new Exception("The ctpId: " + ctpId +" does not exist in the probspec database");

           String ctp_filename = rs.getString("filename");
           String ctp_creator = rs.getString("creator");
           Timestamp ctp_created = rs.getTimestamp("created");

           rs = stmt.executeQuery("SELECT * FROM EXPERIMENT.impl_table WHERE impl_id=" + implId);

           if(!rs.next())
               throw new Exception("The implId: " + implId +" does not exist in the probspec database");

           String impl_filename = rs.getString("filename");
           String impl_creator = rs.getString("creator");
           Timestamp impl_created = rs.getTimestamp("created");


           rs = stmt.executeQuery("SELECT * FROM EXPERIMENT.platform_table WHERE platform_id=" + pltId);

           if(!rs.next())
               throw new Exception("The pltId: " + pltId +" does not exist in the probspec database");

           String plt_filename = rs.getString("filename");
           String plt_creator = rs.getString("creator");
           Timestamp plt_created = rs.getTimestamp("created");

           System.out.println("Generating a cpp for: ");
           System.out.println("System: " + sys_desc + " created by: " + sys_creator + " on: " + sys_created);
           System.out.println("RTP: " + ctp_filename + " created by: " + ctp_creator + " on: " + ctp_created);
           System.out.println("IMPL: " + impl_filename + " created by: " + impl_creator + " on: " + impl_created);
           System.out.println("PLATFORM: " + plt_filename + " created by: " + plt_creator + " on: " + plt_created);
           System.out.println("Source Id: " + sourceId);

           RunTimeParameters.init( ctp_filename, ctpId, con );
           ImplManager.init(implId, con);
           PltManager.init(pltId, con);

           UDF_RegistryIF udfRegistry = new UDF_RegistryIF(con);

           RunTimeParameters.getCTPFromDB2();
           RunTimeParameters.processRunTimeParameters( udfRegistry );

           if( dumpRTP ) {
               RunTimeParameters.dumpParams(systemId);
           }

           if( implId == -1 )
               throw new Exception("ERROR:: implId is not set");

           if( pltId == -1 )
               throw new Exception("ERROR:: pltId is not set");

           FileWriter out = new FileWriter( cppFile );

           java.util.Date now = new java.util.Date();
           out.write("//File generated on: " + now +"\n");
           out.write("//%database_name:" + databaseName + "\n");
           out.write("//%source_id:" + sourceId + "\n");
           out.write("//%system_id:" + systemId + "\n");
           out.write("//%ctp_id:" + ctpId + "\n");
           out.write("//%impl_id:" + implId + "\n");
           out.write("//%plt_id:" + pltId + "\n");
           out.write("//%probspec_version:v1.0.0\n");
           out.write("//%probspec_tag:RTP file is comming in from db2\n");
           String magicNumber = "0xFACEB0B5";
           out.write("//%magic_number:"+magicNumber+"\n");

           CompileScriptGenerator csg = new CompileScriptGenerator( con );
           csg.dumpScript( implId, pltId, out );

           MSDGenerator pg  = new MSDGenerator( con, udfRegistry );
           pg.dumpCpp( systemId, sourceId, magicNumber, out );

           out.flush();
           out.close();
           con.close();
       }
       catch(Exception e) {
           e.printStackTrace();
       }
   }

    private static void usage() {
        System.out.println("Input: <system ID> <ctp ID> <Impl ID> <Plt ID> <cpp file name wo/.cpp> [optional flags]");
        System.out.println("Or");
        System.out.println("Input: -source_id <source ID> <Impl ID> <Plt ID> <cpp file name wo/.cpp> [optional flags]");
        System.out.println("flags: ");
        System.out.println("       -v   --> Verbose mode");
        System.out.println("       -database <database_name> --> \"bluegene\" by default");
        System.out.println("       -dumpRTP --> dumps the parsed contents of the RTP file (for debugging)");
        System.out.println("       -debug   --> turns on debugging info");
        System.out.println("       -checkIntegrity   --> checks that the site list and the corresponding param list are of the same size");
        System.out.println("       -regen <existing_cpp_filename> --> Regenerates a fresh cpp file and a fresh script file.");
        return;
    }

    private static int getAttrFromFile(String filename, String attr) throws Exception {
        BufferedReader fIn = null;

        try {
            fIn = new BufferedReader(new FileReader(filename));
        } catch (FileNotFoundException e) {
            System.err.println(e.getMessage());
            System.exit(1);
        }

        String line = null;
        try {
            line         = fIn.readLine();
        } catch (IOException e) {
            e.printStackTrace();
            System.out.println("SimpleParser::parser():: Error reading the rtp file");
            System.exit(1);
        }

        while(line != null) {

            line.trim();
            if(line.startsWith("//%"+attr)) {
                String attrString = line.substring(line.indexOf(":")+1, line.length());
                int attr_id = Integer.parseInt( attrString );
                return attr_id;
            }

            line = fIn.readLine();
        }

        throw new Exception("ERROR:: File: " + filename + " is not regeneratable... ");
    }
}
