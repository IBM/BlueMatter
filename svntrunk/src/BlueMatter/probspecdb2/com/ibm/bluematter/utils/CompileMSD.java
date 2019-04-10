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

import java.util.*;
import java.io.*;
import java.sql.*;
import java.text.*;

public class CompileMSD {

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
    
    private static void deleteExecutableId( int executableId, String databaseName ) {
	String delStr = "delete from experiment.executable where executable_id = " 
	    + java.lang.Integer.toString(executableId);

	try {
	    Connection con = DriverManager.getConnection( "jdbc:db2:" + databaseName );
	    Statement stmt = con.createStatement();

	    stmt.execute(delStr);

	    stmt.close();
	    con.close();
	} catch ( Exception e ) {
	    e.printStackTrace();
	    System.exit(1);
	}
    }

    private static void setBuildEnd(int executableId, String databaseName) {
	String updateStr = "update experiment.executable set ( build_end )=( CURRENT TIMESTAMP ) where executable_id = " 
	    + java.lang.Integer.toString(executableId);

	try {
	    Connection con = DriverManager.getConnection( "jdbc:db2:" + databaseName );
	    Statement stmt = con.createStatement();

	    stmt.execute(updateStr);

	    stmt.close();
	    con.close();
	} catch ( Exception e ) {
	    e.printStackTrace();
	    System.exit(1);
	}
    }
    
    private static int reserveExecutableId(String databaseName, String builder, String sourceIdStr, String pltIdStr, String implIdStr, String installPath, String installTag) {
	
	String exStr = "insert into experiment.executable ( builder, source_id," 
	    + " platform_id, impl_id, build_begin, install_image_tag, install_image_path) " 
	    + " VALUES ('" + builder + "'," + sourceIdStr + "," + pltIdStr + ","
	    + implIdStr + ", CURRENT TIMESTAMP, '" + installTag + "', '" + installPath + "' )";

	int executableId = -1;

	try {
	    Connection con = DriverManager.getConnection( "jdbc:db2:" + databaseName );
	    Statement stmt = con.createStatement();

	    stmt.execute(exStr);

	    ResultSet rs = 
		stmt.executeQuery("select IDENTITY_VAL_LOCAL() from experiment.executable");
	    if(!rs.next())
		throw new Exception("ERROR:: Could not get the executable id");

	    executableId = rs.getInt(1);

	    stmt.close();
	    con.close();
	} catch ( Exception e ) {
	    e.printStackTrace();
	}
	return executableId;
    }

    public static void main(String argv[]) {

        int argc = argv.length;
        if(argc < 1 || argv[0].equals("-usage") ) {
            usage();
            return;
        }

        String msdFile = argv[0];

        String databaseName = new String("mdsetup");
	String installBase = new String("Not Defined");
	String installTag = new String("Not Defined");

        int i=1;
        while (i < argc) {
            if( argv[i].equals("-database")) {
                if((i != argc-1) || argv[i+1].startsWith("-")) {
                    databaseName = argv[i+1];
		    ++i;
                }
                else {
                    usage();
                    return;
                }
            }
	    else if(argv[i].equals("-installbase")) {
		if (i != argc-1) {
		    installBase = argv[i+1];
		    ++i;
		}
	    }
            i++;
        }

        try {



            BufferedReader fIn=null;
            try {
                fIn = new BufferedReader(new FileReader(msdFile));
            } catch (FileNotFoundException e) {
                System.err.println(e.getMessage());
                System.exit(1);
            }

            String line = null;
            try {
                line         = fIn.readLine();
            } catch (IOException e) {
                e.printStackTrace();
                System.out.println("Error reading the msd file");
                System.exit(1);
            }

            String scriptName = "compile_script.temp";

            Runtime runTime = Runtime.getRuntime();
             try {
                 Process p3 = runTime.exec("rm -rf "+ scriptName);
                 p3.waitFor();
             } catch (IOException e) {}

            FileWriter out = new FileWriter( scriptName );

            boolean yankScript = false;
            String sourceIdStr = "-1";
            String implIdStr   = "-1";
            String pltIdStr    = "-1";
            while(line!=null) {
                line.trim();

                if(line.startsWith("//%source_id")) {
                    sourceIdStr = line.substring(line.lastIndexOf(":")+1, line.length());
                }
                if(line.startsWith("//%impl_id")) {
                    implIdStr = line.substring(line.lastIndexOf(":")+1, line.length());
                }
                if(line.startsWith("//%plt_id")) {
                    pltIdStr = line.substring(line.lastIndexOf(":")+1, line.length());
                }

                if(line.equals("END_COMPILE_SCRIPT **/")) {
                    out.flush();
                    out.close();
                    break;
                }

                if(yankScript) {
                    out.write(line+"\n");
                }

                if(line.equals("/** BEGIN_COMPILE_SCRIPT")) {
                    yankScript = true;
                }



                line = fIn.readLine();
            }

            Process p2 = runTime.exec("whoami");
            p2.waitFor();
            BufferedReader in1 = new BufferedReader(new InputStreamReader(p2.getInputStream()));

            String builder = (in1.readLine()).trim();
            in1.close();

	    // Reserve executable_id in experiment.executable table
	    int executableId = reserveExecutableId( databaseName, builder, sourceIdStr,  pltIdStr,  implIdStr, installBase, installTag);

            // Call the script

            String msdFilePrefix = msdFile.substring(0,msdFile.lastIndexOf("."));
            String commandToRun = "ksh executable_id=" 
		+ java.lang.Integer.toString(executableId) 
		+ " ksh " + scriptName + " " + msdFilePrefix + " 2>&1" ;
            System.out.println(commandToRun) ;
            Process p4 = runTime.exec(commandToRun);
            BufferedReader in  = new BufferedReader(new InputStreamReader(p4.getInputStream()));
            BufferedReader err = new BufferedReader(new InputStreamReader(p4.getErrorStream()));

            String readLine = in.readLine();
            while( readLine != null ) {
                System.out.println( readLine );
                readLine = in.readLine();
            } 
            
            readLine = err.readLine();
            while( readLine != null ) {
                System.err.println( readLine );
                readLine = err.readLine();
            }

            p4.waitFor();
            
            int exitStatus = p4.exitValue();
            if( exitStatus != 0 ) {
                System.err.println("ERROR:: Exit status "+exitStatus);
		deleteExecutableId(executableId, databaseName);
                System.exit( exitStatus );
            }

	    setBuildEnd(executableId, databaseName);
            
            Process p5 = runTime.exec("rm -rf "+scriptName);
            p5.waitFor();

        }
        catch (Exception e) {
                e.printStackTrace();
                System.exit(1);
        }
        return;
    }

    private static void usage() {
        System.out.println("Input: <msd .cpp file > [flags]");
        System.out.println("flags: -database <database_name> --> \"mdsetup\" by default");
        return;
    }
}

