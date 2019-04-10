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
 package com.ibm.bluematter.udf_registry;

import java.sql.*;

class UDFRegistryDB2Setup {

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
      Connection con = null;

      //  URL is jdbc:db2:dbname 
      int i=0;
      String databaseName = new String("bluegene");
      int argc = argv.length;
      while(i < argc ) {
	  if( argv[i].equals("-database")) {
	      if((i != argc-1) || argv[i+1].startsWith("-")) {
		  databaseName = argv[i+1];
	      }
	      else {
		  usage();
		  return;
	      }
	  }
	  i++;
      }

      String url = "jdbc:db2:" + databaseName;

      try {
         
            //  connect with default id/password 
         con = DriverManager.getConnection(url);
         
         Statement stmt = con.createStatement();
         
	 stmt.execute("CREATE SCHEMA Registry ");	 
	 stmt.execute(" CREATE TABLE Registry.UDF_Record (udf_id     INT NOT NULL,"
		      + "udf_name   VARCHAR(50) NOT NULL,"
		      + "entry_date DATE,"
		      + "entry_time TIME,"
		      + "arg_check_sum INT,"
		      + "body_check_sum INT,"
		      + "site_count     INT,"
		      + "param_count    INT,"
		      + "PRIMARY KEY (udf_id))");

	 
	 stmt.execute("	CREATE TABLE Registry.UDF_Category (udf_id     INT NOT NULL,"
		      + "category_name    VARCHAR(40),"
		      + "PRIMARY KEY (udf_id),"
		      + "FOREIGN KEY (udf_id) REFERENCES Registry.UDF_Record ON DELETE CASCADE);");
	 
	 stmt.execute("	CREATE TABLE Registry.UDF_Variable (udf_id     INT NOT NULL, "
		      + "var_name   VARCHAR(40) NOT NULL, "
		      + "type_desc  VARCHAR(30), "
		      + "type_size  INT, "
		      + "PRIMARY KEY (udf_id , var_name), "
		      + "FOREIGN KEY (udf_id) REFERENCES Registry.UDF_Record ON DELETE CASCADE);");
	 

         stmt.close();
         con.close();
      } catch( Exception e ) {
	  e.printStackTrace();
      }
   }
    
    private static void usage() {
	System.out.println("Input: [flags]");
	System.out.println("flags: -database <database_name> --> \"bluegene\" by default");
	return;
    }
}
