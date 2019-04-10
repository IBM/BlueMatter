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
// Date: Sep 12, 2001
// Class: 
// Description: Provides probspecgen the proper access to the db2 UDF_Registry
// Modification:
// Adopted from UDF_RegistryIF.hpp
// ************************************************************************
package com.ibm.bluematter.db2probspec;

import java.sql.*;

public class UDF_RegistryIF {
    
    private Connection con = null;
    
    public static final int CODENOTFOUND = -1;

    public UDF_RegistryIF(Connection c) {
	try {
	    con = c;
	} catch (Exception e) {
	    e.printStackTrace();
	}
    }
    
    // These are all the functionality of UDF_Registry used by probspecgen
//     UDF_RegistryIF::GetUDFCodeByName("LennardJonesForce", ljCode);
//     UDF_RegistryIF::GetStructureStringByUDFCode(ljCode, ljStruct);
//     UDF_RegistryIF::GetStructureNameByUDFCode(ljCode, ljStruct);
//     UDF_RegistryIF::GetNSitesByUDFCode( udfCode, siteCount1 );
//     UDF_RegistryIF::CODENOTFOUND
//     UDF_RegistryIF::GetArgCountByUDFCode(udfCode, argCount);
//     UDF_RegistryIF::GetArgByOrd(udfCode, j, ordName, ordType);
    
    public boolean IsCategoryForCode(int udfCode, String category) {
	try {
	    Statement stmt = con.createStatement();
	    ResultSet rs = stmt.executeQuery("SELECT udf_id FROM Registry.UDF_Category "
					     + "WHERE udf_id=" + udfCode 
					     + " AND category_name LIKE '" + category +"'");
	    
	    if(rs.next()) {
		stmt.close();
		return true;
	    }
	    else {
		stmt.close();
		return false;
	    }	    

	} catch (SQLException e) {
	    e.printStackTrace();	
	}

	return false;
    }
    
    public int GetNumberOfUDFCodes() {
	try {
	    Statement stmt = con.createStatement();
	    ResultSet rs = stmt.executeQuery("SELECT COUNT(*) FROM Registry.UDF_Record");
	    
	    rs.next();
	    int result = rs.getInt(1);
	    stmt.close();
	    return result;
	    
	} catch (SQLException e) {
	    e.printStackTrace();	
	}
	
	return -1;
    }

    public int GetUDFCodeByName(String name) throws Exception {
	try {
	    Statement stmt = con.createStatement();
	    ResultSet rs = stmt.executeQuery("SELECT udf_id FROM Registry.UDF_Record "
					     +"WHERE udf_name LIKE '" + name  + "'");
	    int code;
	    while(rs.next()) {
		code = rs.getInt("udf_id");
		stmt.close();		
		return code;
	    }
	
	    stmt.close();
	    throw new Exception("No udf_id was found for udf_name: " + name);
	    	    
	} catch (SQLException e) {
	    e.printStackTrace();	}

	return -1;
    }

    public String GetUDFNameByCode(int code) throws Exception {
	try {
	    Statement stmt = con.createStatement();
	    ResultSet rs = stmt.executeQuery("SELECT udf_name FROM Registry.UDF_Record "
					     +"WHERE udf_id="+code);
	    String name;
	    while(rs.next()) {
		name = rs.getString(1);
		stmt.close();		
		return name;
	    }
	
	    stmt.close();
	    throw new Exception("No udf_name was found for udf_id: " + code);
	    	    
	} catch (SQLException e) {
	    e.printStackTrace();	}

	return null;
    }

