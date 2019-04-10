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
   File: MolSysQuery.java
   Author: RSG
   Date: May 30, 2002
   Class: MolSysQuery
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

public class MolSysQuery
{
    public final static void execute(HttpServletRequest req, PrintWriter out, Connection conn)
    throws java.lang.Exception
    {
	StringBuffer pred = new StringBuffer(128);
	int predCount = 0;
	Enumeration e = req.getParameterNames();
	while (e.hasMoreElements())
	    {
		String name = (String)e.nextElement();
		String value = req.getParameter(name);
		if (value.length() != 0)
		    {

			if (name.equals("sys_id"))
			    {
				++predCount;
				pred.append(" and mdsystem.system.sys_id = ");
				pred.append(value);
			    }
			else if (name.equals("sys_desc"))
			    {
				++predCount;
				pred.append(" and sys_desc = \'");
				pred.append(value.toUpperCase());
				pred.append("\'");
			    }
			else if (name.equals("creator"))
			    {
				++predCount;
				pred.append(" and creator = \'");
				pred.append(value);
				pred.append("\'");
			    }
			else if (name.equals("created_after"))
			    {
				++predCount;
				pred.append(" and created > timestamp_iso(\'");
				pred.append(value);
				pred.append("\')");
			    }
			else if (name.equals("created_before"))
			    {
				++predCount;
				pred.append(" and created < timestamp_iso(\'");
				pred.append(value);
				pred.append("\')");
			    }
			else if (name.equals("sys_class"))
			    {
				++predCount;
				pred.append(" and sys_class = \'");
				pred.append(value);
				pred.append("\'");
			    }
			else if (name.equals("ff_family"))
			    {
				++predCount;
				pred.append(" and ff_family.family = \'");
				pred.append(value.toUpperCase());
				pred.append("\'");
			    }
			else if (name.equals("ff_spec"))
			    {
				++predCount;
				pred.append(" and ff_spec.spec = \'");
				pred.append(value.toUpperCase());
				pred.append("\'");
			    }

		    }
	    }

	out.println("<html>\n<head>");
	out.println("<title>Query Results</title>\n</head>\n<body><br><br>");

	String cmd = new String("with  ff_family (sys_id, family) as (select sys_id, param_value from mdsystem.global_parameter where param_id = 'ff_family'), ff_spec (sys_id, spec) as (select sys_id, param_value from mdsystem.global_parameter where param_id = 'ff_spec') select mdsystem.system.sys_id, mdsystem.system.sys_desc, mdsystem.system.creator, mdsystem.system.created, mdsystem.system.sys_class, ff_family.family, ff_spec.spec from mdsystem.system, ff_family, ff_spec where mdsystem.system.sys_id = ff_family.sys_id and mdsystem.system.sys_id = ff_spec.sys_id" + pred.toString());

	out.println("<br><br>");
	out.println("<table align=center border cellpadding=8>");
	out.println("<caption>Results from query: ");
	out.println(cmd);
	out.println("</caption>");
	out.println("<tr align=center><th>System ID</th><th>Description</th><th>Force Field Family</th><th>Force Field Version</th><th>Creator</th><th>Created</th><th>System Class</th></tr>");
	try
	    {
		Statement stmt = conn.createStatement();
		ResultSet rs = null;
		rs = stmt.executeQuery(cmd);
		while (rs.next())
		    {
			out.println("<tr align=left>");
			Integer sysID = new Integer(rs.getInt(1));
			String sysDesc = rs.getString(2);
			String creator = rs.getString(3);
			Timestamp created = rs.getTimestamp(4);
			String sysClass = rs.getString(5);
			String ffFamily = rs.getString(6);
			String ffSpec = rs.getString(7);

			out.println("<td><a href=molsys?mode=sys_view&sys_id=" + sysID.toString() + ">" + sysID.toString() + "</a></td>");
			out.println("<td>" + sysDesc + "</td>");
			out.println("<td>" + ffFamily + "</td>");
			out.println("<td>" + ffSpec + "</td>");
			out.println("<td>" + creator + "</td>");
			out.println("<td>" + created + "</td>");
			out.println("<td>" + sysClass + "</td>");
			out.println("</tr>");
		    }
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
		throw ex;
	    }
	out.println("</table></body></html>");
	
    }
}
