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
 // ****************************************************************
/**
   File: MolSysView.java
   Author: RSG
   Date: May 30, 2002
   Class: MolSysView
   Description: Processes queries to return subsets of molecular systems
                stored in a relational database.
 */
// ****************************************************************

package com.ibm.bluematter.servlet;

import java.sql.*;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class MolSysView
{
    public final static void execute(HttpServletRequest req, PrintWriter out, Connection conn)
    throws java.lang.Exception
    {
	String sysID = req.getParameter("sys_id");
	if (sysID == null)
	    {
		out.println("ERROR: No sys_id provided in query");
		return;
	    }

	out.println("<html>\n<head>\n");
	out.println("<title>Molecular System " + sysID +
		    "</title>\n</head>\n<body><br><br>\n");
	out.println("<h1> Molecular System " + sysID + "</h1><br><br>\n");

	String cmd = new String("select sys_desc, creator,created, sys_class from mdsystem.system where sys_id = " + sysID);
	out.println("<br><br>");
	try
	    {
		out.println("<table align=center cellpadding=8>");
		out.println("<caption>Vital Statistics: ");
		out.println(cmd);
		out.println("</caption>");
		out.println("<tr align=center><th>Description</th><th>Creator</th><th>Created</th><th>System Class</th></tr>");
		Statement stmt = conn.createStatement();
		ResultSet rs = null;
		rs = stmt.executeQuery(cmd);
		while (rs.next())
		    {
			out.println("<tr align=left>");
			String sysDesc = rs.getString(1);
			String creator = rs.getString(2);
			Timestamp created = rs.getTimestamp(3);
			String sysClass = rs.getString(4);
			out.println("<td>" + sysDesc + "</td>");
			out.println("<td>" + creator + "</td>");
			out.println("<td>" + created + "</td>");
			out.println("<td>" + sysClass + "</td>");
			out.println("</tr>");
		    }
		out.println("</table><br><br>");

		// Global Parameter Table
		out.println("<table align=center border cellpadding=8>");
		out.println("<caption>Global Parameters</caption>");
		out.println("<tr align=center><th>Parameter</th><th>Type</th><th>Value</th><th>Symbolic Constant</th></tr>");
		rs = stmt.executeQuery("select param_id, param_desc, param_value, symbolic_constant from mdsystem.global_parameter, mdsystem.global_parameter_type where mdsystem.global_parameter.sys_id = mdsystem.global_parameter_type.sys_id and mdsystem.global_parameter.param_type = mdsystem.global_parameter_type.param_type and mdsystem.global_parameter.sys_id = " + sysID);
		while(rs.next())
		    {
			out.println("<tr align=left>");
			String paramID = rs.getString(1);
			String paramType = rs.getString(2);
			String paramValue = rs.getString(3);
			int isSymbolic = rs.getInt(4);
			out.println("<td>" + paramID + "</td>");
			out.println("<td>" + paramType + "</td>");
			out.println("<td>" + paramValue + "</td>");
			if (isSymbolic == 1)
			    {
				out.println("<td>yes</td>");
			    }
			else
			    {
				out.println("<td>no</td>");
			    }
		    }
		out.println("</table><br><br>");

		// UDF Invocation Table
		out.println("<table align=center border cellpadding=8>");
		out.println("<caption>UDF Invocation Table</caption>");
		out.println("<tr align=center><th>UDF</th><th>Site Tuple List ID</th><th>Site Tuple List Description</th><th>Param Tuple List ID</th><th>Param Tuple List Description</th></tr>");
		rs = stmt.executeQuery("select udf_id, mdsystem.udf_invocation_table.site_tuple_list_id, site_tuple_desc, mdsystem.udf_invocation_table.param_tuple_list_id, param_tuple_desc from mdsystem.udf_invocation_table, mdsystem.sitetuplelist, mdsystem.paramtuplelist where mdsystem.udf_invocation_table.sys_id = mdsystem.sitetuplelist.sys_id and mdsystem.udf_invocation_table.sys_id = mdsystem.paramtuplelist.sys_id and mdsystem.udf_invocation_table.site_tuple_list_id = mdsystem.sitetuplelist.site_tuple_list_id and mdsystem.udf_invocation_table.param_tuple_list_id = mdsystem.paramtuplelist.param_tuple_list_id and mdsystem.udf_invocation_table.sys_id = " + sysID);
		while(rs.next())
		    {
			out.println("<tr align=left>");
			String udfId = rs.getString(1);
			Integer siteTupleListId = new Integer(rs.getInt(2));
			String siteTupleListDesc = rs.getString(3);
			Integer paramTupleListId = new Integer(rs.getInt(4));
			String paramTupleListDesc = rs.getString(5);
			out.println("<td><a href=molsys?mode=udf_view&sys_id=" + sysID + "&udf_id=" + udfId + "&site_list_id=" + siteTupleListId.toString() + "&param_list_id=" + paramTupleListId.toString() + ">" + udfId + "</a></td>");
			out.println("<td>" + siteTupleListId.toString() + "</td>");
			out.println("<td>" + siteTupleListDesc + "</td>");
			out.println("<td>" + paramTupleListId.toString() + "</td>");
			out.println("<td>" + paramTupleListDesc + "</td>");
		    }
		out.println("</table><br><br>");
	    }
	catch (SQLException ex)
	    {
		out.println("SQL Error processing query:<br>");
		out.println(ex.toString());
		throw ex;
	    }
	catch (Exception ex)
	    {
		out.println("Error processing query:<br>");
		out.println(ex.toString());
		//		ex.printStackTrace();
		//		throw new ServletException();
		throw ex;
	    }

	out.println("<h2>Search the Site List</h2><br><br>");
	out.println("<form action=molsys method=POST>");
	out.println("<input type=hidden name=mode value=site_view>");
	out.println("<input type=hidden name=sys_id value=" + sysID + ">");
	out.println("Site ID greater than or equal to: <input type=text size=20 name=site_low><br>");
	out.println("Site ID less than or equal to: <input type=text size=20 name=site_high><br>");
	out.println("Chemical Component: <input type=text size=20 name=component_name><br>");
	out.println("Chemical Component Instance: <input type=text size=20 name=cc_instance_id><br>");
	out.println("Molecule Instance: <input type=text size=20 name=mol_instance_id><br>");
	out.println("Element: <input type=text size=20 name=element_symbol><br>");
	out.println("Atom Tag: <input type=text size=20 name=atom_tag><br>");
	out.println("<input type=submit><br><br>");
	out.println("<input type=reset><br>");
	out.println("</form>");

	//	out.println("<a href=molsys?mode=site_view&sys_id=" + sysID + "><big>Site List</big></a><br>");
	out.println("</body></html>");
	
    }
}