    public String GetStructureStringByUDFCode(int code) throws Exception {
	try {
	    Statement stmt  = con.createStatement();
	    Statement stmt1 = con.createStatement(); 

	    ResultSet rs = stmt.executeQuery("SELECT udf_name FROM Registry.UDF_Record "
					     +"WHERE udf_id =" + code  +";");
	    
	    ResultSet rs1 = stmt1.executeQuery("SELECT var_name, type_desc, type_size FROM Registry.UDF_Variable " 
					      + " WHERE udf_id = " + code + " ORDER BY type_size DESC, var_name ASC;");
	    
	    String structureName = null;
	   
	    while(rs.next()) {
		structureName  = rs.getString("udf_name") + "_Params";
	    }
	    
	    if(structureName == null) {
		stmt.close();
		throw new Exception("No structure was found for udf_id: " + code);
	    }

	    String result = "struct " + structureName + "\n";
	    result += "{\n";
	    while(rs1.next()) {
		result += "\t" + rs1.getString("type_desc") + " " + rs1.getString("var_name") + ";\n";
	    }
	    
	    result += "};\n";
	    
	    stmt.close();
	    return result;
	} catch (SQLException e) {
	    e.printStackTrace();
	}
	return null;
    }

    public String GetStructureNameByUDFCode(int code) throws Exception {
	try {
	    Statement stmt = con.createStatement();

	    ResultSet rs = stmt.executeQuery("SELECT udf_name FROM Registry.UDF_Record "
					     +"WHERE udf_id=" + code  +";");

	    String result = null;
	    while(rs.next()) {
		result = rs.getString("udf_name") + "_Params";
		stmt.close();		
		return result;
	    }
	    
	    stmt.close();
	    throw new Exception("No site_count  was found for udf_id: " + code);
	
	} catch (SQLException e) {
	    e.printStackTrace();
	}

	return null;
    }

    public int GetNSitesByUDFCode(int code) throws Exception {
	try {
	    Statement stmt = con.createStatement();
	    ResultSet rs = stmt.executeQuery("SELECT site_count FROM Registry.UDF_Record "
					     +"WHERE udf_id=" + code  +";");
	    int siteCount;
	    while(rs.next()) {
		siteCount = rs.getInt("site_count");
		stmt.close();		
		return siteCount;
	    }
	    
	    stmt.close();
	    throw new Exception("No site_count  was found for udf_id: " + code);

	} catch (SQLException e) {
	    e.printStackTrace();
	}
	return -1;
    }

    public int GetArgCountByUDFCode(int code) throws Exception {
	try {
	    Statement stmt = con.createStatement();

	    ResultSet rs = stmt.executeQuery("SELECT COUNT( var_name ) FROM Registry.UDF_Variable " 
					     + " WHERE udf_id = " + code + ";");
	    int argCount;
	    while(rs.next()) {
		argCount = rs.getInt(1);
		stmt.close();		
		return argCount;
	    }

	    stmt.close();	    
	    throw new Exception("No arg_count was found for udf_id: " + code);

	} catch (SQLException e) {
	    e.printStackTrace();
	}
	return -1;
    }

    public boolean ReportsEnergy( String name ) throws Exception {
        Statement stmt = con.createStatement();
            
        ResultSet rs = stmt.executeQuery("SELECT count( distinct ruc.udf_id) "
                                         + " FROM registry.udf_record as rur, registry.udf_category as ruc " 
                                         + " WHERE rur.udf_name='"+name+"' and ruc.udf_id=rur.udf_id "
                                         + " and ruc.category_name='ReportsEnergy' ");
        rs.next();
        
        return ( rs.getInt(1) != 0 );
    }
    

    public String GetArgByOrd(int code, int argIndex) throws Exception {
	try {
	    Statement stmt = con.createStatement();
	
	    ResultSet rs = stmt.executeQuery("SELECT var_name, type_desc FROM Registry.UDF_Variable " 
					     + " WHERE udf_id = " + code + " ORDER BY type_size DESC, var_name ASC;");

	    int count = 0;
	    String ordName;
	    while(rs.next()) {
		if (count == argIndex) {
		    ordName = rs.getString("var_name");
		    stmt.close();		
		    return ordName;
		}
		count++;
	    }
	    
	    stmt.close();	    
	    throw new Exception("No ordinal was found for udf_id: " + code + " and index: " + argIndex);
	    
	} catch (SQLException e) {
	    e.printStackTrace();
	}
	
	return null;
    }

    public void close() {
	try {
	    con.close();
	} catch (Exception e) {
	    e.printStackTrace();
	}	
    }
}
