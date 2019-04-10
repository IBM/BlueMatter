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
   File: MolUdfView.java
   Author: RSG
   Date: June 1, 2002
   Class: MolUdfView
   Description: Displays information associated with a particular udf invocation
 */
// ****************************************************************

package com.ibm.bluematter.servlet;

import java.sql.*;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class MolUdfView
{
    public final static void execute(HttpServletRequest req, PrintWriter out, Connection conn)
    throws java.lang.Exception
    {
	String sysID = req.getParameter("sys_id");

	if ((sysID == null) || sysID.equals(""))
	    {
		out.println("ERROR: No sys_id provided in query");
		return;
	    }

	String udfID = req.getParameter("udf_id");

	if ((udfID == null) || udfID.equals(""))
	    {
		out.println("ERROR: No udf_id provided in query");
		return;
	    }

	String siteListID = req.getParameter("site_list_id");

	if ((siteListID == null) || siteListID.equals(""))
	    {
		out.println("ERROR: No site_list_id provided in query");
		return;
	    }

	String paramListID = req.getParameter("param_list_id");

	if ((paramListID == null) || paramListID.equals(""))
	    {
		out.println("ERROR: No param_list_id provided in query");
		return;
	    }



	out.println("<html>\n<head>\n");
	out.println("<title>UDF Invocation Information for UDF " + udfID + " in molecular system" + sysID +
		    "</title>\n</head>\n<body><br><br>\n");
	out.println("<h1> Molecular System  <a href=molsys?mode=sys_view&sys_id=" + sysID + ">" + sysID + "</a></h1><br><br>\n");

	String cmd = new String("select sys_desc, creator,created, sys_class from mdsystem.system where sys_id = " + sysID);
	out.println("<br><br>");
	try
	    {
		Statement stmt = conn.createStatement();
		ResultSet rs = null;
		rs = stmt.executeQuery("select min(site_ordinal), max(site_ordinal) from mdsystem.sitetupledata where sys_id = " + sysID + " and site_tuple_list_id = " + siteListID);
		rs.next();
		int siteCount = rs.getInt(2) - rs.getInt(1) + 1;

		out.println("<table border align=center cellpadding=8>");
		out.println("<caption>UDF: " + udfID);
		out.println("</caption>");
		out.println("<tr align=center>");
		for (int i = 1; i <= siteCount; ++i)
		    {
			Integer siteOrd = new Integer(i);
			out.println("<th>Site " + siteOrd.toString() + "</th>");
		    }
		// parameter tuple spec
		int paramCount = 0;
		rs = stmt.executeQuery("select param_name from mdsystem.paramtuplespec where sys_id = " + sysID + " and param_tuple_list_id = " + paramListID + " order by param_id asc");
		while(rs.next())
		    {
			String paramName = rs.getString(1);
			out.println("<th>" + paramName + "</th>");
			++paramCount;
		    }
		out.println("</tr>");

		Statement stmt2 = conn.createStatement();
		rs = stmt.executeQuery("select site_id, site_tuple_id from mdsystem.sitetupledata where sys_id = " + sysID + " and site_tuple_list_id = " + siteListID + " order by site_tuple_id, site_ordinal");
		ResultSet rs2 = null;
		rs2 = stmt2.executeQuery("select param_value, param_tuple_id from mdsystem.paramtupledata where sys_id = " + sysID + " and param_tuple_list_id = " + paramListID + " order by param_tuple_id, param_id");

		String[] site = new String[siteCount];
		String[] param = new String[paramCount];
		int tupleCount = 0;
		while (rs.next())
		    {
			++tupleCount;
			out.println("<tr align=left>");
			// sites
			StringBuffer siteList = new StringBuffer(128);
			for (int i = 0; i < siteCount; ++i)
			    {
				if (i > 0)
				    {
					if (!rs.next())
					    {
						break;
					    }
				    }
				Integer siteID = new Integer(rs.getInt(1));
				site[i] = siteID.toString();
				int siteTupleID = rs.getInt(2);
				siteList.append("&site_id=");
				siteList.append(siteID.toString());
			    }
			for (int i = 0; i < siteCount; ++i)
			    {
				out.println("<td><a href=molsys?mode=site_view&sys_id=" + sysID + siteList.toString() + "#site_" + site[i] + ">" + site[i] + "</a></td>");
			    }

			for (int i = 0; i < paramCount; ++i)
			    {
				if (!rs2.next())
				    {
					break;
				    }
				String paramValue = rs2.getString(1);
				int paramTupleID = rs2.getInt(2);
				out.println("<td>" + paramValue + "</td>");
			    }
			out.println("</tr>");
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
	out.println("</body></html>");
	
    }
}
