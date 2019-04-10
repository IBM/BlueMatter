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

public class SysIdFromDB2 {

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

      String systemName = (argv[0].toUpperCase()).trim();
      String databaseName = new String("mdsetup");
      
            //  URL is jdbc:db2:dbname 
      Connection con = null;
      String url = "jdbc:db2:" + databaseName ;

      try {
         
         //  connect with default id/password 
         con = DriverManager.getConnection(url);
   Statement stmt = con.createStatement();
         
         ResultSet rs = stmt.executeQuery("SELECT sys_id, creator, created FROM MDSYSTEM.SYSTEM "
                                       + " WHERE sys_desc='" + systemName + "'"
                                       + " ORDER BY created DESC");
         
         if(!rs.next()) {
             System.out.println("No record found for sys_desc: " + systemName);
             return;
         }
             

         do{
             int sysId = rs.getInt("sys_id");
             String creator = rs.getString("creator");
             Timestamp created = rs.getTimestamp("created");
             System.out.println("System Id: " + sysId + " Creator: " + creator + " Created: " + created);
         } while(rs.next());
   
      }
      catch(Exception e) {
          System.out.println(e.getMessage());
      }
   }

    private static void usage() {
        System.out.println("Usage: SysIdFromDB2 <sys_desc>");
    }
}
