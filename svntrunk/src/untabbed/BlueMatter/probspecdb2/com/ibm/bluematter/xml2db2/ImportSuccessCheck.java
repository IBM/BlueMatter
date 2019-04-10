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

public class ImportSuccessCheck {

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
       
       int i=0;
       String databaseName = new String("mdsetup");
       String dirName = null;
       
       int systemId = -1;
       
       while( i < argc ) {
    if (argv[i].equals("-database")) {
        if((i==argc-1) || argv[i+1].startsWith("-")) {
      usage();
      return;
        }
        databaseName = argv[i+1];
          }
    else if (argv[i].equals("-sysid")) {
        if((i==argc-1) || argv[i+1].startsWith("-")) {
      usage();
      return;
        }

              systemId = Integer.parseInt(argv[i+1]);
    }  else if ( argv[i].equals("-dir")) {
        if((i==argc-1) || argv[i+1].startsWith("-")) {
      usage();
      return;
        }

        dirName = argv[i+1];
    } 
    i++;
       } 
       
       if((dirName == null) || (systemId == -1)) {
           usage();
           return; 
       }

       //  URL is jdbc:db2:dbname 
       Connection con = null;	     
       String url = "jdbc:db2:" + databaseName;
       
       try {
           
           //  connect with default id/password 
           con = DriverManager.getConnection(url);
           
           Statement stmt = con.createStatement();
           
           
           DB2TableNames tableNames = new DB2TableNames();
       
           for(int j=0; j<tableNames.size(); j++) {
               String tableName = tableNames.getNthName(j);
               String path = dirName + "/" + tableName + ".DEL";
               int lineCount = getLineCountInFile(path);
               ResultSet rs = stmt.executeQuery("SELECT COUNT(*) "
                                                +"FROM MDSYSTEM."+tableName
                                                +" WHERE sys_id=" + systemId);
               if(!rs.next()) {
                   System.out.println("ERROR: Problem occured while getting COUNT(*) for table: " + tableName);
                   return;
               }
               
               int numberOfRows = rs.getInt(1);
               if(numberOfRows != lineCount) {
                   System.out.println("ERROR: Mismatch in the number of rows inserted into db2 for table: " + tableName);
                   System.out.println("ERROR: The number of rows NOT inserted: " + (lineCount-numberOfRows ));
                   return;
               }
           }

           System.out.println("SUCCESS: All "+tableNames.size()+" del files were imported correctly.");
       } catch (Exception e) {
           e.printStackTrace();
           System.out.println(e.getMessage());
           System.exit(1);
       }
   }
    
    // Returns the number of non-blank rows in a file.
    private static int getLineCountInFile(String fullPath) {
        try {
          BufferedReader  fIn = new BufferedReader(new FileReader(fullPath));
          String line = fIn.readLine();
          int lineCount = 0;
          while(line != null) {
              line.trim();
              if(!line.equals(""))
                  lineCount++;
              line = fIn.readLine();
          }

          return lineCount;
        }
        catch(Exception e) {
            System.out.println(e.getMessage());
            System.exit(1);
            return -1;
        }
    }
    
    private static void usage() {
        System.out.println("Input: -dir <del files directory> -sysid <sys_id> [optional flags]");
  System.out.println("flags: -database <database_name>         \"mdsetup\" by default");
    }
}
