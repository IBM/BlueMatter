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

class DumpSysId {

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
      
      int i=0;
      boolean showAll = false;
      String systemName = null;
      String databaseName = new String("bluegene");
      while (i < argc) {
	  if(argv[i].equals("-all")){
	      showAll = true;
	  } else if( argv[i].equals("-system")){
	      if((i != argc-1) ||  argv[i+1].startsWith("-")) {
		  systemName = argv[i+1];
	      }
	      else {
		  usage();
		  return;
	      }
	  } else if( argv[i].equals("-database")){
	      if((i != argc-1) ||  argv[i+1].startsWith("-")) {
		  databaseName = argv[i+1];
	      }
	      else {
		  usage();
		  return;
	      }
	  }
	  i++;
      }
      
            //  URL is jdbc:db2:dbname 
      Connection con = null;
      String url = "jdbc:db2:" + databaseName;

      try {
	  
	  //  connect with default id/password
	  con = DriverManager.getConnection(url);
	  Statement stmt = con.createStatement();
	  if(showAll) {
	      ResultSet rs = stmt.executeQuery("SELECT SYS_DESC FROM MDSYSTEM.SYSTEM");
	      while(rs.next()) {
		  System.out.println(rs.getString(1));
	      }
	  } else if (systemName != null) {
	      ResultSet rs = stmt.executeQuery("SELECT SYS_DESC FROM MDSYSTEM.SYSTEM WHERE SYS_DESC LIKE '"
					       + systemName.toUpperCase()  + "%'");
	      while(rs.next()) {
		  System.out.println(rs.getString(1));
	      }
	      
	  } else {
	      usage();
	      return;
	  }
	  
	  stmt.close();
	  con.close();
      } catch (Exception e){
	  e.printStackTrace();
      }
   }
    
    private static void usage() {
	System.out.println("Input: [flags]");
	System.out.println("flags: -all                      --> displays all the sysIdent available in the probspec database.");
	System.out.println("       -system <system_name>     --> displays the sysIdent which corresponds to the system.");
	System.out.println("       -database <database_name> --> \"bluegene\" by default");
	return;
    }
}

