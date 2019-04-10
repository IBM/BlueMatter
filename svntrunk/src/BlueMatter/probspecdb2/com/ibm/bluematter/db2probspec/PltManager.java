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

import java.util.*;
import java.sql.*;
import java.io.*;
import com.ibm.bluematter.db2probspec.*;

public class PltManager 
{
    private static int pltId = -1;
    private static Connection conn = null;
    private static Statement  stmt = null;

    public static void init( int plt_id, Connection con) 
    {
        pltId = plt_id;
        conn = con;
        
        try {
            stmt = con.createStatement();
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(1);
        }
    }

    public static boolean IsSpiBuild( ) throws Exception
    {
        return ( getParamFromDB2Platform("spi") != null );
    }
    
    private static String getParamFromDB2Platform( String paramName ) throws Exception 
    {        
        ResultSet rs = null;
        
        rs = stmt.executeQuery("select value from experiment.platform_params"
                               + " where platform_id="+pltId
                               + " and name LIKE '"+paramName+"'");
        if(!rs.next()) 
            return null;
          
        return rs.getString("value");
    }
}
